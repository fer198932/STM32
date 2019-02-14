/********************************************************************************************************
*   直线型加减速算法 2019年2月10日18:45:08 byYJY
********************************************************************************************************/

#include "AddSubSpeed_L.h"

extern Plus_Data 	cmd_Plus_Data;												// 脉冲数据，控制电机运动
extern GPIO_Structure_XX  	StepMotor_Dir[AXIS_NUM]; 		// 电机方向IO口
// 脉冲数的中断计数
extern volatile u32 pluNumPWM[AXIS_NUM];
/* PWM 初始化的结构体  */
extern GPIO_Structure_XX 		GPIO_PWM_Plus[AXIS_NUM];

u32 subPlusCnt[5] = {0, 0, 0, 0, 0};					// 减速阶段所需的脉冲数
u16 correctSyncValue[5] = {0, 0, 0, 0, 0};		// 修正减速中间频率的减速阶段脉冲值

// u32 savePlusNum[5] = {0, 0, 0, 0, 0};		// 用来保存加减速时 加速阶段走过的脉冲数

static u32 maxClk, minClk;
static u32 nAxisClk_Cur[AXIS_NUM];

static u8 maxClkNum = 5;			// 等于5表示有误，不再修正

// 低速运行的计数器，单位为addtime。 0-4:x、y、z、a、b
static u32 minSpeedTimeCnt[5] = {0, 0, 0, 0, 0};	

// 每一步需要的时间，低频率下需要考虑 0-4:x、y、z、a、b
static u32 clkTimeCntNeed[5];								

// 进入减速阶段的标记，RESET：不进入减速，SET：开始减速
FlagStatus startSubFlag[5] = {RESET, RESET, RESET, RESET, RESET};	

// 是否保存脉冲数的标签，锁定在加速状态 RESET：不保存， SET：保存 0-4：x、y、z、a、b
static FlagStatus savePlusNumFlag[5] = {RESET, RESET, RESET, RESET, RESET};		

// 各轴与TIM的映射关系
extern 	NAxis_TIM_Structure 	nAxis_TIM_Structure[AXIS_NUM];



// 加减速算法的初始化程序 系统运动前调用
void addSubSpeed_L_Init(void)
{
	// 调整频率
	adjustClk();
	
	// 设定方向  有问题，暂时屏蔽 byYJY
//	setStepMotorDir();
	
	// 设定加减速的相关运动参数
	setSubAddSpeedData();
	
	// 各轴初始化
	nAxisInit();
}

// 加减速定时器中断服务程序  考虑改成内联函数或宏定义，提高运行效率  byYJY
void addSubTime_IRQ(void)
{
	u8 i;
	for(i=0; i<AXIS_NUM; i++)
	{
		if((0 != cmd_Plus_Data.clk[i]) && (0 != cmd_Plus_Data.plusNum[i]))
			nAddSubTime_IRQ(i);		
	}
}

// 各轴的加减速-定时器中断服务程序
static void nAddSubTime_IRQ(u8 nAxis)
{	
	// 速度太低不进行加减速
	if(n_Axis_Min_Clk(nAxis) >= cmd_Plus_Data.clk[nAxis])
		return;
	
	// 最低速运行阶段
	if(0 != minSpeedTimeCnt[nAxis])
	{
		minSpeedTimeCnt[nAxis]--;
	}
	// 加速阶段
	else if(SET == savePlusNumFlag[nAxis])
	{
		if(nAxisClk_Cur[nAxis] <= cmd_Plus_Data.clk[nAxis]) 		// 加速阶段
		{
			nAxisClk_Cur[nAxis]++;
			if(clkTimeCntNeed[nAxis])													// 低频率下需要较长的维持时间
			{
				clkTimeCntNeed[nAxis]--;
			}
			else 
			{
				// 如果PSC值修改了，再重新赋值
				if(1 == if_PSC_Modify(nAxis, calPSC(nAxisClk_Cur[nAxis])))
				{
					nAxisSetPWM(nAxis_TIM_Structure[nAxis].TIM_N, calPSC(nAxisClk_Cur[nAxis]));
				}
				clkTimeCntNeed[nAxis] = calPWM_TimeCnt(nAxisClk_Cur[nAxis], TIM_OUT);
			}
		}
		else 
		{
//			savePlusNum[nAxis] = pluNumPWM[nAxis];
			savePlusNumFlag[nAxis] = RESET;										// 关闭标签，锁定在匀、减速阶段
		}
	}
	// 减速阶段
	else if(SET == startSubFlag[nAxis])
	{
		if(nAxisClk_Cur[nAxis] > n_Axis_Min_Clk(nAxis))			// 递减到最小频率，小心递减到”负数”
		{
			nAxisClk_Cur[nAxis]--;
			if(clkTimeCntNeed[nAxis])
			{
				clkTimeCntNeed[nAxis]--;
			} else
			{
				if( 1 == if_PSC_Modify(nAxis, calPSC(nAxisClk_Cur[nAxis])) )
				{
					nAxisSetPWM(nAxis_TIM_Structure[nAxis].TIM_N, calPSC(nAxisClk_Cur[nAxis]));
				}					
				clkTimeCntNeed[nAxis] = calPWM_TimeCnt(nAxisClk_Cur[nAxis], TIM_OUT);
			}
		}
	}
}

// 设定加减速的相关运动参数
static void	setSubAddSpeedData(void)
{
	u8 i;
	
	for(i=0; i<AXIS_NUM; i++)
	{
		// 加速前的低速运行阶段
		calMinSpeedTimeCnt(i);
		// 得到一个完整脉冲后再修改频率
		clkTimeCntNeed[i] = calPWM_TimeCnt(nAxisClk_Cur[i], TIM_OUT);
		// 打开加速阶段的标记
		savePlusNumFlag[i] = SET;
		
//		savePlusNum[i] = 0;
		startSubFlag[i] = RESET;
		// 脉冲数多+1，避免约掉小数（后续是否考虑四舍五入？） byYJY
		subPlusCnt[i] = calSubPlusCnt(i, cmd_Plus_Data.clk[i]) + 1;	
		
		correctSyncValue[i] = correctSync(n_Axis_Min_Clk(i), i);
	}
}

// 各轴初始化
static void nAxisInit(void)
{
	u8 i;
	
	for(i=0; i<AXIS_NUM; i++)
	{
		n_Axis_Init(i);
	}
}
// 指定轴的初始化
static void n_Axis_Init(u8 nAxis)
{
//	TIM_TypeDef* 	TIM_Temp;
	u16 psc;
	
	// 判断是否要加减速
	if(cmd_Plus_Data.clk[nAxis] > n_Axis_Min_Clk(nAxis))
	{
		nAxisClk_Cur[nAxis] = n_Axis_Min_Clk(nAxis);
	}
	else 			// 不加减速，并修正原速度
	{
		// 速度修正 
		correctNoAddSubClk(nAxis, cmd_Plus_Data.plusNum[nAxis]);
		nAxisClk_Cur[nAxis] = cmd_Plus_Data.clk[nAxis];
	}
	
	psc = calPSC(nAxisClk_Cur[nAxis]);
	
//	nAxisSetPWM(nAxis_TIM_Structure[nAxis].TIM_N, psc);  // 第一次会失败
	// PWM初始化 
	Tim_PWM_Init(&nAxis_TIM_Structure[nAxis], 0x04, 0x04, TIM_ARR, psc);
	
}

// 设定方向
static void	setStepMotorDir(void)
{
	u8 i;
	for(i=0; i<AXIS_NUM; i++)
	{
		if(0 == cmd_Plus_Data.dir[i]) 								// 正向
			Motor_Dir_Set(StepMotor_Dir+i, POS_DIR);
		else 																					// 负向
			Motor_Dir_Set(StepMotor_Dir+i, NEG_DIR);
	}
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
		maxClkNum = i;
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

// 计算非最大频率在最低频率下需要运行的时间（计数值）
// 0：无频率， 0：最大频率， 其它值：小于最大速度的中间频率
static void calMinSpeedTimeCnt(u8 nAxis)
{
	if(0 == cmd_Plus_Data.clk[nAxis])
		minSpeedTimeCnt[nAxis] = 0;
	else if(maxClk == cmd_Plus_Data.clk[nAxis])
		minSpeedTimeCnt[nAxis] = 0;
	else
		minSpeedTimeCnt[nAxis] = maxClk - cmd_Plus_Data.clk[nAxis];	
}

// 计算每次发送脉冲所需要的时间_ 计数， time单位为us，所以有1e6
static u16 calPWM_TimeCnt(u16 clk, u16 time)
{
	return (1e6 / (clk * time));
}

// 修改了返回1，否则返回0
static u8 if_PSC_Modify(u8 nAxis, u16 psc)				
{
	switch(nAxis)
	{
		case 0:
			if(X_PWM->PSC == psc)
				return 0;
			else 
				return 1;
		case 1:
			if(Y_PWM->PSC == psc)
				return 0;
			else 
				return 1;
		case 2:
			if(Z_PWM->PSC == psc)
				return 0;
			else 
				return 1;
		case 3:
			if(A_PWM->PSC == psc)
				return 0;
			else 
				return 1;
		case 4:
			if(B_PWM->PSC == psc)
				return 0;
			else 
				return 1;
		default :
			return 0;
	}
}

// 计算减速阶段需要的脉冲数(浮点数)
static u32 calSubPlusCnt(u8 nAxis, u16 nAxisClk)
{
	u16 min_Clk;
	float timeAddSub;
	u16 addSubPlusCnt;
	
	min_Clk = n_Axis_Min_Clk(nAxis);
	timeAddSub = (nAxisClk - min_Clk) * TIM_OUT;		// 单位us
	addSubPlusCnt = (nAxisClk + min_Clk) / 2.0 * timeAddSub * 1e-6;		// 1e-6：因为是us到s
	return addSubPlusCnt;
}

// 修正同步的参数
static u16 correctSync(u32 nAxisMinClk, u8 nAxis)
{
	// 修正同步的参数
	// 修正的方式还需要研究，暂时先写这个 byYJY
	if((AXIS_NUM > maxClkNum) && (nAxisMinClk != 0) && (maxClk != nAxisMinClk))
		return (nAxisMinClk*minSpeedTimeCnt[nAxis]*TIM_OUT/2e6 + 1);					// 加1修正小数
	else
		return 0;									
}

// 修正不加减速的轴的频率
static void correctNoAddSubClk(u8 nAxis, u32 plusNum)
{
	u32 clk;
	float time = calTotalTime(maxClkNum);						// 该时间由最高频率的轴决定
	if(0 == (int)time)
		return;
	
	clk = (int)((float)(1e6*plusNum) / time);
	if(0 == clk)
		return;
	
	if(cmd_Plus_Data.clk[nAxis] > clk)
		cmd_Plus_Data.clk[nAxis] = clk;
}

// 估算一帧需要的时间， 单位us
static float calTotalTime(u8 nAxis)
{
	u32 min_Clk;
	float timeAddSub;
	float plusNum;
	
	min_Clk = n_Axis_Min_Clk(nAxis);
	plusNum = cmd_Plus_Data.plusNum[nAxis];
	
	// 加减速所需时间 us
	timeAddSub = (maxClk - min_Clk) * TIM_OUT;									
	return ((float)((float)1e6*plusNum - (maxClk+min_Clk)*timeAddSub)/
		(float)maxClk + 2 * timeAddSub);
	// 故意让timeAddSub除以2，使得波形同步，原因未知  byYJY
//	return ((float)(1e6*plusNum - (max_Clk+min_Clk)*timeAddSub/2)/(float)max_Clk + 2 * timeAddSub);
}
