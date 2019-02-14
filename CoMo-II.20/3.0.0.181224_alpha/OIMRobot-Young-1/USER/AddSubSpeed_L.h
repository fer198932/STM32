/********************************************************************************************************
*   ֱ���ͼӼ����㷨 2019��2��10��18:45:08 byYJY
********************************************************************************************************/

#ifndef __ADDSUBSPEED_L_H
#define __ADDSUBSPEED_L_H 

#include "sys.h"
#include "config.h"
#include "ComDataProc.h"
#include "StepMotor.h"

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

// �Ӽ��ٶ�ʱ���жϷ������  ���Ǹĳ�����������궨�壬�������Ч��  byYJY
void addSubTime_IRQ(void);

// ����ļӼ���-��ʱ���жϷ������
static void nAddSubTime_IRQ(u8 nAxis);

// �趨�Ӽ��ٵ�����˶�����
static void	setSubAddSpeedData(void);

// ������Ƶ��
static u32 preProcMaxClk(void);

// ��÷������СƵ�� �������ֵ�ٵ���
static u32 preProcMinClk(u32 max_Clk);

// �����ʼ��
static void nAxisInit(void);

// ָ����ĳ�ʼ��
static void n_Axis_Init(u8 nAxis);

// �趨����
static void	setStepMotorDir(void);

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

// ��������Ƶ�������Ƶ������Ҫ���е�ʱ�䣨����ֵ��
// 0����Ƶ�ʣ� 0�����Ƶ�ʣ� ����ֵ��С������ٶȵ��м�Ƶ��
static void calMinSpeedTimeCnt(u8 nAxis);

// ����ÿ�η�����������Ҫ��ʱ��_ ������ time��λΪus��������1e6
static u16 calPWM_TimeCnt(u16 clk, u16 time);

// �޸��˷���1�����򷵻�0
static u8 if_PSC_Modify(u8 nAxis, u16 psc);

// ������ٽ׶���Ҫ��������(������)
static u32 calSubPlusCnt(u8 nAxis, u16 nAxisClk);

// ����ͬ���Ĳ���
static u16 correctSync(u32 nAxisMinClk, u8 nAxis);

// �������Ӽ��ٵ����Ƶ��
static void correctNoAddSubClk(u8 nAxis, u32 plusNum);

// ����һ֡��Ҫ��ʱ�䣬 ��λus
static float calTotalTime(u8 nAxis);

#endif
