#ifndef _TIMER_H
#define _TIMER_H
#include "sys.h"
#include "config.h"
#include "ComDataProc.h"
//////////////////////////////////////////////////////////////////////////////////	 
// 定时器 2019年1月29日20:43:08 byYJY
////////////////////////////////////////////////////////////////////////////////// 	

/****   公式： TIM_OUT = TIM_ARR * TIM_PSC / TIM_CLK   *******/
#define 	TIM_OUT						20																	// 输出时间，  单位us	 取值范围：3~21854
#define  	TIM_CLK 					(_SYSCLK/2) 												// 定时器时钟，单位MHz (84MHz)
#define 	TIM_ARR 					28 																	// 重装载值
#define 	TIM_PSC 					(TIM_CLK * TIM_OUT / TIM_ARR) 			// 预分频系数



#define 		ADDSUB_TIMER 			TIM7 						// 加减速使用的定时器
#define		 	TIM_IRQ_ADDSUB 		/* 加减速使用的中断服务程序 */   \
do 		\
{ 			\
	temp++;					\
	if(1e6 < temp)  \
	{ \
		respUsartMsg("TIM7\r\n", 6); 		\
		temp = 0; 			\
	} \
} while(0)





// 定时器初始化
void Timer_Init(void);

// 定时器初始化函数 注：arr可以为u32
void Tim_Base_Init(TIM_TypeDef* TIM_N, u16 arr, u16 psc, FunctionalState Enable_IRQ,
	uint8_t IRQPreempPriority, uint8_t IRQSubPriority);


/* 定时器的宏操作 */
//#define 		RCC_APB1_TIMER(n)  		RCC_APB1Periph_TIM##n			// 时钟线 TIM2、3、4、5、6、7、12、13、14
//#define 		RCC_APB2_TIMER(n)  		RCC_APB2Periph_TIM##n			// 时钟线 TIM1、8、9、10、11
//#define			TIM_TYPE(n)						TIM##n										// 定时器，如：TIM7
//#define 		TIM_IRQ(n)						TIM##n_IRQn								// 中断服务程序 如：TIM7_IRQn







//void TIM6_Int_Init(u16 arr,u16 psc);
//void TIM7_Int_Init(u16 arr,u16 psc);
#endif
