#ifndef _PWM_H
#define _PWM_H
#include "sys.h"
#include "timer.h"

//////////////////////////////////////////////////////////////////////////////////	 
//  PWM生成程序  2019年1月29日20:16:48 byYJY							  
////////////////////////////////////////////////////////////////////////////////// 	

/*  TIM 定义  */
#define 		X_PWM  			TIM3
#define 		X_CH_EXTI		2 					// 对应通道 PWM_X_EXTI
#define 		X_CH_OUT		1 					// 对应通道 PWM_X_OUT

#define 		Y_PWM  			TIM2
#define 		Y_CH_EXTI		4
#define 		Y_CH_OUT		1

#define 		Z_PWM  			TIM4
#define 		Z_CH_EXTI 	3
#define 		Z_CH_OUT		4

#define 		A_PWM  			TIM9
#define 		A_CH_EXTI		1
#define 		A_CH_OUT		2

#define 		B_PWM  			TIM12
#define 		B_CH_EXTI		1
#define 		B_CH_OUT		2

#define 		DUTY_CYCLE	0 		// 占空比的开关，置零即关闭占空比调整（默认为50%） 暂时没做 byYJY

#define 		GPIO_ERROR	255 	// 设置为该值时，不进行相关初始化


/* 为了提高运行效率，PWM的频率改变采用宏实现 dc:占空比 */
#if DUTY_CYCLE
	#define 	xAxisSetPWM(psc, channel_exti, dc_exti, channel_out, dc_out) 		\
	do \
	{ \
		X_PWM->PSC = (psc); 			/* psc值决定频率 */ \
		X_PWM->CCR##channel_exti = (dc_exti);  /* 占空比设定 */ \
		X_PWM->CCR##channel_out = (dc_out);   /* */ \
	} while(0)
#else
	#define 	nAxisSetPWM(TIM_N, psc) 		\
	do \
	{ \
		/* if(TIM_N->PSC != psc) */	\
			TIM_N->PSC = (psc); 			/* psc值决定频率 */ \
	} while(0)
#endif

// 各轴与TIM的映射关系结构体
typedef struct {
	u8 										nAxis;
	TIM_TypeDef*	 				TIM_N;
	u8 										ch_exti;								// 中断通道
	u8 										ch_out;									// 输出通道
	// 中断线
	GPIO_Structure_XX 		GPIO_PWM_EXTI; 					// 中断GPIO口的结构体
	GPIO_Structure_XX 		GPIO_PWM_OUT; 					// 输出GPIO口的结构体
} NAxis_TIM_Structure;
	
typedef enum {PWM_EXTI = 0, PWM_OUT = !PWM_EXTI} PWM_Channel; 		// PWM的通道
	
// PWM初始化函数
void PWM_Init(void);
	
// 获得指定轴与TIM之间的映射关系
void nAxis2TIM(u8 nAxis, NAxis_TIM_Structure* 	nAxis_TIM_Structure);

// 根据CLK设定PWM的输出频率
// void setAxisPWM(u8 nAxis, u32 clk, u16 dutyCycle1, u16 dutyCycle2);

// 定时器初始化 ch1、ch2为PWM输出通道(为0啥都不干)
void Tim_PWM_Init(const NAxis_TIM_Structure* 	nAxis_TIM_Structure, uint8_t NVIC_IRQPreemptionPriority, 
	uint8_t NVIC_IRQSubPriority, u16 arr, u16 psc, PWM_Channel PWM_ch);

// 定时器输出PIN脚初始化
static void GPIO_PWM_Init(const GPIO_Structure_XX *GPIO_Temp);

// 输出通道设置 DC：duty cycle 占空比
static void Set_PWM_Channel(TIM_TypeDef* TIM_N, TIM_OCInitTypeDef*  TIM_OCInitStructure, u8 ch1, u16 DC);

// PWM 关闭或使能
void PWM_Cmd(TIM_TypeDef* TIM_N, FunctionalState state, u8 ch);

// PWM 强制拉高或取消 ch:输出通道
static void PWM_Forced2High(TIM_TypeDef* TIM_N, FunctionalState state, u8 ch);

// 通过CLK频率计算预分频系数 PSC
u16 calPSC(u32 clk);
// 通过PSC计算频率， 注意寄存器溢出问题  单位HZ
u32 calClk_PSC(u16 psc);


#endif
