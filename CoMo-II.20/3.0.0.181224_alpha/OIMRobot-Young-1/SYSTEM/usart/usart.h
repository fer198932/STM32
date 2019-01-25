#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "config.h"
#include "dma.h"
//////////////////////////////////////////////////////////////////////////////////	 
// ���ڳ���
////////////////////////////////////////////////////////////////////////////////// 	

#define USART_REC_LEN  			200  	//�����������ֽ��� 200

#if EN_USART1
	#define USART_X USART1
#elif EN_USART2
	#define USART_X USART2
#elif EN_USART3
	#define USART_X USART3
#else
#endif
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
// extern u16 USART_RX_STA;         		//����״̬���	
//����봮���жϽ��գ��벻Ҫע�����º궨��
void uart_init(u32 bound);


// �����жϷ������
#define USART_IRQ \
do \
{ \
	{ \
		if(USART_GetITStatus(USART_X, USART_IT_IDLE) != RESET)  /* �����ж�(���յ������ݱ�����0x0d 0x0a��β) */ \
		{ \
			Res = USART_X->SR;    /* ͨ����SR(״̬�Ĵ���)��DR(���ݼĴ���)���ж� */   \
			Res = USART_X->DR;		\
			DMA_USART_SEND(10);  /* ���ڷ���һ������ */     \
		} 	 \
	} \
} while(0)



#endif

//if(USART_RX_STA&0x4000)/* ���յ���0x0d */ \
//{ \
//	if(Res!=0x0a)USART_RX_STA=0;/*���մ���,���¿�ʼ*/ \
//	else USART_RX_STA|=0x8000;	/*��������� */  \
//} \
//else /*��û�յ�0X0D*/ \
//{	\
//	if(Res==0x0d)USART_RX_STA|=0x4000; \
//	else \
//	{ \
//		USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ; \
//		USART_RX_STA++; \
//		if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;/*�������ݴ���,���¿�ʼ����	  */ \
//	}		 \
//} \


