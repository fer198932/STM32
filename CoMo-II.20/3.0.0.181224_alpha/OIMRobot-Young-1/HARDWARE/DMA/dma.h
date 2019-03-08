#ifndef __DMA_H
#define	__DMA_H	   
#include "sys.h"
#include "config.h"
#include "buffer.h"
#include "usart.h"	

//////////////////////////////////////////////////////////////////////////////////	 
// DMA����
// 1��Ӧ���ڴ���ͨ��
// 2019��1��25��17:23:04 byYJY 
////////////////////////////////////////////////////////////////////////////////// 	 

#if (1 == USART_EN)
	#define	DMA_Stream_Tx 	DMA2_Stream7
	#define DMA_Stream_Rx 	DMA2_Stream5
	#define DMA_Channel_Tx 	DMA_Channel_4
	#define DMA_Channel_Rx 	DMA_Channel_4
	// ������ɱ�־λ
	#define DMA_FLAG_Tx 		DMA_FLAG_TCIF7
	#define DMA_FLAG_Rx	 		DMA_FLAG_TCIF5
#elif (2 == USART_EN)
	#define	DMA_Stream_Tx 	DMA1_Stream6
	#define DMA_Stream_Rx 	DMA1_Stream5
	#define DMA_Channel_Tx 	DMA_Channel_4
	#define DMA_Channel_Rx 	DMA_Channel_4
	// ������ɱ�־λ
	#define DMA_FLAG_Tx 		DMA_FLAG_TCIF6
	#define DMA_FLAG_Rx	 		DMA_FLAG_TCIF5
#elif (3 == USART_EN)
	#define	DMA_Stream_Tx 	DMA1_Stream3
	#define DMA_Stream_Rx 	DMA1_Stream1
	#define DMA_Channel_Tx 	DMA_Channel_4
	#define DMA_Channel_Rx 	DMA_Channel_4
	// ������ɱ�־λ
	#define DMA_FLAG_Tx 		DMA_FLAG_TCIF3
	#define DMA_FLAG_Rx	 		DMA_FLAG_TCIF1
#else
#endif	

// ���ڽ���һ��DMA���� ����
// #define DMA_USART_SEND(amount) (DMA_Restart((DMA_Stream_Tx), (amount)))

// DMA���ó���
// DMA_Streamx:DMA������,
// chx:DMAͨ��ѡ��
// p_ar:�����ַ
// m_ar:�洢����ַ
// amount:���ݴ�����  
// dir:���䷽��
// DMA_Mode_X: ��ͨģʽ  ����ѭ��ģʽ
void DMA_Config(DMA_Stream_TypeDef *DMA_Streamx, uint32_t chx, uint32_t p_ar, 
	uint32_t m_ar, uint32_t amount, uint32_t dir, uint32_t DMA_Mode_X);

// ����һ��DMA����ĳ���
void DMA_Restart(DMA_Stream_TypeDef *DMA_Streamx, uint32_t amount);

// ����ʹ��DMA��ʽ�ĳ�ʼ������
void DMA_USART_Init(void);
















#endif






























