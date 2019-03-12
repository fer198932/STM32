#ifndef _CHANGETOOL_H
#define _CHANGETOOL_H

/********************************************************************************************************
* 换刀程序
* 2019年3月8日09:27:59 byYJY
********************************************************************************************************/

#include "sys.h"
#include "config.h"
#include "Return2Zero.h"


#define		TOOLS_QTY					(4+1)				// 四把刀，还需要0来表示没有刀具
#define		TOOL_OFFSET_X			20					// 刀具在X轴上的偏移量，单位mm
#define		TOOL_OFFSET_Y			20					// 刀具在Y轴上的偏移量，单位mm
#define		DISTANCE_Z				10					// 主轴缓转前，Z轴到刀具的距离，单位mm


typedef	enum {Assmble_Tool = 0, Clean_Tool = 1 } ChangeTool_Mode;
typedef	enum {Clamp = 0, UN_Clamp = 1 } Clamp_Mode;

// 刀具位置信息的结构体
typedef struct {
	u8						tool_Num;
	u32					 	tool_Pos[3];			// X、Y、Z轴可以定位刀具位置，脉冲数
} ToolPos;


// 脱刀程序
static ErrorStatus cleanTool(u8 toolNo);

// 装刀程序
static ErrorStatus assemTool(u8 toolNo);

// 刀具初始化，确定刀具编号、刀具位置等信息
void Tool_Init(void);

// 从上位机获得刀具位置信息
static void getToolPosFromPC(ToolPos tool_pos[]);

// 校正刀具位置，默认为刀具1位置
void calibrateToolPos(void);

// 运动到刀具位置
static void move2ToolPos(u8 toolNo);

// 换刀程序， mode：0-脱刀，1-装刀
void changeTools(u8 toolNo, ChangeTool_Mode mode);

// 刀具电磁铁IO口初始化
static void tools_IO_Init(void);

// 刀具电磁铁控制
static void tools_ElecMag_Ctrl(Clamp_Mode mode);

























#endif
