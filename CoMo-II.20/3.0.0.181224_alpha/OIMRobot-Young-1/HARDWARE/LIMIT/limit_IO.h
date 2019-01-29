#ifndef _LIMIT_IO_H
#define _LIMIT_IO_H
#include "sys.h"
#include "config.h"
#include "delay.h"
#include "ComDataProc.h"
#include "main.h"
/********************************************************************************************************
*    ��λIO�ڵ���س���
********************************************************************************************************/

// IO�ڳ�ʼ������
void limit_IO_Init(void);
// ��ʼ������
static void limitInit(GPIO_Structure_XX *GPIO_Temp, const char str[]);

// ��ȡIO�������ƽ
static uint8_t readOutput(GPIO_Structure_XX *GPIO_Temp);

// 0��������λ��1��û�д���
uint8_t X_P_Lim(void);
uint8_t X_N_Lim(void);
uint8_t Y_P_Lim(void);
uint8_t Y_N_Lim(void);
uint8_t Z_P_Lim(void);
uint8_t Z_N_Lim(void);
uint8_t A_Lim(void);
uint8_t B_Lim(void);

// �鿴�Ƿ�����λ����
// mode:0,��֧��������;1,֧��������;
static void LIMIT_Scan(void);				// u8 mode

// ѭ���鿴�Ƿ�����λ��������������λ
void procLimit(void);

#endif 

