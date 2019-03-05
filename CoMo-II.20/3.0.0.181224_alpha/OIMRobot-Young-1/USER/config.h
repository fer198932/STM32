#ifndef __CONFIG_H
#define __CONFIG_H
/* ���ļ�������һЩ���õĿ���ϵͳ����ѡ��Լ�д�Ĵ��붼Ӧ�ð�����ͷ�ļ� */

#include "macro_operate.h"				// �����

/* ϵͳ���ܿ��أ� 1--�򿪲��Դ��룬0--�ر� */
// �������ϵĲ��Դ��룬������̽���߰�������֤����
#define _TEST_ON_ALIENTEK 1

/*  ϵͳʱ�� */
#define _SYSCLK	168								// ϵͳʱ��168MHz

/* �������� USART1 USART2 USART3  */
#define 	EN_USART1 	1						// ��ʹ�ܴ���2�� EN_USART2 	1
#define 	BOUND_X			115200


/* ��϶��������λum */
#define 	BacklashCompensation				40		

/* LED IO�� */
#define 	GPIO_LED				"PF9"

/* ��λ IO�� */
#define 	X_P_LIMIT 			"PE2"					// X+��λ
#define 	X_N_LIMIT 			"PE4"					// X-��λ ����ΪX��λ
#define 	Y_P_LIMIT 			"PA11"					// Y+��λ
#define 	Y_N_LIMIT 			"PA11"					// Y-��λ ����ΪY��λ
#define 	Z_P_LIMIT 			"PA11"					// Z+��λ
#define 	Z_N_LIMIT 			"PA11"					// Z-��λ ����ΪZ��λ
#define 	A_LIMIT 				"PA11"					// A��λ
#define 	B_LIMIT 				"PA11"					// B��λ

/******* �ж�IO��  ע�⣺��PA11ֻ�����ӵ��ж���11 ***********/
/* ������������IO��  �����жϼ����IO�� ע��PWM��IO�ڲ������ѡ�� */
#define		EXTI_LINE_X				4															// �ж��ߣ������ĵ��ж�IO�ڶ�Ӧ  ��PC4->�ж���4
#define 	EXTI_X_PLUS				"PC4" 		/* �ж���4 */				// ����PWM���ж�IO��
#define 	PWM_X_EXTI				"PA7"			/* TIM3_CH2 */			// ���PWM���жϵ�IO��   	
//#define 	PWM_X_OUT					"PA6"  		/* TIM3_CH1 */			// ���PWM�������IO��		
#define 	PWM_X_OUT					"PB4"  		/* TIM3_CH1 */			// ���PWM�������IO��		 ̽���߰�������
#define 	DIR_X							"PA4" 		/*          */ 			// ����˶�����

#if _TEST_ON_ALIENTEK
	#define		EXTI_LINE_Y				2															// �ж��ߣ������ĵ��ж�IO�ڶ�Ӧ  ��PC4->�ж���4
	#define 	EXTI_Y_PLUS				"PA2"			/* �ж���2  */		// ����PWM���ж�IO��  ̽���߰����ϲ������ж���1����ʼ��ʱ�ᱻ���
#else
	#define		EXTI_LINE_Y				1															// �ж��ߣ������ĵ��ж�IOkou��Ӧ  ��PC4->�ж���4
	#define 	EXTI_Y_PLUS				"PA1"			/* �ж���1  */		// ����PWM���ж�IO��  
#endif
#define 	PWM_Y_EXTI				"PA3"			/* TIM2_CH4 */			// ���PWM���жϵ�IO��   	
#define 	PWM_Y_OUT					"PA5"  		/* TIM2_CH1 */			// ���PWM�������IO��
#define 	DIR_Y							"PC3" 		/*          */ 			// ����˶�����

#define		EXTI_LINE_Z				6															// �ж��ߣ������ĵ��ж�IO�ڶ�Ӧ  ��PC4->�ж���4
#define 	EXTI_Z_PLUS				"PB6"			/* �ж���6 */				// ����PWM���ж�IO��
#define 	PWM_Z_EXTI				"PB8"			/* TIM4_CH3 */			// ���PWM���жϵ�IO��
#define 	PWM_Z_OUT					"PB9"  		/* TIM4_CH4 */			// ���PWM�������IO��
#define 	DIR_Z							"PB7" 		/*          */ 			// ����˶�����

#define		EXTI_LINE_A				14															// �ж��ߣ������ĵ��ж�IO�ڶ�Ӧ  ��PC4->�ж���4
#define 	EXTI_A_PLUS				"XXX"			/* �ж���11 */			// ����PWM���ж�IO��
#define 	PWM_A_EXTI				"PE6"			/* TIM3_CH1 */			// ���PWM���жϵ�IO��
#define 	PWM_A_OUT					"XXX"  		/* TIM3_CH1 */			// ���PWM�������IO��
#define 	DIR_A							"XXX" 		/*          */ 			// ����˶�����

#define		EXTI_LINE_B				15															// �ж��ߣ������ĵ��ж�IO�ڶ�Ӧ  ��PC4->�ж���4
#define 	EXTI_B_PLUS				"XXX"			/* �ж���11 */			// ����PWM���ж�IO��
#define 	PWM_B_EXTI				"PB14"		/* TIM3_CH1 */			// ���PWM���жϵ�IO��
#define 	PWM_B_OUT					"XXX"  		/* TIM3_CH1 */			// ���PWM�������IO��
#define 	DIR_B							"XXX" 		/*          */ 			// ����˶�����

#define 	EXTI_LINE_UrgentStop		3					/* �ж���0  */		// �ж��ߣ���ͣ��ť���ж���
#define		EXTI_UrgentStop_IO			"PF3"												// ��ͣ���ж�IO�ڣ���Ӧ̽���߰����ϵ�KEY_UP						



// ���԰�ť�ж� ����ɾ��
#define 	_KEY1 						"XXX" 		// ���º�����
//#define 	_KEY1 						"PE3" 		// ���º�����

/******* �ж�IO��  ע�⣺��PA11ֻ�����ӵ��ж���11 ***********/



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

/*  0x0E // ״̬ : ��Ӧָ����	 */
#define 	HARDWARE_URGENTSTOP				0x3B			// Ӳ����ͣ
#define 	HARDWARE_URGENTCANCEL			0x41			// Ӳ����ͣ




/* �˶�ϵͳ���˶�������  */
#define AXIS_NUM 		5     	// 5��ϵͳ ˳��X��Y��Z��A��B

// �ߴ������������⣬��ʱδ������ȷ�����
typedef enum {TBD_DIR = 0, POS_DIR = 1, NEG_DIR = 2} Motor_Dir; 		// ����˶�����


#endif
