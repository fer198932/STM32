/********************************************************************************************************
* ִ������Ĵ������
********************************************************************************************************/
#include "command.h"

// ���ڷ���������
extern Proc_Data proc_Data; 		// �������ݣ��г�Աָ��plus_Data
extern Plus_Data plus_Data;		// �������ݣ����Ƶ���˶�

Proc_Data cmd_Proc_Data; 		// �������ݣ��г�Աָ��plus_Data
Plus_Data cmd_Plus_Data;			// �������ݣ����Ƶ���˶�

// �����ڷ��������ݸ��Ƶ����صĽṹ���� 
static void usartData2cmd(void)
{
	cmd_Plus_Data = plus_Data;
	cmd_Proc_Data = proc_Data;
	cmd_Proc_Data.cmd_Data = &plus_Data; 		// ����ָ��
	// ��־λ ���ݸ�����󣬿��Խ�����һ��ָ��
//	DataPro_IsOK_Flag = IS_OK;
}

// �Լ����
void selfCheckFunc(void)
{
	const u8 length = 8;
	u8 backResString[length];
	
	// ���ݸ���
	usartData2cmd();
	
	cmd_Proc_Data.resp_Status = 0x01;
	// 	temp = UrgentStopTest();		//��ͣ���ؼ�� 0:���£�1:�ɿ���2:������
	
	setRespStr(backResString, length);
	
	// ����λ���ַ���������
	backResString[6] = cmd_Proc_Data.resp_Status;
	
	// ���ڷ���
#if PRIN2DISP
	respUsartMsg("SELCK\r\n", length);
#else
	respUsartMsg(backResString, length);
#endif
}

// �˶����ݴ������
void motionDataProc(void)
{
	
	// ���ݸ���
	usartData2cmd();
	
	// �������ͬʱ��ʼ�˶�
	StepMotor_Start();
	
}



