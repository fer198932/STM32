/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          原能智能制造机器人
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : Main.C
* By  : Hiker Lee
* 2018-04-26

*
*仅用于CNC调试，修改了协议，增加脉冲数和频率为3个字节；
*0530准备调试换刀；
*0608增加电机加减速功能；
*0614增加18B20；1、PD11主轴温度；2、PD12保护罩温度；
*0625调整了加减速部分；
*0710添加启动检测急停开关是否按下；修改串口数据处理的第二帧缓存；
*0720添加RZ7899直流电机驱动；PE0；PE1
*0724通信改到Uart3;
********************************************************************************************************/


#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "key.h"
#include "lwip_comm.h"
#include "LAN8720.h"
#include "usmart.h"
#include "timer.h"
#include "lcd.h"
#include "sram.h"
#include "malloc.h"
#include "lwip_comm.h"
#include "includes.h"
#include "lwipopts.h"
#include "udp_demo.h"
#include "ChangeTools.h"

#include "SysInit.h"
#include "ComDataProc.h"
#include "ds18b20.h"

#define FILTER_N 5
#define DATA_GROUP 10	// 一次处理10组数据
#define ARRAY_SIZE 500 	// 1024的大小存在数组过大越界的问题，暂时选500，后续完善需要用到动态数组

unsigned char uartDataRecv[1024];
unsigned char uartDataGroupBuf[DATA_GROUP][ARRAY_SIZE];	// 用来缓冲uartDataRecvProc里面的数据
unsigned char uartDataRecvProc[1024];
unsigned char recvFrameok=0;
unsigned int commandKey;		// 判断是单组还是多组
unsigned int iGroup;		// 用来标记多组递减的
unsigned int sizeUartData;	// 复制的长度
unsigned char respFlag = 0;		// 是否回复的标记， 0：回复  1：不回复

short temperature1,temperature2;	
short temperature1Pro[FILTER_N+1],temperature2Pro[FILTER_N+1];	

u8 temCollectionFlag=0;
u8 temDataFilterCount=0;

extern unsigned int recvFrameProcLength;

extern u32 xAxisPlusNum,yAxisPlusNum,zAxisPlusNum,aAxisPlusNum,bAxisPlusNum;
extern u16 xAxisClk,yAxisClk,zAxisClk,aAxisClk,bAxisClk;
extern unsigned char backResString[20];

extern u32 pwmNumTim12;
extern u32 pwmNumTim9;
extern u32 pwmNumTim3;
extern u32 pwmNumTim4;
extern u32 pwmNumTim2;

extern u8 xAxisStepFlag; 
extern u8 yAxisStepFlag; 
extern u8 zAxisStepFlag; 
extern u8 aAxisStepFlag; 
extern u8 bAxisStepFlag; 

extern u8 urgentStopFlag; 
extern u8 bAxis2ZeroFlag; 

extern u8 dataPreProcFlag; 	//0:已完成预处理，待第二部处理；1:等待下一帧数据的预处理；

extern u8 addtime;

extern nAxisSetData allAxisSetData[3];			// X、Y、Z轴设定的运动参数
extern SetDataPerAxis setDataPerAxis;	// 完整记录某一轴的运动参数，暂时为X轴

extern u8 testSetDataFlag;			// 标记当前的运行状态

//KEY任务
#define KEY_TASK_PRIO 		8
//任务堆栈大小
#define KEY_STK_SIZE		128	
//任务堆栈
OS_STK KEY_TASK_STK[KEY_STK_SIZE];
//任务函数
void key_task(void *pdata);   

//LED任务
//任务优先级
#define LED_TASK_PRIO		9
//任务堆栈大小
#define LED_STK_SIZE		64
//任务堆栈
OS_STK	LED_TASK_STK[LED_STK_SIZE];
//任务函数
void led_task(void *pdata);  

//rs232DataProc任务
//任务优先级
#define DATAPROC_TASK_PRIO		12
//任务堆栈大小
#define DATAPRO_STK_SIZE		256
//任务堆栈
OS_STK	DATAPRO_TASK_STK[DATAPRO_STK_SIZE];
//任务函数
void rs232DataProc_task(void *pdata);  


//在LCD上显示地址信息任务
//任务优先级
#define DISPLAY_TASK_PRIO	10
//任务堆栈大小
#define DISPLAY_STK_SIZE	128
//任务堆栈
OS_STK	DISPLAY_TASK_STK[DISPLAY_STK_SIZE];
//任务函数
void display_task(void *pdata);

//START任务
//任务优先级
#define START_TASK_PRIO		11
//任务堆栈大小
#define START_STK_SIZE		128
//任务堆栈
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata); 

//在LCD上显示地址信息
//mode:1 显示DHCP获取到的地址
//	  其他 显示静态地址
void show_address(u8 mode)
{
	u8 buf[30];
	if(mode==2)
	{
		sprintf((char*)buf,"DHCP IP :%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//打印动态IP地址
		LCD_ShowString(30,170,210,16,16,buf); 
		sprintf((char*)buf,"DHCP GW :%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//打印网关地址
		LCD_ShowString(30,190,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//打印子网掩码地址
		LCD_ShowString(30,210,210,16,16,buf); 
		LCD_ShowString(30,230,210,16,16,"Port:8089!"); 
	}
	else 
	{
		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//打印动态IP地址
		LCD_ShowString(30,170,210,16,16,buf); 
		sprintf((char*)buf,"Static GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//打印网关地址
		LCD_ShowString(30,190,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//打印子网掩码地址
		LCD_ShowString(30,210,210,16,16,buf); 
		LCD_ShowString(30,230,210,16,16,"Port:8089!"); 
	}	
}

// 组数据分组解析函数
void uartDataGroupProc(void)
{
	u8 i;
	for(i=0; i<DATA_GROUP; i++)		// 循环10次将uartDataRecvProc里面的数据放入uartDataGroupBuf
	{
		uartDataGroupBuf[i][0] = 0x0d;		// 第一个数据为帧标识
		uartDataGroupBuf[i][1] = 0x23;		// 命令标识，注意：后续改为0x0023
		uartDataGroupBuf[i][2] = 0x00;
		
		// 数据部分
		memcpy(uartDataGroupBuf[i]+3, uartDataRecvProc+(3+i*30), 30);	// 复制30个数据过来
		
		uartDataGroupBuf[i][33] = 0xe2;		// CRC校验码
		uartDataGroupBuf[i][34] = 0xe1;
		uartDataGroupBuf[i][35] = 0x6a;
		uartDataGroupBuf[i][36] = 0xf;
	}
}
// 打印分组数据的函数
void showDataGroup(void)
{
	u8 i, j;
	printf("打印组发送接收到的数据：\r\n");
	for(i=0; i<DATA_GROUP; i++)
	{
		printf("第%d组:\r\n", i);
		for(j=0; j<50; j++)		// 固定打印50个，够了
			{
				printf("%x ", uartDataGroupBuf[i][j]);
			}
		printf("\r\n\r\n");
	}
}

// 打印接收到的数据
void showUartDataRecvProc(void)
{
	u32 i;
	printf("接收到的数据：\r\n");
	for(i=0; i<320; i++)
	{
		printf("%x ", uartDataRecvProc[i]);
	}
	printf("\r\n\r\n");
}

int main(void)
{
	u8 i;
	u32 temp;
	
	delay_init(168);       	//延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//中断分组配置
//	uart_init(115200);   	//串口波特率设置 hiker
//	usmart_dev.init(84);	//初始化USMART hiker
//	LED_Init(); 			//LED初始化  hiker
//	KEY_Init();  			//按键初始化  hiker
//	LCD_Init();  			//LCD初始化  hiker

	uart2_init(115200);			//hiker
	uart3_init(115200);			//hiker	用于通信
//	uart_init(115200);			// 探索者板子专用的串口 forTest
	
	testIO_Init();					//PC3
 	motorDirIO_Init();			//PF0,1,2,3,4

 	TIM6_Base_Init(5000-1,8400-1);	//定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数5000次为500ms     //用于串口数据异常处理	
//	TIM_Cmd(TIM6,ENABLE);			//用于串口数据异常处理							  //使能定时器3

	addtime=40;									// 增加延时，编码器用
	TIM7_Int_Init(addtime*10-1,8400-1);		//定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数400次为40ms    20ms 	10ms	//用于电机加减速处理
	TIM_Cmd(TIM7,DISABLE);			//test

// 作为脉冲计数使用 byYJY
	TIM14_Int_Init(37-1,280-1);			//84M/2800=30khz的计数频率,重装载值1000，所以PWM频率为 30k/1000=30hz. 
//	TIM14_Int_Init(5000-1,8400-1);		//定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数5000次为500ms				//用于温度采集
//	TIM_Cmd(TIM14,ENABLE);			//test


 	EXTIX_Init();			//180428	中断管脚:PE2,3,4;PB0,1;PG5,6;
 	KEY_Init();				//限位初始化；PF5,6,7,8,10,13,14,15;PG0,1,2,3,4;PA11,12
 	mainToolFlagIOInit();	//PG7
	motorIO_Init();			//PG8,9

	DCMotor_Init();			//PE0,PE1
  	
	// 刀具位置的初始化，换刀程序用
	savedStepsInit();
	
	// 初始化用来记录设定的运动参数的结构体数组
	memset(allAxisSetData, 0, sizeof(nAxisSetData)*3);		
	memset(&setDataPerAxis, 0, sizeof(setDataPerAxis));
	
	xAxisStepFlag = 2;
	yAxisStepFlag = 2;
	zAxisStepFlag = 2;
	aAxisStepFlag = 2;
	bAxisStepFlag = 2;
	
	 NVIC_Config();

	 DS18B20_Init1();
	 DS18B20_Init2();

	 bAxis2ZeroFlag = 1;	//只发信号，不做停止动作；
	 
	 while(0)	// 测试刀具的电磁铁锁定功能
	 {
		 GPIO_SetBits(GPIOB,GPIO_Pin_12);			// 松开刀具
		 delay_s(2);
		 GPIO_ResetBits(GPIOB,GPIO_Pin_12);		// 夹紧刀具
		 delay_s(2);
	 }
	 
	 while(0) 		// 探索者板子上的测试程序
	 {
		 printf("main函数！！\r\n");
		 
		 // 调用数据分组解析函数
		 uartDataGroupProc();
		
		 // 显示
		 showDataGroup();
		 showUartDataRecvProc();
		 printf("//////////////////////////////////////////////////////////\r\n");
		
		// 组数据解析	
		
		 
		 delay_ms(100);
	 }
	 
	while(0)
		{
		USART_SendData(USART3, 'a');		 //向串口2发送数据
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
		
		delay_ms(10);	
		USART_SendData(USART3, 'y');		 //向串口2发送数据
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
		}
	while(0)
		{
			bAxisDIR_ON;
			delay_ms(10);	
			bAxisDIR_OFF;
			delay_ms(10);	
		}
	while(1)		// 关闭原来的测试程序
		{
			key_Proc5axisComm((void*)0);

			if(recvFrameok == 1)	//接收到完整信息；
			{
				// 利用commandKey来判断是单组还是多组
				commandKey = uartDataRecvProc[1]+(uartDataRecvProc[2]<<8);
				
				if(0x23 == commandKey)		// 处理多组数据
				{					
					// 调用数据分组解析函数
					uartDataGroupProc();
					
					sizeUartData = sizeof(uartDataRecvProc);					
					for(iGroup=0; iGroup<DATA_GROUP-1; iGroup++)		// 只循环9次
					{
						memcpy(uartDataRecvProc, uartDataGroupBuf[iGroup], sizeUartData);
						
						// 置位5轴的步进标记
						respFlag = 1;	// 标记为不可回复状态
						// 像处理单组数据一样来执行后续步骤
						if(uartDataRecvProc[0]==DATAINFO)
						{
							while(1)
							{
								if((xAxisStepFlag==2)&&(yAxisStepFlag==2)&&(zAxisStepFlag==2)&&(aAxisStepFlag==2)&&(bAxisStepFlag==2))
								{
									TIM_Cmd(TIM7,DISABLE);			//			用于电机加减速处理					
									rs232DataProcStep2();			
									recvFrameok = 0;
									
									// 回复设定参数的数据
									//coderResBack();
									
									break;
								}
							}
							
						}
						else
						{
							rs232DataProcStep1();			
							recvFrameok = 0;
						}
					}
					
					// 置位5轴的步进标记,最后一次使用
					respFlag = 0;	// 标记为可回复状态
					// 最后一次再回复上位机
					// 像处理单组数据一样来执行后续步骤
					memcpy(uartDataRecvProc, uartDataGroupBuf[DATA_GROUP-1], sizeUartData);
					if(uartDataRecvProc[0]==DATAINFO)
					{
						while(1)
						{
							if((xAxisStepFlag==2)&&(yAxisStepFlag==2)&&(zAxisStepFlag==2)&&(aAxisStepFlag==2)&&(bAxisStepFlag==2))
							{
								TIM_Cmd(TIM7,DISABLE);			//			用于电机加减速处理					
								rs232DataProcStep2();			
								recvFrameok = 0;
								break;
							}
						}
					}
					else
					{
						rs232DataProcStep1();			
						recvFrameok = 0;
					}					
				} else {					// 处理单组数据（沿用之前的方法不变）
					
					respFlag = 0;	// 标记为可回复状态
					if(uartDataRecvProc[0]==DATAINFO)
					{
						if((xAxisStepFlag==2)&&(yAxisStepFlag==2)&&(zAxisStepFlag==2)&&(aAxisStepFlag==2)&&(bAxisStepFlag==2))
						{
							TIM_Cmd(TIM7,DISABLE);			//			用于电机加减速处理					
							rs232DataProcStep2();
							recvFrameok = 0;
							
							// 回复设定参数的数据
						}
					}
					else
					{
						rs232DataProcStep1();
						recvFrameok = 0;
					}
				}
			}

			if(urgentStopFlag ==1)			//急停
			{
			/*	
				if((xAxisStepFlag==2)&&(yAxisStepFlag==2)&&(zAxisStepFlag==2)&&(aAxisStepFlag==2)&&(bAxisStepFlag==2))
				{
					
					backResString[0] = FrameB1;
					backResString[1] = FrameB2;
					backResString[2] = STATUSINFO;
					
					backResString[3] = 9;
					backResString[4] = 0x00;
					
					backResString[5] = 0x3b;
					backResString[6] = 0x00;
					
					backResString[7] = 0x01;
					backResString[8] = FrameEnd;
					for(i=0;i<9;i++)
						{
							USART_SendData(USART3,backResString[i]); 
							while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
						}
				}
				else
				{//*/
					
					backResString[0] = FrameB1;
					backResString[1] = FrameB2;
					backResString[2] = STATUSINFO;
					
					backResString[3] = 23;
					backResString[4] = 0x00;
					
					backResString[5] = 0x3b;		
					backResString[6] = 0x00;
					
					backResString[7] = pwmNumTim3 & 0xff;					//x
					backResString[8] = (pwmNumTim3 >>8) & 0xff;
					backResString[9] = (pwmNumTim3 >>16) & 0xff;
					
					backResString[10] = pwmNumTim2 & 0xff;						//y
					backResString[11] = (pwmNumTim2 >>8) & 0xff;
					backResString[12] = (pwmNumTim2 >>16) & 0xff;
					
					backResString[13] = pwmNumTim4 & 0xff;					//z
					backResString[14] = (pwmNumTim4 >>8) & 0xff;
					backResString[15] = (pwmNumTim4 >>16) & 0xff;
					
					backResString[16] = pwmNumTim9 & 0xff;					//a
					backResString[17] = (pwmNumTim9 >>8) & 0xff;
					backResString[18] = (pwmNumTim9 >>16) & 0xff;
					
					backResString[19] = pwmNumTim12 & 0xff; 				//b
					backResString[20] = (pwmNumTim12 >>8) & 0xff;
					backResString[21] = (pwmNumTim12 >>16) & 0xff;
					
					backResString[22] = FrameEnd;
					
					for(i=0;i<23;i++)
						{
							USART_SendData(USART3,backResString[i]); 
							while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
						}
			//	}
				urgentStopFlag = 3;			//按键按下后处理完毕

			}
			else if(urgentStopFlag ==0)		//急停取消
			{
				
				backResString[0] = FrameB1;
				backResString[1] = FrameB2;
				backResString[2] = STATUSINFO;
				
				backResString[3] = 9;
				backResString[4] = 0x00;
				
				backResString[5] = 0x41;		
				backResString[6] = 0x00;
				
				backResString[7] = 0x01;					
				backResString[8] = FrameEnd;
				
				for(i=0;i<9;i++)
					{
						USART_SendData(USART3,backResString[i]); 
						while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
					}
				urgentStopFlag = 2;

				
				xAxisStepFlag = 2;			//急停恢复后，可以继续下一步动作；所以，置标志位；
				yAxisStepFlag = 2;
				zAxisStepFlag = 2;
				aAxisStepFlag = 2;
				bAxisStepFlag = 2;

				recvFrameok = 0;			//急停恢复后，可以继续下一步动作；但是已收到的数据作废；
			}

			if(temCollectionFlag == 1)
			{
				temCollectionFlag = 0;
				
				temperature1Pro[FILTER_N] = DS18B20_Get_Temp1();
				temperature2Pro[FILTER_N] = DS18B20_Get_Temp2();

				for(i = 0; i < FILTER_N; i++) 
					{
		    			temperature1Pro[i] = temperature1Pro[i + 1]; // 所有数据左移，低位仍掉
		    			temperature1 += temperature1Pro[i];

						
		    			temperature2Pro[i] = temperature2Pro[i + 1]; // 所有数据左移，低位仍掉
		    			temperature2 += temperature2Pro[i];
  					}

				if(temDataFilterCount<4)
					temDataFilterCount++;
				else
				{
					temDataFilterCount = 0;
				}

				if(temDataFilterCount == 4)
				{
					temperature1 = temperature1/FILTER_N;			//0x003C	主轴温度传感	具体数据
					temperature2 = temperature2/FILTER_N;			//0x003E	保护罩温度传感	具体数据

					
					backResString[0] = FrameB1;
					backResString[1] = FrameB2;
					backResString[2] = STATUSINFO;
					
					backResString[3] = 9;
					backResString[4] = 0x00;
					
					backResString[5] = 0x3C;		
					backResString[6] = 0x00;
					
					backResString[7] = temperature1 && 0xFF;
					backResString[8] = (temperature1>>8) && 0xFF;
					
					backResString[9] = temperature2 && 0xFF;
					backResString[10] = (temperature2>>8) && 0xFF;
					
					backResString[11] = FrameEnd;
					
					for(i=0;i<12;i++)
						{
					//		USART_SendData(USART3,backResString[i]); 
					//		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//等待发送结束
						}
				}
				else if(temDataFilterCount == 0)
				{
					temperature1 = 0;
					temperature2 = 0;
				}
			}
			
		}

}

//start任务
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr;
	pdata = pdata ;
	
	OSStatInit();  			//初始化统计任务
	OS_ENTER_CRITICAL();  	//关中断
#if	LWIP_DHCP
	lwip_comm_dhcp_creat();	//创建DHCP任务
#endif
	OSTaskCreate(led_task,(void*)0,(OS_STK*)&LED_TASK_STK[LED_STK_SIZE-1],LED_TASK_PRIO); 						//创建LED任务
	OSTaskCreate(key_task,(void*)0,(OS_STK*)&KEY_TASK_STK[KEY_STK_SIZE-1],KEY_TASK_PRIO); 						//创建KEY任务
	OSTaskCreate(display_task,(void*)0,(OS_STK*)&DISPLAY_TASK_STK[DISPLAY_STK_SIZE-1],DISPLAY_TASK_PRIO); 		//显示任务
	OSTaskCreate(rs232DataProc_task,(void*)0,(OS_STK*)&DATAPRO_TASK_STK[DATAPRO_STK_SIZE-1],DATAPROC_TASK_PRIO); //显示任务
	OSTaskSuspend(OS_PRIO_SELF); 	//挂起start_task任务
	OS_EXIT_CRITICAL();  			//开中断
}


//显示地址等信息的任务函数
void display_task(void *pdata)
{
	while(1)
	{ 
#if LWIP_DHCP									//当开启DHCP的时候
		if(lwipdev.dhcpstatus != 0) 			//开启DHCP
		{
			show_address(lwipdev.dhcpstatus );	//显示地址信息
			OSTaskSuspend(OS_PRIO_SELF); 		//显示完地址信息后挂起自身任务
		}
#else
		show_address(0); 						//显示静态地址
		OSTaskSuspend(OS_PRIO_SELF); 			//显示完地址信息后挂起自身任务
#endif //LWIP_DHCP
		OSTimeDlyHMSM(0,0,0,500);
	}
}

//key任务
void key_task(void *pdata)
{
	u8 key; 
	while(1)
	{
		key = key_Proc5axisComm(0);
		if(key==KEYXa_PRES) 				//发送数据
		{
			udp_flag |= LWIP_SEND_DATA; //标记LWIP有数据要发送
		}
		OSTimeDlyHMSM(0,0,0,10);  		//延时10ms
	}
}

//led任务
void led_task(void *pdata)
{
	while(1)
	{
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,0,500); 		//延时500ms
 	}
}



//rs232DataProc任务
void rs232DataProc_task(void *pdata)
{
	while(1)
	{
		if(recvFrameok == 1)
		{
	//		rs232DataProc();
			recvFrameok = 0;
		}
		
		OSTimeDlyHMSM(0,0,0,500); 		//延时500ms
 	}
}







