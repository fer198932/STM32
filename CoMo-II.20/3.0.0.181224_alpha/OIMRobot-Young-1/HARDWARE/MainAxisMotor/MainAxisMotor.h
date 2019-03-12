#ifndef __MAINAXIS_MOTOR_H
#define	__MAINAXIS_MOTOR_H	   
#include "sys.h"
#include "config.h"
#include "pwm.h"

//////////////////////////////////////////////////////////////////////////////////	 
// ���������Ƴ���
// 2019��3��6��13:42:10 byYJY
////////////////////////////////////////////////////////////////////////////////// 	 

/* ���������˶�����  */
#define 	MAINMOTOR_DECODE_DC1			10													// ����ռ�ձ�1����λ%����12%
#define 	MAINMOTOR_DECODE_DC2			5														// ����ռ�ձ�2����λ%����12%
#define		MAINMOTOR_DECODE_DELAY1		2000												// �����ӳ�ʱ�䣬��λms
#define		MAINMOTOR_DECODE_DELAY2		1000												// �����ӳ�ʱ�䣬��λms

#define		MAINMOTOR_DC							12													// ��������ռ�ձȣ���λ%����12%

#define		MAINMOTOR_ARR							200
#define		MAINMOTOR_CLK							50													// ���������˶�Ƶ�ʣ���λHz

#define 	MAINMOTOR_PSC 					((TIM_CLK * MHz_2_Hz) / (MAINMOTOR_CLK * MAINMOTOR_ARR))

// ���������뷽ʽ HM_Decode���ֶ��� Auto_Decode���Զ�
typedef enum {HM_Decode = 0, Auto_Decode = !HM_Decode} Decode_Mode;

// �������
static void mainMotorDecode(void);

// �����ʼ��
void mainMotor_Init(void);

// ���Ṥ��
void mainMotorWork(Decode_Mode mode);


// ����ֹͣ
void mainMotorStop(void);

// �趨��������ռ�ձȣ�dc��ռ�ձ� ��λ%  
void setMainMotor_DC(u8 ch, u8 dc);

// ��������ת-װ����
void mainMotor_WorkSlowly(void);


















#endif






























