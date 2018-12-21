#ifndef __EXTI_H
#define __EXTI_H	 
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
#include "AddSubSpeed.h"

#define xPWMCnt pwmNumTim3
#define yPWMCnt pwmNumTim2	
#define zPWMCnt pwmNumTim4	
#define aPWMCnt pwmNumTim9	
#define bPWMCnt pwmNumTim12	

#define xSetPWMCnt(cnt) (pwmNumTim3 = (cnt))
#define ySetPWMCnt(cnt) (pwmNumTim2 = (cnt))
#define zSetPWMCnt(cnt) (pwmNumTim4 = (cnt))
#define aSetPWMCnt(cnt) (pwmNumTim9 = (cnt))
#define bSetPWMCnt(cnt) (pwmNumTim12 = (cnt))

void EXTIX_Init(void);	//�ⲿ�жϳ�ʼ��		 					   

// ���ñ�־λ����¼�߹�����������
extern u8 stepsFlag[5]; 				// ��X��Y��Z��A��B�ļ��� 1���򿪣� 0���ر�

// ��¼�߹���������
extern ChangeToolsSavedSteps stepsTemp;


#endif

























