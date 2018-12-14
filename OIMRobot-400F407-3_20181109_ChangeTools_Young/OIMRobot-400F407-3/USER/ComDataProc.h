#ifndef __COMDATAPROC_H
#define __COMDATAPROC_H 

#include "sys.h"
#include "Reset.h"

#define SET_DATA_TEST 1			// 1�������˶����������ã�0���ر�

#define IF_ADDSUB_WORK_COMDATAPROC		// �Ƿ���мӼ����˶����жϣ�ע�ͺ󲻼Ӽ���

// ����������趨���˶������Ľṹ��
typedef struct 
{
	u32 nAxisPlusNum;				// �趨�������������������о���
	u16 nAxisClk;						// �趨�����Ƶ�ʣ�Ҳ�����ٲ��ֵ�Ƶ��
	
	u16 addSubClkSet[10];		// ����Ϊ7�Σ�����ÿ���趨������Ƶ�ʣ���10�����˿���
	u16 subLastTime;				// ���һ�Σ����ٲ��֣��ܵ�ʱ��
	u16 constTime;					// �������е�ʱ�䣬��40ms�ı���
} nAxisSetData;		// ĳ�����趨���˶�����

void preProcMaxNum(void);
u8 preProcStep(void);
void addSubFlagSet(void);
void addSubSpeed(void);
void motor5workstart(void);

extern void selfCheckFunc(void);

// ��е��㸴λ������
extern void reset2Zero();

extern u8 resetStatusFlag;

// ���޲����ظ���λ����ʱ����
void rs232DataProcStep3(void);

#ifdef RESET_DEBUG
// ����㵽��ʼλ�ã�debugģʽ�²ſ���
extern void zero2Start(void);
extern u8 resetStatusFlag;			// 0:���㣬 1������ʼ��
#endif

#endif
