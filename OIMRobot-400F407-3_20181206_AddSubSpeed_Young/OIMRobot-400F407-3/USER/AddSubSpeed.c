/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          原能智能制造机器人
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : AddSubSpeed.C
* By  : Young
* 2018-12-06
* 修改加减速算法为S曲线
* 2018-06-28
********************************************************************************************************/

#include "AddSubSpeed.h"

// 全局变量区
extern u32 xAxisPlusNum,yAxisPlusNum,zAxisPlusNum,aAxisPlusNum,bAxisPlusNum;
extern u16 xAxisClk,yAxisClk,zAxisClk,aAxisClk,bAxisClk;
extern unsigned char  xAxisDir,yAxisDir,zAxisDir,aAxisDir,bAxisDir;
u16 xAxisClk_Cur,yAxisClk_Cur,zAxisClk_Cur,aAxisClk_Cur,bAxisClk_Cur;

extern u8 addtime;
extern u32 pwmNumTim12, pwmNumTim9, pwmNumTim3, pwmNumTim4, pwmNumTim2;
extern u8 xAxisStepFlag, yAxisStepFlag, zAxisStepFlag, aAxisStepFlag, bAxisStepFlag; 
extern u32 savePlusNum[5];		// 用来保存加减速时 加速阶段走过的脉冲数

AddSubSetting addSubSetting;					// 加减速算法策略

// 最大、最小频率，注意要提前赋值好
u16 max_Clk, min_Clk;
// 对应最大频率的轴， 0-4：x、y、z、a、b
u8 maxClkNum = 5;			// 等于5表示有误，不再修正

u8 ifUseAddSubFlag = 0;								// 0：使用加减速， 1：不使用
u8 respMsgData = 0;										// 0: 一切正常，1：不ok-可能存在爬行现象
u8 maxValueFlag, minValueFlag;				// 最大值、最小值标记， 0-4：x、y、z、a、b
u16 clkPerStep;												// 步进的频率
u16 clkTimeCntNeed[5];								// 每一步需要的时间，低频率下需要考虑 0-4:x、y、z、a、b
u16 minSpeedTimeCnt[5] = {0, 0, 0, 0, 0};		// 低速运行的计数器，单位为addtime。 0-4:x、y、z、a、b
u8 savePlusNumFlag[5] = {0, 0, 0, 0, 0};	// 是否保存脉冲数的标签，锁定在加速状态 0：不保存， 1：保存 0-4：x、y、z、a、b
u8 startSubFlag[5] = {0, 0, 0, 0, 0};			// 进入减速阶段的标记，0：不进入减速，1：开始减速
u16 subPlusCnt[5] = {0, 0, 0, 0, 0};		// 减速阶段所需的脉冲数
u16 correctSyncValue[5] = {0, 0, 0, 0, 0};		// 修正减速中间频率的减速阶段脉冲值

// 加减速主程序 clk_Src：原速度， clk_Dst：目标速度
void addSubSpeed_Cur(u16 clk_Src, u16 clk_Dst)
{
	// 初始化的过程必须想办法锁定，以免被中断产生问题 临界区
	nAxisInit();
	
	nAxisStart();
	
	// 系统启动后（可能未停止），即回复上位机 1:ok
	respMsg(1);
}
void addSubSpeed_CurTest(u16 clk_Src, u16 clk_Dst)
{
	zAxisClk_Cur = Z_AXIS_MIN_CLK;
	TIM4_PWM_Init(ARR_VAL-1, calPSC(zAxisClk_Cur)-1);				// 速度初始化
	savePlusNumFlag[2] = 1;																		// 刚开始需要保存加速阶段的脉冲数
//	calClkPerStep();
}

// 各轴启动
void nAxisStart(void)
{
	motor5workstart();
	subAddTimerStart();
}

// 计算最低频率需要的运行时间（计数值） 
// 0：无频率， 0：最大频率， 其它值：小于最大速度的中间频率
void calMinSpeedTimeCnt(void)
{
	// X轴
	if(0 == xAxisClk)
		minSpeedTimeCnt[0] = 0;
	else if(max_Clk == xAxisClk)
		minSpeedTimeCnt[0] = 0;
	else
		minSpeedTimeCnt[0] = max_Clk - xAxisClk;
	
	// Y轴
	if(0 == yAxisClk)
		minSpeedTimeCnt[1] = 0;
	else if(max_Clk == yAxisClk)
		minSpeedTimeCnt[1] = 0;
	else
		minSpeedTimeCnt[1] = max_Clk - yAxisClk;
	
	// Z轴
	if(0 == zAxisClk)
		minSpeedTimeCnt[2] = 0;
	else if(max_Clk == zAxisClk)
		minSpeedTimeCnt[2] = 0;
	else
		minSpeedTimeCnt[2] = max_Clk - zAxisClk;
	
	// A轴
	if(0 == aAxisClk)
		minSpeedTimeCnt[3] = 0;
	else if(max_Clk == aAxisClk)
		minSpeedTimeCnt[3] = 0;
	else
		minSpeedTimeCnt[3] = max_Clk - aAxisClk;
	
	// B轴
	if(0 == bAxisClk)
		minSpeedTimeCnt[4] = 0;
	else if(max_Clk == bAxisClk)
		minSpeedTimeCnt[4] = 0;
	else
		minSpeedTimeCnt[4] = max_Clk - bAxisClk;	
}

// 加减速定时器中断服务程序  考虑改成内联函数或宏定义，提高运行效率  byYJY
void addSubTime_IRQ(void)
{
	if((xAxisPlusNum != 0) && (xAxisClk != 0))
		xAddSubTime_IRQ();
	if((yAxisPlusNum != 0) && (yAxisClk != 0))
		yAddSubTime_IRQ();
	if((zAxisPlusNum != 0) && (zAxisClk != 0))
		zAddSubTime_IRQ();
	if((aAxisPlusNum != 0) && (aAxisClk != 0))
		aAddSubTime_IRQ();
	if((bAxisPlusNum != 0) && (bAxisClk != 0))
		bAddSubTime_IRQ();
}
void xAddSubTime_IRQ(void)
{
	// X轴
	// 速度太低不进行加减速
	if(X_AXIS_MIN_CLK >= xAxisClk)
		return;
	
	// 最低速运行阶段
	if(0 != minSpeedTimeCnt[0])
	{
		minSpeedTimeCnt[0]--;
	}
	// 加速阶段
	else if(1 == savePlusNumFlag[0])												// 锁定在加速阶段
	{
		if(xAxisClk_Cur <= xAxisClk)										// 加速阶段
		{
			xAxisClk_Cur++;
			if(clkTimeCntNeed[0])													// 低频率下需要较长的维持时间
			{
				clkTimeCntNeed[0]--;
			} else
			{
				if( ifX_PscModify(calPSC(xAxisClk_Cur)) )			// 如果PSC值修改了，再重新赋值
				{
					xAxisSetPWM(calPSC(xAxisClk_Cur), calDutyCycle(xAxisClk_Cur), calDutyCycle(xAxisClk_Cur));
				}					
				clkTimeCntNeed[0] = calPWM_TimeCnt(xAxisClk_Cur, setAddTime());
			}
		} 
		else {
			savePlusNum[0] = xPWMCnt;
			savePlusNumFlag[0] = 0;															// 关闭标签，锁定在匀、减速阶段
		} 
	}
	// 减速阶段
	else if(startSubFlag[0]) 				// 减速阶段，标记为1时开始减速，在exti.c中置位
	{
		if(xAxisClk_Cur > X_AXIS_MIN_CLK)					// 递减到最小频率，小心递减到”负数”
		{
			xAxisClk_Cur--;
			if(clkTimeCntNeed[0])
			{
				clkTimeCntNeed[0]--;
			} else
			{
				if( ifX_PscModify(calPSC(xAxisClk_Cur)) )
				{
					xAxisSetPWM(calPSC(xAxisClk_Cur), calDutyCycle(xAxisClk_Cur), calDutyCycle(xAxisClk_Cur));
				}					
				clkTimeCntNeed[0] = calPWM_TimeCnt(xAxisClk_Cur, setAddTime());
			}
		}
	}	
}
void yAddSubTime_IRQ(void)
{
	// Y轴
	// 速度太低不进行加减速
	if(Y_AXIS_MIN_CLK >= yAxisClk)
		return;
	
	// 最低速运行阶段
	if(0 != minSpeedTimeCnt[1])
	{
		minSpeedTimeCnt[1]--;
	}
	// 加速阶段
	else if(1 == savePlusNumFlag[1])												// 锁定在加速阶段
	{
		if(yAxisClk_Cur <= yAxisClk)										// 加速阶段
		{
			yAxisClk_Cur++;
			if(clkTimeCntNeed[1])													// 低频率下需要较长的维持时间
			{
				clkTimeCntNeed[1]--;
			} else
			{
				if( ifY_PscModify(calPSC(yAxisClk_Cur)) )			// 如果PSC值修改了，再重新赋值
				{
					yAxisSetPWM(calPSC(yAxisClk_Cur), calDutyCycle(yAxisClk_Cur), calDutyCycle(yAxisClk_Cur));
				}					
				clkTimeCntNeed[1] = calPWM_TimeCnt(yAxisClk_Cur, setAddTime());
			}
		} 
		else {
			savePlusNum[1] = yPWMCnt;
			savePlusNumFlag[1] = 0;															// 关闭标签，锁定在匀、减速阶段
		} 
	}
	// 减速阶段
//	else if((yAxisPlusNum - yPWMCnt) < (savePlusNum[1]-correctSync(yAxisClk, 1))) // 减速阶段，剩余的脉冲数小于加速阶段的脉冲数
	else if(startSubFlag[1]) 				// 减速阶段，标记为1时开始减速，在exti.c中置位
	{
		if(yAxisClk_Cur > Y_AXIS_MIN_CLK)					// 递减到最小频率，小心递减到”负数”
		{
			yAxisClk_Cur--;
			if(clkTimeCntNeed[1])
			{
				clkTimeCntNeed[1]--;
			} else
			{
				if( ifY_PscModify(calPSC(yAxisClk_Cur)) )
				{
					yAxisSetPWM(calPSC(yAxisClk_Cur), calDutyCycle(yAxisClk_Cur), calDutyCycle(yAxisClk_Cur));
				}					
				clkTimeCntNeed[1] = calPWM_TimeCnt(yAxisClk_Cur, setAddTime());
			}
		}
	}	
}
void zAddSubTime_IRQ(void)
{
	// Z轴
	// 速度太低不进行加减速
	if(Z_AXIS_MIN_CLK >= zAxisClk)
		return;
	
	// 最低速运行阶段
	if(0 != minSpeedTimeCnt[2])
	{
		minSpeedTimeCnt[2]--;
	}
	// 加速阶段
	else if(1 == savePlusNumFlag[2])												// 锁定在加速阶段
	{
		if(zAxisClk_Cur <= zAxisClk)										// 加速阶段
		{
			zAxisClk_Cur++;
			if(clkTimeCntNeed[2])													// 低频率下需要较长的维持时间
			{
				clkTimeCntNeed[2]--;
			} else
			{
				if( ifZ_PscModify(calPSC(zAxisClk_Cur)) )			// 如果PSC值修改了，再重新赋值
				{
					zAxisSetPWM(calPSC(zAxisClk_Cur), calDutyCycle(zAxisClk_Cur), calDutyCycle(zAxisClk_Cur));
				}					
				clkTimeCntNeed[2] = calPWM_TimeCnt(zAxisClk_Cur, setAddTime());
			}
		} 
		else {
			savePlusNum[2] = zPWMCnt;										// test
			savePlusNumFlag[2] = 0;															// 关闭标签，锁定在匀、减速阶段
		} 
	}
	// 减速阶段
	else if(startSubFlag[2]) 				// 减速阶段，标记为1时开始减速，在exti.c中置位
	{
		if(zAxisClk_Cur > Z_AXIS_MIN_CLK)					// 递减到最小频率，小心递减到”负数”
		{
			zAxisClk_Cur--;
			if(clkTimeCntNeed[2])
			{
				clkTimeCntNeed[2]--;
			} else
			{
				if( ifZ_PscModify(calPSC(zAxisClk_Cur)) )
				{
					zAxisSetPWM(calPSC(zAxisClk_Cur), calDutyCycle(zAxisClk_Cur), calDutyCycle(zAxisClk_Cur));
				}					
				clkTimeCntNeed[2] = calPWM_TimeCnt(zAxisClk_Cur, setAddTime());
			}
		}
	}
}
void aAddSubTime_IRQ(void)
{
	// A轴
	// 速度太低不进行加减速
	if(A_AXIS_MIN_CLK >= aAxisClk)
		return;
	
	// 最低速运行阶段
	if(0 != minSpeedTimeCnt[3])
	{
		minSpeedTimeCnt[3]--;
	}
	// 加速阶段
	else if(1 == savePlusNumFlag[3])												// 锁定在加速阶段
	{
		if(aAxisClk_Cur <= aAxisClk)										// 加速阶段
		{
			aAxisClk_Cur++;
			if(clkTimeCntNeed[3])													// 低频率下需要较长的维持时间
			{
				clkTimeCntNeed[3]--;
			} else
			{
				if( ifA_PscModify(calPSC(aAxisClk_Cur)) )			// 如果PSC值修改了，再重新赋值
				{
					aAxisSetPWM(calPSC(aAxisClk_Cur), calDutyCycle(aAxisClk_Cur), calDutyCycle(aAxisClk_Cur));
				}					
				clkTimeCntNeed[3] = calPWM_TimeCnt(aAxisClk_Cur, setAddTime());
			}
		} 
		else {
			savePlusNum[3] = aPWMCnt;										// test
			savePlusNumFlag[3] = 0;															// 关闭标签，锁定在匀、减速阶段
		} 
	}
	// 减速阶段
	else if(startSubFlag[3]) 				// 减速阶段，标记为1时开始减速，在exti.c中置位
	{
		if(aAxisClk_Cur > A_AXIS_MIN_CLK)					// 递减到最小频率，小心递减到”负数”
		{
			aAxisClk_Cur--;
			if(clkTimeCntNeed[3])
			{
				clkTimeCntNeed[3]--;
			} else
			{
				if( ifA_PscModify(calPSC(aAxisClk_Cur)) )
				{
					aAxisSetPWM(calPSC(aAxisClk_Cur), calDutyCycle(aAxisClk_Cur), calDutyCycle(aAxisClk_Cur));
				}
				clkTimeCntNeed[3] = calPWM_TimeCnt(aAxisClk_Cur, setAddTime());
			}
		}
	}
}
void bAddSubTime_IRQ(void)
{
	// B轴
	// 速度太低不进行加减速
	if(B_AXIS_MIN_CLK >= bAxisClk)
//	{ 
//		if(0 != savePlusNum[4])  // 暂时保留这段
//		{
//			correctNoAddSubClk(4, bAxisPlusNum);		// 不加减速，并修正原速度  
//			bAxisSetPWM(calPSC(bAxisClk), calDutyCycle(bAxisClk), calDutyCycle(bAxisClk));
//		}
		return;
//	}
	
	// 最低速运行阶段
	if(0 != minSpeedTimeCnt[4])
	{
		minSpeedTimeCnt[4]--;
	}
	// 加速阶段
	else if(1 == savePlusNumFlag[4])												// 锁定在加速阶段
	{
		if(bAxisClk_Cur <= bAxisClk)										// 加速阶段
		{
			bAxisClk_Cur++;
			if(clkTimeCntNeed[4])													// 低频率下需要较长的维持时间
			{
				clkTimeCntNeed[4]--;
			} else
			{
				if( ifB_PscModify(calPSC(bAxisClk_Cur)) )			// 如果PSC值修改了，再重新赋值
				{
					bAxisSetPWM(calPSC(bAxisClk_Cur), calDutyCycle(bAxisClk_Cur), calDutyCycle(bAxisClk_Cur));
				}					
				clkTimeCntNeed[4] = calPWM_TimeCnt(bAxisClk_Cur, setAddTime());
			}
		} 
		else {
			savePlusNum[4] = bPWMCnt;										// test
			savePlusNumFlag[4] = 0;															// 关闭标签，锁定在匀、减速阶段
		} 
	}
	// 减速阶段
	else if(startSubFlag[4]) 				// 减速阶段，标记为1时开始减速，在exti.c中置位
	{
		if(bAxisClk_Cur > B_AXIS_MIN_CLK)					// 递减到最小频率，小心递减到”负数”
		{
			bAxisClk_Cur--;
			if(clkTimeCntNeed[4])
			{
				clkTimeCntNeed[4]--;
			} else
			{
				if( ifB_PscModify(calPSC(bAxisClk_Cur)) )
				{
					bAxisSetPWM(calPSC(bAxisClk_Cur), calDutyCycle(bAxisClk_Cur), calDutyCycle(bAxisClk_Cur));
				}					
				clkTimeCntNeed[4] = calPWM_TimeCnt(bAxisClk_Cur, setAddTime());
			}
		}
	}
}

// 各轴初始化
void nAxisInit(void)
{
	// 最大、最小频率计算，重要，必须提前计算
	max_Clk = preProcMaxClk_Cur();
	min_Clk = preProcMinClk_Cur();
	// 调整各轴频率符合要求 该函数可返回状态值
	adjustClk(max_Clk, min_Clk);
	// 计算最小速度的持续时间
	calMinSpeedTimeCnt();
	memset(savePlusNum, 0, sizeof(savePlusNum));
	memset(subPlusCnt, 0, sizeof(subPlusCnt));
	memset(correctSyncValue, 0, sizeof(correctSyncValue));
	
	if((xAxisPlusNum != 0) && (xAxisClk != 0))
		xAxisInit();
	
	if((yAxisPlusNum != 0) && (yAxisClk != 0))
		yAxisInit();
	
	if((zAxisPlusNum != 0) && (zAxisClk != 0))
		zAxisInit();
	
	if((aAxisPlusNum != 0) && (aAxisClk != 0))
		aAxisInit();
	
	if((bAxisPlusNum != 0) && (bAxisClk != 0))
		bAxisInit();
}
// X轴初始化
void xAxisInit(void)
{
	// X轴各项参数初始化
	if(xAxisClk > X_AXIS_MIN_CLK)
		xAxisClk_Cur = X_AXIS_MIN_CLK;
	else
	{
		correctNoAddSubClk(0, xAxisPlusNum);		// 不加减速，并修正原速度
		xAxisClk_Cur = xAxisClk;				
	}
	
	// 计算减速阶段的脉冲修正值
	correctSyncValue[0] = correctSync(X_AXIS_MIN_CLK, 0);
	
	subPlusCnt[0] = calSubPlusCnt(0, xAxisClk) + 1;		// 脉冲数多+1，避免约掉小数（后续是否考虑四舍五入？） byYJY
	clkTimeCntNeed[0] = calPWM_TimeCnt(xAxisClk_Cur, setAddTime());		// 注意addtime的值跟TIM7相关	
	xAxis_PWM_Init(ARR_VAL-1, calPSC(xAxisClk_Cur)-1);				// 速度初始化
	xAxisSetPWM(calPSC(xAxisClk_Cur)-1, calDutyCycle(xAxisClk_Cur), calDutyCycle(xAxisClk_Cur));	// 设置PWM的参数，寄存器方法
	savePlusNumFlag[0] = 1;						// 用来确定减速阶段的标记
	xSetPWMCnt(0);									// PWM中断计数置零
	xAxisStepFlag = 0;							// 打开正在运行的标记
	if(xAxisDir==0)									// 注意在 ComDataProc.c 中已经判断了方向，但是这里判断更好
	{
		xAxisDIR_ON;
	}
	else
	{
		xAxisDIR_OFF;
	}	
}
// Y轴初始化
void yAxisInit(void)
{
	// Y轴各项参数初始化
	if(yAxisClk > Y_AXIS_MIN_CLK)
		yAxisClk_Cur = Y_AXIS_MIN_CLK;
	else
	{
		correctNoAddSubClk(1, yAxisPlusNum);		// 不加减速，并修正原速度
		yAxisClk_Cur = yAxisClk;				
	}
	
	// 计算减速阶段的脉冲修正值
	correctSyncValue[1] = correctSync(Y_AXIS_MIN_CLK, 1);
	
	subPlusCnt[1] = calSubPlusCnt(1, yAxisClk) + 1;	// 脉冲数多+1，避免约掉小数（后续是否考虑四舍五入？） byYJY
	clkTimeCntNeed[1] = calPWM_TimeCnt(yAxisClk_Cur, setAddTime());		// 注意addtime的值跟TIM7相关	
	yAxis_PWM_Init(ARR_VAL-1, calPSC(yAxisClk_Cur)-1);				// 速度初始化
	// 设置PWM的参数，寄存器方法
	yAxisSetPWM(calPSC(yAxisClk_Cur)-1, calDutyCycle(yAxisClk_Cur), calDutyCycle(yAxisClk_Cur));	
	savePlusNumFlag[1] = 1;						// 用来确定减速阶段的标记
	ySetPWMCnt(0);									// PWM中断计数置零
	yAxisStepFlag = 0;							// 打开正在运行的标记
	if(yAxisDir==0)									// 注意在 ComDataProc.c 中已经判断了方向，但是这里判断更好
	{
		yAxisDIR_ON;
	}
	else
	{
		yAxisDIR_OFF;
	}	
}
// Z轴初始化
void zAxisInit(void)
{
	// Z轴各项参数初始化
	if(zAxisClk > Z_AXIS_MIN_CLK)
		zAxisClk_Cur = Z_AXIS_MIN_CLK;
	else
	{
		correctNoAddSubClk(2, zAxisPlusNum);		// 不加减速，并修正原速度
		zAxisClk_Cur = zAxisClk;				
	}
	
	// 计算减速阶段的脉冲修正值
	correctSyncValue[2] = correctSync(Z_AXIS_MIN_CLK, 2);
	
	subPlusCnt[2] = calSubPlusCnt(2, zAxisClk) + 1;					// 脉冲数多+1，避免约掉小数（后续是否考虑四舍五入？） byYJY
	clkTimeCntNeed[2] = calPWM_TimeCnt(zAxisClk_Cur, setAddTime());		// 注意addtime的值跟TIM7相关	
	zAxis_PWM_Init(ARR_VAL-1, calPSC(zAxisClk_Cur)-1);				// 速度初始化
	zAxisSetPWM(calPSC(zAxisClk_Cur)-1, calDutyCycle(zAxisClk_Cur), calDutyCycle(zAxisClk_Cur));	// 设置PWM的参数，寄存器方法
	savePlusNumFlag[2] = 1;						// 用来确定减速阶段的标记
	zSetPWMCnt(0);									// PWM中断计数置零
	zAxisStepFlag = 0;							// 打开正在运行的标记
	if(zAxisDir==0)									// 注意在 ComDataProc.c 中已经判断了方向，但是这里判断更好
	{
		zAxisDIR_ON;
	}
	else
	{
		zAxisDIR_OFF;
	}
}
// A轴初始化
void aAxisInit(void)
{
	// A轴各项参数初始化
	if(aAxisClk > A_AXIS_MIN_CLK)
		aAxisClk_Cur = A_AXIS_MIN_CLK;
	else
	{
		correctNoAddSubClk(3, aAxisPlusNum);		// 不加减速，并修正原速度
		aAxisClk_Cur = aAxisClk;				
	}
	
	// 计算减速阶段的脉冲修正值
	correctSyncValue[3] = correctSync(A_AXIS_MIN_CLK, 3);
	
	subPlusCnt[3] = calSubPlusCnt(3, aAxisClk) + 1;					// 脉冲数多+1，避免约掉小数（后续是否考虑四舍五入？） byYJY
	clkTimeCntNeed[3] = calPWM_TimeCnt(aAxisClk_Cur, setAddTime());		// 注意addtime的值跟TIM7相关	
	aAxis_PWM_Init(ARR_VAL-1, calPSC(aAxisClk_Cur)-1);				// 速度初始化
	aAxisSetPWM(calPSC(aAxisClk_Cur)-1, calDutyCycle(aAxisClk_Cur), calDutyCycle(aAxisClk_Cur));	// 设置PWM的参数，寄存器方法
	savePlusNumFlag[3] = 1;						// 用来确定减速阶段的标记
	aSetPWMCnt(0);									// PWM中断计数置零
	aAxisStepFlag = 0;							// 打开正在运行的标记
	if(aAxisDir==0)									// 注意在 ComDataProc.c 中已经判断了方向，但是这里判断更好
	{
		aAxisDIR_ON;
	}
	else
	{
		aAxisDIR_OFF;
	}
}
// B轴初始化
void bAxisInit(void)
{
	// B轴各项参数初始化
	if(bAxisClk > B_AXIS_MIN_CLK)
		bAxisClk_Cur = B_AXIS_MIN_CLK;
	else
	{
		correctNoAddSubClk(4, bAxisPlusNum);		// 不加减速，并修正原速度
		bAxisClk_Cur = bAxisClk;				
	}
	
	// 计算减速阶段的脉冲修正值
	correctSyncValue[4] = correctSync(B_AXIS_MIN_CLK, 4);
	
	subPlusCnt[4] = calSubPlusCnt(4, bAxisClk) + 1;					// 脉冲数多+1，避免约掉小数（后续是否考虑四舍五入？） byYJY
	clkTimeCntNeed[4] = calPWM_TimeCnt(bAxisClk_Cur, setAddTime());		// 注意addtime的值跟TIM7相关	
	bAxis_PWM_Init(ARR_VAL-1, calPSC(bAxisClk_Cur)-1);				// 速度初始化
	bAxisSetPWM(calPSC(bAxisClk_Cur)-1, calDutyCycle(bAxisClk_Cur), calDutyCycle(bAxisClk_Cur));	// 设置PWM的参数，寄存器方法
	savePlusNumFlag[4] = 1;						// 用来确定减速阶段的标记
	bSetPWMCnt(0);									// PWM中断计数置零
	bAxisStepFlag = 0;							// 打开正在运行的标记
	if(bAxisDir==0)									// 注意在 ComDataProc.c 中已经判断了方向，但是这里判断更好
	{
		bAxisDIR_ON;
	}
	else
	{
		bAxisDIR_OFF;
	}
}

// 修正同步的参数
u16 correctSync(u16 nAxisMinClk, u8 nAxis)
{
	// 修正同步的参数
	// 修正的方式还需要研究，暂时先写这个 byYJY
	if((5 > maxClkNum) && (nAxisMinClk != 0) && (max_Clk != nAxisMinClk))
		return (nAxisMinClk*minSpeedTimeCnt[nAxis]*setAddTime()/2e6 + 1);					// 加1修正小数
	else
		return 0;									
}

// 计算每次发送脉冲所需要的时间_ 计数， time单位为us，所以有1e6
u16 calPWM_TimeCnt(u16 clk, u16 time)
{
	return (1e6 / (clk * time));
}

// 修正不加减速的轴的频率
void correctNoAddSubClk(u8 nAxis, u32 plusNum)
{
	u16 clk;
	float time = calTotalTime(maxClkNum);						// 该时间由最高频率的轴决定
	if(0 == (int)time)
		return;
	
	clk = (int)((float)(1e6*plusNum) / time);
	if(0 == clk)
		return;
	
	switch(nAxis)
	{
		case 0:
			if(xAxisClk > clk)
				xAxisClk = clk;
		break;
			
		case 1:
			if(yAxisClk > clk)
				yAxisClk = clk;
		break;
			
		case 2:
			if(zAxisClk > clk)
				zAxisClk = clk;
		break;
			
		case 3:
			if(aAxisClk > clk)
				aAxisClk = clk;
		break;
			
		case 4:
			if(bAxisClk > clk)
				bAxisClk = clk;
		break;
			
		default:
			break;					
	}	
}
// 计算减速阶段需要的脉冲数(浮点数)
u16 calSubPlusCnt(u8 nAxis, u16 nAxisClk)
{
	u16 min_Clk;
	float timeAddSub;
	u16 addSubPlusCnt;
	
	switch(nAxis)
	{
		case 0:
			min_Clk = X_AXIS_MIN_CLK;
		break;
		
		case 1:
			min_Clk = Y_AXIS_MIN_CLK;
		break;
		
		case 2:
			min_Clk = Z_AXIS_MIN_CLK;
		break;
		
		case 3:
			min_Clk = A_AXIS_MIN_CLK;
		break;
		
		case 4:
			min_Clk = B_AXIS_MIN_CLK;
		
		default:
			return 0;
	}
	
	timeAddSub = (nAxisClk - min_Clk) * setAddTime();		// 单位us
	addSubPlusCnt = (nAxisClk + min_Clk) / 2.0 * timeAddSub * 1e-6;		// 1e-6：因为是us到s
	return addSubPlusCnt;
}
//u16 calSubPlusCnt(u8 nAxis)
//{
//	u16 min_Clk;
//	float timeAddSub;
//	u16 addSubPlusCnt;
//	
//	switch(nAxis)
//	{
//		case 0:
//			min_Clk = X_AXIS_MIN_CLK;
//		break;
//		
//		case 1:
//			min_Clk = Y_AXIS_MIN_CLK;
//		break;
//		
//		case 2:
//			min_Clk = Z_AXIS_MIN_CLK;
//		break;
//		
//		case 3:
//			min_Clk = A_AXIS_MIN_CLK;
//		break;
//		
//		case 4:
//			min_Clk = B_AXIS_MIN_CLK;
//		
//		default:
//			return 0;
//	}
//	
//	timeAddSub = (max_Clk - min_Clk) * setAddTime();		// 单位us
//	addSubPlusCnt = (max_Clk + min_Clk) / 2.0 * timeAddSub * 1e-6;		// 1e-6：因为是us到s
//	return addSubPlusCnt;
//}

// 估算一帧需要的时间， 单位us
float calTotalTime(u8 nAxis)
{
	u16 min_Clk;
	float timeAddSub;
	float plusNum;
	switch(nAxis)
	{
		case 0:
			min_Clk = X_AXIS_MIN_CLK;
			plusNum = xAxisPlusNum;
		break;
		
		case 1:
			min_Clk = Y_AXIS_MIN_CLK;
			plusNum = yAxisPlusNum;
		break;
		
		case 2:
			min_Clk = Z_AXIS_MIN_CLK;
			plusNum = zAxisPlusNum;
		break;
		
		case 3:
			min_Clk = A_AXIS_MIN_CLK;
			plusNum = aAxisPlusNum;
		break;
		
		case 4:
			min_Clk = B_AXIS_MIN_CLK;
			plusNum = bAxisPlusNum;
		break;
		
		default:
			return 0;
	}
	
	// 加减速所需时间 us
	timeAddSub = (max_Clk - min_Clk) * setAddTime();									
	return ((float)(1e6*plusNum - (max_Clk+min_Clk)*timeAddSub)/(float)max_Clk + 2 * timeAddSub);
	// 故意让timeAddSub除以2，使得波形同步，原因未知  byYJY
//	return ((float)(1e6*plusNum - (max_Clk+min_Clk)*timeAddSub/2)/(float)max_Clk + 2 * timeAddSub);
}

// 计算占空比的函数
u16 calDutyCycle(u16 clk)
{
	u16 dutyCycleCnt;
	dutyCycleCnt = (PLUS_WIDTH * ARR_VAL * clk) / 1e6;
	if(0 == dutyCycleCnt)																		// 占空比不能为0
		dutyCycleCnt = 1;	
	return dutyCycleCnt;
}

// 计算每次步进的频率
void calClkPerStep(void)
{
	clkPerStep = (xAxisClk - xAxisClk_Cur) / (ADDTIME * 1000 / addtime);		// 暂时不考虑减速情况
}

// 通过CLK频率计算预分频系数 PSC
u16 calPSC(u16 clk)
{
	u16 psc;
	psc = PSC_CLK / clk;
	return psc;
}

//// 设置加减速的算法结构体
//void setAddSubSetting(void)
//{
//	u16 maxClk = preProcMaxClk_Cur();
//	u16 minClk = preProcMinClk_Cur();
//	adjustClk(maxClk, minClk);
//	
//	/////////////
//}

// 频率调整，使得频率位于合适范围内。0：调整ok，1：有轴运行在最低频率以下  byYJY
u8 adjustClk(u16 maxClk, u16 minClk)
{
	u8 status = 0;
	if(xAxisClk == maxClk)								// X轴最大值
	{
		if(maxClk > X_AXIS_MAX_CLK)					// 需要降频
		{
			underClk();
			maxClk = preProcMaxClk_Cur();
			minClk = preProcMinClk_Cur();
			adjustClk(maxClk, minClk);
		} else if(maxClk < X_AXIS_MIN_CLK)	// 需要升频
		{
//			overClk();											// 不升频
//			maxClk = preProcMaxClk_Cur();
//			minClk = preProcMinClk_Cur();
//			adjustClk(maxClk, minClk);
		}
	}
	
	if(yAxisClk == maxClk)								// y轴最大值
	{
		if(maxClk > Y_AXIS_MAX_CLK)					// 需要降频
		{
			underClk();
			maxClk = preProcMaxClk_Cur();
			minClk = preProcMinClk_Cur();
			adjustClk(maxClk, minClk);
		} else if(maxClk < Y_AXIS_MIN_CLK)	// 需要升频
		{
//			overClk();
//			maxClk = preProcMaxClk_Cur();
//			minClk = preProcMinClk_Cur();
//			adjustClk(maxClk, minClk);
		}
	}
	
	if(zAxisClk == maxClk)								// z轴最大值
	{
		if(maxClk > Z_AXIS_MAX_CLK)					// 需要降频
		{
			underClk();
			maxClk = preProcMaxClk_Cur();
			minClk = preProcMinClk_Cur();
			adjustClk(maxClk, minClk);
		} else if(maxClk < Z_AXIS_MIN_CLK)	// 需要升频
		{
//			overClk();
//			maxClk = preProcMaxClk_Cur();
//			minClk = preProcMinClk_Cur();
//			adjustClk(maxClk, minClk);
		}
	}
	
	if(aAxisClk == maxClk)								// a轴最大值
	{
		if(maxClk > A_AXIS_MAX_CLK)					// 需要降频
		{
			underClk();
			maxClk = preProcMaxClk_Cur();
			minClk = preProcMinClk_Cur();
			adjustClk(maxClk, minClk);
		} else if(maxClk < A_AXIS_MIN_CLK)	// 需要升频
		{
//			overClk();
//			maxClk = preProcMaxClk_Cur();
//			minClk = preProcMinClk_Cur();
//			adjustClk(maxClk, minClk);
		}
	}
	
	if(bAxisClk == maxClk)								// b轴最大值
	{
		if(maxClk > B_AXIS_MAX_CLK)					// 需要降频
		{
			underClk();
			maxClk = preProcMaxClk_Cur();
			minClk = preProcMinClk_Cur();
			adjustClk(maxClk, minClk);
		} else if(maxClk < B_AXIS_MIN_CLK)	// 需要升频
		{
//			overClk();
//			maxClk = preProcMaxClk_Cur();
//			minClk = preProcMinClk_Cur();
//			adjustClk(maxClk, minClk);
		}
	}	
	
	// 最小值作为辅助判断条件
	if(0 != minClk)
	{
		if(xAxisClk == minClk)								// x轴为最小值
		{
			if(minClk < X_AXIS_MIN_CLK)					// 最小值过小
			{
				if(ifOverClkOK(maxClk))									// 判断是否可以升频
				{
					overClk();
					maxClk = preProcMaxClk_Cur();
					minClk = preProcMinClk_Cur();
					adjustClk(maxClk, minClk);
				} else {
					status = 1;											// 标记有轴运行在最低频率以下
				}
			}
		}
		
		if(yAxisClk == minClk)								// y轴为最小值
		{
			if(minClk < Y_AXIS_MIN_CLK)					// 最小值过小
			{
				if(ifOverClkOK(maxClk))									// 判断是否可以升频
				{
					overClk();
					maxClk = preProcMaxClk_Cur();
					minClk = preProcMinClk_Cur();
					adjustClk(maxClk, minClk);
				} else {
					status = 1;											// 标记有轴运行在最低频率以下
				}
			}
		}
		
		if(zAxisClk == minClk)								// z轴为最小值
		{
			if(minClk < Z_AXIS_MIN_CLK)					// 最小值过小
			{
				if(ifOverClkOK(maxClk))									// 判断是否可以升频
				{
					overClk();
					maxClk = preProcMaxClk_Cur();
					minClk = preProcMinClk_Cur();
					adjustClk(maxClk, minClk);
				} else {
					status = 1;											// 标记有轴运行在最低频率以下
				}
			}
		}
		
		if(aAxisClk == minClk)								// a轴为最小值
		{
			if(minClk < A_AXIS_MIN_CLK)					// 最小值过小
			{
				if(ifOverClkOK(maxClk))									// 判断是否可以升频
				{
					overClk();
					maxClk = preProcMaxClk_Cur();
					minClk = preProcMinClk_Cur();
					adjustClk(maxClk, minClk);
				} else {
					status = 1;											// 标记有轴运行在最低频率以下
				}
			}
		}
		
		if(bAxisClk == minClk)								// b轴为最小值
		{
			if(minClk < B_AXIS_MIN_CLK)					// 最小值过小
			{
				if(ifOverClkOK(maxClk))									// 判断是否可以升频
				{
					overClk();
					maxClk = preProcMaxClk_Cur();
					minClk = preProcMinClk_Cur();
					adjustClk(maxClk, minClk);
				} else {
					status = 1;											// 标记有轴运行在最低频率以下
				}
			}
		}
	}	
	return status;
}

// 是否可以升频的判断 0:不可以， 1：可以
u8 ifOverClkOK(u16 maxClk)
{
	u8 result = 1;
	if((xAxisClk << 1) > X_AXIS_MAX_CLK)		// 升频后大于最大频率，不允许升频
	{
		result = 0;
		return result;
	}
	if((yAxisClk << 1) > Y_AXIS_MAX_CLK)		// 升频后大于最大频率，不允许升频
	{
		result = 0;
		return result;
	}
	if((zAxisClk << 1) > Z_AXIS_MAX_CLK)		// 升频后大于最大频率，不允许升频
	{
		result = 0;
		return result;
	}
	if((aAxisClk << 1) > A_AXIS_MAX_CLK)		// 升频后大于最大频率，不允许升频
	{
		result = 0;
		return result;
	}
	if((bAxisClk << 1) > B_AXIS_MAX_CLK)		// 升频后大于最大频率，不允许升频
	{
		result = 0;
		return result;
	}	
	return result;
}

// 降频，有精度丢失
void underClk(void)
{
	xAxisClk = (xAxisClk >> 1);
	yAxisClk = (yAxisClk >> 1);
	zAxisClk = (zAxisClk >> 1);
	aAxisClk = (aAxisClk >> 1);
	bAxisClk = (bAxisClk >> 1);
}
// 升频，速度太慢时使用，优先级低
void overClk(void)
{
	xAxisClk = (xAxisClk << 1);
	yAxisClk = (yAxisClk << 1);
	zAxisClk = (zAxisClk << 1);
	aAxisClk = (aAxisClk << 1);
	bAxisClk = (bAxisClk << 1);
}

// 计算给定时间内运行的脉冲数 timeCnt 为 addtime的倍数
u32 calPlusNum(u16 clk, u8 timeCnt)
{
	u32 plusNum;
	plusNum = (timeCnt * addSubSetting.addSubTimePerStep) / 1000;		// 注意可能丢失精度
	return plusNum;
}

// 计算运行时间，浮点运算
float calTime(u32 plusNum, u16 clk)
{
	float time;
	time = ((float)plusNum * (float)1000.0) / (float)clk;				// 单位ms
	return time;
}

// 没有加减速
void noAddSubSpeed(void)
{
	xAxisClkSet(xAxisClk);
	yAxisClkSet(yAxisClk);
	zAxisClkSet(zAxisClk);
	aAxisClkSet(aAxisClk);
	bAxisClkSet(bAxisClk);
}

// 收到串口消息回复 0：not ok， 1：ok
void respMsg(u8 status)
{
	u8 i;
	u8 backResString[9];
	
	backResString[0] = FrameB1;
	backResString[1] = FrameB2;
	backResString[2] = DATAINFO;
	
	backResString[3] = 9;
	backResString[4] = 0x00;
	
	backResString[5] = 0x22;
	backResString[6] = 0x00;
	
	backResString[7] = status;					// 是否ok
	backResString[8] = FrameEnd;
	
	// 标记为0才回复
	if(0 == respFlag)
	{
		for(i=0;i<9;i++)
			{
				USART_SendData(USART3,backResString[i]); 
				while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);	//等待发送结束
			}
	}
}

// 错误信息打印
void printf_error(void)
{
	
}

// 加减速策略初始化
void addSubSetting_Init(u8 addCount)
{
	addSubSetting.addCount = addCount;
	addSubSetting.clkPerStep = (u16 *)malloc(addCount * sizeof(u16));
	if(!addSubSetting.clkPerStep)										// 分配内存失败
	{
		printf_error();
		ifUseAddSubFlag = 1;													// 不使用加减速
		return;
	}
	addSubSetting.addSubTimePerStep	= setAddTime();	// 设置加减速的每步的运行时间
	addSubSetting.constSpeedCnt = 0;								// 匀速运动时间
}

// 检查上位机发来的数据是否有误，暂时没写
u8 checkUartData(void)
{
	return 1;
}

// 设置加减速的每步的运行时间
u8 setAddTime(void)
{
	// 算法未定，暂时返回 addtime
	return addtime;
}

// 释放动态数组
void addSubSetting_Free(void)
{
	addSubSetting.addCount = 0;
	if(NULL != addSubSetting.clkPerStep)
	{
		free(addSubSetting.clkPerStep);
		addSubSetting.clkPerStep = NULL;
	}
	addSubSetting.addSubTimePerStep = 0;
	addSubSetting.constSpeedCnt = 0;
}

// 最大转速对应的轴 如果两个轴都是最大频率，排号靠后的优先
void nAxisMaxClkNum(const u16 maxClk)
{
	if(maxClk == xAxisClk)
		maxClkNum = 0;
	else if(maxClk == yAxisClk)
		maxClkNum = 1;
	else if(maxClk == zAxisClk)
		maxClkNum = 2;
	else if(maxClk == aAxisClk)
		maxClkNum = 3;
	else if(maxClk == bAxisClk)
		maxClkNum = 4;
	else 
		maxClkNum = 5;							// 5表示有误
}
// 获得最大频率
u16 preProcMaxClk_Cur(void)
{
	u16 maxClk;
	maxClk = max(yAxisClk,xAxisClk);
	maxClk = max(maxClk,zAxisClk);
	maxClk = max(maxClk,aAxisClk);
	maxClk = max(maxClk,bAxisClk);
	nAxisMaxClkNum(maxClk);					// 同时计算出哪个轴是最大频率
	return maxClk;
}

// 获得最大脉冲数
u16 preProcMaxNum_Cur(void)
{
	u16 maxPlusNum;
	maxPlusNum = max(yAxisPlusNum,xAxisPlusNum);
	maxPlusNum = max(maxPlusNum,zAxisPlusNum);
	maxPlusNum = max(maxPlusNum,aAxisPlusNum);
	maxPlusNum = max(maxPlusNum,bAxisPlusNum);
	return maxPlusNum;
}

// 获得最小频率
u16 preProcMinClk_Cur(void)
{
	u16 minClk = 0;
	if((0 != yAxisClk) && (0 != xAxisClk))
		minClk = min(yAxisClk,xAxisClk);
	if(0 != zAxisClk)
		minClk = min(minClk,zAxisClk);
	if(0 != aAxisClk)
		minClk = min(minClk,aAxisClk);
	if(0 != bAxisClk)
		minClk = min(minClk,bAxisClk);
	return minClk;
}

// 获得最小脉冲数
u16 preProcMinNum_Cur(void)
{
	u16 minPlusNum = 0;
	if((0 != yAxisClk) && (0 != xAxisClk))
		minPlusNum = min(yAxisPlusNum,xAxisPlusNum);
	if(0 != zAxisClk)
		minPlusNum = min(minPlusNum,zAxisPlusNum);
	if(0 != aAxisClk)
		minPlusNum = min(minPlusNum,aAxisPlusNum);
	if(0 != bAxisClk)
		minPlusNum = min(minPlusNum,bAxisPlusNum);
	return minPlusNum;
}
