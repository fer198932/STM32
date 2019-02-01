#ifndef __STEPMOTOR_H
#define	__STEPMOTOR_H	   
#include "sys.h"
#include "config.h"
#include "delay.h"
#include "pwm.h"

//////////////////////////////////////////////////////////////////////////////////	 
// ����������Ƴ���
// 1��
// 2019��1��31��13:20:00 byYJY
////////////////////////////////////////////////////////////////////////////////// 	 

#define 	DIR_EX_DALAY 		100 			// ���������Ҫ�趨һ��ʱ����ʱ  ��λus

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

// �˶���ֹͣ �Ĵ�����ʽ�����Ч��
#define 	nAxis_StepMotor_Start(TIM_N) 			((TIM_N)->CR1 |= TIM_CR1_CEN)
#define 	nAxis_StepMotor_Stop(TIM_N) 			((TIM_N)->CR1 &= (uint16_t)~TIM_CR1_CEN)

extern 	GPIO_Structure_XX  	StepMotor_Dir[AXIS_NUM];

typedef enum {NEG_DIR = 0, POS_DIR = !NEG_DIR} Motor_Dir; 		// ����˶�����

// ���������ʼ�� ��Ҫ�Ƿ���IO�ڵĳ�ʼ��
void StepMotor_Init(void);

// ��������˶�IO�ڷ����ʼ��
static void Motor_Dir_Init(GPIO_Structure_XX *GPIO_Temp, const char str[]);

// ����˶������趨
void Motor_Dir_Set(GPIO_Structure_XX *GPIO_Temp, Motor_Dir dir);

// �������ͬʱ��ʼ�˶�����֤ͬ���� �Ĵ�����ʽ�����Ч��
void StepMotor_Start(void);


#endif





























