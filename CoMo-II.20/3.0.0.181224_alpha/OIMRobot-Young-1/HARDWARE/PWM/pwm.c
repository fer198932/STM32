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
	
	
	
}

// 定时器初始化
static void Tim_PWM_Init(GPIO_Structure_XX *GPIO_Temp, TIM_TypeDef* TIM_N, 
	const char Pin_PWM_str[], EXTITrigger_TypeDef EXTI_Trigger_N, 
	uint8_t NVIC_IRQPreemptionPriority,	uint8_t NVIC_IRQSubPriority, u16 arr, u16 psc)
{
	AF_Structure_XX TIM_AF_Structure;
	
	// GPIO 初始化
	GPIO_PWM_Init(GPIO_Temp, Pin_PWM_str); 	
	
	// 定时器初始化
	Tim_Base_Init(TIM_N, arr, psc, DISABLE, 
		NVIC_IRQPreemptionPriority, NVIC_IRQSubPriority, &TIM_AF_Structure);
	
	// 复用映射
	GPIO_PinAFConfig(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_PinSource_N, 
		TIM_AF_Structure.GPIO_AF);
	
//	TIM_TimeBaseInitTypeDef 	PWM_TimeBaseStructure;
//	TIM_OCInitTypeDef  				PWM_OCInitStructure;
//	AF_Structure_XX 					PWM_AF_Structure;

//	// 构造PWM复用结构体
//	TIM_Structure_Init(TIM_N, &PWM_AF_Structure);
//	
//	// 使能时钟
//	if((u32)TIM1 > (u32)TIM_N) 			// 时钟线 TIM2、3、4、5、6、7、12、13、14
//		RCC_APB1PeriphClockCmd(PWM_AF_Structure.RCC_Periph_N, ENABLE);
//	else 														// 时钟线 TIM1、8、9、10、11
//		RCC_APB2PeriphClockCmd(PWM_AF_Structure.RCC_Periph_N, ENABLE);
	
	
	
}

// 定时器输出PIN脚初始化
static void GPIO_PWM_Init(GPIO_Structure_XX *GPIO_Temp, const char Pin_PWM_str[])
{
	GPIO_InitTypeDef 		GPIO_InitStructure;
	
	// GPIO结构体构造
	GPIO_Structure_Init(Pin_PWM_str, GPIO_Temp);	
	
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











