/********************************************************************************************************
* ִ������Ĵ������
********************************************************************************************************/
#include "command.h"


extern	Flag_Structure 	flag_Struct;
extern volatile 	FunctionalState 		Offline_Work_Flag; 		// �����ѻ��ӹ��ı��
extern FunctionalState	 nAxisStatus[AXIS_NUM];  		// �����Ƿ�����˶��ı�־

extern Motion_Strcuture 	motion_Data;	

// ���ڷ���������
extern Proc_Data proc_Data; 		// �������ݣ��г�Աָ��plus_Data
extern Plus_Data plus_Data;			// �������ݣ����Ƶ���˶�

Proc_Data cmd_Proc_Data; 				// �������ݣ��г�Աָ��plus_Data
Plus_Data cmd_Plus_Data;				// �������ݣ����Ƶ���˶�


/* ��������������  */
#if PRIN2DISP
#else
extern	u8	 backResString_Motion[RESP_MOTIONMSG_LENGTH];
#endif	


// ���ڷ����������
void CMD_Proc(void)
{	
	usartData2cmd(); 		// ���ƴ����������
	
	/* ���������˶�����:��ˮ��ģʽ����Լʱ�� */
	if(DATAINFO == cmd_Proc_Data.cmd_Type)
	{
		if(PLUS_DATA == cmd_Proc_Data.cmd_Excute)
		{
			// ���÷�����λ��������
#if PRIN2DISP
#else
			setRespStr_Motion(backResString_Motion, RESP_MOTIONMSG_LENGTH, 0x01);
#endif	
			// �˶�״̬��ʼ��
			AddSubSpeedInit_Pre();
			
			// �ظ���λ��
#if PRIN2DISP
//			respUsartMsg("PWM_CMD\r\n", 10);
#else
			respUsartMsg(backResString_Motion, RESP_MOTIONMSG_LENGTH);
#endif	
		}
	}
}


// ִ������
void CMD_Execute(void)
{
//	usartData2cmd(); 		// ���ƴ����������
	
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
//	cmd_Plus_Data = plus_Data;
//	cmd_Proc_Data = proc_Data;
	mymemcpy(&cmd_Plus_Data, &plus_Data, sizeof(cmd_Plus_Data));
	mymemcpy(&cmd_Proc_Data, &proc_Data, sizeof(cmd_Proc_Data));
	cmd_Proc_Data.cmd_Data = &cmd_Plus_Data; 		// ����ָ��
	
	// ��־λ ���ݸ�����󣬿��Խ�����һ�δ������ݴ���
//	Cmd_Copied_Falg = ENABLE; 			
}

// �Լ����
void selfCheckFunc(void)
{
	const u8 length = 8;
	u8 backResString[length];
	
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
	flag_Struct.Cmd_Executing_Flag = RESET;
}

// �˶����ݴ������
void motionDataProc(void)
{	
	switch(cmd_Proc_Data.cmd_Excute)
	{
		case PLUS_DATA:
			// �˶�״̬��ʼ��
			motion_Init();
			delay_us(MOTION_START_DALAY); 			// һ����ʱ
			
			// �������ͬʱ��ʼ�˶�
			StepMotor_Start();

#if NO_ADDSUBSPEED
		/* �رռӼ��� */
#else		
			// �Ӽ��ٶ�ʱ������
			EN_ADDSUB_TIMER;
#endif
			break;
		
		case OFFLINE_DATA:
			Offline_Work_Flag = ENABLE;
			break;
		
		default:
			respMsgError("�˶�����ָ��������\r\n", 1);
			break;			
	}
}

#if PRIN2DISP
#else
// �趨�˶����ݵĴ��ڷ�������ĸ�ʽ
static void setRespStr_Motion(u8 respStr[], u16 length, u8 status)
{
	setRespStr(respStr, RESP_MOTIONMSG_LENGTH);
	respStr[2] = DATAINFO;
	respStr[5] = PLUS_DATA;
	respStr[6] = 0x00;
	respStr[7] = status;
}
#endif



