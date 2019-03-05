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


extern	 BacklashCompensation_Structure		backlashCompen;

/* 全局变量 (主要为各种标志位) */
Flag_Structure 	flag_Struct;
//volatile 	FlagStatus 	DMA_Out_Flag 	= RESET;								// DMA溢出，重新初始化
//volatile 	FlagStatus 	USART_IDLE_Flag 	= RESET; 						// 串口空闲中断标志，串口数据处理
//volatile 	FunctionalState 	Cmd_Execute_Flag	= DISABLE;					// 指令处理完成标志
//volatile	FunctionalState 	Cmd_Copied_Falg = ENABLE; 			// 串口数据是否可复制的标签（命令执行完后才可)

volatile 	FunctionalState 	Offline_Work_Flag = DISABLE; 		// 进入脱机加工的标记
volatile 	u32 	Offline_Data_Num = 0;					// 脱机加工数据的编号


#if _TEST_ON_ALIENTEK
void Delay(__IO uint32_t nCount);

void Delay(__IO uint32_t nCount)
{
  while(nCount--){}
}
#endif

int main(void)
{	
	u32 t=0;
	u8 urgentStopTemp1, urgentStopTemp2;
	
	
	Sys_Init();		// 系统初始化函数
	Sys_Enable();	// 系统使能函数
	
	// test
//	while(1)
//	{
//		StepMotor_Move(X_Axis, 5000, 4000, POS_DIR);
//		delay_ms(5000);
//	}
	
  while(1){
		/* 循环扫描是否有限位发生  */
		procLimit();		
		
		
		/* 发生串口空闲中断 */
		if(SET == flag_Struct.USART_IDLE_Flag)
		{
			flag_Struct.USART_IDLE_Flag = RESET;
			
			if(RESET == flag_Struct.Cmd_ProcDone_Flag)
			{
				flag_Struct.Cmd_ProcDone_Flag = SET;
				
				UsartDataProc();
				
				CMD_Proc();		

				
			}
			else
			{
				respMsgError("前一条命令还未处理完毕！\r\n", 1);				
				continue;
			}
		}
		
		if((SET == flag_Struct.Cmd_ProcDone_Flag) && (RESET == flag_Struct.Cmd_Executing_Flag) && (flag_Struct.UrgentStop_Flag != UrgentStop_Locked))
		{
			flag_Struct.Cmd_Execute_En = ENABLE;
		}
		
		
		/* 执行命令  */
		if(ENABLE == flag_Struct.Cmd_Execute_En)
		{
			flag_Struct.Cmd_Execute_En = DISABLE;
			
			flag_Struct.Cmd_Executing_Flag = SET;
			
			CMD_Execute();					
		}
		
		
		/* 限位处理 */
//		if(0 != flag_Struct.Limiti_Flag)
//		{
//			StepMotor_Limited(flag_Struct.Limiti_Flag);
//		}
		
		/* 急停按钮处理  */
		if(UrgentStop_Locked == flag_Struct.UrgentStop_Flag)
		{
			UrgentStop_Proc_Macro;
		}
		
		
		
//		/* 发生串口空闲中断时进入  */
//		if(SET == flag_Struct.USART_IDLE_Flag)
//		{	
//			/* 命令处理完毕  */
//			if(RESET == flag_Struct.Cmd_ProcDone_Flag)
//			{
//				flag_Struct.USART_IDLE_Flag = RESET;
//				
//				
//				UsartDataProc();
//				
//				CMD_Proc();
//				
//				flag_Struct.Cmd_ProcDone_Flag = SET;
//			}
//			
//			/*  执行完命令后进入  */
//			if(RESET == flag_Struct.Cmd_Executing_Flag)
//			{
//				flag_Struct.Cmd_Execute_En = ENABLE;
//			}	
//		}		

//		
//		/* 执行命令 */
//		if(ENABLE == flag_Struct.Cmd_Execute_En)
//		{
//			flag_Struct.Cmd_Execute_En = DISABLE; 
//			
//			flag_Struct.Cmd_Executing_Flag = SET;						// 命令执行完成前不允许再次执行
//			
//			CMD_Execute();
//			
//			flag_Struct.Cmd_ProcDone_Flag = RESET;			
//		}
		
		/* 脱机加工 */
		if((0 != Offline_Data_Num) && (ENABLE == Offline_Work_Flag))
		{
			delay_ms(10);

#if PRIN2DISP			
			// test
			printf("t:%d\r\n", Offline_Data_Num);
#endif
			
 			Offline_Work_Flag = DISABLE;
 			Offline_Data_Num--;
			offlineWork(LENGTH_OFFL - Offline_Data_Num - 1);
			
		}
		
		
		
		
		// 换向测试
//		if(t%2)
//			XAxisDir_Pos;
//		else
//			XAxisDir_Neg;
		
//    printf("t:%d\r\n",t);
//		delay_ms(500);
		t++;
		
//		TIM_Cmd(ADDSUB_TIMER, ENABLE);
//		if(X_P_Lim())
//			printf("没有限位\r\n");
//		else
//			printf("限位\r\n");
		
//		LED0 = t%2;

	}
}


// 系统初始化函数 （初始化函数尽量只调用一次，因为野指针的存在可能导致系统问题）
void Sys_Init(void)
{
	Flag_Struct_Init();
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
	
	/* 自检 */
	selfCheckFunc();
	
	/* 运动消除换向间隙 */
	Move2CompensateBacklash();
	
	
	/* 使能加减速时钟， 测试用，可删除  */
//	TIM_Cmd(ADDSUB_TIMER, ENABLE);
	
	/* 使能pwm， 测试用，可删除  */
//	PWM_Cmd(X_PWM, ENABLE, X_CH_EXTI);
//	PWM_Cmd(X_PWM, ENABLE, X_CH_OUT);
//	
//	PWM_Cmd(Y_PWM, ENABLE, Y_CH_EXTI);
//	PWM_Cmd(Y_PWM, ENABLE, Y_CH_OUT);
//	
//	PWM_Cmd(Z_PWM, ENABLE, Z_CH_EXTI);
//	PWM_Cmd(Z_PWM, ENABLE, Z_CH_OUT); 
}


// 标记类结构体初始化
void Flag_Struct_Init(void)
{
	mymemset(&flag_Struct, 0, sizeof(flag_Struct));
//	flag_Struct.Cmd_Execute_Flag = ENABLE;
	mymemset(&backlashCompen, 0, sizeof(backlashCompen));
}






