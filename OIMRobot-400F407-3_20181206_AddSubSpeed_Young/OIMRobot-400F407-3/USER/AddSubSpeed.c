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
* �޸ļӼ����㷨ΪS����
* 2018-06-28
********************************************************************************************************/

#include "AddSubSpeed.h"

// ȫ�ֱ�����
extern u32 xAxisPlusNum,yAxisPlusNum,zAxisPlusNum,aAxisPlusNum,bAxisPlusNum;
extern u16 xAxisClk,yAxisClk,zAxisClk,aAxisClk,bAxisClk;
extern unsigned char  xAxisDir,yAxisDir,zAxisDir,aAxisDir,bAxisDir;
u16 xAxisClk_Cur,yAxisClk_Cur,zAxisClk_Cur,aAxisClk_Cur,bAxisClk_Cur;

extern u8 addtime;
extern u32 pwmNumTim12, pwmNumTim9, pwmNumTim3, pwmNumTim4, pwmNumTim2;
extern u8 xAxisStepFlag, yAxisStepFlag, zAxisStepFlag, aAxisStepFlag, bAxisStepFlag; 
extern u32 savePlusNum[5];		// ��������Ӽ���ʱ ���ٽ׶��߹���������

AddSubSetting addSubSetting;					// �Ӽ����㷨����

#if _OUT_DATA_PROC
extern nAxisSetData allAxisSetData[3];				// ��¼�趨���˶�����	
#endif

// �����СƵ�ʣ�ע��Ҫ��ǰ��ֵ��
u16 max_Clk, min_Clk;
// ��Ӧ���Ƶ�ʵ��ᣬ 0-4��x��y��z��a��b
u8 maxClkNum = 5;			// ����5��ʾ���󣬲�������

u8 ifUseAddSubFlag = 0;								// 0��ʹ�üӼ��٣� 1����ʹ��
u8 respMsgData = 0;										// 0: һ��������1����ok-���ܴ�����������
u8 maxValueFlag, minValueFlag;				// ���ֵ����Сֵ��ǣ� 0-4��x��y��z��a��b
u16 clkPerStep;												// ������Ƶ��
u16 clkTimeCntNeed[5];								// ÿһ����Ҫ��ʱ�䣬��Ƶ������Ҫ���� 0-4:x��y��z��a��b
u16 minSpeedTimeCnt[5] = {0, 0, 0, 0, 0};		// �������еļ���������λΪaddtime�� 0-4:x��y��z��a��b
u8 savePlusNumFlag[5] = {0, 0, 0, 0, 0};	// �Ƿ񱣴��������ı�ǩ�������ڼ���״̬ 0�������棬 1������ 0-4��x��y��z��a��b
u8 startSubFlag[5] = {0, 0, 0, 0, 0};			// ������ٽ׶εı�ǣ�0����������٣�1����ʼ����
u16 subPlusCnt[5] = {0, 0, 0, 0, 0};		// ���ٽ׶������������
u16 correctSyncValue[5] = {0, 0, 0, 0, 0};		// ���������м�Ƶ�ʵļ��ٽ׶�����ֵ
// u8 avoidUrgentChangeDirFlag = 0;							// �����������ı�ǩ ��С�ķ���������������

// �Ӽ��������� clk_Src��ԭ�ٶȣ� clk_Dst��Ŀ���ٶ�
void addSubSpeed_Cur(u16 clk_Src, u16 clk_Dst)
{
	// ��ʼ���Ĺ��̱�����취���������ⱻ�жϲ������� �ٽ���
	nAxisInit();
		
	nAxisStart();
	
#if _OUT_DATA_PROC
	// ����������������
	setDataAssign();
	respOutData2PC();
	delay_ms(500);
#endif
	
	// ϵͳ�����󣨿���δֹͣ�������ظ���λ�� 1:ok
	respMsg(1);
}
void addSubSpeed_CurTest(u16 clk_Src, u16 clk_Dst)
{
	zAxisClk_Cur = Z_AXIS_MIN_CLK;
	TIM4_PWM_Init(ARR_VAL-1, calPSC(zAxisClk_Cur)-1);				// �ٶȳ�ʼ��
	savePlusNumFlag[2] = 1;																		// �տ�ʼ��Ҫ������ٽ׶ε�������
//	calClkPerStep();
}

// ��������
void nAxisStart(void)
{
	motor5workstart();
	subAddTimerStart();
}

// �������Ƶ����Ҫ������ʱ�䣨����ֵ�� 
// 0����Ƶ�ʣ� 0�����Ƶ�ʣ� ����ֵ��С������ٶȵ��м�Ƶ��
void calMinSpeedTimeCnt(void)
{
	// X��
	if(0 == xAxisClk)
		minSpeedTimeCnt[0] = 0;
	else if(max_Clk == xAxisClk)
		minSpeedTimeCnt[0] = 0;
	else
		minSpeedTimeCnt[0] = max_Clk - xAxisClk;
	
	// Y��
	if(0 == yAxisClk)
		minSpeedTimeCnt[1] = 0;
	else if(max_Clk == yAxisClk)
		minSpeedTimeCnt[1] = 0;
	else
		minSpeedTimeCnt[1] = max_Clk - yAxisClk;
	
	// Z��
	if(0 == zAxisClk)
		minSpeedTimeCnt[2] = 0;
	else if(max_Clk == zAxisClk)
		minSpeedTimeCnt[2] = 0;
	else
		minSpeedTimeCnt[2] = max_Clk - zAxisClk;
	
	// A��
	if(0 == aAxisClk)
		minSpeedTimeCnt[3] = 0;
	else if(max_Clk == aAxisClk)
		minSpeedTimeCnt[3] = 0;
	else
		minSpeedTimeCnt[3] = max_Clk - aAxisClk;
	
	// B��
	if(0 == bAxisClk)
		minSpeedTimeCnt[4] = 0;
	else if(max_Clk == bAxisClk)
		minSpeedTimeCnt[4] = 0;
	else
		minSpeedTimeCnt[4] = max_Clk - bAxisClk;	
}

// �Ӽ��ٶ�ʱ���жϷ������  ���Ǹĳ�����������궨�壬�������Ч��  byYJY
void addSubTime_IRQ(void)
{
	if((xAxisPlusNum != 0) && (xAxisClk != 0))
		xAddSubTime_IRQ();
	if((yAxisPlusNum != 0) && (yAxisClk != 0))
		yAddSubTime_IRQ();
	if((zAxisPlusNum != 0) && (zAxisClk != 0))
		zAddSubTime_IRQ();
	if((aAxisPlusNum != 0) && (aAxisClk != 0))
		aAddSubTime_IRQ();
	if((bAxisPlusNum != 0) && (bAxisClk != 0))
		bAddSubTime_IRQ();
}
void xAddSubTime_IRQ(void)
{
	// X��
	// �ٶ�̫�Ͳ����мӼ���
	if(X_AXIS_MIN_CLK >= xAxisClk)
		return;
	
	// ��������н׶�
	if(0 != minSpeedTimeCnt[0])
	{
		minSpeedTimeCnt[0]--;
	}
	// ���ٽ׶�
	else if(1 == savePlusNumFlag[0])												// �����ڼ��ٽ׶�
	{
		if(xAxisClk_Cur <= xAxisClk)										// ���ٽ׶�
		{
			xAxisClk_Cur++;
			if(clkTimeCntNeed[0])													// ��Ƶ������Ҫ�ϳ���ά��ʱ��
			{
				clkTimeCntNeed[0]--;
			} else
			{
				if( ifX_PscModify(calPSC(xAxisClk_Cur)) )			// ���PSCֵ�޸��ˣ������¸�ֵ
				{
					xAxisSetPWM(calPSC(xAxisClk_Cur), calDutyCycle(xAxisClk_Cur), calDutyCycle(xAxisClk_Cur));
				}					
				clkTimeCntNeed[0] = calPWM_TimeCnt(xAxisClk_Cur, setAddTime());
			}
		} 
		else {
			savePlusNum[0] = xPWMCnt;
			savePlusNumFlag[0] = 0;															// �رձ�ǩ���������ȡ����ٽ׶�
		} 
	}
	// ���ٽ׶�
	else if(startSubFlag[0]) 				// ���ٽ׶Σ����Ϊ1ʱ��ʼ���٣���exti.c����λ
	{
		if(xAxisClk_Cur > X_AXIS_MIN_CLK)					// �ݼ�����СƵ�ʣ�С�ĵݼ�����������
		{
			xAxisClk_Cur--;
			if(clkTimeCntNeed[0])
			{
				clkTimeCntNeed[0]--;
			} else
			{
				if( ifX_PscModify(calPSC(xAxisClk_Cur)) )
				{
					xAxisSetPWM(calPSC(xAxisClk_Cur), calDutyCycle(xAxisClk_Cur), calDutyCycle(xAxisClk_Cur));
				}					
				clkTimeCntNeed[0] = calPWM_TimeCnt(xAxisClk_Cur, setAddTime());
			}
		}
	}	
}
void yAddSubTime_IRQ(void)
{
	// Y��
	// �ٶ�̫�Ͳ����мӼ���
	if(Y_AXIS_MIN_CLK >= yAxisClk)
		return;
	
	// ��������н׶�
	if(0 != minSpeedTimeCnt[1])
	{
		minSpeedTimeCnt[1]--;
	}
	// ���ٽ׶�
	else if(1 == savePlusNumFlag[1])												// �����ڼ��ٽ׶�
	{
		if(yAxisClk_Cur <= yAxisClk)										// ���ٽ׶�
		{
			yAxisClk_Cur++;
			if(clkTimeCntNeed[1])													// ��Ƶ������Ҫ�ϳ���ά��ʱ��
			{
				clkTimeCntNeed[1]--;
			} else
			{
				if( ifY_PscModify(calPSC(yAxisClk_Cur)) )			// ���PSCֵ�޸��ˣ������¸�ֵ
				{
					yAxisSetPWM(calPSC(yAxisClk_Cur), calDutyCycle(yAxisClk_Cur), calDutyCycle(yAxisClk_Cur));
				}					
				clkTimeCntNeed[1] = calPWM_TimeCnt(yAxisClk_Cur, setAddTime());
			}
		} 
		else {
			savePlusNum[1] = yPWMCnt;
			savePlusNumFlag[1] = 0;															// �رձ�ǩ���������ȡ����ٽ׶�
		} 
	}
	// ���ٽ׶�
//	else if((yAxisPlusNum - yPWMCnt) < (savePlusNum[1]-correctSync(yAxisClk, 1))) // ���ٽ׶Σ�ʣ���������С�ڼ��ٽ׶ε�������
	else if(startSubFlag[1]) 				// ���ٽ׶Σ����Ϊ1ʱ��ʼ���٣���exti.c����λ
	{
		if(yAxisClk_Cur > Y_AXIS_MIN_CLK)					// �ݼ�����СƵ�ʣ�С�ĵݼ�����������
		{
			yAxisClk_Cur--;
			if(clkTimeCntNeed[1])
			{
				clkTimeCntNeed[1]--;
			} else
			{
				if( ifY_PscModify(calPSC(yAxisClk_Cur)) )
				{
					yAxisSetPWM(calPSC(yAxisClk_Cur), calDutyCycle(yAxisClk_Cur), calDutyCycle(yAxisClk_Cur));
				}					
				clkTimeCntNeed[1] = calPWM_TimeCnt(yAxisClk_Cur, setAddTime());
			}
		}
	}	
}
void zAddSubTime_IRQ(void)
{
	// Z��
	// �ٶ�̫�Ͳ����мӼ���
	if(Z_AXIS_MIN_CLK >= zAxisClk)
		return;
	
	// ��������н׶�
	if(0 != minSpeedTimeCnt[2])
	{
		minSpeedTimeCnt[2]--;
	}
	// ���ٽ׶�
	else if(1 == savePlusNumFlag[2])												// �����ڼ��ٽ׶�
	{
		if(zAxisClk_Cur <= zAxisClk)										// ���ٽ׶�
		{
			zAxisClk_Cur++;
			if(clkTimeCntNeed[2])													// ��Ƶ������Ҫ�ϳ���ά��ʱ��
			{
				clkTimeCntNeed[2]--;
			} else
			{
				if( ifZ_PscModify(calPSC(zAxisClk_Cur)) )			// ���PSCֵ�޸��ˣ������¸�ֵ
				{
					zAxisSetPWM(calPSC(zAxisClk_Cur), calDutyCycle(zAxisClk_Cur), calDutyCycle(zAxisClk_Cur));
				}					
				clkTimeCntNeed[2] = calPWM_TimeCnt(zAxisClk_Cur, setAddTime());
			}
		} 
		else {
			savePlusNum[2] = zPWMCnt;										// test
			savePlusNumFlag[2] = 0;															// �رձ�ǩ���������ȡ����ٽ׶�
		} 
	}
	// ���ٽ׶�
	else if(startSubFlag[2]) 				// ���ٽ׶Σ����Ϊ1ʱ��ʼ���٣���exti.c����λ
	{
		if(zAxisClk_Cur > Z_AXIS_MIN_CLK)					// �ݼ�����СƵ�ʣ�С�ĵݼ�����������
		{
			zAxisClk_Cur--;
			if(clkTimeCntNeed[2])
			{
				clkTimeCntNeed[2]--;
			} else
			{
				if( ifZ_PscModify(calPSC(zAxisClk_Cur)) )
				{
					zAxisSetPWM(calPSC(zAxisClk_Cur), calDutyCycle(zAxisClk_Cur), calDutyCycle(zAxisClk_Cur));
				}					
				clkTimeCntNeed[2] = calPWM_TimeCnt(zAxisClk_Cur, setAddTime());
			}
		}
	}
}
void aAddSubTime_IRQ(void)
{
	// A��
	// �ٶ�̫�Ͳ����мӼ���
	if(A_AXIS_MIN_CLK >= aAxisClk)
		return;
	
	// ��������н׶�
	if(0 != minSpeedTimeCnt[3])
	{
		minSpeedTimeCnt[3]--;
	}
	// ���ٽ׶�
	else if(1 == savePlusNumFlag[3])												// �����ڼ��ٽ׶�
	{
		if(aAxisClk_Cur <= aAxisClk)										// ���ٽ׶�
		{
			aAxisClk_Cur++;
			if(clkTimeCntNeed[3])													// ��Ƶ������Ҫ�ϳ���ά��ʱ��
			{
				clkTimeCntNeed[3]--;
			} else
			{
				if( ifA_PscModify(calPSC(aAxisClk_Cur)) )			// ���PSCֵ�޸��ˣ������¸�ֵ
				{
					aAxisSetPWM(calPSC(aAxisClk_Cur), calDutyCycle(aAxisClk_Cur), calDutyCycle(aAxisClk_Cur));
				}					
				clkTimeCntNeed[3] = calPWM_TimeCnt(aAxisClk_Cur, setAddTime());
			}
		} 
		else {
			savePlusNum[3] = aPWMCnt;										// test
			savePlusNumFlag[3] = 0;															// �رձ�ǩ���������ȡ����ٽ׶�
		} 
	}
	// ���ٽ׶�
	else if(startSubFlag[3]) 				// ���ٽ׶Σ����Ϊ1ʱ��ʼ���٣���exti.c����λ
	{
		if(aAxisClk_Cur > A_AXIS_MIN_CLK)					// �ݼ�����СƵ�ʣ�С�ĵݼ�����������
		{
			aAxisClk_Cur--;
			if(clkTimeCntNeed[3])
			{
				clkTimeCntNeed[3]--;
			} else
			{
				if( ifA_PscModify(calPSC(aAxisClk_Cur)) )
				{
					aAxisSetPWM(calPSC(aAxisClk_Cur), calDutyCycle(aAxisClk_Cur), calDutyCycle(aAxisClk_Cur));
				}
				clkTimeCntNeed[3] = calPWM_TimeCnt(aAxisClk_Cur, setAddTime());
			}
		}
	}
}
void bAddSubTime_IRQ(void)
{
	// B��
	// �ٶ�̫�Ͳ����мӼ���
	if(B_AXIS_MIN_CLK >= bAxisClk)
//	{ 
//		if(0 != savePlusNum[4])  // ��ʱ�������
//		{
//			correctNoAddSubClk(4, bAxisPlusNum);		// ���Ӽ��٣�������ԭ�ٶ�  
//			bAxisSetPWM(calPSC(bAxisClk), calDutyCycle(bAxisClk), calDutyCycle(bAxisClk));
//		}
		return;
//	}
	
	// ��������н׶�
	if(0 != minSpeedTimeCnt[4])
	{
		minSpeedTimeCnt[4]--;
	}
	// ���ٽ׶�
	else if(1 == savePlusNumFlag[4])												// �����ڼ��ٽ׶�
	{
		if(bAxisClk_Cur <= bAxisClk)										// ���ٽ׶�
		{
			bAxisClk_Cur++;
			if(clkTimeCntNeed[4])													// ��Ƶ������Ҫ�ϳ���ά��ʱ��
			{
				clkTimeCntNeed[4]--;
			} else
			{
				if( ifB_PscModify(calPSC(bAxisClk_Cur)) )			// ���PSCֵ�޸��ˣ������¸�ֵ
				{
					bAxisSetPWM(calPSC(bAxisClk_Cur), calDutyCycle(bAxisClk_Cur), calDutyCycle(bAxisClk_Cur));
				}					
				clkTimeCntNeed[4] = calPWM_TimeCnt(bAxisClk_Cur, setAddTime());
			}
		} 
		else {
			savePlusNum[4] = bPWMCnt;										// test
			savePlusNumFlag[4] = 0;															// �رձ�ǩ���������ȡ����ٽ׶�
		} 
	}
	// ���ٽ׶�
	else if(startSubFlag[4]) 				// ���ٽ׶Σ����Ϊ1ʱ��ʼ���٣���exti.c����λ
	{
		if(bAxisClk_Cur > B_AXIS_MIN_CLK)					// �ݼ�����СƵ�ʣ�С�ĵݼ�����������
		{
			bAxisClk_Cur--;
			if(clkTimeCntNeed[4])
			{
				clkTimeCntNeed[4]--;
			} else
			{
				if( ifB_PscModify(calPSC(bAxisClk_Cur)) )
				{
					bAxisSetPWM(calPSC(bAxisClk_Cur), calDutyCycle(bAxisClk_Cur), calDutyCycle(bAxisClk_Cur));
				}					
				clkTimeCntNeed[4] = calPWM_TimeCnt(bAxisClk_Cur, setAddTime());
			}
		}
	}
}

// �����ʼ��
void nAxisInit(void)
{
	// �����СƵ�ʼ��㣬��Ҫ��������ǰ����
	max_Clk = preProcMaxClk_Cur();
	min_Clk = preProcMinClk_Cur();
	// ��������Ƶ�ʷ���Ҫ�� �ú����ɷ���״ֵ̬
	adjustClk(max_Clk, min_Clk);
	// ������С�ٶȵĳ���ʱ��
	calMinSpeedTimeCnt();
	memset(savePlusNum, 0, sizeof(savePlusNum));
	memset(subPlusCnt, 0, sizeof(subPlusCnt));
	memset(correctSyncValue, 0, sizeof(correctSyncValue));
	
	if((xAxisPlusNum != 0) && (xAxisClk != 0))
		xAxisInit();
	
	if((yAxisPlusNum != 0) && (yAxisClk != 0))
		yAxisInit();
	
	if((zAxisPlusNum != 0) && (zAxisClk != 0))
		zAxisInit();
	
	if((aAxisPlusNum != 0) && (aAxisClk != 0))
		aAxisInit();
	
	if((bAxisPlusNum != 0) && (bAxisClk != 0))
		bAxisInit();
}
// X���ʼ��
void xAxisInit(void)
{
	// X����������ʼ��
	if(xAxisClk > X_AXIS_MIN_CLK)
		xAxisClk_Cur = X_AXIS_MIN_CLK;
	else
	{
		correctNoAddSubClk(0, xAxisPlusNum);		// ���Ӽ��٣�������ԭ�ٶ�
		xAxisClk_Cur = xAxisClk;				
	}
	
	// ������ٽ׶ε���������ֵ
	correctSyncValue[0] = correctSync(X_AXIS_MIN_CLK, 0);
	
	subPlusCnt[0] = calSubPlusCnt(0, xAxisClk) + 1;		// ��������+1������Լ��С���������Ƿ����������룿�� byYJY
	clkTimeCntNeed[0] = calPWM_TimeCnt(xAxisClk_Cur, setAddTime());		// ע��addtime��ֵ��TIM7���	
	xAxis_PWM_Init(ARR_VAL-1, calPSC(xAxisClk_Cur)-1);				// �ٶȳ�ʼ��
	xAxisSetPWM(calPSC(xAxisClk_Cur)-1, calDutyCycle(xAxisClk_Cur), calDutyCycle(xAxisClk_Cur));	// ����PWM�Ĳ������Ĵ�������
	savePlusNumFlag[0] = 1;						// ����ȷ�����ٽ׶εı��
	xSetPWMCnt(0);									// PWM�жϼ�������
	xAxisStepFlag = 0;							// ���������еı��
	
	if(xAxisDir==0)									// ע���� ComDataProc.c ���Ѿ��ж��˷��򣬵��������жϸ���
	{
		xAxisDIR_ON;
	}
	else
	{
		xAxisDIR_OFF;
	}	 
}
// Y���ʼ��
void yAxisInit(void)
{
	// Y����������ʼ��
	if(yAxisClk > Y_AXIS_MIN_CLK)
		yAxisClk_Cur = Y_AXIS_MIN_CLK;
	else
	{
		correctNoAddSubClk(1, yAxisPlusNum);		// ���Ӽ��٣�������ԭ�ٶ�
		yAxisClk_Cur = yAxisClk;				
	}
	
	// ������ٽ׶ε���������ֵ
	correctSyncValue[1] = correctSync(Y_AXIS_MIN_CLK, 1);
	
	subPlusCnt[1] = calSubPlusCnt(1, yAxisClk) + 1;	// ��������+1������Լ��С���������Ƿ����������룿�� byYJY
	clkTimeCntNeed[1] = calPWM_TimeCnt(yAxisClk_Cur, setAddTime());		// ע��addtime��ֵ��TIM7���	
	yAxis_PWM_Init(ARR_VAL-1, calPSC(yAxisClk_Cur)-1);				// �ٶȳ�ʼ��
	// ����PWM�Ĳ������Ĵ�������
	yAxisSetPWM(calPSC(yAxisClk_Cur)-1, calDutyCycle(yAxisClk_Cur), calDutyCycle(yAxisClk_Cur));	
	savePlusNumFlag[1] = 1;						// ����ȷ�����ٽ׶εı��
	ySetPWMCnt(0);									// PWM�жϼ�������
	yAxisStepFlag = 0;							// ���������еı��
	if(yAxisDir==0)									// ע���� ComDataProc.c ���Ѿ��ж��˷��򣬵��������жϸ���
	{
		yAxisDIR_ON;
	}
	else
	{
		yAxisDIR_OFF;
	}	
}
// Z���ʼ��
void zAxisInit(void)
{
	// Z����������ʼ��
	if(zAxisClk > Z_AXIS_MIN_CLK)
		zAxisClk_Cur = Z_AXIS_MIN_CLK;
	else
	{
		correctNoAddSubClk(2, zAxisPlusNum);		// ���Ӽ��٣�������ԭ�ٶ�
		zAxisClk_Cur = zAxisClk;				
	}
	
	// ������ٽ׶ε���������ֵ
	correctSyncValue[2] = correctSync(Z_AXIS_MIN_CLK, 2);
	
	subPlusCnt[2] = calSubPlusCnt(2, zAxisClk) + 1;					// ��������+1������Լ��С���������Ƿ����������룿�� byYJY
	clkTimeCntNeed[2] = calPWM_TimeCnt(zAxisClk_Cur, setAddTime());		// ע��addtime��ֵ��TIM7���	
	zAxis_PWM_Init(ARR_VAL-1, calPSC(zAxisClk_Cur)-1);				// �ٶȳ�ʼ��
	zAxisSetPWM(calPSC(zAxisClk_Cur)-1, calDutyCycle(zAxisClk_Cur), calDutyCycle(zAxisClk_Cur));	// ����PWM�Ĳ������Ĵ�������
	savePlusNumFlag[2] = 1;						// ����ȷ�����ٽ׶εı��
	zSetPWMCnt(0);									// PWM�жϼ�������
	zAxisStepFlag = 0;							// ���������еı��
	if(zAxisDir==0)									// ע���� ComDataProc.c ���Ѿ��ж��˷��򣬵��������жϸ���
	{
		zAxisDIR_ON;
	}
	else
	{
		zAxisDIR_OFF;
	}
}
// A���ʼ��
void aAxisInit(void)
{
	// A����������ʼ��
	if(aAxisClk > A_AXIS_MIN_CLK)
		aAxisClk_Cur = A_AXIS_MIN_CLK;
	else
	{
		correctNoAddSubClk(3, aAxisPlusNum);		// ���Ӽ��٣�������ԭ�ٶ�
		aAxisClk_Cur = aAxisClk;				
	}
	
	// ������ٽ׶ε���������ֵ
	correctSyncValue[3] = correctSync(A_AXIS_MIN_CLK, 3);
	
	subPlusCnt[3] = calSubPlusCnt(3, aAxisClk) + 1;					// ��������+1������Լ��С���������Ƿ����������룿�� byYJY
	clkTimeCntNeed[3] = calPWM_TimeCnt(aAxisClk_Cur, setAddTime());		// ע��addtime��ֵ��TIM7���	
	aAxis_PWM_Init(ARR_VAL-1, calPSC(aAxisClk_Cur)-1);				// �ٶȳ�ʼ��
	aAxisSetPWM(calPSC(aAxisClk_Cur)-1, calDutyCycle(aAxisClk_Cur), calDutyCycle(aAxisClk_Cur));	// ����PWM�Ĳ������Ĵ�������
	savePlusNumFlag[3] = 1;						// ����ȷ�����ٽ׶εı��
	aSetPWMCnt(0);									// PWM�жϼ�������
	aAxisStepFlag = 0;							// ���������еı��
	if(aAxisDir==0)									// ע���� ComDataProc.c ���Ѿ��ж��˷��򣬵��������жϸ���
	{
		aAxisDIR_ON;
	}
	else
	{
		aAxisDIR_OFF;
	}
}
// B���ʼ��
void bAxisInit(void)
{
	// B����������ʼ��
	if(bAxisClk > B_AXIS_MIN_CLK)
		bAxisClk_Cur = B_AXIS_MIN_CLK;
	else
	{
		correctNoAddSubClk(4, bAxisPlusNum);		// ���Ӽ��٣�������ԭ�ٶ�
		bAxisClk_Cur = bAxisClk;				
	}
	
	// ������ٽ׶ε���������ֵ
	correctSyncValue[4] = correctSync(B_AXIS_MIN_CLK, 4);
	
	subPlusCnt[4] = calSubPlusCnt(4, bAxisClk) + 1;					// ��������+1������Լ��С���������Ƿ����������룿�� byYJY
	clkTimeCntNeed[4] = calPWM_TimeCnt(bAxisClk_Cur, setAddTime());		// ע��addtime��ֵ��TIM7���	
	bAxis_PWM_Init(ARR_VAL-1, calPSC(bAxisClk_Cur)-1);				// �ٶȳ�ʼ��
	bAxisSetPWM(calPSC(bAxisClk_Cur)-1, calDutyCycle(bAxisClk_Cur), calDutyCycle(bAxisClk_Cur));	// ����PWM�Ĳ������Ĵ�������
	savePlusNumFlag[4] = 1;						// ����ȷ�����ٽ׶εı��
	bSetPWMCnt(0);									// PWM�жϼ�������
	bAxisStepFlag = 0;							// ���������еı��
	if(bAxisDir==0)									// ע���� ComDataProc.c ���Ѿ��ж��˷��򣬵��������жϸ���
	{
		bAxisDIR_ON;
	}
	else
	{
		bAxisDIR_OFF;
	}
}

// ����ͬ���Ĳ���
u16 correctSync(u16 nAxisMinClk, u8 nAxis)
{
	// ����ͬ���Ĳ���
	// �����ķ�ʽ����Ҫ�о�����ʱ��д��� byYJY
	if((5 > maxClkNum) && (nAxisMinClk != 0) && (max_Clk != nAxisMinClk))
		return (nAxisMinClk*minSpeedTimeCnt[nAxis]*setAddTime()/2e6 + 1);					// ��1����С��
	else
		return 0;									
}

// ����ÿ�η�����������Ҫ��ʱ��_ ������ time��λΪus��������1e6
u16 calPWM_TimeCnt(u16 clk, u16 time)
{
	return (1e6 / (clk * time));
}

// �������Ӽ��ٵ����Ƶ��
void correctNoAddSubClk(u8 nAxis, u32 plusNum)
{
	u16 clk;
	float time = calTotalTime(maxClkNum);						// ��ʱ�������Ƶ�ʵ������
	if(0 == (int)time)
		return;
	
	clk = (int)((float)(1e6*plusNum) / time);
	if(0 == clk)
		return;
	
	switch(nAxis)
	{
		case 0:
			if(xAxisClk > clk)
				xAxisClk = clk;
		break;
			
		case 1:
			if(yAxisClk > clk)
				yAxisClk = clk;
		break;
			
		case 2:
			if(zAxisClk > clk)
				zAxisClk = clk;
		break;
			
		case 3:
			if(aAxisClk > clk)
				aAxisClk = clk;
		break;
			
		case 4:
			if(bAxisClk > clk)
				bAxisClk = clk;
		break;
			
		default:
			break;					
	}	
}
// ������ٽ׶���Ҫ��������(������)
u16 calSubPlusCnt(u8 nAxis, u16 nAxisClk)
{
	u16 min_Clk;
	float timeAddSub;
	u16 addSubPlusCnt;
	
	switch(nAxis)
	{
		case 0:
			min_Clk = X_AXIS_MIN_CLK;
		break;
		
		case 1:
			min_Clk = Y_AXIS_MIN_CLK;
		break;
		
		case 2:
			min_Clk = Z_AXIS_MIN_CLK;
		break;
		
		case 3:
			min_Clk = A_AXIS_MIN_CLK;
		break;
		
		case 4:
			min_Clk = B_AXIS_MIN_CLK;
		
		default:
			return 0;
	}
	
	timeAddSub = (nAxisClk - min_Clk) * setAddTime();		// ��λus
	addSubPlusCnt = (nAxisClk + min_Clk) / 2.0 * timeAddSub * 1e-6;		// 1e-6����Ϊ��us��s
	return addSubPlusCnt;
}
//u16 calSubPlusCnt(u8 nAxis)
//{
//	u16 min_Clk;
//	float timeAddSub;
//	u16 addSubPlusCnt;
//	
//	switch(nAxis)
//	{
//		case 0:
//			min_Clk = X_AXIS_MIN_CLK;
//		break;
//		
//		case 1:
//			min_Clk = Y_AXIS_MIN_CLK;
//		break;
//		
//		case 2:
//			min_Clk = Z_AXIS_MIN_CLK;
//		break;
//		
//		case 3:
//			min_Clk = A_AXIS_MIN_CLK;
//		break;
//		
//		case 4:
//			min_Clk = B_AXIS_MIN_CLK;
//		
//		default:
//			return 0;
//	}
//	
//	timeAddSub = (max_Clk - min_Clk) * setAddTime();		// ��λus
//	addSubPlusCnt = (max_Clk + min_Clk) / 2.0 * timeAddSub * 1e-6;		// 1e-6����Ϊ��us��s
//	return addSubPlusCnt;
//}

// ����һ֡��Ҫ��ʱ�䣬 ��λus
float calTotalTime(u8 nAxis)
{
	u16 min_Clk;
	float timeAddSub;
	float plusNum;
	switch(nAxis)
	{
		case 0:
			min_Clk = X_AXIS_MIN_CLK;
			plusNum = xAxisPlusNum;
		break;
		
		case 1:
			min_Clk = Y_AXIS_MIN_CLK;
			plusNum = yAxisPlusNum;
		break;
		
		case 2:
			min_Clk = Z_AXIS_MIN_CLK;
			plusNum = zAxisPlusNum;
		break;
		
		case 3:
			min_Clk = A_AXIS_MIN_CLK;
			plusNum = aAxisPlusNum;
		break;
		
		case 4:
			min_Clk = B_AXIS_MIN_CLK;
			plusNum = bAxisPlusNum;
		break;
		
		default:
			return 0;
	}
	
	// �Ӽ�������ʱ�� us
	timeAddSub = (max_Clk - min_Clk) * setAddTime();									
	return ((float)(1e6*plusNum - (max_Clk+min_Clk)*timeAddSub)/(float)max_Clk + 2 * timeAddSub);
	// ������timeAddSub����2��ʹ�ò���ͬ����ԭ��δ֪  byYJY
//	return ((float)(1e6*plusNum - (max_Clk+min_Clk)*timeAddSub/2)/(float)max_Clk + 2 * timeAddSub);
}

// ����ռ�ձȵĺ���
u16 calDutyCycle(u16 clk)
{
	u16 dutyCycleCnt;
	dutyCycleCnt = (PLUS_WIDTH * ARR_VAL * clk) / 1e6;
	if(0 == dutyCycleCnt)																		// ռ�ձȲ���Ϊ0
		dutyCycleCnt = 1;	
	return dutyCycleCnt;
}

// ����ÿ�β�����Ƶ��
void calClkPerStep(void)
{
	clkPerStep = (xAxisClk - xAxisClk_Cur) / (ADDTIME * 1000 / addtime);		// ��ʱ�����Ǽ������
}

// ͨ��CLKƵ�ʼ���Ԥ��Ƶϵ�� PSC
u16 calPSC(u16 clk)
{
	u16 psc;
	psc = PSC_CLK / clk;
	return psc;
}

//// ���üӼ��ٵ��㷨�ṹ��
//void setAddSubSetting(void)
//{
//	u16 maxClk = preProcMaxClk_Cur();
//	u16 minClk = preProcMinClk_Cur();
//	adjustClk(maxClk, minClk);
//	
//	/////////////
//}

// Ƶ�ʵ�����ʹ��Ƶ��λ�ں��ʷ�Χ�ڡ�0������ok��1���������������Ƶ������  byYJY
u8 adjustClk(u16 maxClk, u16 minClk)
{
	u8 status = 0;
	if(xAxisClk == maxClk)								// X�����ֵ
	{
		if(maxClk > X_AXIS_MAX_CLK)					// ��Ҫ��Ƶ
		{
			underClk();
			maxClk = preProcMaxClk_Cur();
			minClk = preProcMinClk_Cur();
			adjustClk(maxClk, minClk);
		} else if(maxClk < X_AXIS_MIN_CLK)	// ��Ҫ��Ƶ
		{
//			overClk();											// ����Ƶ
//			maxClk = preProcMaxClk_Cur();
//			minClk = preProcMinClk_Cur();
//			adjustClk(maxClk, minClk);
		}
	}
	
	if(yAxisClk == maxClk)								// y�����ֵ
	{
		if(maxClk > Y_AXIS_MAX_CLK)					// ��Ҫ��Ƶ
		{
			underClk();
			maxClk = preProcMaxClk_Cur();
			minClk = preProcMinClk_Cur();
			adjustClk(maxClk, minClk);
		} else if(maxClk < Y_AXIS_MIN_CLK)	// ��Ҫ��Ƶ
		{
//			overClk();
//			maxClk = preProcMaxClk_Cur();
//			minClk = preProcMinClk_Cur();
//			adjustClk(maxClk, minClk);
		}
	}
	
	if(zAxisClk == maxClk)								// z�����ֵ
	{
		if(maxClk > Z_AXIS_MAX_CLK)					// ��Ҫ��Ƶ
		{
			underClk();
			maxClk = preProcMaxClk_Cur();
			minClk = preProcMinClk_Cur();
			adjustClk(maxClk, minClk);
		} else if(maxClk < Z_AXIS_MIN_CLK)	// ��Ҫ��Ƶ
		{
//			overClk();
//			maxClk = preProcMaxClk_Cur();
//			minClk = preProcMinClk_Cur();
//			adjustClk(maxClk, minClk);
		}
	}
	
	if(aAxisClk == maxClk)								// a�����ֵ
	{
		if(maxClk > A_AXIS_MAX_CLK)					// ��Ҫ��Ƶ
		{
			underClk();
			maxClk = preProcMaxClk_Cur();
			minClk = preProcMinClk_Cur();
			adjustClk(maxClk, minClk);
		} else if(maxClk < A_AXIS_MIN_CLK)	// ��Ҫ��Ƶ
		{
//			overClk();
//			maxClk = preProcMaxClk_Cur();
//			minClk = preProcMinClk_Cur();
//			adjustClk(maxClk, minClk);
		}
	}
	
	if(bAxisClk == maxClk)								// b�����ֵ
	{
		if(maxClk > B_AXIS_MAX_CLK)					// ��Ҫ��Ƶ
		{
			underClk();
			maxClk = preProcMaxClk_Cur();
			minClk = preProcMinClk_Cur();
			adjustClk(maxClk, minClk);
		} else if(maxClk < B_AXIS_MIN_CLK)	// ��Ҫ��Ƶ
		{
//			overClk();
//			maxClk = preProcMaxClk_Cur();
//			minClk = preProcMinClk_Cur();
//			adjustClk(maxClk, minClk);
		}
	}	
	
	// ��Сֵ��Ϊ�����ж�����
	if(0 != minClk)
	{
		if(xAxisClk == minClk)								// x��Ϊ��Сֵ
		{
			if(minClk < X_AXIS_MIN_CLK)					// ��Сֵ��С
			{
				if(ifOverClkOK(maxClk))									// �ж��Ƿ������Ƶ
				{
					overClk();
					maxClk = preProcMaxClk_Cur();
					minClk = preProcMinClk_Cur();
					adjustClk(maxClk, minClk);
				} else {
					status = 1;											// ����������������Ƶ������
				}
			}
		}
		
		if(yAxisClk == minClk)								// y��Ϊ��Сֵ
		{
			if(minClk < Y_AXIS_MIN_CLK)					// ��Сֵ��С
			{
				if(ifOverClkOK(maxClk))									// �ж��Ƿ������Ƶ
				{
					overClk();
					maxClk = preProcMaxClk_Cur();
					minClk = preProcMinClk_Cur();
					adjustClk(maxClk, minClk);
				} else {
					status = 1;											// ����������������Ƶ������
				}
			}
		}
		
		if(zAxisClk == minClk)								// z��Ϊ��Сֵ
		{
			if(minClk < Z_AXIS_MIN_CLK)					// ��Сֵ��С
			{
				if(ifOverClkOK(maxClk))									// �ж��Ƿ������Ƶ
				{
					overClk();
					maxClk = preProcMaxClk_Cur();
					minClk = preProcMinClk_Cur();
					adjustClk(maxClk, minClk);
				} else {
					status = 1;											// ����������������Ƶ������
				}
			}
		}
		
		if(aAxisClk == minClk)								// a��Ϊ��Сֵ
		{
			if(minClk < A_AXIS_MIN_CLK)					// ��Сֵ��С
			{
				if(ifOverClkOK(maxClk))									// �ж��Ƿ������Ƶ
				{
					overClk();
					maxClk = preProcMaxClk_Cur();
					minClk = preProcMinClk_Cur();
					adjustClk(maxClk, minClk);
				} else {
					status = 1;											// ����������������Ƶ������
				}
			}
		}
		
		if(bAxisClk == minClk)								// b��Ϊ��Сֵ
		{
			if(minClk < B_AXIS_MIN_CLK)					// ��Сֵ��С
			{
				if(ifOverClkOK(maxClk))									// �ж��Ƿ������Ƶ
				{
					overClk();
					maxClk = preProcMaxClk_Cur();
					minClk = preProcMinClk_Cur();
					adjustClk(maxClk, minClk);
				} else {
					status = 1;											// ����������������Ƶ������
				}
			}
		}
	}	
	return status;
}

// �Ƿ������Ƶ���ж� 0:�����ԣ� 1������
u8 ifOverClkOK(u16 maxClk)
{
	u8 result = 1;
	if((xAxisClk << 1) > X_AXIS_MAX_CLK)		// ��Ƶ��������Ƶ�ʣ���������Ƶ
	{
		result = 0;
		return result;
	}
	if((yAxisClk << 1) > Y_AXIS_MAX_CLK)		// ��Ƶ��������Ƶ�ʣ���������Ƶ
	{
		result = 0;
		return result;
	}
	if((zAxisClk << 1) > Z_AXIS_MAX_CLK)		// ��Ƶ��������Ƶ�ʣ���������Ƶ
	{
		result = 0;
		return result;
	}
	if((aAxisClk << 1) > A_AXIS_MAX_CLK)		// ��Ƶ��������Ƶ�ʣ���������Ƶ
	{
		result = 0;
		return result;
	}
	if((bAxisClk << 1) > B_AXIS_MAX_CLK)		// ��Ƶ��������Ƶ�ʣ���������Ƶ
	{
		result = 0;
		return result;
	}	
	return result;
}

// ��Ƶ���о��ȶ�ʧ
void underClk(void)
{
	xAxisClk = (xAxisClk >> 1);
	yAxisClk = (yAxisClk >> 1);
	zAxisClk = (zAxisClk >> 1);
	aAxisClk = (aAxisClk >> 1);
	bAxisClk = (bAxisClk >> 1);
}
// ��Ƶ���ٶ�̫��ʱʹ�ã����ȼ���
void overClk(void)
{
	xAxisClk = (xAxisClk << 1);
	yAxisClk = (yAxisClk << 1);
	zAxisClk = (zAxisClk << 1);
	aAxisClk = (aAxisClk << 1);
	bAxisClk = (bAxisClk << 1);
}

// �������ʱ�������е������� timeCnt Ϊ addtime�ı���
u32 calPlusNum(u16 clk, u8 timeCnt)
{
	u32 plusNum;
	plusNum = (timeCnt * addSubSetting.addSubTimePerStep) / 1000;		// ע����ܶ�ʧ����
	return plusNum;
}

// ��������ʱ�䣬��������
float calTime(u32 plusNum, u16 clk)
{
	float time;
	time = ((float)plusNum * (float)1000.0) / (float)clk;				// ��λms
	return time;
}

// û�мӼ���
void noAddSubSpeed(void)
{
	xAxisClkSet(xAxisClk);
	yAxisClkSet(yAxisClk);
	zAxisClkSet(zAxisClk);
	aAxisClkSet(aAxisClk);
	bAxisClkSet(bAxisClk);
}

// �յ�������Ϣ�ظ� 0��not ok�� 1��ok
void respMsg(u8 status)
{
	u8 i;
	u8 backResString[9];
	
	backResString[0] = FrameB1;
	backResString[1] = FrameB2;
	backResString[2] = DATAINFO;
	
	backResString[3] = 9;
	backResString[4] = 0x00;
	
	backResString[5] = 0x22;
	backResString[6] = 0x00;
	
	backResString[7] = status;					// �Ƿ�ok
	backResString[8] = FrameEnd;
	
	// ���Ϊ0�Żظ�
	if(0 == respFlag)
	{
		for(i=0;i<9;i++)
			{
				USART_SendData(USART3,backResString[i]); 
				while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);	//�ȴ����ͽ���
			}
	}
}

// ������Ϣ��ӡ
void printf_error(void)
{
	
}

// �Ӽ��ٲ��Գ�ʼ��
void addSubSetting_Init(u8 addCount)
{
	addSubSetting.addCount = addCount;
	addSubSetting.clkPerStep = (u16 *)malloc(addCount * sizeof(u16));
	if(!addSubSetting.clkPerStep)										// �����ڴ�ʧ��
	{
		printf_error();
		ifUseAddSubFlag = 1;													// ��ʹ�üӼ���
		return;
	}
	addSubSetting.addSubTimePerStep	= setAddTime();	// ���üӼ��ٵ�ÿ��������ʱ��
	addSubSetting.constSpeedCnt = 0;								// �����˶�ʱ��
}

// �����λ�������������Ƿ�������ʱûд
u8 checkUartData(void)
{
	return 1;
}

// ���üӼ��ٵ�ÿ��������ʱ��
u8 setAddTime(void)
{
	// �㷨δ������ʱ���� addtime
	return addtime;
}

// �ͷŶ�̬����
void addSubSetting_Free(void)
{
	addSubSetting.addCount = 0;
	if(NULL != addSubSetting.clkPerStep)
	{
		free(addSubSetting.clkPerStep);
		addSubSetting.clkPerStep = NULL;
	}
	addSubSetting.addSubTimePerStep = 0;
	addSubSetting.constSpeedCnt = 0;
}

// ���ת�ٶ�Ӧ���� ��������ᶼ�����Ƶ�ʣ��źſ��������
void nAxisMaxClkNum(const u16 maxClk)
{
	if(maxClk == xAxisClk)
		maxClkNum = 0;
	else if(maxClk == yAxisClk)
		maxClkNum = 1;
	else if(maxClk == zAxisClk)
		maxClkNum = 2;
	else if(maxClk == aAxisClk)
		maxClkNum = 3;
	else if(maxClk == bAxisClk)
		maxClkNum = 4;
	else 
		maxClkNum = 5;							// 5��ʾ����
}
// ������Ƶ��
u16 preProcMaxClk_Cur(void)
{
	u16 maxClk;
	maxClk = max(yAxisClk,xAxisClk);
	maxClk = max(maxClk,zAxisClk);
	maxClk = max(maxClk,aAxisClk);
	maxClk = max(maxClk,bAxisClk);
	nAxisMaxClkNum(maxClk);					// ͬʱ������ĸ��������Ƶ��
	return maxClk;
}

// ������������
u16 preProcMaxNum_Cur(void)
{
	u16 maxPlusNum;
	maxPlusNum = max(yAxisPlusNum,xAxisPlusNum);
	maxPlusNum = max(maxPlusNum,zAxisPlusNum);
	maxPlusNum = max(maxPlusNum,aAxisPlusNum);
	maxPlusNum = max(maxPlusNum,bAxisPlusNum);
	return maxPlusNum;
}

// �����СƵ��
u16 preProcMinClk_Cur(void)
{
	u16 minClk = 0;
	if((0 != yAxisClk) && (0 != xAxisClk))
		minClk = min(yAxisClk,xAxisClk);
	if(0 != zAxisClk)
		minClk = min(minClk,zAxisClk);
	if(0 != aAxisClk)
		minClk = min(minClk,aAxisClk);
	if(0 != bAxisClk)
		minClk = min(minClk,bAxisClk);
	return minClk;
}

// �����С������
u16 preProcMinNum_Cur(void)
{
	u16 minPlusNum = 0;
	if((0 != yAxisClk) && (0 != xAxisClk))
		minPlusNum = min(yAxisPlusNum,xAxisPlusNum);
	if(0 != zAxisClk)
		minPlusNum = min(minPlusNum,zAxisPlusNum);
	if(0 != aAxisClk)
		minPlusNum = min(minPlusNum,aAxisPlusNum);
	if(0 != bAxisClk)
		minPlusNum = min(minPlusNum,bAxisPlusNum);
	return minPlusNum;
}
