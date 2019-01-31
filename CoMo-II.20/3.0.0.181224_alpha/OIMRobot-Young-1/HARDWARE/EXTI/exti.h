#ifndef __EXTI_H
#define __EXTI_H	 
#include "sys.h" 
#include "config.h"
#include "ComDataProc.h"
#include "main.h"

/********************************************************************************************************
*       中断的IO口处理程序  2019年1月29日09:26:51 byYJY
********************************************************************************************************/

// 中断线与GPIO映射关系



// 外部中断初始化		 	
void EXTI_Config_Init(void);		


// 中断服务程序
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

// 对应的IO口中断初始化
// 对应的IO口中断初始化
static void RCC_Periph_N(GPIO_Structure_XX *GPIO_Temp, const char str[], EXTITrigger_TypeDef EXTI_Trigger_N, 
	uint8_t NVIC_IRQPreemptionPriority,	uint8_t NVIC_IRQSubPriority);

// IO口的GPIO初始化
static void EXTI_GPIO_Init(GPIO_Structure_XX *GPIO_Temp, const char str[]);

// 中断使能或关闭
static void EXTI_Cmd(uint32_t EXTI_Line_N, FunctionalState state);

// 所有中断线使能
void EXTI_Enable(void);

/* 为脉冲服务的中断程序 */
#define EXTI_IRQ_PWM(n, TIM_N)  			\
do 		\
{ \
	pluNumPWM[n]++; 			\
	if(pluNumPWM[n] >= cmd_Plus_Data.plusNum[n])  	\
	{ \
		nAxis_StepMotor_Stop(TIM_N); 				\
		pluNumPWM[n] = 0; 		\
		respUsartMsg("PWM_EXTI\r\n", 10);		\
	} \
} while(0)










#endif

























