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
#include "beep.h"



extern unsigned char recvFrameFlag;
extern unsigned char recvFrameState;

extern u8 addSubStepSum;
extern u8 addSubStepNo;
extern u8 addSubFlag;			//0:需要定时器配置加减速；1:不需要；
extern u32 workTimeMSSum;
extern u32 workedTimeMS;

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


extern u8 addtime;
extern u32 CountNeedAdd;
extern u32 xAxisPlusNum,yAxisPlusNum,zAxisPlusNum,aAxisPlusNum,bAxisPlusNum;
extern u32 maxPlusNum;
u32 testNum;
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
void TIM7_IRQHandler(void)						//40ms
{
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)==SET) 		//溢出中断
	{
		TIM_ClearITPendingBit(TIM7,TIM_IT_Update);			//清除中断标志位
		
		LED0 = ~LED0;
//		BEEP = ~BEEP;
		if(0xfff0 > TIM7->PSC)
			TIM7->PSC += 100;

//		if(workedTimeMS == 0)
//			{
//				i=0;
//				memset(testTemp, 0, sizeof(u16)*12*5);
//			}
//		if(workedTimeMS < (0xffffffff-addtime))
//			workedTimeMS = workedTimeMS + addtime;
//		
//		if(workedTimeMS < (addSubStepSum+3)*addtime)
//		{
//			if((addSubFlag == 0)&&(addSubStepNo<addSubStepSum))		//addSubStepNo = addSubStepSum;
//				{
//					addSubStepNo = addSubStepNo + 1; 
//				/*	
//					if(addSubStepNo == addSubStepSum)
//					{
//						temp1 = 0;	//just test
//					}
//					//*/
//					speedReSet(addSubStepSum,addSubStepNo);
//					
//					//////////////////////////////////
//				/*	
//					temp1 = pwmNumTim3;
//					temp2 = pwmNumTim2;
//					temp3 = pwmNumTim4;
//					temp4 = pwmNumTim9;
//					temp5 = pwmNumTim12;
//				
//					testTemp[i][0] = temp1;
//					testTemp[i][1] = temp2;
//					testTemp[i][2] = temp3;
//					testTemp[i][3] = temp4;
//					testTemp[i][4] = temp5;

//					i++;
//				//*/	
//					motor5workstart();
//						
//					//////////////////////////////////
//					
//				}
//		}
////		else if(workedTimeMS > (workTimeMSSum - 500))
////		{
////		/*
////			temp1 = pwmNumTim3;
////			temp2 = pwmNumTim2;
////			temp3 = pwmNumTim4;
////			temp4 = pwmNumTim9;
////			temp5 = pwmNumTim12;
////		//*/

////			if((pwmNumTim3 > maxPlusNum-315)||(pwmNumTim2 > maxPlusNum-315)||(pwmNumTim4 > maxPlusNum-315)||(pwmNumTim9 > maxPlusNum-315)||(pwmNumTim12 > maxPlusNum-315))
////			{
////					if((addSubFlag == 0)&&(addSubStepNo>1)) 			//addSubStepNo = 1;
////						{
////							addSubStepNo--; 
////							speedReSet(addSubStepSum,addSubStepNo);
////							
////							//////////////////////////////////
////						/*	
////							testTemp[i][0] = temp1;
////							testTemp[i][1] = temp2;
////							testTemp[i][2] = temp3;
////							testTemp[i][3] = temp4;
////							testTemp[i][4] = temp5;
////				
////							i++;
////						//*/	
////							motor5workstart();
////								
////							//////////////////////////////////
////							
////				//			if(addSubStepNo==1)
////				//				TIM_Cmd(TIM7,DISABLE);			//			用于电机加减速处理			
////						}

////			}
////		}
//		else
//		{
//				testNum=0;
//				if(xAxisPlusNum==maxPlusNum)
//					testNum=pwmNumTim3;
//				else if(yAxisPlusNum==maxPlusNum)
//					testNum=pwmNumTim2;
//				else if(zAxisPlusNum==maxPlusNum)
//					testNum=pwmNumTim4;
//				else if(aAxisPlusNum==maxPlusNum)
//					testNum=pwmNumTim9;
//				else if(bAxisPlusNum==maxPlusNum)
//					testNum=pwmNumTim12;
//				
//				if(testNum>=maxPlusNum-CountNeedAdd)
//				{
//					if((addSubFlag == 0)&&(addSubStepNo>1)) 			//addSubStepNo = 1;
//					{
//						addSubStepNo--; 
//						speedReSet(addSubStepSum,addSubStepNo);
//						motor5workstart();
//					}	
//				}
//		}
//	//	test_REV;
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
//	GPIO_InitTypeDef GPIO_InitStructure;
//	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//	TIM_OCInitTypeDef  TIM_OCInitStructure;
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);  	//TIM14时钟使能    
//	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); 	//使能PORTF时钟	
//	
//	GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,GPIO_AF_TIM14); //GPIOF9复用为定时器14
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;           //GPIOF9
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
//	GPIO_Init(GPIOF,&GPIO_InitStructure);              //初始化PF9
//	  
//	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
//	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
//	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
//	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
//	
//	TIM_TimeBaseInit(TIM14,&TIM_TimeBaseStructure);//初始化定时器14
//	
//	//初始化TIM14 Channel1 PWM模式	 
//	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
// 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
//	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性:TIM输出比较极性低
//	TIM_OC1Init(TIM14, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM1 4OC1

//	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);  //使能TIM14在CCR1上的预装载寄存器
// 
//  TIM_ARRPreloadConfig(TIM14,ENABLE);//ARPE使能 
//	
//	TIM_Cmd(TIM14, ENABLE);  //使能TIM14
}


void TIM8_TRG_COM_TIM14_IRQHandler(void)    						//  TIM14 中断
{
	if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) 		// 检查指定的TIM14中断发生与否
		{
			TIM_ClearITPendingBit(TIM14,TIM_IT_Update);			//清除中断标志位
			temCollectionFlag = 1;
			test_REV;
		}
}

