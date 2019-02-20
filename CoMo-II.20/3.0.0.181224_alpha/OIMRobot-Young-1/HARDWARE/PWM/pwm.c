//////////////////////////////////////////////////////////////////////////////////	 
//  PWM生成程序  2019年1月29日20:16:48 byYJY							  
////////////////////////////////////////////////////////////////////////////////// 

#include "pwm.h"

// 返回当前轴允许的最大、最小频率
extern u32 n_Axis_Max_Clk(u8 i);
extern u32 n_Axis_Min_Clk(u8 i);

/* PWM 初始化的结构体  */
NAxis_TIM_Structure 				nAxis_TIM_Structure[AXIS_NUM];

// PWM初始化函数
void PWM_Init(void)
{
	u8 i;
	for(i=0; i<AXIS_NUM; i++)
	{
		/*  构建PIN脚与TIM之间的映射关系  */
		nAxis2TIM(i, &nAxis_TIM_Structure[i]);
		
		/*  各轴PWM初始化  */
		Tim_PWM_Init(&nAxis_TIM_Structure[i], 0x04, 0x04, TIM_ARR, TIM_PSC, PWM_EXTI);
//		delay_us(5);
		Tim_PWM_Init(&nAxis_TIM_Structure[i], 0x04, 0x04, TIM_ARR, TIM_PSC, PWM_OUT);
//		delay_us(5);
	}
}

// 获得指定轴与TIM之间的映射关系
void nAxis2TIM(u8 nAxis, NAxis_TIM_Structure* 	nAxis_TIM_Structure)
{
//	TIM_Temp = TIM_Temp;		// 屏蔽warning 不能屏蔽！
	switch(nAxis)
	{
		case 0: 								// X轴
			nAxis_TIM_Structure->nAxis = nAxis;
			nAxis_TIM_Structure->TIM_N = X_PWM;
			nAxis_TIM_Structure->ch_exti = X_CH_EXTI;
			nAxis_TIM_Structure->ch_out = X_CH_OUT;
			if(ERROR == GPIO_Structure_Make(PWM_X_EXTI, &(nAxis_TIM_Structure->GPIO_PWM_EXTI)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			if(ERROR == GPIO_Structure_Make(PWM_X_OUT, &(nAxis_TIM_Structure->GPIO_PWM_OUT)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			break;
		case 1:									// Y轴
			nAxis_TIM_Structure->nAxis = nAxis;
			nAxis_TIM_Structure->TIM_N = Y_PWM;
			nAxis_TIM_Structure->ch_exti = Y_CH_EXTI;
			nAxis_TIM_Structure->ch_out = Y_CH_OUT;
			if(ERROR == GPIO_Structure_Make(PWM_Y_EXTI, &(nAxis_TIM_Structure->GPIO_PWM_EXTI)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			if(ERROR == GPIO_Structure_Make(PWM_Y_OUT, &(nAxis_TIM_Structure->GPIO_PWM_OUT)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			break;
		case 2:									// Z轴
			nAxis_TIM_Structure->nAxis = nAxis;
			nAxis_TIM_Structure->TIM_N = Z_PWM;
			nAxis_TIM_Structure->ch_exti = Z_CH_EXTI;
			nAxis_TIM_Structure->ch_out = Z_CH_OUT;
			if(ERROR == GPIO_Structure_Make(PWM_Z_EXTI, &(nAxis_TIM_Structure->GPIO_PWM_EXTI)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			if(ERROR == GPIO_Structure_Make(PWM_Z_OUT, &(nAxis_TIM_Structure->GPIO_PWM_OUT)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			break;
		case 3:									// A轴
			nAxis_TIM_Structure->nAxis = nAxis;
			nAxis_TIM_Structure->TIM_N = A_PWM;
			nAxis_TIM_Structure->ch_exti = A_CH_EXTI;
			nAxis_TIM_Structure->ch_out = A_CH_OUT;
			if(ERROR == GPIO_Structure_Make(PWM_A_EXTI, &(nAxis_TIM_Structure->GPIO_PWM_EXTI)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			if(ERROR == GPIO_Structure_Make(PWM_A_OUT, &(nAxis_TIM_Structure->GPIO_PWM_OUT)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			break;
		case 4:									// B轴
			nAxis_TIM_Structure->nAxis = nAxis;
			nAxis_TIM_Structure->TIM_N = B_PWM;
			nAxis_TIM_Structure->ch_exti = B_CH_EXTI;
			nAxis_TIM_Structure->ch_out = B_CH_OUT;
			if(ERROR == GPIO_Structure_Make(PWM_B_EXTI, &(nAxis_TIM_Structure->GPIO_PWM_EXTI)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			if(ERROR == GPIO_Structure_Make(PWM_B_OUT, &(nAxis_TIM_Structure->GPIO_PWM_OUT)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			break;
		default:
			respMsgError("PWM中设定的轴数过多\r\n", 1);
	}
}




// 定时器初始化 1、PWM_ch为PWM输出通道 2、中断优先级无效 可写0x04
void Tim_PWM_Init(const NAxis_TIM_Structure* 	nAxis_TIM_Structure, uint8_t NVIC_IRQPreemptionPriority, 
	uint8_t NVIC_IRQSubPriority, u16 arr, u16 psc, PWM_Channel PWM_ch)
{
	AF_Structure_XX 		TIM_AF_Structure;
	TIM_OCInitTypeDef  	TIM_OCInitStructure;
	
	// PIN脚设置有误，直接退出初始化
	if(GPIO_ERROR == nAxis_TIM_Structure->nAxis)
		return;
	
	// 定时器初始化
	Tim_Base_Init(nAxis_TIM_Structure->TIM_N, arr, psc, DISABLE, 
		NVIC_IRQPreemptionPriority, NVIC_IRQSubPriority, &TIM_AF_Structure);
	
	// GPIO 初始化
	if(PWM_EXTI == PWM_ch)  // EXTI
		GPIO_PWM_Init(&nAxis_TIM_Structure->GPIO_PWM_EXTI); 
	else 										// OUT
		GPIO_PWM_Init(&nAxis_TIM_Structure->GPIO_PWM_OUT); 	
	
	// 复用映射
	if(PWM_EXTI == PWM_ch) 	// EXTI
		GPIO_PinAFConfig(nAxis_TIM_Structure->GPIO_PWM_EXTI.GPIO_Port, 
			nAxis_TIM_Structure->GPIO_PWM_EXTI.GPIO_PinSource_N, TIM_AF_Structure.GPIO_AF);
	else										// OUT
		GPIO_PinAFConfig(nAxis_TIM_Structure->GPIO_PWM_OUT.GPIO_Port, 
			nAxis_TIM_Structure->GPIO_PWM_OUT.GPIO_PinSource_N, TIM_AF_Structure.GPIO_AF);
	
	// 初始化PWM输出通道
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 								// 选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 		// 比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 				// 输出极性 注：默认拉高  byYJY
	
	// 设置输出通道
	if(PWM_EXTI == PWM_ch) 	// EXTI
		Set_PWM_Channel(nAxis_TIM_Structure->TIM_N, &TIM_OCInitStructure, 
			nAxis_TIM_Structure->ch_exti, (arr>>1));
	else										// OUT
		Set_PWM_Channel(nAxis_TIM_Structure->TIM_N, &TIM_OCInitStructure, 
			nAxis_TIM_Structure->ch_out, (arr>>1));
	
	// 关闭PWM，并强制拉高
	if(PWM_EXTI == PWM_ch) 	// EXTI
		PWM_Cmd(nAxis_TIM_Structure->TIM_N, DISABLE, nAxis_TIM_Structure->ch_exti);	
	else										// OUT
		PWM_Cmd(nAxis_TIM_Structure->TIM_N, DISABLE, nAxis_TIM_Structure->ch_out);	
}

// 定时器输出PIN脚初始化
static void GPIO_PWM_Init(const GPIO_Structure_XX *GPIO_Temp)
{
	GPIO_InitTypeDef 		GPIO_InitStructure;
	
	// GPIO结构体构造
//	if(!GPIO_Structure_Make(Pin_PWM_str, GPIO_Temp))
//		return;	
	
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
			TIM_OC1Init(TIM_N, TIM_OCInitStructure); 				 									// 根据T指定的参数初始化外设
			TIM_OC1PreloadConfig(TIM_N, TIM_OCPreload_Enable); 								// 使能TIM在CCR上的预装载寄存器
			TIM_ARRPreloadConfig(TIM_N, ENABLE);															// ARPE使能 			
			// 设定占空比 按50%
			TIM_SetCompare1(TIM_N, DC);				
			break;
		
		case 2:
			// 通道2
			TIM_OC2Init(TIM_N, TIM_OCInitStructure); 				 									// 根据T指定的参数初始化外设
			TIM_OC2PreloadConfig(TIM_N, TIM_OCPreload_Enable); 								// 使能TIM在CCR上的预装载寄存器
			TIM_ARRPreloadConfig(TIM_N, ENABLE);															// ARPE使能 			
			// 设定占空比 按50%
			TIM_SetCompare2(TIM_N, DC);		
			break;
		
		case 3:
			// 通道3
			TIM_OC3Init(TIM_N, TIM_OCInitStructure); 				 									// 根据T指定的参数初始化外设
			TIM_OC3PreloadConfig(TIM_N, TIM_OCPreload_Enable); 								// 使能TIM在CCR上的预装载寄存器
			TIM_ARRPreloadConfig(TIM_N, ENABLE);															// ARPE使能 			
			// 设定占空比 按50%
			TIM_SetCompare3(TIM_N, DC);		
			break;
		
		case 4:
			// 通道4
			TIM_OC4Init(TIM_N, TIM_OCInitStructure); 				 									// 根据T指定的参数初始化外设
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
		PWM_Forced2High(TIM_N, DISABLE, ch); 		// 取消强制拉高
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

// PWM 关闭或使能 宏定义版本
//#define	PWM_Cmd(TIM_N, state, ch)  \
//do 	\
//{ \
//	if(ENABLE == state) 		/* 开启  */ \
//	{ 	\
//		PWM_Forced2High(TIM_N, DISABLE, ch); \
//		delay_us(5); \
//		TIM_Cmd(TIM_N, ENABLE); \
//	}		\
//	else 										/* 关闭  */ \
//	{ \
//		TIM_Cmd(TIM_N, DISABLE);  							/* 关闭TIM */ \
//		delay_us(5); \
//		PWM_Forced2High(TIM_N, ENABLE, ch);  		/* 强制拉高 */ \
//	}	 \
//} while(0)

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

// 通过CLK频率计算预分频系数 PSC 注意寄存器溢出问题
u16 calPSC(u32 clk, u8 nAxis)
{
	u16 psc;
	if((clk >= n_Axis_Min_Clk(nAxis)) && (clk <= n_Axis_Max_Clk(nAxis)))
	{
		psc = PSC_CLK / clk;
	}
	else
	{
		psc = PSC_CLK / n_Axis_Min_Clk(nAxis);
	}
	return psc;
}

// 通过PSC计算频率， 注意寄存器溢出问题  单位HZ
u32 calClk_PSC(u16 psc)
{
	u32 clk;
	clk = PSC_CLK / psc;
	return clk;
}



