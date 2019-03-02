/********************************************************************************************************
*       中断的IO口处理程序  2019年1月29日09:26:51 byYJY
********************************************************************************************************/

#include "exti.h"

extern 	Flag_Structure 								flag_Struct;
extern 	volatile 	FunctionalState 		Offline_Work_Flag; 		// 进入脱机加工的标记
extern 	Motion_Strcuture 							motion_Data;	
extern 	FunctionalState	 							nAxisStatus[AXIS_NUM];  	// 各轴是否可以运动的标志



static GPIO_Structure_XX GPIO_EXTI_Plus[AXIS_NUM]; 			// 电机反馈的PWM接收中断 顺序：X、Y、Z、A、B


#if _TEST	
	/* 按钮中断，测试用  */
static	GPIO_Structure_XX 	GPIO_Key1;
#endif	



// 脉冲数的中断计数
vu32 plusNumPWM[AXIS_NUM] = {0, 0, 0, 0, 0};


// 外部中断初始化		 	
void EXTI_Config_Init(void)
{
	/* 电机PWM反馈接收IO口初始化 中断线0-4 抢占优先级为0x0-最高 */
	RCC_Periph_N(GPIO_EXTI_Plus+0, EXTI_X_PLUS, EXTI_Trigger_Rising, 0x00, 0x00);
//	delay_ms(5);
	RCC_Periph_N(GPIO_EXTI_Plus+1, EXTI_Y_PLUS, EXTI_Trigger_Rising, 0x00, 0x00);
//	delay_ms(5);
	RCC_Periph_N(GPIO_EXTI_Plus+2, EXTI_Z_PLUS, EXTI_Trigger_Rising, 0x00, 0x01);
//	delay_ms(5);
	RCC_Periph_N(GPIO_EXTI_Plus+3, EXTI_A_PLUS, EXTI_Trigger_Rising, 0x00, 0x02);
//	delay_ms(5);
	RCC_Periph_N(GPIO_EXTI_Plus+4, EXTI_B_PLUS, EXTI_Trigger_Rising, 0x00, 0x02);
//	delay_ms(5);

#if _TEST	
	/* 按钮中断，测试用  */
	RCC_Periph_N(&GPIO_Key1, _KEY1, EXTI_Trigger_Rising, 0x01, 0x00);
#endif		
}

// 中断服务程序
void EXTI0_IRQHandler(void)
{ 	
	EXTI_ClearITPendingBit(EXTI_Line0);			// 清除中断标志位
	
//	EXTI_IRQ_PWM(2, Z_PWM, Z);
	
	EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
}

// 中断服务程序
void EXTI1_IRQHandler(void)
{ 	
	EXTI_ClearITPendingBit(EXTI_Line1);			// 清除中断标志位
	
#if _TEST_ON_ALIENTEK
	/* 啥都不干 */
#else
	EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#endif
	
//	EXTI_IRQ_PWM(1, Y_PWM, Y);
	
	
}

// 中断服务程序
void EXTI2_IRQHandler(void)
{ 
	EXTI_ClearITPendingBit(EXTI_Line2);			// 清除中断标志位
	
#if _TEST_ON_ALIENTEK	
	EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#else
	/* 啥都不干 */
#endif
	
//	EXTI_IRQ_PWM(2, Z_PWM, Z);
}

// 中断服务程序
void EXTI3_IRQHandler(void)
{ 
	EXTI_ClearITPendingBit(EXTI_Line3);			// 清除中断标志位
	
//	EXTI_IRQ_PWM(3, A_PWM, A);
}

// 中断服务程序
void EXTI4_IRQHandler(void)
{ 
	EXTI_ClearITPendingBit(EXTI_Line4);			// 清除中断标志位
	
	EXTI_IRQ_PWM_MACRO(0, X_PWM, X_CH_EXTI, X_CH_OUT); 
	
//	/* 中断服务程序 */
//	EXTI->PR = EXTI_Line4; 	/* 清除中断 */
//	
//	pluNumPWM[0]++;
//	
//	/* 进入减速阶段 */
//	if((SUB_SPEED != addSubSpeed_Status[0]) && 
//		(Psc_Data_Cur[0].addSpeed_NeedPlusNum > (cmd_Plus_Data.plusNum[0] - pluNumPWM[0])))
//	{
////		ADDSUB_TIMER->CNT = 0;
//		addSubSpeed_Status[0] = SUB_SPEED; 
//	}
//	
//	/*  运动完成 关闭PWM */
//	if(cmd_Plus_Data.plusNum[0] < pluNumPWM[0])
//	{
//		/* nAxis_StepMotor_Stop_MACRO(TIM_N, ch_exti, ch_out); 		*/
//		// 通道2 ch_exti
//		X_PWM->CR1 &= (uint16_t)~TIM_CR1_CEN;															// 停止PWm脉冲
//		X_PWM->CCMR1 = ((X_PWM->CCMR1) & ((uint16_t)~TIM_CCMR1_OC2M)) | 
//										((uint16_t)(TIM_ForcedAction_Active << 8)); 			// PWM电平强制拉高		
//		// 通道1 ch_out
//		X_PWM->CR1 &= (uint16_t)~TIM_CR1_CEN;															// 停止PWm脉冲	
//		X_PWM->CCMR1 = ((X_PWM->CCMR1) & ((uint16_t)~TIM_CCMR1_OC1M)) | 
//										((uint16_t)(TIM_ForcedAction_Active)); 			// PWM电平强制拉高		
//		
//		nAxisStatus[0] = DISABLE;
//		
//		/* respUsartMsg("PWM_EXTI\r\n", 10); */
//		
//		/* 所有的轴都停止才停止 ADDSUB_TIMER  */
//		if(0 == nAxis_Motion_Flag)
//		{
//#if OFFLINE_WORK
//			Offline_Work_Flag = ENABLE;
//#endif
//			/* TIM_Cmd(ADDSUB_TIMER, DISABLE); */
//			X_PWM->CR1 &= (uint16_t)~TIM_CR1_CEN;
//		}
//	}	
}

// 中断服务程序
void EXTI9_5_IRQHandler(void)
{ 
//	EXTI_ClearITPendingBit(EXTI_Line1);			// 清除中断标志位
	
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)		// KEY1测试
	{
		EXTI_ClearITPendingBit(EXTI_Line5);				// 清除中断标志位
		respUsartMsg("KEY1\r\n", 6);							// 打印
		
	}
	
	if(EXTI_GetITStatus(EXTI_Line6) != RESET)		// KEY1测试
	{
		EXTI_ClearITPendingBit(EXTI_Line6);				// 清除中断标志位
		
		EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
		
		
		
//		respUsartMsg("EXTI_6\r\n", 8);							// 打印		
	}

}

// 中断服务程序
void EXTI15_10_IRQHandler(void)
{ 
//	EXTI_ClearITPendingBit(EXTI_Line1);			// 清除中断标志位
	
}



// 对应的IO口中断初始化
static void RCC_Periph_N(GPIO_Structure_XX *GPIO_Temp, const char str[], EXTITrigger_TypeDef EXTI_Trigger_N, 
	uint8_t NVIC_IRQPreemptionPriority,	uint8_t NVIC_IRQSubPriority)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	/* 对应的GPIO口初始化 */
	EXTI_GPIO_Init(GPIO_Temp, str);	
	
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
static void EXTI_GPIO_Init(GPIO_Structure_XX *GPIO_Temp, const char str[])
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(!GPIO_Structure_Make(str, GPIO_Temp))
		return;
	
	RCC_AHB1PeriphClockCmd(GPIO_Temp->RCC_Periph_N, ENABLE);		// 使能时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Temp->GPIO_Pin_N;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  			//输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//100M
	
#if _TEST	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;  			//下拉输入  测试板子上的KEY1时用，可删除
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  			// 上拉输入  测试板子上的KEY1时用，可删除
#else
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  			//上拉输入 
#endif
	
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

#if _TEST	
	EXTI_Cmd(GPIO_Key1.EXTI_Line_N, ENABLE);	
#endif
	
//	EXTI_Cmd(EXTI_Line4, ENABLE);
}

// 测试用的各轴停止函数
static void nAxis_StepMotor_Stop_MACRO(TIM_TypeDef* TIM_N, u8 ch_exti, u8 ch_out)
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
//			TIM_Cmd(ADDSUB_TIMER, DISABLE);
			DIS_ADDSUB_TIMER;
#if PRIN2DISP					
			respUsartMsg("PWM_EXTI\r\n", 10);
#else
//			respUsartMsg(backResString_Exti, RESP_MOTIONMSG_LENGTH);
#endif
		}
	}
}




