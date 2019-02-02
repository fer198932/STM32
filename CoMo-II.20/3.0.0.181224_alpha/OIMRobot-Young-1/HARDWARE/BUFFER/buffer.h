#ifndef _BUFFER_H
#define _BUFFER_H
#include "sys.h"
#include "malloc.h"
#include "dma.h"	

/********************************************************************************************************
*   �������BUFFER�Ĺ������
********************************************************************************************************/

#define BUF_SIZE 				(1024-4)			// ��������С ��λ�ֽ�-8λ ��֤1024���ֽڶ���

// ʣ��ռ�С�ڸ�ֵʱ���ر�DMA���������ݱ�����(���ڷ��͵��������Ҫ������ֵ)
#define RESERVED_SIZE 	64						

// typedef enum { POS_SEQ = 0, NEG_SEQ = !POS_SEQ } sequence; 	// Ĭ��ΪPOS_SEQ״̬

// ��������������ݵĻ�����
typedef struct {
	u8 data[BUF_SIZE];				
	u16 start;					// ���ݵĿ�ʼλ��
	u16 end;						// ���ݵĽ���λ��
//	u16 length;					// ���ݳ��ȣ�����Ϊ0ʱ start=end=0
//	u8 bufStatus;				// ��������ʱ��������ѭ���������ݣ���״̬λ�жϿ�ʼλ�ͽ���λ�Ƿ���
} Buffer;

extern Buffer buffer_Rec; 				// �������ݵĻ�����
extern Buffer buffer_Trans;			// �������ݵĻ�����

// buffer�ĳ�ʼ������ ȫ����ʼ��Ϊ0 ������Ĭ�ϳ�ʼ�����
void buffer_Init(void);

// buffer���ú��� ���ý������ݵĻ�����
static void buffer_Reset(void);

// ���BUFFER�Ƿ��㹻�󣬷��������BUFFER
void checkBuffer(void);

#endif 

