#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stm32f4xx_conf.h"
#include "sys.h" 
#include "config.h"
#include "dma.h"
//////////////////////////////////////////////////////////////////////////////////	 
// 串口程序
////////////////////////////////////////////////////////////////////////////////// 	

#define USART_REC_LEN  			200  	//定义最大接收字节数 200

#if EN_USART1
	#define USART_X USART1
#elif EN_USART2
	#define USART_X USART2
#elif EN_USART3
	#define USART_X USART3
#else
#endif
	  	
extern u8  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
// extern u16 USART_RX_STA;         		//接收状态标记	
//如果想串口中断接收，请不要注释以下宏定义
void uart_init(u32 bound);


// 串口中断服务程序
#define USART_IRQ \
do \
{ \
	{ \
		if(USART_GetITStatus(USART_X, USART_IT_IDLE) != RESET)  /* 接收中断(接收到的数据必须是0x0d 0x0a结尾) */ \
		{ \
			Res = USART_X->SR;    /* 通过读SR(状态寄存器)和DR(数据寄存器)清中断 */   \
			Res = USART_X->DR;		\
			DMA_USART_SEND(10);  /* 串口发送一次数据 */     \
		} 	 \
	} \
} while(0)



#endif

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


