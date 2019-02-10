/********************************************************************************************************
*   ֱ���ͼӼ����㷨 2019��2��10��18:45:08 byYJY
********************************************************************************************************/

#ifndef __ADDSUBSPEED_L_H
#define __ADDSUBSPEED_L_H 

#include "sys.h"
#include "config.h"
#include "ComDataProc.h"

// ���ø���������Ƶ�ʣ���Ƶ�����˶������С����궨��ȷ��
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


// �Ӽ����㷨�ĳ�ʼ������ ϵͳ�˶�ǰ����
void addSubSpeed_L_Init(void);

// ������Ƶ��
static u32 preProcMaxClk(void);

// ��÷������СƵ�� �������ֵ�ٵ���
static u32 preProcMinClk(u32 max_Clk);

// Ƶ�ʵ�����ʹ��Ƶ��λ�ں��ʷ�Χ�ڡ�0������ok��1���������������Ƶ������
static u8 adjustClk(void);

// ���ص�ǰ������������СƵ��
static u32 n_Axis_Max_Clk(u8 i);
static u32 n_Axis_Min_Clk(u8 i);

// ��Ƶ���о��ȶ�ʧ
static void reduceClk(void);
// ��Ƶ���ٶ�̫��ʱʹ�ã����ȼ���
static void boostClk(void);

// �Ƿ������Ƶ���ж� 
static FunctionalState ifBoostClkOK(void);



#endif
