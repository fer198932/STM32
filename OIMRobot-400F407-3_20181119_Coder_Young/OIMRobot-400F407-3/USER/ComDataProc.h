#ifndef __COMDATAPROC_H
#define __COMDATAPROC_H 

#include "sys.h"

#define SET_DATA_TEST 1			// 1：开启运动参数的设置，0：关闭

// 保存各个轴设定的运动参数的结构体
typedef struct 
{	
	u32 nAxisPlusNum;				// 设定的脉冲数，决定了运行距离
	u16 nAxisClk;						// 设定的最高频率，也是匀速部分的频率
	u16 nDir;								// 设定的方向
	
	u16 addSubClkSet[9];		// 最多分为7段，保存每段设定的设置频率，按9保存了看看
	u16 subLastTime;				// 最后一段（减速部分）跑的时间
	u16 constTime;					// 匀速运行的时间，是40ms的倍数
} nAxisSetData;		// 某个轴设定的运动参数

// 完整抓取设定的运动参数的结构体 - X轴
typedef struct 
{
	u8 nAxisFlag; 							// 标记当前是哪一轴的数据
	u8 incClkDataFlag;					// 递增来记录CLK数据的标记
	
	u32 nAxisPlusNum;						// 设定的脉冲数，决定了运行距离
	u16 nAxisClk;								// 设定的最高频率，也是匀速部分的频率
	u16 nDir;										// 设定的方向
	
	u16 setClk[16];							// 保存设定的频率
	u16 setClkTime[16];					// 设定频率的运行时间，以40ms为单位
	
	u16 subLastTime;				// 最后一段（减速部分）跑的时间
	
} SetDataPerAxis;

extern void selfCheckFunc(void);
//

#endif
