/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          原能智能制造机器人
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : Motor.C
* By  : Hiker Lee
* 2018-04-10
********************************************************************************************************/

#include "sys.h"

//==============================
//
//	配置中断向量表及优先级
//
//==============================
void NVIC_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

#ifdef  VECT_TAB_RAM
  NVIC_SetVectorTable(NVIC_VectTab_RAM, 0x0);
#else  
  NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0);
#endif

	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;				
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void DCMotor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE); 	//使能GPIOD的时钟
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;		//输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  	//推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;  //无上下拉
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //高速GPIO
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOE,GPIO_Pin_0|GPIO_Pin_1); 	//GPIOF9,10高电平
}

void DCMotorFa(void)		//前进
{
	GPIO_SetBits(GPIOE,GPIO_Pin_0); 	
	GPIO_ResetBits(GPIOE,GPIO_Pin_1); 	
}

void DCMotorBa(void)		//后退
{
	GPIO_ResetBits(GPIOE,GPIO_Pin_0); 	
	GPIO_SetBits(GPIOE,GPIO_Pin_1); 	
}

void DCMotorBreak(void)		//刹车
{
	GPIO_SetBits(GPIOE,GPIO_Pin_0); 	
	GPIO_SetBits(GPIOE,GPIO_Pin_1); 	
}

void DCMotorStop(void)			//悬空
{
	GPIO_ResetBits(GPIOE,GPIO_Pin_0); 	
	GPIO_ResetBits(GPIOE,GPIO_Pin_1); 	
}

void Clamp(void)		//夹紧
{
	DCMotorFa();
	delay_ms(1000);
	DCMotorStop();
}

void UnClamp(void)	//松开
{
	DCMotorBa();
	delay_ms(1000);
	DCMotorStop();
}

