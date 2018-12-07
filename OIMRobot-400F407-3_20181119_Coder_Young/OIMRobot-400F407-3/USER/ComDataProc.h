#ifndef __COMDATAPROC_H
#define __COMDATAPROC_H 

#include "sys.h"

#define SET_DATA_TEST 1			// 1�������˶����������ã�0���ر�

// ����������趨���˶������Ľṹ��
typedef struct 
{	
	u32 nAxisPlusNum;				// �趨�������������������о���
	u16 nAxisClk;						// �趨�����Ƶ�ʣ�Ҳ�����ٲ��ֵ�Ƶ��
	u16 nDir;								// �趨�ķ���
	
	u16 addSubClkSet[9];		// ����Ϊ7�Σ�����ÿ���趨������Ƶ�ʣ���9�����˿���
	u16 subLastTime;				// ���һ�Σ����ٲ��֣��ܵ�ʱ��
	u16 constTime;					// �������е�ʱ�䣬��40ms�ı���
} nAxisSetData;		// ĳ�����趨���˶�����

// ����ץȡ�趨���˶������Ľṹ�� - X��
typedef struct 
{
	u8 nAxisFlag; 							// ��ǵ�ǰ����һ�������
	u8 incClkDataFlag;					// ��������¼CLK���ݵı��
	
	u32 nAxisPlusNum;						// �趨�������������������о���
	u16 nAxisClk;								// �趨�����Ƶ�ʣ�Ҳ�����ٲ��ֵ�Ƶ��
	u16 nDir;										// �趨�ķ���
	
	u16 setClk[16];							// �����趨��Ƶ��
	u16 setClkTime[16];					// �趨Ƶ�ʵ�����ʱ�䣬��40msΪ��λ
	
	u16 subLastTime;				// ���һ�Σ����ٲ��֣��ܵ�ʱ��
	
} SetDataPerAxis;

extern void selfCheckFunc(void);
//

#endif
