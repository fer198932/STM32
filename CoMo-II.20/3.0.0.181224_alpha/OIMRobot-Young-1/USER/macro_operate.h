#ifndef __MACRO_OPERATE_H
#define __MACRO_OPERATE_H
#include "sys.h"
#include "ComDataProc.h"
/* ��������� */



typedef struct {
	uint16_t GPIO_Pin_X;							// pin��
	GPIO_TypeDef *GPIO_Base;					// �˿ں�
	uint32_t RCC_Periph_X;						// ʱ����
//	uint32_t GPIOx_ODR_Addr;					// �����ַӳ��
} GPIO_XX;

// ���GPIO��pin��
// #define GPIO_Pin_X(GPIO_Pin) 		GPIO_Pin_9

// �����ַ��������ʼ����GPIO�ṹ��
GPIO_XX GPIO_XX_Init(const char str[]);


#endif
