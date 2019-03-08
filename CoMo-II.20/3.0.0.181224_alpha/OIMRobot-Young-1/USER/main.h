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
#include "MainAxisMotor.h"	

/* 系统功能开关： 1--打开测试代码，0--关闭 */
// 测试代码，测试代码通常属于验证编写的代码，不算入系统代码
#define _TEST 0							

// 开发板上的测试代码，方便在探索者板子上验证代码
// #define _TEST_ON_ALIENTEK 1  -->在"config.h"




// 背隙补偿开关
#define		BACKLASH_COMPENSATION 		0


// 脱机加工开关
#define OFFLINE_WORK 	0

// 加减速开关
#define NO_ADDSUBSPEED 0




typedef enum {
UrgentStop_Init = 0, 
UrgentStop_Down = 1, 
UrgentStop_Up = 2, 
UrgentStop_Falling = 3, 
UrgentStop_Rising = 4,
UrgentStop_Locked = 5} UrgentStop_Status;

// 标记类结构体，注意Flag_Struct_Init里面初始化了默认值（大部分为0），不要轻易初始化为0！
typedef struct {
	volatile 		FlagStatus 					DMA_Out_Flag;								// DMA溢出，重新初始化
	
	volatile 		FlagStatus 					USART_IDLE_Flag; 						// 串口空闲中断标志，串口数据处理	
	volatile 		FlagStatus 					USART_Data_Proc_Flag;				// 串口数据处理完毕
	
	volatile 		FlagStatus					Cmd_Executing_Flag;					// 置位后表示命令正在执行
	volatile 		FlagStatus 					Cmd_ProcDone_Flag;					// 命令处理完毕的标记
	
	volatile 		FunctionalState 		Cmd_Execute_En;							// 可以执行命令
//	volatile	FunctionalState 	Cmd_Proc_Falg; 								// 串口数据可以进行处理
	
	volatile		UrgentStop_Status		UrgentStop_Flag;						// 急停按钮的状态	
	
	volatile 		FlagStatus					MainMotor_Start_Flag;				// 主轴电机启动标志
	volatile 		FlagStatus					MainMotor_Stop_Flag;				// 主轴电机停止标志
	
	vu32														Limiti_Flag;	
	
//	volatile		FlagStatus					Retun2Zero_Flag;						// 回零标记
} Flag_Structure;



// 系统初始化函数
void Sys_Init(void);

// 系统各外设使能函数，如LED使能、中断使能等
void Sys_Enable(void);

// 标记类结构体初始化
void Flag_Struct_Init(void);

#endif /* __MAIN_H */
