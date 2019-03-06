/********************************************************************************************************
*    S�ͼӼ����㷨 2019��2��13��09:02:11 byYJY
*    ASS : AddSubSpeed �Ӽ�����д
********************************************************************************************************/

#include "AddSubSpeed_S.h"


//extern 											Plus_Data cmd_Plus_Data;						// �������ݣ����Ƶ���˶�
//extern 											Proc_Data cmd_Proc_Data; 						// �������ݣ��г�Աָ��plus_Data
extern 		FunctionalState	 			nAxisStatus[AXIS_NUM];  						// �����Ƿ�����˶��ı�־
extern 		NAxis_TIM_Structure		nAxis_TIM_Structure[AXIS_NUM];  		// PWM ��ʼ���Ľṹ��  
extern 		vu32 									plusNumPWM[AXIS_NUM]; 							// ���������жϼ���
extern 		vu16 									addSubSpeed_StepNum[AXIS_NUM]; 			// ��¼�Ӽ��ٶ�ʱ�����еĲ���
extern 		Flag_Structure 				flag_Struct;
extern		const	 u32	 					Backlash_PlusNum;
extern		BacklashCompensation_Structure		backlashCompen;


// �˶�����
Motion_Strcuture 	motion_Data;	
Motion_Strcuture 	motion_Data_Pre;	


// �˶����ݵĳ�ʼ��
void motion_Init(void)
{
	mymemcpy(&motion_Data, &motion_Data_Pre, sizeof(motion_Data));
	
	// ��־λ
	
	
	
	// �趨�˶�����
	setStepMotorDir();
	delay_us(DIR_EX_DALAY); 			// һ����ʱ
	
	// ������˶�״̬��ʼ��
	nAxisMotion_Init();
}


// �Ӽ�����ز������õĳ�ʼ��
void AddSubSpeedInit_Pre(void)
{
	// ��������˶�����
	resetMotionData();
	
	// ����Ƶ�ʵ����ʷ�Χ
	adjustClk();
	
	// ����Ӽ�����ηֶ�
	cal_AddSubSpeed_Div();
	
	// ����S�����ߵ�PSC����
	cal_S_Line();	
}

// ������˶�״̬��ʼ��
static void	nAxisMotion_Init(void)
{
	u8 i;
	
	// ����״̬��ʼ�� �����˶�
	for(i=0; i<AXIS_NUM; i++)
	{
		plusNumPWM[i] = 0;
		addSubSpeed_StepNum[i] = ADD_SPEED;
		nAxisStatus[i] = ENABLE;
		
		// �������˶�����ı�
		if(SET == backlashCompen.DirChange_Flag[i])
		{
			backlashCompen.DirChange_Flag[i] = RESET;
			
			motion_Data.cmd_Datas.plus_Datas.plusNum[i] += Backlash_PlusNum;			
		}
		
#if NO_ADDSUBSPEED
//		nAxisClk_Cur[i] = cmd_Plus_Data.clk[i];
		nAxisClk_Cur[i] = n_Axis_Min_Clk(i);
#else
//		nAxisClk_Cur[i] = () ? : ;
#endif
		nAxisSetPWM(nAxis_TIM_Structure[i].TIM_N, calPSC(motion_Data.nAxisClk_Cur[i], i));	
		
		// ���������¼������¼Ĵ���
		TIM_GenerateEvent(nAxis_TIM_Structure[i].TIM_N, TIM_EventSource_Update);
	}
	delay_ms(1); 		// һ���ӳ٣�ʹ��PSCֵ���ú�
}

// ��������˶�����
static void resetMotionData(void)
{	
	//comData_Init();		// �ڸú����Ѿ���ʼ���������ٴγ�ʼ��
	
	motion_Data_Pre.maxClkNum = AXIS_NUM;
	motion_Data_Pre.addSubTime = TIM_OUT;
}

// �趨�˶�����mode���趨�Ƿ񲹳���϶
static void	setStepMotorDir(void)
{
	u8 i;
	for(i=0; i<AXIS_NUM; i++)
	{
		if(TBD_DIR != motion_Data.cmd_Datas.plus_Datas.dir[i])
		{
			Motor_Dir_Set(StepMotor_Dir+i, motion_Data.cmd_Datas.plus_Datas.dir[i]);
			
			// ������
			if(ENABLE == backlashCompen.DirChange_En)
			{
				if(backlashCompen.motorDirOld[i] != motion_Data.cmd_Datas.plus_Datas.dir[i])
				{
					backlashCompen.motorDirOld[i] = motion_Data.cmd_Datas.plus_Datas.dir[i];
					
					backlashCompen.DirChange_Flag[i] = SET;
				}
			}
		}
	}
}


// ����Ӽ�����ηֶ�
static void cal_AddSubSpeed_Div(void)
{
	u8 i;
	float maxClk_length; 			// �Ӽ��ٲ���
	u8 maxClkNum = motion_Data_Pre.maxClkNum;
//	float correct_Value;			// ����clk�ı���
	
	// ���Ƶ����������Ϊ�ֶ����� ���������BUG  byYJY
	maxClk_length = (motion_Data_Pre.cmd_Datas.plus_Datas.clk[maxClkNum] - n_Axis_Min_Clk(maxClkNum)) / AddSub_Step_DIV;
	
	if((AXIS_NUM > maxClkNum) && (maxClk_length < MIN_STEP_NUM))		// maxClk_length̫С��Ϊ��ʱ���Ӽ���
	{
		// �����᲻ʹ�üӼ����˶�
		En_ASS_Flag(DISABLE, ALL_Axis);
		return;
	}
	
	// test ������С��500�Ķ����Ӽ���
//	if(cmd_Plus_Data.plusNum[maxClkNum] < 500)
//	{
//		correct_Value = cmd_Plus_Data.clk[maxClkNum] / n_Axis_Min_Clk(maxClkNum);
//		for(i=0; i<AXIS_NUM; i++)
//		{
//			cmd_Plus_Data.clk[i] = cmd_Plus_Data.clk[i] / correct_Value;

//			if(50 > cmd_Plus_Data.clk[i])
//			{
//				cmd_Plus_Data.clk[i] = n_Axis_Min_Clk(i);
//			}
//			
//			Psc_Data_Cur[i].enAddSubFlag = DISABLE;
//		}
//		return;
//	}
	
	// ����������Ĳ���
	for(i=0; i<AXIS_NUM; i++)
	{
		// �ٶ�̫Сʱ�������мӼ���
		if(motion_Data_Pre.cmd_Datas.plus_Datas.clk[i] < n_Axis_Min_Clk(i))
		{
			En_ASS_Flag(DISABLE, (N_Axis)i);
			motion_Data_Pre.PSC_Data[i].length = 0;									
//			continue;
		} 
		else
		{
			En_ASS_Flag(ENABLE, (N_Axis)i);
			motion_Data_Pre.PSC_Data[i].length = maxClk_length;			// ͬ���ļӼ��ٲ���
		}
	}
}



// Ƶ�ʵ�����ʹ��Ƶ��λ�ں��ʷ�Χ�ڡ�0������ok��1���������������Ƶ������
static u8 adjustClk(void)
{
	u8 i;
	u8 status = 0;
	motion_Data_Pre.maxClk = preProcMaxClk();
	motion_Data_Pre.minClk = preProcMinClk(motion_Data_Pre.maxClk);
	
	// ���ֵ�ж�
	for(i=0; i<AXIS_NUM; i++)
	{
		if(motion_Data_Pre.cmd_Datas.plus_Datas.clk[i] == motion_Data_Pre.maxClk) 				// ���Ƶ��
		{
			if(motion_Data_Pre.maxClk > n_Axis_Max_Clk(i)) 					// ��Ҫ��Ƶ
			{
				reduceClk();
				motion_Data_Pre.maxClk = preProcMaxClk();
				motion_Data_Pre.minClk = preProcMinClk(motion_Data_Pre.maxClk);
				adjustClk();
			} 
			else if(motion_Data_Pre.maxClk < n_Axis_Max_Clk(i)) 		// ��Ҫ��Ƶ
			{
				// nope;
			}
		}
	}
	
	// ��Сֵ�ж�
	if(0 != motion_Data_Pre.minClk)
	{
		for(i=0; i<AXIS_NUM; i++)
		{
			if(motion_Data_Pre.cmd_Datas.plus_Datas.clk[i] == motion_Data_Pre.minClk) 				// ��СƵ��
			{
				if(motion_Data_Pre.minClk < n_Axis_Min_Clk(i))   			// ��Сֵ��С
				{
					if(ENABLE == ifBoostClkOK()) 			// ������Ƶ
					{
						boostClk();
						motion_Data_Pre.maxClk = preProcMaxClk();
						motion_Data_Pre.minClk = preProcMinClk(motion_Data_Pre.maxClk);
						adjustClk();
					}
					else															// ��������Ƶ
					{
						status = 1;											// ����������������Ƶ������
					}
				}
			}
		}
	}	
	return status;
}

// �������S�ͼ�������
static void cal_S_Line(void)
{
	u8 i;
//	u16 minStepNum_Length; 			// ��С�Ĳ�����	
	float needPlusNum, needTotalTime;
	float freq_Temp;
	u8 maxClkNum = motion_Data_Pre.maxClkNum;
	
	/* �����Ƶ�����ڵ�����ΪS���ߵ��趨��׼  */
	// ���Ƶ����ļӼ��ٹر�
	if(DISABLE == motion_Data_Pre.PSC_Data[maxClkNum].enAddSubFlag)
	{
		for(i=0; i<AXIS_NUM; i++)
		{
			motion_Data_Pre.nAxisClk_Cur[i] = motion_Data_Pre.cmd_Datas.plus_Datas.clk[i];
		}
		
		return;
	}
	// ���Ƶ�����S�����߼������
	if(DISABLE == calSModelLine(freq_Temp, motion_Data_Pre.PSC_Data[maxClkNum].psc_data, motion_Data_Pre.PSC_Data[maxClkNum].length,
				motion_Data_Pre.cmd_Datas.plus_Datas.clk[maxClkNum], n_Axis_Min_Clk(maxClkNum), S_FLEXIBLE))
	{
		En_ASS_Flag(DISABLE ,ALL_Axis);
		
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[maxClkNum] = n_Axis_Min_Clk(maxClkNum);			// ��������Ƶ�ʽ���
		
		needTotalTime = calTotalNeedTime(maxClkNum);		
		
		// �����������Ƶ��
		for(i=0; i<AXIS_NUM; i++)
		{
			if(i == maxClkNum)
			{
				motion_Data_Pre.nAxisClk_Cur[i] = n_Axis_Min_Clk(i);
				continue;
			}
			
			CorrectClk(i, needTotalTime);
		}
		return;
	}
	
	// ��������Ҫ�����������浽�ṹ���У���Ϊ�жϼ��ٽ׶ε�����
	needPlusNum = calAddSpeed_NeedPlusNum(motion_Data_Pre.PSC_Data[maxClkNum].psc_data, motion_Data_Pre.PSC_Data[maxClkNum].length);		
	motion_Data_Pre.PSC_Data[maxClkNum].addSpeed_NeedPlusNum = needPlusNum;
	
	// ���Ƶ�������������������һ�������ĵļӼ��٣��Ǿͽ��Ӽ��ٹر�
	if((2 * needPlusNum + ConstS_NeedPlus) > motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[maxClkNum])
	{
		En_ASS_Flag(DISABLE ,ALL_Axis);
		
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[maxClkNum] = n_Axis_Min_Clk(maxClkNum);			// ��������Ƶ�ʽ���
		
		needTotalTime = calTotalNeedTime(maxClkNum);		
		
		// �����������Ƶ��
		for(i=0; i<AXIS_NUM; i++)
		{
			if(i == maxClkNum)
			{
				motion_Data_Pre.nAxisClk_Cur[i] = n_Axis_Min_Clk(i);
				continue;
			}
			
			CorrectClk(i, needTotalTime);
		}
		
		return;
	}	
	
	// �����������Ҫ������ʱ�䣬��λus
	needTotalTime = calTotalNeedTime(maxClkNum);
	
	for(i=0; i<AXIS_NUM; i++)
	{
		if(maxClkNum == i) 		// �����
		{
			motion_Data_Pre.nAxisClk_Cur[i] = n_Axis_Min_Clk(i);
			continue;
		}
		
		if(ENABLE == motion_Data_Pre.PSC_Data[i].enAddSubFlag)
		{
			if(DISABLE == calSModelLine(freq_Temp, motion_Data_Pre.PSC_Data[i].psc_data, motion_Data_Pre.PSC_Data[i].length,
				motion_Data_Pre.cmd_Datas.plus_Datas.clk[i], n_Axis_Min_Clk(i), S_FLEXIBLE))
			{
				En_ASS_Flag(DISABLE, (N_Axis)i);
				
				CorrectClk(i, needTotalTime);
				
				continue;
			}
			
			// ��������Ҫ�����������浽�ṹ���У���Ϊ�жϼ��ٽ׶ε�����
			needPlusNum = calAddSpeed_NeedPlusNum(motion_Data_Pre.PSC_Data[i].psc_data, motion_Data_Pre.PSC_Data[i].length);			
			motion_Data_Pre.PSC_Data[i].addSpeed_NeedPlusNum = needPlusNum;
			
			// ����ܵ���������������һ�������Ӽ��٣��Ǿͽ��Ӽ��ٹر�
			if((2 * needPlusNum + ConstS_NeedPlus) > motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[i])
			{
				En_ASS_Flag(DISABLE, (N_Axis)i);
				
				CorrectClk(i, needTotalTime);				
			}
		}
		else if(DISABLE == motion_Data_Pre.PSC_Data[i].enAddSubFlag) 			// ���Ӽ���
		{
			//  �������Ƶ�ʻ��ѵ�ʱ������Ƶ��
			CorrectClk(i, needTotalTime);
		}
	}
}

/* 	����S�ͼ������ߵĺ��� */
static FunctionalState calSModelLine(float fre, u16 period[], float len, 
	float fre_max, float fre_min, float flexible)
{
	int i;
	float deno;
	float melo;
	float delt = fre_max - fre_min;
	
	// byYJY ///////////////////////////////
//	mymemset(period, 0, DATA_LENGTH * sizeof(u16));
	
	// �жϲ����Ƿ�ϸ�
	if((len < 0) || (fre_max < fre_min))
		return DISABLE;
	
	for(i=0; i<len; i++)
	{
		melo = flexible * (i-len/2) / (len/2);
		deno = (float)(1.0 / (1.0 + (double)expf(-melo))); 
		fre = delt * deno + fre_min;
		period[i] = (u16)(PSC_CLK / fre);
	}
	i=i;
	return ENABLE;
}

// ������Ƶ��
static u32 preProcMaxClk(void)
{
	volatile u8 i;
	u32 max_Clk = motion_Data_Pre.cmd_Datas.plus_Datas.clk[0];
	for(i=1; i<AXIS_NUM; i++)
	{
		max_Clk = max(max_Clk, motion_Data_Pre.cmd_Datas.plus_Datas.clk[i]);
	}
	// �ҵ����Ƶ�ʶ�Ӧ����
	for(i=0; i<AXIS_NUM; i++)
	{
		if(max_Clk == motion_Data_Pre.cmd_Datas.plus_Datas.clk[i])
			motion_Data_Pre.maxClkNum = i;
	}
	
	return max_Clk;
}

// ��÷������СƵ�� �������ֵ�ٵ���
static u32 preProcMinClk(u32 max_Clk)
{
	volatile u8 i;
	u32 min_Clk = 0;
	
	for(i=0; i<AXIS_NUM; i++)
	{
		if((0 == min_Clk) && (max_Clk != motion_Data_Pre.cmd_Datas.plus_Datas.clk[i]))
		{
			min_Clk = motion_Data_Pre.cmd_Datas.plus_Datas.clk[i];			
		}
		else if(0 != motion_Data_Pre.cmd_Datas.plus_Datas.clk[i])
		{
			min_Clk = min(min_Clk, motion_Data_Pre.cmd_Datas.plus_Datas.clk[i]);
		}
	}
	return min_Clk;
}

// ��Ƶ���о��ȶ�ʧ
static void reduceClk(void)
{
	u8 i;
	for(i=0; i<AXIS_NUM; i++)
	{
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[i] = (motion_Data_Pre.cmd_Datas.plus_Datas.clk[i] >> 1);
	}
}
// ��Ƶ���ٶ�̫��ʱʹ�ã����ȼ���
static void boostClk(void)
{
	u8 i;
	for(i=0; i<AXIS_NUM; i++)
	{
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[i] = (motion_Data_Pre.cmd_Datas.plus_Datas.clk[i] << 1);
	}
}

// ���ص�ǰ������������СƵ��
u32 n_Axis_Max_Clk(u8 i)
{
	switch(i)
	{
		case 0:
			return X_AXIS_MAX_CLK;
		case 1:
			return Y_AXIS_MAX_CLK;
		case 2:
			return Z_AXIS_MAX_CLK;
		case 3:
			return A_AXIS_MAX_CLK;
		case 4:
			return B_AXIS_MAX_CLK;
		default:
			return 0x0;
	}
}
u32 n_Axis_Min_Clk(u8 i)
{
	switch(i)
	{
		case 0:
			return X_AXIS_MIN_CLK;
		case 1:
			return Y_AXIS_MIN_CLK;
		case 2:
			return Z_AXIS_MIN_CLK;
		case 3:
			return A_AXIS_MIN_CLK;
		case 4:
			return B_AXIS_MIN_CLK;
		default:
			return 0x0;
	}
}

// �Ƿ������Ƶ���ж� 
static FunctionalState ifBoostClkOK(void)
{
	u8 i;
	
	for(i=0; i<AXIS_NUM; i++)
	{
		if((motion_Data_Pre.cmd_Datas.plus_Datas.clk[i] << 1) > n_Axis_Max_Clk(i)) // ��Ƶ��������Ƶ�ʣ���������Ƶ
		{
			return DISABLE;
		}
	}	
	
	return ENABLE;
}

// ���㵱ǰ�����£���ɼ�������Ҫ��������  
// ��ע���£� https://blog.csdn.net/gtkknd/article/details/52199963 byYJY
static float calAddSpeed_NeedPlusNum(u16 period[], float len)
{
	u16 i;
	u32 clk;
	float needPlusNum = 0.0;
	u16 length = (int)len;	
	
	for(i=0; i<length; i++)
	{
		clk = calClk_PSC(period[i]);
		needPlusNum += motion_Data_Pre.addSubTime * clk;			// ע�⣺addSubTime ��λΪus
	}
	needPlusNum *= (float)1e-6;								
	
	return needPlusNum;
}

// ����ָ������������Ҫ��ʱ�� ��λus ȷ���������ߺ��ٵ���
static float calTotalNeedTime(u8 nAxis)
{	
	u32 i;
	u32 maxStep;
	float constSpeedTime; 		// ���ٽ׶���Ҫ��ʱ��
	float plus_Num;						// ������������
	
	if(DISABLE == motion_Data_Pre.PSC_Data[nAxis].enAddSubFlag) 		// δ�����Ӽ���
		return (motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[nAxis] * MHz_2_Hz / motion_Data_Pre.cmd_Datas.plus_Datas.clk[nAxis]);
	
	
	maxStep = (int)(motion_Data_Pre.PSC_Data[nAxis].length - 1);
	// ���ٽ׶�����˶�
	if(motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[nAxis] < motion_Data_Pre.PSC_Data[nAxis].addSpeed_NeedPlusNum)
	{
		i = 0;
		
		plus_Num = 0;
		while(plus_Num < motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[nAxis])
		{
			if(i >=  motion_Data_Pre.PSC_Data[nAxis].length) 		// iֵ����
			{
				plus_Num += calClk_PSC(motion_Data_Pre.PSC_Data[nAxis].psc_data[maxStep]) * motion_Data_Pre.addSubTime / MHz_2_Hz;
				respMsgError("��������ʱ���쳣!\r\n", 1);
			}
			else 
			{
				plus_Num += calClk_PSC(motion_Data_Pre.PSC_Data[nAxis].psc_data[i]) * motion_Data_Pre.addSubTime / MHz_2_Hz;
			}
			i++;
		}
		
		return (i * motion_Data_Pre.addSubTime);
	}
	
	// ���ٽ׶�����˶�
	else if((motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[nAxis] < (motion_Data_Pre.PSC_Data[nAxis].addSpeed_NeedPlusNum) * 2))
	{
		i = 0;
		plus_Num = motion_Data_Pre.PSC_Data[nAxis].addSpeed_NeedPlusNum;
		while(plus_Num < motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[nAxis])
		{
			if(i >=  motion_Data_Pre.PSC_Data[nAxis].length) 		// iֵ����
			{
				plus_Num += calClk_PSC(motion_Data_Pre.PSC_Data[nAxis].psc_data[maxStep]) * motion_Data_Pre.addSubTime / MHz_2_Hz;
				respMsgError("��������ʱ���쳣!\r\n", 1);
			}
			else 
			{
				plus_Num += calClk_PSC(motion_Data_Pre.PSC_Data[nAxis].psc_data[motion_Data_Pre.PSC_Data[nAxis].length - i - 1]) * 
										motion_Data_Pre.addSubTime / MHz_2_Hz;
			}
			i++;
		}
		
		return ((motion_Data_Pre.addSubTime * motion_Data_Pre.PSC_Data[nAxis].length) + (i * motion_Data_Pre.addSubTime));
	} 
	
	// ���һ���������˶�
	else 
	{
		constSpeedTime = (motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[nAxis] - 
			2 * motion_Data_Pre.PSC_Data[nAxis].addSpeed_NeedPlusNum) / motion_Data_Pre.cmd_Datas.plus_Datas.clk[nAxis];
		
		return (motion_Data_Pre.addSubTime * motion_Data_Pre.PSC_Data[nAxis].length * 2 + constSpeedTime);
	}	
}


// ������С�Ĳ�����
//static u16 calMinStepNumLength(PSC_Data_Array	Psc_Data_Cur[])
//{
//	u8 i;
//	u16 minStepNumLength = MIN_STEP_LENGTH__INIT_VAULE;
//	
//	for(i=0; i<AXIS_NUM; i++)
//	{
//		if(0 != Psc_Data_Cur[i].length)
//		{
//			minStepNumLength = min(minStepNumLength, Psc_Data_Cur[i].length);
//		}
//	}
//	
//	return minStepNumLength;
//}

// �򿪻�رռӼ��ٹ��ܵı��
static void En_ASS_Flag(FunctionalState status, N_Axis n_Axis)
{
	u8 i;
	
	if(ALL_Axis == n_Axis)
	{
		for(i=0; i<AXIS_NUM; i++)
		{
			motion_Data_Pre.PSC_Data[i].enAddSubFlag = status;
		}
	}
	else
	{
		motion_Data_Pre.PSC_Data[n_Axis].enAddSubFlag = status;
	}
}

// ����ָ�����Ƶ�ʣ�����������ʼƵ��
static ErrorStatus CorrectClk(u8 nAxis, float needTime)
{
	u32 clkTemp;
	ErrorStatus status;
	
	clkTemp = motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[nAxis] * MHz_2_Hz / needTime;
	
	if(clkTemp <= motion_Data_Pre.cmd_Datas.plus_Datas.clk[nAxis])
	{
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[nAxis] = clkTemp;
		status = SUCCESS;
	}
	else if(clkTemp <= n_Axis_Min_Clk(nAxis))
	{
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[nAxis] = motion_Data_Pre.cmd_Datas.plus_Datas.clk[nAxis];
		status = ERROR;
	}
	else
	{
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[nAxis] = n_Axis_Min_Clk(nAxis);
		status = ERROR;
	}
	
	motion_Data_Pre.nAxisClk_Cur[nAxis] = motion_Data_Pre.cmd_Datas.plus_Datas.clk[nAxis];
	
	return status;
}








