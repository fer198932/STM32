#ifndef __CHANGETOOLS_H
#define __CHANGETOOLS_H 

#include "delay.h"
#include "sys.h"
#include "SysInit.h"

#define TEST_STEPSNUM 0			// 1: ���Բ����� 0: ȡ����������

#define X_AXIS_RST_CLK 5333			// ����Ƶ�ʣ�8ϸ����5333��ӦF800
#define Y_AXIS_RST_CLK 5333			// ����Ƶ�ʣ�8ϸ����5333��ӦF800
#define Z_AXIS_RST_CLK 5333			// ����Ƶ�ʣ�8ϸ����5333��ӦF800

// #define IF_ADDSUB_WORK_CHANGETOOLS					// ���ƼӼ����Ƿ����ĺ꣬ע�ͺ�Ӽ��ٲ����������޻���������

void delay_s(u8 delay);

// �������߲����Ľṹ��
typedef struct
{
	u32 xSteps[5];			// [0]:�ӹ���������߹��ľ��룻 [1-4]:����㵽�����߹��ľ��룬�豣�沢�ϴ�����λ��
	u32 ySteps[5];
	u32 zSteps[5];
} ChangeToolsSavedSteps;

// ��������Ƶ��
extern u32 xAxisPlusNum,yAxisPlusNum,zAxisPlusNum,aAxisPlusNum,bAxisPlusNum;
extern u16 xAxisClk,yAxisClk,zAxisClk,aAxisClk,bAxisClk;

extern u8 addSubStepNo;
extern u32 workedTimeMS;
extern u8 addcount;

void yAxisUpNStep(u32 steps);

// �ֶ�ʽ�Ӽ���-�����������ã� nAxis ��ʾĳһ�� 0-x, 1-y, 2-z
void setSpeedRST_Seg(u16 clk, u8 nAxis);

// ����ʽ�Ӽ���-������������


#endif
