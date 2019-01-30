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
#define 	GPIO_LED				"PF9"

/* ��λ IO�� */
#define 	X_P_LIMIT 			"PE2"					// X+��λ
#define 	X_N_LIMIT 			"PE3"					// X-��λ ����ΪX��λ
#define 	Y_P_LIMIT 			"PF2"					// Y+��λ
#define 	Y_N_LIMIT 			"PF2"					// Y-��λ ����ΪY��λ
#define 	Z_P_LIMIT 			"PF2"					// Z+��λ
#define 	Z_N_LIMIT 			"PF2"					// Z-��λ ����ΪZ��λ
#define 	A_LIMIT 				"PF2"					// A��λ
#define 	B_LIMIT 				"PF2"					// B��λ

/******* �ж�IO��  ע�⣺��PA11ֻ�����ӵ��ж���11 ***********/
/* �����PWM��  �жϼ����IO��  */
#define 	EXTI_X_PLUS			"PA11" 				// ����PWM���ж�IO��
#define 	PWM_X_EXTI			"PA11"				// ���PWM���жϵ�IO��
#define 	PWM_X_OUT				"PA11"  			// ���PWM�������IO��

#define 	EXTI_Y_PLUS			"PA11"
#define 	PWM_Y_EXTI			"PA11"				// ���PWM���жϵ�IO��
#define 	PWM_Y_OUT				"PA11"  			// ���PWM�������IO��

#define 	EXTI_Z_PLUS			"PA11"
#define 	PWM_Z_EXTI			"PA11"				// ���PWM���жϵ�IO��
#define 	PWM_Z_OUT				"PA11"  			// ���PWM�������IO��

#define 	EXTI_A_PLUS			"PA11"
#define 	PWM_A_EXTI			"PA11"				// ���PWM���жϵ�IO��
#define 	PWM_A_OUT				"PA11"  			// ���PWM�������IO��

#define 	EXTI_B_PLUS			"PA11"
#define 	PWM_B_EXTI			"PA11"				// ���PWM���жϵ�IO��
#define 	PWM_B_OUT				"PA11"  			// ���PWM�������IO��



// ���԰�ť�ж� ����ɾ��
#define 	_KEY1 						"PE4" 		// ���º�����
//#define 	_KEY1 						"PE3" 		// ���º�����



/*********����ͨ��Э��**********/
#define FrameB1 		0x0E     //����֡ͷ
#define FrameB2			0x09     //����֡ͷ
#define FrameEnd		0xFF     //����֡β

/*********����ͨ������֡������**********/
#define SELFCHECK 		0x0B     	// �Լ�
#define CONTROLINFO		0x0C     	// ����
#define DATAINFO			0x0D     	// ����
#define STATUSINFO		0x0E     	// ״̬
#define INVALID_CMD 	0x00 			// ��Ч���Ĭ��״̬��û��������

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

/* �˶�ϵͳ���˶�������  */
#define AXIS_NUM 		5     	// 5��ϵͳ ˳��X��Y��Z��A��B

#endif
