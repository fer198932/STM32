/********************************************************************************************************
* ִ������Ĵ������
********************************************************************************************************/
#include "command.h"


extern		Flag_Structure 								flag_Struct;
extern 		volatile 	FunctionalState 		Offline_Work_Flag; 					// �����ѻ��ӹ��ı��
extern 		FunctionalState	 							nAxisStatus[AXIS_NUM];  		// �����Ƿ�����˶��ı�־

extern 		Motion_Strcuture 							motion_Data;	
extern 		Motion_Strcuture 							motion_Data_Pre;	

extern 		RespMsgArray									respMsgStr;										// �ظ���λ������Ϣ����	


// ���ڷ���������
//extern Proc_Data proc_Data; 		// �������ݣ��г�Աָ��plus_Data
//extern Plus_Data plus_Data;			// �������ݣ����Ƶ���˶�

//Proc_Data cmd_Proc_Data; 				// �������ݣ��г�Աָ��plus_Data
//Plus_Data cmd_Plus_Data;				// �������ݣ����Ƶ���˶�




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
			controlFunc();
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
}

// �Լ����
void selfCheckFunc(void)
{	
	u8 status;		// ��ͣ��ť��״̬
	
	motion_Data.cmd_Datas.resp_Status = 0x01;
	
	status = UrgentStopTest();		//��ͣ���ؼ�� 
	
	setRespStr_SlefCheck(&(motion_Data.cmd_Datas), respMsgStr.respMsg_SelfCheck, RespMsg_SELFCHECK_LENGTH, status);
	
	// ���ڷ���
	respUsartMsg(respMsgStr.respMsg_SelfCheck, RespMsg_SELFCHECK_LENGTH);
	
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
	
	/* �ظ���λ��  */
	// ���÷�����λ��������
	setRespStr_Motion(&(motion_Data_Pre.cmd_Datas), respMsgStr.respMsg_Motion, RespMsg_MOTION_LENGTH, 0x01);
	respUsartMsg(respMsgStr.respMsg_Motion, RespMsg_MOTION_LENGTH);	
}

// �����������ĺ���
void controlFunc(void)
{
	switch(motion_Data.cmd_Datas.cmd_Excute)
	{
		case MAINAXIS_WORK:
			mainMotorWork(Auto_Decode);
			break;
		
		case MAINAXIS_STOP:
			mainMotorStop();
			break;
		
		default:
			respMsgError("��������ָ��������\r\n", 1);
			break;		
	}
	
	/* �ظ���λ��  */
	// ���÷�����λ��������
	setRespStr_Control(&(motion_Data_Pre.cmd_Datas), respMsgStr.respMsg_Control, RespMsg_CONTROL_LENGTH, 0x01);
	respUsartMsg(respMsgStr.respMsg_Control, RespMsg_CONTROL_LENGTH);	
	
	// ����ִ�����ִ��������������λ
	flag_Struct.Cmd_Executing_Flag = RESET;
}



