/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          原能智能制造机器人
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F407ZG （仅限该芯片！！）
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
		/* 循环扫描是否有限位发生  */
		procLimit();		

		/*  检查BUFFER是否需要重置 */
		checkBuffer();						
		
		/* 如果发生了串口的DMA溢出，重新初始化 */
		if(ERROR == USARTRx_DMAOut_Flag) 
		{
			DMA_USART_Init();
			USARTRx_DMAOut_Flag = SUCCESS;
		}
		
		/* 根据串口发送的命令进行处理  */
		if(IS_OK == USARTRx_IfOK_Flag) 		
		{
			procDataStep();		
			USARTRx_IfOK_Flag = NOT_OK; 		// 指令处理完后重新处理下一条指令
		}
		
		
		
		// 换向测试
//		if(t%2)
//			XAxisDir_Pos;
//		else
//			XAxisDir_Neg;
		
//    printf("t:%d\r\n",t);
//		delay_ms(500);
		t++;
		
//		TIM_Cmd(TIM7, ENABLE);
//		if(X_P_Lim())
//			printf("没有限位\r\n");
//		else
//			printf("限位\r\n");
		
//		LED0 = t%2;

	}
  
#endif
}


// 系统初始化函数 （初始化函数尽量只调用一次，因为野指针的存在可能导致系统问题）
void Sys_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);					// 中断分组配置
	buffer_Init();					// 数据缓冲区初始化
	comData_Init();
	uart_init(BOUND_X);
	DMA_USART_Init();		// 串口的DMA传输方式初始化
	delay_init(_SYSCLK);
	LED_Init();
	limit_IO_Init();
	Timer_Init();	
	PWM_Init();
	EXTI_Config_Init();				// 中断IO口初始化 中断最后初始化
	StepMotor_Init();				// 步进电机方向IO口初始化
}

// 系统各外设使能函数，如LED使能、中断使能等
void Sys_Enable(void)
{
//	USART_Cmd(USART_X, ENABLE);			// 串口使能	
	///////////// 还有各种中断使能  byYJY ///////////////
	
	/* DMA 串口使能 */
	USART_DMACmd(USART_X, USART_DMAReq_Tx, ENABLE);  // 使能串口的DMA发送     
	USART_DMACmd(USART_X, USART_DMAReq_Rx, ENABLE);  // 使能串口的DMA接收
	
	/* 中断使能 */
	EXTI_Enable();
	
	/* 使能加减速时钟， 测试用，可删除  */
	TIM_Cmd(TIM7, ENABLE);
	
	/* 使能pwm， 测试用，可删除  */
	PWM_Cmd(X_PWM, ENABLE, X_CH_EXTI);
	PWM_Cmd(X_PWM, ENABLE, X_CH_OUT);
}







