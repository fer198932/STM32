//////////////////////////////////////////////////////////////////////////////////	 
//  PWM生成程序  2019年1月29日20:16:48 byYJY							  
////////////////////////////////////////////////////////////////////////////////// 

#include "pwm.h"

/* PWM 初始化的结构体  */
static  	GPIO_Structure_XX 		GPIO_PWM_Plus[AXIS_NUM];
// static 		AF_Structure_XX 			AF_PWM_Plus[AXIS_NUM];

// PWM初始化函数
void PWM_Init(void)
{
	/*  各轴PWM初始化  */
	Tim_PWM_Init(GPIO_PWM_Plus+0, X_PWM, PWM_X_EXTI, 0x04, 0x04, TIM_ARR, TIM_PSC, X_CH_EXTI);
	Tim_PWM_Init(GPIO_PWM_Plus+0, X_PWM, PWM_X_OUT, 0x04, 0x04, TIM_ARR, TIM_PSC, X_CH_OUT);
	
	Tim_PWM_Init(GPIO_PWM_Plus+1, Y_PWM, PWM_Y_EXTI, 0x04, 0x04, TIM_ARR, TIM_PSC, Y_CH_EXTI);
	Tim_PWM_Init(GPIO_PWM_Plus+1, Y_PWM, PWM_Y_OUT, 0x04, 0x04, TIM_ARR, TIM_PSC, Y_CH_OUT);
	
	Tim_PWM_Init(GPIO_PWM_Plus+2, Z_PWM, PWM_Z_EXTI, 0x04, 0x04, TIM_ARR, TIM_PSC, Z_CH_EXTI);
	Tim_PWM_Init(GPIO_PWM_Plus+2, Z_PWM, PWM_Z_OUT, 0x04, 0x04, TIM_ARR, TIM_PSC, Z_CH_OUT);
	
	Tim_PWM_Init(GPIO_PWM_Plus+3, A_PWM, PWM_A_EXTI, 0x04, 0x04, TIM_ARR, TIM_PSC, A_CH_EXTI);
	Tim_PWM_Init(GPIO_PWM_Plus+3, A_PWM, PWM_A_OUT, 0x04, 0x04, TIM_ARR, TIM_PSC, A_CH_OUT);
	
	Tim_PWM_Init(GPIO_PWM_Plus+4, B_PWM, PWM_B_EXTI, 0x04, 0x04, TIM_ARR, TIM_PSC, B_CH_EXTI);
	Tim_PWM_Init(GPIO_PWM_Plus+4, B_PWM, PWM_B_OUT, 0x04, 0x04, TIM_ARR, TIM_PSC, B_CH_OUT);
	
	
}

// 定时器初始化 1、ch1、ch2为PWM输出通道(为0啥都不干) 2、中断优先级无效 可写0x04
static void Tim_PWM_Init(GPIO_Structure_XX *GPIO_Temp, TIM_TypeDef* TIM_N, const char Pin_PWM_str[], 
	uint8_t NVIC_IRQPreemptionPriority,	uint8_t NVIC_IRQSubPriority, u16 arr, u16 psc, u8 ch1)
{
	AF_Structure_XX 		TIM_AF_Structure;
	TIM_OCInitTypeDef  	TIM_OCInitStructure;
	
	// GPIO 初始化
	GPIO_PWM_Init(GPIO_Temp, Pin_PWM_str); 	
	
	// 定时器初始化
	Tim_Base_Init(TIM_N, arr, psc, DISABLE, 
		NVIC_IRQPreemptionPriority, NVIC_IRQSubPriority, &TIM_AF_Structure);
	
	// 复用映射
	GPIO_PinAFConfig(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_PinSource_N, 
		TIM_AF_Structure.GPIO_AF);
	
	// 初始化PWM输出通道
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 								// 选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 		// 比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 				// 输出极性 注：默认拉高  byYJY
	
	// 设置输出通道
	Set_PWM_Channel(TIM_N, &TIM_OCInitStructure, ch1, (arr>>1));
	
	// 关闭PWM，并强制拉高
	PWM_Cmd(TIM_N, DISABLE, ch1);	
}

// 定时器输出PIN脚初始化
static void GPIO_PWM_Init(GPIO_Structure_XX *GPIO_Temp, const char Pin_PWM_str[])
{
	GPIO_InitTypeDef 		GPIO_InitStructure;
	
	// GPIO结构体构造
	if(!GPIO_Structure_Make(Pin_PWM_str, GPIO_Temp))
		return;	
	
	// 使能时钟
	RCC_AHB1PeriphClockCmd(GPIO_Temp->RCC_Periph_N, ENABLE);
	
	// GPIO配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Temp->GPIO_Pin_N;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  			// 复用输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	// 100M
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 			// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  			// 上拉 
	
	GPIO_Init(GPIO_Temp->GPIO_Port, &GPIO_InitStructure);
}

// 输出通道设置 DC：duty cycle 占空比
static void Set_PWM_Channel(TIM_TypeDef* TIM_N, TIM_OCInitTypeDef*  TIM_OCInitStructure, u8 ch1, u16 DC)
{
	switch(ch1)
	{
		case 0:
			// 不设定
			break;
		
		case 1:
			// 通道1
			TIM_OC1Init(TIM_N, TIM_OCInitStructure); 				 								// 根据T指定的参数初始化外设
			TIM_OC1PreloadConfig(TIM_N, TIM_OCPreload_Enable); 								// 使能TIM在CCR上的预装载寄存器
			TIM_ARRPreloadConfig(TIM_N, ENABLE);															// ARPE使能 			
			// 设定占空比 按50%
			TIM_SetCompare1(TIM_N, DC);				
			break;
		
		case 2:
			// 通道2
			TIM_OC2Init(TIM_N, TIM_OCInitStructure); 				 								// 根据T指定的参数初始化外设
			TIM_OC2PreloadConfig(TIM_N, TIM_OCPreload_Enable); 								// 使能TIM在CCR上的预装载寄存器
			TIM_ARRPreloadConfig(TIM_N, ENABLE);															// ARPE使能 			
			// 设定占空比 按50%
			TIM_SetCompare2(TIM_N, DC);		
			break;
		
		case 3:
			// 通道3
			TIM_OC3Init(TIM_N, TIM_OCInitStructure); 				 								// 根据T指定的参数初始化外设
			TIM_OC3PreloadConfig(TIM_N, TIM_OCPreload_Enable); 								// 使能TIM在CCR上的预装载寄存器
			TIM_ARRPreloadConfig(TIM_N, ENABLE);															// ARPE使能 			
			// 设定占空比 按50%
			TIM_SetCompare3(TIM_N, DC);		
			break;
		
		case 4:
			// 通道4
			TIM_OC4Init(TIM_N, TIM_OCInitStructure); 				 								// 根据T指定的参数初始化外设
			TIM_OC4PreloadConfig(TIM_N, TIM_OCPreload_Enable); 								// 使能TIM在CCR上的预装载寄存器
			TIM_ARRPreloadConfig(TIM_N, ENABLE);															// ARPE使能 			
			// 设定占空比 按50%
			TIM_SetCompare4(TIM_N, DC);		
			break;
		
		default :
			respMsgError("PWM通道设定错误！\r\n", 1);
			break;
	}
}

// PWM 关闭或使能
void PWM_Cmd(TIM_TypeDef* TIM_N, FunctionalState state, u8 ch)
{
	if(ENABLE == state) 		// 开启
	{
		PWM_Forced2High(TIM_N, DISABLE, ch);
		delay_us(5);
		TIM_Cmd(TIM_N, ENABLE);
	}		
	else 										// 关闭
	{
		TIM_Cmd(TIM_N, DISABLE);  							// 关闭TIM
		delay_us(5);
		PWM_Forced2High(TIM_N, ENABLE, ch);  		// 强制拉高
	}
}

// PWM 强制拉高或取消 ch:输出通道
static void PWM_Forced2High(TIM_TypeDef* TIM_N, FunctionalState state, u8 ch)
{
	if(ENABLE == state) 		// 强制拉高
	{
		switch(ch)
		{
			case 1:
				TIM_ForcedOC1Config(TIM_N, TIM_ForcedAction_Active); 		// 强制拉高
				break;
			
			case 2:
				TIM_ForcedOC2Config(TIM_N, TIM_ForcedAction_Active); 		// 强制拉高
				break;
			
			case 3:
				TIM_ForcedOC3Config(TIM_N, TIM_ForcedAction_Active); 		// 强制拉高
				break;
			
			case 4:
				TIM_ForcedOC4Config(TIM_N, TIM_ForcedAction_Active); 		// 强制拉高
				break;
			
			default:
				respMsgError("PWM强制拉高设置有误\r\n", 1);
				break;
		}
	}		
	else 										// 取消强制拉高
	{
		switch(ch)
		{
			case 1:
				TIM_SelectOCxM(TIM_N, TIM_Channel_1, TIM_OCMode_PWM1);  // 注意定时器模式TIM_OCMode_PWM1
				TIM_CCxCmd(TIM_N, TIM_Channel_1, TIM_CCx_Enable);				
				break;
			
			case 2:
				TIM_SelectOCxM(TIM_N, TIM_Channel_2, TIM_OCMode_PWM1);  // 注意定时器模式TIM_OCMode_PWM1
				TIM_CCxCmd(TIM_N, TIM_Channel_2, TIM_CCx_Enable);				
				break;
			
			case 3:
				TIM_SelectOCxM(TIM_N, TIM_Channel_3, TIM_OCMode_PWM1);  // 注意定时器模式TIM_OCMode_PWM1
				TIM_CCxCmd(TIM_N, TIM_Channel_3, TIM_CCx_Enable);				
				break;
			
			case 4:
				TIM_SelectOCxM(TIM_N, TIM_Channel_4, TIM_OCMode_PWM1);  // 注意定时器模式TIM_OCMode_PWM1
				TIM_CCxCmd(TIM_N, TIM_Channel_4, TIM_CCx_Enable);				
				break;
			
			default:
				respMsgError("PWM取消强制拉高设置有误\r\n", 1);
				break;
		}
	}	
}







