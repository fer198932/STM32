/********************************************************************************************************
*                                 �˽��Ǽӹ����� ֻ��һ��
********************************************************************************************************/

#include "OffLine_Data.h"

extern 							Plus_Data		 				cmd_Plus_Data;						// �������ݣ����Ƶ���˶�
extern 	volatile 		FunctionalState 		Offline_Work_Flag; 				// �����ѻ��ӹ��ı��

// �ѻ��ӹ�
void offlineWork(u32 num)
{
	offl_Data_Set(num);
	
	/* �Ӽ��� */
	// �˶�״̬��ʼ��
	AddSubSpeedInit_Pre();
	
	// �������ͬʱ��ʼ�˶�
	StepMotor_Start();
	
	// �Ӽ��ٶ�ʱ������
	if(ENABLE != Offline_Work_Flag)
	{
		EN_ADDSUB_TIMER;
	}
}

// �˶���������
static void offl_Data_Set(u32 num)
{
	// X��
	if(plusNum_OFFL[num][0] < 0)
	{
		cmd_Plus_Data.dir[0] = NEG_DIR;
		cmd_Plus_Data.plusNum[0] = (-plusNum_OFFL[num][0]);
		cmd_Plus_Data.clk[0] = clk_OFFL[num][0];
	}
	else if(plusNum_OFFL[num][0] > 0)
	{
		cmd_Plus_Data.dir[0] = POS_DIR;
		cmd_Plus_Data.plusNum[0] = plusNum_OFFL[num][0];
		cmd_Plus_Data.clk[0] = clk_OFFL[num][0];
	}
	else
	{
		cmd_Plus_Data.dir[0] = TBD_DIR;
		cmd_Plus_Data.plusNum[0] = 0;
		cmd_Plus_Data.clk[0] = 0;
	}
	
	// Y��
	if(plusNum_OFFL[num][1] < 0)
	{
		cmd_Plus_Data.dir[1] = NEG_DIR;
		cmd_Plus_Data.plusNum[1] = (-plusNum_OFFL[num][1]);
		cmd_Plus_Data.clk[1] = clk_OFFL[num][1];
	}
	else if(plusNum_OFFL[num][1] > 0)
	{
		cmd_Plus_Data.dir[1] = POS_DIR;
		cmd_Plus_Data.plusNum[1] = plusNum_OFFL[num][1];
		cmd_Plus_Data.clk[1] = clk_OFFL[num][1];
	}
	else
	{
		cmd_Plus_Data.dir[1] = TBD_DIR;
		cmd_Plus_Data.plusNum[1] = 0;
		cmd_Plus_Data.clk[1] = 0;
	}
	
	// Z��
	if(plusNum_OFFL[num][2] < 0)
	{
		cmd_Plus_Data.dir[2] = NEG_DIR;
		cmd_Plus_Data.plusNum[2] = (-plusNum_OFFL[num][2]);
		cmd_Plus_Data.clk[2] = clk_OFFL[num][2];
	}
	else if(plusNum_OFFL[num][2] > 0)
	{
		cmd_Plus_Data.dir[2] = POS_DIR;
		cmd_Plus_Data.plusNum[2] = plusNum_OFFL[num][2];
		cmd_Plus_Data.clk[2] = clk_OFFL[num][2];
	}
	else
	{
		cmd_Plus_Data.dir[2] = TBD_DIR;
		cmd_Plus_Data.plusNum[2] = 0;
		cmd_Plus_Data.clk[2] = 0;
	}
	
	/* ��ʱ���� */
	// A��
	cmd_Plus_Data.dir[3] = TBD_DIR;
	cmd_Plus_Data.plusNum[3] = 0;
	cmd_Plus_Data.clk[3] = 0;
	// B��
	cmd_Plus_Data.dir[4] = TBD_DIR;
	cmd_Plus_Data.plusNum[4] = 0;
	cmd_Plus_Data.clk[4] = 0;
	
/*	
	// A��
	if(plusNum_OFFL[num][3] < 0)
	{
		cmd_Plus_Data.dir[3] = NEG_DIR;
		cmd_Plus_Data.plusNum[3] = (-plusNum_OFFL[num][3]);
		cmd_Plus_Data.clk[3] = clk_OFFL[num][3];
	}
	else if(plusNum_OFFL[num][3] > 0)
	{
		cmd_Plus_Data.dir[3] = POS_DIR;
		cmd_Plus_Data.plusNum[3] = plusNum_OFFL[num][3];
		cmd_Plus_Data.clk[3] = clk_OFFL[num][3];
	}
	else
	{
		cmd_Plus_Data.dir[3] = TBD_DIR;
		cmd_Plus_Data.plusNum[3] = 0;
		cmd_Plus_Data.clk[3] = 0;
	}
	
	// B��
	if(plusNum_OFFL[num][4] < 0)
	{
		cmd_Plus_Data.dir[4] = NEG_DIR;
		cmd_Plus_Data.plusNum[4] = (-plusNum_OFFL[num][4]);
		cmd_Plus_Data.clk[4] = clk_OFFL[num][4];
	}
	else if(plusNum_OFFL[num][4] > 0)
	{
		cmd_Plus_Data.dir[4] = POS_DIR;
		cmd_Plus_Data.plusNum[4] = plusNum_OFFL[num][4];
		cmd_Plus_Data.clk[4] = clk_OFFL[num][4];
	}
	else
	{
		cmd_Plus_Data.dir[4] = TBD_DIR;
		cmd_Plus_Data.plusNum[4] = 0;
		cmd_Plus_Data.clk[4] = 0;
	}
	*/
}

// ��ʱ����
const s32 plusNum_OFFL[LENGTH_OFFL][3] = {
0,0,2000
};

const u32 clk_OFFL[LENGTH_OFFL][3] = {
0,0,5332
};


