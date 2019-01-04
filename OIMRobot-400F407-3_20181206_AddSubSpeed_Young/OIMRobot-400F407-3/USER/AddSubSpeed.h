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

// ���ø���������Ƶ�ʣ���Ƶ�����˶������С����궨��ȷ��
#define X_AXIS_MIN_CLK 		500
#define Y_AXIS_MIN_CLK 		500
#define Z_AXIS_MIN_CLK 		500
#define A_AXIS_MIN_CLK 		500
#define B_AXIS_MIN_CLK 		500

#define X_AXIS_MAX_CLK 		6000
#define Y_AXIS_MAX_CLK 		6000
#define Z_AXIS_MAX_CLK 		6000
#define A_AXIS_MAX_CLK 		6000
#define B_AXIS_MAX_CLK 		6000

#define TIMER_CLK 84e6									// ��ʱ��ʱ�ӣ�84MHz
#define ARR_VAL 560											// �Զ���װֵ ���Ե�����������ܱ�TIMER_CLK����
#define PSC_CLK (TIMER_CLK / ARR_VAL) 	// ʱ��Ԥ��Ƶ��ת��ϵ��
#define ADDTIME 100											// �ڸ�ʱ����Ҫ��ɼ��ٻ���ٵĹ��̣���λms ��ʱTIM7Ϊ10us

#define PLUS_WIDTH 20										// �����ȣ���λΪus	byYJY

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((a)<(b)?(a):(b))

// �ⲿ����
extern u8 maxClkNum;
extern u16 max_Clk;
//extern u32 savePlusNum[5];		// ��������Ӽ���ʱ ���ٽ׶��߹���������
extern u16 minSpeedTimeCnt[5];		// �������еļ���������λΪaddtime�� 0-4:x��y��z��a��b

// ����������趨���˶������Ľṹ��
typedef struct 
{
	u8 addCount;						// �Ӽ����ܹ��������ٲ�
	u8 addSubTimePerStep;		// �Ӽ��ٵ�ʱ�䣬Ĭ��Ϊaddtime=40ms
	u16 constSpeedCnt;			// �����˶��ĳ���ʱ�䣬ΪaddSubTimePerStep�ı���
	u16 *clkPerStep;				// ���ÿһ��Ƶ�ʵĶ�̬���飬ע���ͷ��ڴ�
	
} AddSubSetting;					// ĳ�����趨���˶�����

// ����ÿ�β�����Ƶ��
void calClkPerStep(void);
// ͨ��CLKƵ�ʼ���Ԥ��Ƶϵ�� PSC
u16 calPSC(u16 clk);

// Ƶ�ʵ�����ʹ��Ƶ��λ�ں��ʷ�Χ�ڡ�0������ok��1���������������Ƶ������  byYJY
u8 adjustClk(u16 maxClk, u16 minClk);

// �Ƿ������Ƶ���ж� 0:���ܣ� 1�� ����
u8 ifOverClkOK(u16 maxClk);
// ��Ƶ���о��ȶ�ʧ
void underClk(void);
// ��Ƶ���ٶ�̫��ʱʹ�ã����ȼ���
void overClk(void);

// ���üӼ��ٵ�ÿ��������ʱ��
u8 setAddTime(void);

// û�мӼ���
void noAddSubSpeed(void);

// ����Ƶ�����õ���ٶ�
extern void xAxisClkSet(u16 xAxisClk);
extern void xAxisClkSet_Cur(u16 xAxisClk, u16 xAxisPsc);
extern void yAxisClkSet(u16 xAxisClk);
extern void zAxisClkSet(u16 xAxisClk);
extern void aAxisClkSet(u16 xAxisClk);
extern void bAxisClkSet(u16 xAxisClk);

// ������Ƶ��
u16 preProcMaxClk_Cur(void);
// ������������
u16 preProcMaxNum_Cur(void);
// �����СƵ��
u16 preProcMinClk_Cur(void);
// �����С������
u16 preProcMinNum_Cur(void);

// ����ռ�ձȵĺ���
u16 calDutyCycle(u16 clk);

// �����ʼ��
void nAxisInit(void);
// X���ʼ��
void xAxisInit(void);
// Y���ʼ��
void yAxisInit(void);
// Z���ʼ��
void zAxisInit(void);
// A���ʼ��
void aAxisInit(void);
// B���ʼ��
void bAxisInit(void);

// ����ÿ�η�����������Ҫ��ʱ��_ ������ time��λΪus��������1e6
u16 calPWM_TimeCnt(u16 clk, u16 time);

// ��������
void nAxisStart(void);

// �Ӽ��ٶ�ʱ���жϷ������
void addSubTime_IRQ(void);
void xAddSubTime_IRQ(void);
void yAddSubTime_IRQ(void);
void zAddSubTime_IRQ(void);
void aAddSubTime_IRQ(void);
void bAddSubTime_IRQ(void);

// ������ٽ׶���Ҫ��������(������)
u16 calSubPlusCnt(u8 nAxis, u16 nAxisClk);

// �������Ƶ����Ҫ������ʱ�䣨����ֵ�� 
// 0����Ƶ�ʣ� 1�����Ƶ�ʣ� ����ֵ��С������ٶȵ��м�Ƶ��
void calMinSpeedTimeCnt(void);

// ����ͬ���Ĳ���
u16 correctSync(u16 nAxisMinClk, u8 nAxis);

// �������Ӽ��ٵ����Ƶ��
void correctNoAddSubClk(u8 nAxis, u32 plusNum);

// ����һ֡��Ҫ��ʱ�䣬 ��λus
float calTotalTime(u8 nAxis);

// �յ�������Ϣ�ظ� 0��ok�� 1��not ok
void respMsg(u8 status);

#endif
