/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          原能智能制造机器人
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : Timer.C
* By  : Hiker Lee
* 2018-05-03
********************************************************************************************************/

#include "timer.h"
#include "led.h"
#include "lwip_comm.h"
#include "SysInit.h"
#include "ChangeTools.h"		// 包含了换刀步数的结构体
#include "AddSubSpeed.h"


extern unsigned char recvFrameFlag;
extern unsigned char recvFrameState;

extern u8 addSubStepSum;
extern u8 addSubStepNo;
extern u8 addSubFlag;			//0:需要定时器配置加减速；1:不需要；
extern u32 workTimeMSSum;
extern u32 workedTimeMS;

extern u16 xAxisClk,yAxisClk,zAxisClk,aAxisClk,bAxisClk;
extern u16 xAxisClk_Cur,yAxisClk_Cur,zAxisClk_Cur,aAxisClk_Cur,bAxisClk_Cur;
extern u32 xAxisPlusNum,yAxisPlusNum,zAxisPlusNum,aAxisPlusNum,bAxisPlusNum;
extern u8 xAxisStepFlag; 
extern u8 yAxisStepFlag; 
extern u8 zAxisStepFlag; 
extern u8 aAxisStepFlag; 
extern u8 bAxisStepFlag; 

u16 temp1,temp2,temp3,temp4,temp5;
u16 testTemp[12][5];
u8 i=0;

extern u32 pwmNumTim12;
extern u32 pwmNumTim9;
extern u32 pwmNumTim3;
extern u32 pwmNumTim4;
extern u32 pwmNumTim2;

extern u8 dataPreProcFlag;		//0:已完成预处理，待第二部处理；1:等待下一帧数据的预处理；
extern u32 maxPlusNum;

extern u8 temCollectionFlag;

// 换刀程序用来计数的
extern ChangeToolsSavedSteps stepsTemp;
extern u8 stepsFlag[5];

extern u8 addtime;
extern u32 CountNeedAdd;
extern u32 xAxisPlusNum,yAxisPlusNum,zAxisPlusNum,aAxisPlusNum,bAxisPlusNum;
extern u32 maxPlusNum;
u32 testNum;

extern u16 clkPerStep;												// 步进的频率
// 通过CLK频率计算预分频系数 PSC
extern u16 calPSC(u16 clk);
extern u8 savePlusNumFlag;								// 是否保存脉冲数的标签， 0：不保存， 1：保存

extern void xAxisClkSet(u16 yAxisClk);

void testIO_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);		//使能GPIOF时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//普通输入模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//上拉
  GPIO_Init(GPIOC, &GPIO_InitStructure);					//初始化GPIOF0,1,2,3,4

} 

//通用定时器6中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//定时器溢出时间计算方法:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=定时器工作频率,单位:Mhz
//这里使用的是定时器6!

void TIM6_Base_Init(u16 arr,u16 psc)
{
	  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	  
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);  			///使能TIM6时钟
	  
	  TIM_TimeBaseInitStructure.TIM_Period = arr;   				//自动重装载值
	  TIM_TimeBaseInitStructure.TIM_Prescaler=psc;					//定时器分频
	  TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	  TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	  
	  TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStructure);			//初始化TIM6
	  
	  TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE); 						//允许定时器6更新中断
	  
	  NVIC_InitStructure.NVIC_IRQChannel=TIM6_DAC_IRQn; 			//定时器6中断
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 	//抢占优先级1
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 			//子优先级3
	  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	  NVIC_Init(&NVIC_InitStructure);

	  TIM_Cmd(TIM6,DISABLE); 										//使能定时器3
}

void TIM6_DAC_IRQHandler(void)    						//  TIM6 中断
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) 		// 检查指定的TIM6中断发生与否
		{
			TIM_ClearITPendingBit(TIM6, TIM_IT_Update  );  	//清除中断
			recvFrameFlag = 0;								// 串口接收清除标志
			recvFrameState = 0;

		/*	xAxisStepFlag = 2;								//5轴未完成动作，可以重新启动；
			yAxisStepFlag = 2;
			zAxisStepFlag = 2;
			aAxisStepFlag = 2;
			bAxisStepFlag = 2;
		*/
		
			dataPreProcFlag = 1; 	//0:已完成预处理，待第二部处理；1:等待下一帧数据的预处理；

			test_REV;
		}
}

void TIM7_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);  			///使能TIM7时钟
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  					//定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 	//向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period=arr;   					//自动重装载值
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE); 						//允许定时器7更新中断
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn; 					//定时器7中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 		//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; 			//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM7,DISABLE); 											//使能定时器7
}

//定时器7中断服务函数
void TIM7_IRQHandler(void)						// 
{
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)==SET) 		//溢出中断
	{
		TIM_ClearITPendingBit(TIM7,TIM_IT_Update);			//清除中断标志位
		
		addSubTime_IRQ();																// 考虑改成内联函数或宏定义，提高运行效率  byYJY
	}
}


void TIM14_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);  			///使能TIM7时钟
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  					//定时器分频
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 	//向上计数模式
	TIM_TimeBaseInitStructure.TIM_Period=arr;   					//自动重装载值
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM14,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM14,TIM_IT_Update,ENABLE); 						//允许定时器7更新中断
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM8_TRG_COM_TIM14_IRQn; 					//定时器7中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x03; 		//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; 			//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM14,DISABLE); 											//使能定时器7
}

void TIM8_TRG_COM_TIM14_IRQHandler(void)    						//  TIM14 中断
{
	if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) 		// 检查指定的TIM14中断发生与否
		{
			TIM_ClearITPendingBit(TIM14,TIM_IT_Update);			//清除中断标志位
			// 这种方法不太好，注释掉先
//			if(stepsFlag[0])
//				stepsTemp.xSteps[0]++;														// x轴计数
//			if(stepsFlag[1])
//				stepsTemp.ySteps[0]++;														// y轴计数
//			if(stepsFlag[2])
//				stepsTemp.zSteps[0]++;														// z轴计数
		}
}

/* 原温度采集中断，注释掉
void TIM8_TRG_COM_TIM14_IRQHandler(void)    						//  TIM14 中断
{
	if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) 		// 检查指定的TIM14中断发生与否
		{
			TIM_ClearITPendingBit(TIM14,TIM_IT_Update);			//清除中断标志位
			temCollectionFlag = 1;
			test_REV;
		}
}
*/

// 启动加减速定时器
void subAddTimerStart(void)
{
	TIM7->CNT = 0;
	TIM_Cmd(TIM7, ENABLE);
}
// 关闭加减速定时器
void subAddTimerStop(void)
{
	TIM_Cmd(TIM7, DISABLE);
}

