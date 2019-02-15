/********************************************************************************************************
*    S型加减速算法 2019年2月13日09:02:11 byYJY
********************************************************************************************************/

#ifndef __ADDSUBSPEED_S_H
#define __ADDSUBSPEED_S_H 

#include "sys.h"
#include <math.h>
#include "pwm.h"

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


#define 	S_FLEXIBLE 				6								// S曲线的压缩情况，越大曲线越斜，越小越接近匀加速， 理想取值4-6
#define 	DATA_LENGTH				1024						// 存放加减速psc值的空间长度，后续可考虑转为动态数组
#define 	AddSub_Time_MAX		200							// 加减速阶段最长的时间，单位ms，对应频率从0~10000的情况
#define		AddSub_Step_DIV		((float)10)			// 最大频率按该值进行分段 如0~10000时，即分为1000段

#define 	MIN_STEP_NUM			((float)5.0) 		// 小于该值的步数认为不合理

typedef enum {ADD_SPEED = 0, CONST_SPEED = 1, SUB_SPEED = 2} AddSubSpeedStatus;

// 保存PSC值的结构体
typedef struct {
	u16 							length;
	u16 							psc_data[DATA_LENGTH-1];
	FunctionalState		enAddSubFlag; 												// 使能加减速的开关
	float 						addSpeed_NeedPlusNum; 			// 加速阶段需要的脉冲数
} PSC_Data_Array;


/* 	计算S型加速曲线的函数 参考：https://blog.csdn.net/weixin_40567368/article/details/81742938
*  	fre: 频率曲线-频率值，暂时保存
*  	period[] : 定时器装载的PSC值
*	 	len : 加减速的步数
*  	fre_max : 结束时候的频率
*		fre_min : 起始频率
* 	flexible : S曲线的压缩情况，越大曲线越斜，越小越接近匀加速， 理想取值4-6
*/
static void calSModelLine(float fre, u16 period[], float len, 
	float fre_max, float fre_min, float flexible);

// 加减速相关参数设置的初始化
void AddSubSpeed_Init(void);

// 设定运动方向
static void	setStepMotorDir(void);

// 频率调整，使得频率位于合适范围内。0：调整ok，1：有轴运行在最低频率以下
static u8 adjustClk(void);

// 获得最大频率
static u32 preProcMaxClk(void);

// 获得非零的最小频率 先求最大值再调用
static u32 preProcMinClk(u32 max_Clk);

// 降频，有精度丢失
static void reduceClk(void);
// 升频，速度太慢时使用，优先级低
static void boostClk(void);

// 返回当前轴允许的最大、最小频率
static u32 n_Axis_Max_Clk(u8 i);
static u32 n_Axis_Min_Clk(u8 i);

// 是否可以升频的判断 
static FunctionalState ifBoostClkOK(void);

// 计算加减速如何分段
static void cal_AddSubSpeed_Div(void);

// 各轴计算S型加速曲线
static void cal_S_Line(void);


// 计算当前曲线下，完成加速所需要的脉冲数
// 关注文章： https://blog.csdn.net/gtkknd/article/details/52199963 byYJY
static float calAddSpeed_NeedPlusNum(u16 period[], float len);

// 估算指定轴运行所需要的时间 单位us
static float calTotalNeedTime(u8 nAxis);

// 重置相关运动参数
static void resetMotionData(void);

// 各轴的运动状态初始化
static void	nAxisMotion_Init(void);



#endif
