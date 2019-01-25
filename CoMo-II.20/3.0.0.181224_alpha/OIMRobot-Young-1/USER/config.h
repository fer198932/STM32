#ifndef __CONFIG_H
#define __CONFIG_H
/* ���ļ�������һЩ���õĿ���ϵͳ����ѡ��Լ�д�Ĵ��붼Ӧ�ð�����ͷ�ļ� */

#include "macro_operate.h"				// �����

/*  ϵͳʱ�� */
#define _SYSCLK	168								// ϵͳʱ��168MHz

/* �������� USART1 USART2 USART3  */
#define 	EN_USART1 	1						// ��ʹ�ܴ���2�� EN_USART2 	1
#define 	BOUND_X			115200

/* LED IO�� */
#define 	GPIO_LED		"PF9"

/* ��λ IO�� */
#define X_P_LIMIT 		"PE2"					// X+��λ
#define X_N_LIMIT 		"PE3"					// X-��λ ����ΪX��λ
#define Y_P_LIMIT 		"PE4"					// Y+��λ
#define Y_N_LIMIT 		"PE5"					// Y-��λ ����ΪY��λ
#define Z_P_LIMIT 		"PE6"					// Z+��λ
#define Z_N_LIMIT 		"PE7"					// Z-��λ ����ΪZ��λ
#define A_LIMIT 			"PE8"					// A��λ
#define B_LIMIT 			"PE9"					// B��λ




/*********����ͨ��Э��**********/
#define FrameB1 		0x0E     //����֡ͷ
#define FrameB2			0x09     //����֡ͷ
#define FrameEnd		0xFF     //����֡β

/*********����ͨ������֡������**********/
#define SELFCHECK 		0x0B     //�Լ�
#define CONTROLINFO		0x0C     //����
#define DATAINFO		0x0D     //����
#define STATUSINFO		0x0E     //״̬

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



#endif
