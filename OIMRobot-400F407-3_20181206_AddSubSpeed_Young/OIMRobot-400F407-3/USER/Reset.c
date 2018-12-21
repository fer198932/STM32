/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          原能智能制造机器人
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : AddSubSpeed.C
* By  : Young
* 2018-12-06
* 系统复位到机械零点的程序，传感器触发
* 2018-06-28
********************************************************************************************************/
#include <string.h>					// 包括 memset 函数
#include "Reset.h"

u8 resetStatusFlag = 0;

// 机械零点复位主程序 
void reset2Zero(void)
{
	mainMotorStop();	// 刀轴停止转动
	delay_s(1);
	
	// 从工加工点到零点
	stepsTemp.xSteps[0] = 0;					// 计数置零
	stepsTemp.ySteps[0] = 0;
	stepsTemp.zSteps[0] = 0;			

	axis3Back2ORGWithCnt();									// 回到零点， 带计步数的功能	
//	dataFromTemp2Saved(0);									// 数据保存	
	
#ifdef RESET_DEBUG
	resetStatusFlag = 1;					// 下一次点击会回到起始点
#endif
	respFlag = 1;	// 标记为不可回复状态
	respUartMsg();
}

#ifdef RESET_DEBUG
// 从零点到开始位置，debug模式下才开启
void zero2Start(void)
{
	// 保存的值重新写入，准备返回原点
	changeToolsSavedSteps.xSteps[0] = stepsTemp.xSteps[0];
	changeToolsSavedSteps.ySteps[0] = stepsTemp.ySteps[0];
	changeToolsSavedSteps.zSteps[0] = stepsTemp.zSteps[0];
	
	// 调用换刀的回零点程序	
	toolsFromORG2Start();	
	
	// 设置标志位
	xAxisStepFlag = 2;
	yAxisStepFlag = 2;
	zAxisStepFlag = 2;
	
	resetStatusFlag = 0;					// 下一次点击会回到零点
	
	respFlag = 1;	// 标记为不可回复状态
	respUartMsg();
}
#endif

// 回复给串口
void respUartMsg(void)
{
	u8 i;
	u8 backResString[28];
	u8 size = sizeof(backResString);
	memset(backResString, 0, size);
	
	backResString[0] = FrameB1;
	backResString[1] = FrameB2;
	backResString[2] = CONTROLINFO;
	
	backResString[3] = size;							// 数据长度
	backResString[4] = 0x00;
	
	backResString[5] = 0x0F;					// 指令码
	backResString[6] = 0x00;
	
	// 数据。 注意四字节对齐问题
	memcpy(backResString+7, &stepsTemp.xSteps[0], 4);					// X轴 u32
	memcpy(backResString+11, &stepsTemp.ySteps[0], 4);				// Y轴
	memcpy(backResString+15, &stepsTemp.zSteps[0], 4);				// Z轴
	
	backResString[27] = FrameEnd;
	
	// 回复
	for(i=0;i<size;i++)
	{
		USART_SendData(USART3,backResString[i]); 
		while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);	//等待发送结束
	}	
}
