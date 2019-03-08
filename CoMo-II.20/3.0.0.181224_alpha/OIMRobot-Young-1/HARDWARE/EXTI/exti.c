/********************************************************************************************************
*       中断的IO口处理程序  2019年1月29日09:26:51 byYJY
********************************************************************************************************/

#include "exti.h"

extern 		Flag_Structure 								flag_Struct;
extern 		volatile 	FunctionalState 		Offline_Work_Flag; 		// 进入脱机加工的标记
extern 		Motion_Strcuture 							motion_Data;	
extern 		FunctionalState	 							nAxisStatus[AXIS_NUM];  	// 各轴是否可以运动的标志



static 		GPIO_Structure_XX	 		GPIO_EXTI_Plus[AXIS_NUM]; 			// 电机反馈的PWM接收中断 顺序：X、Y、Z、A、B
GPIO_Structure_XX			EXTI_UrgentStop;										// 急停中断
GPIO_Structure_XX			EXTI_MainMotor_Start;								// 急停中断





// 脉冲数的中断计数
vu32 plusNumPWM[AXIS_NUM] = {0, 0, 0, 0, 0};
	


// 外部中断初始化		 	
void EXTI_Config_Init(void)
{	
	/* 电机PWM反馈接收IO口初始化 中断线0-4 抢占优先级为0x0-最高 */
	EXTI_Line_Init(GPIO_EXTI_Plus+0, EXTI_X_PLUS, EXTI_LINE_X, EXTI_Trigger_Rising, 0x00, 0x00);
//	delay_ms(5);
	EXTI_Line_Init(GPIO_EXTI_Plus+1, EXTI_Y_PLUS, EXTI_LINE_Y, EXTI_Trigger_Rising, 0x00, 0x00);
//	delay_ms(5);
	EXTI_Line_Init(GPIO_EXTI_Plus+2, EXTI_Z_PLUS, EXTI_LINE_Z, EXTI_Trigger_Rising, 0x00, 0x01);
//	delay_ms(5);
	EXTI_Line_Init(GPIO_EXTI_Plus+3, EXTI_A_PLUS, EXTI_LINE_A, EXTI_Trigger_Rising, 0x00, 0x02);
//	delay_ms(5);
	EXTI_Line_Init(GPIO_EXTI_Plus+4, EXTI_B_PLUS, EXTI_LINE_B, EXTI_Trigger_Rising, 0x00, 0x02);
//	delay_ms(5);
	
	// 急停中断初始化
	EXTI_Line_Init(&EXTI_UrgentStop, EXTI_UrgentStop_IO, EXTI_LINE_UrgentStop, EXTI_Trigger_Rising_Falling, 0x00, 0x00);
	
	// 主轴电机启动初始化
	EXTI_Line_Init(&EXTI_MainMotor_Start, EXTI_MAINMOTOR_START, EXTI_LINE_MAINMOTOR, EXTI_Trigger_Falling, 0x01, 0x00);


}

// 中断服务程序
void EXTI0_IRQHandler(void)
{ 	
	EXTI_ClearITPendingBit(EXTI_Line0);			// 清除中断标志位
	
#if 		(EXTI_LINE_X	==	0)
	EXTI_IRQ_PWM_MACRO(0, X_PWM, X_CH_EXTI, X_CH_OUT);
#elif 	(EXTI_LINE_Y == 0)
	EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#elif 	(EXTI_LINE_Z == 0)
	EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
#elif 	(EXTI_LINE_A == 0)
	EXTI_IRQ_PWM_MACRO(3, A_PWM, A_CH_EXTI, A_CH_OUT);
#elif 	(EXTI_LINE_B == 0)
	EXTI_IRQ_PWM_MACRO(4, B_PWM, B_CH_EXTI, B_CH_OUT);
#elif 	(EXTI_LINE_UrgentStop == 0)
	EXTI_IRQ_UrgentStop_MACRO();
#elif 	(EXTI_LINE_MAINMOTOR == 0)
	EXTI_IRQ_MainMotorWork_MACRO();
#else
//	#warning 	EXTI_Line0 has problem! 
#endif	
}

// 中断服务程序
void EXTI1_IRQHandler(void)
{ 	
	EXTI_ClearITPendingBit(EXTI_Line1);			// 清除中断标志位

#if 		(EXTI_LINE_X	==	1)
	EXTI_IRQ_PWM_MACRO(0, X_PWM, X_CH_EXTI, X_CH_OUT);
#elif 	(EXTI_LINE_Y == 1)
	EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#elif 	(EXTI_LINE_Z == 1)
	EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
#elif 	(EXTI_LINE_Z == 1)
	EXTI_IRQ_PWM_MACRO(3, A_PWM, A_CH_EXTI, A_CH_OUT);
#elif 	(EXTI_LINE_B == 1)
	EXTI_IRQ_PWM_MACRO(4, B_PWM, B_CH_EXTI, B_CH_OUT);
#elif 	(EXTI_LINE_UrgentStop == 1)
	EXTI_IRQ_UrgentStop_MACRO();
#elif 	(EXTI_LINE_MAINMOTOR == 1)
	EXTI_IRQ_MainMotorWork_MACRO();
#else
//	#warning 	EXTI_Line1 has problem! 
#endif	
}

// 中断服务程序
void EXTI2_IRQHandler(void)
{ 
	EXTI_ClearITPendingBit(EXTI_Line2);			// 清除中断标志位

#if 		(EXTI_LINE_X	==	2)
	EXTI_IRQ_PWM_MACRO(0, X_PWM, X_CH_EXTI, X_CH_OUT);
#elif 	(EXTI_LINE_Y == 2)
	EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#elif 	(EXTI_LINE_Z == 2)
	EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
#elif 	(EXTI_LINE_Z == 2)
	EXTI_IRQ_PWM_MACRO(3, A_PWM, A_CH_EXTI, A_CH_OUT);
#elif 	(EXTI_LINE_B == 2)
	EXTI_IRQ_PWM_MACRO(4, B_PWM, B_CH_EXTI, B_CH_OUT);
#elif 	(EXTI_LINE_UrgentStop == 2)
	EXTI_IRQ_UrgentStop_MACRO();
#elif 	(EXTI_LINE_MAINMOTOR == 2)
	EXTI_IRQ_MainMotorWork_MACRO();
#else
//	#warning 	EXTI_Line2 has problem! 
#endif	
}

// 中断服务程序
void EXTI3_IRQHandler(void)
{ 
	EXTI_ClearITPendingBit(EXTI_Line3);			// 清除中断标志位
	
#if 		(EXTI_LINE_X	==	3)
	EXTI_IRQ_PWM_MACRO(0, X_PWM, X_CH_EXTI, X_CH_OUT);
#elif 	(EXTI_LINE_Y == 3)
	EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#elif 	(EXTI_LINE_Z == 3)
	EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
#elif 	(EXTI_LINE_Z == 3)
	EXTI_IRQ_PWM_MACRO(3, A_PWM, A_CH_EXTI, A_CH_OUT);
#elif 	(EXTI_LINE_B == 3)
	EXTI_IRQ_PWM_MACRO(4, B_PWM, B_CH_EXTI, B_CH_OUT);
#elif 	(EXTI_LINE_UrgentStop == 3)
	EXTI_IRQ_UrgentStop_MACRO();
#elif 	(EXTI_LINE_MAINMOTOR == 3)
	EXTI_IRQ_MainMotorWork_MACRO();
#else
//	#warning 	EXTI_Line3 has problem! 
#endif	
}

// 中断服务程序
void EXTI4_IRQHandler(void)
{ 
	EXTI_ClearITPendingBit(EXTI_Line4);			// 清除中断标志位
	
#if 		(EXTI_LINE_X	==	4)
	EXTI_IRQ_PWM_MACRO(0, X_PWM, X_CH_EXTI, X_CH_OUT);
#elif 	(EXTI_LINE_Y == 4)
	EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#elif 	(EXTI_LINE_Z == 4)
	EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
#elif 	(EXTI_LINE_Z == 4)
	EXTI_IRQ_PWM_MACRO(3, A_PWM, A_CH_EXTI, A_CH_OUT);
#elif 	(EXTI_LINE_B == 4)
	EXTI_IRQ_PWM_MACRO(4, B_PWM, B_CH_EXTI, B_CH_OUT);
#elif 	(EXTI_LINE_UrgentStop == 4)
	EXTI_IRQ_UrgentStop_MACRO();
#elif 	(EXTI_LINE_MAINMOTOR == 4)
	EXTI_IRQ_MainMotorWork_MACRO();
#else
//	#warning 	EXTI_Line4 has problem! 
#endif	
}

// 中断服务程序
void EXTI9_5_IRQHandler(void)
{ 
//	EXTI_ClearITPendingBit(EXTI_Line1);			// 清除中断标志位
	
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)		// KEY1测试
	{
		EXTI_ClearITPendingBit(EXTI_Line5);				// 清除中断标志位
//		respUsartMsg("KEY1\r\n", 6);							// 打印
#if 		(EXTI_LINE_X	==	5)
		EXTI_IRQ_PWM_MACRO(0, X_PWM, X_CH_EXTI, X_CH_OUT);
#elif 	(EXTI_LINE_Y == 5)
		EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#elif 	(EXTI_LINE_Z == 5)
		EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
#elif 	(EXTI_LINE_Z == 5)
		EXTI_IRQ_PWM_MACRO(3, A_PWM, A_CH_EXTI, A_CH_OUT);
#elif 	(EXTI_LINE_B == 5)
		EXTI_IRQ_PWM_MACRO(4, B_PWM, B_CH_EXTI, B_CH_OUT);
#elif 	(EXTI_LINE_UrgentStop == 5)
		EXTI_IRQ_UrgentStop_MACRO();
#elif 	(EXTI_LINE_MAINMOTOR == 5)
	EXTI_IRQ_MainMotorWork_MACRO();
#else
//	#warning 	EXTI_Line5 has problem! 
#endif			
	}
	
	if(EXTI_GetITStatus(EXTI_Line6) != RESET)		// KEY1测试
	{
		EXTI_ClearITPendingBit(EXTI_Line6);				// 清除中断标志位
		
#if 		(EXTI_LINE_X	==	6)
		EXTI_IRQ_PWM_MACRO(0, X_PWM, X_CH_EXTI, X_CH_OUT);
#elif 	(EXTI_LINE_Y == 6)
		EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#elif 	(EXTI_LINE_Z == 6)
		EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
#elif 	(EXTI_LINE_Z == 6)
		EXTI_IRQ_PWM_MACRO(3, A_PWM, A_CH_EXTI, A_CH_OUT);
#elif 	(EXTI_LINE_B == 6)
		EXTI_IRQ_PWM_MACRO(4, B_PWM, B_CH_EXTI, B_CH_OUT);
#elif 	(EXTI_LINE_UrgentStop == 6)
		EXTI_IRQ_UrgentStop_MACRO();
#elif 	(EXTI_LINE_MAINMOTOR == 6)
	EXTI_IRQ_MainMotorWork_MACRO();
#else
//	#warning 	EXTI_Line0 has problem! 
#endif			
	}
	
	if(EXTI_GetITStatus(EXTI_Line7) != RESET)		// KEY1测试
	{
		EXTI_ClearITPendingBit(EXTI_Line7);				// 清除中断标志位
		
#if 		(EXTI_LINE_X	==	7)
		EXTI_IRQ_PWM_MACRO(0, X_PWM, X_CH_EXTI, X_CH_OUT);
#elif 	(EXTI_LINE_Y == 7)
		EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#elif 	(EXTI_LINE_Z == 7)
		EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
#elif 	(EXTI_LINE_Z == 7)
		EXTI_IRQ_PWM_MACRO(3, A_PWM, A_CH_EXTI, A_CH_OUT);
#elif 	(EXTI_LINE_B == 7)
		EXTI_IRQ_PWM_MACRO(4, B_PWM, B_CH_EXTI, B_CH_OUT);
#elif 	(EXTI_LINE_UrgentStop == 7)
		EXTI_IRQ_UrgentStop_MACRO();
#elif 	(EXTI_LINE_MAINMOTOR == 7)
	EXTI_IRQ_MainMotorWork_MACRO();
#else
//	#warning 	EXTI_Line0 has problem! 
#endif			
	}
	
	if(EXTI_GetITStatus(EXTI_Line8) != RESET)		// KEY1测试
	{
		EXTI_ClearITPendingBit(EXTI_Line8);				// 清除中断标志位
		
#if 		(EXTI_LINE_X	==	8)
		EXTI_IRQ_PWM_MACRO(0, X_PWM, X_CH_EXTI, X_CH_OUT);
#elif 	(EXTI_LINE_Y == 8)
		EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#elif 	(EXTI_LINE_Z == 8)
		EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
#elif 	(EXTI_LINE_Z == 8)
		EXTI_IRQ_PWM_MACRO(3, A_PWM, A_CH_EXTI, A_CH_OUT);
#elif 	(EXTI_LINE_B == 8)
		EXTI_IRQ_PWM_MACRO(4, B_PWM, B_CH_EXTI, B_CH_OUT);
#elif 	(EXTI_LINE_UrgentStop == 8)
		EXTI_IRQ_UrgentStop_MACRO();
#elif 	(EXTI_LINE_MAINMOTOR == 8)
	EXTI_IRQ_MainMotorWork_MACRO();
#else
//	#warning 	EXTI_Line0 has problem! 
#endif			
	}
	
	if(EXTI_GetITStatus(EXTI_Line9) != RESET)		// KEY1测试
	{
		EXTI_ClearITPendingBit(EXTI_Line9);				// 清除中断标志位
		
#if 		(EXTI_LINE_X	==	9)
		EXTI_IRQ_PWM_MACRO(0, X_PWM, X_CH_EXTI, X_CH_OUT);
#elif 	(EXTI_LINE_Y == 9)
		EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#elif 	(EXTI_LINE_Z == 9)
		EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
#elif 	(EXTI_LINE_Z == 9)
		EXTI_IRQ_PWM_MACRO(3, A_PWM, A_CH_EXTI, A_CH_OUT);
#elif 	(EXTI_LINE_B == 9)
		EXTI_IRQ_PWM_MACRO(4, B_PWM, B_CH_EXTI, B_CH_OUT);
#elif 	(EXTI_LINE_UrgentStop == 9)
		EXTI_IRQ_UrgentStop_MACRO();
#elif 	(EXTI_LINE_MAINMOTOR == 9)
	EXTI_IRQ_MainMotorWork_MACRO();
#else
//	#warning 	EXTI_Line0 has problem! 
#endif			
	}
}

// 中断服务程序
void EXTI15_10_IRQHandler(void)
{ 
//	EXTI_ClearITPendingBit(EXTI_Line1);			// 清除中断标志位
	if(EXTI_GetITStatus(EXTI_Line10) != RESET)		// KEY1测试
	{
		EXTI_ClearITPendingBit(EXTI_Line10);				// 清除中断标志位
		
#if 		(EXTI_LINE_X	==	10)
		EXTI_IRQ_PWM_MACRO(0, X_PWM, X_CH_EXTI, X_CH_OUT);
#elif 	(EXTI_LINE_Y == 10)
		EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#elif 	(EXTI_LINE_Z == 10)
		EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
#elif 	(EXTI_LINE_Z == 10)
		EXTI_IRQ_PWM_MACRO(3, A_PWM, A_CH_EXTI, A_CH_OUT);
#elif 	(EXTI_LINE_B == 10)
		EXTI_IRQ_PWM_MACRO(4, B_PWM, B_CH_EXTI, B_CH_OUT);
#elif 	(EXTI_LINE_UrgentStop == 10)
		EXTI_IRQ_UrgentStop_MACRO();
#elif 	(EXTI_LINE_MAINMOTOR == 10)
	EXTI_IRQ_MainMotorWork_MACRO();
#else
//	#warning 	EXTI_Line0 has problem! 
#endif			
	}
	
	if(EXTI_GetITStatus(EXTI_Line11) != RESET)		// KEY1测试
	{
		EXTI_ClearITPendingBit(EXTI_Line11);				// 清除中断标志位
		
#if 		(EXTI_LINE_X	==	11)
		EXTI_IRQ_PWM_MACRO(0, X_PWM, X_CH_EXTI, X_CH_OUT);
#elif 	(EXTI_LINE_Y == 11)
		EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#elif 	(EXTI_LINE_Z == 11)
		EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
#elif 	(EXTI_LINE_Z == 11)
		EXTI_IRQ_PWM_MACRO(3, A_PWM, A_CH_EXTI, A_CH_OUT);
#elif 	(EXTI_LINE_B == 11)
		EXTI_IRQ_PWM_MACRO(4, B_PWM, B_CH_EXTI, B_CH_OUT);
#elif 	(EXTI_LINE_UrgentStop == 11)
		EXTI_IRQ_UrgentStop_MACRO();
#elif 	(EXTI_LINE_MAINMOTOR == 11)
	EXTI_IRQ_MainMotorWork_MACRO();
#else
//	#warning 	EXTI_Line0 has problem! 
#endif			
	}
	
	if(EXTI_GetITStatus(EXTI_Line12) != RESET)		// KEY1测试
	{
		EXTI_ClearITPendingBit(EXTI_Line12);				// 清除中断标志位
		
#if 		(EXTI_LINE_X	==	12)
		EXTI_IRQ_PWM_MACRO(0, X_PWM, X_CH_EXTI, X_CH_OUT);
#elif 	(EXTI_LINE_Y == 12)
		EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#elif 	(EXTI_LINE_Z == 12)
		EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
#elif 	(EXTI_LINE_Z == 12)
		EXTI_IRQ_PWM_MACRO(3, A_PWM, A_CH_EXTI, A_CH_OUT);
#elif 	(EXTI_LINE_B == 12)
		EXTI_IRQ_PWM_MACRO(4, B_PWM, B_CH_EXTI, B_CH_OUT);
#elif 	(EXTI_LINE_UrgentStop == 12)
		EXTI_IRQ_UrgentStop_MACRO();
#elif 	(EXTI_LINE_MAINMOTOR == 12)
	EXTI_IRQ_MainMotorWork_MACRO();
#else
//	#warning 	EXTI_Line0 has problem! 
#endif			
	}
	
	if(EXTI_GetITStatus(EXTI_Line13) != RESET)		// KEY1测试
	{
		EXTI_ClearITPendingBit(EXTI_Line13);				// 清除中断标志位
		
#if 		(EXTI_LINE_X	==	13)
		EXTI_IRQ_PWM_MACRO(0, X_PWM, X_CH_EXTI, X_CH_OUT);
#elif 	(EXTI_LINE_Y == 13)
		EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#elif 	(EXTI_LINE_Z == 13)
		EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
#elif 	(EXTI_LINE_Z == 13)
		EXTI_IRQ_PWM_MACRO(3, A_PWM, A_CH_EXTI, A_CH_OUT);
#elif 	(EXTI_LINE_B == 13)
		EXTI_IRQ_PWM_MACRO(4, B_PWM, B_CH_EXTI, B_CH_OUT);
#elif 	(EXTI_LINE_UrgentStop == 13)
		EXTI_IRQ_UrgentStop_MACRO();
#elif 	(EXTI_LINE_MAINMOTOR == 13)
	EXTI_IRQ_MainMotorWork_MACRO();
#else
//	#warning 	EXTI_Line0 has problem! 
#endif			
	}
	
	if(EXTI_GetITStatus(EXTI_Line14) != RESET)		// KEY1测试
	{
		EXTI_ClearITPendingBit(EXTI_Line14);				// 清除中断标志位
		
#if 		(EXTI_LINE_X	==	14)
		EXTI_IRQ_PWM_MACRO(0, X_PWM, X_CH_EXTI, X_CH_OUT);
#elif 	(EXTI_LINE_Y == 14)
		EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#elif 	(EXTI_LINE_Z == 14)
		EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
#elif 	(EXTI_LINE_Z == 14)
		EXTI_IRQ_PWM_MACRO(3, A_PWM, A_CH_EXTI, A_CH_OUT);
#elif 	(EXTI_LINE_B == 14)
		EXTI_IRQ_PWM_MACRO(4, B_PWM, B_CH_EXTI, B_CH_OUT);
#elif 	(EXTI_LINE_UrgentStop == 14)
		EXTI_IRQ_UrgentStop_MACRO();
#elif 	(EXTI_LINE_MAINMOTOR == 14)
	EXTI_IRQ_MainMotorWork_MACRO();
#else
//	#warning 	EXTI_Line0 has problem! 
#endif			
	}
	
	if(EXTI_GetITStatus(EXTI_Line15) != RESET)		// KEY1测试
	{
		EXTI_ClearITPendingBit(EXTI_Line15);				// 清除中断标志位
		
#if 		(EXTI_LINE_X	==	15)
		EXTI_IRQ_PWM_MACRO(0, X_PWM, X_CH_EXTI, X_CH_OUT);
#elif 	(EXTI_LINE_Y == 15)
		EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#elif 	(EXTI_LINE_Z == 15)
		EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
#elif 	(EXTI_LINE_Z == 15)
		EXTI_IRQ_PWM_MACRO(3, A_PWM, A_CH_EXTI, A_CH_OUT);
#elif 	(EXTI_LINE_B == 15)
		EXTI_IRQ_PWM_MACRO(4, B_PWM, B_CH_EXTI, B_CH_OUT);
#elif 	(EXTI_LINE_UrgentStop == 15)
		EXTI_IRQ_UrgentStop_MACRO();
#elif 	(EXTI_LINE_MAINMOTOR == 15)
	EXTI_IRQ_MainMotorWork_MACRO();
#else
//	#warning 	EXTI_Line0 has problem! 
#endif			
	}	
}



// 对应的IO口中断初始化
static void EXTI_Line_Init(GPIO_Structure_XX *GPIO_Temp, const char str[], uint8_t Exti_Line_N, EXTITrigger_TypeDef EXTI_Trigger_N,  
	uint8_t NVIC_IRQPreemptionPriority,	uint8_t NVIC_IRQSubPriority)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	/* 对应的GPIO口初始化 */
	EXTI_GPIO_Init(GPIO_Temp, str, Exti_Line_N);	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);		// 使能SYSCFG时钟 中断使用
	
	SYSCFG_EXTILineConfig(GPIO_Temp->EXTI_PortGPIO_N, GPIO_Temp->EXTI_PinSource_N);		// 连接到中断线 
	
	// 配置中断线 
	EXTI_InitStructure.EXTI_Line = GPIO_Temp->EXTI_Line_N;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//中断事件
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_N; 	// 触发方式 注意考虑触发方式 byYJY
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//中断线使能
	EXTI_Init(&EXTI_InitStructure);							//配置
	
	// 配置中断 ///////
	NVIC_InitStructure.NVIC_IRQChannel = GPIO_Temp->NVIC_IRQ_N;			//外部中断0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_IRQPreemptionPriority;//抢占优先级0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_IRQSubPriority;		//子优先级2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//使能外部中断通道
	NVIC_Init(&NVIC_InitStructure);								//配置	
	
	// 关闭中断
	EXTI_Cmd(GPIO_Temp->EXTI_Line_N, DISABLE);
}

// IO口的GPIO初始化
static void EXTI_GPIO_Init(GPIO_Structure_XX *GPIO_Temp, const char str[], uint8_t Exti_Line_N)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(ERROR == GPIO_Structure_Make(str, GPIO_Temp))
		return;
	
	if(Exti_Line_N != GPIO_Temp->EXTI_PinSource_N)
	{
		respMsgError("中断线设置错误，请检查config.h！\r\n", 1);
		return;
	}
	
	RCC_AHB1PeriphClockCmd(GPIO_Temp->RCC_Periph_N, ENABLE);		// 使能时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Temp->GPIO_Pin_N;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  			//输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//100M


	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  			//上拉输入 

	
	GPIO_Init(GPIO_Temp->GPIO_Port, &GPIO_InitStructure);	
}

// 中断使能或关闭
static void EXTI_Cmd(uint32_t EXTI_Line_N, FunctionalState state)
{
	if(ENABLE == state)
	{
		EXTI->IMR |= EXTI_Line_N;					// 使能外部中断
	}
	else 
	{
		EXTI->IMR &= ~(EXTI_Line_N); 			// 屏蔽外部中断
	}
}

// 所有中断线使能
void EXTI_Enable(void)
{
	EXTI_Cmd((GPIO_EXTI_Plus+0)->EXTI_Line_N, ENABLE);
	EXTI_Cmd((GPIO_EXTI_Plus+1)->EXTI_Line_N, ENABLE);
	EXTI_Cmd((GPIO_EXTI_Plus+2)->EXTI_Line_N, ENABLE);
	EXTI_Cmd((GPIO_EXTI_Plus+3)->EXTI_Line_N, ENABLE);
	EXTI_Cmd((GPIO_EXTI_Plus+4)->EXTI_Line_N, ENABLE);
	
	// 急停中断使能
	EXTI_Cmd(EXTI_UrgentStop.EXTI_Line_N, ENABLE);
	
	// 主轴电机启动中断使能
	EXTI_Cmd(EXTI_MainMotor_Start.EXTI_Line_N, ENABLE);
	


	
//	EXTI_Cmd(EXTI_Line4, ENABLE);
}

// 测试用的各轴停止函数
void nAxis_StepMotor_Stop_MACRO(TIM_TypeDef* TIM_N, u8 ch_exti, u8 ch_out)
{
	PWM_Cmd(TIM_N, DISABLE, ch_exti); 	
	PWM_Cmd(TIM_N, DISABLE, ch_out); 	
}

// 测试用的中断服务程序
static void EXTI_IRQ_PWM_MACRO(u8 n, TIM_TypeDef *TIM_N, u8 ch_exti, u8 ch_out)
{	
	plusNumPWM[n]++;
	
	/* 进入减速阶段 */
	if((CONST_SPEED == motion_Data.addSubSpeed_Status[n]) && 
		(motion_Data.PSC_Data[n].addSpeed_NeedPlusNum > (motion_Data.cmd_Datas.plus_Datas.plusNum[n] - plusNumPWM[n])))
	{
//		ADDSUB_TIMER->CNT = 0;
		motion_Data.addSubSpeed_Status[n] = SUB_SPEED; 
	}
	
	/*  运动完成 关闭PWM */
#if _TEST_ON_ALIENTEK
	if(motion_Data.cmd_Datas.plus_Datas.plusNum[n] <= plusNumPWM[n])
#else
	if(cmd_Plus_Data.plusNum[n] <= (pluNumPWM[n]-1))
#endif
	{
		nAxis_StepMotor_Stop_MACRO(TIM_N, ch_exti, ch_out); 		//  注意这里用宏定义
		
		nAxisStatus[n] = DISABLE;
		
		/* 所有的轴都停止才停止 ADDSUB_TIMER  */
		if(0 == nAxis_Motion_Flag)
		{
			flag_Struct.Cmd_Executing_Flag = RESET;
			
#if OFFLINE_WORK
			Offline_Work_Flag = ENABLE;
#endif
			DIS_ADDSUB_TIMER;
#if PRIN2DISP					
			respUsartMsg("PWM_EXTI\r\n", 10);
#else
//			respUsartMsg(backResString_Exti, RESP_MOTIONMSG_LENGTH);
#endif
		}
	}
}


// 测试用的急停中断测试程序
static void EXTI_IRQ_UrgentStop_MACRO(void)
{
	flag_Struct.UrgentStop_Flag = UrgentStop_Locked;
}

// 主轴启动中断服务程序
static void EXTI_IRQ_MainMotorWork_MACRO(void)
{
	flag_Struct.MainMotor_Start_Flag = SET;
}








