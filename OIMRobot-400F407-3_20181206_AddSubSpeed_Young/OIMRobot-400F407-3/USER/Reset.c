/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          ԭ���������������
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : AddSubSpeed.C
* By  : Young
* 2018-12-06
* ϵͳ��λ����е���ĳ��򣬴���������
* 2018-06-28
********************************************************************************************************/
#include <string.h>					// ���� memset ����
#include "Reset.h"

u8 resetStatusFlag = 0;

// ��е��㸴λ������ 
void reset2Zero(void)
{
	mainMotorStop();	// ����ֹͣת��
	delay_s(1);
	
	// �ӹ��ӹ��㵽���
	stepsTemp.xSteps[0] = 0;					// ��������
	stepsTemp.ySteps[0] = 0;
	stepsTemp.zSteps[0] = 0;			

	axis3Back2ORGWithCnt();									// �ص���㣬 ���Ʋ����Ĺ���	
//	dataFromTemp2Saved(0);									// ���ݱ���	
	
#ifdef RESET_DEBUG
	resetStatusFlag = 1;					// ��һ�ε����ص���ʼ��
#endif
	respFlag = 1;	// ���Ϊ���ɻظ�״̬
	respUartMsg();
}

#ifdef RESET_DEBUG
// ����㵽��ʼλ�ã�debugģʽ�²ſ���
void zero2Start(void)
{
	// �����ֵ����д�룬׼������ԭ��
	changeToolsSavedSteps.xSteps[0] = stepsTemp.xSteps[0];
	changeToolsSavedSteps.ySteps[0] = stepsTemp.ySteps[0];
	changeToolsSavedSteps.zSteps[0] = stepsTemp.zSteps[0];
	
	// ���û����Ļ�������	
	toolsFromORG2Start();	
	
	// ���ñ�־λ
	xAxisStepFlag = 2;
	yAxisStepFlag = 2;
	zAxisStepFlag = 2;
	
	resetStatusFlag = 0;					// ��һ�ε����ص����
	
	respFlag = 1;	// ���Ϊ���ɻظ�״̬
	respUartMsg();
}
#endif

// �ظ�������
void respUartMsg(void)
{
	u8 i;
	u8 backResString[28];
	u8 size = sizeof(backResString);
	memset(backResString, 0, size);
	
	backResString[0] = FrameB1;
	backResString[1] = FrameB2;
	backResString[2] = CONTROLINFO;
	
	backResString[3] = size;							// ���ݳ���
	backResString[4] = 0x00;
	
	backResString[5] = 0x0F;					// ָ����
	backResString[6] = 0x00;
	
	// ���ݡ� ע�����ֽڶ�������
	memcpy(backResString+7, &stepsTemp.xSteps[0], 4);					// X�� u32
	memcpy(backResString+11, &stepsTemp.ySteps[0], 4);				// Y��
	memcpy(backResString+15, &stepsTemp.zSteps[0], 4);				// Z��
	
	backResString[27] = FrameEnd;
	
	// �ظ�
	for(i=0;i<size;i++)
	{
		USART_SendData(USART3,backResString[i]); 
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);	//�ȴ����ͽ���
	}	
}
