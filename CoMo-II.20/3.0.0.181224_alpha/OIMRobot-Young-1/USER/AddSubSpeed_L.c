/********************************************************************************************************
*   直线型加减速算法 2019年2月10日18:45:08 byYJY
********************************************************************************************************/

#include "AddSubSpeed_L.h"

extern Plus_Data cmd_Plus_Data;			// 脉冲数据，控制电机运动

static u32 maxClk, minClk;

// 加减速算法的初始化程序 系统运动前调用
void addSubSpeed_L_Init(void)
{
	// 调整频率
	adjustClk();
	
	
}

// 频率调整，使得频率位于合适范围内。0：调整ok，1：有轴运行在最低频率以下
static u8 adjustClk(void)
{
	u8 i;
	u8 status = 0;
	maxClk = preProcMaxClk();
	minClk = preProcMinClk(maxClk);
	
	// 最大值判断
	for(i=0; i<AXIS_NUM; i++)
	{
		if(cmd_Plus_Data.clk[i] == maxClk) 				// 最大频率
		{
			if(maxClk > n_Axis_Max_Clk(i)) 					// 需要降频
			{
				reduceClk();
				maxClk = preProcMaxClk();
				minClk = preProcMinClk(maxClk);
				adjustClk();
			} 
			else if(maxClk < n_Axis_Max_Clk(i)) 		// 需要升频
			{
				// nope;
			}
		}
	}
	
	// 最小值判断
	if(0 != minClk)
	{
		for(i=0; i<AXIS_NUM; i++)
		{
			if(cmd_Plus_Data.clk[i] == minClk) 				// 最小频率
			{
				if(minClk < n_Axis_Min_Clk(i))   			// 最小值过小
				{
					if(ENABLE == ifBoostClkOK()) 			// 可以升频
					{
						boostClk();
						maxClk = preProcMaxClk();
						minClk = preProcMinClk(maxClk);
						adjustClk();
					}
					else															// 不可以升频
					{
						status = 1;											// 标记有轴运行在最低频率以下
					}
				}
			}
		}
	}	
	return status;
}


// 返回当前轴允许的最大、最小频率
static u32 n_Axis_Max_Clk(u8 i)
{
	switch(i)
	{
		case 0:
			return X_AXIS_MAX_CLK;
		case 1:
			return Y_AXIS_MAX_CLK;
		case 2:
			return Z_AXIS_MAX_CLK;
		case 3:
			return A_AXIS_MAX_CLK;
		case 4:
			return B_AXIS_MAX_CLK;
		default:
			return 0x0;
	}
}
static u32 n_Axis_Min_Clk(u8 i)
{
	switch(i)
	{
		case 0:
			return X_AXIS_MIN_CLK;
		case 1:
			return Y_AXIS_MIN_CLK;
		case 2:
			return Z_AXIS_MIN_CLK;
		case 3:
			return A_AXIS_MIN_CLK;
		case 4:
			return B_AXIS_MIN_CLK;
		default:
			return 0x0;
	}
}

// 获得最大频率
static u32 preProcMaxClk(void)
{
	volatile u8 i;
	u32 max_Clk = cmd_Plus_Data.clk[0];
	for(i=1; i<AXIS_NUM; i++)
	{
		max_Clk = max(max_Clk, cmd_Plus_Data.clk[i]);
	}
	return max_Clk;
}

// 获得非零的最小频率 先求最大值再调用
static u32 preProcMinClk(u32 max_Clk)
{
	volatile u8 i;
	u32 min_Clk = 0;
	
	for(i=0; i<AXIS_NUM; i++)
	{
		if((0 == min_Clk) && (max_Clk != cmd_Plus_Data.clk[i]))
		{
			min_Clk = cmd_Plus_Data.clk[i];			
		}
		else if(0 != cmd_Plus_Data.clk[i])
		{
			min_Clk = min(min_Clk, cmd_Plus_Data.clk[i]);
		}
	}
	return min_Clk;
}

// 降频，有精度丢失
static void reduceClk(void)
{
	u8 i;
	for(i=0; i<AXIS_NUM; i++)
	{
		cmd_Plus_Data.clk[i] = (cmd_Plus_Data.clk[i] >> 1);
	}
}
// 升频，速度太慢时使用，优先级低
static void boostClk(void)
{
	u8 i;
	for(i=0; i<AXIS_NUM; i++)
	{
		cmd_Plus_Data.clk[i] = (cmd_Plus_Data.clk[i] << 1);
	}
}

// 是否可以升频的判断 
static FunctionalState ifBoostClkOK(void)
{
	u8 i;
	
	for(i=0; i<AXIS_NUM; i++)
	{
		if((cmd_Plus_Data.clk[i] << 1) > n_Axis_Max_Clk(i)) // 升频后大于最大频率，不允许升频
		{
			return DISABLE;
		}
	}	
	
	return ENABLE;
}




