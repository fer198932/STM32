#ifndef _COMMAND_H
#define _COMMAND_H
/*  ���ݴ������ */
#include "sys.h"
#include "config.h"
#include "main.h"

// ִ������
void CMD_Execute(void);

// �����ڷ��������ݸ��Ƶ����صĽṹ���� 
static void usartData2cmd(void);

// �Լ����
void selfCheckFunc(void);

// �˶����ݴ������
void motionDataProc(void);



#endif
