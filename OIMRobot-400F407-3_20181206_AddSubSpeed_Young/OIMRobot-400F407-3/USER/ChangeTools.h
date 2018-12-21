#ifndef __CHANGETOOLS_H
#define __CHANGETOOLS_H 

#include "delay.h"
#include "sys.h"
#include "SysInit.h"

#define TEST_STEPSNUM 0			// 1: 测试步数； 0: 取消步数测试

#define X_AXIS_RST_CLK 5333			// 归零频率，8细分下5333对应F800
#define Y_AXIS_RST_CLK 5333			// 归零频率，8细分下5333对应F800
#define Z_AXIS_RST_CLK 5333			// 归零频率，8细分下5333对应F800

// #define IF_ADDSUB_WORK_CHANGETOOLS					// 控制加减速是否工作的宏，注释后加减速不工作，仅限换刀回零用

void delay_s(u8 delay);

// 保存行走步数的结构体
typedef struct
{
	u32 xSteps[5];			// [0]:从工件到零点走过的距离； [1-4]:从零点到刀具走过的距离，需保存并上传到上位机
	u32 ySteps[5];
	u32 zSteps[5];
} ChangeToolsSavedSteps;

// 脉冲数、频率
extern u32 xAxisPlusNum,yAxisPlusNum,zAxisPlusNum,aAxisPlusNum,bAxisPlusNum;
extern u16 xAxisClk,yAxisClk,zAxisClk,aAxisClk,bAxisClk;

extern u8 addSubStepNo;
extern u32 workedTimeMS;
extern u8 addcount;

void yAxisUpNStep(u32 steps);

// 分段式加减速-换刀及回零用， nAxis 表示某一轴 0-x, 1-y, 2-z
void setSpeedRST_Seg(u16 clk, u8 nAxis);

// 连续式加减速-换刀及回零用


#endif
