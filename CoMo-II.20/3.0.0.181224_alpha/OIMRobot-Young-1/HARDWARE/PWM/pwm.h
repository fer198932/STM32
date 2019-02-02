#ifndef _PWM_H
#define _PWM_H
#include "sys.h"
#include "timer.h"

//////////////////////////////////////////////////////////////////////////////////	 
//  PWM生成程序  2019年1月29日20:16:48 byYJY							  
////////////////////////////////////////////////////////////////////////////////// 	

/*  TIM 定义  */
#define 		X_PWM  			TIM3
#define 		X_CH_EXTI		2 					// 对应通道 PWM_X_EXTI
#define 		X_CH_OUT		1 					// 对应通道 PWM_X_OUT

#define 		Y_PWM  			TIM2
#define 		Y_CH_EXTI		1
#define 		Y_CH_OUT		4

#define 		Z_PWM  			TIM4
#define 		Z_CH_EXTI 	3
#define 		Z_CH_OUT		4

#define 		A_PWM  			TIM9
#define 		A_CH_EXTI		1
#define 		A_CH_OUT		2

#define 		B_PWM  			TIM12
#define 		B_CH_EXTI		1
#define 		B_CH_OUT		2

// PWM初始化函数
void PWM_Init(void);

// 定时器初始化 ch1、ch2为PWM输出通道(为0啥都不干)
static void Tim_PWM_Init(GPIO_Structure_XX *GPIO_Temp, TIM_TypeDef* TIM_N, const char Pin_PWM_str[], 
	uint8_t NVIC_IRQPreemptionPriority,	uint8_t NVIC_IRQSubPriority, u16 arr, u16 psc, u8 ch1);

// 定时器输出PIN脚初始化
static void GPIO_PWM_Init(GPIO_Structure_XX *GPIO_Temp, const char Pin_PWM_str[]);

// 输出通道设置 DC：duty cycle 占空比
static void Set_PWM_Channel(TIM_TypeDef* TIM_N, TIM_OCInitTypeDef*  TIM_OCInitStructure, u8 ch1, u16 DC);

// PWM 关闭或使能
void PWM_Cmd(TIM_TypeDef* TIM_N, FunctionalState state, u8 ch);

// PWM 强制拉高或取消 ch:输出通道
static void PWM_Forced2High(TIM_TypeDef* TIM_N, FunctionalState state, u8 ch);


#endif
