#ifndef _PWMINT_H
#define _PWMINT_H
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//��ʱ�� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/6/16
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
#include "sys.h"
#include "AddSubSpeed.h"

#define xAxis_PWM_Init(arr, psc) (TIM3_PWM_Init((arr),(psc)))
#define yAxis_PWM_Init(arr, psc) (TIM2_PWM_Init((arr),(psc)))
#define zAxis_PWM_Init(arr, psc) (TIM4_PWM_Init((arr),(psc)))
#define aAxis_PWM_Init(arr, psc) (TIM9_PWM_Init((arr),(psc)))
#define bAxis_PWM_Init(arr, psc) (TIM12_PWM_Init((arr),(psc)))

// #define DUTY_CYCLE											// ����ռ�ձȵĿ��أ�ע�͵����ر�ռ�ձȡ�����鿴PWM����

void TIM3_PWM_Init(u32 arr, u16 psc);		//X���ٶȳ�ʼ��
void TIM2_PWM_Init(u32 arr, u16 psc);		//Y���ٶȳ�ʼ��
void TIM4_PWM_Init(u32 arr, u16 psc);		//Z���ٶȳ�ʼ��
void TIM9_PWM_Init(u32 arr,u16 psc);		//A
void TIM12_PWM_Init(u32 arr,u32 psc);			//b��

// ����PWM���εĺ�����arrֵ�̶���Ƶ�ʺ�ռ�ձȿɵ�
void xAxisSetPWM(u16 psc, u16 dRatio1, u16 dRatio2);
void yAxisSetPWM(u16 psc, u16 dRatio1, u16 dRatio2);
void zAxisSetPWM(u16 psc, u16 dRatio1, u16 dRatio2);
void aAxisSetPWM(u16 psc, u16 dRatio1, u16 dRatio2);
void bAxisSetPWM(u16 psc, u16 dRatio1, u16 dRatio2);

// �ж�PSC�Ƿ��޸�
u8 ifX_PscModify(u16 psc);				// �޸��˷���1�����򷵻�0
u8 ifY_PscModify(u16 psc);				// �޸��˷���1�����򷵻�0
u8 ifZ_PscModify(u16 psc);				// �޸��˷���1�����򷵻�0
u8 ifA_PscModify(u16 psc);				// �޸��˷���1�����򷵻�0
u8 ifB_PscModify(u16 psc);				// �޸��˷���1�����򷵻�0

// PWM��ʼ��������ʱ��ʼ��
void PWM_Init(void);

void TIM3_PWM_Stop(void);
void TIM2_PWM_Stop(void);
void TIM4_PWM_Stop(void);
void TIM9_PWM_Stop(void);
void TIM12_PWM_Stop(void);

void mainMotorDecode(void);
void mainMotorWork(void);
#endif
