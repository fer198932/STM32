#ifndef _COMDATAPROC_H
#define _COMDATAPROC_H
/*  数据处理程序 */
#include "sys.h"
#include "usart.h"
#include "config.h"
#include "main.h"
#include "command.h"
#include "buffer.h"


typedef enum { IS_OK = 0, NOT_OK = !IS_OK} IfOK_Status;


// 串口接收一帧数据完成的标志
extern	 volatile 	IfOK_Status 	USARTRx_IfOK_Flag;

// 串口DMA数据溢出的标志
extern volatile 	ErrorStatus 	USARTRx_DMAOut_Flag;

// 数据是否处理完毕
// extern	volatile	IfOK_Status 	DataPro_IsOK_Flag;

// 位置游标
typedef struct {
	u16 start;
	u16 end;
} PosCur;

// 从串口得到的处理后的命令
typedef struct {
	u8 cmd_Type;  					// 命令类型、帧标识 如：#define SELFCHECK 		0x0B     //自检
	u8 cmd_Excute;					// 需要执行的命令、指令码
	u8 cmd_Value;						// 命令取值
	
	u8 resp_Excute;					// 回复指令码 通常resp_Excute=cmd_Excute
	u8 resp_Status;					// 回复状态
	void* cmd_Data; 				// 指向命令的数据结构体，脉冲数据等情况下需要用到
} Proc_Data; 

// 脉冲数据结构体 
typedef struct {
	u32 				plusNum[AXIS_NUM];				// 脉冲数
	u32 				clk[AXIS_NUM];						// 频率：决定了运动速度
	Motor_Dir	 	dir[AXIS_NUM];						// 电机运动方向
} Plus_Data;

// 命令数据结构体的初始化
void comData_Init(void);

// 从缓冲区得到数据并进行处理、启动相关电机的程序
void UsartDataProc(void);

// 处理对应区间上的缓冲区数据
static IfOK_Status bufData_Proc_Region(PosCur posCur);

// proc结构体指向对应数据结构体
static void proc_pt_motionData(void* _data);

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
void setRespStr(u8 resStr[], u16 length);

// 回复上位机的程序 异常或错误
void respMsgError(const char str[], u8 status);

// 回复串口数据信息
void respUsartMsg(const u8 backResString[], u16 length);

#endif
