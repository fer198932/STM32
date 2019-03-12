/********************************************************************************************************
* 换刀程序
* 2019年3月8日09:27:59 byYJY
********************************************************************************************************/
#include "ChangeTools.h"


extern 		Flag_Structure 					flag_Struct;
extern		Motion_Strcuture 				motion_Data_Pre;	
extern		DesPosition 						workPos;								// 回零前的工作位置



// 刀具位置
static 		ToolPos								toolPos[TOOLS_QTY];
static 		GPIO_Structure_XX			tools_Electromagnet;								// 刀具电磁铁的IO口



/* 临时存放刀具信息，后续由上位机发回刀具位置信息 */
static		ToolPos		toolPos_Temp = {
	1, 										// 刀具
	1000, 1000, 1000			// 刀具位置
};



// 刀具初始化，确定刀具编号、刀具位置等信息
void Tool_Init(void)
{
	mymemset(toolPos, 0, sizeof(toolPos));
	
	tools_IO_Init();
	
	getToolPosFromPC(toolPos);	
}

// 换刀程序， mode：0-脱刀，1-装刀
void changeTools(u8 toolNo, ChangeTool_Mode mode)
{
	if(Clean_Tool == mode)				// 脱刀
	{
		cleanTool(flag_Struct.ToolsNum_Flag);
		// 回到原加工位置
		zeroBack2Pos(&workPos);
	}
	else if(Assmble_Tool == mode)
	{
		assemTool(flag_Struct.ToolsNum_Flag);
		// 回到原加工位置
		zeroBack2Pos(&workPos);
	}
}

// 校正刀具位置，默认为刀具1位置。移动到刀具位置后才能运行该程序
void calibrateToolPos(void)
{
	// 回零
	return2Zero();
	
	// 暂时先打印出来
	printf("X:%d, Y:%d, Z:%d \r\n", workPos.posPlusNum[0], workPos.posPlusNum[1], workPos.posPlusNum[2]);	
	
	// 也可以赋值给toolPos_Temp
}


// 脱刀程序
static ErrorStatus cleanTool(u8 toolNo)
{	
	if((0 == toolNo) || (toolNo >= TOOLS_QTY))
	{
		respMsgError("没有刀具或刀具错误，不能脱刀！\r\n", 1);		
		return ERROR;
	}
	
	// 运动到刀具位置（上空10mm）
	move2ToolPos(toolNo);
	
	// 刀具电磁铁松开
	tools_ElecMag_Ctrl(UN_Clamp);
	
	// Z轴缓慢下行到刀具位置
	StepMotor_Move(Z_Axis, Z_AXIS_MIN_CLK, DISTANCE_Z*SUBDIV_NUM, NEG_DIR);
	StepMotor_Move_Done();
	delay_ms(100);
	
	// 刀具电磁铁锁紧
	tools_ElecMag_Ctrl(Clamp);
	
	// Z轴缓慢往上脱刀
	StepMotor_Move(Z_Axis, Z_AXIS_MIN_CLK, DISTANCE_Z*SUBDIV_NUM, POS_DIR);
	StepMotor_Move_Done();
	delay_ms(100);
	
	// 脱刀之后回零 byYJY
	return2Zero();
	
	flag_Struct.ToolsNum_Flag = 0;
	
	return SUCCESS;
}

// 装刀程序
static ErrorStatus assemTool(u8 toolNo)
{
	if(toolNo >= TOOLS_QTY)
	{
		respMsgError("装刀刀具有误\r\n", 1);
		return ERROR;
	}
	if(flag_Struct.ToolsNum_Flag == toolNo)
	{
		respMsgError("当前刀具不用更换\r\n", 1);
		return SUCCESS;
	}
	
	// 需要先脱刀
	if(flag_Struct.ToolsNum_Flag != 0)
	{
		cleanTool(toolNo);
	}
	else
	{	
		// 回零
		return2Zero();
	}
	
	// 运动到刀具位置上方
	move2ToolPos(toolNo);
	
	// 刀具电磁铁松开
	tools_ElecMag_Ctrl(UN_Clamp);
	
	// 主轴电机缓转
	mainMotor_WorkSlowly();
	
	// Z轴缓慢往下装刀
	StepMotor_Move(Z_Axis, Z_AXIS_MIN_CLK, DISTANCE_Z*SUBDIV_NUM, NEG_DIR);
	StepMotor_Move_Done();
	delay_ms(100);
	
	// Z轴缓慢往上
	StepMotor_Move(Z_Axis, Z_AXIS_MIN_CLK, DISTANCE_Z*SUBDIV_NUM, POS_DIR);
	StepMotor_Move_Done();
	delay_ms(100);
	
	// 刀具电磁铁夹紧
	tools_ElecMag_Ctrl(Clamp);
	
	// 装刀之后回零	
	return2Zero();
	
	flag_Struct.ToolsNum_Flag = toolNo;
	
	return SUCCESS;
}

// 从上位机获得刀具位置信息
static void getToolPosFromPC(ToolPos tool_pos[])
{
/*	// 发送信息询问上位机刀具位置信息
	respUsartMsg("tool", 5);
	delay_ms(1);					// 等待上位机返回
	while(SET != flag_Struct.USART_IDLE_Flag) ;		// 等待上位机返回
	flag_Struct.USART_IDLE_Flag = RESET;	
	UsartDataProc();			// 串口数据处理
	motion_Data_Pre				// 从这里面获得数据 */
	
	// 刀具1
	tool_pos[1].tool_Num = 1;
	tool_pos[1].tool_Pos[0] = toolPos_Temp.tool_Pos[0];
	tool_pos[1].tool_Pos[1] = toolPos_Temp.tool_Pos[1];
	tool_pos[1].tool_Pos[2] = toolPos_Temp.tool_Pos[2];
	
	// 刀具2
	tool_pos[2].tool_Num = 2;
	tool_pos[2].tool_Pos[0] = toolPos_Temp.tool_Pos[0] + TOOL_OFFSET_X*SUBDIV_NUM;
	tool_pos[2].tool_Pos[1] = toolPos_Temp.tool_Pos[1] + TOOL_OFFSET_X*SUBDIV_NUM;
	tool_pos[2].tool_Pos[2] = toolPos_Temp.tool_Pos[2];
	
	// 刀具3
	tool_pos[1].tool_Num = 3;
	// .............
	
	// 刀具4
	tool_pos[1].tool_Num = 4;
	// .............
	
	flag_Struct.ToolsNum_Flag = toolPos_Temp.tool_Num;
}


// 运动到刀具位置
static void move2ToolPos(u8 toolNo)
{
	DesPosition toolPostion;
	
	mymemset(&toolPostion, 0, sizeof(toolPostion));
	
	// 回零
	return2Zero();
	
	// 设置刀具位置
	toolPostion.posDir[X_Axis] = POS_DIR;
	toolPostion.posPlusNum[X_Axis] = toolPos[toolNo].tool_Pos[X_Axis];
	toolPostion.posDir[Y_Axis] = NEG_DIR;
	toolPostion.posPlusNum[Y_Axis] = toolPos[toolNo].tool_Pos[Y_Axis];
	toolPostion.posDir[Z_Axis] = NEG_DIR;
	toolPostion.posPlusNum[Z_Axis] = toolPos[toolNo].tool_Pos[Z_Axis] - DISTANCE_Z*SUBDIV_NUM;
	
	// 运动到刀具位置
	zeroBack2Pos(&toolPostion);	
}

// 刀具电磁铁IO口初始化
static void tools_IO_Init(void)
{
	limitInit(&tools_Electromagnet, TOOLS_ELECTROMAGNET);
	tools_ElecMag_Ctrl(UN_Clamp);
}

// 刀具电磁铁控制
static void tools_ElecMag_Ctrl(Clamp_Mode mode)
{
	if(Clamp == mode)								// 电磁铁夹紧
	{
		GPIO_ResetBits(tools_Electromagnet.GPIO_Port, tools_Electromagnet.GPIO_Pin_N);
		delay_ms(1);
	}
	else if(UN_Clamp == mode)				//  电磁铁松开，时间不能超过10s
	{
		GPIO_SetBits(tools_Electromagnet.GPIO_Port, tools_Electromagnet.GPIO_Pin_N);
		delay_ms(1);
	}
}














