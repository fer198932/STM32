/********************************************************************************************************
*       中断的IO口处理程序  2019年1月29日09:26:51 byYJY
********************************************************************************************************/

#include "exti.h"

static GPIO_Structure_XX GPIO_EXTI_Plus[AXIS_NUM]; 			// 电机反馈的PWM接收中断 顺序：X、Y、Z、A、B

#if _TEST	
	/* 按钮中断，测试用  */
static	GPIO_Structure_XX 	GPIO_Key1;
#endif	

// 指令中设定的运动参数
extern Proc_Data cmd_Proc_Data; 		// 命令数据，有成员指向plus_Data
extern Plus_Data cmd_Plus_Data;			// 脉冲数据，控制电机运动

// 脉冲数的中断计数
volatile u32 pluNumPWM[AXIS_NUM] = {0, 0, 0, 0, 0};

// 各轴是否可以运动 
extern volatile 	FunctionalState	 nAxisStatus[AXIS_NUM];

// 外部中断初始化		 	
void EXTI_Config_Init(void)
{
	/* 电机PWM反馈接收IO口初始化 中断线0-4 抢占优先级为0x0-最高 */
	RCC_Periph_N(GPIO_EXTI_Plus+0, EXTI_X_PLUS, EXTI_Trigger_Rising, 0x00, 0x00);
	RCC_Periph_N(GPIO_EXTI_Plus+1, EXTI_Y_PLUS, EXTI_Trigger_Rising, 0x00, 0x00);
	RCC_Periph_N(GPIO_EXTI_Plus+2, EXTI_Z_PLUS, EXTI_Trigger_Rising, 0x00, 0x01);
	RCC_Periph_N(GPIO_EXTI_Plus+3, EXTI_A_PLUS, EXTI_Trigger_Rising, 0x00, 0x02);
	RCC_Periph_N(GPIO_EXTI_Plus+4, EXTI_B_PLUS, EXTI_Trigger_Rising, 0x00, 0x02);

#if _TEST	
	/* 按钮中断，测试用  */
	RCC_Periph_N(&GPIO_Key1, _KEY1, EXTI_Trigger_Rising, 0x01, 0x00);
#endif	
	
}

// 中断服务程序
void EXTI0_IRQHandler(void)
{ 	
	EXTI_ClearITPendingBit(EXTI_Line0);			// 清除中断标志位
	
	EXTI_IRQ_PWM(0, X_PWM, X);
}

// 中断服务程序
void EXTI1_IRQHandler(void)
{ 	
	EXTI_ClearITPendingBit(EXTI_Line1);			// 清除中断标志位
	
	EXTI_IRQ_PWM(1, Y_PWM, Y);
}

// 中断服务程序
void EXTI2_IRQHandler(void)
{ 
	EXTI_ClearITPendingBit(EXTI_Line2);			// 清除中断标志位
	
	EXTI_IRQ_PWM(2, Z_PWM, Z);
}

// 中断服务程序
void EXTI3_IRQHandler(void)
{ 
	EXTI_ClearITPendingBit(EXTI_Line3);			// 清除中断标志位
	
	EXTI_IRQ_PWM(3, A_PWM, A);
}

// 中断服务程序
void EXTI4_IRQHandler(void)
{ 
	EXTI_ClearITPendingBit(EXTI_Line4);			// 清除中断标志位
	
	EXTI_IRQ_PWM(0, X_PWM, X);
	
//	pluNumPWM[0]++; 			\
//	if(pluNumPWM[0] >= cmd_Plus_Data.plusNum[0])  	\
//	{ \
//		nAxis_StepMotor_Stop(X); 				\
//		nAxisStatus[0] = ENABLE; /* 该轴可运动 */		\
//		pluNumPWM[0] = 0; 		\
//		respUsartMsg("PWM_EXTI\r\n", 10);		\
//	} \
	
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
	
	EXTI_Cmd(GPIO_Key1.EXTI_Line_N, ENABLE);	
	
//	EXTI_Cmd(EXTI_Line4, ENABLE);
}


