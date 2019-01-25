#ifndef _COMDATAPROC_H
#define _COMDATAPROC_H
/*  数据处理程序 */
#include "sys.h"
#include "usart.h"

// 回复上位机的程序 异常或错误
void respMsgError(u8 status);

// 回复串口数据信息
void respUsartMsg(const u8 backResString[], u16 length);

#endif
