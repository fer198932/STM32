/********************************************************************************************************
* 执行命令的处理程序
********************************************************************************************************/
#include "command.h"


extern		Flag_Structure 								flag_Struct;
extern 		volatile 	FunctionalState 		Offline_Work_Flag; 					// 进入脱机加工的标记
extern 		FunctionalState	 							nAxisStatus[AXIS_NUM];  		// 各轴是否可以运动的标志

extern 		Motion_Strcuture 							motion_Data;	
extern 		Motion_Strcuture 							motion_Data_Pre;	

extern 		RespMsgArray									respMsgStr;										// 回复上位机的消息数组	


// 串口发来的数据
//extern Proc_Data proc_Data; 		// 命令数据，有成员指向plus_Data
//extern Plus_Data plus_Data;			// 脉冲数据，控制电机运动

//Proc_Data cmd_Proc_Data; 				// 命令数据，有成员指向plus_Data
//Plus_Data cmd_Plus_Data;				// 脉冲数据，控制电机运动




// 串口发来的命令处理
void CMD_Proc(void)
{	
//	usartData2cmd(); 		// 复制串口数据到本地命令
	
	/* 处理脉冲运动数据:流水线模式，节约时间 */
	if(DATAINFO == motion_Data_Pre.cmd_Datas.cmd_Type)
	{
		if(PLUS_DATA == motion_Data_Pre.cmd_Datas.cmd_Excute)
		{
			// 运动状态初始化
			AddSubSpeedInit_Pre();
		}
	}
}


// 执行命令
void CMD_Execute(void)
{
	usartData2cmd(); 		// 复制串口命令到本地
	
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
			respMsgError("无效命令！\r\n", 1);
			break;
		
		default:
			respMsgError("命令解析后运行时错误！\r\n", 1);
			break;		
		
	}
	
}

// 将串口发来的数据复制到本地的结构体中 
void usartData2cmd(void)
{
	mymemcpy(&motion_Data, &motion_Data_Pre, sizeof(motion_Data));
	
	// 标志位
	flag_Struct.Cmd_ProcDone_Flag = RESET;	
}

// 自检程序
void selfCheckFunc(void)
{	
	u8 status;		// 急停按钮的状态
	
	motion_Data.cmd_Datas.resp_Status = 0x01;
	
	status = UrgentStopTest();		//急停开关检测 
	
	setRespStr_SlefCheck(&(motion_Data.cmd_Datas), respMsgStr.respMsg_SelfCheck, RespMsg_SELFCHECK_LENGTH, status);
	
	// 串口发回
	respUsartMsg(respMsgStr.respMsg_SelfCheck, RespMsg_SELFCHECK_LENGTH);
	
	// 自检程序执行完后，执行命令标记重新置位
	flag_Struct.Cmd_Executing_Flag = RESET;
}

// 运动数据处理程序
void motionDataProc(void)
{	
	switch(motion_Data.cmd_Datas.cmd_Excute)
	{
		case PLUS_DATA:
			// 运动状态初始化
			motion_Init();
			delay_us(MOTION_START_DALAY); 			// 一定延时
			
			// 步进电机同时开始运动
			StepMotor_Start();

			// 加减速定时器开启
			EN_ADDSUB_TIMER;
			break;
		
		case OFFLINE_DATA:
			Offline_Work_Flag = ENABLE;
			break;
		
		default:
			respMsgError("运动数据指令码有误\r\n", 1);
			break;			
	}
	
	/* 回复上位机  */
	// 设置反馈上位机的数组
	setRespStr_Motion(&(motion_Data_Pre.cmd_Datas), respMsgStr.respMsg_Motion, RespMsg_MOTION_LENGTH, 0x01);
	respUsartMsg(respMsgStr.respMsg_Motion, RespMsg_MOTION_LENGTH);	
}

// 处理控制命令的函数
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
		
		case CHANGE_TOOL1:
			changeTools(1, Assmble_Tool);
			break;
		
		case CHANGE_TOOL2:
			changeTools(2, Assmble_Tool);
			break;
		
		case CHANGE_TOOL3:
			changeTools(3, Assmble_Tool);
			break;
		
		case CHANGE_TOOL4:
			changeTools(4, Assmble_Tool);
			break;
		
		case CLEAN_TOOLS:
			changeTools(flag_Struct.ToolsNum_Flag, Clean_Tool);
			break;
		
		case CALIBRATE_TOOL_POS:			// 命令: 0E 09 0C 08 00 14 00 FF
			calibrateToolPos();
			/* 不回复上位机 */
			flag_Struct.Cmd_Executing_Flag = RESET;
			return;
		
		default:
			respMsgError("控制命令指令码有误\r\n", 1);
			break;		
	}
	
	/* 回复上位机  */
	// 设置反馈上位机的数组
	setRespStr_Control(&(motion_Data_Pre.cmd_Datas), respMsgStr.respMsg_Control, RespMsg_CONTROL_LENGTH, 0x01);
	respUsartMsg(respMsgStr.respMsg_Control, RespMsg_CONTROL_LENGTH);	
	
	// 程序执行完后，执行命令标记重新置位
	flag_Struct.Cmd_Executing_Flag = RESET;
}



