#ifndef _RETURN2ZERO_H
#define _RETURN2ZERO_H

/********************************************************************************************************
* 回零程序 
* 2019年3月8日09:29:46 byYJY
*
*
********************************************************************************************************/

#include "sys.h"
#include "config.h"
#include "StepMotor.h"

// 最大行程，单位mm
#define		MAX_DIS			100
#define		MIN_DIS			1				// 较小的进给量




// 运动目的地，存放的脉冲数
typedef struct {
	u32						posPlusNum[AXIS_NUM];
	Motor_Dir			posDir[AXIS_NUM];
} DesPosition;




// 机械零点复位主程序 
void return2Zero(void);

// 指定轴从工作位置移动到限位点
static void workPos2Limit(N_Axis n, u32 _clk, Motor_Dir _dir, u8 (*fuc_limit)());

// 从零点到指定位置，按设定的值运动
static void zeroBack2Pos(DesPosition* pDesPos);


























#endif
