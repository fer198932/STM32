#ifndef __DMA_H
#define	__DMA_H	   
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
// DMA程序
// 1、应用在串口通信
// 2019年1月25日17:23:04 byYJY 
////////////////////////////////////////////////////////////////////////////////// 	 

// 串口进行一次DMA传输 发送
#define DMA_USART_SEND(amount) (DMA_Enable((DMA2_Stream7), (amount)))

// DMA配置程序
// DMA_Streamx:DMA数据流,
// chx:DMA通道选择
// p_ar:外设地址
// m_ar:存储器地址
// amount:数据传输量  
// dir:传输方向
void DMA_Config(DMA_Stream_TypeDef *DMA_Streamx, uint32_t chx, 
	uint32_t p_ar, uint32_t m_ar, uint32_t amount, uint32_t dir);

// 开启一次DMA传输的程序
void DMA_Enable(DMA_Stream_TypeDef *DMA_Streamx, uint32_t amount);

// 串口使用DMA方式的初始化函数
void DMA_USART_Init(void);
















#endif






























