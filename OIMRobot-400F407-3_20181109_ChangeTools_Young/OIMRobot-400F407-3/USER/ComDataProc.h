#ifndef __COMDATAPROC_H
#define __COMDATAPROC_H 

#include "sys.h"

#define SET_DATA_TEST 1			// 1�������˶����������ã�0���ر�

// ����������趨���˶������Ľṹ��
typedef struct 
{
	u32 nAxisPlusNum;				// �趨�������������������о���
	u16 nAxisClk;						// �趨�����Ƶ�ʣ�Ҳ�����ٲ��ֵ�Ƶ��
	
	u16 addSubClkSet[10];		// ����Ϊ7�Σ�����ÿ���趨������Ƶ�ʣ���10�����˿���
	u16 subLastTime;				// ���һ�Σ����ٲ��֣��ܵ�ʱ��
	u16 constTime;					// �������е�ʱ�䣬��40ms�ı���
} nAxisSetData;		// ĳ�����趨���˶�����

extern void selfCheckFunc(void);
//

#endif
