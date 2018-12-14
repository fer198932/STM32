/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          ԭ���������������
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : Timer.C
* By  : Hiker Lee
* 2018-05-11
* 2018-06-02 �޸�IO
********************************************************************************************************/

#include "timer.h"
#include "led.h"
#include "SysInit.h"

//Ť�ݶ�	Twist Screw
//#define mTSDIR_LOCK 	GPIO_SetBits(GPIOA,GPIO_Pin_8)
//#define mTSDIR_UNLOCK 	GPIO_ResetBits(GPIOA,GPIO_Pin_8)
#define mTSDIR_LOCK 	GPIO_ResetBits(GPIOD,GPIO_Pin_7)
#define mTSDIR_UNLOCK 	GPIO_SetBits(GPIOD,GPIO_Pin_7)

//��������	CHANGE TOOL
#define mCTDIR_LOCK 	GPIO_ResetBits(GPIOB,GPIO_Pin_13)	
#define mCTDIR_UNLOCK 	GPIO_SetBits(GPIOB,GPIO_Pin_13)
//���ƽ� 	TARGET PUSH
#define mTPDIR_LOCK 	GPIO_ResetBits(GPIOE,GPIO_Pin_1)
#define mTPDIR_UNLOCK 	GPIO_SetBits(GPIOE,GPIO_Pin_1)

//Ť�ݶ�pwm
#define mTSPWM_HIGH 	GPIO_SetBits(GPIOA,GPIO_Pin_4)
#define mTSPWM_LOW 		GPIO_ResetBits(GPIOA,GPIO_Pin_4)
//��������pwm
#define mCTPWM_HIGH 	GPIO_SetBits(GPIOB,GPIO_Pin_12)	
#define mCTPWM_LOW 		GPIO_ResetBits(GPIOB,GPIO_Pin_12)
//���ƽ�PWM
#define mTPPWM_HIGH 	GPIO_SetBits(GPIOE,GPIO_Pin_0)
#define mTPPWM_LOW 		GPIO_ResetBits(GPIOE,GPIO_Pin_0)

#define changeToolLockPOS 	GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_4)
#define autoLOCKZeroPOS 	GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_8)

void motorIO_Init(void)			//ʣ��3���������IO�ڳ�ʼ��
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);		//ʹ��GPIOFʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//����
  GPIO_Init(GPIOA, &GPIO_InitStructure);					//��ʼ��GPIOF0,1,2,3,4

  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);		//ʹ��GPIOFʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//����
  GPIO_Init(GPIOD, &GPIO_InitStructure);					//��ʼ��GPIOF0,1,2,3,4

  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);		//ʹ��GPIOFʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 |GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//����
  GPIO_Init(GPIOB, &GPIO_InitStructure);					//��ʼ��GPIOF0,1,2,3,4

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);		//ʹ��GPIOFʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);					//��ʼ��GPIOF0,1,2,3,4

  //����ʹ��λ�ô�������
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);		//ʹ��GPIOGʱ��
 
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;	  //��1-4���ѵ������λ
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;			  //����
  GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;			  //��������
  GPIO_Init(GPIOG,&GPIO_InitStructure); 				  //��ʼ��GPIOA
} 

void motorTwistScrewLock(void)
{
	u16 i;
	//FANGXIANG 
	mTSDIR_LOCK;

	for(i=0;i<14000;i++)
	{
		mTSPWM_LOW;
		delay_us(500);
		mTSPWM_HIGH;
		delay_us(500);
	}
}

void motorTwistScrewUnLock(void)
{
	u16 i;
	//FANGXIANG 
	mTSDIR_UNLOCK;

	for(i=0;i<14000;i++)
	{
		mTSPWM_LOW;
		delay_ms(1);
		mTSPWM_HIGH;
		delay_ms(1);
	}
}

void motorChangeToolsLock(void)		//����֤����1000��ȫ���н�ok
{
	u16 i;
	//����
	mCTDIR_LOCK;
	
	for(i=0;i<1000;i++)
	{
		mCTPWM_LOW;
		delay_us(100);
		mCTPWM_HIGH;
		delay_us(100);
	}
}

void motorChangeToolsUnLock(void)	//�ɿ������
{
	u16 i=0;
	//����
	mCTDIR_UNLOCK;
	
	for(i=0;i<3000;i++)
	{
		if(changeToolLockPOS == 1)
		{
			mCTPWM_LOW;
			delay_us(100);
			mCTPWM_HIGH;
			delay_us(100);
		}
		else
		{
			return;
		}
	}
}

void motorTargetPUSH(void)	//����㿪ʼ�� ���ƽ�3800�����ʣ�
{
	u16 i;
	//����
	mTPDIR_LOCK;
	
	for(i=0;i<3800;i++)
	{
		mTPPWM_LOW;
		delay_us(100);
		mTPPWM_HIGH;
		delay_us(100);
	}
}


void motorTargetBack(void)	//�������
{
	u16 i=0;
	
	//����
	mTPDIR_UNLOCK;
	for(i=0;i<5000;i++)
	{
		if(autoLOCKZeroPOS == 1)
		{
			mTPPWM_LOW;
			delay_us(100);
			mTPPWM_HIGH;
			delay_us(100);
		}
		else
		{
			return;
		}
	}

}

