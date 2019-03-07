#ifndef __CONFIG_H
#define __CONFIG_H
/* ���ļ�������һЩ���õĿ���ϵͳ����ѡ��Լ�д�Ĵ��붼Ӧ�ð�����ͷ�ļ� */

#include "macro_operate.h"				// �����


/********************************************************���ܿ���*************************************************************************/
/* ϵͳ���ܿ��أ� 1--�򿪲��Դ��룬0--�ر� */
// ������
#define 	PRIN2DISP 	0   			// ��ӡ��ʾ��Ϣ�������Ƿ��ؿ�������
// �������ϵĲ��Դ��룬������̽���߰�������֤����
#define 	_TEST_ON_ALIENTEK 	1


#define 	BOARD			1     	// 0���ް��ӣ�1��̽���߿����壬2���ɵ����ذ壬3���µ����ذ�
/*****************************************************************************************************************************************/



/*********************************************************ϵͳ����************************************************************************/
/*  ϵͳʱ�� */
#define _SYSCLK	168								// ϵͳʱ��168MHz



/* ��϶��������λum */
#define 	BacklashCompensation				40		
/*****************************************************************************************************************************************/


/*********************************************************IO������************************************************************************/
////////////////////////////////////////////////////////̽���߿�����//////////////////////////////////////////////////////////////
#if (1 == BOARD)			// ̽���߿�����


/* �������� USART1 USART2 USART3  */
#define 	EN_USART1 	1						// ��ʹ�ܴ���2�� EN_USART2 	1
#define 	BOUND_X			115200

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

/* ������IO�� */
#define		EXTI_LINE_MAINMOTOR			4															// �ж���
#define		EXTI_MAINMOTOR_START		"PE4"													// ����������IO�ڣ����ֶ�����
#define 	PWM_MAINMOTOR_OUT				"PA7"			/* TIM14_CH1 */
#define 	PWM_MAINMOTOR_DIR				"XXX"

/******* �ж�IO��  ע�⣺��PA11ֻ�����ӵ��ж���11 ***********/
/* ������������IO��  �����жϼ����IO�� ע��PWM��IO�ڲ������ѡ�� */
#define		EXTI_LINE_X							3															// �ж��ߣ������ĵ��ж�IO�ڶ�Ӧ  ��PC4->�ж���4
#define 	EXTI_X_PLUS							"PB3" 		/* �ж���3 */				// ����PWM���ж�IO��
#define 	PWM_X_EXTI							"PB5"			/* TIM3_CH2 */			// ���PWM���жϵ�IO��   	
//#define 	EXTI_X_PLUS						"PC4" 		/* �ж���4 */				// ����PWM���ж�IO��
//#define 	PWM_X_EXTI						"PA7"			/* TIM3_CH2 */			// ���PWM���жϵ�IO��   	
//#define 	PWM_X_OUT							"PA6"  		/* TIM3_CH1 */			// ���PWM�������IO��		
#define 	PWM_X_OUT								"PB4"  		/* TIM3_CH1 */			// ���PWM�������IO��		 ̽���߰�������
#define 	DIR_X										"PA4" 		/*          */ 			// ����˶�����

#if _TEST_ON_ALIENTEK
	#define		EXTI_LINE_Y						2															// �ж��ߣ������ĵ��ж�IO�ڶ�Ӧ  ��PC4->�ж���4
	#define 	EXTI_Y_PLUS						"PA2"			/* �ж���2  */			// ����PWM���ж�IO��  ̽���߰����ϲ������ж���1����ʼ��ʱ�ᱻ���
#else
	#define		EXTI_LINE_Y						1															// �ж��ߣ������ĵ��ж�IOkou��Ӧ  ��PC4->�ж���4
	#define 	EXTI_Y_PLUS						"PA1"			/* �ж���1  */			// ����PWM���ж�IO��  
#endif
#define 	PWM_Y_EXTI							"PA3"			/* TIM2_CH4 */			// ���PWM���жϵ�IO��   	
#define 	PWM_Y_OUT								"PA5"  		/* TIM2_CH1 */			// ���PWM�������IO��
#define 	DIR_Y										"PC3" 		/*          */ 			// ����˶�����

#define		EXTI_LINE_Z							6															// �ж��ߣ������ĵ��ж�IO�ڶ�Ӧ  ��PC4->�ж���4
#define 	EXTI_Z_PLUS							"PB6"			/* �ж���6 */				// ����PWM���ж�IO��
#define 	PWM_Z_EXTI							"PB8"			/* TIM4_CH3 */			// ���PWM���жϵ�IO��
#define 	PWM_Z_OUT								"PB9"  		/* TIM4_CH4 */			// ���PWM�������IO��
#define 	DIR_Z										"PB7" 		/*          */ 			// ����˶�����

#define		EXTI_LINE_A							14															// �ж��ߣ������ĵ��ж�IO�ڶ�Ӧ  ��PC4->�ж���4
#define 	EXTI_A_PLUS							"XXX"			/* �ж���11 */			// ����PWM���ж�IO��
#define 	PWM_A_EXTI							"PE6"			/* TIM3_CH1 */			// ���PWM���жϵ�IO��
#define 	PWM_A_OUT								"XXX"  		/* TIM3_CH1 */			// ���PWM�������IO��
#define 	DIR_A										"XXX" 		/*          */ 			// ����˶�����

#define		EXTI_LINE_B							15															// �ж��ߣ������ĵ��ж�IO�ڶ�Ӧ  ��PC4->�ж���4
#define 	EXTI_B_PLUS							"XXX"			/* �ж���11 */			// ����PWM���ж�IO��
#define 	PWM_B_EXTI							"PB14"		/* TIM3_CH1 */			// ���PWM���жϵ�IO��
#define 	PWM_B_OUT								"XXX"  		/* TIM3_CH1 */			// ���PWM�������IO��
#define 	DIR_B										"XXX" 		/*          */ 			// ����˶�����

#define 	EXTI_LINE_UrgentStop		5					/* �ж���5  */		// �ж��ߣ���ͣ��ť���ж���
#define		EXTI_UrgentStop_IO			"PF5"												// ��ͣ���ж�IO�ڣ���Ӧ̽���߰����ϵ�KEY_UP			

// ���԰�ť�ж� ����ɾ��
#define 	_KEY1 						"XXX" 		// ���º�����
//#define 	_KEY1 						"PE3" 		// ���º�����

/******* �ж�IO��  ע�⣺��PA11ֻ�����ӵ��ж���11 ***********/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////�ɵ����ذ�////////////////////////////////////////////////////////////////
#elif (2 == BOARD)

/* �������� USART1 USART2 USART3  */
#define 	EN_USART3 	1						// ��ʹ�ܴ���2�� EN_USART2 	1
#define 	BOUND_X			115200

/* LED IO�� */
#define 	GPIO_LED				"PD6"

/* ��λ IO�� */
#define 	X_P_LIMIT 			"PF5"					// X+��λ
#define 	X_N_LIMIT 			"PF6"					// X-��λ ����ΪX��λ
#define 	Y_P_LIMIT 			"PF7"					// Y+��λ
#define 	Y_N_LIMIT 			"PF8"					// Y-��λ ����ΪY��λ
#define 	Z_P_LIMIT 			"PF10"					// Z+��λ
#define 	Z_N_LIMIT 			"PF13"					// Z-��λ ����ΪZ��λ
#define 	A_LIMIT 				"PF14"					// A��λ
#define 	B_LIMIT 				"PF15"					// B��λ

/* ������IO�� */
#define		EXTI_LINE_MAINMOTOR			6															// �ж���
#define		EXTI_MAINMOTOR_START		"PG6"													// ����������IO�ڣ����ֶ�����
#define 	PWM_MAINMOTOR_OUT				"PF9"			/* TIM14_CH1 */
//#define 	PWM_MAINMOTOR_DIR				"XXX"

/******* �ж�IO��  ע�⣺��PA11ֻ�����ӵ��ж���11 ***********/
/* ������������IO��  �����жϼ����IO�� ע��PWM��IO�ڲ������ѡ�� */
#define		EXTI_LINE_X							1															// �ж��ߣ������ĵ��ж�IO�ڶ�Ӧ  ��PC4->�ж���4
#define 	EXTI_X_PLUS							"PB1" 		/* �ж���1 */				// ����PWM���ж�IO��
#define 	PWM_X_EXTI							"PB5"			/* TIM3_CH2 */			// ���PWM���жϵ�IO��  	
#define 	PWM_X_OUT								"PA6"  		/* TIM3_CH1 */			// ���PWM�������IO��		 ̽���߰�������
#define 	DIR_X										"PF0" 		/*          */ 			// ����˶�����

#define		EXTI_LINE_Y							0															// �ж��ߣ������ĵ��ж�IO�ڶ�Ӧ  ��PC4->�ж���4
#define 	EXTI_Y_PLUS							"PB0"			/* �ж���0  */			// ����PWM���ж�IO��  ̽���߰����ϲ������ж���1����ʼ��ʱ�ᱻ���
#define 	PWM_Y_EXTI							"PA3"			/* TIM2_CH4 */			// ���PWM���жϵ�IO��   	
#define 	PWM_Y_OUT								"PA5"  		/* TIM2_CH1 */			// ���PWM�������IO��
#define 	DIR_Y										"PF1" 		/*          */ 			// ����˶�����

#define		EXTI_LINE_Z							2															// �ж��ߣ������ĵ��ж�IO�ڶ�Ӧ  ��PC4->�ж���4
#define 	EXTI_Z_PLUS							"PE2"			/* �ж���2 */				// ����PWM���ж�IO��
#define 	PWM_Z_EXTI							"PB8"			/* TIM4_CH3 */			// ���PWM���жϵ�IO��
#define 	PWM_Z_OUT								"PB9"  		/* TIM4_CH4 */			// ���PWM�������IO��
#define 	DIR_Z										"PF2" 		/*          */ 			// ����˶�����

#define		EXTI_LINE_A							4															// �ж��ߣ������ĵ��ж�IO�ڶ�Ӧ  ��PC4->�ж���4
#define 	EXTI_A_PLUS							"PE4"			/* �ж���4  */			// ����PWM���ж�IO��
#define 	PWM_A_EXTI							"PE6"			/* TIM9_CH2 */			// ���PWM���жϵ�IO��
#define 	PWM_A_OUT								"PE5"  		/* TIM9_CH1 */			// ���PWM�������IO��
#define 	DIR_A										"PF3" 		/*          */ 			// ����˶�����

#define		EXTI_LINE_B							3															// �ж��ߣ������ĵ��ж�IO�ڶ�Ӧ  ��PC4->�ж���4
#define 	EXTI_B_PLUS							"PE3"			/* �ж���3  */			// ����PWM���ж�IO��
#define 	PWM_B_EXTI							"PB14"		/* TIM12_CH1*/			// ���PWM���жϵ�IO��
#define 	PWM_B_OUT								"PB15"  	/* TIM12_CH2*/			// ���PWM�������IO��
#define 	DIR_B										"PF4" 		/*          */ 			// ����˶�����

#define 	EXTI_LINE_UrgentStop		5					/* �ж���5  */			// �ж��ߣ���ͣ��ť���ж���
#define		EXTI_UrgentStop_IO			"PG5"													// ��ͣ���ж�IO�ڣ���Ӧ̽���߰����ϵ�KEY_UP			

/******* �ж�IO��  ע�⣺��PA11ֻ�����ӵ��ж���11 ***********/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
/*****************************************************************************************************************************************/









/*********************************************************ͨѶЭ��*************************************************************************/
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

/*  0x0C // ���� : ��Ӧָ����	 */
#define 	MAINAXIS_WORK				0x08			// ���Ṥ��
#define 	MAINAXIS_STOP				0x01			// ����ֹͣ
/*****************************************************************************************************************************************/






/********************************************************����****************************************************************************/
/* �˶�ϵͳ���˶�������  */
#define AXIS_NUM 		5     	// 5��ϵͳ ˳��X��Y��Z��A��B
/*****************************************************************************************************************************************/



// �ߴ������������⣬��ʱδ������ȷ�����
typedef enum {TBD_DIR = 0, POS_DIR = 1, NEG_DIR = 2} Motor_Dir; 		// ����˶�����


#endif
