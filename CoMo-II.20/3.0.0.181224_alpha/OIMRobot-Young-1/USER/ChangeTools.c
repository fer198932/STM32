/********************************************************************************************************
* ��������
* 2019��3��8��09:27:59 byYJY
********************************************************************************************************/
#include "ChangeTools.h"


extern 		Flag_Structure 					flag_Struct;
extern		Motion_Strcuture 				motion_Data_Pre;	
extern		DesPosition 						workPos;								// ����ǰ�Ĺ���λ��



// ����λ��
static 		ToolPos								toolPos[TOOLS_QTY];
static 		GPIO_Structure_XX			tools_Electromagnet;								// ���ߵ������IO��



/* ��ʱ��ŵ�����Ϣ����������λ�����ص���λ����Ϣ */
static		ToolPos		toolPos_Temp = {
	1, 										// ����
	1000, 1000, 1000			// ����λ��
};



// ���߳�ʼ����ȷ�����߱�š�����λ�õ���Ϣ
void Tool_Init(void)
{
	mymemset(toolPos, 0, sizeof(toolPos));
	
	tools_IO_Init();
	
	getToolPosFromPC(toolPos);	
}

// �������� mode��0-�ѵ���1-װ��
void changeTools(u8 toolNo, ChangeTool_Mode mode)
{
	if(Clean_Tool == mode)				// �ѵ�
	{
		cleanTool(flag_Struct.ToolsNum_Flag);
		// �ص�ԭ�ӹ�λ��
		zeroBack2Pos(&workPos);
	}
	else if(Assmble_Tool == mode)
	{
		assemTool(flag_Struct.ToolsNum_Flag);
		// �ص�ԭ�ӹ�λ��
		zeroBack2Pos(&workPos);
	}
}

// У������λ�ã�Ĭ��Ϊ����1λ�á��ƶ�������λ�ú�������иó���
void calibrateToolPos(void)
{
	// ����
	return2Zero();
	
	// ��ʱ�ȴ�ӡ����
	printf("X:%d, Y:%d, Z:%d \r\n", workPos.posPlusNum[0], workPos.posPlusNum[1], workPos.posPlusNum[2]);	
	
	// Ҳ���Ը�ֵ��toolPos_Temp
}


// �ѵ�����
static ErrorStatus cleanTool(u8 toolNo)
{	
	if((0 == toolNo) || (toolNo >= TOOLS_QTY))
	{
		respMsgError("û�е��߻򵶾ߴ��󣬲����ѵ���\r\n", 1);		
		return ERROR;
	}
	
	// �˶�������λ�ã��Ͽ�10mm��
	move2ToolPos(toolNo);
	
	// ���ߵ�����ɿ�
	tools_ElecMag_Ctrl(UN_Clamp);
	
	// Z�Ỻ�����е�����λ��
	StepMotor_Move(Z_Axis, Z_AXIS_MIN_CLK, DISTANCE_Z*SUBDIV_NUM, NEG_DIR);
	StepMotor_Move_Done();
	delay_ms(100);
	
	// ���ߵ��������
	tools_ElecMag_Ctrl(Clamp);
	
	// Z�Ỻ�������ѵ�
	StepMotor_Move(Z_Axis, Z_AXIS_MIN_CLK, DISTANCE_Z*SUBDIV_NUM, POS_DIR);
	StepMotor_Move_Done();
	delay_ms(100);
	
	// �ѵ�֮����� byYJY
	return2Zero();
	
	flag_Struct.ToolsNum_Flag = 0;
	
	return SUCCESS;
}

// װ������
static ErrorStatus assemTool(u8 toolNo)
{
	if(toolNo >= TOOLS_QTY)
	{
		respMsgError("װ����������\r\n", 1);
		return ERROR;
	}
	if(flag_Struct.ToolsNum_Flag == toolNo)
	{
		respMsgError("��ǰ���߲��ø���\r\n", 1);
		return SUCCESS;
	}
	
	// ��Ҫ���ѵ�
	if(flag_Struct.ToolsNum_Flag != 0)
	{
		cleanTool(toolNo);
	}
	else
	{	
		// ����
		return2Zero();
	}
	
	// �˶�������λ���Ϸ�
	move2ToolPos(toolNo);
	
	// ���ߵ�����ɿ�
	tools_ElecMag_Ctrl(UN_Clamp);
	
	// ��������ת
	mainMotor_WorkSlowly();
	
	// Z�Ỻ������װ��
	StepMotor_Move(Z_Axis, Z_AXIS_MIN_CLK, DISTANCE_Z*SUBDIV_NUM, NEG_DIR);
	StepMotor_Move_Done();
	delay_ms(100);
	
	// Z�Ỻ������
	StepMotor_Move(Z_Axis, Z_AXIS_MIN_CLK, DISTANCE_Z*SUBDIV_NUM, POS_DIR);
	StepMotor_Move_Done();
	delay_ms(100);
	
	// ���ߵ�����н�
	tools_ElecMag_Ctrl(Clamp);
	
	// װ��֮�����	
	return2Zero();
	
	flag_Struct.ToolsNum_Flag = toolNo;
	
	return SUCCESS;
}

// ����λ����õ���λ����Ϣ
static void getToolPosFromPC(ToolPos tool_pos[])
{
/*	// ������Ϣѯ����λ������λ����Ϣ
	respUsartMsg("tool", 5);
	delay_ms(1);					// �ȴ���λ������
	while(SET != flag_Struct.USART_IDLE_Flag) ;		// �ȴ���λ������
	flag_Struct.USART_IDLE_Flag = RESET;	
	UsartDataProc();			// �������ݴ���
	motion_Data_Pre				// �������������� */
	
	// ����1
	tool_pos[1].tool_Num = 1;
	tool_pos[1].tool_Pos[0] = toolPos_Temp.tool_Pos[0];
	tool_pos[1].tool_Pos[1] = toolPos_Temp.tool_Pos[1];
	tool_pos[1].tool_Pos[2] = toolPos_Temp.tool_Pos[2];
	
	// ����2
	tool_pos[2].tool_Num = 2;
	tool_pos[2].tool_Pos[0] = toolPos_Temp.tool_Pos[0] + TOOL_OFFSET_X*SUBDIV_NUM;
	tool_pos[2].tool_Pos[1] = toolPos_Temp.tool_Pos[1] + TOOL_OFFSET_X*SUBDIV_NUM;
	tool_pos[2].tool_Pos[2] = toolPos_Temp.tool_Pos[2];
	
	// ����3
	tool_pos[1].tool_Num = 3;
	// .............
	
	// ����4
	tool_pos[1].tool_Num = 4;
	// .............
	
	flag_Struct.ToolsNum_Flag = toolPos_Temp.tool_Num;
}


// �˶�������λ��
static void move2ToolPos(u8 toolNo)
{
	DesPosition toolPostion;
	
	mymemset(&toolPostion, 0, sizeof(toolPostion));
	
	// ����
	return2Zero();
	
	// ���õ���λ��
	toolPostion.posDir[X_Axis] = POS_DIR;
	toolPostion.posPlusNum[X_Axis] = toolPos[toolNo].tool_Pos[X_Axis];
	toolPostion.posDir[Y_Axis] = NEG_DIR;
	toolPostion.posPlusNum[Y_Axis] = toolPos[toolNo].tool_Pos[Y_Axis];
	toolPostion.posDir[Z_Axis] = NEG_DIR;
	toolPostion.posPlusNum[Z_Axis] = toolPos[toolNo].tool_Pos[Z_Axis] - DISTANCE_Z*SUBDIV_NUM;
	
	// �˶�������λ��
	zeroBack2Pos(&toolPostion);	
}

// ���ߵ����IO�ڳ�ʼ��
static void tools_IO_Init(void)
{
	limitInit(&tools_Electromagnet, TOOLS_ELECTROMAGNET);
	tools_ElecMag_Ctrl(UN_Clamp);
}

// ���ߵ��������
static void tools_ElecMag_Ctrl(Clamp_Mode mode)
{
	if(Clamp == mode)								// ������н�
	{
		GPIO_ResetBits(tools_Electromagnet.GPIO_Port, tools_Electromagnet.GPIO_Pin_N);
		delay_ms(1);
	}
	else if(UN_Clamp == mode)				//  ������ɿ���ʱ�䲻�ܳ���10s
	{
		GPIO_SetBits(tools_Electromagnet.GPIO_Port, tools_Electromagnet.GPIO_Pin_N);
		delay_ms(1);
	}
}














