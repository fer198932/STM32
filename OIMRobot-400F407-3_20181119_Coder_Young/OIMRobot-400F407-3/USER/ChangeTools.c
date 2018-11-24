/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          ԭ���������������
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : ChangeTools.C
* By  : Hiker Lee
* 2018-05-07
* ��д�����߼�����Ϊ����ϵ���ˣ�20180601
* �ϵ������޸ģ����������20180720
* ���ӵ����������Ĺ��� byYJY 2018��11��12��14:13:00
********************************************************************************************************/
#include "ChangeTools.h"

#define zZeroFlag GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_10)				//0:��λ��1:δ��λ��
#define yZeroFlag GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_7)
#define xZeroFlag GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_5)

#define toolY1Flag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_0)
#define toolY2Flag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_1)
#define toolY3Flag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_2)				// û���ˣ�1��4ʱ�õ�
#define toolY4Flag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_3)				// û���ˣ�1��4ʱ�õ�

#define toolX1Flag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_4)
#define toolX2Flag GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11)

#define toolZFlag GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_13)				// ��ΪPF13 byYJY

#define mainMoterToolsFlag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_7)		//0:�е���	1:�޵���	
#define unClampFlag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_8)			//0:�ɿ���λ��	1:�ɿ�δ��λ��
#define clampFlag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_10)				//0:�н���λ��	1:�н�δ��λ��

#define sheelOpenCloseFlag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_9)		//0:��Ǵ򿪣�	1:��ǹرգ�

u8 targeToolNo=0;
u8 usingToolNo=0;

extern u8 xAxisStepFlag;
extern u8 yAxisStepFlag;
extern u8 zAxisStepFlag; 
extern unsigned char backResString[20];

extern u32 pwmNumTim2;
extern u32 pwmNumTim3;
extern u32 pwmNumTim4;
extern u32 xAxisPlusNum,yAxisPlusNum,zAxisPlusNum,aAxisPlusNum,bAxisPlusNum;

// ���ñ�־λ
u8 stepsFlag[5] = {0, 0, 0, 0, 0}; 				// ��X��Y��Z��A��B�ļ��� 1���򿪣� 0���ر�

ChangeToolsSavedSteps changeToolsSavedSteps;	// ����λ�����ݳ�ʼ��
ChangeToolsSavedSteps stepsTemp;				// ��ʱ�����߹��Ĳ���
// ������λ�����͵����ݣ��Ե���λ�õȽ��г�ʼ��
void savedStepsInit(void)
{
	// ��ʼ������������λ�ù�ϵ
	changeToolsSavedSteps.xSteps[0] = 0;
	changeToolsSavedSteps.ySteps[0] = 0;
	changeToolsSavedSteps.zSteps[0] = 0;
	
	// ��ʼ�����������֮���λ�ù�ϵ������λ����������ʱд���̶�ֵ
	changeToolsSavedSteps.xSteps[1] = 0xf5ab;//0xf555;//0xf548;	 // ����1��λ��
	changeToolsSavedSteps.ySteps[1] = 0x276a;//0x254e;//0x2540;
	changeToolsSavedSteps.zSteps[1] = 0x7327;//0x73ac;//0x3f2f;
	
	changeToolsSavedSteps.xSteps[2] = 10000;	// ����2��λ��
	changeToolsSavedSteps.ySteps[2] = 10000;
	changeToolsSavedSteps.zSteps[2] = 10000;
	
	changeToolsSavedSteps.xSteps[3] = 10000;	// ����3��λ��
	changeToolsSavedSteps.ySteps[3] = 10000;
	changeToolsSavedSteps.zSteps[3] = 10000;
	
	changeToolsSavedSteps.xSteps[4] = 10000;	// ����4��λ��
	changeToolsSavedSteps.ySteps[4] = 10000;
	changeToolsSavedSteps.zSteps[4] = 10000;
	
}

void mainToolFlagIOInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  //����ʹ��λ�ô�������
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);		//ʹ��GPIOGʱ��
 
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;	  				//��1-4���ѵ������λ
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;			  	//����
  GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;			  	//��������
  GPIO_Init(GPIOG,&GPIO_InitStructure); 				  	//��ʼ��GPIOG
} 


void delay_s(u8 delay)
{
	u8 i;
	for(i=0;i<delay;i++)
	{
		delay_ms(1000);
	}
}

void axis3Back2ORG(void)
{
	//	Z�����
	// zAxisDIR_ON;				//z���㷽��
	zAxisDIR_OFF;					// ������
	disableZaxisCountINT(); 	//��ֹz������жϣ�
	zAxisClkSet(8000);			//z����Ƶ���趨��
	TIM_Cmd(TIM4, ENABLE);		//ʹ��z���㶯����	
	while(zZeroFlag==1);		//������ɣ�
	TIM_Cmd(TIM4, DISABLE); 	//������ɣ�PWMֹͣ��
	enableZaxisCountINT();		//����z������жϣ�
	delay_s(2);

	//	Y�����
	yAxisDIR_OFF;				//y���㷽��
	disableYaxisCountINT(); 	//��ֹy������жϣ�
	yAxisClkSet(8000);			//y����Ƶ���趨��
	TIM_Cmd(TIM2, ENABLE);		//ʹ��y���㶯����	
//	while(yZeroFlag==1);		//������ɣ�
	while(toolY1Flag==1);			// ������
	TIM_Cmd(TIM2, DISABLE); 	//������ɣ�PWMֹͣ��
	enableYaxisCountINT();		//����y������жϣ�
	delay_s(2);
	
	//	X�����
	xAxisDIR_ON;				//x���㷽��   //xAxisDIR_ON
	disableXaxisCountINT(); 	//��ֹx������жϣ�
	xAxisClkSet(8000);			//x����Ƶ���趨��
	TIM_Cmd(TIM3, ENABLE);		//ʹ��x���㶯����
//	while(xZeroFlag==1);		//������ɣ�
	while(toolX2Flag==1);			// ������
	TIM_Cmd(TIM3, DISABLE); 	//������ɣ�PWMֹͣ�
	enableXaxisCountINT();		//����x������жϣ�	
	delay_s(2);
}

// �ܼƲ����Ļص�ԭ��
void axis3Back2ORGWithCnt(void)
{
	//	Z�����
	zAxisDIR_OFF;				//z���㷽��
	disableZaxisCountINT(); 	//��ֹz������жϣ�
	zAxisClkSet(8000);			//z����Ƶ���趨��
	// ��ʼ����������
	stepsFlag[2] = 1;					// ���ñ�־λ��z�Ὺʼ���� 1:������ 0��ֹͣ����
	TIM_Cmd(TIM14,ENABLE);		// �򿪼���
	TIM_Cmd(TIM4, ENABLE);		//ʹ��z���㶯����
	
	while(zZeroFlag==1);		//������ɣ�
	
	TIM_Cmd(TIM4, DISABLE); 	//������ɣ�PWMֹͣ��
	TIM_Cmd(TIM14,DISABLE);		
	stepsFlag[2] = 0;					//  �رռ���
	enableZaxisCountINT();		//����z������жϣ�
	delay_s(2);

	//	Y�����
	yAxisDIR_OFF;				//y���㷽��
	disableYaxisCountINT(); 	//��ֹy������жϣ�
	yAxisClkSet(8000);			//y����Ƶ���趨��
	// ��ʼ����������
	stepsFlag[1] = 1;					// ���ñ�־λ��y�Ὺʼ���� 1:������ 0��ֹͣ����
	TIM_Cmd(TIM14,ENABLE);		// �򿪼���
	TIM_Cmd(TIM2, ENABLE);		//ʹ��y���㶯����
	
//	while(yZeroFlag==1);		//������ɣ�
	while(toolY1Flag==1);			// ������
	
	TIM_Cmd(TIM2, DISABLE); 	//������ɣ�PWMֹͣ��
	TIM_Cmd(TIM14,DISABLE);
	stepsFlag[1] = 0;
	enableYaxisCountINT();		//����y������жϣ�
	delay_s(2);
	
	//	X�����
	xAxisDIR_ON;				//x���㷽�� 
	disableXaxisCountINT(); 	//��ֹx������жϣ�
	xAxisClkSet(8000);			//x����Ƶ���趨��
	// ��ʼ����������
	stepsFlag[0] = 1;					// ���ñ�־λ��x�Ὺʼ���� 1:������ 0��ֹͣ����
	TIM_Cmd(TIM14,ENABLE);		// �������
	TIM_Cmd(TIM3, ENABLE);		//ʹ��x���㶯����
	
//	while(xZeroFlag==1);		//������ɣ�
	while(toolX2Flag==1);			// ������
	
	TIM_Cmd(TIM3, DISABLE); 	//������ɣ�PWMֹͣ�
	TIM_Cmd(TIM14,DISABLE);		// �ر��������
	stepsFlag[0] = 0;
	enableXaxisCountINT();		//����x������жϣ�
	delay_s(2);
}

void dataFromTemp2Saved(u8 num)		// ����ŵ�temp���ݽ��б��棺0���ӹ��㵽��� 1-4����㵽����
{
	changeToolsSavedSteps.xSteps[num] = stepsTemp.xSteps[0];
	changeToolsSavedSteps.ySteps[num] = stepsTemp.ySteps[0];
	changeToolsSavedSteps.zSteps[num] = stepsTemp.zSteps[0];
}

void toolsFromORG2Start(void)	// ����㷵��ԭ��������λ��
{
	//	x�������ƣ�������λ�ã�
	xAxisDIR_OFF;				// x���ƶ�����
	xAxisPlusNum = changeToolsSavedSteps.xSteps[0];		//x���ƶ��Ĳ�����
	pwmNumTim3 = 0; 		
	xAxisClkSet(8000);			//x�����Ƶ��ٶȣ�
	TIM_Cmd(TIM3, ENABLE);		//��ʼ������	
	while(xAxisStepFlag!=2);	//������ɣ�
	xAxisStepFlag = 0;
	delay_s(8);
	
	//	y�������ƣ�������λ�ã�
	yAxisDIR_ON;				// y���ƶ�����
	yAxisPlusNum = changeToolsSavedSteps.ySteps[0];		//y���ƶ��Ĳ�����
	pwmNumTim2 = 0; 		
	yAxisClkSet(8000);			//y�����Ƶ��ٶȣ�
	TIM_Cmd(TIM2, ENABLE);		//��ʼ������	
	while(yAxisStepFlag!=2);	//������ɣ�
	yAxisStepFlag = 0;
	delay_s(8);
	
	//	z�����ƣ�������λ�ã�
	zAxisDIR_ON;				// z���ƶ�����
	zAxisPlusNum = changeToolsSavedSteps.zSteps[0];		//z���ƶ��Ĳ�����
	pwmNumTim4 = 0; 		
	zAxisClkSet(8000);			//z�����Ƶ��ٶȣ�
	TIM_Cmd(TIM4, ENABLE);		//��ʼ������	
	while(zAxisStepFlag!=2);	//������ɣ�
	zAxisStepFlag = 0;
	delay_s(5);
}

void fromORG2Tools(void)	// ����㵽����λ��
{
	
	//	x�������ƣ�������λ�ã�
	xAxisDIR_OFF;				// x���ƶ�����
	xAxisPlusNum = changeToolsSavedSteps.xSteps[targeToolNo];		//x���ƶ��Ĳ�����
	pwmNumTim3 = 0; 		
	xAxisClkSet(8000);			//x�����Ƶ��ٶȣ�
	TIM_Cmd(TIM3, ENABLE);		//��ʼ������	
	while(xAxisStepFlag!=2);	//������ɣ�
	xAxisStepFlag = 0;
	delay_s(8);
	
	//	y�������ƣ�������λ�ã�
	yAxisDIR_ON;				// y���ƶ�����
	yAxisPlusNum = changeToolsSavedSteps.ySteps[targeToolNo];		//y���ƶ��Ĳ�����
	pwmNumTim2 = 0; 		
	yAxisClkSet(8000);			//y�����Ƶ��ٶȣ�
	TIM_Cmd(TIM2, ENABLE);		//��ʼ������	
	while(yAxisStepFlag!=2);	//������ɣ�
	yAxisStepFlag = 0;
	delay_s(8);
	
	//	z�����ƣ�������λ�ã�
	zAxisDIR_ON;				// z���ƶ�����
	zAxisPlusNum = changeToolsSavedSteps.zSteps[targeToolNo];		//z���ƶ��Ĳ�����
	pwmNumTim4 = 0; 		
	zAxisClkSet(8000);			//z�����Ƶ��ٶȣ�
	TIM_Cmd(TIM4, ENABLE);		//��ʼ������	
	while(zAxisStepFlag!=2);	//������ɣ�
	zAxisStepFlag = 0;
	delay_s(5);
}

void xFromORG2ToolsXaPOS(void)
{
	//	X���ƶ���ȡ��λ�õ�XA
	xAxisDIR_OFF;				//x���򵶼��ƶ�
	disableXaxisCountINT(); 	//��ֹx������жϣ�
	xAxisClkSet(8000);			//x����Ƶ���趨��
	TIM_Cmd(TIM3, ENABLE);  	//ʹ��x
	
	while(toolX1Flag==1);		//������ɣ�
	
	TIM_Cmd(TIM3, DISABLE); 	//������ɣ�PWMֹͣ��
	enableXaxisCountINT();		//����x������жϣ�
}

void xFromORG2ToolsXbPOS(void)
{
	//	X���ƶ���ȡ��λ�õ�XB
	xAxisDIR_OFF;				//x���򵶼��ƶ���ON��ΪOFF
	disableXaxisCountINT(); 	//��ֹx������жϣ�
	xAxisClkSet(8000);			//x����Ƶ���趨��
	TIM_Cmd(TIM3, ENABLE);  	//ʹ��x
	
	while(toolX2Flag==1);		//������ɣ�
	TIM_Cmd(TIM3, DISABLE); 	//������ɣ�PWMֹͣ��
	enableXaxisCountINT();		//����x������жϣ�
}

void yFromORG2ToolsPOS(u8 dir,u8 pos)
{
	//	Y���ƶ���ȡ��λ�õ�Y
	if(dir==1)
		yAxisDIR_OFF;				//y���㷽��
	else
		yAxisDIR_ON;
	
	disableYaxisCountINT(); 	//��ֹy������жϣ�
	yAxisClkSet(8000);			//y����Ƶ���趨��
	TIM_Cmd(TIM2, ENABLE);
	
	if(pos == 1)
		{
			while(toolY1Flag==1);		//����1λ�ã�
		}
	else if(pos == 2)
		{
			while(toolY2Flag==1);		//����2λ�ã�
		}
	
	else if(pos == 3)
		{
			while(toolY3Flag==1);		//����3λ�ã�
		}
	
	else if(pos == 4)
		{
			while(toolY4Flag==1);		//����4λ�ã�
		}
	TIM_Cmd(TIM2, DISABLE); 			// �رն�ʱ�� 
	enableYaxisCountINT();				//����y������жϣ�
}

void xAxisUpNStep(u32 steps)
{
	//	x�������ƣ�������λ�ã�
	xAxisDIR_OFF;				// x���ƶ�����
	xAxisPlusNum = steps;		//x�����ƵĲ�����
	pwmNumTim3 = 0; 		
	xAxisClkSet(8000);			//x�����Ƶ��ٶȣ�
	TIM_Cmd(TIM3, ENABLE);		//��ʼ������
	
	while(xAxisStepFlag!=2);	//������ɣ�
	xAxisStepFlag = 0;
}

void xAxisDownNStep(u32 steps)
{
	//	x�Ḻ���ƶ���������λ�ã�
	xAxisDIR_ON;				// x���ƶ�����
	xAxisPlusNum = steps;		//x�����ƵĲ�����
	pwmNumTim3 = 0; 		
	xAxisClkSet(8000);			//x�����Ƶ��ٶȣ�
	TIM_Cmd(TIM3, ENABLE);		//��ʼ������
	
	while(xAxisStepFlag!=2);	//������ɣ�
	xAxisStepFlag = 0;
}

void yAxisUpNStep(u32 steps)
{
	//	y�������ƣ�������λ�ã�
	yAxisDIR_ON;				// y���ƶ�����
	yAxisPlusNum = steps;		//y�����ƵĲ�����
	pwmNumTim2 = 0; 		
	yAxisClkSet(8000);			//y�����Ƶ��ٶȣ�
	TIM_Cmd(TIM2, ENABLE);		//��ʼ������
	
	while(yAxisStepFlag!=2);	//������ɣ�
	yAxisStepFlag = 0;
}

void zAxisDownNStep(u32 steps)
{
	//	Z�����ƣ��ϵ���
	zAxisDIR_ON;				//z ���˷���Z�᷽�򲻶ԣ� byYJY 2018��11��9��16:11:39
	zAxisPlusNum = steps;		//Z�����ƵĲ�����
	pwmNumTim4 = 0; 		
	zAxisClkSet(8000);			//z�����Ƶ��ٶȣ�
	TIM_Cmd(TIM4, ENABLE);		//��ʼ������
	
	while(zAxisStepFlag!=2);	//������ɣ�
	zAxisStepFlag = 0;
}

void zAxisDown2toolZFlag(void)
{
	//	Z�����ƣ��ϵ���
	zAxisDIR_ON;				//z ���˷���Z�᷽�򲻶ԣ� byYJY 2018��11��9��16:11:39
	disableZaxisCountINT(); 	//��ֹz������жϣ�
	zAxisClkSet(8000);			//z����Ƶ���趨��
	TIM_Cmd(TIM4, ENABLE);		//ʹ��z���㶯����
	
	while(toolZFlag==1);		//������ɣ�
	TIM_Cmd(TIM4, DISABLE); 	//������ɣ�PWMֹͣ��
	enableZaxisCountINT();		//����z������жϣ�
}

void zAxisUpNStep(u32 steps)
{
	//	Z�����ƣ��ж��Ƿ��ϵ��ɹ���
	zAxisDIR_OFF;				//z ���˷���Z�᷽�򲻶ԣ� byYJY 2018��11��9��16:11:39
	zAxisPlusNum = steps;		//Z�����ƵĲ�����
	pwmNumTim4 = 0; 
	zAxisClkSet(8000);			//z�����Ƶ��ٶȣ�
	TIM_Cmd(TIM4, ENABLE);		//��ʼ������
	
	while(zAxisStepFlag!=2);	//������ɣ�
	zAxisStepFlag = 0;
}

void zAxisUp2zZeroFlag(void)
{
	//	Z�����ƣ��ж��Ƿ��ϵ��ɹ���
	zAxisDIR_OFF;				//z ���˷���Z�᷽�򲻶ԣ� byYJY 2018��11��9��16:11:39
	disableZaxisCountINT(); 	//��ֹz������жϣ�
	zAxisClkSet(8000);			//z����Ƶ���趨��
	TIM_Cmd(TIM4, ENABLE);		//ʹ��z���㶯����
	
	while(zZeroFlag==1);		//������ɣ�
	TIM_Cmd(TIM4, DISABLE); 	//������ɣ�PWMֹͣ��
	enableZaxisCountINT();		//����z������жϣ�
}

void sendDataFillIn(u8 value)
{
	//�����ɹ�������Ϣ��
	backResString[0] = FrameB1;
	backResString[1] = FrameB2;
	backResString[2] = CONTROLINFO;
	
	backResString[3] = 9;
	backResString[4] = 0x00;
	
	backResString[5] = 3+targeToolNo;
	backResString[6] = 0x00;
	
	backResString[7] = value;
	
	backResString[8] = FrameEnd;
}

void sendMessage2PC(void)
{
	u8 i=0;
	for(i=0;i<9;i++)
		{
			USART_SendData(USART3,backResString[i]); 
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//�ȴ����ͽ���
		}
}

// �ѵ�
u8 cleanTools(void)
{
	if(mainMoterToolsFlag==0)		//0:�е���	1:�޵��� ������Ҫ�ж����޵�
	{		
		sendDataFillIn(0x02);
		sendMessage2PC();
		return 1;
	}
	else if(mainMoterToolsFlag==1)
	{
		mainMotorStop();	// ����ֹͣת��
		delay_s(2);

		/* �µĻ������̣�byYJY 2018��11��9��13:02:23  */	
		// �ӹ��ӹ��㵽���
		stepsTemp.xSteps[0] = 0;					// ��������
		stepsTemp.ySteps[0] = 0;
		stepsTemp.zSteps[0] = 0;					
		axis3Back2ORGWithCnt();									// �ص���㣬 ���Ʋ����Ĺ���	
		dataFromTemp2Saved(0);									// ���ݱ���	
		delay_s(2);
		delay_s(5);	// ����۲�
		// ����㵽����λ��
		GPIO_SetBits(GPIOB,GPIO_Pin_12);			// �ɿ�����
		delay_s(2);
		
		fromORG2Tools();		// ��㵽����
		delay_s(2);
		delay_s(5);	// ����۲�
		
		/* �ϵ��ߣ�������Ҫ���������ʵ���תһ�²��ܶ�λ��Z�������Ҫ������һ�㣬 byYJY */	
		
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);			// �н����ߣ��Ա��ѵ�
		delay_s(2);
		
		// Z������һ�ξ��룬���ж��Ƿ��е�����
		zAxisUpNStep(10000);									// ����Ӧ�ðγ����ߣ��ѵ��ɹ�
		delay_s(2);		
		delay_s(5);	// ����۲�
		
		if( 1 /*mainMoterToolsFlag == 0*/)		//�ϵ��ɹ�	��ʱĬ���ϵ����ɹ� byYJY
		{
			// ����ԭ�㣬���ݼ�¼������ԭ·���أ������Ӻ��� 
			#if TEST_STEPSNUM
				stepsTemp.xSteps[targeToolNo] = 0;	// ��������
				stepsTemp.ySteps[targeToolNo] = 0;	// ��������
				stepsTemp.zSteps[targeToolNo] = 0;	// ��������
				axis3Back2ORGWithCnt();
				dataFromTemp2Saved(targeToolNo);									// ���ݱ���	
			#else
				axis3Back2ORG();
			#endif
			delay_s(2);			
			delay_s(5);	// ����۲�
			
			// ������Ϣ�����ܻ᷵������ byYJY ע�͵�  ��Ȼû��������ȥ
			// sendDataFillIn(0x01);
			// sendMessage2PC();
			
			// ���ع�����λ��
			toolsFromORG2Start();
			delay_s(2);			
			
			// ���ñ�־λ
			xAxisStepFlag = 2;
			yAxisStepFlag = 2;
			zAxisStepFlag = 2;
			
			return 0;
		}
		else 							//�ϵ�ʧ��	1  ����ת�Ż������£��ٴ������ϵ�
		{			
			if(mainMoterToolsFlag == 0) //�ϵ��ɹ�
			{
				/* ������ԭ�㣬���ݼ�¼������ԭ·���أ������Ӻ��� */
			
				sendDataFillIn(0x01);
				sendMessage2PC();
				return 0;
			}
			else						//�ϵ�ʧ��	2
			{
				if(mainMoterToolsFlag == 0) //�ϵ��ɹ�
				{
					/* ������ԭ�㣬���ݼ�¼������ԭ·���أ������Ӻ��� */
			
					sendDataFillIn(0x01);
					sendMessage2PC();
					return 0;
				}
				else						//�ϵ�ʧ��	3
				{
					sendDataFillIn(0x02);
					sendMessage2PC();
					return 1;
				}
			}
		}		
	}
}

// װ��
u8 assemTools(void)
{
	if(mainMoterToolsFlag==0)		//0:�е���	1:�޵���
	{		
		sendDataFillIn(0x02);
		sendMessage2PC();
		return 1;
	}
	else if(mainMoterToolsFlag==1)
	{
		mainMotorStop();	// ����ֹͣת��
		delay_s(2);

		/* �µĻ������̣�byYJY 2018��11��9��13:02:23  */	
		// �ӹ��ӹ��㵽���
		stepsTemp.xSteps[0] = 0;					// ��������
		stepsTemp.ySteps[0] = 0;
		stepsTemp.zSteps[0] = 0;					
		axis3Back2ORGWithCnt();									// �ص���㣬 ���Ʋ����Ĺ���	
		dataFromTemp2Saved(0);									// ���ݱ���	
		delay_s(2);
		delay_s(5);	// ����۲�
		// ����㵽����λ��
//		stepsTemp.xSteps[targeToolNo] = changeToolsSavedSteps.xSteps[targeToolNo];	// �������õ���㵽���ߵĲ���
//		stepsTemp.ySteps[targeToolNo] = changeToolsSavedSteps.xSteps[targeToolNo];
//		stepsTemp.zSteps[targeToolNo] = changeToolsSavedSteps.xSteps[targeToolNo];		
//		// X���ƶ��̶�����������λ��
//		xAxisUpNStep(stepsTemp.xSteps[targeToolNo]);	
//		yAxisUpNStep(stepsTemp.ySteps[targeToolNo]);
//		zAxisDownNStep(stepsTemp.zSteps[targeToolNo]);									// Z�����ƣ�����������Ҫ��ת����
		// ������λ��֮ǰ�ʹ򿪵��߼����ĵ����
		GPIO_SetBits(GPIOB,GPIO_Pin_12);			// �ɿ�����
		delay_s(2);
		
		fromORG2Tools();
		delay_s(2);
		delay_s(5);	// ����۲�
		
		/* �ϵ��ߣ�������Ҫ���������ʵ���תһ�²��ܶ�λ��Z�������Ҫ������һ�㣬 byYJY */	
			

		// ���ӵ������ơ�����0.5mm�Ĺ���
//		xAxisUpNStep(200);										// 400��������1mm
//		delay_s(2);
//		xAxisDownNStep(200);
//		delay_s(2);
		
		// Z������һ�ξ��룬���ж��Ƿ��е�����
		zAxisUpNStep(10000);
		delay_s(2);		
		delay_s(5);	// ����۲�
		
		if( 1 /*mainMoterToolsFlag == 0*/)		//�ϵ��ɹ�	��ʱĬ���ϵ����ɹ� byYJY
		{
			// ����ԭ�㣬���ݼ�¼������ԭ·���أ������Ӻ��� 
			#if TEST_STEPSNUM
				stepsTemp.xSteps[targeToolNo] = 0;	// ��������
				stepsTemp.ySteps[targeToolNo] = 0;	// ��������
				stepsTemp.zSteps[targeToolNo] = 0;	// ��������
				axis3Back2ORGWithCnt();
				dataFromTemp2Saved(targeToolNo);									// ���ݱ���	
			#else
				axis3Back2ORG();
			#endif
			delay_s(2);			
			delay_s(5);	// ����۲�
			
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);			// �н�ʣ�൶��
			// ������Ϣ�����ܻ᷵������ byYJY ע�͵�  ��Ȼû��������ȥ
			// sendDataFillIn(0x01);
			// sendMessage2PC();
			
			// ���ع�����λ��
			toolsFromORG2Start();
			delay_s(2);			
			
			// ���ñ�־λ
			xAxisStepFlag = 2;
			yAxisStepFlag = 2;
			zAxisStepFlag = 2;
			
			// �����ã������ѵ�ѭ��
			cleanTools();
			
			return 0;
		}
		else 							//�ϵ�ʧ��	1  ����ת�Ż������£��ٴ������ϵ�
		{			
			if(mainMoterToolsFlag == 0) //�ϵ��ɹ�
			{
				/* ������ԭ�㣬���ݼ�¼������ԭ·���أ������Ӻ��� */
			
				sendDataFillIn(0x01);
				sendMessage2PC();
				return 0;
			}
			else						//�ϵ�ʧ��	2
			{
				if(mainMoterToolsFlag == 0) //�ϵ��ɹ�
				{
					/* ������ԭ�㣬���ݼ�¼������ԭ·���أ������Ӻ��� */
			
					sendDataFillIn(0x01);
					sendMessage2PC();
					return 0;
				}
				else						//�ϵ�ʧ��	3
				{
					sendDataFillIn(0x02);
					sendMessage2PC();
					return 1;
				}
			}
		}		
	}
}

// ����1
u8 changeToolsStep1(void)
{
	/*
	if(mainMoterToolsFlag==1)		//0:�е���	1:�޵���
	{
		sendDataFillIn(0x02);
		sendMessage2PC();
		return 1;
		
	}
	else if(mainMoterToolsFlag==0)
	{
		mainMotorStop();
		delay_s(120);

		axis3Back2ORG();
//		xFromORG2ToolsPOS();
		yFromORG2ToolsPOS(0,usingToolNo);

//		zAxisDown(10000);
		//�н�
		motorChangeToolsLock();
//		zAxisUp(10000);

		if(mainMoterToolsFlag == 1)	//�ѵ��ɹ�
		{
			return 0;
		}
		else							//�ѵ�ʧ��	1
		{
			//�ɿ�
			motorChangeToolsUnLock();
	//		zAxisDown(10000);
			//�н�
			motorChangeToolsLock();
	//		zAxisUp(10000);
			
			if(mainMoterToolsFlag == 1) //�ѵ��ɹ�
			{
				return 0;
			}
			else						//�ѵ�ʧ��	2
			{
				//�ɿ�
				motorChangeToolsUnLock();
	//			zAxisDown(10000);
				//�н�
				motorChangeToolsLock();
//				zAxisUp(10000);
				
				if(mainMoterToolsFlag == 1) //�ѵ��ɹ�
				{
					return 0;
				}
				else					//�ѵ�ʧ��	3
				{
					sendDataFillIn(0x02);
					sendMessage2PC();
					return 1;
				}
			}
		}
	}
	*/
}

// ����2
u8 changeToolsStep2(void)
{
	/*
	u8 dir;
	if(usingToolNo<targeToolNo)
		dir = 0;
	else
		dir = 1;
	
	yFromORG2ToolsPOS(dir,targeToolNo);
	
//	zAxisDown(10000);
//	zAxisUp(10000);
	
	if(mainMoterToolsFlag == 0) 	//�ϵ��ɹ�
	{
		axis3Back2ORG();
		sendDataFillIn(0x01);
		sendMessage2PC();
		return 0;
	}
	else							//�ϵ�ʧ��	1
	{
//		zAxisDown(10000);
//		zAxisUp(10000);
		
		if(mainMoterToolsFlag == 0) //�ϵ��ɹ�
		{
			axis3Back2ORG();
			sendDataFillIn(0x01);
			sendMessage2PC();
			return 0;
		}
		else						//�ϵ�ʧ��	2
		{
//			zAxisDown(10000);
//			zAxisUp(10000);
			
			if(mainMoterToolsFlag == 0) //�ϵ��ɹ�
			{
				axis3Back2ORG();
				sendDataFillIn(0x01);
				sendMessage2PC();
				return 0;
			}
			else						//�ϵ�ʧ��	3
			{
				sendDataFillIn(0x02);
				sendMessage2PC();
				return 1;
			}
		}
	}*/
}

