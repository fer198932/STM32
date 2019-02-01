/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          ԭ���������������
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F407ZG �����޸�оƬ������
*                                              
* File : Main.C
* By  : Young
* 2018-12-24
*
*��д��λ�����Ƴ��� 2018��12��24��15:45:07
********************************************************************************************************/
#include "main.h"

/* ȫ�ֱ��� (��ҪΪ���ֱ�־λ) */
volatile 	FlagStatus 	DMA_Out_Flag 	= RESET;								// DMA��������³�ʼ��
volatile 	FlagStatus 	USART_IDLE_Flag 	= RESET; 						// ���ڿ����жϱ�־���������ݴ���
volatile 	FunctionalState 	Cmd_Done_Flag	= ENABLE;					// ָ�����ɱ�־
volatile	FunctionalState 	Cmd_Copied_Falg = ENABLE; 			// ���������Ƿ�ɸ��Ƶı�ǩ������ִ�����ſ�)


#if _TEST_ON_ALIENTEK
void Delay(__IO uint32_t nCount);

void Delay(__IO uint32_t nCount)
{
  while(nCount--){}
}
#endif

int main(void)
{
#if _TEST_ON_ALIENTEK
	
	u32 t=0;
	
	Sys_Init();		// ϵͳ��ʼ������
	Sys_Enable();	// ϵͳʹ�ܺ���
	
  while(1){
		/* ѭ��ɨ���Ƿ�����λ����  */
		procLimit();		

		/*  ���BUFFER�Ƿ���Ҫ���� */
		checkBuffer();						
		
		/* ��������˴��ڵ�DMA��������³�ʼ�� */
		if(SET == DMA_Out_Flag) 
		{
			DMA_USART_Init();
			DMA_Out_Flag = RESET;
		}
		
		/* �������ڿ����жϲ��ҿ��Խ������ݸ���ʱ�Ž���  */
		if((SET == USART_IDLE_Flag)	&& (ENABLE == Cmd_Copied_Falg))
		{
			Cmd_Copied_Falg = DISABLE; 				// ��������ǰ�������ٴ�������
			UsartDataProc();		
			USART_IDLE_Flag = RESET; 		
		}
		
		/* ִ������ */
		if(ENABLE == Cmd_Done_Flag)
		{
			Cmd_Done_Flag = DISABLE; 			// ִ�����ǰ�����ٴν���
			CMD_Execute();
			// ִ������������ط��ж�
		}
		
		
		
		// �������
//		if(t%2)
//			XAxisDir_Pos;
//		else
//			XAxisDir_Neg;
		
//    printf("t:%d\r\n",t);
//		delay_ms(500);
		t++;
		
//		TIM_Cmd(TIM7, ENABLE);
//		if(X_P_Lim())
//			printf("û����λ\r\n");
//		else
//			printf("��λ\r\n");
		
//		LED0 = t%2;

	}
  
#endif
}


// ϵͳ��ʼ������ ����ʼ����������ֻ����һ�Σ���ΪҰָ��Ĵ��ڿ��ܵ���ϵͳ���⣩
void Sys_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);					// �жϷ�������
	buffer_Init();					// ���ݻ�������ʼ��
	comData_Init();
	uart_init(BOUND_X);
	DMA_USART_Init();		// ���ڵ�DMA���䷽ʽ��ʼ��
	delay_init(_SYSCLK);
	LED_Init();
	limit_IO_Init();
	Timer_Init();	
	PWM_Init();
	EXTI_Config_Init();				// �ж�IO�ڳ�ʼ�� �ж�����ʼ��
	StepMotor_Init();				// �����������IO�ڳ�ʼ��
}

// ϵͳ������ʹ�ܺ�������LEDʹ�ܡ��ж�ʹ�ܵ�
void Sys_Enable(void)
{
//	USART_Cmd(USART_X, ENABLE);			// ����ʹ��	
	///////////// ���и����ж�ʹ��  byYJY ///////////////
	
	/* DMA ����ʹ�� */
	USART_DMACmd(USART_X, USART_DMAReq_Tx, ENABLE);  // ʹ�ܴ��ڵ�DMA����     
	USART_DMACmd(USART_X, USART_DMAReq_Rx, ENABLE);  // ʹ�ܴ��ڵ�DMA����
	
	/* �ж�ʹ�� */
	EXTI_Enable();
	
	/* ʹ�ܼӼ���ʱ�ӣ� �����ã���ɾ��  */
	TIM_Cmd(TIM7, ENABLE);
	
	/* ʹ��pwm�� �����ã���ɾ��  */
	PWM_Cmd(X_PWM, ENABLE, X_CH_EXTI);
	PWM_Cmd(X_PWM, ENABLE, X_CH_OUT);
}






