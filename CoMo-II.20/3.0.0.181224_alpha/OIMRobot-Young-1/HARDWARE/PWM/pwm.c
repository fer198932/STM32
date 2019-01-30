//////////////////////////////////////////////////////////////////////////////////	 
//  PWM���ɳ���  2019��1��29��20:16:48 byYJY							  
////////////////////////////////////////////////////////////////////////////////// 

#include "pwm.h"

/* PWM ��ʼ���Ľṹ��  */
static  	GPIO_Structure_XX 		GPIO_PWM_Plus[AXIS_NUM];
// static 		AF_Structure_XX 			AF_PWM_Plus[AXIS_NUM];

// PWM��ʼ������
void PWM_Init(void)
{
	
	
	
}

// ��ʱ����ʼ��
static void Tim_PWM_Init(GPIO_Structure_XX *GPIO_Temp, TIM_TypeDef* TIM_N, 
	const char Pin_PWM_str[], EXTITrigger_TypeDef EXTI_Trigger_N, 
	uint8_t NVIC_IRQPreemptionPriority,	uint8_t NVIC_IRQSubPriority, u16 arr, u16 psc)
{
	AF_Structure_XX TIM_AF_Structure;
	
	// GPIO ��ʼ��
	GPIO_PWM_Init(GPIO_Temp, Pin_PWM_str); 	
	
	// ��ʱ����ʼ��
	Tim_Base_Init(TIM_N, arr, psc, DISABLE, 
		NVIC_IRQPreemptionPriority, NVIC_IRQSubPriority, &TIM_AF_Structure);
	
	// ����ӳ��
	GPIO_PinAFConfig(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_PinSource_N, 
		TIM_AF_Structure.GPIO_AF);
	
//	TIM_TimeBaseInitTypeDef 	PWM_TimeBaseStructure;
//	TIM_OCInitTypeDef  				PWM_OCInitStructure;
//	AF_Structure_XX 					PWM_AF_Structure;

//	// ����PWM���ýṹ��
//	TIM_Structure_Init(TIM_N, &PWM_AF_Structure);
//	
//	// ʹ��ʱ��
//	if((u32)TIM1 > (u32)TIM_N) 			// ʱ���� TIM2��3��4��5��6��7��12��13��14
//		RCC_APB1PeriphClockCmd(PWM_AF_Structure.RCC_Periph_N, ENABLE);
//	else 														// ʱ���� TIM1��8��9��10��11
//		RCC_APB2PeriphClockCmd(PWM_AF_Structure.RCC_Periph_N, ENABLE);
	
	
	
}

// ��ʱ�����PIN�ų�ʼ��
static void GPIO_PWM_Init(GPIO_Structure_XX *GPIO_Temp, const char Pin_PWM_str[])
{
	GPIO_InitTypeDef 		GPIO_InitStructure;
	
	// GPIO�ṹ�幹��
	GPIO_Structure_Init(Pin_PWM_str, GPIO_Temp);	
	
	// ʹ��ʱ��
	RCC_AHB1PeriphClockCmd(GPIO_Temp->RCC_Periph_N, ENABLE);
	
	// GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Temp->GPIO_Pin_N;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  			// �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	// 100M
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 			// ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  			// ���� 
	
	GPIO_Init(GPIO_Temp->GPIO_Port, &GPIO_InitStructure);
}











