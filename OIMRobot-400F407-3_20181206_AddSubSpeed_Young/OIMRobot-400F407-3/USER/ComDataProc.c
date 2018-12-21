/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          ԭ���������������
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

//////////////////////////�Լ�	/////////////////////
#define CHUANKOUZHUANGTAI 		0X01     //
#define YULIU1 					0X02     //
#define YULIU2 					0X03     //

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

extern unsigned char uartDataRecvProc[1024];
unsigned char backResString[20];
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
u8 addSubFlag=1;	//0:��Ҫ��ʱ�����üӼ��٣�1:����Ҫ��

u8 dataPreProcFlag = 1;		//0:�����Ԥ�������ڶ�������1:�ȴ���һ֡���ݵ�Ԥ����
u32 maxPlusNum=0;

extern u32 keyValue;		//5�����λ����λ������
u8 limitSportJudgFlag = 0;	// 1��λֹͣ��0������

u8 addcount=0;//��¼�Ӽ��ٵĲ����������ж��Ƿ���Ҫ�Ӽ���

u8 addtime;//����ʱ�����

u32 CountNeedAdd;//�ۼƼ�����������������

extern unsigned char respFlag;	// ����Ƿ�ظ�
extern u16 xAxisClk_Cur,yAxisClk_Cur,zAxisClk_Cur,aAxisClk_Cur,bAxisClk_Cur;

void stepMotor5AxisStop(void)
{
	TIM_Cmd(TIM3, DISABLE); 	//ʹ��x
	TIM_Cmd(TIM2, DISABLE);		//ʹ��y
	TIM_Cmd(TIM4, DISABLE); 	//ʹ��z
	TIM_Cmd(TIM9, DISABLE); 	//ʹ��a
	TIM_Cmd(TIM12, DISABLE);	//ʹ��b

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

	temp = UrgentStopTest();		//��ͣ���ؼ�� 0:���£�1:�ɿ���2:������
	
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
				while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);	//�ȴ����ͽ���
			}
	}
}

void controlFunc(void)
{
	unsigned char i=0;
	unsigned char printFlag=0;
	unsigned char commandKey;
	
		
	commandKey =  uartDataRecvProc[1]+(uartDataRecvProc[2]<<8);		//uartDataRecvProc[1];  //uartDataRecvProc[2] ����

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
		// �ϵ�	
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
		//�ѵ�	char����������0x500�ǲ���̫���ˣ� ����ָ��û���ڽӿ��ĵ��У� byYJY 2018��11��9��11:16:49
		case 0x0400:
			usingToolNo = 1;
			cleanTools();
			break;
		// ��λ�ػ�е��㣬����¼�»�����߹��ľ���
		case 0x000F:			
			#ifdef RESET_DEBUG
			while(1)
			{
				reset2Zero();
				
				// y���˶�100mm-�޼Ӽ���
//				yAxisUpNStep(100 * 400);			// 8ϸ��					
				
				// y���˶�100mm-�мӼ���
				yAxisStepFlag = 0;
				yAxisPlusNum = 1;						// ����Ϊ0
				rs232DataProcStep4();		
				while(yAxisStepFlag != 2) ;
				
				delay_s(2);				
			}
			
		// �ٴε�����ص�ѭ��
//				if(0 == resetStatusFlag)
//					reset2Zero();
//				else
//					zero2Start();
			#else
			reset2Zero();
			#endif
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
		//����	
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
		//����ok		
		case 0x0008:		//0E 09 0C 08 00 08 00 FF
			//mainMotorDecode();		//�����ֶ����룻
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
			TIM_Cmd(TIM3, DISABLE); 	//ʹ��x
			TIM_Cmd(TIM4, DISABLE); 	//ʹ��z
			TIM_Cmd(TIM9, DISABLE); 	//ʹ��a
			TIM_Cmd(TIM12, DISABLE);	//ʹ��b
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
				while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
			}
	}
}

void statusFunc(void)
{
	unsigned char i=0;
	unsigned char printFlag=0;
	unsigned char commandKey;
	
	//0E 09 0C 08 00 01 00 FF
	
	commandKey =  uartDataRecvProc[1]+(uartDataRecvProc[2]<<8);		//uartDataRecvProc[1];  //uartDataRecvProc[2] ����

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
					while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);//�ȴ����ͽ���
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
//B //0E 09 0D 2A 00 22 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 E0 2E 00 00 20 80 E2 E1 6A 0F FF			//90��

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
				
		// ����0x23�Ĵ���
			case 0x23:			// �鷢�Ͳ����õ�
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
	if(keyValue != 0)	//����λ
	{
	
		backResString[0] = FrameB1;
		backResString[1] = FrameB2;
		backResString[2] = STATUSINFO;
		
		backResString[3] = 9;
		backResString[4] = 0x00;
		
		backResString[8] = FrameEnd;
	
			if(keyValue&0x0001) //KEYXa_PRES
			{
				if(xAxisDir==1)		//������
					{
						stepMotor5AxisStop();
						
						backResString[5] = 0x31;
						backResString[6] = 0x00;
						backResString[7] = 0x01;
						
						for(i=0;i<9;i++)
							{
								USART_SendData(USART3,backResString[i]); 
								while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//�ȴ����ͽ���
							}
						return 1;
					}
			}
			if((keyValue>>1)&0x0001)	//KEYXb_PRES
			{
				if(xAxisDir==0)		//������
					{
						stepMotor5AxisStop();
						
						backResString[5] = 0x31;
						backResString[6] = 0x00;
						backResString[7] = 0x02;
						
						for(i=0;i<9;i++)
							{
								USART_SendData(USART3,backResString[i]); 
								while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//�ȴ����ͽ���
							}
						return 1;
					}
			}
			if((keyValue>>2)&0x0001)	//KEYYa_PRES
			{
				if(yAxisDir==1)		//������
					{
						stepMotor5AxisStop();
						
						backResString[5] = 0x32;
						backResString[6] = 0x00;
						backResString[7] = 0x01;
						
						for(i=0;i<9;i++)
							{
								USART_SendData(USART3,backResString[i]); 
								while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//�ȴ����ͽ���
							}
						return 1;
					}
			}
		
			if((keyValue>>3)&0x0001)	//KEYYb_PRES
			{
				if(yAxisDir==0)		//������
					{
						stepMotor5AxisStop();
						
						backResString[5] = 0x32;
						backResString[6] = 0x00;
						backResString[7] = 0x02;
						
						for(i=0;i<9;i++)
							{
								USART_SendData(USART3,backResString[i]); 
								while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//�ȴ����ͽ���
							}
						return 1;
					}
			}
		
			if((keyValue>>4)&0x0001)	//KEYZa_PRES
			{
				if(zAxisDir==1)		//������
					{
						stepMotor5AxisStop();
						
						backResString[5] = 0x33;
						backResString[6] = 0x00;
						backResString[7] = 0x01;
						
						for(i=0;i<9;i++)
							{
								USART_SendData(USART3,backResString[i]); 
								while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//�ȴ����ͽ���
							}
						return 1;
					}
			}
		
			if((keyValue>>5)&0x0001)	//KEYZb_PRES
			{
				if(zAxisDir==0)		//������
					{
						stepMotor5AxisStop();
						
						backResString[5] = 0x33;
						backResString[6] = 0x00;
						backResString[7] = 0x02;
						
						for(i=0;i<9;i++)
							{
								USART_SendData(USART3,backResString[i]); 
								while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//�ȴ����ͽ���
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

u32 preProcMinPlusNum(void)		//��ò�Ϊ0����Сֵ��
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
	
	return time;			//�������ʱ�䣬��λms
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
		addStep = 4;		//ֱ�ӱ�Ƶ����
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
//		addStep = 4;		//ֱ�ӱ�Ƶ����
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
				yAxisClkSet(yAxisClk);
			if(xAxisPlusNum>=1)
				xAxisClkSet(xAxisClk);
			if(zAxisPlusNum>=1)
				zAxisClkSet(zAxisClk);
			if(aAxisPlusNum>=1)
				aAxisClkSet(aAxisClk);
			if(bAxisPlusNum>=1)
				bAxisClkSet(bAxisClk);
			break;
		case 2:		//stepNo = 1;2;		//2000,4000
					//2000~4000
					//1000~2000	2000~4000
			if(yAxisPlusNum>=1)
				yAxisClkSet(yAxisClk>>(2-stepNo));
			if(xAxisPlusNum>=1)
				xAxisClkSet(xAxisClk>>(2-stepNo));
			if(zAxisPlusNum>=1)
				zAxisClkSet(zAxisClk>>(2-stepNo));
			if(aAxisPlusNum>=1)
				aAxisClkSet(aAxisClk>>(2-stepNo));
			if(bAxisPlusNum>=1)
				bAxisClkSet(bAxisClk>>(2-stepNo));
			break;
		case 3:		//stepNo = 1;2;3;		//2000,4000,8000
					//4000~8000
					//1000~2000	2000~4000	4000~8000
			if(yAxisPlusNum>=1)
				yAxisClkSet(yAxisClk>>(3-stepNo));
			if(xAxisPlusNum>=1)
				xAxisClkSet(xAxisClk>>(3-stepNo));
			if(zAxisPlusNum>=1)
				zAxisClkSet(zAxisClk>>(3-stepNo));
			if(aAxisPlusNum>=1)
				aAxisClkSet(aAxisClk>>(3-stepNo));
			if(bAxisPlusNum>=1)
				bAxisClkSet(bAxisClk>>(3-stepNo));
			break;
		case 4:		//stepNo = 1;2;3;4;	//���ٲ���
					//8000~12000
					//1000~1500	2000~3000	4000~6000	8000~12000
			if(yAxisPlusNum>=1)
				yAxisClkSet(yAxisClk>>(4-stepNo));
			if(xAxisPlusNum>=1)
				xAxisClkSet(xAxisClk>>(4-stepNo));
			if(zAxisPlusNum>=1)
				zAxisClkSet(zAxisClk>>(4-stepNo));
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
					yAxisClkSet(yAxisClk>>(4-stepNo));
				if(xAxisPlusNum>=1)
					xAxisClkSet(xAxisClk>>(4-stepNo));
				if(zAxisPlusNum>=1)
					zAxisClkSet(zAxisClk>>(4-stepNo));
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>(4-stepNo));
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>(4-stepNo));
			}
			else if(stepNo == 4)
			{
				if(yAxisPlusNum>=1)
					yAxisClkSet((yAxisClk>>1)+(yAxisClk>>2)+(yAxisClk>>3));
				if(xAxisPlusNum>=1)
					xAxisClkSet((xAxisClk>>1)+(xAxisClk>>2)+(xAxisClk>>3));
				if(zAxisPlusNum>=1)
					zAxisClkSet((zAxisClk>>1)+(zAxisClk>>2)+(zAxisClk>>3));
				if(aAxisPlusNum>=1)
					aAxisClkSet((aAxisClk>>1)+(aAxisClk>>2)+(aAxisClk>>3));
				if(bAxisPlusNum>=1)
					bAxisClkSet((bAxisClk>>1)+(bAxisClk>>2)+(bAxisClk>>3));
			}
			else if(stepNo == 5)
			{
				if(yAxisPlusNum>=1)
					yAxisClkSet(yAxisClk);
				if(xAxisPlusNum>=1)
					xAxisClkSet(xAxisClk);
				if(zAxisPlusNum>=1)
					zAxisClkSet(zAxisClk);
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
					yAxisClkSet(yAxisClk>>(4-stepNo));
				if(xAxisPlusNum>=1)
					xAxisClkSet(xAxisClk>>(4-stepNo));
				if(zAxisPlusNum>=1)
					zAxisClkSet(zAxisClk>>(4-stepNo));
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>(4-stepNo));
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>(4-stepNo));
			}
			else if(stepNo == 4)
			{
				if(yAxisPlusNum>=1)
					yAxisClkSet((yAxisClk>>1)+(yAxisClk>>2));
				if(xAxisPlusNum>=1)
					xAxisClkSet((xAxisClk>>1)+(xAxisClk>>2));
				if(zAxisPlusNum>=1)
					zAxisClkSet((zAxisClk>>1)+(zAxisClk>>2));
				if(aAxisPlusNum>=1)
					aAxisClkSet((aAxisClk>>1)+(aAxisClk>>2));
				if(bAxisPlusNum>=1)
					bAxisClkSet((bAxisClk>>1)+(bAxisClk>>2));
			}
			else if(stepNo == 5)
			{
				if(yAxisPlusNum>=1)
					yAxisClkSet((yAxisClk>>1)+(yAxisClk>>2)+(yAxisClk>>3));
				if(xAxisPlusNum>=1)
					xAxisClkSet((xAxisClk>>1)+(xAxisClk>>2)+(xAxisClk>>3));
				if(zAxisPlusNum>=1)
					zAxisClkSet((zAxisClk>>1)+(zAxisClk>>2)+(zAxisClk>>3));
				if(aAxisPlusNum>=1)
					aAxisClkSet((aAxisClk>>1)+(aAxisClk>>2)+(aAxisClk>>3));
				if(bAxisPlusNum>=1)
					bAxisClkSet((bAxisClk>>1)+(bAxisClk>>2)+(bAxisClk>>3));
			}
			else if(stepNo == 6)
			{
				if(yAxisPlusNum>=1)
					yAxisClkSet(yAxisClk);
				if(xAxisPlusNum>=1)
					xAxisClkSet(xAxisClk);
				if(zAxisPlusNum>=1)
					zAxisClkSet(zAxisClk);
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
					yAxisClkSet(yAxisClk>>(4-stepNo));
				if(xAxisPlusNum>=1)
					xAxisClkSet(xAxisClk>>(4-stepNo));
				if(zAxisPlusNum>=1)
					zAxisClkSet(zAxisClk>>(4-stepNo));
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>(4-stepNo));
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>(4-stepNo));
			}
			else if(stepNo == 4)
			{
				if(yAxisPlusNum>=1)
					yAxisClkSet((yAxisClk>>1)+(yAxisClk>>2));
				if(xAxisPlusNum>=1)
					xAxisClkSet((xAxisClk>>1)+(xAxisClk>>2));
				if(zAxisPlusNum>=1)
					zAxisClkSet((zAxisClk>>1)+(zAxisClk>>2));
				if(aAxisPlusNum>=1)
					aAxisClkSet((aAxisClk>>1)+(aAxisClk>>2));
				if(bAxisPlusNum>=1)
					bAxisClkSet((bAxisClk>>1)+(bAxisClk>>2));
			}
			else if(stepNo == 5)
			{
				if(yAxisPlusNum>=1)
					yAxisClkSet((yAxisClk>>1)+(yAxisClk>>2)+(yAxisClk>>3));
				if(xAxisPlusNum>=1)
					xAxisClkSet((xAxisClk>>1)+(xAxisClk>>2)+(xAxisClk>>3));
				if(zAxisPlusNum>=1)
					zAxisClkSet((zAxisClk>>1)+(zAxisClk>>2)+(zAxisClk>>3));
				if(aAxisPlusNum>=1)
					aAxisClkSet((aAxisClk>>1)+(aAxisClk>>2)+(aAxisClk>>3));
				if(bAxisPlusNum>=1)
					bAxisClkSet((bAxisClk>>1)+(bAxisClk>>2)+(bAxisClk>>3));
			}
			else if(stepNo == 6)
			{
				if(yAxisPlusNum>=1)
					yAxisClkSet((yAxisClk>>1)+(yAxisClk>>2)+(yAxisClk>>3)+(yAxisClk>>4));
				if(xAxisPlusNum>=1)
					xAxisClkSet((xAxisClk>>1)+(xAxisClk>>2)+(xAxisClk>>3)+(xAxisClk>>4));
				if(zAxisPlusNum>=1)
					zAxisClkSet((zAxisClk>>1)+(zAxisClk>>2)+(zAxisClk>>3)+(zAxisClk>>4));
				if(aAxisPlusNum>=1)
					aAxisClkSet((aAxisClk>>1)+(aAxisClk>>2)+(aAxisClk>>3)+(aAxisClk>>4));
				if(bAxisPlusNum>=1)
					bAxisClkSet((bAxisClk>>1)+(bAxisClk>>2)+(bAxisClk>>3)+(bAxisClk>>4));
			}
			
			else if(stepNo == 7)
			{
				if(yAxisPlusNum>=1)
					yAxisClkSet(yAxisClk);
				if(xAxisPlusNum>=1)
					xAxisClkSet(xAxisClk);
				if(zAxisPlusNum>=1)
					zAxisClkSet(zAxisClk);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk);
			}
			break;
		default:
			break;
	}
//	switch(stepSum)
//		{
//		case 1:
//			if(yAxisPlusNum>=1)
//				yAxisClkSet(yAxisClk);
//			if(xAxisPlusNum>=1)
//				xAxisClkSet(xAxisClk);
//			if(zAxisPlusNum>=1)
//				zAxisClkSet(zAxisClk);
//			if(aAxisPlusNum>=1)
//				aAxisClkSet(aAxisClk);
//			if(bAxisPlusNum>=1)
//				bAxisClkSet(bAxisClk);
//			break;
//		case 2:		
//			if(yAxisPlusNum>=1)
//				yAxisClkSet(yAxisClk>>(2-stepNo));
//			if(xAxisPlusNum>=1)
//				xAxisClkSet(xAxisClk>>(2-stepNo));
//			if(zAxisPlusNum>=1)
//				zAxisClkSet(zAxisClk>>(2-stepNo));
//			if(aAxisPlusNum>=1)
//				aAxisClkSet(aAxisClk>>(2-stepNo));
//			if(bAxisPlusNum>=1)
//				bAxisClkSet(bAxisClk>>(2-stepNo));
//			break;
//		case 3:		
//			if(yAxisPlusNum>=1)
//				yAxisClkSet(yAxisClk>>(3-stepNo));
//			if(xAxisPlusNum>=1)
//				xAxisClkSet(xAxisClk>>(3-stepNo));
//			if(zAxisPlusNum>=1)
//				zAxisClkSet(zAxisClk>>(3-stepNo));
//			if(aAxisPlusNum>=1)
//				aAxisClkSet(aAxisClk>>(3-stepNo));
//			if(bAxisPlusNum>=1)
//				bAxisClkSet(bAxisClk>>(3-stepNo));
//			break;
//		case 4:		
//			if(yAxisPlusNum>=1)
//				yAxisClkSet(yAxisClk>>(4-stepNo));
//			if(xAxisPlusNum>=1)
//				xAxisClkSet(xAxisClk>>(4-stepNo));
//			if(zAxisPlusNum>=1)
//				zAxisClkSet(zAxisClk>>(4-stepNo));
//			if(aAxisPlusNum>=1)
//				aAxisClkSet(aAxisClk>>(4-stepNo));
//			if(bAxisPlusNum>=1)
//				bAxisClkSet(bAxisClk>>(4-stepNo));
//			break;
//		case 5:		
//			if(yAxisPlusNum>=1)
//				yAxisClkSet(yAxisClk>>(5-stepNo));
//			if(xAxisPlusNum>=1)
//				xAxisClkSet(xAxisClk>>(5-stepNo));
//			if(zAxisPlusNum>=1)
//				zAxisClkSet(zAxisClk>>(5-stepNo));
//			if(aAxisPlusNum>=1)
//				aAxisClkSet(aAxisClk>>(5-stepNo));
//			if(bAxisPlusNum>=1)
//				bAxisClkSet(bAxisClk>>(5-stepNo));
//			break;
//		case 6:		
//			if(yAxisPlusNum>=1)
//				yAxisClkSet(yAxisClk>>(6-stepNo));
//			if(xAxisPlusNum>=1)
//				xAxisClkSet(xAxisClk>>(6-stepNo));
//			if(zAxisPlusNum>=1)
//			{
//				send_zAxisClk=zAxisClk>>(6-stepNo);
//				if(stepNo==2||stepNo==3||stepNo==4||stepNo==5)
//				{
//					send_zAxisClk=send_zAxisClk;
//				}
//				zAxisClkSet(send_zAxisClk);
//			}
//				
//			if(aAxisPlusNum>=1)
//				aAxisClkSet(aAxisClk>>(6-stepNo));
//			if(bAxisPlusNum>=1)
//				bAxisClkSet(bAxisClk>>(6-stepNo));
//			break;
//		case 7:		
//			if(yAxisPlusNum>=1)
//				yAxisClkSet(yAxisClk>>(7-stepNo));
//			if(xAxisPlusNum>=1)
//				xAxisClkSet(xAxisClk>>(7-stepNo));
//			if(zAxisPlusNum>=1)
//				zAxisClkSet(zAxisClk>>(7-stepNo));
//			if(aAxisPlusNum>=1)
//				aAxisClkSet(aAxisClk>>(7-stepNo));
//			if(bAxisPlusNum>=1)
//				bAxisClkSet(bAxisClk>>(7-stepNo));
//			break;
//			case 8:		
//			if(yAxisPlusNum>=1)
//				yAxisClkSet(yAxisClk>>(8-stepNo));
//			if(xAxisPlusNum>=1)
//				xAxisClkSet(xAxisClk>>(8-stepNo));
//			if(zAxisPlusNum>=1)
//				zAxisClkSet(zAxisClk>>(8-stepNo));
//			if(aAxisPlusNum>=1)
//				aAxisClkSet(aAxisClk>>(8-stepNo));
//			if(bAxisPlusNum>=1)
//				bAxisClkSet(bAxisClk>>(8-stepNo));
//			break;
//			case 9:		
//			if(yAxisPlusNum>=1)
//				yAxisClkSet(yAxisClk>>(9-stepNo));
//			if(xAxisPlusNum>=1)
//				xAxisClkSet(xAxisClk>>(9-stepNo));
//			if(zAxisPlusNum>=1)
//				zAxisClkSet(zAxisClk>>(9-stepNo));
//			if(aAxisPlusNum>=1)
//				aAxisClkSet(aAxisClk>>(9-stepNo));
//			if(bAxisPlusNum>=1)
//				bAxisClkSet(bAxisClk>>(9-stepNo));
//			break;
//		default:
//			break;
//		}	
}

void speedReSet(u8 stepSum,u8 stepNo)
{
	
	TIM_Cmd(TIM3, DISABLE); 	//ʹ��x
	TIM_Cmd(TIM4, DISABLE); 	//ʹ��z
	TIM_Cmd(TIM9, DISABLE); 	//ʹ��a
	TIM_Cmd(TIM12, DISABLE);	//ʹ��b
//	TIM_Cmd(TIM5, DISABLE);		//�����ģ�
	TIM_Cmd(TIM2, DISABLE);		//ʹ��y,���������

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
			addSubFlag = 0;			//0:��Ҫ��ʱ�����üӼ��٣�1:����Ҫ�
	}
	else
	{
			addSubFlag = 1;
	}
	
	
//	u32 minPlus;
//	
//	workTimeMSSum = preProcTime();
//	minPlus = preProcMinPlusNum();
//	
//	if((workTimeMSSum>100)&&(minPlus>25))		//20*8 =160ms
//	{
//		addSubFlag = 0;			//0:��Ҫ��ʱ�����üӼ��٣�1:����Ҫ��
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
//			addSubFlag = 0;			//0:��Ҫ��ʱ�����üӼ��٣�1:����Ҫ�
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
			addSubStepSum = preProcStep();		//��üӼ��ٲ���
			speedSet(addSubStepSum,addSubStepNo);
			break;
		case 1:
			if(maxClk < 12000)
			{
				if(yAxisPlusNum>=1)
					yAxisClkSet(yAxisClk);
				if(xAxisPlusNum>=1)
					xAxisClkSet(xAxisClk);
				if(zAxisPlusNum>=1)
					zAxisClkSet(zAxisClk);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk);
			}
			else if((maxClk >= 12000)&&(maxClk < 24000))
			{
				if(yAxisPlusNum>=1)
					yAxisClkSet(yAxisClk>>1);
				if(xAxisPlusNum>=1)
					xAxisClkSet(xAxisClk>>1);
				if(zAxisPlusNum>=1)
					zAxisClkSet(zAxisClk>>1);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>1);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>1);
			}
			else if((maxClk >= 24000)&&(maxClk < 48000))
			{
				if(yAxisPlusNum>=1)
					yAxisClkSet(yAxisClk>>2);
				if(xAxisPlusNum>=1)
					xAxisClkSet(xAxisClk>>2);
				if(zAxisPlusNum>=1)
					zAxisClkSet(zAxisClk>>2);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>2);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>2);
			}
			else		//48000~65535		/8
			{
				if(yAxisPlusNum>=1)
					yAxisClkSet(yAxisClk>>3);
				if(xAxisPlusNum>=1)
					xAxisClkSet(xAxisClk>>3);
				if(zAxisPlusNum>=1)
					zAxisClkSet(zAxisClk>>3);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>3);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>3);
			}
			break;
		case 2:
			if(yAxisPlusNum>=1)
					yAxisClkSet(yAxisClk);
				if(xAxisPlusNum>=1)
					xAxisClkSet(xAxisClk);
				if(zAxisPlusNum>=1)
					zAxisClkSet(zAxisClk);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk);
				break;
		case 3:
//			setFre=500;
//			if(yAxisPlusNum>=1 && maxPlusNum==yAxisPlusNum)
//			{
//				yAxisClkSet(setFre);
//				if(xAxisPlusNum>=1)
//					xAxisClkSet(xAxisClk*(setFre/yAxisClk));
//				if(zAxisPlusNum>=1)
//					zAxisClkSet(zAxisClk*(setFre/yAxisClk));
//				if(aAxisPlusNum>=1)
//					aAxisClkSet(aAxisClk*(setFre/yAxisClk));
//				if(bAxisPlusNum>=1)
//					bAxisClkSet(bAxisClk*(setFre/yAxisClk));
//			}
//			else if(xAxisPlusNum>=1 && maxPlusNum==xAxisPlusNum)
//			{
//				xAxisClkSet(setFre);
//				if(yAxisPlusNum>=1)
//					yAxisClkSet(yAxisClk*(setFre/xAxisClk));
//				if(zAxisPlusNum>=1)
//					zAxisClkSet(zAxisClk*(setFre/xAxisClk));
//				if(aAxisPlusNum>=1)
//					aAxisClkSet(aAxisClk*(setFre/xAxisClk));
//				if(bAxisPlusNum>=1)
//					bAxisClkSet(bAxisClk*(setFre/xAxisClk));
//			}
//			else if(zAxisPlusNum>=1 && maxPlusNum==zAxisPlusNum)
//			{
//				zAxisClkSet(setFre);
//				if(yAxisPlusNum>=1)
//					yAxisClkSet(yAxisClk*(setFre/zAxisClk));
//				if(xAxisPlusNum>=1)
//					xAxisClkSet(xAxisClk*(setFre/zAxisClk));
//				if(aAxisPlusNum>=1)
//					aAxisClkSet(aAxisClk*(setFre/zAxisClk));
//				if(bAxisPlusNum>=1)
//					bAxisClkSet(bAxisClk*(setFre/zAxisClk));
//			}
//			break;
			if(maxClk < 1000)
			{
				if(yAxisPlusNum>=1)
					yAxisClkSet(yAxisClk);
				if(xAxisPlusNum>=1)
					xAxisClkSet(xAxisClk);
				if(zAxisPlusNum>=1)
					zAxisClkSet(zAxisClk);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk);
			}
			else if((maxClk >= 1000)&&(maxClk < 2000))
			{
				if(yAxisPlusNum>=1)
					yAxisClkSet(yAxisClk>>1);
				if(xAxisPlusNum>=1)
					xAxisClkSet(xAxisClk>>1);
				if(zAxisPlusNum>=1)
					zAxisClkSet(zAxisClk>>1);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>1);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>1);
			}
			else if((maxClk >= 2000)&&(maxClk < 4000))
			{
				if(yAxisPlusNum>=1)
					yAxisClkSet(yAxisClk>>2);
				if(xAxisPlusNum>=1)
					xAxisClkSet(xAxisClk>>2);
				if(zAxisPlusNum>=1)
					zAxisClkSet(zAxisClk>>2);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>2);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>2);
			}
			else if((maxClk >= 4000)&&(maxClk < 8000))	//48000~65535		/8
			{
				if(yAxisPlusNum>=1)
					yAxisClkSet(yAxisClk>>3);
				if(xAxisPlusNum>=1)
					xAxisClkSet(xAxisClk>>3);
				if(zAxisPlusNum>=1)
					zAxisClkSet(zAxisClk>>3);
				if(aAxisPlusNum>=1)
					aAxisClkSet(aAxisClk>>3);
				if(bAxisPlusNum>=1)
					bAxisClkSet(bAxisClk>>3);
			}
			else if((maxClk >= 8000)&&(maxClk < 16000))	//48000~65535		/8
			{
				if(yAxisPlusNum>=1)
					yAxisClkSet(yAxisClk>>4);
				if(xAxisPlusNum>=1)
					xAxisClkSet(xAxisClk>>4);
				if(zAxisPlusNum>=1)
					zAxisClkSet(zAxisClk>>4);
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
	
	if((xAxisPlusNum == 0) || (xAxisClk == 0))
		xAxisStepFlag = 2;
	else
		TIM_Cmd(TIM3, ENABLE);	//ʹ��x
		
	if((yAxisPlusNum == 0) || (yAxisClk == 0))
		yAxisStepFlag = 2;
	else
		TIM_Cmd(TIM2, ENABLE);	//ʹ��y 		//Tim52_Output_Pulse(yAxisPlusNum);
	
	if((zAxisPlusNum == 0) || (zAxisClk == 0))
		zAxisStepFlag = 2;
	else
		TIM_Cmd(TIM4, ENABLE);	//ʹ��z
		
	if((aAxisPlusNum == 0) || (aAxisClk == 0))
		aAxisStepFlag = 2;
	else
		TIM_Cmd(TIM9, ENABLE);	//ʹ��a
		
	if((bAxisPlusNum == 0) || (bAxisClk == 0))
		bAxisStepFlag = 2;
	else
		TIM_Cmd(TIM12, ENABLE);  //ʹ��b
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
				dataPreProcFlag = 0;		//0:�����Ԥ�������ڶ�������1:�ȴ���һ֡���ݵ�Ԥ����
				break;
			case STATUSINFO:
				statusFunc();
				break;
			default:
				break;
		}
}

void rs232DataProcStep2(void)
{
	unsigned char i=0;

	switch(uartDataRecvProc[0])
		{
			case DATAINFO:
				dataFunc();
				limitSportJudgFlag = LimitSportJudg();		// ��λ�õ�

				if(limitSportJudgFlag ==1)
					return;
				
				preProcMaxNum();	
		
				xAxisStepFlag = 0;		//0614
				yAxisStepFlag = 0;
				zAxisStepFlag = 0;
				aAxisStepFlag = 0;
				bAxisStepFlag = 0;
				
				addSubStepNo = 1;		//�Ӽ��ٵĲ�����������
				workedTimeMS = 0;		//�Ӽ��ٵļ�ʱ���
				
				// ���Ӽ���
//				noAddSubSpeed();
				
				// б��ʽ�Ӽ���  byYJY
				addSubSpeed_Cur(0, 0);
				
//				pwmNumTim12 =0;			//����������㣻
//				pwmNumTim9 = 0;
//				pwmNumTim4 = 0;
//				pwmNumTim3 = 0;
//				pwmNumTim2 = 0;
//					
//				motor5workstart();		//5��Ķ�ʱ��������
//				
//				TIM7->CNT = 0;					//���ڵ���Ӽ��ٴ���	40ms
//				TIM_Cmd(TIM7,ENABLE);			//���ڵ���Ӽ��ٴ���	40ms
			
//				backResString[0] = FrameB1;
//				backResString[1] = FrameB2;
//				backResString[2] = DATAINFO;
//				
//				backResString[3] = 9;
//				backResString[4] = 0x00;
//				
//				backResString[5] = 0x22;
//				backResString[6] = 0x00;
//				
//				backResString[7] = 0x01;
//				backResString[8] = FrameEnd;
//				
//				// ���Ϊ0�Żظ�
//				if(0 == respFlag)
//				{
//					for(i=0;i<9;i++)
//						{
//							USART_SendData(USART3,backResString[i]); 
//							while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);	//�ȴ����ͽ���
//						}
//				}

				dataPreProcFlag = 1;		//0:�����Ԥ�������ڶ�������1:�ȴ���һ֡���ݵ�Ԥ����
			break;
		default:
			break;
		}
}


// ���޲����ظ���λ����ʱ����
void rs232DataProcStep4(void)
{
	unsigned char i=0;
	// ��ʼ��
//	xAxisClk = X_AXIS_RST_CLK;				// for test
//	yAxisClk = 0;	
	xAxisClk = 0;
	yAxisClk = Y_AXIS_RST_CLK;
	zAxisClk = 0;
	aAxisClk = 0;
	bAxisClk = 0;
	
//	xAxisPlusNum = 20 * 400;				// for test
//	yAxisPlusNum = 0;	
	xAxisPlusNum = 0;
	yAxisPlusNum = 100 * 400;			// 100mm
	zAxisPlusNum = 0;
	aAxisPlusNum = 0;
	bAxisPlusNum = 0;
	
//	xAxisDIR_ON;								// for test	
	yAxisDIR_OFF;
	
	preProcMaxNum();	

	xAxisStepFlag = 0;		//0614
	yAxisStepFlag = 0;
	zAxisStepFlag = 0;
	aAxisStepFlag = 0;
	bAxisStepFlag = 0;
	
	addSubStepNo = 1;		//�Ӽ��ٵĲ�����������
	workedTimeMS = 0;		//�Ӽ��ٵļ�ʱ���

	// ����PWMƵ�ʵļӼ���  byYJY
	addSubSpeed_Cur(0, 0);
	
	pwmNumTim12 =0;			//����������㣻
	pwmNumTim9 = 0;
	pwmNumTim4 = 0;
	pwmNumTim3 = 0;
	pwmNumTim2 = 0;
		
	motor5workstart();		//5��Ķ�ʱ��������
	
	TIM7->CNT = 0;					//���ڵ���Ӽ��ٴ���	40ms
	TIM_Cmd(TIM7,ENABLE);			//���ڵ���Ӽ��ٴ���	40ms

//	dataPreProcFlag = 1;		//0:�����Ԥ�������ڶ�������1:�ȴ���һ֡���ݵ�Ԥ����
}
