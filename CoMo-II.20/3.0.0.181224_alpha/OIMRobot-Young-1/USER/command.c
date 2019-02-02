/********************************************************************************************************
* ִ������Ĵ������
********************************************************************************************************/
#include "command.h"

extern volatile	FunctionalState 	Cmd_Copied_Falg; 			// ���������Ƿ�ɸ��Ƶı�ǩ������ִ�����ſ�)

// ���ڷ���������
extern Proc_Data proc_Data; 		// �������ݣ��г�Աָ��plus_Data
extern Plus_Data plus_Data;			// �������ݣ����Ƶ���˶�

Proc_Data cmd_Proc_Data; 				// �������ݣ��г�Աָ��plus_Data
Plus_Data cmd_Plus_Data;				// �������ݣ����Ƶ���˶�

extern volatile FunctionalState	 nAxisStatus[AXIS_NUM];  		// �����Ƿ�����˶� 


// ִ������
void CMD_Execute(void)
{
	usartData2cmd(); 		// ���ƴ����������
	
	switch(cmd_Proc_Data.cmd_Type)
	{
		case SELFCHECK:
			selfCheckFunc();
			break;
		
		case CONTROLINFO:
			
			break;
		
		case DATAINFO:
			motionDataProc();
			break;
		
		case STATUSINFO:
			
			break;
		
		case INVALID_CMD:
			respMsgError("��Ч���\r\n", 1);
			break;
		
		default:
			respMsgError("�������������ʱ����\r\n", 1);
			break;		
		
	}
	
}

// �����ڷ��������ݸ��Ƶ����صĽṹ���� 
static void usartData2cmd(void)
{
	cmd_Plus_Data = plus_Data;
	cmd_Proc_Data = proc_Data;
	cmd_Proc_Data.cmd_Data = &cmd_Plus_Data; 		// ����ָ��
	
	// ��־λ ���ݸ�����󣬿��Խ�����һ�δ������ݴ���
	Cmd_Copied_Falg = ENABLE; 			
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
	
	// �Լ����ִ�����ִ��������������λ
	
}

// �˶����ݴ������
void motionDataProc(void)
{
	u8 i;
	// ����״̬��ʼ�� ������
	for(i=0; i<AXIS_NUM; i++)
		nAxisStatus[i] = DISABLE;
	
	// �������ͬʱ��ʼ�˶�
	StepMotor_Start();
	
}



