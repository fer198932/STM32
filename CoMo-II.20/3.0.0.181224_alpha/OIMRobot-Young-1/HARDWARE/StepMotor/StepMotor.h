#ifndef __STEPMOTOR_H
#define	__STEPMOTOR_H	   
#include "sys.h"
#include "config.h"
#include "delay.h"
#include "pwm.h"

//////////////////////////////////////////////////////////////////////////////////	 
// 步进电机控制程序
// 1、
// 2019年1月31日13:20:00 byYJY
////////////////////////////////////////////////////////////////////////////////// 	 

#define 	SUBDIV_NUM						400					// 细分数：如400脉冲=1mm
//#define 	Backlash_PlusNum			BacklashCompensation * SUBDIV_NUM / (1e3)		// 间隙对应的脉冲数

#define		StepMotor_MinClk			500
#define		StepMotor_MaxClk			10000


#define 	DIR_EX_DALAY 					10000 			// 换方向后需要设定一定时间延时  单位us
#define 	MOTION_START_DALAY 		10000 			// 运动停止后需要设定一定时间延时  单位us

#define 	XAxisDir_Pos 		(Motor_Dir_Set(StepMotor_Dir+0, POS_DIR)) 
#define 	XAxisDir_Neg 		(Motor_Dir_Set(StepMotor_Dir+0, NEG_DIR)) 

#define 	YAxisDir_Pos 		(Motor_Dir_Set(StepMotor_Dir+1, POS_DIR)) 
#define 	YAxisDir_Neg 		(Motor_Dir_Set(StepMotor_Dir+1, NEG_DIR)) 

#define 	ZAxisDir_Pos 		(Motor_Dir_Set(StepMotor_Dir+2, POS_DIR)) 
#define 	ZAxisDir_Neg 		(Motor_Dir_Set(StepMotor_Dir+2, NEG_DIR)) 

#define 	AAxisDir_Pos 		(Motor_Dir_Set(StepMotor_Dir+3, POS_DIR)) 
#define 	AAxisDir_Neg 		(Motor_Dir_Set(StepMotor_Dir+3, NEG_DIR)) 

#define 	BAxisDir_Pos 		(Motor_Dir_Set(StepMotor_Dir+4, POS_DIR)) 
#define 	BAxisDir_Neg 		(Motor_Dir_Set(StepMotor_Dir+4, NEG_DIR)) 

// 运动或停止 寄存器方式，提高效率
// #define 	nAxis_StepMotor_Start(TIM_N) 			((TIM_N)->CR1 |= TIM_CR1_CEN)

#define 	nAxis_Motion_Flag 	(nAxisStatus[0] | nAxisStatus[1] | nAxisStatus[2] | nAxisStatus[3] | nAxisStatus[4])


// 换向，间隙补偿的结构体
typedef struct {
	volatile		Motor_Dir 					motorDirOld[AXIS_NUM];					// 前一次的运动方向
	volatile		FlagStatus					DirChange_Flag[AXIS_NUM];				// 换向的标志
	volatile		FunctionalState			DirChange_En;										// 初始状态关闭	
} BacklashCompensation_Structure;


#define	nAxis_StepMotor_Start(n) 	\
do \
{	\
	PWM_Cmd(n##_PWM, ENABLE, n##_CH_EXTI); 	\
	PWM_Cmd(n##_PWM, ENABLE, n##_CH_OUT); 	\
} while(0)

// #define 	nAxis_StepMotor_Stop(TIM_N) 			((TIM_N)->CR1 &= (uint16_t)~TIM_CR1_CEN)
#define	nAxis_StepMotor_Stop(n) 	\
do \
{	\
	PWM_Cmd(n##_PWM, DISABLE, n##_CH_EXTI); 	\
	PWM_Cmd(n##_PWM, DISABLE, n##_CH_OUT); 	\
} while(0)

extern 	GPIO_Structure_XX  	StepMotor_Dir[AXIS_NUM];

// typedef enum {TBD_DIR = 0, POS_DIR = 1, NEG_DIR = 2} Motor_Dir; 		// 电机运动方向

// 步进电机初始化 主要是方向IO口的初始化
void StepMotor_Init(void);

// 步进电机运动IO口方向初始化
static void Motor_Dir_Init(GPIO_Structure_XX *GPIO_Temp, const char str[]);

// 电机运动方向设定
void Motor_Dir_Set(GPIO_Structure_XX *GPIO_Temp, Motor_Dir dir);

// 步进电机同时开始运动（保证同步） 寄存器方式，提高效率
void StepMotor_Start(void);

// 步进电机同时停止（同步） 没有强制拉高
void StepMotor_Stop_Macro(void);


// 步进电机运行设定距离
void StepMotor_Move(N_Axis n_axis, u32 Clk, u32 PlusNum, Motor_Dir Dir);
// 所有轴的步进电机运行设定距离
void StepMotor_All_Move(u32 Clk, u32 PlusNum, Motor_Dir Dir[]);

// 初始化时候的间隙补偿
void Move2CompensateBacklash(void);

// 运动停止前，会死循环在该函数里
void StepMotor_Move_Done(void);



#endif






























