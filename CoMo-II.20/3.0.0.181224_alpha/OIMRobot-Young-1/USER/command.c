/********************************************************************************************************
* 执行命令的处理程序
********************************************************************************************************/
#include "command.h"


extern		Flag_Structure 								flag_Struct;
extern 		volatile 	FunctionalState 		Offline_Work_Flag; 					// 进入脱机加工的标记
extern 		FunctionalState	 							nAxisStatus[AXIS_NUM];  		// 各轴是否可以运动的标志

extern 		Motion_Strcuture 							motion_Data;	
extern 		Motion_Strcuture 							motion_Data_Pre;	


// 串口发来的数据
//extern Proc_Data proc_Data; 		// 命令数据，有成员指向plus_Data
//extern Plus_Data plus_Data;			// 脉冲数据，控制电机运动

//Proc_Data cmd_Proc_Data; 				// 命令数据，有成员指向plus_Data
//Plus_Data cmd_Plus_Data;				// 脉冲数据，控制电机运动


/* 反馈的命令数组  */
#if PRIN2DISP
#else
extern	u8	 backResString_Motion[RESP_MOTIONMSG_LENGTH];
#endif	


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
			
			/* 回复上位机  */
			// 设置反馈上位机的数组
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
	
//	cmd_Plus_Data = plus_Data;
//	cmd_Proc_Data = proc_Data;
//	mymemcpy(&cmd_Plus_Data, &plus_Data, sizeof(cmd_Plus_Data));
//	mymemcpy(&cmd_Proc_Data, &proc_Data, sizeof(cmd_Proc_Data));
//	cmd_Proc_Data.cmd_Data = &cmd_Plus_Data; 		// 重新指向
	
	// 标志位 数据复制完后，可以进行下一次串口数据处理
//	Cmd_Copied_Falg = ENABLE; 			
}

// 自检程序
void selfCheckFunc(void)
{
	const u8 length = 8;
	u8 backResString[length];
	
	motion_Data.cmd_Datas.resp_Status = 0x01;
	// 	temp = UrgentStopTest();		//急停开关检测 0:按下；1:松开；2:抖动；
	
	setRespStr(&(motion_Data.cmd_Datas), backResString, length);
	
	// 其余位的字符数据设置
	backResString[6] = motion_Data.cmd_Datas.resp_Status;
	
	// 串口发回
#if PRIN2DISP
	respUsartMsg("SELCK\r\n", length);
#else
	respUsartMsg(backResString, length);
#endif
	
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

#if NO_ADDSUBSPEED
		/* 关闭加减速 */
#else		
			// 加减速定时器开启
			EN_ADDSUB_TIMER;
#endif
			break;
		
		case OFFLINE_DATA:
			Offline_Work_Flag = ENABLE;
			break;
		
		default:
			respMsgError("运动数据指令码有误\r\n", 1);
			break;			
	}
	
/* 反馈的命令数组  */
#if PRIN2DISP
#else
	respUsartMsg(backResString_Motion, RESP_MOTIONMSG_LENGTH);
#endif	
}



