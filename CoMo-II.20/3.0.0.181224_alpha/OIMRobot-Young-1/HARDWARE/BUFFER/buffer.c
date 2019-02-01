/********************************************************************************************************
*    �������BUFFER�Ĺ������
********************************************************************************************************/
#include "buffer.h"

Buffer buffer_Rec; 				// �������ݵĻ�����
Buffer buffer_Trans;			// �������ݵĻ�����

static const u16 Buf_Struct_Size = sizeof(Buffer); 			// Buffer �ṹ��Ĵ�С 

// buffer�ĳ�ʼ������ ȫ����ʼ��Ϊ0 ������Ĭ�ϳ�ʼ�����
void buffer_Init(void)
{
//	memset ��ʹ��
	mymemset(&buffer_Rec, 0, Buf_Struct_Size);
	mymemset(&buffer_Trans, 0, Buf_Struct_Size);
}

// buffer���ú��� ���ý������ݵĻ�����
static void buffer_Reset(void)
{
	buffer_Rec.start = 0;
	buffer_Rec.end = 0;
	// DMA����
	DMA_Restart(DMA_Stream_Rx, BUF_SIZE);
}

// ���BUFFER�Ƿ��㹻�󣬷��������BUFFER
void checkBuffer(void)
{
	// ���������С��Ԥ��ֵ���������û�����
		if((BUF_SIZE - buffer_Rec.end) < RESERVED_SIZE)
			buffer_Reset();	
}








