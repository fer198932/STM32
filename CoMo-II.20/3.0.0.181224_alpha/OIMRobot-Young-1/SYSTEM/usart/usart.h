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
// 串口程序
////////////////////////////////////////////////////////////////////////////////// 	

// #define USART_REC_LEN  			BUF_SIZE  	//定义最大接收字节数 200

#if (1 == USART_EN)
	#define USART_X 		USART1
	#define USART_TX 		"PA9"						// 串口Tx
	#define USART_RX 		"PA10"					// 串口Rx
#elif (2 == USART_EN)
	#define USART_X 		USART2
	#define USART_TX 		"PA2"						// 串口Tx
	#define USART_RX 		"PA3"						// 串口Rx
#elif (3 == USART_EN)
	#define USART_X 		USART3
	#define USART_TX 		"PB10"					// 串口Tx
	#define USART_RX 		"PB11"					// 串口Rx
#else
	#error	USART Init Error!
#endif

// 初始化GPIO和串口的数据结构体
static void GPIO_AF_Usart_Init(USART_TypeDef* USARTx, 
	const char Pin_Tx_str[], const char Pin_Rx_str[]);

void uart_init(u32 bound);

// 串口空闲中断服务程序
void USART_IRQ_Macro(volatile u8 temp);


// 串口中断服务程序
#define USART_IRQ \
do \
{ \
	if(USART_GetITStatus(USART_X, USART_IT_IDLE) != RESET)  /* 空闲中断 */ \
	{ \
		/* DMA接收方式 */ 		\
		temp = USART_X->SR;    /* 通过读SR(状态寄存器)和DR(数据寄存器)清空闲中断 */   \
		temp = USART_X->DR;		\
			\
		/* end指向的位置=设置的接收长度-剩余的DMA缓存大小 （始终等于） */ 	\
		buffer_Rec.end = BUF_SIZE - DMA_GetCurrDataCounter(DMA_Stream_Rx);		\
			\
		if(RESET == USART_IDLE_Flag) 	/* 如果指令处理完成 */		\
			USART_IDLE_Flag = SET; 			/* 接收到一帧数据 */ 			\
	} 	  	\
} while(0)



#endif


//// 串口中断服务程序

/* DMA溢出判断，存档  */
//		/* 如果剩余的空间小于RESERVED_SIZE，关闭DMA （注：处理数据之后应重新开启）  */ \
//		if((BUF_SIZE + buffer_Rec.start - buffer_Rec.end) < RESERVED_SIZE) 		\
//		{ 	\
//			DMA_Out_Flag = SET; /* 不常发生，不用判断 */  \
//			DMA_Cmd(DMA_Stream_Rx, DISABLE); 	\
//			/*	USARTRx_DMAOut_Flag = ERROR; 		*/	\
//			respMsgError("DMA缓冲区溢出\r\n", 1); 	\
//		} \
		


//#define USART_IRQ \
//do \
//{ \
//	if(USART_GetITStatus(USART_X, USART_IT_IDLE) != RESET)  /* 空闲中断 */ \
//	{ \
//		temp = USART_X->SR;    /* 通过读SR(状态寄存器)和DR(数据寄存器)清空闲中断 */   \
//		temp = USART_X->DR;		\
//		/* DMA接收方式 */ 		\
//		/* end指向的位置=设置的接收长度-剩余的DMA缓存大小 （始终等于） */ 	\
//		buffer_Rec.end = BUF_SIZE - DMA_GetCurrDataCounter(DMA_Stream_Rx);		\
//		if(buffer_Rec.start <= buffer_Rec.end)  /* 正序 */ 		\
//		{ 			\
//			/* 如果剩余的空间小于RESERVED_SIZE，关闭DMA （注：处理数据之后应重新开启）  */ \
//			if((BUF_SIZE + buffer_Rec.start - buffer_Rec.end) < RESERVED_SIZE) 		\
//			{ 	\
//				DMA_Cmd(DMA_Stream_Rx, DISABLE); 	\
//				respMsgError("DMA缓冲区溢出\r\n", 1); 	\
//			} \
//		} 	\
//		else 																		/* 逆序 */  	\
//		{ 	\
//			/* 如果剩余的空间小于RESERVED_SIZE，关闭DMA （注：处理数据之后应重新开启）  */ 	\
//			if((buffer_Rec.start - buffer_Rec.end) < RESERVED_SIZE) 	\
//			{ 	\
//				DMA_Cmd(DMA_Stream_Rx, DISABLE); 	\
//				respMsgError("DMA缓冲区溢出\r\n", 1); 	\
//			} 	\
//		} 	\
//		if(IS_OK == DataPro_IsOK_Flag) /* 数据处理完后才能进行一下次处理 */ 	\
//			UsartDataProc();		/* 根据串口发送的命令进行处理  */  		\
//	} 	  	\
//} while(0)




//if(USART_RX_STA&0x4000)/* 接收到了0x0d */ \
//{ \
//	if(Res!=0x0a)USART_RX_STA=0;/*接收错误,重新开始*/ \
//	else USART_RX_STA|=0x8000;	/*接收完成了 */  \
//} \
//else /*还没收到0X0D*/ \
//{	\
//	if(Res==0x0d)USART_RX_STA|=0x4000; \
//	else \
//	{ \
//		USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ; \
//		USART_RX_STA++; \
//		if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;/*接收数据错误,重新开始接收	  */ \
//	}		 \
//} \


