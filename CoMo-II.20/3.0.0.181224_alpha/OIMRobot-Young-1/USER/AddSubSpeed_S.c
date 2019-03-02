/********************************************************************************************************
*    S型加减速算法 2019年2月13日09:02:11 byYJY
*    ASS : AddSubSpeed 加减速缩写
********************************************************************************************************/

#include "AddSubSpeed_S.h"


//extern 											Plus_Data cmd_Plus_Data;						// 脉冲数据，控制电机运动
//extern 											Proc_Data cmd_Proc_Data; 						// 命令数据，有成员指向plus_Data
extern FunctionalState	 		nAxisStatus[AXIS_NUM];  						// 各轴是否可以运动的标志
extern NAxis_TIM_Structure	nAxis_TIM_Structure[AXIS_NUM];  		// PWM 初始化的结构体  
extern vu32 								plusNumPWM[AXIS_NUM]; 								// 脉冲数的中断计数
extern vu16 								addSubSpeed_StepNum[AXIS_NUM]; 			// 记录加减速定时器运行的步数



// 运动参数
Motion_Strcuture 	motion_Data;	
Motion_Strcuture 	motion_Data_Pre;	


// 运动数据的初始化
void motion_Init(void)
{
	mymemcpy(&motion_Data, &motion_Data_Pre, sizeof(motion_Data));
	
	// 标志位
	
	
	
	// 设定运动方向
	setStepMotorDir();
	
	// 各轴的运动状态初始化
	nAxisMotion_Init();
}


// 加减速相关参数设置的初始化
void AddSubSpeedInit_Pre(void)
{
	// 重置相关运动参数
	resetMotionData();
	
	// 调整频率到合适范围
	adjustClk();
	
	// 计算加减速如何分段
	cal_AddSubSpeed_Div();
	
	// 计算S型曲线的PSC数据
	cal_S_Line();	
}

// 各轴的运动状态初始化
static void	nAxisMotion_Init(void)
{
	u8 i;
	
	// 各轴状态初始化 允许运动
	for(i=0; i<AXIS_NUM; i++)
	{
		plusNumPWM[i] = 0;
		addSubSpeed_StepNum[i] = ADD_SPEED;
		nAxisStatus[i] = ENABLE;
		
#if NO_ADDSUBSPEED
//		nAxisClk_Cur[i] = cmd_Plus_Data.clk[i];
		nAxisClk_Cur[i] = n_Axis_Min_Clk(i);
#else
//		nAxisClk_Cur[i] = () ? : ;
#endif
		nAxisSetPWM(nAxis_TIM_Structure[i].TIM_N, calPSC(motion_Data.nAxisClk_Cur[i], i));	
		
		// 主动发起事件，更新寄存器
		TIM_GenerateEvent(nAxis_TIM_Structure[i].TIM_N, TIM_EventSource_Update);
	}
	delay_ms(1); 		// 一定延迟，使得PSC值设置好
}

// 重置相关运动参数
static void resetMotionData(void)
{	
	//comData_Init();		// 在该函数已经初始化，不能再次初始化
	
	motion_Data_Pre.maxClkNum = AXIS_NUM;
	motion_Data_Pre.addSubTime = TIM_OUT;
}

// 设定运动方向
static void	setStepMotorDir(void)
{
	u8 i;
	for(i=0; i<AXIS_NUM; i++)
	{
		Motor_Dir_Set(StepMotor_Dir+i, motion_Data.cmd_Datas.plus_Datas.dir[i]);
	}
}


// 计算加减速如何分段
static void cal_AddSubSpeed_Div(void)
{
	u8 i;
	float maxClk_length; 			// 加减速步数
	u8 maxClkNum = motion_Data_Pre.maxClkNum;
//	float correct_Value;			// 修正clk的比例
	
	// 最大频率所在轴作为分段依据 这里可能有BUG  byYJY
	maxClk_length = (motion_Data_Pre.cmd_Datas.plus_Datas.clk[maxClkNum] - n_Axis_Min_Clk(maxClkNum)) / AddSub_Step_DIV;
	
	if((AXIS_NUM > maxClkNum) && (maxClk_length < MIN_STEP_NUM))		// maxClk_length太小或为负时不加减速
	{
		// 所有轴不使用加减速运动
		En_ASS_Flag(DISABLE, ALL_Axis);
		return;
	}
	
	// test 脉冲数小于500的都不加减速
//	if(cmd_Plus_Data.plusNum[maxClkNum] < 500)
//	{
//		correct_Value = cmd_Plus_Data.clk[maxClkNum] / n_Axis_Min_Clk(maxClkNum);
//		for(i=0; i<AXIS_NUM; i++)
//		{
//			cmd_Plus_Data.clk[i] = cmd_Plus_Data.clk[i] / correct_Value;

//			if(50 > cmd_Plus_Data.clk[i])
//			{
//				cmd_Plus_Data.clk[i] = n_Axis_Min_Clk(i);
//			}
//			
//			Psc_Data_Cur[i].enAddSubFlag = DISABLE;
//		}
//		return;
//	}
	
	// 设置所有轴的参数
	for(i=0; i<AXIS_NUM; i++)
	{
		// 速度太小时，不进行加减速
		if(motion_Data_Pre.cmd_Datas.plus_Datas.clk[i] < n_Axis_Min_Clk(i))
		{
			En_ASS_Flag(DISABLE, (N_Axis)i);
			motion_Data_Pre.PSC_Data[i].length = 0;									
			continue;
		} 
		else
		{
			En_ASS_Flag(ENABLE, (N_Axis)i);
			motion_Data_Pre.PSC_Data[i].length = maxClk_length;			// 同样的加减速步数
		}
	}
}



// 频率调整，使得频率位于合适范围内。0：调整ok，1：有轴运行在最低频率以下
static u8 adjustClk(void)
{
	u8 i;
	u8 status = 0;
	motion_Data_Pre.maxClk = preProcMaxClk();
	motion_Data_Pre.minClk = preProcMinClk(motion_Data_Pre.maxClk);
	
	// 最大值判断
	for(i=0; i<AXIS_NUM; i++)
	{
		if(motion_Data_Pre.cmd_Datas.plus_Datas.clk[i] == motion_Data_Pre.maxClk) 				// 最大频率
		{
			if(motion_Data_Pre.maxClk > n_Axis_Max_Clk(i)) 					// 需要降频
			{
				reduceClk();
				motion_Data_Pre.maxClk = preProcMaxClk();
				motion_Data_Pre.minClk = preProcMinClk(motion_Data_Pre.maxClk);
				adjustClk();
			} 
			else if(motion_Data_Pre.maxClk < n_Axis_Max_Clk(i)) 		// 需要升频
			{
				// nope;
			}
		}
	}
	
	// 最小值判断
	if(0 != motion_Data_Pre.minClk)
	{
		for(i=0; i<AXIS_NUM; i++)
		{
			if(motion_Data_Pre.cmd_Datas.plus_Datas.clk[i] == motion_Data_Pre.minClk) 				// 最小频率
			{
				if(motion_Data_Pre.minClk < n_Axis_Min_Clk(i))   			// 最小值过小
				{
					if(ENABLE == ifBoostClkOK()) 			// 可以升频
					{
						boostClk();
						motion_Data_Pre.maxClk = preProcMaxClk();
						motion_Data_Pre.minClk = preProcMinClk(motion_Data_Pre.maxClk);
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

// 各轴计算S型加速曲线
static void cal_S_Line(void)
{
	u8 i;
//	u16 minStepNum_Length; 			// 最小的步进数	
	float needPlusNum, needTotalTime;
	float freq_Temp;
	u8 maxClkNum = motion_Data_Pre.maxClkNum;
	
	/* 以最大频率所在的轴作为S曲线的设定基准  */
	// 最大频率轴的加减速关闭
	if(DISABLE == motion_Data_Pre.PSC_Data[maxClkNum].enAddSubFlag)
	{
		for(i=0; i<AXIS_NUM; i++)
		{
			motion_Data_Pre.nAxisClk_Cur[i] = motion_Data_Pre.cmd_Datas.plus_Datas.clk[i];
		}
		
		return;
	}
	// 最大频率轴的S型曲线计算错误
	if(DISABLE == calSModelLine(freq_Temp, motion_Data_Pre.PSC_Data[maxClkNum].psc_data, motion_Data_Pre.PSC_Data[maxClkNum].length,
				motion_Data_Pre.cmd_Datas.plus_Datas.clk[maxClkNum], n_Axis_Min_Clk(maxClkNum), S_FLEXIBLE))
	{
		En_ASS_Flag(DISABLE ,ALL_Axis);
		
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[maxClkNum] = n_Axis_Min_Clk(maxClkNum);			// 将最大轴的频率降低
		
		needTotalTime = calTotalNeedTime(maxClkNum);		
		
		// 调整所有轴的频率
		for(i=0; i<AXIS_NUM; i++)
		{
			if(i == maxClkNum)
			{
				continue;
			}
			
			CorrectClk(i, needTotalTime);
		}
		return;
	}
	
	// 将加速需要的脉冲数保存到结构体中，作为判断减速阶段的条件
	needPlusNum = calAddSpeed_NeedPlusNum(motion_Data_Pre.PSC_Data[maxClkNum].psc_data, motion_Data_Pre.PSC_Data[maxClkNum].length);		
	motion_Data_Pre.PSC_Data[maxClkNum].addSpeed_NeedPlusNum = needPlusNum;
	
	// 最大频率轴的脉冲数不够进行一次完整的的加减速，那就将加减速关闭
	if((2 * needPlusNum + ConstS_NeedPlus) > motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[maxClkNum])
	{
		En_ASS_Flag(DISABLE ,ALL_Axis);
		
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[maxClkNum] = n_Axis_Min_Clk(maxClkNum);			// 将最大轴的频率降低
		
		needTotalTime = calTotalNeedTime(maxClkNum);		
		
		// 调整所有轴的频率
		for(i=0; i<AXIS_NUM; i++)
		{
			if(i == maxClkNum)
			{
				continue;
			}
			
			CorrectClk(i, needTotalTime);
		}
		
		return;
	}	
	
	// 计算最大轴需要的运行时间，单位us
	needTotalTime = calTotalNeedTime(maxClkNum);
	
	for(i=0; i<AXIS_NUM; i++)
	{
		if(maxClkNum == i) 		// 最大轴
		{
			motion_Data_Pre.nAxisClk_Cur[i] = n_Axis_Min_Clk(i);
			continue;
		}
		
		if(ENABLE == motion_Data_Pre.PSC_Data[i].enAddSubFlag)
		{
			if(DISABLE == calSModelLine(freq_Temp, motion_Data_Pre.PSC_Data[i].psc_data, motion_Data_Pre.PSC_Data[i].length,
				motion_Data_Pre.cmd_Datas.plus_Datas.clk[i], n_Axis_Min_Clk(i), S_FLEXIBLE))
			{
				En_ASS_Flag(DISABLE, (N_Axis)i);
				
				CorrectClk(i, needTotalTime);
				
				continue;
			}
			
			// 将加速需要的脉冲数保存到结构体中，作为判断减速阶段的条件
			needPlusNum = calAddSpeed_NeedPlusNum(motion_Data_Pre.PSC_Data[i].psc_data, motion_Data_Pre.PSC_Data[i].length);			
			motion_Data_Pre.PSC_Data[i].addSpeed_NeedPlusNum = needPlusNum;
			
			// 如果总的脉冲数不够进行一次完整加减速，那就将加减速关闭
			if((2 * needPlusNum + ConstS_NeedPlus) > motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[i])
			{
				En_ASS_Flag(DISABLE, (N_Axis)i);
				
				CorrectClk(i, needTotalTime);				
			}
		}
		else if(DISABLE == motion_Data_Pre.PSC_Data[i].enAddSubFlag) 			// 不加减速
		{
			//  根据最大频率花费的时间修正频率
			CorrectClk(i, needTotalTime);
		}
	}
}

/* 	计算S型加速曲线的函数 */
static FunctionalState calSModelLine(float fre, u16 period[], float len, 
	float fre_max, float fre_min, float flexible)
{
	int i;
	float deno;
	float melo;
	float delt = fre_max - fre_min;
	
	// byYJY ///////////////////////////////
//	mymemset(period, 0, DATA_LENGTH * sizeof(u16));
	
	// 判断参数是否合格
	if((len < 0) || (fre_max < fre_min))
		return DISABLE;
	
	for(i=0; i<len; i++)
	{
		melo = flexible * (i-len/2) / (len/2);
		deno = (float)(1.0 / (1.0 + (double)expf(-melo))); 
		fre = delt * deno + fre_min;
		period[i] = (u16)(PSC_CLK / fre);
	}
	i=i;
	return ENABLE;
}

// 获得最大频率
static u32 preProcMaxClk(void)
{
	volatile u8 i;
	u32 max_Clk = motion_Data_Pre.cmd_Datas.plus_Datas.clk[0];
	for(i=1; i<AXIS_NUM; i++)
	{
		max_Clk = max(max_Clk, motion_Data_Pre.cmd_Datas.plus_Datas.clk[i]);
	}
	// 找到最大频率对应的轴
	for(i=0; i<AXIS_NUM; i++)
	{
		if(max_Clk == motion_Data_Pre.cmd_Datas.plus_Datas.clk[i])
			motion_Data_Pre.maxClkNum = i;
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
		if((0 == min_Clk) && (max_Clk != motion_Data_Pre.cmd_Datas.plus_Datas.clk[i]))
		{
			min_Clk = motion_Data_Pre.cmd_Datas.plus_Datas.clk[i];			
		}
		else if(0 != motion_Data_Pre.cmd_Datas.plus_Datas.clk[i])
		{
			min_Clk = min(min_Clk, motion_Data_Pre.cmd_Datas.plus_Datas.clk[i]);
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
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[i] = (motion_Data_Pre.cmd_Datas.plus_Datas.clk[i] >> 1);
	}
}
// 升频，速度太慢时使用，优先级低
static void boostClk(void)
{
	u8 i;
	for(i=0; i<AXIS_NUM; i++)
	{
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[i] = (motion_Data_Pre.cmd_Datas.plus_Datas.clk[i] << 1);
	}
}

// 返回当前轴允许的最大、最小频率
u32 n_Axis_Max_Clk(u8 i)
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
u32 n_Axis_Min_Clk(u8 i)
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

// 是否可以升频的判断 
static FunctionalState ifBoostClkOK(void)
{
	u8 i;
	
	for(i=0; i<AXIS_NUM; i++)
	{
		if((motion_Data_Pre.cmd_Datas.plus_Datas.clk[i] << 1) > n_Axis_Max_Clk(i)) // 升频后大于最大频率，不允许升频
		{
			return DISABLE;
		}
	}	
	
	return ENABLE;
}

// 计算当前曲线下，完成加速所需要的脉冲数  
// 关注文章： https://blog.csdn.net/gtkknd/article/details/52199963 byYJY
static float calAddSpeed_NeedPlusNum(u16 period[], float len)
{
	u16 i;
	u32 clk;
	float needPlusNum = 0.0;
	u16 length = (int)len;	
	
	for(i=0; i<length; i++)
	{
		clk = calClk_PSC(period[i]);
		needPlusNum += motion_Data_Pre.addSubTime * clk;			// 注意：addSubTime 单位为us
	}
	needPlusNum *= (float)1e-6;								
	
	return needPlusNum;
}

// 估算指定轴运行所需要的时间 单位us 确定加速曲线后再调用
static float calTotalNeedTime(u8 nAxis)
{	
	u32 i;
	u32 maxStep;
	float constSpeedTime; 		// 匀速阶段需要的时间
	float plus_Num;						// 发出的脉冲数
	
	if(DISABLE == motion_Data_Pre.PSC_Data[nAxis].enAddSubFlag) 		// 未开启加减速
		return (motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[nAxis] * MHz_2_Hz / motion_Data_Pre.cmd_Datas.plus_Datas.clk[nAxis]);
	
	
	maxStep = (int)(motion_Data_Pre.PSC_Data[nAxis].length - 1);
	// 加速阶段完成运动
	if(motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[nAxis] < motion_Data_Pre.PSC_Data[nAxis].addSpeed_NeedPlusNum)
	{
		i = 0;
		
		plus_Num = 0;
		while(plus_Num < motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[nAxis])
		{
			if(i >=  motion_Data_Pre.PSC_Data[nAxis].length) 		// i值有误
			{
				plus_Num += calClk_PSC(motion_Data_Pre.PSC_Data[nAxis].psc_data[maxStep]) * motion_Data_Pre.addSubTime / MHz_2_Hz;
				respMsgError("计算运行时间异常!\r\n", 1);
			}
			else 
			{
				plus_Num += calClk_PSC(motion_Data_Pre.PSC_Data[nAxis].psc_data[i]) * motion_Data_Pre.addSubTime / MHz_2_Hz;
			}
			i++;
		}
		
		return (i * motion_Data_Pre.addSubTime);
	}
	
	// 减速阶段完成运动
	else if((motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[nAxis] < (motion_Data_Pre.PSC_Data[nAxis].addSpeed_NeedPlusNum) * 2))
	{
		i = 0;
		plus_Num = motion_Data_Pre.PSC_Data[nAxis].addSpeed_NeedPlusNum;
		while(plus_Num < motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[nAxis])
		{
			if(i >=  motion_Data_Pre.PSC_Data[nAxis].length) 		// i值有误
			{
				plus_Num += calClk_PSC(motion_Data_Pre.PSC_Data[nAxis].psc_data[maxStep]) * motion_Data_Pre.addSubTime / MHz_2_Hz;
				respMsgError("计算运行时间异常!\r\n", 1);
			}
			else 
			{
				plus_Num += calClk_PSC(motion_Data_Pre.PSC_Data[nAxis].psc_data[motion_Data_Pre.PSC_Data[nAxis].length - i - 1]) * 
										motion_Data_Pre.addSubTime / MHz_2_Hz;
			}
			i++;
		}
		
		return ((motion_Data_Pre.addSubTime * motion_Data_Pre.PSC_Data[nAxis].length) + (i * motion_Data_Pre.addSubTime));
	} 
	
	// 完成一个完整的运动
	else 
	{
		constSpeedTime = (motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[nAxis] - 
			2 * motion_Data_Pre.PSC_Data[nAxis].addSpeed_NeedPlusNum) / motion_Data_Pre.cmd_Datas.plus_Datas.clk[nAxis];
		
		return (motion_Data_Pre.addSubTime * motion_Data_Pre.PSC_Data[nAxis].length * 2 + constSpeedTime);
	}	
}


// 计算最小的步进数
//static u16 calMinStepNumLength(PSC_Data_Array	Psc_Data_Cur[])
//{
//	u8 i;
//	u16 minStepNumLength = MIN_STEP_LENGTH__INIT_VAULE;
//	
//	for(i=0; i<AXIS_NUM; i++)
//	{
//		if(0 != Psc_Data_Cur[i].length)
//		{
//			minStepNumLength = min(minStepNumLength, Psc_Data_Cur[i].length);
//		}
//	}
//	
//	return minStepNumLength;
//}

// 打开或关闭加减速功能的标记
static void En_ASS_Flag(FunctionalState status, N_Axis n_Axis)
{
	u8 i;
	
	if(ALL_Axis == n_Axis)
	{
		for(i=0; i<AXIS_NUM; i++)
		{
			motion_Data_Pre.PSC_Data[i].enAddSubFlag = status;
		}
	}
	else
	{
		motion_Data_Pre.PSC_Data[n_Axis].enAddSubFlag = status;
	}
}

// 修正指定轴的频率，并设置其起始频率
static ErrorStatus CorrectClk(u8 nAxis, float needTime)
{
	u32 clkTemp;
	ErrorStatus status;
	
	clkTemp = motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[nAxis] * MHz_2_Hz / needTime;
	
	if(clkTemp <= motion_Data_Pre.cmd_Datas.plus_Datas.clk[nAxis])
	{
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[nAxis] = clkTemp;
		status = SUCCESS;
	}
	else if(clkTemp <= n_Axis_Min_Clk(nAxis))
	{
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[nAxis] = motion_Data_Pre.cmd_Datas.plus_Datas.clk[nAxis];
		status = ERROR;
	}
	else
	{
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[nAxis] = n_Axis_Min_Clk(nAxis);
		status = ERROR;
	}
	
	motion_Data_Pre.nAxisClk_Cur[nAxis] = motion_Data_Pre.cmd_Datas.plus_Datas.clk[nAxis];
	
	return status;
}








