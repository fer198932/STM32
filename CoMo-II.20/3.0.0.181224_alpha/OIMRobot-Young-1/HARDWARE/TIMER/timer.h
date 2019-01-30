#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
#include "config.h"
#include "ComDataProc.h"
//////////////////////////////////////////////////////////////////////////////////	 
// ��ʱ�� 2019��1��29��20:43:08 byYJY
////////////////////////////////////////////////////////////////////////////////// 	

/****   ��ʽ�� TIM_OUT = TIM_ARR * TIM_PSC / TIM_CLK   *******/
#define 	TIM_OUT						20																	// ���ʱ�䣬  ��λus	 ȡֵ��Χ��3~21854
#define 	TIM_FREQ					(1e6 / TIM_OUT) 										// ���Ƶ��   ��λHz  �磺��ǰΪ50kHz
#define  	TIM_CLK 					(_SYSCLK/2) 												// ��ʱ��ʱ�ӣ���λMHz (84MHz)
#define 	TIM_ARR 					28 																	// ��װ��ֵ
#define 	TIM_PSC 					(TIM_CLK * TIM_OUT / TIM_ARR) 			// Ԥ��Ƶϵ��



#define 		ADDSUB_TIMER 			TIM7 						// �Ӽ���ʹ�õĶ�ʱ��
#define		 	TIM_IRQ_ADDSUB 		/* �Ӽ���ʹ�õ��жϷ������ */   \
do 		\
{ 			\
	temp++;					\
	if(1e6 < temp)  \
	{ \
		respUsartMsg("TIM7\r\n", 6); 		\
		temp = 0; 			\
	} \
} while(0)





// ��ʱ����ʼ��
void Timer_Init(void);


// ��ʱ����ʼ������ ע��arr����Ϊu32
void Tim_Base_Init(TIM_TypeDef* TIM_N, u16 arr, u16 psc, FunctionalState Enable_IRQ,
	uint8_t IRQPreempPriority, uint8_t IRQSubPriority, AF_Structure_XX* TIM_AF_Structure);


/* ��ʱ���ĺ���� */
//#define 		RCC_APB1_TIMER(n)  		RCC_APB1Periph_TIM##n			// ʱ���� TIM2��3��4��5��6��7��12��13��14
//#define 		RCC_APB2_TIMER(n)  		RCC_APB2Periph_TIM##n			// ʱ���� TIM1��8��9��10��11
//#define			TIM_TYPE(n)						TIM##n										// ��ʱ�����磺TIM7
//#define 		TIM_IRQ(n)						TIM##n_IRQn								// �жϷ������ �磺TIM7_IRQn







//void TIM6_Int_Init(u16 arr,u16 psc);
//void TIM7_Int_Init(u16 arr,u16 psc);
#endif
