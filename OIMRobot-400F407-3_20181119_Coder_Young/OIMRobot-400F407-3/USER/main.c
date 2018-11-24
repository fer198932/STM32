/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          ԭ���������������
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
*������CNC���ԣ��޸���Э�飬������������Ƶ��Ϊ3���ֽڣ�
*0530׼�����Ի�����
*0608���ӵ���Ӽ��ٹ��ܣ�
*0614����18B20��1��PD11�����¶ȣ�2��PD12�������¶ȣ�
*0625�����˼Ӽ��ٲ��֣�
*0710���������⼱ͣ�����Ƿ��£��޸Ĵ������ݴ���ĵڶ�֡���棻
*0720���RZ7899ֱ�����������PE0��PE1
*0724ͨ�Ÿĵ�Uart3;
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
#define DATA_GROUP 10	// һ�δ���10������
#define ARRAY_SIZE 500 	// 1024�Ĵ�С�����������Խ������⣬��ʱѡ500������������Ҫ�õ���̬����

unsigned char uartDataRecv[1024];
unsigned char uartDataGroupBuf[DATA_GROUP][ARRAY_SIZE];	// ��������uartDataRecvProc���������
unsigned char uartDataRecvProc[1024];
unsigned char recvFrameok=0;
unsigned int commandKey;		// �ж��ǵ��黹�Ƕ���
unsigned int iGroup;		// ������Ƕ���ݼ���
unsigned int sizeUartData;	// ���Ƶĳ���
unsigned char respFlag = 0;		// �Ƿ�ظ��ı�ǣ� 0���ظ�  1�����ظ�

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

extern u8 dataPreProcFlag; 	//0:�����Ԥ�������ڶ�������1:�ȴ���һ֡���ݵ�Ԥ����

extern u8 addtime;

extern nAxisSetData allAxisSetData[3];			// X��Y��Z���趨���˶�����
extern u8 testSetDataFlag;			// ��ǵ�ǰ������״̬

//KEY����
#define KEY_TASK_PRIO 		8
//�����ջ��С
#define KEY_STK_SIZE		128	
//�����ջ
OS_STK KEY_TASK_STK[KEY_STK_SIZE];
//������
void key_task(void *pdata);   

//LED����
//�������ȼ�
#define LED_TASK_PRIO		9
//�����ջ��С
#define LED_STK_SIZE		64
//�����ջ
OS_STK	LED_TASK_STK[LED_STK_SIZE];
//������
void led_task(void *pdata);  

//rs232DataProc����
//�������ȼ�
#define DATAPROC_TASK_PRIO		12
//�����ջ��С
#define DATAPRO_STK_SIZE		256
//�����ջ
OS_STK	DATAPRO_TASK_STK[DATAPRO_STK_SIZE];
//������
void rs232DataProc_task(void *pdata);  


//��LCD����ʾ��ַ��Ϣ����
//�������ȼ�
#define DISPLAY_TASK_PRIO	10
//�����ջ��С
#define DISPLAY_STK_SIZE	128
//�����ջ
OS_STK	DISPLAY_TASK_STK[DISPLAY_STK_SIZE];
//������
void display_task(void *pdata);

//START����
//�������ȼ�
#define START_TASK_PRIO		11
//�����ջ��С
#define START_STK_SIZE		128
//�����ջ
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata); 

//��LCD����ʾ��ַ��Ϣ
//mode:1 ��ʾDHCP��ȡ���ĵ�ַ
//	  ���� ��ʾ��̬��ַ
void show_address(u8 mode)
{
	u8 buf[30];
	if(mode==2)
	{
		sprintf((char*)buf,"DHCP IP :%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//��ӡ��̬IP��ַ
		LCD_ShowString(30,170,210,16,16,buf); 
		sprintf((char*)buf,"DHCP GW :%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//��ӡ���ص�ַ
		LCD_ShowString(30,190,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//��ӡ���������ַ
		LCD_ShowString(30,210,210,16,16,buf); 
		LCD_ShowString(30,230,210,16,16,"Port:8089!"); 
	}
	else 
	{
		sprintf((char*)buf,"Static IP:%d.%d.%d.%d",lwipdev.ip[0],lwipdev.ip[1],lwipdev.ip[2],lwipdev.ip[3]);						//��ӡ��̬IP��ַ
		LCD_ShowString(30,170,210,16,16,buf); 
		sprintf((char*)buf,"Static GW:%d.%d.%d.%d",lwipdev.gateway[0],lwipdev.gateway[1],lwipdev.gateway[2],lwipdev.gateway[3]);	//��ӡ���ص�ַ
		LCD_ShowString(30,190,210,16,16,buf); 
		sprintf((char*)buf,"NET MASK:%d.%d.%d.%d",lwipdev.netmask[0],lwipdev.netmask[1],lwipdev.netmask[2],lwipdev.netmask[3]);	//��ӡ���������ַ
		LCD_ShowString(30,210,210,16,16,buf); 
		LCD_ShowString(30,230,210,16,16,"Port:8089!"); 
	}	
}

// �����ݷ����������
void uartDataGroupProc(void)
{
	u8 i;
	for(i=0; i<DATA_GROUP; i++)		// ѭ��10�ν�uartDataRecvProc��������ݷ���uartDataGroupBuf
	{
		uartDataGroupBuf[i][0] = 0x0d;		// ��һ������Ϊ֡��ʶ
		uartDataGroupBuf[i][1] = 0x23;		// �����ʶ��ע�⣺������Ϊ0x0023
		uartDataGroupBuf[i][2] = 0x00;
		
		// ���ݲ���
		memcpy(uartDataGroupBuf[i]+3, uartDataRecvProc+(3+i*30), 30);	// ����30�����ݹ���
		
		uartDataGroupBuf[i][33] = 0xe2;		// CRCУ����
		uartDataGroupBuf[i][34] = 0xe1;
		uartDataGroupBuf[i][35] = 0x6a;
		uartDataGroupBuf[i][36] = 0xf;
	}
}
// ��ӡ�������ݵĺ���
void showDataGroup(void)
{
	u8 i, j;
	printf("��ӡ�鷢�ͽ��յ������ݣ�\r\n");
	for(i=0; i<DATA_GROUP; i++)
	{
		printf("��%d��:\r\n", i);
		for(j=0; j<50; j++)		// �̶���ӡ50��������
			{
				printf("%x ", uartDataGroupBuf[i][j]);
			}
		printf("\r\n\r\n");
	}
}

// ��ӡ���յ�������
void showUartDataRecvProc(void)
{
	u32 i;
	printf("���յ������ݣ�\r\n");
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
	
	delay_init(168);       	//��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);	//�жϷ�������
//	uart_init(115200);   	//���ڲ��������� hiker
//	usmart_dev.init(84);	//��ʼ��USMART hiker
//	LED_Init(); 			//LED��ʼ��  hiker
//	KEY_Init();  			//������ʼ��  hiker
//	LCD_Init();  			//LCD��ʼ��  hiker

	uart2_init(115200);			//hiker
	uart3_init(115200);			//hiker	����ͨ��
//	uart_init(115200);			// ̽���߰���ר�õĴ��� forTest
	
	testIO_Init();					//PC3
 	motorDirIO_Init();			//PF0,1,2,3,4

 	TIM6_Base_Init(5000-1,8400-1);	//��ʱ��ʱ��84M����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�����5000��Ϊ500ms     //���ڴ��������쳣����	
//	TIM_Cmd(TIM6,ENABLE);			//���ڴ��������쳣����							  //ʹ�ܶ�ʱ��3

	addtime=40;
	TIM7_Int_Init(addtime*10-1,8400-1);		//��ʱ��ʱ��84M����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�����400��Ϊ40ms    20ms 	10ms	//���ڵ���Ӽ��ٴ���
	TIM_Cmd(TIM7,DISABLE);			//test

// ��Ϊ�������ʹ�� byYJY
	TIM14_Int_Init(37-1,280-1);			//84M/2800=30khz�ļ���Ƶ��,��װ��ֵ1000������PWMƵ��Ϊ 30k/1000=30hz. 
//	TIM14_Int_Init(5000-1,8400-1);		//��ʱ��ʱ��84M����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�����5000��Ϊ500ms				//�����¶Ȳɼ�
//	TIM_Cmd(TIM14,ENABLE);			//test


 	EXTIX_Init();			//180428	�жϹܽ�:PE2,3,4;PB0,1;PG5,6;
 	KEY_Init();				//��λ��ʼ����PF5,6,7,8,10,13,14,15;PG0,1,2,3,4;PA11,12
 	mainToolFlagIOInit();	//PG7
	motorIO_Init();			//PG8,9

	DCMotor_Init();			//PE0,PE1
  	
	// ����λ�õĳ�ʼ��������������
	savedStepsInit();
	
	// ��ʼ��������¼�趨���˶������Ľṹ������
	memset(allAxisSetData, 0, sizeof(nAxisSetData)*3);		
	
	xAxisStepFlag = 2;
	yAxisStepFlag = 2;
	zAxisStepFlag = 2;
	aAxisStepFlag = 2;
	bAxisStepFlag = 2;
	
	 NVIC_Config();

	 DS18B20_Init1();
	 DS18B20_Init2();

	 bAxis2ZeroFlag = 1;	//ֻ���źţ�����ֹͣ������
	 
	 while(0)	// ���Ե��ߵĵ������������
	 {
		 GPIO_SetBits(GPIOB,GPIO_Pin_12);			// �ɿ�����
		 delay_s(2);
		 GPIO_ResetBits(GPIOB,GPIO_Pin_12);		// �н�����
		 delay_s(2);
	 }
	 
	 while(0) 		// ̽���߰����ϵĲ��Գ���
	 {
		 printf("main��������\r\n");
		 
		 // �������ݷ����������
		 uartDataGroupProc();
		
		 // ��ʾ
		 showDataGroup();
		 showUartDataRecvProc();
		 printf("//////////////////////////////////////////////////////////\r\n");
		
		// �����ݽ���	
		
		 
		 delay_ms(100);
	 }
	 
	while(0)
		{
		USART_SendData(USART3, 'a');		 //�򴮿�2��������
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		
		delay_ms(10);	
		USART_SendData(USART3, 'y');		 //�򴮿�2��������
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
		}
	while(0)
		{
			bAxisDIR_ON;
			delay_ms(10);	
			bAxisDIR_OFF;
			delay_ms(10);	
		}
	while(1)		// �ر�ԭ���Ĳ��Գ���
		{
			key_Proc5axisComm((void*)0);

			if(recvFrameok == 1)	//���յ�������Ϣ��
			{
				// ����commandKey���ж��ǵ��黹�Ƕ���
				commandKey = uartDataRecvProc[1]+(uartDataRecvProc[2]<<8);
				
				if(0x23 == commandKey)		// �����������
				{					
					// �������ݷ����������
					uartDataGroupProc();
					
					sizeUartData = sizeof(uartDataRecvProc);					
					for(iGroup=0; iGroup<DATA_GROUP-1; iGroup++)		// ֻѭ��9��
					{
						memcpy(uartDataRecvProc, uartDataGroupBuf[iGroup], sizeUartData);
						
						// ��λ5��Ĳ������
						respFlag = 1;	// ���Ϊ���ɻظ�״̬
						// ����������һ����ִ�к�������
						if(uartDataRecvProc[0]==DATAINFO)
						{
							while(1)
							{
								if((xAxisStepFlag==2)&&(yAxisStepFlag==2)&&(zAxisStepFlag==2)&&(aAxisStepFlag==2)&&(bAxisStepFlag==2))
								{
									TIM_Cmd(TIM7,DISABLE);			//			���ڵ���Ӽ��ٴ���					
									rs232DataProcStep2();			
									recvFrameok = 0;
									
									// �ظ��趨����������
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
					
					// ��λ5��Ĳ������,���һ��ʹ��
					respFlag = 0;	// ���Ϊ�ɻظ�״̬
					// ���һ���ٻظ���λ��
					// ����������һ����ִ�к�������
					memcpy(uartDataRecvProc, uartDataGroupBuf[DATA_GROUP-1], sizeUartData);
					if(uartDataRecvProc[0]==DATAINFO)
					{
						while(1)
						{
							if((xAxisStepFlag==2)&&(yAxisStepFlag==2)&&(zAxisStepFlag==2)&&(aAxisStepFlag==2)&&(bAxisStepFlag==2))
							{
								TIM_Cmd(TIM7,DISABLE);			//			���ڵ���Ӽ��ٴ���					
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
				} else {					// ���������ݣ�����֮ǰ�ķ������䣩
					
					respFlag = 0;	// ���Ϊ�ɻظ�״̬
					if(uartDataRecvProc[0]==DATAINFO)
					{
						if((xAxisStepFlag==2)&&(yAxisStepFlag==2)&&(zAxisStepFlag==2)&&(aAxisStepFlag==2)&&(bAxisStepFlag==2))
						{
							TIM_Cmd(TIM7,DISABLE);			//			���ڵ���Ӽ��ٴ���					
							rs232DataProcStep2();
							recvFrameok = 0;
							
							// �ظ��趨����������
						}
					}
					else
					{
						rs232DataProcStep1();
						recvFrameok = 0;
					}
				}
			}

			if(urgentStopFlag ==1)			//��ͣ
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
							while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
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
							while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
						}
			//	}
				urgentStopFlag = 3;			//�������º������

			}
			else if(urgentStopFlag ==0)		//��ͣȡ��
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
						while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
					}
				urgentStopFlag = 2;

				
				xAxisStepFlag = 2;			//��ͣ�ָ��󣬿��Լ�����һ�����������ԣ��ñ�־λ��
				yAxisStepFlag = 2;
				zAxisStepFlag = 2;
				aAxisStepFlag = 2;
				bAxisStepFlag = 2;

				recvFrameok = 0;			//��ͣ�ָ��󣬿��Լ�����һ���������������յ����������ϣ�
			}

			if(temCollectionFlag == 1)
			{
				temCollectionFlag = 0;
				
				temperature1Pro[FILTER_N] = DS18B20_Get_Temp1();
				temperature2Pro[FILTER_N] = DS18B20_Get_Temp2();

				for(i = 0; i < FILTER_N; i++) 
					{
		    			temperature1Pro[i] = temperature1Pro[i + 1]; // �����������ƣ���λ�Ե�
		    			temperature1 += temperature1Pro[i];

						
		    			temperature2Pro[i] = temperature2Pro[i + 1]; // �����������ƣ���λ�Ե�
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
					temperature1 = temperature1/FILTER_N;			//0x003C	�����¶ȴ���	��������
					temperature2 = temperature2/FILTER_N;			//0x003E	�������¶ȴ���	��������

					
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
					//		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//�ȴ����ͽ���
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

//start����
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr;
	pdata = pdata ;
	
	OSStatInit();  			//��ʼ��ͳ������
	OS_ENTER_CRITICAL();  	//���ж�
#if	LWIP_DHCP
	lwip_comm_dhcp_creat();	//����DHCP����
#endif
	OSTaskCreate(led_task,(void*)0,(OS_STK*)&LED_TASK_STK[LED_STK_SIZE-1],LED_TASK_PRIO); 						//����LED����
	OSTaskCreate(key_task,(void*)0,(OS_STK*)&KEY_TASK_STK[KEY_STK_SIZE-1],KEY_TASK_PRIO); 						//����KEY����
	OSTaskCreate(display_task,(void*)0,(OS_STK*)&DISPLAY_TASK_STK[DISPLAY_STK_SIZE-1],DISPLAY_TASK_PRIO); 		//��ʾ����
	OSTaskCreate(rs232DataProc_task,(void*)0,(OS_STK*)&DATAPRO_TASK_STK[DATAPRO_STK_SIZE-1],DATAPROC_TASK_PRIO); //��ʾ����
	OSTaskSuspend(OS_PRIO_SELF); 	//����start_task����
	OS_EXIT_CRITICAL();  			//���ж�
}


//��ʾ��ַ����Ϣ��������
void display_task(void *pdata)
{
	while(1)
	{ 
#if LWIP_DHCP									//������DHCP��ʱ��
		if(lwipdev.dhcpstatus != 0) 			//����DHCP
		{
			show_address(lwipdev.dhcpstatus );	//��ʾ��ַ��Ϣ
			OSTaskSuspend(OS_PRIO_SELF); 		//��ʾ���ַ��Ϣ�������������
		}
#else
		show_address(0); 						//��ʾ��̬��ַ
		OSTaskSuspend(OS_PRIO_SELF); 			//��ʾ���ַ��Ϣ�������������
#endif //LWIP_DHCP
		OSTimeDlyHMSM(0,0,0,500);
	}
}

//key����
void key_task(void *pdata)
{
	u8 key; 
	while(1)
	{
		key = key_Proc5axisComm(0);
		if(key==KEYXa_PRES) 				//��������
		{
			udp_flag |= LWIP_SEND_DATA; //���LWIP������Ҫ����
		}
		OSTimeDlyHMSM(0,0,0,10);  		//��ʱ10ms
	}
}

//led����
void led_task(void *pdata)
{
	while(1)
	{
		LED0 = !LED0;
		OSTimeDlyHMSM(0,0,0,500); 		//��ʱ500ms
 	}
}



//rs232DataProc����
void rs232DataProc_task(void *pdata)
{
	while(1)
	{
		if(recvFrameok == 1)
		{
	//		rs232DataProc();
			recvFrameok = 0;
		}
		
		OSTimeDlyHMSM(0,0,0,500); 		//��ʱ500ms
 	}
}







