#ifndef __RESET_H
#define __RESET_H 

#include "sys.h"
#include "ChangeTools.h"

#define RESET_DEBUG			// debug ģʽ�£��ٴε����λ��ť�᷵����ʼ�㣬����ģʽ�£�ע�͵���������

#ifdef RESET_DEBUG
extern u32 xAxisPlusNum,yAxisPlusNum,zAxisPlusNum,aAxisPlusNum,bAxisPlusNum;
extern void toolsFromORG2Start(void);	// ����㷵��ԭ��������λ��
extern ChangeToolsSavedSteps changeToolsSavedSteps;

extern u8 xAxisStepFlag; 
extern u8 yAxisStepFlag; 
extern u8 zAxisStepFlag; 
extern u8 aAxisStepFlag; 
extern u8 bAxisStepFlag; 
#endif

// ����ֹͣת��
extern void mainMotorStop(void);

extern void delay_s(u8 delay);

// �����˶����ݵĽṹ��
extern ChangeToolsSavedSteps stepsTemp;

// �ܼƲ����Ļص�ԭ��
extern void axis3Back2ORGWithCnt(void);

// ����ŵ�temp���ݽ��б��棺0���ӹ��㵽��� 1-4����㵽����
extern void dataFromTemp2Saved(u8 num);

// �ظ�������
void respUartMsg(void);

extern unsigned char respFlag;		// �Ƿ�ظ��ı�ǣ� 0���ظ�  1�����ظ�
#endif
