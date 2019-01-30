/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          ԭ���������������
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F407ZG
*                                              
* File : Main.C
* By  : Young
* 2018-12-24
*
*��д��λ�����Ƴ��� 2018��12��24��15:45:07
********************************************************************************************************/
#include "main.h"

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
		procLimit();							// ѭ��ɨ���Ƿ�����λ����
		checkBuffer();						// ���BUFFER�Ƿ���Ҫ����
		
//    printf("t:%d\r\n",t);
		delay_ms(500);
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


// ϵͳ��ʼ������
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
	PWM_Cmd(X_PWM, ENABLE, X_CH1_EXTI);
}







