/********************************************************************************************************
*   通过宏定义构造初始化用的结构体
********************************************************************************************************/
#include "macro_operate.h"

extern void respMsgError(const char str[], u8 status);

// 根据字符串构造初始化的GPIO结构体
ErrorStatus GPIO_Structure_Make(const char str[], GPIO_Structure_XX *GPIO_Temp)
{	
	if('P' != str[0])
	{
		respMsgError("构造GPIO错误\r\n", 1);
		return ERROR;
	}
	
	switch(str[1])
	{
		case 'A':
			GPIO_Temp->GPIO_Port = GPIOA;
			GPIO_Temp->RCC_Periph_N = RCC_AHB1Periph_GPIOA;
			GPIO_Temp->EXTI_PortGPIO_N = EXTI_PortSourceGPIOA;
			break;
		
		case 'B':
			GPIO_Temp->GPIO_Port = GPIOB;
			GPIO_Temp->RCC_Periph_N = RCC_AHB1Periph_GPIOB;
			GPIO_Temp->EXTI_PortGPIO_N = EXTI_PortSourceGPIOB;
			break;
		
		case 'C':
			GPIO_Temp->GPIO_Port = GPIOC;
			GPIO_Temp->RCC_Periph_N = RCC_AHB1Periph_GPIOC;
			GPIO_Temp->EXTI_PortGPIO_N = EXTI_PortSourceGPIOC;
			break;
		
		case 'D':
			GPIO_Temp->GPIO_Port = GPIOD;
			GPIO_Temp->RCC_Periph_N = RCC_AHB1Periph_GPIOD;
			GPIO_Temp->EXTI_PortGPIO_N = EXTI_PortSourceGPIOD;
			break;
		
		case 'E':
			GPIO_Temp->GPIO_Port = GPIOE;
			GPIO_Temp->RCC_Periph_N = RCC_AHB1Periph_GPIOE;
			GPIO_Temp->EXTI_PortGPIO_N = EXTI_PortSourceGPIOE;
			break;
		
		case 'F':
			GPIO_Temp->GPIO_Port = GPIOF;
			GPIO_Temp->RCC_Periph_N = RCC_AHB1Periph_GPIOF;
			GPIO_Temp->EXTI_PortGPIO_N = EXTI_PortSourceGPIOF;
			break;
		
		case 'G':
			GPIO_Temp->GPIO_Port = GPIOG;
			GPIO_Temp->RCC_Periph_N = RCC_AHB1Periph_GPIOG;
			GPIO_Temp->EXTI_PortGPIO_N = EXTI_PortSourceGPIOG;
			break;
		
		case 'H':
			GPIO_Temp->GPIO_Port = GPIOH;
			GPIO_Temp->RCC_Periph_N = RCC_AHB1Periph_GPIOH;
			GPIO_Temp->EXTI_PortGPIO_N = EXTI_PortSourceGPIOH;
			break;
		
		case 'I':
			GPIO_Temp->GPIO_Port = GPIOI;
			GPIO_Temp->RCC_Periph_N = RCC_AHB1Periph_GPIOI;
			GPIO_Temp->EXTI_PortGPIO_N = EXTI_PortSourceGPIOI;
			break;
		
		case 'J':
			GPIO_Temp->GPIO_Port = GPIOJ;
			GPIO_Temp->RCC_Periph_N = RCC_AHB1Periph_GPIOJ;
			GPIO_Temp->EXTI_PortGPIO_N = EXTI_PortSourceGPIOJ;
			break;
		
		case 'K':
			GPIO_Temp->GPIO_Port = GPIOK;
			GPIO_Temp->RCC_Periph_N = RCC_AHB1Periph_GPIOK;
			GPIO_Temp->EXTI_PortGPIO_N = EXTI_PortSourceGPIOK;
			break;
		
		default:
			respMsgError("构造GPIO错误\r\n", 1);
			return ERROR;
		
	}
	
	switch(str[2])
	{		
		case '0':
			GPIO_Temp->GPIO_Pin_N = GPIO_Pin_0;
			GPIO_Temp->GPIO_PinSource_N = GPIO_PinSource0;
			GPIO_Temp->EXTI_PinSource_N = EXTI_PinSource0; 
			GPIO_Temp->EXTI_Line_N = EXTI_Line0;
			GPIO_Temp->NVIC_IRQ_N = EXTI0_IRQn;
			break;
		
		case '1':
			if('\0' == str[3])
			{
				GPIO_Temp->GPIO_Pin_N = GPIO_Pin_1;
				GPIO_Temp->GPIO_PinSource_N = GPIO_PinSource1;
				GPIO_Temp->EXTI_PinSource_N = EXTI_PinSource1;
				GPIO_Temp->EXTI_Line_N = EXTI_Line1;
				GPIO_Temp->NVIC_IRQ_N = EXTI1_IRQn;
			}
			else
			{
				switch(str[3])
				{
					case '0':
						GPIO_Temp->GPIO_Pin_N = GPIO_Pin_10;
						GPIO_Temp->GPIO_PinSource_N = GPIO_PinSource10;
						GPIO_Temp->EXTI_PinSource_N = EXTI_PinSource10;
						GPIO_Temp->EXTI_Line_N = EXTI_Line10;
						GPIO_Temp->NVIC_IRQ_N = EXTI15_10_IRQn;
						break;
					
					case '1':
						GPIO_Temp->GPIO_Pin_N = GPIO_Pin_11;
						GPIO_Temp->GPIO_PinSource_N = GPIO_PinSource11;
						GPIO_Temp->EXTI_PinSource_N = EXTI_PinSource11;
						GPIO_Temp->EXTI_Line_N = EXTI_Line11;
						GPIO_Temp->NVIC_IRQ_N = EXTI15_10_IRQn;
						break;
					
					case '2':
						GPIO_Temp->GPIO_Pin_N = GPIO_Pin_12;
						GPIO_Temp->GPIO_PinSource_N = GPIO_PinSource12;
						GPIO_Temp->EXTI_PinSource_N = EXTI_PinSource12;
						GPIO_Temp->EXTI_Line_N = EXTI_Line12;
						GPIO_Temp->NVIC_IRQ_N = EXTI15_10_IRQn;
						break;
					
					case '3':
						GPIO_Temp->GPIO_Pin_N = GPIO_Pin_13;
						GPIO_Temp->GPIO_PinSource_N = GPIO_PinSource13;
						GPIO_Temp->EXTI_PinSource_N = EXTI_PinSource13;
						GPIO_Temp->EXTI_Line_N = EXTI_Line13;
						GPIO_Temp->NVIC_IRQ_N = EXTI15_10_IRQn;
						break;
					
					case '4':
						GPIO_Temp->GPIO_Pin_N = GPIO_Pin_14;
						GPIO_Temp->GPIO_PinSource_N = GPIO_PinSource14;
						GPIO_Temp->EXTI_PinSource_N = EXTI_PinSource14;
						GPIO_Temp->EXTI_Line_N = EXTI_Line14;
						GPIO_Temp->NVIC_IRQ_N = EXTI15_10_IRQn;
						break;
					
					case '5':
						GPIO_Temp->GPIO_Pin_N = GPIO_Pin_15;
						GPIO_Temp->GPIO_PinSource_N = GPIO_PinSource15;
						GPIO_Temp->EXTI_PinSource_N = EXTI_PinSource15;
						GPIO_Temp->EXTI_Line_N = EXTI_Line15;
						GPIO_Temp->NVIC_IRQ_N = EXTI15_10_IRQn;
						break;
					
					default:
						respMsgError("构造GPIO错误\r\n", 1);
						return ERROR;					
				}				
			}
			break;
			
		case '2':
			GPIO_Temp->GPIO_Pin_N = GPIO_Pin_2;
			GPIO_Temp->GPIO_PinSource_N = GPIO_PinSource2;
			GPIO_Temp->EXTI_PinSource_N = EXTI_PinSource2;
			GPIO_Temp->EXTI_Line_N = EXTI_Line2;
			GPIO_Temp->NVIC_IRQ_N = EXTI2_IRQn;
			break;
		
		case '3':
			GPIO_Temp->GPIO_Pin_N = GPIO_Pin_3;
			GPIO_Temp->GPIO_PinSource_N = GPIO_PinSource3;
			GPIO_Temp->EXTI_PinSource_N = EXTI_PinSource3;
			GPIO_Temp->EXTI_Line_N = EXTI_Line3;
			GPIO_Temp->NVIC_IRQ_N = EXTI3_IRQn;
			break;
		
		case '4':
			GPIO_Temp->GPIO_Pin_N = GPIO_Pin_4;
			GPIO_Temp->GPIO_PinSource_N = GPIO_PinSource4;
			GPIO_Temp->EXTI_PinSource_N = EXTI_PinSource4;
			GPIO_Temp->EXTI_Line_N = EXTI_Line4;
			GPIO_Temp->NVIC_IRQ_N = EXTI4_IRQn;
			break;
		
		case '5':
			GPIO_Temp->GPIO_Pin_N = GPIO_Pin_5;
			GPIO_Temp->GPIO_PinSource_N = GPIO_PinSource5;
			GPIO_Temp->EXTI_PinSource_N = EXTI_PinSource5;
			GPIO_Temp->EXTI_Line_N = EXTI_Line5;
			GPIO_Temp->NVIC_IRQ_N = EXTI9_5_IRQn;
			break;
		
		case '6':
			GPIO_Temp->GPIO_Pin_N = GPIO_Pin_6;
			GPIO_Temp->GPIO_PinSource_N = GPIO_PinSource6;
			GPIO_Temp->EXTI_PinSource_N = EXTI_PinSource6;
			GPIO_Temp->EXTI_Line_N = EXTI_Line6;
			GPIO_Temp->NVIC_IRQ_N = EXTI9_5_IRQn;
			break;
		
		case '7':
			GPIO_Temp->GPIO_Pin_N = GPIO_Pin_7;
			GPIO_Temp->GPIO_PinSource_N = GPIO_PinSource7;
			GPIO_Temp->EXTI_PinSource_N = EXTI_PinSource7;
			GPIO_Temp->EXTI_Line_N = EXTI_Line7;
			GPIO_Temp->NVIC_IRQ_N = EXTI9_5_IRQn;
			break;
		
		case '8':
			GPIO_Temp->GPIO_Pin_N = GPIO_Pin_8;
			GPIO_Temp->GPIO_PinSource_N = GPIO_PinSource8;
			GPIO_Temp->EXTI_PinSource_N = EXTI_PinSource8;
			GPIO_Temp->EXTI_Line_N = EXTI_Line8;
			GPIO_Temp->NVIC_IRQ_N = EXTI9_5_IRQn;
			break;
			
		case '9':
			GPIO_Temp->GPIO_Pin_N = GPIO_Pin_9;
			GPIO_Temp->GPIO_PinSource_N = GPIO_PinSource9;
			GPIO_Temp->EXTI_PinSource_N = EXTI_PinSource9;
			GPIO_Temp->EXTI_Line_N = EXTI_Line9;
			GPIO_Temp->NVIC_IRQ_N = EXTI9_5_IRQn;
			break;
		
		default:
			respMsgError("构造GPIO错误\r\n", 1);
			return ERROR;		
	}
	return SUCCESS;
}

// 构造TIM的结构体
ErrorStatus TIM_Structure_Make(TIM_TypeDef* TIM_N, AF_Structure_XX *TIM_Structure)
{
	// 映射
	TIM_Structure->AF_TypeDef = TIM_N;
	
	switch((u32)TIM_N)
	{
		case (u32)TIM1:
			TIM_Structure->RCC_Periph_N = RCC_APB2Periph_TIM1;			// 时钟线
//			TIM_Structure->IRQ_Channel_N = TIM1_IRQn;								// 中断服务程序
			TIM_Structure->GPIO_AF = GPIO_AF_TIM1; 									// 复用功能
			break;
		
		case (u32)TIM2:
			TIM_Structure->RCC_Periph_N = RCC_APB1Periph_TIM2;			// 时钟线
			TIM_Structure->IRQ_Channel_N = TIM2_IRQn;								// 中断服务程序
			TIM_Structure->GPIO_AF = GPIO_AF_TIM2; 									// 复用功能
			break;
		
		case (u32)TIM3:
			TIM_Structure->RCC_Periph_N = RCC_APB1Periph_TIM3;			// 时钟线
			TIM_Structure->IRQ_Channel_N = TIM3_IRQn;								// 中断服务程序
			TIM_Structure->GPIO_AF = GPIO_AF_TIM3; 									// 复用功能
			break;
		
		case (u32)TIM4:
			TIM_Structure->RCC_Periph_N = RCC_APB1Periph_TIM4;			// 时钟线
			TIM_Structure->IRQ_Channel_N = TIM4_IRQn;								// 中断服务程序
			TIM_Structure->GPIO_AF = GPIO_AF_TIM4; 									// 复用功能
			break;
		
		case (u32)TIM5:
			TIM_Structure->RCC_Periph_N = RCC_APB1Periph_TIM5;			// 时钟线
			TIM_Structure->IRQ_Channel_N = TIM5_IRQn;								// 中断服务程序
			TIM_Structure->GPIO_AF = GPIO_AF_TIM5; 									// 复用功能
			break;
		
		case (u32)TIM6:
			TIM_Structure->RCC_Periph_N = RCC_APB1Periph_TIM6;			// 时钟线
//			TIM_Structure->IRQ_Channel_N = TIM6_IRQn;								// 中断服务程序
//			TIM_Structure->GPIO_AF = GPIO_AF_TIM6; 									// 复用功能
			break;
		
		case (u32)TIM7:
			TIM_Structure->RCC_Periph_N = RCC_APB1Periph_TIM7;			// 时钟线
			TIM_Structure->IRQ_Channel_N = TIM7_IRQn;								// 中断服务程序
//			TIM_Structure->GPIO_AF = GPIO_AF_TIM7; 									// 复用功能
			break;
		
		case (u32)TIM9:
			TIM_Structure->RCC_Periph_N = RCC_APB2Periph_TIM9;			// 时钟线
//			TIM_Structure->IRQ_Channel_N = TIM9_IRQn;								// 中断服务程序
			TIM_Structure->GPIO_AF = GPIO_AF_TIM9; 									// 复用功能
			break;
		
		case (u32)TIM11:
			TIM_Structure->RCC_Periph_N = RCC_APB2Periph_TIM11;					// 时钟线
	//		TIM_Structure->IRQ_Channel_N = TIM11_IRQn;								// 中断服务程序
			TIM_Structure->GPIO_AF = GPIO_AF_TIM11; 									// 复用功能
			break;
		
		case (u32)TIM12:
			TIM_Structure->RCC_Periph_N = RCC_APB1Periph_TIM12;			// 时钟线
//			TIM_Structure->IRQ_Channel_N = TIM12_IRQn;								// 中断服务程序
			TIM_Structure->GPIO_AF = GPIO_AF_TIM12; 									// 复用功能
			break;
		
		case (u32)TIM14:
			TIM_Structure->RCC_Periph_N = RCC_APB1Periph_TIM14;			// 时钟线
//			TIM_Structure->IRQ_Channel_N = TIM14_IRQn;								// 中断服务程序
			TIM_Structure->GPIO_AF = GPIO_AF_TIM14; 									// 复用功能
			break;
		
		default:
			respMsgError("TIM设置错误！\r\n", 1);
			return ERROR;
	}
	return SUCCESS;
}

// 构造PWM的结构体
//void PWM_Structure_Make(const char str[], GPIO_Structure_XX *GPIO_Temp, 
//	PWM_Structure_XX* PWM_Temp)
//{
//	// 映射
//	PWM_Temp->GPIO_PWM_N = GPIO_Temp;
//	
//	GPIO_Structure_Make(str, GPIO_Temp);
//	
//	switch(str[1])
//	{
//		case 'A':
//			
//			break;
//		
//		default:
//			respMsgError("构造GPIO错误\r\n", 1);
//			break;
//		
//	}
//}



