#ifndef __DMA_H
#define	__DMA_H	   
#include "sys.h"
#include "config.h"
#include "buffer.h"
#include "usart.h"	

//////////////////////////////////////////////////////////////////////////////////	 
// DMA程序
// 1、应用在串口通信
// 2019年1月25日17:23:04 byYJY 
////////////////////////////////////////////////////////////////////////////////// 	 

#if (1 == USART_EN)
	#define	DMA_Stream_Tx 	DMA2_Stream7
	#define DMA_Stream_Rx 	DMA2_Stream5
	#define DMA_Channel_Tx 	DMA_Channel_4
	#define DMA_Channel_Rx 	DMA_Channel_4
	// 传输完成标志位
	#define DMA_FLAG_Tx 		DMA_FLAG_TCIF7
	#define DMA_FLAG_Rx	 		DMA_FLAG_TCIF5
#elif (2 == USART_EN)
	#define	DMA_Stream_Tx 	DMA1_Stream6
	#define DMA_Stream_Rx 	DMA1_Stream5
	#define DMA_Channel_Tx 	DMA_Channel_4
	#define DMA_Channel_Rx 	DMA_Channel_4
	// 传输完成标志位
	#define DMA_FLAG_Tx 		DMA_FLAG_TCIF6
	#define DMA_FLAG_Rx	 		DMA_FLAG_TCIF5
#elif (3 == USART_EN)
	#define	DMA_Stream_Tx 	DMA1_Stream3
	#define DMA_Stream_Rx 	DMA1_Stream1
	#define DMA_Channel_Tx 	DMA_Channel_4
	#define DMA_Channel_Rx 	DMA_Channel_4
	// 传输完成标志位
	#define DMA_FLAG_Tx 		DMA_FLAG_TCIF3
	#define DMA_FLAG_Rx	 		DMA_FLAG_TCIF1
#else
#endif	

// 串口进行一次DMA传输 发送
// #define DMA_USART_SEND(amount) (DMA_Restart((DMA_Stream_Tx), (amount)))

// DMA配置程序
// DMA_Streamx:DMA数据流,
// chx:DMA通道选择
// p_ar:外设地址
// m_ar:存储器地址
// amount:数据传输量  
// dir:传输方向
// DMA_Mode_X: 普通模式  还是循环模式
void DMA_Config(DMA_Stream_TypeDef *DMA_Streamx, uint32_t chx, uint32_t p_ar, 
	uint32_t m_ar, uint32_t amount, uint32_t dir, uint32_t DMA_Mode_X);

// 开启一次DMA传输的程序
void DMA_Restart(DMA_Stream_TypeDef *DMA_Streamx, uint32_t amount);

// 串口使用DMA方式的初始化函数
void DMA_USART_Init(void);
















#endif






























