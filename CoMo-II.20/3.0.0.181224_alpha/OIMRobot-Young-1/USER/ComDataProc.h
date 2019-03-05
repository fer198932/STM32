#ifndef _COMDATAPROC_H
#define _COMDATAPROC_H
/*  数据处理程序 */
#include "sys.h"
#include "usart.h"
#include "config.h"
#include "main.h"
#include "command.h"
#include "buffer.h"
#include "OffLine_Data.h"


#define  DATA_LENGTH_POSITION 	3 			// 数据长度对应的位置

typedef enum { IS_OK = 0, NOT_OK = !IS_OK} IfOK_Status;





/* 回复上位机的消息的结构体 */
#define 	RespMsg_SELFCHECK_LENGTH							8
#define		RespMsg_MOTION_LENGTH 								9
#define		RespMsg_LIMIT_LENGTH									9
#define		RespMsg_UrgentStopDown_Length					23
#define		RespMsg_UrgentStopUp_Length						9

typedef struct {
	u8 		respMsg_SelfCheck[RespMsg_SELFCHECK_LENGTH];
	u8 		respMsg_Motion[RespMsg_MOTION_LENGTH];
	u8 		respMsg_Limit[RespMsg_LIMIT_LENGTH];
	u8 		respMsg_UrgentStopDown[RespMsg_UrgentStopDown_Length];
	u8 		respMsg_UrgentStopUp[RespMsg_UrgentStopUp_Length];
} RespMsgArray;



// 位置游标
typedef struct {
	u16 start;
	u16 end;
} PosCur;

// 脉冲数据结构体 
typedef struct {
	u32 				plusNum[AXIS_NUM];				// 脉冲数
	u32 				clk[AXIS_NUM];						// 频率：决定了运动速度
	Motor_Dir	 	dir[AXIS_NUM];						// 电机运动方向
} Plus_Data;

// 从串口得到的处理后的命令
typedef struct {
	u8 						cmd_Type;  						// 命令类型、帧标识 如：#define SELFCHECK 		0x0B     //自检
	u8 						cmd_Excute;						// 需要执行的命令、指令码
	u32 					cmd_Value;						// 命令取值
//	u32			offline_length;						// 脱机加工数据的长度
	
	u8 						resp_Excute;					// 回复指令码 通常resp_Excute=cmd_Excute
	u8 						resp_Status;					// 回复状态
//	void* 	cmd_Data; 								// 指向命令的数据结构体，脉冲数据等情况下需要用到 暂时没用到，慎用！
	Plus_Data			plus_Datas;						// 脉冲运动参数
} Proc_Data; 



/* 加减速相关的结构体  */
#define 	DATA_LENGTH				1024						// 存放加减速psc值的空间长度，后续可考虑转为动态数组

typedef enum {ADD_SPEED = 0, CONST_SPEED = 1, SUB_SPEED = 2} AddSubSpeedStatus;

// 保存PSC值的结构体
typedef struct {
	u16 							length;
	u16 							psc_data[DATA_LENGTH];
	FunctionalState		enAddSubFlag; 												// 使能加减速的开关
	float 						addSpeed_NeedPlusNum; 			// 加速阶段需要的脉冲数
} PSC_Data_Array;

// 保存初始化后的运动数据的结构体 初始化时默认为0
typedef struct {
	PSC_Data_Array 	PSC_Data[AXIS_NUM];
	
	// 加减速状态标记
	AddSubSpeedStatus addSubSpeed_Status[AXIS_NUM];
	
	float 			addSubTime;											// 每步加减速的时间 即ADDSUB_TIMER的周期时间 单位us
	u32 				maxClk;
	u32 				minClk;
	u32 				nAxisClk_Cur[AXIS_NUM];					// 当前频率
	u8 					maxClkNum;											// 最大频率对应的轴 等于AXIS_NUM表示有误或未设置	

	Proc_Data		cmd_Datas;											// 命令数据
} Motion_Strcuture;

/* 加减速相关的结构体  */


// 命令数据结构体的初始化
void comData_Init(void);

// 从缓冲区得到数据并进行处理、启动相关电机的程序
void UsartDataProc(void);

// 处理脉冲数据的函数，帧标识0X0D、指令码0X22
static void plus_Data_Proc(PosCur posCur);

// 脱机加工 帧标识0X0D、指令码0X25
static void offline_Data_Proc(PosCur posCur);

// 处理对应区间上的缓冲区数据
static IfOK_Status bufData_Proc_Region(PosCur posCur);

// 检查从缓冲区读取的数据是否合格
static IfOK_Status bufData_Proc(void);

// 得到命令设定的数据长度
static u16 getSetDataSize(PosCur posCur);

// 得到命令设定的脉冲数 shift：偏移量 0-4 对应5轴
static u32 getSetDataPlusNum(PosCur posCur, u8 shift);

// 得到命令设定的频率 shift：偏移量 0-4 对应5轴
static u32 getSetDataClk(PosCur posCur, u8 shift);

// 得到命令设定的方向 shift：偏移量 0-4 对应5轴
static u8 getSetDataDir(PosCur posCur, u8 shift);

// 检查OK后，设定命令数据的结构体
static IfOK_Status setCmdData(PosCur posCur);

// 设置回复字符串的格式
void setRespStr(Proc_Data* pCmd, u8 resStr[], u16 length);

// 回复上位机的程序 异常或错误
void respMsgError(const char str[], u8 status);

// 回复串口数据信息
void respUsartMsg(const u8 backResString[], u16 length);

// 设定运动数据的串口反馈数组的格式
void setRespStr_Motion(Proc_Data* pCmd, u8 respStr[], u16 length, u8 status);


// 自检消息的反馈数组
void setRespStr_SlefCheck(Proc_Data* pCmd, u8 respStr[], u16 length, u8 status);

// 急停消息的反馈数组设置
void setRespStr_UrgentStop(u8 respStr[], u16 length, u8 status);










#endif
