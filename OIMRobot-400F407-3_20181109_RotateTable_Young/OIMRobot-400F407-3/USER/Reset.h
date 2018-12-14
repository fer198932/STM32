#ifndef __RESET_H
#define __RESET_H 

#include "sys.h"
#include "ChangeTools.h"

#define RESET_DEBUG			// debug 模式下，再次点击复位按钮会返回起始点，其余模式下（注释掉）仅回零

#ifdef RESET_DEBUG
extern u32 xAxisPlusNum,yAxisPlusNum,zAxisPlusNum,aAxisPlusNum,bAxisPlusNum;
extern void toolsFromORG2Start(void);	// 从零点返回原来工件的位置
extern ChangeToolsSavedSteps changeToolsSavedSteps;

extern u8 xAxisStepFlag; 
extern u8 yAxisStepFlag; 
extern u8 zAxisStepFlag; 
extern u8 aAxisStepFlag; 
extern u8 bAxisStepFlag; 
#endif

// 主轴停止转动
extern void mainMotorStop(void);

extern void delay_s(u8 delay);

// 保存运动数据的结构体
extern ChangeToolsSavedSteps stepsTemp;

// 能计步数的回到原点
extern void axis3Back2ORGWithCnt(void);

// 将编号的temp数据进行保存：0：加工点到零点 1-4：零点到刀具
extern void dataFromTemp2Saved(u8 num);

// 回复给串口
void respUartMsg(void);

extern unsigned char respFlag;		// 是否回复的标记， 0：回复  1：不回复
#endif
