/********************************************************************************************************
* ִ������Ĵ������
********************************************************************************************************/
#include "command.h"

extern Proc_Data proc_Data; 		// �������ݣ��г�Աָ��plus_Data
extern Plus_Data plus_Data;			// �������ݣ����Ƶ���˶�

// �Լ����
void selfCheckFunc(void)
{
	const u8 length = 8;
	u8 backResString[length];
	proc_Data.resp_Status = 0x01;
	// 	temp = UrgentStopTest();		//��ͣ���ؼ�� 0:���£�1:�ɿ���2:������
	
	setRespStr(backResString, length);
	
	// ����λ���ַ���������
	backResString[6] = proc_Data.resp_Status;
	
	// ���ڷ���
#if PRIN2DISP
	respUsartMsg("SELCK\r\n", length);
#else
	respUsartMsg(backResString, length);
#endif
}



