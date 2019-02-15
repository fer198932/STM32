/********************************************************************************************************
*    S�ͼӼ����㷨 2019��2��13��09:02:11 byYJY
********************************************************************************************************/

#include "AddSubSpeed_S.h"


extern 											Plus_Data cmd_Plus_Data;						// �������ݣ����Ƶ���˶�
extern FunctionalState	 		nAxisStatus[AXIS_NUM];  						// �����Ƿ�����˶��ı�־
extern NAxis_TIM_Structure	nAxis_TIM_Structure[AXIS_NUM];  		// PWM ��ʼ���Ľṹ��  
extern u32 									pluNumPWM[AXIS_NUM]; 								// ���������жϼ���
extern u16 									addSubSpeed_StepNum[AXIS_NUM]; 			// ��¼�Ӽ��ٶ�ʱ�����еĲ���

// �洢PSCֵ�Ľṹ������
PSC_Data_Array	Psc_Data_Cur[AXIS_NUM];
// �Ӽ���״̬���
AddSubSpeedStatus addSubSpeed_Status[AXIS_NUM] = 
	{ADD_SPEED, ADD_SPEED, ADD_SPEED, ADD_SPEED, ADD_SPEED};	


static float addSubTime = TIM_OUT;		// ÿ���Ӽ��ٵ�ʱ�� ��ADDSUB_TIMER������ʱ�� ��λus
static u32 maxClk, minClk;
static u32 nAxisClk_Cur[AXIS_NUM];
static u8 maxClkNum = AXIS_NUM;				// ���Ƶ�ʶ�Ӧ���� ����AXIS_NUM��ʾ�����δ����



// �Ӽ�����ز������õĳ�ʼ��
void AddSubSpeed_Init(void)
{
	// ��������˶�����
	resetMotionData();
	
	// �趨�˶�����
	setStepMotorDir();
	
	// ����Ƶ�ʵ����ʷ�Χ
	adjustClk();
	
	// ����Ӽ�����ηֶ�
	cal_AddSubSpeed_Div();
	
	// ����S�����ߵ�PSC����
	cal_S_Line();
	
	// ������˶�״̬��ʼ��
	nAxisMotion_Init();
	
}

// ������˶�״̬��ʼ��
static void	nAxisMotion_Init(void)
{
	u8 i;
	
	// ����״̬��ʼ�� �����˶�
	for(i=0; i<AXIS_NUM; i++)
	{
//		pluNumPWM[i] = cmd_Plus_Data.plusNum[i];
		nAxisStatus[i] = ENABLE;
		nAxisSetPWM(nAxis_TIM_Structure[i].TIM_N, calPSC(nAxisClk_Cur[i]));
	}
	delay_ms(1); 		// һ���ӳ٣�ʹ��PSCֵ���ú�
}

// ��������˶�����
static void resetMotionData(void)
{
	u8 i;
	mymemset(Psc_Data_Cur, 0, AXIS_NUM * sizeof(PSC_Data_Array));
	mymemset(nAxisClk_Cur, 0, sizeof(nAxisClk_Cur));
	maxClk = 0;
	minClk = 0;
	maxClkNum = AXIS_NUM;
	mymemset(pluNumPWM, 0, sizeof(pluNumPWM));
	for(i=0; i<AXIS_NUM; i++)
	{
		addSubSpeed_StepNum[i] = 0;
		addSubSpeed_Status[i] = ADD_SPEED; 			// ����״̬
	}	
}

// �趨�˶�����
static void	setStepMotorDir(void)
{
	u8 i;
	for(i=0; i<AXIS_NUM; i++)
	{
		Motor_Dir_Set(StepMotor_Dir+i, cmd_Plus_Data.dir[i]);
	}
}


// ����Ӽ�����ηֶ�
static void cal_AddSubSpeed_Div(void)
{
	u8 i;
	float maxClk_length; 			// �Ӽ��ٲ���
	
	// ���Ƶ����������Ϊ�ֶ����� ���������BUG  byYJY
	maxClk_length = (cmd_Plus_Data.clk[maxClkNum] - n_Axis_Min_Clk(maxClkNum)) / AddSub_Step_DIV;
	
	if((AXIS_NUM > maxClkNum) && (maxClk_length < MIN_STEP_NUM))		// maxClk_length̫С��Ϊ��ʱ���Ӽ���
	{
		// �����᲻ʹ�üӼ����˶�
		for(i=0; i<AXIS_NUM; i++)
			Psc_Data_Cur[i].enAddSubFlag = DISABLE;
		return;
	}
	
	
	// ����������Ĳ���
	for(i=0; i<AXIS_NUM; i++)
	{
		// �ٶ�̫Сʱ�������мӼ���
		if(cmd_Plus_Data.clk[i] < n_Axis_Min_Clk(i))
		{
			Psc_Data_Cur[i].enAddSubFlag = DISABLE;
			Psc_Data_Cur[i].length = 0;									
			continue;
		} 
		else
		{
			Psc_Data_Cur[i].enAddSubFlag = ENABLE;
			Psc_Data_Cur[i].length = maxClk_length;			// ͬ���ļӼ��ٲ���
		}
	}
}



// Ƶ�ʵ�����ʹ��Ƶ��λ�ں��ʷ�Χ�ڡ�0������ok��1���������������Ƶ������
static u8 adjustClk(void)
{
	u8 i;
	u8 status = 0;
	maxClk = preProcMaxClk();
	minClk = preProcMinClk(maxClk);
	
	// ���ֵ�ж�
	for(i=0; i<AXIS_NUM; i++)
	{
		if(cmd_Plus_Data.clk[i] == maxClk) 				// ���Ƶ��
		{
			if(maxClk > n_Axis_Max_Clk(i)) 					// ��Ҫ��Ƶ
			{
				reduceClk();
				maxClk = preProcMaxClk();
				minClk = preProcMinClk(maxClk);
				adjustClk();
			} 
			else if(maxClk < n_Axis_Max_Clk(i)) 		// ��Ҫ��Ƶ
			{
				// nope;
			}
		}
	}
	
	// ��Сֵ�ж�
	if(0 != minClk)
	{
		for(i=0; i<AXIS_NUM; i++)
		{
			if(cmd_Plus_Data.clk[i] == minClk) 				// ��СƵ��
			{
				if(minClk < n_Axis_Min_Clk(i))   			// ��Сֵ��С
				{
					if(ENABLE == ifBoostClkOK()) 			// ������Ƶ
					{
						boostClk();
						maxClk = preProcMaxClk();
						minClk = preProcMinClk(maxClk);
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
	float needPlusNum, needTotalTime;
	float freq_Temp;
	
	for(i=0; i<AXIS_NUM; i++)
	{
		if(ENABLE == Psc_Data_Cur[i].enAddSubFlag)
		{
			calSModelLine(freq_Temp, Psc_Data_Cur[i].psc_data, Psc_Data_Cur[i].length,
				cmd_Plus_Data.clk[i], n_Axis_Min_Clk(i), S_FLEXIBLE);
			
			// ��������پ����ˣ�������ʱ����
			needPlusNum = 
				calAddSpeed_NeedPlusNum(Psc_Data_Cur[i].psc_data, Psc_Data_Cur[i].length);
			
			// ��������Ҫ�����������浽�ṹ���У���Ϊ�жϼ��ٽ׶ε�����
			Psc_Data_Cur[i].addSpeed_NeedPlusNum = needPlusNum;
			
			// ����ܵ��������������мӼ��٣��Ǿͽ��Ӽ��ٲ�������
			while(needPlusNum > cmd_Plus_Data.plusNum[i])		
			{
				if(Psc_Data_Cur[i].length > MIN_STEP_NUM*2)
				{
					Psc_Data_Cur[i].length = Psc_Data_Cur[i].length >> 1; 	// �о��ȶ�ʧ
					calSModelLine(freq_Temp, Psc_Data_Cur[i].psc_data, Psc_Data_Cur[i].length,
						cmd_Plus_Data.clk[i], n_Axis_Min_Clk(i), S_FLEXIBLE);
					needPlusNum = 
						calAddSpeed_NeedPlusNum(Psc_Data_Cur[i].psc_data, Psc_Data_Cur[i].length);
					Psc_Data_Cur[i].addSpeed_NeedPlusNum = needPlusNum;
				}
				else 																// �޷�����Ӽ����������رռӼ���
				{
					Psc_Data_Cur[i].enAddSubFlag = DISABLE; 		
					break;
				}
			}
		}
		else if(DISABLE == Psc_Data_Cur[i].enAddSubFlag) 			// ���Ӽ���
		{
			//////  �������Ƶ�ʻ��ѵ�ʱ������Ƶ��  //// byYJY
		}
	}
	
	needTotalTime = calTotalNeedTime(maxClkNum);
	// ������Ϻ�����������Ҫ���мӼ��ٵ���
	for(i=0; i<AXIS_NUM; i++)
	{
		if(DISABLE == Psc_Data_Cur[i].enAddSubFlag)  		// ����Ҫ�Ӽ��ٵ���
		{
			//  �������Ƶ�ʻ��ѵ�ʱ������Ƶ��
			nAxisClk_Cur[i] = cmd_Plus_Data.plusNum[i] / needTotalTime;			
			if(nAxisClk_Cur[i] > cmd_Plus_Data.clk[i])
				nAxisClk_Cur[i] = cmd_Plus_Data.clk[i];
		}
		else
		{
			nAxisClk_Cur[i] = n_Axis_Min_Clk(i);
		}
	}
}

/* 	����S�ͼ������ߵĺ��� */
static void calSModelLine(float fre, u16 period[], float len, 
	float fre_max, float fre_min, float flexible)
{
	int i;
	float deno;
	float melo;
	float delt = fre_max - fre_min;
	for(i=0; i<len; i++)
	{
		melo = flexible * (i-len/2) / (len/2);
		deno = (float)(1.0 / (1.0 + (double)expf(-melo))); 
		fre = delt * deno + fre_min;
		period[i] = (u16)(PSC_CLK / fre);
	}
	i=i;
	return;
}

// ������Ƶ��
static u32 preProcMaxClk(void)
{
	volatile u8 i;
	u32 max_Clk = cmd_Plus_Data.clk[0];
	for(i=1; i<AXIS_NUM; i++)
	{
		max_Clk = max(max_Clk, cmd_Plus_Data.clk[i]);
	}
	// �ҵ����Ƶ�ʶ�Ӧ����
	for(i=0; i<AXIS_NUM; i++)
	{
		if(max_Clk == cmd_Plus_Data.clk[i])
			maxClkNum = i;
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
		if((0 == min_Clk) && (max_Clk != cmd_Plus_Data.clk[i]))
		{
			min_Clk = cmd_Plus_Data.clk[i];			
		}
		else if(0 != cmd_Plus_Data.clk[i])
		{
			min_Clk = min(min_Clk, cmd_Plus_Data.clk[i]);
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
		cmd_Plus_Data.clk[i] = (cmd_Plus_Data.clk[i] >> 1);
	}
}
// ��Ƶ���ٶ�̫��ʱʹ�ã����ȼ���
static void boostClk(void)
{
	u8 i;
	for(i=0; i<AXIS_NUM; i++)
	{
		cmd_Plus_Data.clk[i] = (cmd_Plus_Data.clk[i] << 1);
	}
}

// ���ص�ǰ������������СƵ��
static u32 n_Axis_Max_Clk(u8 i)
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
static u32 n_Axis_Min_Clk(u8 i)
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
		if((cmd_Plus_Data.clk[i] << 1) > n_Axis_Max_Clk(i)) // ��Ƶ��������Ƶ�ʣ���������Ƶ
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
		needPlusNum += addSubTime * clk;			// ע�⣺addSubTime ��λΪus
	}
	needPlusNum *= (float)1e-6;								
	
	return needPlusNum;
}

// ����ָ������������Ҫ��ʱ�� ��λus ȷ���������ߺ��ٵ���
static float calTotalNeedTime(u8 nAxis)
{	
	float constSpeedTime; 		// ���ٽ׶���Ҫ��ʱ��
	if(DISABLE == Psc_Data_Cur[nAxis].enAddSubFlag) 		// δ�����Ӽ���
		return (cmd_Plus_Data.plusNum[nAxis] * 1e6 / cmd_Plus_Data.clk[nAxis]);
	
	constSpeedTime = (cmd_Plus_Data.plusNum[nAxis] - 
		2 * Psc_Data_Cur[nAxis].addSpeed_NeedPlusNum) / cmd_Plus_Data.clk[nAxis];
	
	return (addSubTime * Psc_Data_Cur[nAxis].length * 2 + constSpeedTime);
	
}






