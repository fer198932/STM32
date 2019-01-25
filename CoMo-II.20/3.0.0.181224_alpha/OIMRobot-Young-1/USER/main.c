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
		
//    printf("t:%d\r\n",t);
		delay_ms(500);
		t++;
		
//		if(X_P_Lim())
//			printf("û����λ\r\n");
//		else
//			printf("��λ\r\n");
		
//		LED0 = t%2;
		DMA_USART_SEND(10);
		while(1)
		    {
				if(DMA_GetFlagStatus(DMA2_Stream7,DMA_FLAG_TCIF7)!=RESET)//�ȴ�DMA2_Steam7�������
				{ 
					DMA_ClearFlag(DMA2_Stream7,DMA_FLAG_TCIF7);//���DMA2_Steam7������ɱ�־
					break; 
		        } 
		    }	
	}
  
#endif
}


// ϵͳ��ʼ������
void Sys_Init(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);					// �жϷ�������
	uart_init(BOUND_X);
	DMA_USART_Init();		// ���ڵ�DMA���䷽ʽ
	delay_init(_SYSCLK);
	LED_Init();
	limit_IO_Init();		
}

// ϵͳ������ʹ�ܺ�������LEDʹ�ܡ��ж�ʹ�ܵ�
void Sys_Enable(void)
{
//	USART_Cmd(USART_X, ENABLE);			// ����ʹ��	
	///////////// ���и����ж�ʹ��  byYJY ///////////////
	
}







