/********************************************************************************************************
*    输入输出BUFFER的管理程序
********************************************************************************************************/
#include "buffer.h"

Buffer buffer_Rec; 				// 接收数据的缓冲区
Buffer buffer_Trans;			// 发送数据的缓冲区

static const u16 Buf_Struct_Size = sizeof(Buffer); 			// Buffer 结构体的大小 

// buffer的初始化函数 全部初始化为0 不考虑默认初始化情况
void buffer_Init(void)
{
//	memset 不使用
	mymemset(&buffer_Rec, 0, Buf_Struct_Size);
	mymemset(&buffer_Trans, 0, Buf_Struct_Size);
}

// buffer重置函数 重置接收数据的缓冲区
static void buffer_Reset(void)
{
	buffer_Rec.start = 0;
	buffer_Rec.end = 0;
	// DMA重置
	DMA_Restart(DMA_Stream_Rx, BUF_SIZE);
}

// 检查BUFFER是否足够大，否则就重置BUFFER
void checkBuffer(void)
{
	// 如果缓冲区小于预留值，重新设置缓冲区
		if((BUF_SIZE - buffer_Rec.end) < RESERVED_SIZE)
			buffer_Reset();	
}








