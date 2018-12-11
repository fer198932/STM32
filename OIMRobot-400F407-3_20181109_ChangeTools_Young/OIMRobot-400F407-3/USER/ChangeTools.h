#ifndef __CHANGETOOLS_H
#define __CHANGETOOLS_H 

#include "delay.h"
#include "sys.h"
#include "SysInit.h"

#define TEST_STEPSNUM 0			// 1: ���Բ����� 0: ȡ����������

void delay_s(u8 delay);

// �������߲����Ľṹ��
typedef struct
{
	u32 xSteps[5];			// [0]:�ӹ���������߹��ľ��룻 [1-4]:����㵽�����߹��ľ��룬�豣�沢�ϴ�����λ��
	u32 ySteps[5];
	u32 zSteps[5];
} ChangeToolsSavedSteps;

#endif
