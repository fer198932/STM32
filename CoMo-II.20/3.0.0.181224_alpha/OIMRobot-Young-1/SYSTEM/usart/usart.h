#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "config.h"
#include "dma.h"
#include "macro_operate.h"
#include "buffer.h"
#include "ComDataProc.h"
//////////////////////////////////////////////////////////////////////////////////	 
// ���ڳ���
////////////////////////////////////////////////////////////////////////////////// 	

// #define USART_REC_LEN  			BUF_SIZE  	//�����������ֽ��� 200

#if EN_USART1
	#define USART_X 		USART1
	#define USART_TX 		"PA9"						// ����Tx
	#define USART_RX 		"PA10"					// ����Rx
#elif EN_USART2
	#define USART_X 		USART2
	#define USART_TX 		"PA2"						// ����Tx
	#define USART_RX 		"PA3"						// ����Rx
#elif EN_USART3
	#define USART_X 		USART3
	#define USART_TX 		"PB10"					// ����Tx
	#define USART_RX 		"PB11"					// ����Rx
#else
#endif

// ��ʼ��GPIO�ʹ��ڵ����ݽṹ��
static void GPIO_AF_Usart_Init(USART_TypeDef* USARTx, 
	const char Pin_Tx_str[], const char Pin_Rx_str[]);

void uart_init(u32 bound);


// �����жϷ������
#define USART_IRQ \
do \
{ \
	if(USART_GetITStatus(USART_X, USART_IT_IDLE) != RESET)  /* �����ж� */ \
	{ \
		temp = USART_X->SR;    /* ͨ����SR(״̬�Ĵ���)��DR(���ݼĴ���)������ж� */   \
		temp = USART_X->DR;		\
		/* DMA���շ�ʽ */ 		\
		/* endָ���λ��=���õĽ��ճ���-ʣ���DMA�����С ��ʼ�յ��ڣ� */ 	\
		buffer_Rec.end = BUF_SIZE - DMA_GetCurrDataCounter(DMA_Stream_Rx);		\
		/* ���ʣ��Ŀռ�С��RESERVED_SIZE���ر�DMA ��ע����������֮��Ӧ���¿�����  */ \
		if((BUF_SIZE + buffer_Rec.start - buffer_Rec.end) < RESERVED_SIZE) 		\
		{ 	\
			DMA_Out_Flag = SET; /* ���������������ж� */  \
			DMA_Cmd(DMA_Stream_Rx, DISABLE); 	\
			/*	USARTRx_DMAOut_Flag = ERROR; 		*/	\
			respMsgError("DMA���������\r\n", 1); 	\
		} \
		if(RESET == USART_IDLE_Flag) 	/* ���ָ��ڴ����� */		\
			USART_IDLE_Flag = SET; 		/* ���յ�һ֡���� */ 			\
	} 	  	\
} while(0)



#endif


//// �����жϷ������
//#define USART_IRQ \
//do \
//{ \
//	if(USART_GetITStatus(USART_X, USART_IT_IDLE) != RESET)  /* �����ж� */ \
//	{ \
//		temp = USART_X->SR;    /* ͨ����SR(״̬�Ĵ���)��DR(���ݼĴ���)������ж� */   \
//		temp = USART_X->DR;		\
//		/* DMA���շ�ʽ */ 		\
//		/* endָ���λ��=���õĽ��ճ���-ʣ���DMA�����С ��ʼ�յ��ڣ� */ 	\
//		buffer_Rec.end = BUF_SIZE - DMA_GetCurrDataCounter(DMA_Stream_Rx);		\
//		if(buffer_Rec.start <= buffer_Rec.end)  /* ���� */ 		\
//		{ 			\
//			/* ���ʣ��Ŀռ�С��RESERVED_SIZE���ر�DMA ��ע����������֮��Ӧ���¿�����  */ \
//			if((BUF_SIZE + buffer_Rec.start - buffer_Rec.end) < RESERVED_SIZE) 		\
//			{ 	\
//				DMA_Cmd(DMA_Stream_Rx, DISABLE); 	\
//				respMsgError("DMA���������\r\n", 1); 	\
//			} \
//		} 	\
//		else 																		/* ���� */  	\
//		{ 	\
//			/* ���ʣ��Ŀռ�С��RESERVED_SIZE���ر�DMA ��ע����������֮��Ӧ���¿�����  */ 	\
//			if((buffer_Rec.start - buffer_Rec.end) < RESERVED_SIZE) 	\
//			{ 	\
//				DMA_Cmd(DMA_Stream_Rx, DISABLE); 	\
//				respMsgError("DMA���������\r\n", 1); 	\
//			} 	\
//		} 	\
//		if(IS_OK == DataPro_IsOK_Flag) /* ���ݴ��������ܽ���һ�´δ��� */ 	\
//			UsartDataProc();		/* ���ݴ��ڷ��͵�������д���  */  		\
//	} 	  	\
//} while(0)




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


