#ifndef _COMDATAPROC_H
#define _COMDATAPROC_H
/*  ���ݴ������ */
#include "sys.h"
#include "usart.h"

// �ظ���λ���ĳ��� �쳣�����
void respMsgError(u8 status);

// �ظ�����������Ϣ
void respUsartMsg(const u8 backResString[], u16 length);

#endif
