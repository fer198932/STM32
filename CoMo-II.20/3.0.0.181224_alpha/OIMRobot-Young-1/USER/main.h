#ifndef __MAIN_H
#define __MAIN_H

/* ����һЩ���صĴ򿪡��رյ� Ĭ��0���رա�1���� */
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

/* ϵͳ���ܿ��أ� 1--�򿪲��Դ��룬0--�ر� */
// ���Դ��룬���Դ���ͨ��������֤��д�Ĵ��룬������ϵͳ����
#define _TEST 0							

// �������ϵĲ��Դ��룬������̽���߰�������֤����
// #define _TEST_ON_ALIENTEK 1  -->��"config.h"




// ��϶��������
#define		BACKLASH_COMPENSATION 		0


// �ѻ��ӹ�����
#define OFFLINE_WORK 	0

// �Ӽ��ٿ���
#define NO_ADDSUBSPEED 0




typedef enum {
UrgentStop_Init = 0, 
UrgentStop_Down = 1, 
UrgentStop_Up = 2, 
UrgentStop_Falling = 3, 
UrgentStop_Rising = 4,
UrgentStop_Locked = 5} UrgentStop_Status;

// �����ṹ�壬ע��Flag_Struct_Init�����ʼ����Ĭ��ֵ���󲿷�Ϊ0������Ҫ���׳�ʼ��Ϊ0��
typedef struct {
	volatile 		FlagStatus 					DMA_Out_Flag;								// DMA��������³�ʼ��
	
	volatile 		FlagStatus 					USART_IDLE_Flag; 						// ���ڿ����жϱ�־���������ݴ���	
	volatile 		FlagStatus 					USART_Data_Proc_Flag;				// �������ݴ������
	
	volatile 		FlagStatus					Cmd_Executing_Flag;					// ��λ���ʾ��������ִ��
	volatile 		FlagStatus 					Cmd_ProcDone_Flag;					// �������ϵı��
	
	volatile 		FunctionalState 		Cmd_Execute_En;							// ����ִ������
//	volatile	FunctionalState 	Cmd_Proc_Falg; 								// �������ݿ��Խ��д���
	
	volatile		UrgentStop_Status		UrgentStop_Flag;						// ��ͣ��ť��״̬	
	
	volatile 		FlagStatus					MainMotor_Start_Flag;				// ������������־
	volatile 		FlagStatus					MainMotor_Stop_Flag;				// ������ֹͣ��־
	
	vu32														Limiti_Flag;	
} Flag_Structure;



// ϵͳ��ʼ������
void Sys_Init(void);

// ϵͳ������ʹ�ܺ�������LEDʹ�ܡ��ж�ʹ�ܵ�
void Sys_Enable(void);

// �����ṹ���ʼ��
void Flag_Struct_Init(void);

#endif /* __MAIN_H */
