/********************************************************************************************************
* 执行命令的处理程序
********************************************************************************************************/
#include "command.h"

// 串口发来的数据
extern Proc_Data proc_Data; 		// 命令数据，有成员指向plus_Data
extern Plus_Data plus_Data;		// 脉冲数据，控制电机运动

Proc_Data cmd_Proc_Data; 		// 命令数据，有成员指向plus_Data
Plus_Data cmd_Plus_Data;			// 脉冲数据，控制电机运动

// 将串口发来的数据复制到本地的结构体中 
static void usartData2cmd(void)
{
	cmd_Plus_Data = plus_Data;
	cmd_Proc_Data = proc_Data;
	cmd_Proc_Data.cmd_Data = &plus_Data; 		// 重新指向
	// 标志位 数据复制完后，可以进行下一次指令
//	DataPro_IsOK_Flag = IS_OK;
}

// 自检程序
void selfCheckFunc(void)
{
	const u8 length = 8;
	u8 backResString[length];
	
	// 数据复制
	usartData2cmd();
	
	cmd_Proc_Data.resp_Status = 0x01;
	// 	temp = UrgentStopTest();		//急停开关检测 0:按下；1:松开；2:抖动；
	
	setRespStr(backResString, length);
	
	// 其余位的字符数据设置
	backResString[6] = cmd_Proc_Data.resp_Status;
	
	// 串口发回
#if PRIN2DISP
	respUsartMsg("SELCK\r\n", length);
#else
	respUsartMsg(backResString, length);
#endif
}

// 运动数据处理程序
void motionDataProc(void)
{
	
	// 数据复制
	usartData2cmd();
	
	// 步进电机同时开始运动
	StepMotor_Start();
	
}



