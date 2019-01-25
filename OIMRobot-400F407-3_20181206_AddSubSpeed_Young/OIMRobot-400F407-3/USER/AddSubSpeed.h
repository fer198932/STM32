#ifndef __ADDSUBSPEED_H
#define __ADDSUBSPEED_H 

#include "sys.h"
#include <stdlib.h>
#include "pwmint.h"
#include "exti.h"
#include "ComDataProc.h"
#include "timer.h"
#include "string.h"
#include "OutDataProc.h"

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

#define TIMER_CLK 84e6									// 定时器时钟，84MHz
#define ARR_VAL 560											// 自动重装值 可以调整，但最好能被TIMER_CLK整除
#define PSC_CLK (TIMER_CLK / ARR_VAL) 	// 时钟预分频数转化系数
#define ADDTIME 100											// 在该时间内要完成加速或减速的过程，单位ms 此时TIM7为10us

#define PLUS_WIDTH 20										// 脉冲宽度，单位为us	byYJY

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

// 外部变量
extern u8 maxClkNum;
extern u16 max_Clk;
//extern u32 savePlusNum[5];		// 用来保存加减速时 加速阶段走过的脉冲数
extern u16 minSpeedTimeCnt[5];		// 低速运行的计数器，单位为addtime。 0-4:x、y、z、a、b

// 保存各个轴设定的运动参数的结构体
typedef struct 
{
	u8 addCount;						// 加减速总共包括多少步
	u8 addSubTimePerStep;		// 加减速的时间，默认为addtime=40ms
	u16 constSpeedCnt;			// 匀速运动的持续时间，为addSubTimePerStep的倍数
	u16 *clkPerStep;				// 存放每一步频率的动态数组，注意释放内存
	
} AddSubSetting;					// 某个轴设定的运动参数

// 计算每次步进的频率
void calClkPerStep(void);
// 通过CLK频率计算预分频系数 PSC
u16 calPSC(u16 clk);

// 频率调整，使得频率位于合适范围内。0：调整ok，1：有轴运行在最低频率以下  byYJY
u8 adjustClk(u16 maxClk, u16 minClk);

// 是否可以升频的判断 0:不能， 1： 可以
u8 ifOverClkOK(u16 maxClk);
// 降频，有精度丢失
void underClk(void);
// 升频，速度太慢时使用，优先级低
void overClk(void);

// 设置加减速的每步的运行时间
u8 setAddTime(void);

// 没有加减速
void noAddSubSpeed(void);

// 根据频率设置电机速度
extern void xAxisClkSet(u16 xAxisClk);
extern void xAxisClkSet_Cur(u16 xAxisClk, u16 xAxisPsc);
extern void yAxisClkSet(u16 xAxisClk);
extern void zAxisClkSet(u16 xAxisClk);
extern void aAxisClkSet(u16 xAxisClk);
extern void bAxisClkSet(u16 xAxisClk);

// 获得最大频率
u16 preProcMaxClk_Cur(void);
// 获得最大脉冲数
u16 preProcMaxNum_Cur(void);
// 获得最小频率
u16 preProcMinClk_Cur(void);
// 获得最小脉冲数
u16 preProcMinNum_Cur(void);

// 计算占空比的函数
u16 calDutyCycle(u16 clk);

// 各轴初始化
void nAxisInit(void);
// X轴初始化
void xAxisInit(void);
// Y轴初始化
void yAxisInit(void);
// Z轴初始化
void zAxisInit(void);
// A轴初始化
void aAxisInit(void);
// B轴初始化
void bAxisInit(void);

// 计算每次发送脉冲所需要的时间_ 计数， time单位为us，所以有1e6
u16 calPWM_TimeCnt(u16 clk, u16 time);

// 各轴启动
void nAxisStart(void);

// 加减速定时器中断服务程序
void addSubTime_IRQ(void);
void xAddSubTime_IRQ(void);
void yAddSubTime_IRQ(void);
void zAddSubTime_IRQ(void);
void aAddSubTime_IRQ(void);
void bAddSubTime_IRQ(void);

// 计算减速阶段需要的脉冲数(浮点数)
u16 calSubPlusCnt(u8 nAxis, u16 nAxisClk);

// 计算最低频率需要的运行时间（计数值） 
// 0：无频率， 1：最大频率， 其它值：小于最大速度的中间频率
void calMinSpeedTimeCnt(void);

// 修正同步的参数
u16 correctSync(u16 nAxisMinClk, u8 nAxis);

// 修正不加减速的轴的频率
void correctNoAddSubClk(u8 nAxis, u32 plusNum);

// 估算一帧需要的时间， 单位us
float calTotalTime(u8 nAxis);

// 收到串口消息回复 0：ok， 1：not ok
void respMsg(u8 status);

#endif
