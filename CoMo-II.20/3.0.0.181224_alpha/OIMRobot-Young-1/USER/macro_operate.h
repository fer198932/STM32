#ifndef __MACRO_OPERATE_H
#define __MACRO_OPERATE_H
#include "sys.h"
/* ��������� */

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

// ���GPIO��pin��
// #define GPIO_Pin_N(GPIO_Pin) 		GPIO_Pin_9

// �����ַ��������ʼ����GPIO�ṹ��
void GPIO_Structure_Init(const char str[], GPIO_Structure_XX *GPIO_Temp);

// ����TIM�Ľṹ��
void TIM_Structure_Init(TIM_TypeDef* TIM_N, AF_Structure_XX *TIM_Structure);


#endif
