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

#define 	SELFCHECK_OK					0x01		// �Լ�����
#define 	SELFCHECK_URGENTSTOP 	0x03		// ��ͣ��ť��ֵ


/* ��λ�ظ���Ϣ */
#define X_P_MSG1  	0x31			// X+ ��λ
#define X_P_MSG2  	0x01			// X+ ��λ

#define X_N_MSG1  	0x31			// X- ��λ
#define X_N_MSG2  	0x02			// X- ��λ

#define Y_P_MSG1  	0x32			// Y+ ��λ
#define Y_P_MSG2  	0x01			// Y+ ��λ

#define Y_N_MSG1  	0x32			// Y- ��λ
#define Y_N_MSG2  	0x02			// Y- ��λ

#define Z_P_MSG1  	0x33			// Z+ ��λ
#define Z_P_MSG2  	0x01			// Z+ ��λ

#define Z_N_MSG1  	0x33			// Z- ��λ
#define Z_N_MSG2  	0x02			// Z- ��λ

#define A_MSG1  		0x35			// A ��λ
#define A_MSG2  		0x03			// A ��λ

#define B_MSG1  		0x36			// B ��λ
#define B_MSG2  		0x03			// B ��λ



// IO�ڳ�ʼ������
void limit_IO_Init(void);
// ��ʼ������
void limitInit(GPIO_Structure_XX *GPIO_Temp, const char str[]);

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

uint8_t UrgentStop_Lim(void);

uint8_t MainMotor_Start(void);


// �鿴�Ƿ�����λ����
// mode:0,��֧��������;1,֧��������;
static void LIMIT_Scan(void);				// u8 mode

// ѭ���鿴�Ƿ�����λ��������������λ
void procLimit(void);

// ��ͣ����
void UrgentStop_Proc(u8 status);

// �Լ�ļ�ͣ���
u8 UrgentStopTest(void);

//  �����������λ����ֹͣ�˶�����λ����ʱ�����������������Ƶ������
void StepMotor_Limited(vu32 limitValue);






// ����ͣ�ĺ��� ����ע�⣺�����ú꣩
#define UrgentStop_Proc_Macro			\
delay_ms(2);		\
urgentStopTemp1 = UrgentStop_Lim(); 			\
delay_ms(2);		\
urgentStopTemp2 = UrgentStop_Lim(); 			\
if(urgentStopTemp1 == urgentStopTemp2) 	\
{ 	\
	if(0 == urgentStopTemp1)	/* ��ͣ��ť����  */ \
	{	\
		flag_Struct.UrgentStop_Flag = UrgentStop_Down;	\
	}	\
	else	/* ��ͣ��ť�ɿ�  */ \
	{	\
		flag_Struct.UrgentStop_Flag = UrgentStop_Up;	\
	}		\
	UrgentStop_Proc((u8)flag_Struct.UrgentStop_Flag);	\
	continue;		\
} else \
	flag_Struct.UrgentStop_Flag = UrgentStop_Init;	\












#endif 










