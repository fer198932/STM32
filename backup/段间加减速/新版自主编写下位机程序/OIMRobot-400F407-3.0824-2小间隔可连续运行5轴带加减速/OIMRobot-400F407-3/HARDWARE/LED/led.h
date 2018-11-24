#ifndef _LED_H
#define _LED_H
#include "sys.h"
/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          ԭ���������������
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : Led.h
* By  : Hiker Lee
* 2018-09-10
********************************************************************************************************/

//LED�˿ڶ���
#define LEDgreen 	PDout(6)
#define LEDred 		PDout(7)

#define LEDLAMP 	PCout(2)

#define LEDtest 	PCout(3)

#define LASER1 		PCout(6)
#define LASER2 		PCout(7)

#define BEEP 		PDout(13)

#define ELECTROMAGNET PBout(12)

void LED_Init(void); //��ʼ��
void Set_Led(u8 LED,u8 state);
#endif
