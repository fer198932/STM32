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

/* ϵͳ���ܿ��أ� 1--�򿪲��Դ��룬0--�ر� */
// ���Դ��룬���Դ���ͨ��������֤��д�Ĵ��룬������ϵͳ����
#define _TEST 1							

// �������ϵĲ��Դ��룬������̽���߰�������֤����
#define _TEST_ON_ALIENTEK 1

// ������
#define PRIN2DISP 1   			// ��ӡ��ʾ��Ϣ�������Ƿ��ؿ�������


// ϵͳ��ʼ������
void Sys_Init(void);

// ϵͳ������ʹ�ܺ�������LEDʹ�ܡ��ж�ʹ�ܵ�
void Sys_Enable(void);

#endif /* __MAIN_H */
