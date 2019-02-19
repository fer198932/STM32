#ifndef _BUFFER_H
#define _BUFFER_H
#include "sys.h"
#include "malloc.h"
#include "dma.h"	

/********************************************************************************************************
*   输入输出BUFFER的管理程序
********************************************************************************************************/

#define BUF_SIZE 				100			// 缓冲区大小 单位字节-8位 

// 剩余空间小于该值时，关闭DMA，避免数据被覆盖(串口发送的数据最大不要超过该值)
#define RESERVED_SIZE 	10						

// 管理输入输出数据的缓冲区
typedef struct {
	u8 data[BUF_SIZE];				
	u16 start;					// 数据的开始位置
	u16 end;						// 数据的结束位置
//	u16 length;					// 数据长度，长度为0时 start=end=0
//	u8 bufStatus;				// 接收数据时，缓冲区循环接收数据，该状态位判断开始位和结束位是否倒置
} Buffer;

extern Buffer buffer_Rec; 				// 接收数据的缓冲区
extern Buffer buffer_Trans;				// 发送数据的缓冲区

// buffer的初始化函数 全部初始化为0 不考虑默认初始化情况
void buffer_Init(void);

// buffer重置函数 重置接收数据的缓冲区
void buffer_Reset(void);


#endif 

