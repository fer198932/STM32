/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          原能智能制造机器人
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F407ZG
*                                              
* File : Main.C
* By  : Young
* 2018-12-24
*
*重写下位机控制程序 2018年12月24日15:45:07
********************************************************************************************************/
#include "main.h"

#if _TEST_ON_ALIENTEK
void Delay(__IO uint32_t nCount);

void Delay(__IO uint32_t nCount)
{
  while(nCount--){}
}
#endif

int main(void)
{
#if _TEST_ON_ALIENTEK
	
	u32 t=0;
	
	Sys_Init();		// 系统初始化函数
	Sys_Enable();	// 系统使能函数
	
  while(1){
		procLimit();							// 循环扫描是否有限位发生
		
//    printf("t:%d\r\n",t);
		delay_ms(500);
		t++;
		
//		if(X_P_Lim())
//			printf("没有限位\r\n");
//		else
//			printf("限位\r\n");
		
//		LED0 = t%2;
		DMA_USART_SEND(41);
		while(1)
		    {
				if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)//等待DMA2_Steam7传输完成
				{ 
					DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);//清除DMA2_Steam7传输完成标志
					break; 
		        } 
		    }	
	}
  
#endif
}


// 系统初始化函数
void Sys_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);					// 中断分组配置
	uart_init(BOUND_X);
	DMA_USART_Init();		// 串口的DMA传输方式
	delay_init(_SYSCLK);
	LED_Init();
	limit_IO_Init();		
}

// 系统各外设使能函数，如LED使能、中断使能等
void Sys_Enable(void)
{
//	USART_Cmd(USART_X, ENABLE);			// 串口使能	
	///////////// 还有各种中断使能  byYJY ///////////////
	USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);  // 使能串口1的DMA发送     
}







