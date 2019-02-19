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
#define 	Y_P_LIMIT 			"PA11"					// Y+��λ
#define 	Y_N_LIMIT 			"PA11"					// Y-��λ ����ΪY��λ
#define 	Z_P_LIMIT 			"PA11"					// Z+��λ
#define 	Z_N_LIMIT 			"PA11"					// Z-��λ ����ΪZ��λ
#define 	A_LIMIT 				"PA11"					// A��λ
#define 	B_LIMIT 				"PA11"					// B��λ

/******* �ж�IO��  ע�⣺��PA11ֻ�����ӵ��ж���11 ***********/
/* ������������IO��  �����жϼ����IO�� ע��PWM��IO�ڲ������ѡ�� */
#define 	EXTI_X_PLUS			"PC4" 		/* �ж���4 */				// ����PWM���ж�IO��
#define 	PWM_X_EXTI			"PA7"			/* TIM3_CH2 */			// ���PWM���жϵ�IO��   	
//#define 	PWM_X_OUT				"PA6"  		/* TIM3_CH1 */			// ���PWM�������IO��		
#define 	PWM_X_OUT				"PB4"  		/* TIM3_CH1 */			// ���PWM�������IO��		 ̽���߰�������
#define 	DIR_X						"PA4" 		/*          */ 			// ����˶�����

#define 	EXTI_Y_PLUS			"PA2"			/* �ж���2  */		// ����PWM���ж�IO��  ̽���߰����ϲ������ж���1����ʼ��ʱ�ᱻ���
#define 	PWM_Y_EXTI			"PA3"			/* TIM2_CH4 */			// ���PWM���жϵ�IO��   	
#define 	PWM_Y_OUT				"PA5"  		/* TIM2_CH1 */			// ���PWM�������IO��
#define 	DIR_Y						"PC3" 		/*          */ 			// ����˶�����

#define 	EXTI_Z_PLUS			"PB6"			/* �ж���6 */				// ����PWM���ж�IO��
#define 	PWM_Z_EXTI			"PB8"			/* TIM4_CH3 */			// ���PWM���жϵ�IO��
#define 	PWM_Z_OUT				"PB9"  		/* TIM4_CH4 */			// ���PWM�������IO��
#define 	DIR_Z						"PB7" 		/*          */ 			// ����˶�����

#define 	EXTI_A_PLUS			"XXX"			/* �ж���11 */			// ����PWM���ж�IO��
#define 	PWM_A_EXTI			"PE6"			/* TIM3_CH1 */			// ���PWM���жϵ�IO��
#define 	PWM_A_OUT				"XXX"  		/* TIM3_CH1 */			// ���PWM�������IO��
#define 	DIR_A						"XXX" 		/*          */ 			// ����˶�����

#define 	EXTI_B_PLUS			"XXX"			/* �ж���11 */			// ����PWM���ж�IO��
#define 	PWM_B_EXTI			"PB14"		/* TIM3_CH1 */			// ���PWM���жϵ�IO��
#define 	PWM_B_OUT				"XXX"  		/* TIM3_CH1 */			// ���PWM�������IO��
#define 	DIR_B						"XXX" 		/*          */ 			// ����˶�����



// ���԰�ť�ж� ����ɾ��
#define 	_KEY1 						"XXX" 		// ���º�����
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

/* 	0x0D // ���� : ��Ӧָ����	 */
#define PLUS_DATA				0x22			// �������ݴ���
#define OFFLINE_DATA		0x25			// �ѻ��ӹ�ָ����

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

// �ߴ������������⣬��ʱδ������ȷ�����
typedef enum {TBD_DIR = 0, POS_DIR = 1, NEG_DIR = 2} Motor_Dir; 		// ����˶�����

#endif
