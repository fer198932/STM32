#ifndef __CHANGETOOLS_H
#define __CHANGETOOLS_H 

#include "delay.h"
#include "sys.h"
#include "SysInit.h"

#define TEST_STEPSNUM 0			// 1: 测试步数； 0: 取消步数测试

void delay_s(u8 delay);

// 保存行走步数的结构体
typedef struct
{
	u32 xSteps[5];			// [0]:从工件到零点走过的距离； [1-4]:从零点到刀具走过的距离，需保存并上传到上位机
	u32 ySteps[5];
	u32 zSteps[5];
} ChangeToolsSavedSteps;

#endif