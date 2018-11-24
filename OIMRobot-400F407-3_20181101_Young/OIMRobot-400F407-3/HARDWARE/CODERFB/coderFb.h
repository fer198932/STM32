#ifndef _CODERFB_H
#define _CODERFB_H
#include "sys.h"
/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          ԭ���������������
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : Key.h
* By  : Hiker Lee
* 2018-05-02
********************************************************************************************************/

#define QUADRUPLE		// ֧���ı�Ƶ��ע�͵���ȡ��֧���ı�Ƶ
#define NOPLUS_MS 100		// ���������ٺ���û�������ˣ�100����10ms
#define NAXIS 3			// ��������豸��Ĭ��Ϊ3��

typedef struct			// ����coder
{
	u32 setPlusNum[NAXIS];	// �趨������������ӦxAxisPlusNum��5��ֱ�ΪX��Y��Z��A��B	
	u16 setFreq[NAXIS];		// �趨��X��Ƶ�ʣ���ӦxAxisClk��5��ֱ�ΪX��Y��Z��A��B	
	u8 setDir[NAXIS];		// �趨��X�᷽�򣬶�ӦxAxisDir��5��ֱ�ΪX��Y��Z��A��B
	
	u32 coderPlusNum[NAXIS];	// ���������������
	u32 coderPlusTime[NAXIS];		// ����ʱ���������
	u8 coderDir[NAXIS];			// ������ʵ���˶�����0���ı�Ƶģʽ��δ�趨��1������2������
	
//	void (*CoderFbStructure_Init)(void);	// ��ʼ���ṹ��
//	char (*getData)(void);					// ������塢ʱ�䡢Ƶ�ʵȲ�������setZeroFlag�ﵽָ��ֵ�󴥷�
//	char (*uartData)(void);					// ͨ�����ڽ����ݻش���λ��
	
} CoderFbStructure;	

#endif 
