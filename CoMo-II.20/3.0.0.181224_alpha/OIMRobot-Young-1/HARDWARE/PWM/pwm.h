#ifndef _PWM_H
#define _PWM_H
#include "sys.h"
#include "timer.h"

//////////////////////////////////////////////////////////////////////////////////	 
//  PWM���ɳ���  2019��1��29��20:16:48 byYJY							  
////////////////////////////////////////////////////////////////////////////////// 	

/*  TIM ����  */
#define 		X_PWM  			TIM3
#define 		X_CH_EXTI		2 					// ��Ӧͨ�� PWM_X_EXTI
#define 		X_CH_OUT		1 					// ��Ӧͨ�� PWM_X_OUT

#define 		Y_PWM  			TIM2
#define 		Y_CH_EXTI		4
#define 		Y_CH_OUT		1

#define 		Z_PWM  			TIM4
#define 		Z_CH_EXTI 	3
#define 		Z_CH_OUT		4

#define 		A_PWM  			TIM9
#define 		A_CH_EXTI		1
#define 		A_CH_OUT		2

#define 		B_PWM  			TIM12
#define 		B_CH_EXTI		1
#define 		B_CH_OUT		2

#define 		DUTY_CYCLE	0 		// ռ�ձȵĿ��أ����㼴�ر�ռ�ձȵ�����Ĭ��Ϊ50%�� ��ʱû�� byYJY

#define 		GPIO_ERROR	255 	// ����Ϊ��ֵʱ����������س�ʼ��


/* Ϊ���������Ч�ʣ�PWM��Ƶ�ʸı���ú�ʵ�� dc:ռ�ձ� */
#if DUTY_CYCLE
	#define 	xAxisSetPWM(psc, channel_exti, dc_exti, channel_out, dc_out) 		\
	do \
	{ \
		X_PWM->PSC = (psc); 			/* pscֵ����Ƶ�� */ \
		X_PWM->CCR##channel_exti = (dc_exti);  /* ռ�ձ��趨 */ \
		X_PWM->CCR##channel_out = (dc_out);   /* */ \
	} while(0)
#else
	#define 	nAxisSetPWM(TIM_N, psc) 		\
	do \
	{ \
		/* if(TIM_N->PSC != psc) */	\
			TIM_N->PSC = (psc); 			/* pscֵ����Ƶ�� */ \
	} while(0)
#endif

// ������TIM��ӳ���ϵ�ṹ��
typedef struct {
	u8 										nAxis;
	TIM_TypeDef*	 				TIM_N;
	u8 										ch_exti;								// �ж�ͨ��
	u8 										ch_out;									// ���ͨ��
	// �ж���
	GPIO_Structure_XX 		GPIO_PWM_EXTI; 					// �ж�GPIO�ڵĽṹ��
	GPIO_Structure_XX 		GPIO_PWM_OUT; 					// ���GPIO�ڵĽṹ��
} NAxis_TIM_Structure;
	
typedef enum {PWM_EXTI = 0, PWM_OUT = !PWM_EXTI} PWM_Channel; 		// PWM��ͨ��
	
// PWM��ʼ������
void PWM_Init(void);
	
// ���ָ������TIM֮���ӳ���ϵ
void nAxis2TIM(u8 nAxis, NAxis_TIM_Structure* 	nAxis_TIM_Structure);

// ����CLK�趨PWM�����Ƶ��
// void setAxisPWM(u8 nAxis, u32 clk, u16 dutyCycle1, u16 dutyCycle2);

// ��ʱ����ʼ�� ch1��ch2ΪPWM���ͨ��(Ϊ0ɶ������)
void Tim_PWM_Init(const NAxis_TIM_Structure* 	nAxis_TIM_Structure, uint8_t NVIC_IRQPreemptionPriority, 
	uint8_t NVIC_IRQSubPriority, u16 arr, u16 psc, PWM_Channel PWM_ch);

// ��ʱ�����PIN�ų�ʼ��
static void GPIO_PWM_Init(const GPIO_Structure_XX *GPIO_Temp);

// ���ͨ������ DC��duty cycle ռ�ձ�
static void Set_PWM_Channel(TIM_TypeDef* TIM_N, TIM_OCInitTypeDef*  TIM_OCInitStructure, u8 ch1, u16 DC);

// PWM �رջ�ʹ��
void PWM_Cmd(TIM_TypeDef* TIM_N, FunctionalState state, u8 ch);

// PWM ǿ�����߻�ȡ�� ch:���ͨ��
static void PWM_Forced2High(TIM_TypeDef* TIM_N, FunctionalState state, u8 ch);

// ͨ��CLKƵ�ʼ���Ԥ��Ƶϵ�� PSC
u16 calPSC(u32 clk, u8 nAxis);
// ͨ��PSC����Ƶ�ʣ� ע��Ĵ����������  ��λHZ
u32 calClk_PSC(u16 psc);


#endif
