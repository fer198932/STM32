/********************************************************************************************************
*                                 八角星加工数据 只有一步
********************************************************************************************************/

#include "OffLine_Data.h"

extern		Motion_Strcuture 						motion_Data_Pre;					// 暂时没启用流水线模式
extern 		volatile FunctionalState 		Offline_Work_Flag; 				// 进入脱机加工的标记

// 脱机加工
void offlineWork(u32 num)
{
	offl_Data_Set(num);
	
	/* 加减速 */
	// 运动状态初始化
	AddSubSpeedInit_Pre();
	
	// 步进电机同时开始运动
	StepMotor_Start();
	
	// 加减速定时器开启
	if(ENABLE != Offline_Work_Flag)
	{
		EN_ADDSUB_TIMER;
	}
}

// 运动数据设置
static void offl_Data_Set(u32 num)
{
	// X轴
	if(plusNum_OFFL[num][0] < 0)
	{
		motion_Data_Pre.cmd_Datas.plus_Datas.dir[0] = NEG_DIR;
		motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[0] = (-plusNum_OFFL[num][0]);
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[0] = clk_OFFL[num][0];
	}
	else if(plusNum_OFFL[num][0] > 0)
	{
		motion_Data_Pre.cmd_Datas.plus_Datas.dir[0] = POS_DIR;
		motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[0] = plusNum_OFFL[num][0];
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[0] = clk_OFFL[num][0];
	}
	else
	{
		motion_Data_Pre.cmd_Datas.plus_Datas.dir[0] = TBD_DIR;
		motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[0] = 0;
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[0] = 0;
	}
	
	// Y轴
	if(plusNum_OFFL[num][1] < 0)
	{
		motion_Data_Pre.cmd_Datas.plus_Datas.dir[1] = NEG_DIR;
		motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[1] = (-plusNum_OFFL[num][1]);
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[1] = clk_OFFL[num][1];
	}
	else if(plusNum_OFFL[num][1] > 0)
	{
		motion_Data_Pre.cmd_Datas.plus_Datas.dir[1] = POS_DIR;
		motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[1] = plusNum_OFFL[num][1];
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[1] = clk_OFFL[num][1];
	}
	else
	{
		motion_Data_Pre.cmd_Datas.plus_Datas.dir[1] = TBD_DIR;
		motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[1] = 0;
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[1] = 0;
	}
	
	// Z轴
	if(plusNum_OFFL[num][2] < 0)
	{
		motion_Data_Pre.cmd_Datas.plus_Datas.dir[2] = NEG_DIR;
		motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[2] = (-plusNum_OFFL[num][2]);
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[2] = clk_OFFL[num][2];
	}
	else if(plusNum_OFFL[num][2] > 0)
	{
		motion_Data_Pre.cmd_Datas.plus_Datas.dir[2] = POS_DIR;
		motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[2] = plusNum_OFFL[num][2];
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[2] = clk_OFFL[num][2];
	}
	else
	{
		motion_Data_Pre.cmd_Datas.plus_Datas.dir[2] = TBD_DIR;
		motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[2] = 0;
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[2] = 0;
	}
	
	/* 暂时三轴 */
	// A轴
	motion_Data_Pre.cmd_Datas.plus_Datas.dir[3] = TBD_DIR;
	motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[3] = 0;
	motion_Data_Pre.cmd_Datas.plus_Datas.clk[3] = 0;
	// B轴
	motion_Data_Pre.cmd_Datas.plus_Datas.dir[4] = TBD_DIR;
	motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[4] = 0;
	motion_Data_Pre.cmd_Datas.plus_Datas.clk[4] = 0;
	
/*	
	// A轴
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
	
	// B轴
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

// 暂时三轴
const s32 plusNum_OFFL[LENGTH_OFFL][3] = {
0,0,2000
};

const u32 clk_OFFL[LENGTH_OFFL][3] = {
0,0,5332
};


