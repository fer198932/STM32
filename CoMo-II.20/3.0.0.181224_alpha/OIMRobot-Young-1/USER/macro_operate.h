#ifndef __MACRO_OPERATE_H
#define __MACRO_OPERATE_H
#include "sys.h"
/* ��������� */

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

typedef enum {X_Axis = 0, Y_Axis = 1, Z_Axis = 2, A_Axis = 3, B_Axis = 4, ALL_Axis = 5} N_Axis;


// ���ù��ܽṹ��
typedef struct {
	uint32_t RCC_Periph_N;  							// ʱ���� �磺RCC_APB1Periph_USART3 
	uint8_t GPIO_AF;											// ���ù��� �磺GPIO_AF_USART1
	uint8_t IRQ_Channel_N;								// �ж�ͨ�� 
	uint16_t IRQ_Type;										// �ж����� �磺USART_IT_IDLE - ���ڿ����ж�
	void* 	AF_TypeDef;										// ָ���ù��ܵĽṹ�� �磺TIM1
} AF_Structure_XX;

// GPIO�ṹ��
typedef struct {
	uint16_t 				GPIO_Pin_N;								// pin��
	GPIO_TypeDef 		*GPIO_Port;								// �˿ں�  �磺GPIOA
	uint32_t 				RCC_Periph_N;							// ʱ����
	
	uint16_t 				GPIO_PinSource_N; 				// �磺GPIO_PinSource10
	
	uint8_t 				EXTI_PortGPIO_N; 					// GPIO�ж϶˿�
	uint8_t 				EXTI_PinSource_N;					// ָ�����ж���Pin��
	uint32_t 				EXTI_Line_N;							// �ж���
	uint8_t 				NVIC_IRQ_N;								// ��Ӧ�жϷ������
	
//	uint32_t GPIOx_ODR_Addr;				// �����ַӳ��
	
	AF_Structure_XX* AF_Structure;							 	// ָ���PIN�Ÿ��õĽṹ��
	
} GPIO_Structure_XX;

//// PWM�ṹ��
//typedef struct {
//	TIM_TypeDef* 				TIM_N; 							// ��ʱ��
//	uint16_t 						TIM_Channel_N; 			// ��Ӧ��ͨ��
//	GPIO_Structure_XX*	GPIO_PWM_N;	 				// ָ���GPIO��		
//} PWM_Structure_XX;

// ���GPIO��pin��
// #define GPIO_Pin_N(GPIO_Pin) 		GPIO_Pin_9

// �����ַ��������ʼ����GPIO�ṹ��
ErrorStatus GPIO_Structure_Make(const char str[], GPIO_Structure_XX *GPIO_Temp);

// ����TIM�Ľṹ��
ErrorStatus TIM_Structure_Make(TIM_TypeDef* TIM_N, AF_Structure_XX *TIM_Structure);


#endif
