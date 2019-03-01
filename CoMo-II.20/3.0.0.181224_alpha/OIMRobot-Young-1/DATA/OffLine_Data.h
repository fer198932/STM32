#ifndef _BAJIAOXING_ROUGH_H
#define _BAJIAOXING_ROUGH_H

#include "sys.h"
#include "AddSubSpeed_S.h"

#define LENGTH_OFFL 1

// 暂时三轴
extern const s32 plusNum_OFFL[LENGTH_OFFL][3];

extern const u32 clk_OFFL[LENGTH_OFFL][3];


// 脱机加工
void offlineWork(u32 num);

// 运动数据设置
static void offl_Data_Set(u32 num);

#endif

