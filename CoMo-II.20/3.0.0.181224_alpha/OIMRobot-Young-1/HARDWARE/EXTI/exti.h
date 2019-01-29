#ifndef __EXTI_H
#define __EXTI_H	 
#include "sys.h" 
#include "config.h"
#include "ComDataProc.h"
#include "main.h"

/********************************************************************************************************
*       �жϵ�IO�ڴ������  2019��1��29��09:26:51 byYJY
********************************************************************************************************/

// �ж�����GPIOӳ���ϵ



// �ⲿ�жϳ�ʼ��		 	
void EXTI_Config_Init(void);		


// �жϷ������
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

// ��Ӧ��IO���жϳ�ʼ��
// ��Ӧ��IO���жϳ�ʼ��
static void RCC_Periph_N(GPIO_Structure_XX *GPIO_Temp, const char str[], EXTITrigger_TypeDef EXTI_Trigger_N, 
	uint8_t NVIC_IRQPreemptionPriority,	uint8_t NVIC_IRQSubPriority);

// IO�ڵ�GPIO��ʼ��
static void EXTI_GPIO_Init(GPIO_Structure_XX *GPIO_Temp, const char str[]);

// �ж�ʹ�ܻ�ر�
static void EXTI_Cmd(uint32_t EXTI_Line_N, FunctionalState state);

// �����ж���ʹ��
void EXTI_Enable(void);

/* Ϊ���������жϳ��� */
#define EXTI_IRQ_PWM  			\
do 		\
{ \
	 respUsartMsg("KEY1\r\n", 6); 		\
} while(0)










#endif

























