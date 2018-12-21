#ifndef _PWMINT_H
#define _PWMINT_H
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//定时器 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/6/16
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
#include "sys.h"
#include "AddSubSpeed.h"

#define xAxis_PWM_Init(arr, psc) (TIM3_PWM_Init((arr),(psc)))
#define yAxis_PWM_Init(arr, psc) (TIM2_PWM_Init((arr),(psc)))
#define zAxis_PWM_Init(arr, psc) (TIM4_PWM_Init((arr),(psc)))
#define aAxis_PWM_Init(arr, psc) (TIM9_PWM_Init((arr),(psc)))
#define bAxis_PWM_Init(arr, psc) (TIM12_PWM_Init((arr),(psc)))

// #define DUTY_CYCLE											// 调整占空比的开关，注释掉即关闭占空比、方便查看PWM波形

void TIM3_PWM_Init(u32 arr, u16 psc);		//X轴速度初始化
void TIM2_PWM_Init(u32 arr, u16 psc);		//Y轴速度初始化
void TIM4_PWM_Init(u32 arr, u16 psc);		//Z轴速度初始化
void TIM9_PWM_Init(u32 arr,u16 psc);		//A
void TIM12_PWM_Init(u32 arr,u32 psc);			//b轴

// 调整PWM波形的函数，arr值固定，频率和占空比可调
void xAxisSetPWM(u16 psc, u16 dRatio1, u16 dRatio2);
void yAxisSetPWM(u16 psc, u16 dRatio1, u16 dRatio2);
void zAxisSetPWM(u16 psc, u16 dRatio1, u16 dRatio2);
void aAxisSetPWM(u16 psc, u16 dRatio1, u16 dRatio2);
void bAxisSetPWM(u16 psc, u16 dRatio1, u16 dRatio2);

// 判断PSC是否修改
u8 ifX_PscModify(u16 psc);				// 修改了返回1，否则返回0
u8 ifY_PscModify(u16 psc);				// 修改了返回1，否则返回0
u8 ifZ_PscModify(u16 psc);				// 修改了返回1，否则返回0
u8 ifA_PscModify(u16 psc);				// 修改了返回1，否则返回0
u8 ifB_PscModify(u16 psc);				// 修改了返回1，否则返回0

// PWM初始化，开机时初始化
void PWM_Init(void);

void TIM3_PWM_Stop(void);
void TIM2_PWM_Stop(void);
void TIM4_PWM_Stop(void);
void TIM9_PWM_Stop(void);
void TIM12_PWM_Stop(void);

void mainMotorDecode(void);
void mainMotorWork(void);
#endif
