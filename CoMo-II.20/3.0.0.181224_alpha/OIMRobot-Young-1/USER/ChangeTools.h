#ifndef _CHANGETOOL_H
#define _CHANGETOOL_H

/********************************************************************************************************
* ��������
* 2019��3��8��09:27:59 byYJY
********************************************************************************************************/

#include "sys.h"
#include "config.h"
#include "Return2Zero.h"


#define		TOOLS_QTY					(4+1)				// �İѵ�������Ҫ0����ʾû�е���
#define		TOOL_OFFSET_X			20					// ������X���ϵ�ƫ��������λmm
#define		TOOL_OFFSET_Y			20					// ������Y���ϵ�ƫ��������λmm
#define		DISTANCE_Z				10					// ���Ỻתǰ��Z�ᵽ���ߵľ��룬��λmm


typedef	enum {Assmble_Tool = 0, Clean_Tool = 1 } ChangeTool_Mode;
typedef	enum {Clamp = 0, UN_Clamp = 1 } Clamp_Mode;

// ����λ����Ϣ�Ľṹ��
typedef struct {
	u8						tool_Num;
	u32					 	tool_Pos[3];			// X��Y��Z����Զ�λ����λ�ã�������
} ToolPos;


// �ѵ�����
static ErrorStatus cleanTool(u8 toolNo);

// װ������
static ErrorStatus assemTool(u8 toolNo);

// ���߳�ʼ����ȷ�����߱�š�����λ�õ���Ϣ
void Tool_Init(void);

// ����λ����õ���λ����Ϣ
static void getToolPosFromPC(ToolPos tool_pos[]);

// У������λ�ã�Ĭ��Ϊ����1λ��
void calibrateToolPos(void);

// �˶�������λ��
static void move2ToolPos(u8 toolNo);

// �������� mode��0-�ѵ���1-װ��
void changeTools(u8 toolNo, ChangeTool_Mode mode);

// ���ߵ����IO�ڳ�ʼ��
static void tools_IO_Init(void);

// ���ߵ��������
static void tools_ElecMag_Ctrl(Clamp_Mode mode);

























#endif
