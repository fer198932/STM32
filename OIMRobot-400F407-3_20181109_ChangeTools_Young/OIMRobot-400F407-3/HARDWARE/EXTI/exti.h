#ifndef __EXTI_H
#define __EXIT_H	 
#include "sys.h"  	
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//�ⲿ�ж� ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/4
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	 

#include "ChangeTools.h"		// �����˻��������Ľṹ��

void EXTIX_Init(void);	//�ⲿ�жϳ�ʼ��		 					   
void TIM14_PWM_Init(u32 arr,u32 psc);

// ���ñ�־λ����¼�߹�����������
extern u8 stepsFlag[5]; 				// ��X��Y��Z��A��B�ļ��� 1���򿪣� 0���ر�

// ��¼�߹���������
extern ChangeToolsSavedSteps stepsTemp;


#endif

























