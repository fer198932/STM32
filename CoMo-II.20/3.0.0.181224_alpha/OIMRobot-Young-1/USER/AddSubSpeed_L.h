/********************************************************************************************************
*   直线型加减速算法 2019年2月10日18:45:08 byYJY
********************************************************************************************************/

#ifndef __ADDSUBSPEED_L_H
#define __ADDSUBSPEED_L_H 

#include "sys.h"
#include "config.h"
#include "ComDataProc.h"

// 设置各轴的最合适频率：该频率下运动误差最小，需标定后确定
#define X_AXIS_MIN_CLK 		500
#define Y_AXIS_MIN_CLK 		500
#define Z_AXIS_MIN_CLK 		500
#define A_AXIS_MIN_CLK 		500
#define B_AXIS_MIN_CLK 		500

#define X_AXIS_MAX_CLK 		10000
#define Y_AXIS_MAX_CLK 		10000
#define Z_AXIS_MAX_CLK 		10000
#define A_AXIS_MAX_CLK 		10000
#define B_AXIS_MAX_CLK 		10000


// 加减速算法的初始化程序 系统运动前调用
void addSubSpeed_L_Init(void);

// 获得最大频率
static u32 preProcMaxClk(void);

// 获得非零的最小频率 先求最大值再调用
static u32 preProcMinClk(u32 max_Clk);

// 频率调整，使得频率位于合适范围内。0：调整ok，1：有轴运行在最低频率以下
static u8 adjustClk(void);

// 返回当前轴允许的最大、最小频率
static u32 n_Axis_Max_Clk(u8 i);
static u32 n_Axis_Min_Clk(u8 i);

// 降频，有精度丢失
static void reduceClk(void);
// 升频，速度太慢时使用，优先级低
static void boostClk(void);

// 是否可以升频的判断 
static FunctionalState ifBoostClkOK(void);



#endif
