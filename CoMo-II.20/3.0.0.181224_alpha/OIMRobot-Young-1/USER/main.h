#ifndef __MAIN_H
#define __MAIN_H

/* 控制一些开关的打开、关闭等 默认0：关闭、1：打开 */
#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "config.h"
#include "led.h"
#include "limit_IO.h"
#include "dma.h"	
#include "buffer.h"
#include "exti.h"
#include "pwm.h"
#include "timer.h"
#include "StepMotor.h"	

/* 系统功能开关： 1--打开测试代码，0--关闭 */
// 测试代码，测试代码通常属于验证编写的代码，不算入系统代码
#define _TEST 0							

// 开发板上的测试代码，方便在探索者板子上验证代码
// #define _TEST_ON_ALIENTEK 1  -->在"config.h"

// 测试用
#define PRIN2DISP 1   			// 打印提示信息，而不是发回控制命令


// 脱机加工开关
#define OFFLINE_WORK 	1

// 加减速开关
#define NO_ADDSUBSPEED 0


// 系统初始化函数
void Sys_Init(void);

// 系统各外设使能函数，如LED使能、中断使能等
void Sys_Enable(void);

#endif /* __MAIN_H */
