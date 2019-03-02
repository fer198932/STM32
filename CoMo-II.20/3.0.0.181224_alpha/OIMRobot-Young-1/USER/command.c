/********************************************************************************************************
* ִ������Ĵ������
********************************************************************************************************/
#include "command.h"


extern		Flag_Structure 								flag_Struct;
extern 		volatile 	FunctionalState 		Offline_Work_Flag; 					// �����ѻ��ӹ��ı��
extern 		FunctionalState	 							nAxisStatus[AXIS_NUM];  		// �����Ƿ�����˶��ı�־

extern 		Motion_Strcuture 							motion_Data;	
extern 		Motion_Strcuture 							motion_Data_Pre;	


// ���ڷ���������
//extern Proc_Data proc_Data; 		// �������ݣ��г�Աָ��plus_Data
//extern Plus_Data plus_Data;			// �������ݣ����Ƶ���˶�

//Proc_Data cmd_Proc_Data; 				// �������ݣ��г�Աָ��plus_Data
//Plus_Data cmd_Plus_Data;				// �������ݣ����Ƶ���˶�


/* ��������������  */
#if PRIN2DISP
#else
extern	u8	 backResString_Motion[RESP_MOTIONMSG_LENGTH];
#endif	


// ���ڷ����������
void CMD_Proc(void)
{	
//	usartData2cmd(); 		// ���ƴ������ݵ���������
	
	/* ���������˶�����:��ˮ��ģʽ����Լʱ�� */
	if(DATAINFO == motion_Data_Pre.cmd_Datas.cmd_Type)
	{
		if(PLUS_DATA == motion_Data_Pre.cmd_Datas.cmd_Excute)
		{
			// �˶�״̬��ʼ��
			AddSubSpeedInit_Pre();
			
			/* �ظ���λ��  */
			// ���÷�����λ��������
#if PRIN2DISP
#else
			setRespStr_Motion(&(motion_Data_Pre.cmd_Datas), backResString_Motion, RESP_MOTIONMSG_LENGTH, 0x01);
#endif	
//#if PRIN2DISP
////			respUsartMsg("PWM_CMD\r\n", 10);
//#else
//			respUsartMsg(backResString_Motion, RESP_MOTIONMSG_LENGTH);
//#endif	
		}
	}
}


// ִ������
void CMD_Execute(void)
{
	usartData2cmd(); 		// ���ƴ����������
	
	switch(motion_Data.cmd_Datas.cmd_Type)
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
void usartData2cmd(void)
{
	mymemcpy(&motion_Data, &motion_Data_Pre, sizeof(motion_Data));
	
	// ��־λ
	flag_Struct.Cmd_ProcDone_Flag = RESET;
	
//	cmd_Plus_Data = plus_Data;
//	cmd_Proc_Data = proc_Data;
//	mymemcpy(&cmd_Plus_Data, &plus_Data, sizeof(cmd_Plus_Data));
//	mymemcpy(&cmd_Proc_Data, &proc_Data, sizeof(cmd_Proc_Data));
//	cmd_Proc_Data.cmd_Data = &cmd_Plus_Data; 		// ����ָ��
	
	// ��־λ ���ݸ�����󣬿��Խ�����һ�δ������ݴ���
//	Cmd_Copied_Falg = ENABLE; 			
}

// �Լ����
void selfCheckFunc(void)
{
	const u8 length = 8;
	u8 backResString[length];
	
	motion_Data.cmd_Datas.resp_Status = 0x01;
	// 	temp = UrgentStopTest();		//��ͣ���ؼ�� 0:���£�1:�ɿ���2:������
	
	setRespStr(&(motion_Data.cmd_Datas), backResString, length);
	
	// ����λ���ַ���������
	backResString[6] = motion_Data.cmd_Datas.resp_Status;
	
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
	switch(motion_Data.cmd_Datas.cmd_Excute)
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
	
/* ��������������  */
#if PRIN2DISP
#else
	respUsartMsg(backResString_Motion, RESP_MOTIONMSG_LENGTH);
#endif	
}



