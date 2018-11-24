/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          ԭ���������������
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
//	�����ж����������ȼ�
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
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE,ENABLE); 	//ʹ��GPIOD��ʱ��
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;		//���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  	//�������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;  //��������
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //����GPIO
	GPIO_Init(GPIOE,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIOE,GPIO_Pin_0|GPIO_Pin_1); 	//GPIOF9,10�ߵ�ƽ
}

void DCMotorFa(void)		//ǰ��
{
	GPIO_SetBits(GPIOE,GPIO_Pin_0); 	
	GPIO_ResetBits(GPIOE,GPIO_Pin_1); 	
}

void DCMotorBa(void)		//����
{
	GPIO_ResetBits(GPIOE,GPIO_Pin_0); 	
	GPIO_SetBits(GPIOE,GPIO_Pin_1); 	
}

void DCMotorBreak(void)		//ɲ��
{
	GPIO_SetBits(GPIOE,GPIO_Pin_0); 	
	GPIO_SetBits(GPIOE,GPIO_Pin_1); 	
}

void DCMotorStop(void)			//����
{
	GPIO_ResetBits(GPIOE,GPIO_Pin_0); 	
	GPIO_ResetBits(GPIOE,GPIO_Pin_1); 	
}

void Clamp(void)		//�н�
{
	DCMotorFa();
	delay_ms(1000);
	DCMotorStop();
}

void UnClamp(void)	//�ɿ�
{
	DCMotorBa();
	delay_ms(1000);
	DCMotorStop();
}

