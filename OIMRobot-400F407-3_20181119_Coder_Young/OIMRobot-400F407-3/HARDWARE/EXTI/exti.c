/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          原能智能制造机器人
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : Exit.C
* By  : Hiker Lee
* 2018-04-10
* 20180725中断管脚定义修改:PA1-->PB1;PA0-->PB0;
********************************************************************************************************/


#include "exti.h"
#include "delay.h" 
#include "led.h" 
#include "key.h"
#include "ChangeTools.h"		// 包含了换刀步数的结构体
#include "ComDataProc.h"

////////////////////////////////////////////////////////////////////////////////// 

extern u32 xAxisPlusNum,yAxisPlusNum,zAxisPlusNum,aAxisPlusNum,bAxisPlusNum;

extern nAxisSetData allAxisSetData[3];			// X、Y、Z轴设定的运动参数
extern u8 setedDataResFlag;

u32 pwmNumTim12=0;
u32 pwmNumTim9=0;
u32 pwmNumTim3=0;
u32 pwmNumTim4=0;
u32 pwmNumTim2=0;			//Y

extern u8 xAxisStepFlag; 
extern u8 yAxisStepFlag; 
extern u8 zAxisStepFlag; 
extern u8 aAxisStepFlag; 
extern u8 bAxisStepFlag; 

u8 urgentStopFlag = -1; 
// u8 i;

//外部中断0服务程序
void EXTI0_IRQHandler(void)
{
	 EXTI_ClearITPendingBit(EXTI_Line0); 	//清除LINE0上的中断标志位 
	 pwmNumTim2++;
	 if(pwmNumTim2>=yAxisPlusNum)
	 	{
			TIM2_PWM_Stop();
			pwmNumTim2 = 0;
			yAxisStepFlag = 2;
			
			// 记录最后运行的时间
			allAxisSetData[1].subLastTime = TIM7->CNT;
			TIM_Cmd(TIM7,DISABLE);												// 关闭TIM7计时
			setedDataResFlag = 0;													// 运动停止，标记置位
	 	}
}

//外部中断1服务程序
void EXTI1_IRQHandler(void)
{
	 EXTI_ClearITPendingBit(EXTI_Line1); 	//清除LINE1上的中断标志位 
	 pwmNumTim3++;
	 if(pwmNumTim3>=xAxisPlusNum)
	 	{
			TIM3_PWM_Stop();
			pwmNumTim3 = 0;					//180612 test
			xAxisStepFlag = 2;
			
			// 记录最后运行的时间
			allAxisSetData[0].subLastTime = TIM7->CNT;
			TIM_Cmd(TIM7,DISABLE);												// 关闭TIM7计时
			setedDataResFlag = 0;													// 运动停止，标记置位
	 	}
}	

//外部中断2服务程序
void EXTI2_IRQHandler(void)
{
if(EXTI_GetITStatus(EXTI_Line2)!=RESET)		//判断某个线上的中断是否发生 
	{
	EXTI_ClearITPendingBit(EXTI_Line2);    //清除LINE2上的中断标志位 
	pwmNumTim4++;
	if(pwmNumTim4>=zAxisPlusNum)
	   {
		   	TIM4_PWM_Stop();
		    pwmNumTim4 = 0;
		   	zAxisStepFlag = 2;
			 
			 // 记录最后运行的时间
			allAxisSetData[2].subLastTime = TIM7->CNT;
			TIM_Cmd(TIM7,DISABLE);												// 关闭TIM7计时
			setedDataResFlag = 0;													// 运动停止，标记置位
	   }
	}
}
//外部中断3服务程序
void EXTI3_IRQHandler(void)
{
	 EXTI_ClearITPendingBit(EXTI_Line3);  	//清除LINE3上的中断标志位  
	 pwmNumTim12++;
	 if(pwmNumTim12>=bAxisPlusNum)
	 	{
			TIM12_PWM_Stop();
			pwmNumTim12 = 0;
			bAxisStepFlag = 2;
	 	}
}
//外部中断4服务程序
void EXTI4_IRQHandler(void)
{ 
	 EXTI_ClearITPendingBit(EXTI_Line4);	//清除LINE4上的中断标志位  
	 pwmNumTim9++;
	 if(pwmNumTim9>=aAxisPlusNum)
	 	{
			TIM9_PWM_Stop();
			pwmNumTim9 = 0;
			aAxisStepFlag = 2;
	 	}
}

u8 UrgentStopTest(void)
{
	u8 temp,temp2;
	delay_ms(3);
	temp = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_5);
	delay_ms(2);
	temp2 = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_5);		//180625

	if(temp == temp2)
	{
		if(temp == 0)		//按键按下		
		{
			return 0;
		}
		else
			return 1;		//按键弹起
	}
	else
		return 2;			//状态变化，需重新检测；
	
}
//外部中断5服务程序
void EXTI9_5_IRQHandler(void)
{ 
	u8 temp,temp2;
	if(EXTI_GetITStatus(EXTI_Line5)!=RESET) 		//急停信号
	{
		EXTI_ClearITPendingBit(EXTI_Line5); 		//清除中断
		delay_ms(3);
		temp = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_5);
		delay_ms(2);
		temp2 = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_5);		//180625
		if(temp == temp2)
		{
			if(temp == 0)		//按键按下			
			{
				mainMotorStop();
				
				TIM_Cmd(TIM3, DISABLE); 	//使能x
				TIM_Cmd(TIM4, DISABLE); 	//使能z
				TIM_Cmd(TIM9, DISABLE); 	//使能a
				TIM_Cmd(TIM12, DISABLE);	//使能b
				TIM_Cmd(TIM5, DISABLE);		//使能y
				TIM_Cmd(TIM2, DISABLE);
				
				urgentStopFlag = 1;
			}
			else			//按键弹起		//urgentStopFlag = 3;			//按键按下后处理完毕
			{
				if(urgentStopFlag == 3)
				urgentStopFlag = 0;
			}
		}
		
	}	
	if(EXTI_GetITStatus(EXTI_Line6)!=RESET) 		//解码信号
	{
		EXTI_ClearITPendingBit(EXTI_Line6); 		//清除中断
		delay_us(100);
		temp = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_6);
		delay_us(1000);
		temp2 = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_6);
		
		if(temp == temp2)
		{
		if(temp == 0)
			{
			mainMotorDecode();
			delay_us(100);			//手动解码，上位机控制主轴启停；
			mainMotorWork();		//hiker 0522 test
			}
		}
	}
	
	if(EXTI_GetITStatus(EXTI_Line7)!=RESET) 		
	{
		EXTI_ClearITPendingBit(EXTI_Line7); 		//清除中断
	}	
	if(EXTI_GetITStatus(EXTI_Line8)!=RESET) 		
	{
		EXTI_ClearITPendingBit(EXTI_Line8); 		//清除中断
	}	
	if(EXTI_GetITStatus(EXTI_Line9)!=RESET) 		
	{
		EXTI_ClearITPendingBit(EXTI_Line9); 		//清除中断
	}	
}

void EXTIIO_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);		//使能GPIOE时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;				//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//上拉
  GPIO_Init(GPIOE, &GPIO_InitStructure);					//初始化GPIOE0,1,2,3,4

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);		//使能GPIOA时钟
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;					//x轴中断输入
  GPIO_Init(GPIOB, &GPIO_InitStructure);					//初始化GPIOA1

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);		//使能GPIOG时钟
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;			//5急停信号,6解码信号
  GPIO_Init(GPIOG, &GPIO_InitStructure);						//初始化GPIOG5,6
} 

void motorDirIO_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);		//使能GPIOF时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//普通输入模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//上拉
  GPIO_Init(GPIOF, &GPIO_InitStructure);					//初始化GPIOF0,1,2,3,4

} 
	   
//外部中断初始化程序
//初始化PE0~4为中断输入.
void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	EXTIIO_Init(); 	//按键对应的IO口初始化
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);		//使能SYSCFG时钟
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);　
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);		//PB0 连接到中断线0
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);		//PB1 连接到中断线1
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);		//PE2 连接到中断线2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);		//PE3 连接到中断线3
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);		//PE4 连接到中断线4

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource5);		//PG5 连接到中断线5
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource6);		//PG6 连接到中断线6
		
	// 配置EXTI_Line0,1,2,3,4 
	EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1 | EXTI_Line2 | EXTI_Line3 | EXTI_Line4| EXTI_Line6;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//中断事件
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//中断线使能
    EXTI_Init(&EXTI_InitStructure);							//配置

	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;				//中断事件
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; 	//下降沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;						//中断线使能
    EXTI_Init(&EXTI_InitStructure);									//配置
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//外部中断0
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);								//配置

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//外部中断0			x
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);								//配置

	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//外部中断2			z
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级3
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;		//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);								//配置
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			//外部中断3			b
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级2
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);								//配置
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//外部中断4			a
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;		//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);								//配置

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//外部中断4
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 			//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure); 							//配置

}


void disableZaxisCountINT()
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//外部中断2
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级3
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;		//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;			//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);								//配置

}

void enableZaxisCountINT()
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//外部中断2
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级3
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;		//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);								//配置

}

void disableXaxisCountINT()
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//外部中断0
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;			//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);								//配置

}

void enableXaxisCountINT()
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//外部中断0
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//抢占优先级0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);								//配置

}

void disableYaxisCountINT()
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//外部中断0
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;			//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);								//配置

}

void enableYaxisCountINT()
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//外部中断0
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占优先级0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		//子优先级2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
  	NVIC_Init(&NVIC_InitStructure);								//配置

}




