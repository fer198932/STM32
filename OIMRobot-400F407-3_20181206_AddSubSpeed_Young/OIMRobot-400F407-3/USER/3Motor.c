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
* 2018-05-11
* 2018-06-02 修改IO
********************************************************************************************************/

#include "timer.h"
#include "led.h"
#include "SysInit.h"

//扭螺钉	Twist Screw
//#define mTSDIR_LOCK 	GPIO_SetBits(GPIOA,GPIO_Pin_8)
//#define mTSDIR_UNLOCK 	GPIO_ResetBits(GPIOA,GPIO_Pin_8)
#define mTSDIR_LOCK 	GPIO_ResetBits(GPIOD,GPIO_Pin_7)
#define mTSDIR_UNLOCK 	GPIO_SetBits(GPIOD,GPIO_Pin_7)

//换刀锁紧	CHANGE TOOL
#define mCTDIR_LOCK 	GPIO_ResetBits(GPIOB,GPIO_Pin_13)	
#define mCTDIR_UNLOCK 	GPIO_SetBits(GPIOB,GPIO_Pin_13)
//物推进 	TARGET PUSH
#define mTPDIR_LOCK 	GPIO_ResetBits(GPIOE,GPIO_Pin_1)
#define mTPDIR_UNLOCK 	GPIO_SetBits(GPIOE,GPIO_Pin_1)

//扭螺钉pwm
#define mTSPWM_HIGH 	GPIO_SetBits(GPIOA,GPIO_Pin_4)
#define mTSPWM_LOW 		GPIO_ResetBits(GPIOA,GPIO_Pin_4)
//换刀锁紧pwm
#define mCTPWM_HIGH 	GPIO_SetBits(GPIOB,GPIO_Pin_12)	
#define mCTPWM_LOW 		GPIO_ResetBits(GPIOB,GPIO_Pin_12)
//物推进PWM
#define mTPPWM_HIGH 	GPIO_SetBits(GPIOE,GPIO_Pin_0)
#define mTPPWM_LOW 		GPIO_ResetBits(GPIOE,GPIO_Pin_0)

#define changeToolLockPOS 	GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_4)
#define autoLOCKZeroPOS 	GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_8)

void motorIO_Init(void)			//剩余3个电机的邋IO口初始化
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);		//使能GPIOF时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 |GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//普通输入模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);					//初始化GPIOF0,1,2,3,4

  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);		//使能GPIOF时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//普通输入模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);					//初始化GPIOF0,1,2,3,4

  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);		//使能GPIOF时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 |GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//普通输入模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//上拉
  GPIO_Init(GPIOB, &GPIO_InitStructure);					//初始化GPIOF0,1,2,3,4

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);		//使能GPIOF时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//普通输入模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);					//初始化GPIOF0,1,2,3,4

  //以下使能位置传感器；
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);		//使能GPIOG时钟
 
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_8|GPIO_Pin_9;	  //刀1-4，脱刀电机限位
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;			  //输入
  GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;			  //上拉输入
  GPIO_Init(GPIOG,&GPIO_InitStructure); 				  //初始化GPIOA
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

void motorChangeToolsLock(void)		//已验证步数1000，全部夹紧ok
{
	u16 i;
	//方向
	mCTDIR_LOCK;
	
	for(i=0;i<1000;i++)
	{
		mCTPWM_LOW;
		delay_us(100);
		mCTPWM_HIGH;
		delay_us(100);
	}
}

void motorChangeToolsUnLock(void)	//松开是零点
{
	u16 i=0;
	//方向
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

void motorTargetPUSH(void)	//从零点开始推 ，推进3800步合适；
{
	u16 i;
	//方向
	mTPDIR_LOCK;
	
	for(i=0;i<3800;i++)
	{
		mTPPWM_LOW;
		delay_us(100);
		mTPPWM_HIGH;
		delay_us(100);
	}
}


void motorTargetBack(void)	//返回零点
{
	u16 i=0;
	
	//方向
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

