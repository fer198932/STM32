#ifndef _PWM_H
#define _PWM_H
#include "sys.h"
#include "timer.h"

//////////////////////////////////////////////////////////////////////////////////	 
//  PWM���ɳ���  2019��1��29��20:16:48 byYJY							  
////////////////////////////////////////////////////////////////////////////////// 	

/*  TIM ����  */
#define 		X_PWM  			TIM3
#define 		X_CH1_EXTI	1 					// ��Ӧͨ�� PWM_X_EXTI
#define 		X_CH2_OUT		2 					// ��Ӧͨ�� PWM_X_OUT

#define 		Y_PWM  			TIM2
#define 		Y_CH1_EXTI	1
#define 		Y_CH2_OUT		4

#define 		Z_PWM  			TIM4
#define 		Z_CH1_EXTI 	3
#define 		Z_CH2_OUT		4

#define 		A_PWM  			TIM9
#define 		A_CH1_EXTI	1
#define 		A_CH2_OUT		2

#define 		B_PWM  			TIM12
#define 		B_CH1_EXTI	1
#define 		B_CH2_OUT		2

// PWM��ʼ������
void PWM_Init(void);

// ��ʱ����ʼ�� ch1��ch2ΪPWM���ͨ��(Ϊ0ɶ������)
static void Tim_PWM_Init(GPIO_Structure_XX *GPIO_Temp, TIM_TypeDef* TIM_N, const char Pin_PWM_str[], 
	uint8_t NVIC_IRQPreemptionPriority,	uint8_t NVIC_IRQSubPriority, u16 arr, u16 psc, u8 ch1);

// ��ʱ�����PIN�ų�ʼ��
static void GPIO_PWM_Init(GPIO_Structure_XX *GPIO_Temp, const char Pin_PWM_str[]);

// ���ͨ������ DC��duty cycle ռ�ձ�
static void Set_PWM_Channel(TIM_TypeDef* TIM_N, TIM_OCInitTypeDef*  TIM_OCInitStructure, u8 ch1, u16 DC);

// PWM �رջ�ʹ��
void PWM_Cmd(TIM_TypeDef* TIM_N, FunctionalState state, u8 ch);

// PWM ǿ�����߻�ȡ�� ch:���ͨ��
static void PWM_Forced2High(TIM_TypeDef* TIM_N, FunctionalState state, u8 ch);


#endif
