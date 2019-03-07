#ifndef __MAINAXIS_MOTOR_H
#define	__MAINAXIS_MOTOR_H	   
#include "sys.h"
#include "config.h"
#include "pwm.h"

//////////////////////////////////////////////////////////////////////////////////	 
// 主轴电机控制程序
// 2019年3月6日13:42:10 byYJY
////////////////////////////////////////////////////////////////////////////////// 	 

/* 主轴电机的运动参数  */
#define 	MAINMOTOR_DECODE_DC1			10													// 解码占空比1，单位%，如12%
#define 	MAINMOTOR_DECODE_DC2			5														// 解码占空比2，单位%，如12%
#define		MAINMOTOR_DECODE_DELAY1		2000												// 解码延迟时间，单位ms
#define		MAINMOTOR_DECODE_DELAY2		1000												// 解码延迟时间，单位ms

#define		MAINMOTOR_DC							12													// 主轴电机的占空比，单位%，如12%

#define		MAINMOTOR_ARR							200
#define		MAINMOTOR_CLK							50													// 主轴电机的运动频率，单位Hz

#define 	MAINMOTOR_PSC 					((TIM_CLK * MHz_2_Hz) / (MAINMOTOR_CLK * MAINMOTOR_ARR))

// 主轴电机解码方式 HM_Decode：手动， Auto_Decode：自动
typedef enum {HM_Decode = 0, Auto_Decode = !HM_Decode} Decode_Mode;

// 主轴解码
static void mainMotorDecode(void);

// 主轴初始化
void mainMotor_Init(void);

// 主轴工作
void mainMotorWork(Decode_Mode mode);


// 主轴停止
void mainMotorStop(void);

// 设定主轴电机的占空比，dc：占空比 单位%  
void setMainMotor_DC(u8 ch, u8 dc);




















#endif






























