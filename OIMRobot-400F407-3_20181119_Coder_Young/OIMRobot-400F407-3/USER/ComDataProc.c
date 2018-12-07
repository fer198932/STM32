/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          原能智能制造机器人
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : ComDataProc.C
* By  : Hiker Lee
* 2018-04-10
* 2018-06-28
********************************************************************************************************/
#include "delay.h"
#include "sys.h"
#include "SysInit.h"
#include "ComDataProc.h"

//////////////////////////自检	/////////////////////
#define CHUANKOUZHUANGTAI 		0X01     //
#define YULIU1 					0X02     //
#define YULIU2 					0X03     //

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

extern unsigned char uartDataRecvProc[1024];
unsigned char backResString[20];
unsigned char coderResString[98];		// 用来回复设定参数的数组
u32 xAxisPlusNum,yAxisPlusNum,zAxisPlusNum,aAxisPlusNum,bAxisPlusNum;
u16 xAxisClk,yAxisClk,zAxisClk,aAxisClk,bAxisClk;
u16 send_xAxisClk,send_yAxisClk,send_zAxisClk,send_aAxisClk,send_bAxisClk;
unsigned char  xAxisDir,yAxisDir,zAxisDir,aAxisDir,bAxisDir;

u8 xAxisStepFlag = -1; 
u8 yAxisStepFlag = -1; 
u8 zAxisStepFlag = -1; 
u8 aAxisStepFlag = -1; 
u8 bAxisStepFlag = -1; 

u32 workTimeMSSum = 0;
u32 workedTimeMS = 0;

extern u8 TIM8_Pulse_TIM1_Counter_OK; 
extern u8 TIM5_Pulse_TIM2_Counter_OK; 
extern u8 TIM3_Pulse_TIM4_Counter_OK; 

extern u32 pwmNumTim12;
extern u32 pwmNumTim9;
extern u32 pwmNumTim3;
extern u32 pwmNumTim4;
extern u32 pwmNumTim2;

extern u8 urgentStopFlag; 

extern u8 targeToolNo;
extern u8 usingToolNo;

u8 addSubStepSum=1;
u8 addSubStepNo=1;
u8 addSubFlag=1;	//0:需要定时器配置加减速；1:不需要；

u8 dataPreProcFlag = 1;		//0:已完成预处理，待第二部处理；1:等待下一帧数据的预处理；
u32 maxPlusNum=0;

extern u32 keyValue;		//5轴的限位及零位变量；
u8 limitSportJudgFlag = 0;	// 1限位停止；0正常；

u8 addcount=0;//记录加减速的步数，用于判断是否需要加减速

u8 addtime;//加速时间变量

u32 CountNeedAdd;//累计计算加速所需脉冲个数

extern unsigned char respFlag;	// 标记是否回复

nAxisSetData allAxisSetData[3];	// 记录设定的运动参数
SetDataPerAxis setDataPerAxis;	// 完整记录某一轴的运动参数，暂时为X轴
//u16 setClkTemp;									// 临时保存设定的频率
extern u16 setClkTempNext;									// 临时保存频率数据

u8 testSetDataFlag = 0;			// 标记当前的运行状态
u8 setedDataResFlag = 1;		// 标记动作完成，完成后才可以串口返回

void stepMotor5AxisStop(void)
{
	TIM_Cmd(TIM3, DISABLE); 	//使能x
	TIM_Cmd(TIM2, DISABLE);		//使能y
	TIM_Cmd(TIM4, DISABLE); 	//使能z
	TIM_Cmd(TIM9, DISABLE); 	//使能a
	TIM_Cmd(TIM12, DISABLE);	//使能b

	xAxisStepFlag = 2;
	yAxisStepFlag = 2;
	zAxisStepFlag = 2;
	aAxisStepFlag = 2;
	bAxisStepFlag = 2;
}

void selfCheckFunc(void)
{
	unsigned char i=0;
	unsigned char printFlag=0;
	unsigned char commandKey;
	u8 temp;
	
	//0E 09 0b 07 00 01 FF
	
	commandKey = uartDataRecvProc[1];
	switch(commandKey)
	{
		case 0x01:
		case 0x02:
		case 0x03:
			backResString[3] = 0x08;
			backResString[4] = 0x00;
			break;
		default:
				printFlag = 1;
				break;
	}

	temp = UrgentStopTest();		//急停开关检测 0:按下；1:松开；2:抖动；
	
	if(printFlag ==0)
	{
		backResString[0] = FrameB1;
		backResString[1] = FrameB2;
		backResString[2] = SELFCHECK;
		
		
		backResString[5] = commandKey;
		if(temp == 1)
			backResString[6] = 0x01;
		else if(temp == 0)
			backResString[6] = 0x03;
		else
			backResString[6] = 0x04;
		
		backResString[7] = FrameEnd;
		for(i=0;i<8;i++)
			{
				USART_SendData(USART3,backResString[i]); 
				while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);	//等待发送结束
			}
	}
}

void controlFunc(void)
{
	unsigned char i=0;
	unsigned char printFlag=0;
	unsigned char commandKey;
	
		
	commandKey =  uartDataRecvProc[1]+(uartDataRecvProc[2]<<8);		//uartDataRecvProc[1];  //uartDataRecvProc[2] 保留

	switch(commandKey)
	{
		case 0x0001:		//0E 09 0C 08 00 01 00 FF
			mainMotorStop();
			break;
		case 0x0002:		//0e 09 0c 08 00 02 00 00 ff
			//motorTwistScrewLock();
			//motorTargetPUSH();
			Clamp();
			break;
		case 0x0003:		//0e 09 0c 08 00 03 00 00 ff
			//motorTwistScrewUnLock();
			//motorTargetBack();
			UnClamp();
			break;
		// 上刀	
		case 0x0004:		//0e 09 0c 08 00 04 00 00 ff
			targeToolNo = 1;
			assemTools();
			break;
		case 0x0005:
			targeToolNo = 2;
			assemTools();
			break;
		case 0x0006:
			targeToolNo = 3;
			assemTools();
			break;
		case 0x0007:
			targeToolNo = 4;
			assemTools();
			break;
		//脱刀	char变量，但是0x500是不是太大了？ 以下指令没有在接口文档中？ byYJY 2018年11月9日11:16:49
		case 0x0400:
			usingToolNo = 1;
			cleanTools();
			break;
		case 0x0500:
			usingToolNo = 2;
			cleanTools();
			break;
		case 0x0600:
			usingToolNo = 3;
			cleanTools();
			break;
		case 0x0700:
			usingToolNo = 4;
			cleanTools();
			break;
		//换刀	
		case 0x0405:
			usingToolNo = 1;
			targeToolNo = 2;
			changeToolsStep1();
			changeToolsStep2();
			break;
		case 0x0406:
			usingToolNo = 1;
			targeToolNo = 3;
			changeToolsStep1();
			changeToolsStep2();
			break;
		case 0x0407:
			usingToolNo = 1;
			targeToolNo = 4;
			changeToolsStep1();
			changeToolsStep2();
			break;
			
		case 0x0504:
			usingToolNo = 2;
			targeToolNo = 1;
			changeToolsStep1();
			changeToolsStep2();
			break;
		case 0x0506:
			usingToolNo = 2;
			targeToolNo = 3;
			changeToolsStep1();
			changeToolsStep2();
			break;
		case 0x0507:
			usingToolNo = 2;
			targeToolNo = 4;
			changeToolsStep1();
			changeToolsStep2();
			break;
			
		case 0x0604:
			usingToolNo = 3;
			targeToolNo = 1;
			changeToolsStep1();
			changeToolsStep2();
			break;
		case 0x0605:
			usingToolNo = 3;
			targeToolNo = 2;
			changeToolsStep1();
			changeToolsStep2();
			break;
		case 0x0607:
			usingToolNo = 3;
			targeToolNo = 4;
			changeToolsStep1();
			changeToolsStep2();
			break;
				
		case 0x0704:
			usingToolNo = 4;
			targeToolNo = 1;
			changeToolsStep1();
			changeToolsStep2();
			break;
		case 0x0705:
			usingToolNo = 4;
			targeToolNo = 2;
			changeToolsStep1();
			changeToolsStep2();
			break;
		case 0x0706:
			usingToolNo = 4;
			targeToolNo = 3;
			changeToolsStep1();
			changeToolsStep2();
			break;
		//换刀ok		
		case 0x0008:		//0E 09 0C 08 00 08 00 FF
			//mainMotorDecode();		//必须手动解码；
			//delay_us(100);
			mainMotorWork();
			break;
		case 0x000a:
			motorChangeToolsLock();		//test
			break;
		case 0x000b:
			motorChangeToolsUnLock();	//test
			break;
		case 0x0009:
			motorTwistScrewLock();
			break;
		case 0x000c:
			motorTwistScrewUnLock();
			break;
		case 0x000d:		//0E 09 0C 08 00 0D 00 FF
			mainMotorStop();
			TIM_Cmd(TIM3, DISABLE); 	//使能x
			TIM_Cmd(TIM4, DISABLE); 	//使能z
			TIM_Cmd(TIM9, DISABLE); 	//使能a
			TIM_Cmd(TIM12, DISABLE);	//使能b
			TIM_Cmd(TIM5, DISABLE);
			TIM_Cmd(TIM2, DISABLE);
			urgentStopFlag = 1;
			
			printFlag = 1;
			break;
		default:
			printFlag = 1;
			break;
	}

	if(printFlag ==0)
	{
		backResString[0] = FrameB1;
		backResString[1] = FrameB2;
		backResString[2] = CONTROLINFO;
		
		backResString[3] = 9;
		backResString[4] = 0x00;
		
		backResString[5] = commandKey;
		backResString[6] = commandKey>>8;
		backResString[7] = 0x01;
		
		backResString[8] = FrameEnd;
		
		for(i=0;i<9;i++)
			{
				USART_SendData(USART3,backResString[i]); 
				while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
			}
	}
}

void statusFunc(void)
{
	unsigned char i=0;
	unsigned char printFlag=0;
	unsigned char commandKey;
	
	//0E 09 0C 08 00 01 00 FF
	
	commandKey =  uartDataRecvProc[1]+(uartDataRecvProc[2]<<8);		//uartDataRecvProc[1];  //uartDataRecvProc[2] 保留

	switch(commandKey)
	{
		case 0x31:
		case 0x32:
		case 0x33:
		case 0x34:
		case 0x35:
		case 0x36:
		case 0x37:
		case 0x38:
		case 0x39:
		case 0x3a:
			backResString[3] = 9;
			backResString[4] = 0x00;
			break;
		case 0x3b:
		case 0x3c:
		case 0x3d:
		case 0x3e:
		case 0x3f:
		case 0x40:
			backResString[3] = 9;
			backResString[4] = 0x00;
			break;
		default:
			printFlag = 1;
			break;
	}
	
	if(printFlag ==0)
	{
		backResString[0] = FrameB1;
		backResString[1] = FrameB2;
		backResString[2] = STATUSINFO;
		
		backResString[5] = commandKey;
		backResString[6] = 0x00;
		backResString[7] = 0x01;
		
		backResString[8] = FrameEnd;
		
		if(0 == respFlag)
		{
			for(i=0;i<9;i++)
				{
					USART_SendData(USART3,backResString[i]); 
					//delay_us(100);
					while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//等待发送结束
				}
		}
	}
}
void dataFunc(void)
{
	unsigned char i=0;
	unsigned char printFlag=0;
	unsigned int commandKey;
	
	//0E 09 0d +2B(length)+2B(command)+[2B(plus)+ 2B(clk)]x5 + 4B(CRC)+FF
	
	//0E 09 0d 10 00 22 00 0a 00 64 00 00 00 00 00 FF
	//0E 09 0d 10 00 22 00 0a 00 64 00 00 00 00 00 FF
	//0E 09 0D 20 00 22 00 64 00 64 00 64 00 64 80 64 00 64 80 00 00 00 00 00 00 00 00 EF F9 0E A8 FF
	//0E 09 0D 20 00 22 00 64 00 64 00 64 00 64 80 64 00 64 80 64 00 64 80 64 00 64 80 EF F9 0E A8 FF
	//0E 09 0D 20 00 22 00 64 00 64 00 64 00 64 80 64 00 04 81 64 00 64 80 64 00 64 80 EF F9 0E A8 FF
	
	//0E 09 0D 20 00 22 00 (64 00 64 00) (64 00 64 80) (64 00 64 80) (00 00 00 00) (00 00 00 00) EF F9 0E A8 FF
	//0E 09 0D 20 00 22 00 00 00 00 00 00 00 00 00 64 00 98 00 00 00 00 00 00 000 000 00 EC CF F1 83 FF
	
//X+ //0E 09 0D 2A 00 22 00 E0 2E 00 E8 13 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E2 E1 6A 0F FF
//X- //0E 09 0D 2A 00 22 00 E0 2E 00 E8 13 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E2 E1 6A 0F FF

//Y+ //0E 09 0D 2A 00 22 00 00 00 00 00 00 00 E0 2E 00 E8 13 80 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E2 E1 6A 0F FF
//Y- //0E 09 0D 2A 00 22 00 00 00 00 00 00 00 E0 2E 00 E8 13 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E2 E1 6A 0F FF

//Z+ //0E 09 0D 2A 00 22 00 00 00 00 00 00 00 00 00 00 00 00 00 E0 2E 00 E8 13 00 00 00 00 00 00 00 00 00 00 00 00 00 E2 E1 6A 0F FF
//Z- //0E 09 0D 2A 00 22 00 00 00 00 00 00 00 00 00 00 00 00 00 E0 2E 00 E8 13 80 00 00 00 00 00 00 00 00 00 00 00 00 E2 E1 6A 0F FF

//A+ //0E 09 0D 2A 00 22 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E0 2E 00 E8 13 80 00 00 00 00 00 00 E2 E1 6A 0F FF
//A- //0E 09 0D 2A 00 22 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E0 2E 00 E8 13 00 00 00 00 00 00 00 E2 E1 6A 0F FF

//B //0E 09 0D 2A 00 22 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E0 2E 00 00 20 00 E2 E1 6A 0F FF
//B //0E 09 0D 2A 00 22 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E0 2E 00 00 20 80 E2 E1 6A 0F FF			//90度

	//0E 09 0D 2A 00 22 00 00 00 00 00 00 00 00 00 00 00 00 00 E0 2E 00 E8 03 80 00 00 00 00 00 00 00 00 00 00 00 00 E2 E1 6A 0F FF
	//0E 09 0D 2A 00 22 00 (00 00 00 00 00 00) (00 00 00 00 00 00) (68 2E 00 D0 07 00) (00 00 00 00 00 00) (00 00 00 00 00 00) CF 6E 1B F3 FF
	//0E 09 0D 09 00 22 00 01 FF
	commandKey = uartDataRecvProc[1]+(uartDataRecvProc[2]<<8);
	switch(commandKey)
	{
		case 0x21:
			break;
		case 0x22:
			//x axis 345,678
			xAxisPlusNum= uartDataRecvProc[3]+ (uartDataRecvProc[4]<<8)+ (uartDataRecvProc[5]<<16);
			xAxisClk = uartDataRecvProc[6]+ (uartDataRecvProc[7]<<8)+ ((0x7f&uartDataRecvProc[8])<<16);
			xAxisDir = uartDataRecvProc[8] >> 7;
			if(xAxisDir==0)
				{
					xAxisDIR_ON;
				}
			else
				{
					xAxisDIR_OFF;
				}
		//	xAxisStepFlag = 0;				//0614
			
			// ***********************************//
			//y axis 9,10,11,12,13,14
			yAxisPlusNum= uartDataRecvProc[9]+ (uartDataRecvProc[10]<<8)+ (uartDataRecvProc[11]<<16);
			yAxisClk = uartDataRecvProc[12]+ (uartDataRecvProc[13]<<8)+ ((0x7f&uartDataRecvProc[14])<<16);
			yAxisDir = uartDataRecvProc[14] >> 7;
			if(yAxisDir==0)
				{
					yAxisDIR_ON;
				}
			else
				{
					yAxisDIR_OFF;
				}
		//	yAxisStepFlag = 0;
		
			// ***********************************//
			//z axis 15,16,17,18,19,20
			zAxisPlusNum= uartDataRecvProc[15]+ (uartDataRecvProc[16]<<8)+ (uartDataRecvProc[17]<<16);
			zAxisClk = uartDataRecvProc[18]+ (uartDataRecvProc[19]<<8)+ ((0x7f&uartDataRecvProc[20])<<16);
			zAxisDir = uartDataRecvProc[20] >> 7;
			if(zAxisDir==0)
				{
					zAxisDIR_ON;
				}
			else
				{
					zAxisDIR_OFF;
				}
		//	zAxisStepFlag = 0;
		
			// ***********************************//
			//A axis 21,22,23,24,25,26
			aAxisPlusNum= uartDataRecvProc[21]+ (uartDataRecvProc[22]<<8)+ (uartDataRecvProc[23]<<16);
			aAxisClk = uartDataRecvProc[24]+ (uartDataRecvProc[25]<<8)+ ((0x7f&uartDataRecvProc[26])<<16);
			aAxisDir = uartDataRecvProc[26] >> 7;
			if(aAxisDir==0)
				{
					aAxisDIR_ON;
				}
			else
				{
					aAxisDIR_OFF;
				}
		//	aAxisStepFlag = 0;
			// ***********************************//
			//B axis 27,28,29,30,31,32
			bAxisPlusNum= uartDataRecvProc[27]+ (uartDataRecvProc[28]<<8)+ (uartDataRecvProc[29]<<16);
			bAxisClk = uartDataRecvProc[30]+ (uartDataRecvProc[31]<<8)+ ((0x7f&uartDataRecvProc[32])<<16);
			bAxisDir = uartDataRecvProc[32] >> 7;
			if(bAxisDir==0)
				{
					bAxisDIR_ON;
				}
			else
				{
					bAxisDIR_OFF;
				}
		//	bAxisStepFlag = 0;
			break;
				
		// 复制0x23的代码
			case 0x23:			// 组发送测试用的
			//x axis 345,678
			xAxisPlusNum= uartDataRecvProc[3]+ (uartDataRecvProc[4]<<8)+ (uartDataRecvProc[5]<<16);
			xAxisClk = uartDataRecvProc[6]+ (uartDataRecvProc[7]<<8)+ ((0x7f&uartDataRecvProc[8])<<16);
			xAxisDir = uartDataRecvProc[8] >> 7;
			if(xAxisDir==0)
				{
					xAxisDIR_ON;
				}
			else
				{
					xAxisDIR_OFF;
				}
		//	xAxisStepFlag = 0;				//0614
			
			// ***********************************//
			//y axis 9,10,11,12,13,14
			yAxisPlusNum= uartDataRecvProc[9]+ (uartDataRecvProc[10]<<8)+ (uartDataRecvProc[11]<<16);
			yAxisClk = uartDataRecvProc[12]+ (uartDataRecvProc[13]<<8)+ ((0x7f&uartDataRecvProc[14])<<16);
			yAxisDir = uartDataRecvProc[14] >> 7;
			if(yAxisDir==0)
				{
					yAxisDIR_ON;
				}
			else
				{
					yAxisDIR_OFF;
				}
		//	yAxisStepFlag = 0;
		
			// ***********************************//
			//z axis 15,16,17,18,19,20
			zAxisPlusNum= uartDataRecvProc[15]+ (uartDataRecvProc[16]<<8)+ (uartDataRecvProc[17]<<16);
			zAxisClk = uartDataRecvProc[18]+ (uartDataRecvProc[19]<<8)+ ((0x7f&uartDataRecvProc[20])<<16);
			zAxisDir = uartDataRecvProc[20] >> 7;
			if(zAxisDir==0)
				{
					zAxisDIR_ON;
				}
			else
				{
					zAxisDIR_OFF;
				}
		//	zAxisStepFlag = 0;
		
			// ***********************************//
			//A axis 21,22,23,24,25,26
			aAxisPlusNum= uartDataRecvProc[21]+ (uartDataRecvProc[22]<<8)+ (uartDataRecvProc[23]<<16);
			aAxisClk = uartDataRecvProc[24]+ (uartDataRecvProc[25]<<8)+ ((0x7f&uartDataRecvProc[26])<<16);
			aAxisDir = uartDataRecvProc[26] >> 7;
			if(aAxisDir==0)
				{
					aAxisDIR_ON;
				}
			else
				{
					aAxisDIR_OFF;
				}
		//	aAxisStepFlag = 0;
			// ***********************************//
			//B axis 27,28,29,30,31,32
			bAxisPlusNum= uartDataRecvProc[27]+ (uartDataRecvProc[28]<<8)+ (uartDataRecvProc[29]<<16);
			bAxisClk = uartDataRecvProc[30]+ (uartDataRecvProc[31]<<8)+ ((0x7f&uartDataRecvProc[32])<<16);
			bAxisDir = uartDataRecvProc[32] >> 7;
			if(bAxisDir==0)
				{
					bAxisDIR_ON;
				}
			else
				{
					bAxisDIR_OFF;
				}
		//	bAxisStepFlag = 0;
			break;
				
		default:
				printFlag = 1;
				break;
	}
	
//	if(printFlag ==1)
//	{
//		backResString[0] = FrameB1;
//		backResString[1] = FrameB2;
//		backResString[2] = DATAINFO;
//		
//		
//		backResString[5] = commandKey;
//		backResString[6] = 0x01;
//		
//		backResString[7] = FrameEnd;
//		for(i=0;i<8;i++)
//			{
//				USART_SendData(USART3,backResString[i]); 
//				Delay_us(100);
//			}
//	}
}

u8 LimitSportJudg(void)
{
	u8 i;
	if(keyValue != 0)	//有限位
	{
	
		backResString[0] = FrameB1;
		backResString[1] = FrameB2;
		backResString[2] = STATUSINFO;
		
		backResString[3] = 9;
		backResString[4] = 0x00;
		
		backResString[8] = FrameEnd;
	
			if(keyValue&0x0001) //KEYXa_PRES
			{
				if(xAxisDir==1)		//正方向
					{
						stepMotor5AxisStop();
						
						backResString[5] = 0x31;
						backResString[6] = 0x00;
						backResString[7] = 0x01;
						
						for(i=0;i<9;i++)
							{
								USART_SendData(USART3,backResString[i]); 
								while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//等待发送结束
							}
						return 1;
					}
			}
			if((keyValue>>1)&0x0001)	//KEYXb_PRES
			{
				if(xAxisDir==0)		//负方向
					{
						stepMotor5AxisStop();
						
						backResString[5] = 0x31;
						backResString[6] = 0x00;
						backResString[7] = 0x02;
						
						for(i=0;i<9;i++)
							{
								USART_SendData(USART3,backResString[i]); 
								while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//等待发送结束
							}
						return 1;
					}
			}
			if((keyValue>>2)&0x0001)	//KEYYa_PRES
			{
				if(yAxisDir==1)		//正方向
					{
						stepMotor5AxisStop();
						
						backResString[5] = 0x32;
						backResString[6] = 0x00;
						backResString[7] = 0x01;
						
						for(i=0;i<9;i++)
							{
								USART_SendData(USART3,backResString[i]); 
								while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//等待发送结束
							}
						return 1;
					}
			}
		
			if((keyValue>>3)&0x0001)	//KEYYb_PRES
			{
				if(yAxisDir==0)		//负方向
					{
						stepMotor5AxisStop();
						
						backResString[5] = 0x32;
						backResString[6] = 0x00;
						backResString[7] = 0x02;
						
						for(i=0;i<9;i++)
							{
								USART_SendData(USART3,backResString[i]); 
								while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//等待发送结束
							}
						return 1;
					}
			}
		
			if((keyValue>>4)&0x0001)	//KEYZa_PRES
			{
				if(zAxisDir==1)		//正方向
					{
						stepMotor5AxisStop();
						
						backResString[5] = 0x33;
						backResString[6] = 0x00;
						backResString[7] = 0x01;
						
						for(i=0;i<9;i++)
							{
								USART_SendData(USART3,backResString[i]); 
								while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//等待发送结束
							}
						return 1;
					}
			}
		
			if((keyValue>>5)&0x0001)	//KEYZb_PRES
			{
				if(zAxisDir==0)		//负方向
					{
						stepMotor5AxisStop();
						
						backResString[5] = 0x33;
						backResString[6] = 0x00;
						backResString[7] = 0x02;
						
						for(i=0;i<9;i++)
							{
								USART_SendData(USART3,backResString[i]); 
								while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//等待发送结束
							}
						return 1;
					}
			}
			
			return 0;
	}
}

u16 preProcMaxClk(void)
{
	u16 maxClk;
	maxClk = max(yAxisClk,xAxisClk);
	maxClk = max(maxClk,zAxisClk);
	maxClk = max(maxClk,aAxisClk);
	maxClk = max(maxClk,bAxisClk);
	return maxClk;
}
void preProcMaxNum(void)
{
	maxPlusNum = max(yAxisPlusNum,xAxisPlusNum);
	maxPlusNum = max(maxPlusNum,zAxisPlusNum);
	maxPlusNum = max(maxPlusNum,aAxisPlusNum);
	maxPlusNum = max(maxPlusNum,bAxisPlusNum);
}

u32 preProcMinPlusNum(void)		//求得不为0的最小值；
{
	u32 minPlusNum =0;
	u32 temp =0;
	u32 a[5];
	u8 i,j;

	a[0] = yAxisPlusNum;
	a[1] = xAxisPlusNum;
	a[2] = zAxisPlusNum;
	a[3] = aAxisPlusNum;
	a[4] = bAxisPlusNum;
	
	for(i = 0; i < 5 ; i++)
		{
	        for(j = 5 -1 ; j >i ; j--)
	        {
	            if(a[j-1] < a[j])
	            {
	                temp = a[j -1];
	                a[j -1]=a[j];
	                a[j]=temp;
	            }
	        }
		}
	if(a[0]==0)
	{
		if(a[1]==0)	
		{
			if(a[2]==0)
			{
				if(a[3]==0)
				{
						minPlusNum = a[4];
				}
				else
					minPlusNum = a[3];
			}
			else
				minPlusNum = a[2];
		}
		else
			minPlusNum = a[1];
	}
	else
		minPlusNum = a[0];
		
	return minPlusNum;
}

u32 preProcTime(void)
{
	u32 time1,time2,time3,time4,time5;
	u32 time;
	
	if(yAxisClk !=0)
		time1 = (yAxisPlusNum *1000)/yAxisClk;		//ms
	else 
		time1 = 0;
	
	if(xAxisClk !=0)
		time2 = (xAxisPlusNum *1000)/xAxisClk;
	else
		time2 = 0;
	
	if(zAxisClk !=0)
		time3 = (zAxisPlusNum *1000)/zAxisClk;
	else
		time3 = 0;
	
	if(aAxisClk !=0)
		time4 = (aAxisPlusNum *1000)/aAxisClk;
	else
		time4 = 0;
	
	if(bAxisClk !=0)
		time5 = (bAxisPlusNum *1000)/bAxisClk;
	else
		time5 = 0;

	time = max(time1,time2);
	time = max(time,time3);
	time = max(time,time4);
	time = max(time,time5);
	
	return time;			//获得运行时间，单位ms
}


u8 preProcStep(void)
{
	u16 maxClk;
	u8 addStep;

	maxClk = preProcMaxClk();
	
	if(maxClk<2000)				//2000
	{
		addStep = 1;
	}
	else if((maxClk>=2000)&&(maxClk<4000))		//2000~4000
	{
		addStep = 2;
	}
	else if((maxClk>=4000)&&(maxClk<8000))		//4000~8000
	{
		addStep = 3;
	}
	else if((maxClk>=8000)&&(maxClk<12000))		//2000~4000
	{
		addStep = 4;		//直接倍频加速
	}
	else if((maxClk>=12000)&&(maxClk<14000))
	{
		addStep = 5;		//
	}
	else if((maxClk>=14000)&&(maxClk<16000))
	{
		addStep = 6;
	}
	else if((maxClk>=16000)&&(maxClk<18000))
	{
		addStep = 7;
	}
	else if((maxClk>=18000)&&(maxClk<36000))
	{
		//18000~36000
		//9000~18000
		//14000~18000
		//1750~2250  3500~4500	7000~9000	7000+3500~9000+4500 7000+5250~9000+6750 14000~18000
		// |8			|4			|2			75% 				50%+37.5%			|1
		
		yAxisClk = yAxisClk >>1;
		xAxisClk = xAxisClk >>1;
		zAxisClk = zAxisClk >>1;
		aAxisClk = aAxisClk >>1;
		bAxisClk = bAxisClk >>1;
		
		addStep = 7;

		workTimeMSSum = preProcTime();
	}
	else 					//36000~65535
	{
		//36000~65535
		//9000~16383
		//14000~18000
		//1750~2250  3500~4500	7000~9000	7000+3500~9000+4500 7000+5250~9000+6750 14000~18000
		// |8			|4			|2			75% 				50%+37.5%			|1
		
		yAxisClk = yAxisClk >>2;
		xAxisClk = xAxisClk >>2;
		zAxisClk = zAxisClk >>2;
		aAxisClk = aAxisClk >>2;
		bAxisClk = bAxisClk >>2;
		
		addStep = 7;

		workTimeMSSum = preProcTime();
	}

	return addStep;
//	u16 maxClk;
//	u8 addStep;

//	maxClk = preProcMaxClk();
//	
//	if(maxClk<200)				//200
//	{
//		addStep = 1;
//	}
//	else if((maxClk>=200)&&(maxClk<400))		//200~400
//	{
//		addStep = 2;
//	}
//	else if((maxClk>=400)&&(maxClk<800))		//400~800
//	{
//		addStep = 3;
//	}
//	else if((maxClk>=800)&&(maxClk<1600))		//800~1600
//	{
//		addStep = 4;		//直接倍频加速
//	}
//	else if((maxClk>=1600)&&(maxClk<3200))
//	{
//		addStep = 5;		//
//	}
//	else if((maxClk>=3200)&&(maxClk<6400))
//	{
//		addStep = 6;
//	}
//	else if((maxClk>=6400)&&(maxClk<12800))
//	{
//		addStep = 7;
//	}
//	else if((maxClk>=12800)&&(maxClk<25600))
//	{
//		addStep = 8;
//	}
//	else if((maxClk>=25600)&&(maxClk<51200))
//	{
//			addStep = 9;
//	}
//	else 
//	{
//		yAxisClk = yAxisClk >>1;
//		xAxisClk = xAxisClk >>1;
//		zAxisClk = zAxisClk >>1;
//		aAxisClk = aAxisClk >>1;
//		bAxisClk = bAxisClk >>1;
//		
//		addStep = 9;

//		workTimeMSSum = preProcTime();
//	}

//	return addStep;
}

void speedSet(u8 stepSum,u8 stepNo)
{
	switch(stepSum)
	{
		case 1:		//stepNo = 1;			//2000
					//2000
			if(yAxisPlusNum>=1)
				allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk);
			if(xAxisPlusNum>=1)
				allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk);
			if(zAxisPlusNum>=1)
				allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk);
			if(aAxisPlusNum>=1)
				aAxisClkSet(aAxisClk);
			if(bAxisPlusNum>=1)
				bAxisClkSet(bAxisClk);
			break;
		case 2:		//stepNo = 1;2;		//2000,4000
					//2000~4000
					//1000~2000	2000~4000
			if(yAxisPlusNum>=1)
				allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk>>(2-stepNo));
			if(xAxisPlusNum>=1)
				allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk>>(2-stepNo));
			if(zAxisPlusNum>=1)
				allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk>>(2-stepNo));
			if(aAxisPlusNum>=1)
				aAxisClkSet(aAxisClk>>(2-stepNo));
			if(bAxisPlusNum>=1)
				bAxisClkSet(bAxisClk>>(2-stepNo));
			break;
		case 3:		//stepNo = 1;2;3;		//2000,4000,8000
					//4000~8000
					//1000~2000	2000~4000	4000~8000
			if(yAxisPlusNum>=1)
				allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk>>(3-stepNo));
			if(xAxisPlusNum>=1)
				allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk>>(3-stepNo));
			if(zAxisPlusNum>=1)
				allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk>>(3-stepNo));
			if(aAxisPlusNum>=1)
				aAxisClkSet(aAxisClk>>(3-stepNo));
			if(bAxisPlusNum>=1)
				bAxisClkSet(bAxisClk>>(3-stepNo));
			break;
		case 4:		//stepNo = 1;2;3;4;	//加速步骤
					//8000~12000
					//1000~1500	2000~3000	4000~6000	8000~12000
			if(yAxisPlusNum>=1)
				allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk>>(4-stepNo));
			if(xAxisPlusNum>=1)
				allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk>>(4-stepNo));
			if(zAxisPlusNum>=1)
				allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk>>(4-stepNo));
			if(aAxisPlusNum>=1)
				aAxisClkSet(aAxisClk>>(4-stepNo));
			if(bAxisPlusNum>=1)
				bAxisClkSet(bAxisClk>>(4-stepNo));
			break;
		case 5:		//stepNo = 1;2;3;4;5;	//2000,4000,8000,16000
					//12000~14000
					//1500~1750	 3000~3500	6000~7000	6000+4500~7000+5250	12000~14000
					// |8			|4			|2			50%+37.5%			|1
			if(stepNo < 4)
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk>>(4-stepNo));
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk>>(4-stepNo));
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk>>(4-stepNo));
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>(4-stepNo));
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>(4-stepNo));
			}
			else if(stepNo == 4)
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet((yAxisClk>>1)+(yAxisClk>>2)+(yAxisClk>>3));
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet((xAxisClk>>1)+(xAxisClk>>2)+(xAxisClk>>3));
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet((zAxisClk>>1)+(zAxisClk>>2)+(zAxisClk>>3));
				if(aAxisPlusNum>=1)
					aAxisClkSet((aAxisClk>>1)+(aAxisClk>>2)+(aAxisClk>>3));
				if(bAxisPlusNum>=1)
					bAxisClkSet((bAxisClk>>1)+(bAxisClk>>2)+(bAxisClk>>3));
			}
			else if(stepNo == 5)
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk);
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk);
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk);
			}
			break;
		
		case 6:		//stepNo = 1;2;3;4;5;6;7;		//2000,4000,8000,16000
					//14000~16000
					//1750~2000	 3500~4000	7000~8000	7000+3500~9000+4500	7000+5250~9000+6750	14000~16000
					// |8			|4			|2			75%					50%+37.5%			|1
			if(stepNo < 4)
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk>>(4-stepNo));
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk>>(4-stepNo));
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk>>(4-stepNo));
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>(4-stepNo));
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>(4-stepNo));
			}
			else if(stepNo == 4)
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet((yAxisClk>>1)+(yAxisClk>>2));
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet((xAxisClk>>1)+(xAxisClk>>2));
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet((zAxisClk>>1)+(zAxisClk>>2));
				if(aAxisPlusNum>=1)
					aAxisClkSet((aAxisClk>>1)+(aAxisClk>>2));
				if(bAxisPlusNum>=1)
					bAxisClkSet((bAxisClk>>1)+(bAxisClk>>2));
			}
			else if(stepNo == 5)
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet((yAxisClk>>1)+(yAxisClk>>2)+(yAxisClk>>3));
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet((xAxisClk>>1)+(xAxisClk>>2)+(xAxisClk>>3));
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet((zAxisClk>>1)+(zAxisClk>>2)+(zAxisClk>>3));
				if(aAxisPlusNum>=1)
					aAxisClkSet((aAxisClk>>1)+(aAxisClk>>2)+(aAxisClk>>3));
				if(bAxisPlusNum>=1)
					bAxisClkSet((bAxisClk>>1)+(bAxisClk>>2)+(bAxisClk>>3));
			}
			else if(stepNo == 6)
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk);
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk);
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk);
			}
			break;
		
		case 7:		//stepNo = 1;2;3;4;5;6;		//2000,4000,8000,16000
					//16000~18000
					//1750~2250	 3500~4500	7000~9000	7000+3500~9000+4500	7000+5250~9000+6750	14000~18000
					// |8			|4			|2			75%					50%+37.5%			|1
			if(stepNo < 4)
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk>>(4-stepNo));
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk>>(4-stepNo));
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk>>(4-stepNo));
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>(4-stepNo));
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>(4-stepNo));
			}
			else if(stepNo == 4)
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet((yAxisClk>>1)+(yAxisClk>>2));
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet((xAxisClk>>1)+(xAxisClk>>2));
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet((zAxisClk>>1)+(zAxisClk>>2));
				if(aAxisPlusNum>=1)
					aAxisClkSet((aAxisClk>>1)+(aAxisClk>>2));
				if(bAxisPlusNum>=1)
					bAxisClkSet((bAxisClk>>1)+(bAxisClk>>2));
			}
			else if(stepNo == 5)
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet((yAxisClk>>1)+(yAxisClk>>2)+(yAxisClk>>3));
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet((xAxisClk>>1)+(xAxisClk>>2)+(xAxisClk>>3));
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet((zAxisClk>>1)+(zAxisClk>>2)+(zAxisClk>>3));
				if(aAxisPlusNum>=1)
					aAxisClkSet((aAxisClk>>1)+(aAxisClk>>2)+(aAxisClk>>3));
				if(bAxisPlusNum>=1)
					bAxisClkSet((bAxisClk>>1)+(bAxisClk>>2)+(bAxisClk>>3));
			}
			else if(stepNo == 6)
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet((yAxisClk>>1)+(yAxisClk>>2)+(yAxisClk>>3)+(yAxisClk>>4));
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet((xAxisClk>>1)+(xAxisClk>>2)+(xAxisClk>>3)+(xAxisClk>>4));
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet((zAxisClk>>1)+(zAxisClk>>2)+(zAxisClk>>3)+(zAxisClk>>4));
				if(aAxisPlusNum>=1)
					aAxisClkSet((aAxisClk>>1)+(aAxisClk>>2)+(aAxisClk>>3)+(aAxisClk>>4));
				if(bAxisPlusNum>=1)
					bAxisClkSet((bAxisClk>>1)+(bAxisClk>>2)+(bAxisClk>>3)+(bAxisClk>>4));
			}
			
			else if(stepNo == 7)
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk);
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk);
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk);
			}
			break;
		default:
			break;
	}
}

void speedReSet(u8 stepSum,u8 stepNo)
{
	
	TIM_Cmd(TIM3, DISABLE); 	//使能x
	TIM_Cmd(TIM4, DISABLE); 	//使能z
	TIM_Cmd(TIM9, DISABLE); 	//使能a
	TIM_Cmd(TIM12, DISABLE);	//使能b
//	TIM_Cmd(TIM5, DISABLE);		//计数的；
	TIM_Cmd(TIM2, DISABLE);		//使能y,产生脉冲的

	delay_us(10);				//100k
	
	
	TIM_ForcedOC1Config(TIM3, TIM_ForcedAction_Active);
	TIM_ForcedOC2Config(TIM3, TIM_ForcedAction_Active);
	
	TIM_ForcedOC3Config(TIM4, TIM_ForcedAction_Active);
	TIM_ForcedOC4Config(TIM4, TIM_ForcedAction_Active);
	
	TIM_ForcedOC1Config(TIM9, TIM_ForcedAction_Active);
	TIM_ForcedOC2Config(TIM9, TIM_ForcedAction_Active);
	
	TIM_ForcedOC1Config(TIM12, TIM_ForcedAction_Active);
	TIM_ForcedOC2Config(TIM12, TIM_ForcedAction_Active);
	
	TIM_ForcedOC1Config(TIM2, TIM_ForcedAction_Active);
	TIM_ForcedOC3Config(TIM2, TIM_ForcedAction_Active);

	speedSet(stepSum,stepNo);
}



void addSubFlagSet(void)
{
	
	u16 maxClk;
	u8 i;
	maxClk = preProcMaxClk();
	
	preProcMaxNum();
	
	workTimeMSSum = preProcTime();
	

	CountNeedAdd=0;
	for(i=1;i<=addcount;i++)
	{
		//CountNeedAdd=CountNeedAdd + (maxClk>>(addcount-i))*addtime/1000;
		if(addcount<=4)
		{
			CountNeedAdd=CountNeedAdd + (maxClk>>(addcount-i))*addtime/1000;
		}
		else if(addcount==5)
		{
			if(i<4)
			{
				CountNeedAdd=CountNeedAdd + (maxClk>>(addcount-i))*addtime/1000;
			}
			else if(i==4)
			{
				CountNeedAdd=CountNeedAdd+(maxClk>>1+maxClk>>2+maxClk>>3)*addtime/1000;
			}
			else if(i==5)
			{
				CountNeedAdd=CountNeedAdd+maxClk*addtime/1000;
			}
		}
		else if(addcount==6)
		{
			if(addcount<=4)
			{
				CountNeedAdd=CountNeedAdd + (maxClk>>(addcount-i))*addtime/1000;
			}
			else if(i==4)
			{
				CountNeedAdd=CountNeedAdd+(maxClk>>1+maxClk>>2)*addtime/1000;
			}
			else if(i==5)
			{
				CountNeedAdd=CountNeedAdd+(maxClk>>1+maxClk>>2+maxClk>>3)*addtime/1000;
			}
			else if(i==6)
			{
				CountNeedAdd=CountNeedAdd+maxClk*addtime/1000;
			}
		}
		else if(addcount==7)
		{
			if(addcount<=4)
			{
				CountNeedAdd=CountNeedAdd + (maxClk>>(addcount-i))*addtime/1000;
			}
			else if(i==4)
			{
				CountNeedAdd=CountNeedAdd+(maxClk>>1+maxClk>>2)*addtime/1000;
			}
			else if(i==5)
			{
				CountNeedAdd=CountNeedAdd+(maxClk>>1+maxClk>>2+maxClk>>3)*addtime/1000;
			}
			else if(i==6)
			{
				CountNeedAdd=CountNeedAdd+(maxClk>>1+maxClk>>2+maxClk>>3+maxClk>>4)*addtime/1000;
			}
			else if(i==7)
			{
				CountNeedAdd=CountNeedAdd+maxClk*addtime/1000;
			}
		}
	}
	if(maxPlusNum>=2*CountNeedAdd)
	{
			addSubFlag = 0;			//0:需要定时器配置加减速；1:不需要?
	}
	else
	{
			addSubFlag = 1;
			if(maxPlusNum<500)
			{
				addSubFlag = 2;
			}
	}
	
	
//	u32 minPlus;
//	
//	workTimeMSSum = preProcTime();
//	minPlus = preProcMinPlusNum();
//	
//	if((workTimeMSSum>100)&&(minPlus>25))		//20*8 =160ms
//	{
//		addSubFlag = 0;			//0:需要定时器配置加减速；1:不需要；
//	}
//	else
//	{
//		addSubFlag = 1;
//			
//	}
//	//***************Elsie***************************	
//	u16 maxClk;
//	u8 i;
//	maxClk = preProcMaxClk();
//	
//	preProcMaxNum();
//	
//	workTimeMSSum = preProcTime();
//	

//	CountNeedAdd=0;
//	for(i=1;i<addcount;i++)
//	{
//		CountNeedAdd=CountNeedAdd + (maxClk>>(addcount-i))*addtime/1000;
//	}
//	if(maxPlusNum>=2*CountNeedAdd)
//	{
//			addSubFlag = 0;			//0:需要定时器配置加减速；1:不需要?
//	}
//	else
//	{
//			addSubFlag = 1;
//	}
//	if(maxPlusNum<=500)
//	{
//			addSubFlag = 3;
//	}
//	
//	//***************Elsie***************************	
}

void addSubSpeed(void)
{
 	u16 maxClk;
	u16 setFre;
	maxClk = preProcMaxClk();
	
	switch(addSubFlag)
	{
		case 0:
			addSubStepSum = preProcStep();		//获得加减速步数
			speedSet(addSubStepSum,addSubStepNo);
			break;
		case 1:
			if(maxClk < 12000)
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk);	// y轴的设定频率 byYJY
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk);	// x轴的设定频率 byYJY
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk);	// z轴的设定频率 byYJY
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk);
			}
			else if((maxClk >= 12000)&&(maxClk < 24000))
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk>>1);
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk>>1);
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk>>1);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>1);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>1);
			}
			else if((maxClk >= 24000)&&(maxClk < 48000))
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk>>2);
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk>>2);
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk>>2);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>2);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>2);
			}
			else		//48000~65535		/8
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk>>3);
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk>>3);
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk>>3);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>3);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>3);
			}
			break;
		case 2:
			if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk>>3);
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk>>3);
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk>>3);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>3);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>3);
				break;
		case 3:
			if(maxClk < 1000)
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk);
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk);
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk);
			}
			else if((maxClk >= 1000)&&(maxClk < 2000))
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk>>1);
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk>>1);
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk>>1);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>1);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>1);
			}
			else if((maxClk >= 2000)&&(maxClk < 4000))
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk>>2);
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk>>2);
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk>>2);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>2);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>2);
			}
			else if((maxClk >= 4000)&&(maxClk < 8000))	//48000~65535		/8
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk>>3);
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk>>3);
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk>>3);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>3);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>3);
			}
			else if((maxClk >= 8000)&&(maxClk < 16000))	//48000~65535		/8
			{
				if(yAxisPlusNum>=1)
					allAxisSetData[1].addSubClkSet[testSetDataFlag] = yAxisClkSet(yAxisClk>>4);
				if(xAxisPlusNum>=1)
					allAxisSetData[0].addSubClkSet[testSetDataFlag] = xAxisClkSet(xAxisClk>>4);
				if(zAxisPlusNum>=1)
					allAxisSetData[2].addSubClkSet[testSetDataFlag] = zAxisClkSet(zAxisClk>>4);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>4);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>4);
			}
			else
			{
					yAxisClk=yAxisClk;
			}
			break;
		default:
			break;
	}
}


void addSubReSpeed(void)
{
	switch(addSubFlag)
	{
		case 0:
			speedReSet(addSubStepSum,addSubStepNo);
			break;
		default:
			break;
	}
}

void motor5workstart()
{
	
	if(xAxisPlusNum == 0)
		xAxisStepFlag = 2;
	else
		TIM_Cmd(TIM3, ENABLE);	//使能x
		
	if(yAxisPlusNum == 0)
		yAxisStepFlag = 2;
	else
		TIM_Cmd(TIM2, ENABLE);	//使能y 		//Tim52_Output_Pulse(yAxisPlusNum);
	
	if(zAxisPlusNum == 0)
		zAxisStepFlag = 2;
	else
		TIM_Cmd(TIM4, ENABLE);	//使能z
		
	if(aAxisPlusNum == 0)
		aAxisStepFlag = 2;
	else
		TIM_Cmd(TIM9, ENABLE);	//使能a
		
	if(bAxisPlusNum == 0)
		bAxisStepFlag = 2;
	else
		TIM_Cmd(TIM12, ENABLE);  //使能b
}

void rs232DataProcStep1(void)
{
	unsigned char i=0;

	switch(uartDataRecvProc[0])
		{
			case SELFCHECK:
				selfCheckFunc();
				//0711 test
				xAxisStepFlag = 2;
				yAxisStepFlag = 2;
				zAxisStepFlag = 2;
				aAxisStepFlag = 2;
				bAxisStepFlag = 2;
				
				break;
			case CONTROLINFO:
				controlFunc();
				break;
			case DATAINFO:
			//	dataFunc();
			//	preProcMaxNum();			//
				dataPreProcFlag = 0;		//0:已完成预处理，待第二部处理；1:等待下一帧数据的预处理；
				break;
			case STATUSINFO:
				statusFunc();
				break;
			default:
				break;
		}
}

void coderResBack(void)				// 回复编码器设定参数的函数 
{
	u8 i;
	memset(coderResString, 0, sizeof(coderResString));
	
	coderResString[0] = FrameB1;
	coderResString[1] = FrameB2;
	coderResString[2] = STATUSINFO;		// 0x0E : 状态信息
	
	coderResString[3] = 98;						// 数据长度
	coderResString[4] = 0x00;
	
	coderResString[5] = 0x42;					// 指令码
	coderResString[6] = 0x00;
	
	// 90个 allAxisSetData 的数据，小心四字节对齐!!  byYJY
	memcpy(coderResString+7, &allAxisSetData[0], sizeof(allAxisSetData[0]));
	memcpy(coderResString+37, &allAxisSetData[1], sizeof(allAxisSetData[1]));
	memcpy(coderResString+67, &allAxisSetData[2], sizeof(allAxisSetData[2]));
//	memcpy(coderResString+7, &setDataPerAxis, sizeof(setDataPerAxis));
	
	coderResString[97] = FrameEnd;				// 结束位	
	
	for(i=0;i<98;i++)
		{
			USART_SendData(USART3,coderResString[i]); 
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);	//等待发送结束
		}
}

void noAddSubSpeed(void)
{
	xAxisClkSet(xAxisClk);
	yAxisClkSet(yAxisClk);
	zAxisClkSet(zAxisClk);
}

void rs232DataProcStep2(void)
{
	unsigned char i=0;

	switch(uartDataRecvProc[0])
		{
			case DATAINFO:
				dataFunc();
				limitSportJudgFlag = LimitSportJudg();		// 限位用的

				if(limitSportJudgFlag ==1)
					return;
				
				preProcMaxNum();	
		
				xAxisStepFlag = 0;		//0614
				yAxisStepFlag = 0;
				zAxisStepFlag = 0;
				aAxisStepFlag = 0;
				bAxisStepFlag = 0;
				
				addSubStepNo = 1;		//加减速的步数计数器；
				workedTimeMS = 0;		//加减速的计时器?
				
				setedDataResFlag = 1;																// 标记运动是否停止，停止才可以串口回复
				testSetDataFlag = 0;																// 开始计数
				memset(allAxisSetData, 0, sizeof(nAxisSetData)*3);	// 初始化为0 byYJY
				memset(&setDataPerAxis, 0, sizeof(setDataPerAxis));	// 初始化为0 byYJY
				setDataPerAxis.incClkDataFlag++;										// 递增
				
				// 关闭加减速 byYJY
				
				//*************Elsie***********
				addcount=preProcStep();
				//*************Elsie***********
				
				addSubFlagSet();		//0:需要定时器配置加减速；1:不需要?
				//addSubFlag=2;
				addSubSpeed();			//频率设置；
				
//				noAddSubSpeed();
				
				pwmNumTim12 =0;			//脉冲计数清零；
				pwmNumTim9 = 0;
				pwmNumTim4 = 0;
				pwmNumTim3 = 0;
				pwmNumTim2 = 0;
				
				// 开始记录设定的运动参数														
				allAxisSetData[0].nAxisPlusNum = xAxisPlusNum;	// 各轴设定的脉冲数记录
				allAxisSetData[1].nAxisPlusNum = yAxisPlusNum;
				allAxisSetData[2].nAxisPlusNum = zAxisPlusNum;
				
				allAxisSetData[0].nAxisClk = xAxisClk;	// 各轴设定的频率记录
				allAxisSetData[1].nAxisClk = yAxisClk;
				allAxisSetData[2].nAxisClk = zAxisClk;
				
				allAxisSetData[0].addSubClkSet[8] = xAxisClk;	// 判断作用，是否在匀速状态
				allAxisSetData[1].addSubClkSet[8] = yAxisClk;
				allAxisSetData[2].addSubClkSet[8] = zAxisClk;
				
				allAxisSetData[0].nDir = xAxisDir;	// 各轴设定的运动方向 1：正向，0：负向
				allAxisSetData[1].nDir = yAxisDir;
				allAxisSetData[2].nDir = zAxisDir;
				
				testSetDataFlag++;	
				
				// 记录设定的运动参数	byYJY 
				setDataPerAxis.nAxisPlusNum = xAxisPlusNum;		// X轴
				setDataPerAxis.nAxisClk = xAxisClk;
				setDataPerAxis.nDir = xAxisDir;
				setDataPerAxis.setClk[setDataPerAxis.incClkDataFlag] = setClkTempNext;
				setDataPerAxis.setClkTime[setDataPerAxis.incClkDataFlag]++;
				
				
				motor5workstart();		//5轴的定时器启动；
				
			//	if(addSubFlag == 0)	
			//	{
					TIM7->CNT = 0;					//用于电机加减速处理，	40ms
					TIM_Cmd(TIM7,ENABLE);			//用于电机加减速处理，	40ms
			//	}
			
				while(setedDataResFlag)	;		// 运动停止后才可以返回标记 byYJY
				coderResBack();
				delay_s(1);
				
					backResString[0] = FrameB1;
					backResString[1] = FrameB2;
					backResString[2] = DATAINFO;
					
					backResString[3] = 9;
					backResString[4] = 0x00;
					
					backResString[5] = 0x22;
					backResString[6] = 0x00;
					
					backResString[7] = 0x01;
					backResString[8] = FrameEnd;
					
					// 标记为0才回复
					if(0 == respFlag)
					{
						for(i=0;i<9;i++)
							{
								USART_SendData(USART3,backResString[i]); 
								while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);	//等待发送结束
							}
					}

					dataPreProcFlag = 1;		//0:已完成预处理，待第二部处理；1:等待下一帧数据的预处理；
				break;
			default:
				break;
		}
}