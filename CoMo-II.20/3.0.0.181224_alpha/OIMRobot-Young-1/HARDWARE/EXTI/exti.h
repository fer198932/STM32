#ifndef __EXTI_H
#define __EXTI_H	 
#include "sys.h" 
#include "config.h"
#include "ComDataProc.h"
#include "main.h"

/********************************************************************************************************
*       �жϵ�IO�ڴ������  2019��1��29��09:26:51 byYJY
********************************************************************************************************/


/* �ж��߱����� */
#if 		((EXTI_LINE_X < 0) || (EXTI_LINE_X > 16) || (EXTI_LINE_X == EXTI_LINE_Y) || (EXTI_LINE_X == EXTI_LINE_Z) || \
					(EXTI_LINE_X == EXTI_LINE_A) || (EXTI_LINE_X == EXTI_LINE_B) || (EXTI_LINE_X == EXTI_LINE_UrgentStop) || (EXTI_LINE_X == EXTI_LINE_MAINMOTOR))
#error 	EXTI_LINE_X has problem!
#endif

#if 		((EXTI_LINE_Y < 0) || (EXTI_LINE_Y > 16) || (EXTI_LINE_Y == EXTI_LINE_X) || (EXTI_LINE_Y == EXTI_LINE_Z) || \
					(EXTI_LINE_Y == EXTI_LINE_A) || (EXTI_LINE_Y == EXTI_LINE_B) || (EXTI_LINE_Y == EXTI_LINE_UrgentStop) || (EXTI_LINE_Y == EXTI_LINE_MAINMOTOR))
#error 	EXTI_LINE_Y has problem!
#endif

#if 		((EXTI_LINE_Z < 0) || (EXTI_LINE_Z > 16) || (EXTI_LINE_Z == EXTI_LINE_X) || (EXTI_LINE_Z == EXTI_LINE_Y) || \
					(EXTI_LINE_Z == EXTI_LINE_A) || (EXTI_LINE_Z == EXTI_LINE_B) || (EXTI_LINE_Z == EXTI_LINE_UrgentStop) || (EXTI_LINE_Z == EXTI_LINE_MAINMOTOR))
#error 	EXTI_LINE_Z has problem!
#endif

#if 		((EXTI_LINE_A < 0) || (EXTI_LINE_A > 16) || (EXTI_LINE_A == EXTI_LINE_X) || (EXTI_LINE_A == EXTI_LINE_Y) || \
					(EXTI_LINE_A == EXTI_LINE_Z) || (EXTI_LINE_A == EXTI_LINE_B) || (EXTI_LINE_A == EXTI_LINE_UrgentStop) || (EXTI_LINE_A == EXTI_LINE_MAINMOTOR))
#error 	EXTI_LINE_A has problem!
#endif

#if 		((EXTI_LINE_B < 0) || (EXTI_LINE_B > 16) || (EXTI_LINE_B == EXTI_LINE_X) || (EXTI_LINE_B == EXTI_LINE_Y) || \
					(EXTI_LINE_B == EXTI_LINE_Z) || (EXTI_LINE_B == EXTI_LINE_A) || (EXTI_LINE_B == EXTI_LINE_UrgentStop) || (EXTI_LINE_B == EXTI_LINE_MAINMOTOR))
#error 	EXTI_LINE_B has problem!
#endif

#if 		((EXTI_LINE_UrgentStop < 0) || (EXTI_LINE_UrgentStop > 16) || (EXTI_LINE_UrgentStop == EXTI_LINE_X) || 	\
					(EXTI_LINE_UrgentStop == EXTI_LINE_Y) || (EXTI_LINE_UrgentStop == EXTI_LINE_Z) || 	\
					(EXTI_LINE_UrgentStop == EXTI_LINE_A) || (EXTI_LINE_UrgentStop == EXTI_LINE_B) || (EXTI_LINE_UrgentStop == EXTI_LINE_MAINMOTOR))
#error 	EXTI_LINE_UrgentStop has problem!
#endif

#if 		((EXTI_LINE_MAINMOTOR < 0) || (EXTI_LINE_MAINMOTOR > 16) || (EXTI_LINE_MAINMOTOR == EXTI_LINE_X) || 	\
					(EXTI_LINE_MAINMOTOR == EXTI_LINE_Y) || (EXTI_LINE_MAINMOTOR == EXTI_LINE_Z) || 	\
					(EXTI_LINE_MAINMOTOR == EXTI_LINE_A) || (EXTI_LINE_MAINMOTOR == EXTI_LINE_B) || (EXTI_LINE_MAINMOTOR == EXTI_LINE_UrgentStop))
#error 	EXTI_LINE_MAINMOTOR has problem!
#endif

// �ж�����GPIOӳ���ϵ



// �ⲿ�жϳ�ʼ��		 	
void EXTI_Config_Init(void);		


// �жϷ������
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

// ��Ӧ��IO���жϳ�ʼ��
// ��Ӧ��IO���жϳ�ʼ��
static void EXTI_Line_Init(GPIO_Structure_XX *GPIO_Temp, const char str[], uint8_t Exti_Line_N, EXTITrigger_TypeDef EXTI_Trigger_N,   
	uint8_t NVIC_IRQPreemptionPriority,	uint8_t NVIC_IRQSubPriority);

// IO�ڵ�GPIO��ʼ��
static void EXTI_GPIO_Init(GPIO_Structure_XX *GPIO_Temp, const char str[], uint8_t Exti_Line_N);

// �ж�ʹ�ܻ�ر�
static void EXTI_Cmd(uint32_t EXTI_Line_N, FunctionalState state);

// �����ж���ʹ��
void EXTI_Enable(void);

/* Ϊ���������жϳ��� */
//#define EXTI_IRQ_PWM(EXTI_Line_n, n, TIM_N, nAxis)  			\
//do 		\
//{ \
//	EXTI->PR = EXTI_Line_n; 	/* ����ж� */
//	
//	pluNumPWM[n]++;
//	
//	/* ������ٽ׶� */
//	if((SUB_SPEED != addSubSpeed_Status[n]) && 
//		(Psc_Data_Cur[n].addSpeed_NeedPlusNum > (cmd_Plus_Data.plusNum[n] - pluNumPWM[n])))
//	{
////		ADDSUB_TIMER->CNT = 0;
//		addSubSpeed_Status[n] = SUB_SPEED; 
//	}
//	
//	/*  �˶���� �ر�PWM */
//	if(cmd_Plus_Data.plusNum[n] < pluNumPWM[n])
//	{
//		/* nAxis_StepMotor_Stop_MACRO(TIM_N, ch_exti, ch_out); 		*/
//		/* ��д���� */
//		
//		nAxisStatus[n] = DISABLE;
//		
//		/* respUsartMsg("PWM_EXTI\r\n", 10); */
//		
//		/* ���е��ᶼֹͣ��ֹͣ ADDSUB_TIMER  */
//		if(0 == nAxis_Motion_Flag)
//		{
//#if OFFLINE_WORK
//			Offline_Work_Flag = ENABLE;
//#endif
//			/* TIM_Cmd(ADDSUB_TIMER, DISABLE); */
//			TIM_N->CR1 &= (uint16_t)~TIM_CR1_CEN;
//		}
//	}
//	
//	\
//} while(0)


// �����õ��жϷ������
static void EXTI_IRQ_PWM_MACRO(u8 n, TIM_TypeDef *TIM_N, u8 ch_exti, u8 ch_out);

// �����õļ�ͣ�жϲ��Գ���
static void EXTI_IRQ_UrgentStop_MACRO(void);

// �����õĸ���ֹͣ����
void nAxis_StepMotor_Stop_MACRO(TIM_TypeDef* TIM_N, u8 ch_exti, u8 ch_out);


// ���������жϷ������
static void EXTI_IRQ_MainMotorWork_MACRO(void);


// �����ж��߱���ָ�����жϷ������
//#define 	EXTI_HANDLER(n)			\
//do		\
//{	\
//	#if (EXTI_LINE_X == n)		\
//	#endif		\
//}while(0);





#endif

























