#ifndef __DMA_H
#define	__DMA_H	   
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
// DMA����
// 1��Ӧ���ڴ���ͨ��
// 2019��1��25��17:23:04 byYJY 
////////////////////////////////////////////////////////////////////////////////// 	 

// ���ڽ���һ��DMA���� ����
#define DMA_USART_SEND(amount) (DMA_Enable((DMA2_Stream7), (amount)))

// DMA���ó���
// DMA_Streamx:DMA������,
// chx:DMAͨ��ѡ��
// p_ar:�����ַ
// m_ar:�洢����ַ
// amount:���ݴ�����  
// dir:���䷽��
void DMA_Config(DMA_Stream_TypeDef *DMA_Streamx, uint32_t chx, 
	uint32_t p_ar, uint32_t m_ar, uint32_t amount, uint32_t dir);

// ����һ��DMA����ĳ���
void DMA_Enable(DMA_Stream_TypeDef *DMA_Streamx, uint32_t amount);

// ����ʹ��DMA��ʽ�ĳ�ʼ������
void DMA_USART_Init(void);
















#endif






























