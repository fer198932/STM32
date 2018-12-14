/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          原能智能制造机器人
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : Pwmint.C
* By  : Hiker Lee
* 2018-04-26
********************************************************************************************************/

#include "pwmint.h"
#include "led.h"
#include "usart.h"

//TIM12 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM12_PWM_Init(u32 arr,u32 psc)			//b轴
{		 					 
	//此部分需手动修改IO口设置
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM_Cmd(TIM12, DISABLE);  			//使能TIM9

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12,ENABLE);  	//TIM14时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//使能PORTF时钟	
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_TIM12); //GPIOF9复用为定时器14
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_TIM12); //GPIOF9复用为定时器14
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;        //上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure);              	//初始化PF9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM12,&TIM_TimeBaseStructure);//初始化定时器14
	
	//初始化TIM14 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 		//输出极性:TIM输出比较极性低
	TIM_OC1Init(TIM12, &TIM_OCInitStructure); 				 		//根据T指定的参数初始化外设TIM1 4OC1
	TIM_OC1PreloadConfig(TIM12, TIM_OCPreload_Enable); 				//使能TIM14在CCR1上的预装载寄存器

 
 	TIM_OC2Init(TIM12, &TIM_OCInitStructure);			//根据T指定的参数初始化外设TIM1 4OC1
 	TIM_OC2PreloadConfig(TIM12, TIM_OCPreload_Enable);  //使能TIM14在CCR1上的预装载寄存器

  	TIM_ARRPreloadConfig(TIM12,ENABLE);		//ARPE使能 
  	
	TIM_SetCompare1(TIM12,(arr>>1));		//修改比较值，修改占空比
	TIM_SetCompare2(TIM12,(arr>>1));		//修改比较值，修改占空比
	
	TIM_Cmd(TIM12, DISABLE);  				//使能TIM12
 
										  
}  
void TIM12_PWM_Stop(void)
{
	TIM_Cmd(TIM12, DISABLE);  				//使能TIM12
}



//TIM9 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM9_PWM_Init(u32 arr,u32 psc)			//a轴
{		 					 
	//此部分需手动修改IO口设置
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM_Cmd(TIM9, DISABLE);  			//使能TIM9
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);  	//TIM9时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 	//使能PORTF时钟	
	
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource5,GPIO_AF_TIM9); 	//GPIOF9复用为定时器9
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource6,GPIO_AF_TIM9); 	//GPIOF9复用为定时器9
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;        //上拉				//180612 test
	GPIO_Init(GPIOE,&GPIO_InitStructure);              	//初始化PF9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  					//定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 	//向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   					//自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);				//初始化定时器9
	
	//初始化TIM14 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 					//选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	 	//比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 			//输出极性:TIM输出比较极性低
	TIM_OC1Init(TIM9, &TIM_OCInitStructure);  							//根据T指定的参数初始化外设TIM1 4OC1
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);  					//使能TIM14在CCR1上的预装载寄存器

 
 	TIM_OC2Init(TIM9, &TIM_OCInitStructure);			//根据T指定的参数初始化外设TIM1 4OC1
 	TIM_OC2PreloadConfig(TIM9, TIM_OCPreload_Enable);  	//使能TIM14在CCR1上的预装载寄存器

  	TIM_ARRPreloadConfig(TIM9,ENABLE);	//ARPE使能 

	TIM_SetCompare1(TIM9,(arr>>1));		//修改比较值，修改占空比
	TIM_SetCompare2(TIM9,(arr>>1));		//修改比较值，修改占空比
	
	TIM_Cmd(TIM9, DISABLE);  			//使能TIM9
 
										  
}  

void TIM9_PWM_Stop(void)
{
	TIM_Cmd(TIM9, DISABLE);  			//使能TIM9
}


//TIM3 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM3_PWM_Init(u32 arr,u32 psc)		//X轴
{		 					 
	//此部分需手动修改IO口设置
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM_Cmd(TIM3, DISABLE); 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  	//TIM3时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//使能PORTF时钟	
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3); //GPIOF9复用为定时器14
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;        //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure);              	//初始化PF9

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//使能PORTF时钟	
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_TIM3); //GPIOF9复用为定时器14
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;        //上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure);              	//初始化PF9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);//初始化定时器14
	
	//初始化TIM14 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 		//输出极性:TIM输出比较极性低
	TIM_OC1Init(TIM3, &TIM_OCInitStructure); 				 		//根据T指定的参数初始化外设TIM1 4OC1
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable); 				//使能TIM14在CCR1上的预装载寄存器

 
 	TIM_OC2Init(TIM3, &TIM_OCInitStructure);			//根据T指定的参数初始化外设TIM1 4OC1
 	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM14在CCR1上的预装载寄存器

  	TIM_ARRPreloadConfig(TIM3,ENABLE);		//ARPE使能 
  	
	TIM_SetCompare1(TIM3,(arr>>1));		//修改比较值，修改占空比
	TIM_SetCompare2(TIM3,(arr>>1));		//修改比较值，修改占空比
	
	TIM_Cmd(TIM3, DISABLE);  				//使能TIM12
										  
}  

void TIM3_PWM_Stop(void)
{
	TIM_Cmd(TIM3, DISABLE);  			//使能TIM9
}



//TIM4 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM4_PWM_Init(u32 arr,u32 psc)			//z轴
{		 					 
	//此部分需手动修改IO口设置
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM_Cmd(TIM4, DISABLE);  				//使能TIM12
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  	//TIM3时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//使能PORTF时钟	
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_TIM4); //GPIOF9复用为定时器14
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_TIM4); //GPIOF9复用为定时器14
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;        //上拉
	GPIO_Init(GPIOB,&GPIO_InitStructure);              	//初始化PF9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  			//定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   			//自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);//初始化定时器14
	
	//初始化TIM14 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 		//输出极性:TIM输出比较极性低
	TIM_OC3Init(TIM4, &TIM_OCInitStructure); 				 		//根据T指定的参数初始化外设TIM1 4OC1
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable); 				//使能TIM14在CCR1上的预装载寄存器

 
 	TIM_OC4Init(TIM4, &TIM_OCInitStructure);			//根据T指定的参数初始化外设TIM1 4OC1
 	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  //使能TIM14在CCR1上的预装载寄存器

  	TIM_ARRPreloadConfig(TIM4,ENABLE);		//ARPE使能 
  	
	TIM_SetCompare3(TIM4,(arr>>1));		//修改比较值，修改占空比
	TIM_SetCompare4(TIM4,(arr>>1));		//修改比较值，修改占空比
//	TIM_SetCompare3(TIM4,150); 	//修改比较值，修改占空比
//	TIM_SetCompare4(TIM4,150); 	//修改比较值，修改占空比
	
	TIM_Cmd(TIM4, DISABLE);  				//使能TIM12
										  
}  


void TIM4_PWM_Stop(void)
{
	TIM_Cmd(TIM4, DISABLE);  			//使能TIM9
}




//TIM4 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM2_PWM_Init(u32 arr,u32 psc)			//Y
{		 					 
	//此部分需手动修改IO口设置
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM_Cmd(TIM2, DISABLE);  				//使能TIM2
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  	//TIM2时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//使能PORTF时钟	
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM2); //GPIOF9复用为定时器14
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_TIM2); //GPIOF9复用为定时器14
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_3;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;        //上拉
	GPIO_Init(GPIOA,&GPIO_InitStructure);              	//初始化PF9
 
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  			//定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   			//自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);//初始化定时器14
	
	//初始化TIM14 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 		//输出极性:TIM输出比较极性低
	TIM_OC4Init(TIM2, &TIM_OCInitStructure); 				 		//根据T指定的参数初始化外设TIM1 4OC1
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable); 				//使能TIM14在CCR1上的预装载寄存器

 
 	TIM_OC1Init(TIM2, &TIM_OCInitStructure);			//根据T指定的参数初始化外设TIM1 4OC1
 	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //使能TIM14在CCR1上的预装载寄存器

  	TIM_ARRPreloadConfig(TIM2,ENABLE);		//ARPE使能 
  	
	TIM_SetCompare1(TIM2,(arr>>1));		//修改比较值，修改占空比
	TIM_SetCompare4(TIM2,(arr>>1));		//修改比较值，修改占空比
	
	TIM_Cmd(TIM2, DISABLE);  				//使能TIM12
										  
}  


void TIM2_PWM_Stop(void)
{
	TIM_Cmd(TIM2, DISABLE);  			//使能TIM9
}

void xAxisClkSet(u16 xAxisClk)
{
	u16 nPDTemp;
	nPDTemp = 300000UL/xAxisClk;
	TIM3_PWM_Init(nPDTemp-1,280-1);			//84M/2800=30khz的计数频率,重装载值1000，所以PWM频率为 30k/1000=30hz. 
}

void yAxisClkSet(u16 yAxisClk)
{
	u16 nPDTemp;
	nPDTemp = 300000UL/yAxisClk;
	TIM2_PWM_Init(nPDTemp-1,280-1);			//84M/2800=30khz的计数频率,重装载值1000，所以PWM频率为 30k/1000=30hz. 
}


void zAxisClkSet(u16 zAxisClk)
{
	u16 nPDTemp;
	nPDTemp = 300000UL/zAxisClk;
	TIM4_PWM_Init(nPDTemp-1,280-1);			//84M/84=1Mhz的计数频率,重装载值500，所以PWM频率为 1M/500=2Khz. 	
//	TIM4_PWM_Init(300-1,84-1);			//84M/84=1Mhz的计数频率,重装载值500，所以PWM频率为 1M/500=2Khz. 	
}

void aAxisClkSet(u16 aAxisClk)
{
	u16 nPDTemp;
	nPDTemp = 600000UL/aAxisClk;
	TIM9_PWM_Init(nPDTemp-1,280-1);			//42M/2800=15khz的计数频率,重装载值1000，所以PWM频率为 30k/1000=30hz. 
}

void bAxisClkSet(u16 bAxisClk)
{
	u16 nPDTemp;
	nPDTemp = 300000UL/bAxisClk;
	TIM12_PWM_Init(nPDTemp-1,280-1);			//84M/84=1Mhz的计数频率,重装载值500，所以PWM频率为 1M/500=2Khz. 	
}


/////////////////////////////////////// 主轴电机驱动	/////////////////////////////////////
//TIM14 PWM部分初始化 
//PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM14_PWM_Init(u32 arr,u32 psc)
{		 					 
	//此部分需手动修改IO口设置
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	TIM_Cmd(TIM14, DISABLE);  

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);  	//TIM14时钟使能    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); 	//使能PORTF时钟	
	
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,GPIO_AF_TIM14); //GPIOF9复用为定时器14
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
	GPIO_Init(GPIOF,&GPIO_InitStructure);              	//初始化PF9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  			//定时器分频
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=arr;   			//自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM14,&TIM_TimeBaseStructure);		//初始化定时器14
	
	//初始化TIM14 Channel1 PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 		//输出极性:TIM输出比较极性低   //TIM_OCPolarity_Low
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);  						//根据T指定的参数初始化外设TIM1 4OC1

	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);  			//使能TIM14在CCR1上的预装载寄存器
 
  	TIM_ARRPreloadConfig(TIM14,ENABLE);								//ARPE使能 
	
	TIM_Cmd(TIM14, DISABLE);  										//使能TIM14
 
										  
}  

void mainMotorDecode(void)
{
	TIM14_PWM_Init(200-1,8400-1); 		//84M/8400=10Khz的计数频率,重装载值200，所以PWM频率为 10Khz/200=50hz.   
	TIM_Cmd(TIM14, ENABLE);  			//使能TIM14
	TIM_SetCompare1(TIM14,20);			//修改比较值，修改占空比 10%
	delay_ms(2000);	 

	TIM_SetCompare1(TIM14,10);			//修改比较值，修改占空比 10%
	delay_ms(1000);	 
	
	TIM_Cmd(TIM14, DISABLE);  										
}

void mainMotorWork(void)
{
	TIM14_PWM_Init(200-1,8400-1); 		//84M/8400=10Khz的计数频率,重装载值200，所以PWM频率为 10Khz/200=50hz.   
	TIM_Cmd(TIM14, ENABLE);  			//使能TIM14
	TIM_SetCompare1(TIM14,24);			//修改比较值，修改占空比 12%
}

void mainMotorStop(void)
{
	TIM_Cmd(TIM14, DISABLE);  										
}

