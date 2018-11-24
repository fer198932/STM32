#ifndef _CODERFB_H
#define _CODERFB_H
#include "sys.h"
/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          原能智能制造机器人
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

#define QUADRUPLE		// 支持四倍频，注释掉即取消支持四倍频
#define NOPLUS_MS 100		// 编码器多少毫秒没有脉冲了，100代表10ms
#define NAXIS 3			// 多少轴的设备，默认为3轴

typedef struct			// 用于coder
{
	u32 setPlusNum[NAXIS];	// 设定的脉冲数，对应xAxisPlusNum，5轴分别为X、Y、Z、A、B	
	u16 setFreq[NAXIS];		// 设定的X轴频率，对应xAxisClk，5轴分别为X、Y、Z、A、B	
	u8 setDir[NAXIS];		// 设定的X轴方向，对应xAxisDir，5轴分别为X、Y、Z、A、B
	
	u32 coderPlusNum[NAXIS];	// 编码器输出的脉冲
	u32 coderPlusTime[NAXIS];		// 运行时间的脉冲数
	u8 coderDir[NAXIS];			// 编码器实际运动方向，0：四倍频模式下未设定，1：正向，2：负向
	
//	void (*CoderFbStructure_Init)(void);	// 初始化结构体
//	char (*getData)(void);					// 获得脉冲、时间、频率等参数，在setZeroFlag达到指定值后触发
//	char (*uartData)(void);					// 通过串口将数据回传上位机
	
} CoderFbStructure;	

#endif 
