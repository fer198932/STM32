/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          原能智能制造机器人
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : ChangeTools.C
* By  : Hiker Lee
* 2018-05-07
* 重写换刀逻辑，因为坐标系变了；20180601
* 上刀机构修改，程序调整；20180720
* 增加到到零点计数的功能 byYJY 2018年11月12日14:13:00
********************************************************************************************************/
#include "ChangeTools.h"
#include "ComDataProc.h"

#define zZeroFlag GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_10)				//0:到位；1:未到位；
#define yZeroFlag GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_7)
#define xZeroFlag GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_6)

#define toolY1Flag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_0)
#define toolY2Flag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_1)
#define toolY3Flag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_2)				// 没用了，1×4时用的
#define toolY4Flag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_3)				// 没用了，1×4时用的

#define toolX1Flag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_4)
#define toolX2Flag GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11)

#define toolZFlag GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_13)				// 改为PF13 byYJY

#define mainMoterToolsFlag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_7)		//0:有刀；	1:无刀；	
#define unClampFlag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_8)			//0:松开到位；	1:松开未到位；
#define clampFlag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_10)				//0:夹紧到位；	1:夹紧未到位；

#define sheelOpenCloseFlag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_9)		//0:外壳打开；	1:外壳关闭；

u8 targeToolNo=0;
u8 usingToolNo=0;

extern u8 xAxisStepFlag;
extern u8 yAxisStepFlag;
extern u8 zAxisStepFlag; 
extern unsigned char backResString[20];

extern u32 pwmNumTim2;
extern u32 pwmNumTim3;
extern u32 pwmNumTim4;

// 设置标志位
u8 stepsFlag[5] = {0, 0, 0, 0, 0}; 				// 打开X、Y、Z、A、B的计数 1：打开； 0：关闭

ChangeToolsSavedSteps changeToolsSavedSteps;	// 由上位机数据初始化
ChangeToolsSavedSteps stepsTemp;				// 临时保存走过的步数
// 根据上位机发送的数据，对刀具位置等进行初始化
void savedStepsInit(void)
{
	// 初始化工件与零点的位置关系
	changeToolsSavedSteps.xSteps[0] = 0;
	changeToolsSavedSteps.ySteps[0] = 0;
	changeToolsSavedSteps.zSteps[0] = 0;
	
	// 初始化刀具与零点之间的位置关系：由上位机传来，暂时写个固定值
	changeToolsSavedSteps.xSteps[1] = 0xf5ab;//0xf555;//0xf548;	 // 刀具1的位置
	changeToolsSavedSteps.ySteps[1] = 0x276a;//0x254e;//0x2540;
	changeToolsSavedSteps.zSteps[1] = 0x7327;//0x73ac;//0x3f2f;
	
	changeToolsSavedSteps.xSteps[2] = 10000;	// 刀具2的位置
	changeToolsSavedSteps.ySteps[2] = 10000;
	changeToolsSavedSteps.zSteps[2] = 10000;
	
	changeToolsSavedSteps.xSteps[3] = 10000;	// 刀具3的位置
	changeToolsSavedSteps.ySteps[3] = 10000;
	changeToolsSavedSteps.zSteps[3] = 10000;
	
	changeToolsSavedSteps.xSteps[4] = 10000;	// 刀具4的位置
	changeToolsSavedSteps.ySteps[4] = 10000;
	changeToolsSavedSteps.zSteps[4] = 10000;
	
}

void mainToolFlagIOInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  //以下使能位置传感器；
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);		//使能GPIOG时钟
 
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;	  				//刀1-4，脱刀电机限位
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;			  	//输入
  GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;			  	//上拉输入
  GPIO_Init(GPIOG,&GPIO_InitStructure); 				  	//初始化GPIOG
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
	//	Z轴归零
	// zAxisDIR_ON;				//z归零方向
	zAxisDIR_OFF;					// 测试用
	disableZaxisCountINT(); 	//禁止z轴计数中断；
	zAxisClkSet(Z_AXIS_RST_CLK);			//z归零频率设定；
	TIM_Cmd(TIM4, ENABLE);		//使能z归零动作；	
	while(zZeroFlag==1);		//归零完成；
	TIM_Cmd(TIM4, DISABLE); 	//归零完成，PWM停止；
	enableZaxisCountINT();		//开启z轴计数中断；
	delay_s(2);

	//	Y轴归零
	yAxisDIR_OFF;				//y归零方向
	disableYaxisCountINT(); 	//禁止y轴计数中断；
	yAxisClkSet(Y_AXIS_RST_CLK);			//y归零频率设定；
	TIM_Cmd(TIM2, ENABLE);		//使能y归零动作；	
//	while(yZeroFlag==1);		//归零完成；
	while(toolY1Flag==1);			// 测试用
	TIM_Cmd(TIM2, DISABLE); 	//归零完成，PWM停止；
	enableYaxisCountINT();		//开启y轴计数中断；
	delay_s(2);
	
	//	X轴归零
	xAxisDIR_ON;				//x归零方向   //xAxisDIR_ON
	disableXaxisCountINT(); 	//禁止x轴计数中断；
	xAxisClkSet(X_AXIS_RST_CLK);			//x归零频率设定；
	TIM_Cmd(TIM3, ENABLE);		//使能x归零动作；
//	while(xZeroFlag==1);		//归零完成；
	while(toolX2Flag==1);			// 测试用
	TIM_Cmd(TIM3, DISABLE); 	//归零完成，PWM停止�
	enableXaxisCountINT();		//开启x轴计数中断；	
	delay_s(2);
}

// 能计步数的回到原点
void axis3Back2ORGWithCnt(void)
{
	// 不允许打断
	xAxisStepFlag = 0;
	yAxisStepFlag = 0;
	zAxisStepFlag = 0;
	
	//	Z轴归零 ////////////////////////////////////////////////
	zAxisDIR_ON;				//z归零方向
//	disableZaxisCountINT(); 	//禁止z轴计数中断； 禁止后无法计数

	// 只有无加减速运动
	zAxisClkSet(Z_AXIS_RST_CLK>>1);			//z归零频率设定；
	
	// 开始动作并计数
	stepsFlag[2] = 1;					// 设置标志位，z轴开始计数 1:计数； 0：停止计数
	TIM_Cmd(TIM4, ENABLE);		//使能z归零动作；
	
	while(zZeroFlag==1);		//归零完成；
	
	TIM_Cmd(TIM4, DISABLE); 	//归零完成，PWM停止；
	stepsFlag[2] = 0;					//  关闭计数
//	enableZaxisCountINT();		//开启z轴计数中断；
	delay_s(1);

	//	Y轴归零  ///////////////////////////////////////////////////
	yAxisDIR_ON;				//y归零方向
//	disableYaxisCountINT(); 	//禁止y轴计数中断；

	// 只有无加减速运动
	yAxisClkSet(Y_AXIS_RST_CLK>>1);			//y归零频率设定；

	// 开始动作并计数
	stepsFlag[1] = 1;					// 设置标志位，y轴开始计数 1:计数； 0：停止计数
	TIM_Cmd(TIM2, ENABLE);		//使能y归零动作；
	
	while(yZeroFlag==1);		//归零完成；
//	while(toolY1Flag==1);			// 测试用
	
	TIM_Cmd(TIM2, DISABLE); 	//归零完成，PWM停止；
	stepsFlag[1] = 0;
//	enableYaxisCountINT();		//开启y轴计数中断；
	delay_s(1);
	
	//	X轴归零  /////////////////////////////////////////////////
	xAxisDIR_OFF;				//x归零方向 
//	disableXaxisCountINT(); 	//禁止x轴计数中断；

	// 只有无加减速运动
	xAxisClkSet(X_AXIS_RST_CLK>>1);			//x归零频率设定；
	
	// 开始动作并计数
	stepsFlag[0] = 1;					// 设置标志位，x轴开始计数 1:计数； 0：停止计数
	TIM_Cmd(TIM3, ENABLE);		//使能x归零动作；
	
	while(xZeroFlag==1);		//归零完成；
//	while(toolX2Flag==1);			// 测试用
	
	TIM_Cmd(TIM3, DISABLE); 	//归零完成，PWM停止�
	stepsFlag[0] = 0;
//	enableXaxisCountINT();		//开启x轴计数中断；
	delay_s(1);
	
	// 所有动作完成
	stepMotor5AxisStop();
}

void dataFromTemp2Saved(u8 num)		// 将编号的temp数据进行保存：0：加工点到零点 1-4：零点到刀具
{
	changeToolsSavedSteps.xSteps[num] = stepsTemp.xSteps[0];
	changeToolsSavedSteps.ySteps[num] = stepsTemp.ySteps[0];
	changeToolsSavedSteps.zSteps[num] = stepsTemp.zSteps[0];
}

void toolsFromORG2Start(void)	// 从零点返回原来工件的位置
{
	//	x轴正向移，到工件位置； ///////////////////////
	xAxisDIR_ON;				// x轴移动方向
	xAxisPlusNum = changeToolsSavedSteps.xSteps[0];		//x轴移动的步数；
	pwmNumTim3 = 0; 
	xAxisStepFlag = 0;	

#ifndef IF_ADDSUB_WORK_CHANGETOOLS		
	// 无加减速运动
	xAxisClkSet(X_AXIS_RST_CLK);			//x轴下移的速度；
#else
	// 有加减速运动
	setSpeedRST_Seg(X_AXIS_RST_CLK, 0);	
#endif
	
	TIM_Cmd(TIM3, ENABLE);		//开始动作；	
	while(xAxisStepFlag!=2);	//动作完成；
	xAxisStepFlag = 0;
	delay_s(1);
	
	//	y轴正向移，到工件位置； ///////////////////////
	yAxisDIR_OFF;				// y轴移动方向
	yAxisPlusNum = changeToolsSavedSteps.ySteps[0];		//y轴移动的步数；
	pwmNumTim2 = 0; 
	yAxisStepFlag = 0;	

#ifndef IF_ADDSUB_WORK_CHANGETOOLS	
	// 无加减速运动
	yAxisClkSet(Y_AXIS_RST_CLK);			//y轴下移的速度；
#else	
	// 有加减速运动
	setSpeedRST_Seg(Y_AXIS_RST_CLK, 1);	
#endif

	TIM_Cmd(TIM2, ENABLE);		//开始动作；	
	while(yAxisStepFlag!=2);	//动作完成；
	yAxisStepFlag = 0;
	delay_s(1);
	
	//	z轴下移，到工件位置； ////////////////////////////////
	zAxisDIR_OFF;				// z轴移动方向
	zAxisPlusNum = changeToolsSavedSteps.zSteps[0];		//z轴移动的步数；
	pwmNumTim4 = 0; 	
	zAxisStepFlag = 0;	

#ifndef IF_ADDSUB_WORK_CHANGETOOLS	
	// 无加减速运动
	zAxisClkSet(Z_AXIS_RST_CLK);			//z轴下移的速度；
#else	
	// 有加减速运动
	setSpeedRST_Seg(Z_AXIS_RST_CLK, 2);	
#endif

	TIM_Cmd(TIM4, ENABLE);		//开始动作；	
	while(zAxisStepFlag!=2);	//动作完成；
	zAxisStepFlag = 0;
	delay_s(1);
	
	// 所有动作完成
	stepMotor5AxisStop();
}

void fromORG2Tools(void)	// 从零点到刀具位置
{
	
	//	x轴正向移，到刀具位置；
	xAxisDIR_OFF;				// x轴移动方向
	xAxisPlusNum = changeToolsSavedSteps.xSteps[targeToolNo];		//x轴移动的步数；
	pwmNumTim3 = 0; 		
	xAxisClkSet(X_AXIS_RST_CLK);			//x轴下移的速度；
	TIM_Cmd(TIM3, ENABLE);		//开始动作；	
	while(xAxisStepFlag!=2);	//动作完成；
	xAxisStepFlag = 0;
	delay_s(8);
	
	//	y轴正向移，到工件位置；
	yAxisDIR_ON;				// y轴移动方向
	yAxisPlusNum = changeToolsSavedSteps.ySteps[targeToolNo];		//y轴移动的步数；
	pwmNumTim2 = 0; 		
	yAxisClkSet(Y_AXIS_RST_CLK);			//y轴下移的速度；
	TIM_Cmd(TIM2, ENABLE);		//开始动作；	
	while(yAxisStepFlag!=2);	//动作完成；
	yAxisStepFlag = 0;
	delay_s(8);
	
	//	z轴下移，到工件位置；
	zAxisDIR_ON;				// z轴移动方向
	zAxisPlusNum = changeToolsSavedSteps.zSteps[targeToolNo];		//z轴移动的步数；
	pwmNumTim4 = 0; 		
	zAxisClkSet(Z_AXIS_RST_CLK);			//z轴下移的速度；
	TIM_Cmd(TIM4, ENABLE);		//开始动作；	
	while(zAxisStepFlag!=2);	//动作完成；
	zAxisStepFlag = 0;
	delay_s(5);
}

void xFromORG2ToolsXaPOS(void)
{
	//	X轴移动到取刀位置的XA
	xAxisDIR_OFF;				//x轴向刀架移动
	disableXaxisCountINT(); 	//禁止x轴计数中断；
	xAxisClkSet(X_AXIS_RST_CLK);			//x归零频率设定；
	TIM_Cmd(TIM3, ENABLE);  	//使能x
	
	while(toolX1Flag==1);		//归零完成；
	
	TIM_Cmd(TIM3, DISABLE); 	//归零完成，PWM停止；
	enableXaxisCountINT();		//开启x轴计数中断；
}

void xFromORG2ToolsXbPOS(void)
{
	//	X轴移动到取刀位置的XB
	xAxisDIR_OFF;				//x轴向刀架移动，ON改为OFF
	disableXaxisCountINT(); 	//禁止x轴计数中断；
	xAxisClkSet(X_AXIS_RST_CLK);			//x归零频率设定；
	TIM_Cmd(TIM3, ENABLE);  	//使能x
	
	while(toolX2Flag==1);		//归零完成；
	TIM_Cmd(TIM3, DISABLE); 	//归零完成，PWM停止；
	enableXaxisCountINT();		//开启x轴计数中断；
}

void yFromORG2ToolsPOS(u8 dir,u8 pos)
{
	//	Y轴移动到取刀位置的Y
	if(dir==1)
		yAxisDIR_OFF;				//y归零方向
	else
		yAxisDIR_ON;
	
	disableYaxisCountINT(); 	//禁止y轴计数中断；
	yAxisClkSet(Y_AXIS_RST_CLK);			//y归零频率设定；
	TIM_Cmd(TIM2, ENABLE);
	
	if(pos == 1)
		{
			while(toolY1Flag==1);		//到刀1位置；
		}
	else if(pos == 2)
		{
			while(toolY2Flag==1);		//到刀2位置；
		}
	
	else if(pos == 3)
		{
			while(toolY3Flag==1);		//到刀3位置；
		}
	
	else if(pos == 4)
		{
			while(toolY4Flag==1);		//到刀4位置；
		}
	TIM_Cmd(TIM2, DISABLE); 			// 关闭定时器 
	enableYaxisCountINT();				//开启y轴计数中断；
}

void xAxisUpNStep(u32 steps)
{
	//	x轴正向移，到刀具位置；
	xAxisDIR_ON;				// x轴移动方向
	xAxisPlusNum = steps;		//x轴下移的步数；
	pwmNumTim3 = 0; 
	yAxisStepFlag	= 0;	
	xAxisClkSet(X_AXIS_RST_CLK);			//x轴下移的速度；
	TIM_Cmd(TIM3, ENABLE);		//开始动作；
	
	while(xAxisStepFlag!=2);	//动作完成；
	xAxisStepFlag = 0;
}

void xAxisDownNStep(u32 steps)
{
	//	x轴负向移动，到刀具位置；
	xAxisDIR_ON;				// x轴移动方向
	xAxisPlusNum = steps;		//x轴下移的步数；
	pwmNumTim3 = 0; 		
	xAxisClkSet(X_AXIS_RST_CLK);			//x轴下移的速度；
	TIM_Cmd(TIM3, ENABLE);		//开始动作；
	
	while(xAxisStepFlag!=2);	//动作完成；
	xAxisStepFlag = 0;
}

void yAxisUpNStep(u32 steps)
{
	//	y轴正向移，到刀具位置；
	yAxisDIR_OFF;				// y轴移动方向
	yAxisPlusNum = steps;		//y轴下移的步数；
	pwmNumTim2 = 0; 	
	yAxisStepFlag	= 0;
	yAxisClkSet(Y_AXIS_RST_CLK);			//y轴下移的速度；
	TIM_Cmd(TIM2, ENABLE);		//开始动作；
	
	while(yAxisStepFlag!=2);	//动作完成；
	yAxisStepFlag = 0;
}

void zAxisDownNStep(u32 steps)
{
	//	Z轴下移，上刀；
	zAxisDIR_ON;				//z 换了方向，Z轴方向不对， byYJY 2018年11月9日16:11:39
	zAxisPlusNum = steps;		//Z轴下移的步数；
	pwmNumTim4 = 0; 		
	zAxisClkSet(Z_AXIS_RST_CLK);			//z轴下移的速度；
	TIM_Cmd(TIM4, ENABLE);		//开始动作；
	
	while(zAxisStepFlag!=2);	//动作完成；
	zAxisStepFlag = 0;
}

void zAxisDown2toolZFlag(void)
{
	//	Z轴下移，上刀；
	zAxisDIR_ON;				//z 换了方向，Z轴方向不对， byYJY 2018年11月9日16:11:39
	disableZaxisCountINT(); 	//禁止z轴计数中断；
	zAxisClkSet(Z_AXIS_RST_CLK);			//z归零频率设定；
	TIM_Cmd(TIM4, ENABLE);		//使能z归零动作；
	
	while(toolZFlag==1);		//归零完成；
	TIM_Cmd(TIM4, DISABLE); 	//归零完成，PWM停止；
	enableZaxisCountINT();		//开启z轴计数中断；
}

void zAxisUpNStep(u32 steps)
{
	//	Z轴上移，判断是否上刀成功；
	zAxisDIR_OFF;				//z 换了方向，Z轴方向不对， byYJY 2018年11月9日16:11:39
	zAxisPlusNum = steps;		//Z轴下移的步数；
	pwmNumTim4 = 0; 
	zAxisClkSet(Z_AXIS_RST_CLK);			//z轴下移的速度；
	TIM_Cmd(TIM4, ENABLE);		//开始动作；
	
	while(zAxisStepFlag!=2);	//动作完成；
	zAxisStepFlag = 0;
}

void zAxisUp2zZeroFlag(void)
{
	//	Z轴上移，判断是否上刀成功；
	zAxisDIR_OFF;				//z 换了方向，Z轴方向不对， byYJY 2018年11月9日16:11:39
	disableZaxisCountINT(); 	//禁止z轴计数中断；
	zAxisClkSet(Z_AXIS_RST_CLK);			//z归零频率设定；
	TIM_Cmd(TIM4, ENABLE);		//使能z归零动作；
	
	while(zZeroFlag==1);		//归零完成；
	TIM_Cmd(TIM4, DISABLE); 	//归零完成，PWM停止；
	enableZaxisCountINT();		//开启z轴计数中断；
}

void sendDataFillIn(u8 value)
{
	//换刀成功发送信息；
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
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//等待发送结束
		}
}

// 脱刀
u8 cleanTools(void)
{
	if(mainMoterToolsFlag==0)		//0:有刀；	1:无刀； 这里需要判断有无刀
	{		
		sendDataFillIn(0x02);
		sendMessage2PC();
		return 1;
	}
	else if(mainMoterToolsFlag==1)
	{
		mainMotorStop();	// 刀轴停止转动
		delay_s(2);

		/* 新的换刀流程，byYJY 2018年11月9日13:02:23  */	
		// 从工加工点到零点
		stepsTemp.xSteps[0] = 0;					// 计数置零
		stepsTemp.ySteps[0] = 0;
		stepsTemp.zSteps[0] = 0;					
		axis3Back2ORGWithCnt();									// 回到零点， 带计步数的功能	
		dataFromTemp2Saved(0);									// 数据保存	
		delay_s(2);
		delay_s(5);	// 方便观察
		// 从零点到刀具位置
		GPIO_SetBits(GPIOB,GPIO_Pin_12);			// 松开刀具
		delay_s(2);
		
		fromORG2Tools();		// 零点到刀具
		delay_s(2);
		delay_s(5);	// 方便观察
		
		/* 上刀具：可能需要刀轴主机适当慢转一下才能对位，Z轴可能需要往下走一点， byYJY */	
		
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);			// 夹紧刀具，以便脱刀
		delay_s(2);
		
		// Z轴上移一段距离，再判断是否有刀具在
		zAxisUpNStep(10000);									// 这里应该拔出刀具，脱刀成功
		delay_s(2);		
		delay_s(5);	// 方便观察
		
		if( 1 /*mainMoterToolsFlag == 0*/)		//上刀成功	暂时默认上刀都成功 byYJY
		{
			// 返回原点，根据记录的脉冲原路返回，需增加函数 
			#if TEST_STEPSNUM
				stepsTemp.xSteps[targeToolNo] = 0;	// 计数置零
				stepsTemp.ySteps[targeToolNo] = 0;	// 计数置零
				stepsTemp.zSteps[targeToolNo] = 0;	// 计数置零
				axis3Back2ORGWithCnt();
				dataFromTemp2Saved(targeToolNo);									// 数据保存	
			#else
				axis3Back2ORG();
			#endif
			delay_s(2);			
			delay_s(5);	// 方便观察
			
			// 返回消息，可能会返回两次 byYJY 注释掉  不然没法继续下去
			// sendDataFillIn(0x01);
			// sendMessage2PC();
			
			// 返回工件的位置
			toolsFromORG2Start();
			delay_s(2);			
			
			// 设置标志位
			xAxisStepFlag = 2;
			yAxisStepFlag = 2;
			zAxisStepFlag = 2;
			
			return 0;
		}
		else 							//上刀失败	1  再旋转着缓慢往下，再次试着上刀
		{			
			if(mainMoterToolsFlag == 0) //上刀成功
			{
				/* 不返回原点，根据记录的脉冲原路返回，需增加函数 */
			
				sendDataFillIn(0x01);
				sendMessage2PC();
				return 0;
			}
			else						//上刀失败	2
			{
				if(mainMoterToolsFlag == 0) //上刀成功
				{
					/* 不返回原点，根据记录的脉冲原路返回，需增加函数 */
			
					sendDataFillIn(0x01);
					sendMessage2PC();
					return 0;
				}
				else						//上刀失败	3
				{
					sendDataFillIn(0x02);
					sendMessage2PC();
					return 1;
				}
			}
		}		
	}
}

// 装刀
u8 assemTools(void)
{
	if(mainMoterToolsFlag==0)		//0:有刀；	1:无刀；
	{		
		sendDataFillIn(0x02);
		sendMessage2PC();
		return 1;
	}
	else if(mainMoterToolsFlag==1)
	{
		mainMotorStop();	// 刀轴停止转动
		delay_s(2);

		/* 新的换刀流程，byYJY 2018年11月9日13:02:23  */	
		// 从工加工点到零点
		stepsTemp.xSteps[0] = 0;					// 计数置零
		stepsTemp.ySteps[0] = 0;
		stepsTemp.zSteps[0] = 0;					
		axis3Back2ORGWithCnt();									// 回到零点， 带计步数的功能	
		dataFromTemp2Saved(0);									// 数据保存	
		delay_s(2);
		delay_s(5);	// 方便观察
		// 从零点到刀具位置
//		stepsTemp.xSteps[targeToolNo] = changeToolsSavedSteps.xSteps[targeToolNo];	// 读出设置的零点到刀具的步数
//		stepsTemp.ySteps[targeToolNo] = changeToolsSavedSteps.xSteps[targeToolNo];
//		stepsTemp.zSteps[targeToolNo] = changeToolsSavedSteps.xSteps[targeToolNo];		
//		// X轴移动固定步数到刀具位置
//		xAxisUpNStep(stepsTemp.xSteps[targeToolNo]);	
//		yAxisUpNStep(stepsTemp.ySteps[targeToolNo]);
//		zAxisDownNStep(stepsTemp.zSteps[targeToolNo]);									// Z轴下移，后续可能需要慢转往下
		// 到刀具位置之前就打开刀具夹座的电磁铁
		GPIO_SetBits(GPIOB,GPIO_Pin_12);			// 松开刀具
		delay_s(2);
		
		fromORG2Tools();
		delay_s(2);
		delay_s(5);	// 方便观察
		
		/* 上刀具：可能需要刀轴主机适当慢转一下才能对位，Z轴可能需要往下走一点， byYJY */	
			

		// 增加刀具左移、右移0.5mm的功能
//		xAxisUpNStep(200);										// 400个脉冲走1mm
//		delay_s(2);
//		xAxisDownNStep(200);
//		delay_s(2);
		
		// Z轴上移一段距离，再判断是否有刀具在
		zAxisUpNStep(10000);
		delay_s(2);		
		delay_s(5);	// 方便观察
		
		if( 1 /*mainMoterToolsFlag == 0*/)		//上刀成功	暂时默认上刀都成功 byYJY
		{
			// 返回原点，根据记录的脉冲原路返回，需增加函数 
			#if TEST_STEPSNUM
				stepsTemp.xSteps[targeToolNo] = 0;	// 计数置零
				stepsTemp.ySteps[targeToolNo] = 0;	// 计数置零
				stepsTemp.zSteps[targeToolNo] = 0;	// 计数置零
				axis3Back2ORGWithCnt();
				dataFromTemp2Saved(targeToolNo);									// 数据保存	
			#else
				axis3Back2ORG();
			#endif
			delay_s(2);			
			delay_s(5);	// 方便观察
			
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);			// 夹紧剩余刀具
			// 返回消息，可能会返回两次 byYJY 注释掉  不然没法继续下去
			// sendDataFillIn(0x01);
			// sendMessage2PC();
			
			// 返回工件的位置
			toolsFromORG2Start();
			delay_s(2);			
			
			// 设置标志位
			xAxisStepFlag = 2;
			yAxisStepFlag = 2;
			zAxisStepFlag = 2;
			
			// 测试用，进入脱刀循环
			cleanTools();
			
			return 0;
		}
		else 							//上刀失败	1  再旋转着缓慢往下，再次试着上刀
		{			
			if(mainMoterToolsFlag == 0) //上刀成功
			{
				/* 不返回原点，根据记录的脉冲原路返回，需增加函数 */
			
				sendDataFillIn(0x01);
				sendMessage2PC();
				return 0;
			}
			else						//上刀失败	2
			{
				if(mainMoterToolsFlag == 0) //上刀成功
				{
					/* 不返回原点，根据记录的脉冲原路返回，需增加函数 */
			
					sendDataFillIn(0x01);
					sendMessage2PC();
					return 0;
				}
				else						//上刀失败	3
				{
					sendDataFillIn(0x02);
					sendMessage2PC();
					return 1;
				}
			}
		}		
	}
}

// 换刀1
u8 changeToolsStep1(void)
{
	/*
	if(mainMoterToolsFlag==1)		//0:有刀；	1:无刀；
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
		//夹紧
		motorChangeToolsLock();
//		zAxisUp(10000);

		if(mainMoterToolsFlag == 1)	//脱刀成功
		{
			return 0;
		}
		else							//脱刀失败	1
		{
			//松开
			motorChangeToolsUnLock();
	//		zAxisDown(10000);
			//夹紧
			motorChangeToolsLock();
	//		zAxisUp(10000);
			
			if(mainMoterToolsFlag == 1) //脱刀成功
			{
				return 0;
			}
			else						//脱刀失败	2
			{
				//松开
				motorChangeToolsUnLock();
	//			zAxisDown(10000);
				//夹紧
				motorChangeToolsLock();
//				zAxisUp(10000);
				
				if(mainMoterToolsFlag == 1) //脱刀成功
				{
					return 0;
				}
				else					//脱刀失败	3
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

// 换刀2
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
	
	if(mainMoterToolsFlag == 0) 	//上刀成功
	{
		axis3Back2ORG();
		sendDataFillIn(0x01);
		sendMessage2PC();
		return 0;
	}
	else							//上刀失败	1
	{
//		zAxisDown(10000);
//		zAxisUp(10000);
		
		if(mainMoterToolsFlag == 0) //上刀成功
		{
			axis3Back2ORG();
			sendDataFillIn(0x01);
			sendMessage2PC();
			return 0;
		}
		else						//上刀失败	2
		{
//			zAxisDown(10000);
//			zAxisUp(10000);
			
			if(mainMoterToolsFlag == 0) //上刀成功
			{
				axis3Back2ORG();
				sendDataFillIn(0x01);
				sendMessage2PC();
				return 0;
			}
			else						//上刀失败	3
			{
				sendDataFillIn(0x02);
				sendMessage2PC();
				return 1;
			}
		}
	}*/
}

// 分段式加减速-换刀及回零用， nAxis 表示某一轴 0-x, 1-y, 2-z
void setSpeedRST_Seg(u16 clk, u8 nAxis)
{
	// 速度初始化
	switch(nAxis)
	{
		case 0:					// x
			xAxisClk = clk;
			yAxisClk = 0;
			zAxisClk = 0;
			aAxisClk = 0;
			bAxisClk = 0;			
			break;
		
		case 1:					// y
			xAxisClk = 0;
			yAxisClk = clk;
			zAxisClk = 0;
			aAxisClk = 0;
			bAxisClk = 0;			
			break;
		
		case 2:					// z
			xAxisClk = 0;
			yAxisClk = 0;
			zAxisClk = clk;
			aAxisClk = 0;
			bAxisClk = 0;		
			break;
		
		default:
			xAxisClk = 0;
			yAxisClk = 0;
			zAxisClk = 0;
			aAxisClk = 0;
			bAxisClk = 0;		
			break;
	}
	
	// 代码来自 ComDataProc.c 
	preProcMaxNum();
	
	addSubStepNo = 1;		//加减速的步数计数器；
	workedTimeMS = 0;		//加减速的计时器�

	//*************Elsie***********
	addcount=preProcStep();
	//*************Elsie***********
	
	addSubFlagSet();		//0:需要定时器配置加减速；1:不需要�
	//addSubFlag=2;
	addSubSpeed();			//频率设置；
		
	motor5workstart();		//5轴的定时器启动；

	TIM7->CNT = 0;					//用于电机加减速处理，	40ms
	TIM_Cmd(TIM7,ENABLE);			//用于电机加减速处理，	40ms
}

