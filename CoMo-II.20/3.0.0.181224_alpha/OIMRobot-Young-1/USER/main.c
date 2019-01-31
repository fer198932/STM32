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

/* 全局变量 (主要为各种标志位) */
volatile 	FlagStatus 	DMA_Out_Flag 	= RESET;								// DMA溢出，重新初始化
volatile 	FlagStatus 	USART_IDLE_Flag 	= RESET; 						// 串口空闲中断标志，串口数据处理
volatile 	FunctionalState 	Cmd_Done_Flag	= ENABLE;					// 指令处理完成标志
volatile	FunctionalState 	Cmd_Copied_Falg = ENABLE; 			// 串口数据是否可复制的标签（命令执行完后才可)


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
		if(SET == DMA_Out_Flag) 
		{
			DMA_USART_Init();
			DMA_Out_Flag = RESET;
		}
		
		/* 发生串口空闲中断并且可以进行数据复制时才进入  */
		if((SET == USART_IDLE_Flag)	&& (ENABLE == Cmd_Copied_Falg))
		{
			Cmd_Copied_Falg = DISABLE; 				// 命令复制完成前不允许再处理数据
			UsartDataProc();		
			USART_IDLE_Flag = RESET; 		
		}
		
		/* 执行命令 */
		if(ENABLE == Cmd_Done_Flag)
		{
			Cmd_Done_Flag = DISABLE; 			// 执行完成前不许再次进入
			CMD_Execute();
			// 执行完成在其它地方判断
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







