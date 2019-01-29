/********************************************************************************************************
* 执行命令的处理程序
********************************************************************************************************/
#include "command.h"

extern Proc_Data proc_Data; 		// 命令数据，有成员指向plus_Data
extern Plus_Data plus_Data;			// 脉冲数据，控制电机运动

// 自检程序
void selfCheckFunc(void)
{
	const u8 length = 8;
	u8 backResString[length];
	proc_Data.resp_Status = 0x01;
	// 	temp = UrgentStopTest();		//急停开关检测 0:按下；1:松开；2:抖动；
	
	setRespStr(backResString, length);
	
	// 其余位的字符数据设置
	backResString[6] = proc_Data.resp_Status;
	
	// 串口发回
#if PRIN2DISP
	respUsartMsg("SELCK\r\n", length);
#else
	respUsartMsg(backResString, length);
#endif
}



