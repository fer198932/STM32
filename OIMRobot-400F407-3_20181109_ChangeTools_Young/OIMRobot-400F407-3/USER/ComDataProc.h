#ifndef __COMDATAPROC_H
#define __COMDATAPROC_H 

#include "sys.h"
#include "Reset.h"

#define SET_DATA_TEST 1			// 1：开启运动参数的设置，0：关闭

#define IF_ADDSUB_WORK_COMDATAPROC		// 是否进行加减速运动的判断，注释后不加减速

// 保存各个轴设定的运动参数的结构体
typedef struct 
{
	u32 nAxisPlusNum;				// 设定的脉冲数，决定了运行距离
	u16 nAxisClk;						// 设定的最高频率，也是匀速部分的频率
	
	u16 addSubClkSet[10];		// 最多分为7段，保存每段设定的设置频率，按10保存了看看
	u16 subLastTime;				// 最后一段（减速部分）跑的时间
	u16 constTime;					// 匀速运行的时间，是40ms的倍数
} nAxisSetData;		// 某个轴设定的运动参数

void preProcMaxNum(void);
u8 preProcStep(void);
void addSubFlagSet(void);
void addSubSpeed(void);
void motor5workstart(void);

extern void selfCheckFunc(void);

// 机械零点复位主程序
extern void reset2Zero();

extern u8 resetStatusFlag;

// 仅限测试重复定位精度时候用
void rs232DataProcStep3(void);

#ifdef RESET_DEBUG
// 从零点到开始位置，debug模式下才开启
extern void zero2Start(void);
extern u8 resetStatusFlag;			// 0:回零， 1：到起始点
#endif

#endif
