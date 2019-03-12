#ifndef _RETURN2ZERO_H
#define _RETURN2ZERO_H

/********************************************************************************************************
* ������� 
* 2019��3��8��09:29:46 byYJY
*
*
********************************************************************************************************/

#include "sys.h"
#include "config.h"
#include "StepMotor.h"

// ����г̣���λmm
#define		MAX_DIS			200
#define		MAX_CLK			(StepMotor_MaxClk>>1)
#define		MIN_DIS			1				// ��С�Ľ�����




// �˶�Ŀ�ĵأ���ŵ�������
typedef struct {
	u32						posPlusNum[AXIS_NUM];
	Motor_Dir			posDir[AXIS_NUM];
} DesPosition;




// ��е��㸴λ������ 
void return2Zero(void);

// ָ����ӹ���λ���ƶ�����λ��
static void workPos2Limit(N_Axis n, u32 _clk, Motor_Dir _dir, u8 (*fuc_limit)());

// ����㵽ָ��λ�ã����趨��ֵ�˶�
void zeroBack2Pos(DesPosition* pDesPos);


























#endif
