#ifndef __MACRO_OPERATE_H
#define __MACRO_OPERATE_H
#include "sys.h"
/* 宏操作函数 */

// 复用功能结构体
typedef struct {
	uint32_t RCC_Periph_N;  							// 时钟线 如：RCC_APB1Periph_USART3 
	uint8_t GPIO_AF;											// 复用功能 如：GPIO_AF_USART1
	uint8_t IRQ_Channel_N;								// 中断通道 
	uint16_t IRQ_Type;										// 中断类型 如：USART_IT_IDLE - 串口空闲中断
	void* 	AF_TypeDef;										// 指向复用功能的结构体 如：TIM1
} AF_Structure_XX;

// GPIO结构体
typedef struct {
	uint16_t 				GPIO_Pin_N;								// pin脚
	GPIO_TypeDef 		*GPIO_Port;								// 端口号  如：GPIOA
	uint32_t 				RCC_Periph_N;							// 时钟线
	
	uint16_t 				GPIO_PinSource_N; 				// 如：GPIO_PinSource10
	
	uint8_t 				EXTI_PortGPIO_N; 					// GPIO中断端口
	uint8_t 				EXTI_PinSource_N;					// 指定的中断线Pin脚
	uint32_t 				EXTI_Line_N;							// 中断线
	uint8_t 				NVIC_IRQ_N;								// 对应中断服务程序
	
//	uint32_t GPIOx_ODR_Addr;				// 输出地址映射
	
	AF_Structure_XX* AF_Structure;							 	// 指向该PIN脚复用的结构体
	
} GPIO_Structure_XX;

//// PWM结构体
//typedef struct {
//	TIM_TypeDef* 				TIM_N; 							// 定时器
//	uint16_t 						TIM_Channel_N; 			// 对应的通道
//	GPIO_Structure_XX*	GPIO_PWM_N;	 				// 指向的GPIO口		
//} PWM_Structure_XX;

// 获得GPIO的pin脚
// #define GPIO_Pin_N(GPIO_Pin) 		GPIO_Pin_9

// 根据字符串构造初始化的GPIO结构体
ErrorStatus GPIO_Structure_Make(const char str[], GPIO_Structure_XX *GPIO_Temp);

// 构造TIM的结构体
ErrorStatus TIM_Structure_Make(TIM_TypeDef* TIM_N, AF_Structure_XX *TIM_Structure);


#endif
