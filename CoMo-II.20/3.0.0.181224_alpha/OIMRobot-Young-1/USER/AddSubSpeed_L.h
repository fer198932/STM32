/********************************************************************************************************
*   直线型加减速算法 2019年2月10日18:45:08 byYJY
********************************************************************************************************/

#ifndef __ADDSUBSPEED_L_H
#define __ADDSUBSPEED_L_H 

#include "sys.h"
#include "config.h"
#include "ComDataProc.h"
#include "StepMotor.h"

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

// 加减速定时器中断服务程序  考虑改成内联函数或宏定义，提高运行效率  byYJY
void addSubTime_IRQ(void);

// 各轴的加减速-定时器中断服务程序
static void nAddSubTime_IRQ(u8 nAxis);

// 设定加减速的相关运动参数
static void	setSubAddSpeedData(void);

// 获得最大频率
static u32 preProcMaxClk(void);

// 获得非零的最小频率 先求最大值再调用
static u32 preProcMinClk(u32 max_Clk);

// 各轴初始化
static void nAxisInit(void);

// 指定轴的初始化
static void n_Axis_Init(u8 nAxis);

// 设定方向
static void	setStepMotorDir(void);

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

// 计算非最大频率在最低频率下需要运行的时间（计数值）
// 0：无频率， 0：最大频率， 其它值：小于最大速度的中间频率
static void calMinSpeedTimeCnt(u8 nAxis);

// 计算每次发送脉冲所需要的时间_ 计数， time单位为us，所以有1e6
static u16 calPWM_TimeCnt(u16 clk, u16 time);

// 修改了返回1，否则返回0
static u8 if_PSC_Modify(u8 nAxis, u16 psc);

// 计算减速阶段需要的脉冲数(浮点数)
static u32 calSubPlusCnt(u8 nAxis, u16 nAxisClk);

// 修正同步的参数
static u16 correctSync(u32 nAxisMinClk, u8 nAxis);

// 修正不加减速的轴的频率
static void correctNoAddSubClk(u8 nAxis, u32 plusNum);

// 估算一帧需要的时间， 单位us
static float calTotalTime(u8 nAxis);

#endif
