#ifndef _COMMAND_H
#define _COMMAND_H
/*  ���ݴ������ */
#include "sys.h"
#include "config.h"
#include "main.h"
#include "AddSubSpeed_S.h"


// ִ������
void CMD_Execute(void);

// �����ڷ��������ݸ��Ƶ����صĽṹ���� 
void usartData2cmd(void);

// �Լ����
void selfCheckFunc(void);

// �˶����ݴ������
void motionDataProc(void);


// ���ڷ����������
void CMD_Proc(void);




#endif
