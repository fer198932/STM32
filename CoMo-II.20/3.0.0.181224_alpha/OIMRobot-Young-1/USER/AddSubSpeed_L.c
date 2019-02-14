/********************************************************************************************************
*   ֱ���ͼӼ����㷨 2019��2��10��18:45:08 byYJY
********************************************************************************************************/

#include "AddSubSpeed_L.h"

extern Plus_Data 	cmd_Plus_Data;												// �������ݣ����Ƶ���˶�
extern GPIO_Structure_XX  	StepMotor_Dir[AXIS_NUM]; 		// �������IO��
// ���������жϼ���
extern volatile u32 pluNumPWM[AXIS_NUM];
/* PWM ��ʼ���Ľṹ��  */
extern GPIO_Structure_XX 		GPIO_PWM_Plus[AXIS_NUM];

u32 subPlusCnt[5] = {0, 0, 0, 0, 0};					// ���ٽ׶������������
u16 correctSyncValue[5] = {0, 0, 0, 0, 0};		// ���������м�Ƶ�ʵļ��ٽ׶�����ֵ

// u32 savePlusNum[5] = {0, 0, 0, 0, 0};		// ��������Ӽ���ʱ ���ٽ׶��߹���������

static u32 maxClk, minClk;
static u32 nAxisClk_Cur[AXIS_NUM];

static u8 maxClkNum = 5;			// ����5��ʾ���󣬲�������

// �������еļ���������λΪaddtime�� 0-4:x��y��z��a��b
static u32 minSpeedTimeCnt[5] = {0, 0, 0, 0, 0};	

// ÿһ����Ҫ��ʱ�䣬��Ƶ������Ҫ���� 0-4:x��y��z��a��b
static u32 clkTimeCntNeed[5];								

// ������ٽ׶εı�ǣ�RESET����������٣�SET����ʼ����
FlagStatus startSubFlag[5] = {RESET, RESET, RESET, RESET, RESET};	

// �Ƿ񱣴��������ı�ǩ�������ڼ���״̬ RESET�������棬 SET������ 0-4��x��y��z��a��b
static FlagStatus savePlusNumFlag[5] = {RESET, RESET, RESET, RESET, RESET};		

// ������TIM��ӳ���ϵ
extern 	NAxis_TIM_Structure 	nAxis_TIM_Structure[AXIS_NUM];



// �Ӽ����㷨�ĳ�ʼ������ ϵͳ�˶�ǰ����
void addSubSpeed_L_Init(void)
{
	// ����Ƶ��
	adjustClk();
	
	// �趨����  �����⣬��ʱ���� byYJY
//	setStepMotorDir();
	
	// �趨�Ӽ��ٵ�����˶�����
	setSubAddSpeedData();
	
	// �����ʼ��
	nAxisInit();
}

// �Ӽ��ٶ�ʱ���жϷ������  ���Ǹĳ�����������궨�壬�������Ч��  byYJY
void addSubTime_IRQ(void)
{
	u8 i;
	for(i=0; i<AXIS_NUM; i++)
	{
		if((0 != cmd_Plus_Data.clk[i]) && (0 != cmd_Plus_Data.plusNum[i]))
			nAddSubTime_IRQ(i);		
	}
}

// ����ļӼ���-��ʱ���жϷ������
static void nAddSubTime_IRQ(u8 nAxis)
{	
	// �ٶ�̫�Ͳ����мӼ���
	if(n_Axis_Min_Clk(nAxis) >= cmd_Plus_Data.clk[nAxis])
		return;
	
	// ��������н׶�
	if(0 != minSpeedTimeCnt[nAxis])
	{
		minSpeedTimeCnt[nAxis]--;
	}
	// ���ٽ׶�
	else if(SET == savePlusNumFlag[nAxis])
	{
		if(nAxisClk_Cur[nAxis] <= cmd_Plus_Data.clk[nAxis]) 		// ���ٽ׶�
		{
			nAxisClk_Cur[nAxis]++;
			if(clkTimeCntNeed[nAxis])													// ��Ƶ������Ҫ�ϳ���ά��ʱ��
			{
				clkTimeCntNeed[nAxis]--;
			}
			else 
			{
				// ���PSCֵ�޸��ˣ������¸�ֵ
				if(1 == if_PSC_Modify(nAxis, calPSC(nAxisClk_Cur[nAxis])))
				{
					nAxisSetPWM(nAxis_TIM_Structure[nAxis].TIM_N, calPSC(nAxisClk_Cur[nAxis]));
				}
				clkTimeCntNeed[nAxis] = calPWM_TimeCnt(nAxisClk_Cur[nAxis], TIM_OUT);
			}
		}
		else 
		{
//			savePlusNum[nAxis] = pluNumPWM[nAxis];
			savePlusNumFlag[nAxis] = RESET;										// �رձ�ǩ���������ȡ����ٽ׶�
		}
	}
	// ���ٽ׶�
	else if(SET == startSubFlag[nAxis])
	{
		if(nAxisClk_Cur[nAxis] > n_Axis_Min_Clk(nAxis))			// �ݼ�����СƵ�ʣ�С�ĵݼ�����������
		{
			nAxisClk_Cur[nAxis]--;
			if(clkTimeCntNeed[nAxis])
			{
				clkTimeCntNeed[nAxis]--;
			} else
			{
				if( 1 == if_PSC_Modify(nAxis, calPSC(nAxisClk_Cur[nAxis])) )
				{
					nAxisSetPWM(nAxis_TIM_Structure[nAxis].TIM_N, calPSC(nAxisClk_Cur[nAxis]));
				}					
				clkTimeCntNeed[nAxis] = calPWM_TimeCnt(nAxisClk_Cur[nAxis], TIM_OUT);
			}
		}
	}
}

// �趨�Ӽ��ٵ�����˶�����
static void	setSubAddSpeedData(void)
{
	u8 i;
	
	for(i=0; i<AXIS_NUM; i++)
	{
		// ����ǰ�ĵ������н׶�
		calMinSpeedTimeCnt(i);
		// �õ�һ��������������޸�Ƶ��
		clkTimeCntNeed[i] = calPWM_TimeCnt(nAxisClk_Cur[i], TIM_OUT);
		// �򿪼��ٽ׶εı��
		savePlusNumFlag[i] = SET;
		
//		savePlusNum[i] = 0;
		startSubFlag[i] = RESET;
		// ��������+1������Լ��С���������Ƿ����������룿�� byYJY
		subPlusCnt[i] = calSubPlusCnt(i, cmd_Plus_Data.clk[i]) + 1;	
		
		correctSyncValue[i] = correctSync(n_Axis_Min_Clk(i), i);
	}
}

// �����ʼ��
static void nAxisInit(void)
{
	u8 i;
	
	for(i=0; i<AXIS_NUM; i++)
	{
		n_Axis_Init(i);
	}
}
// ָ����ĳ�ʼ��
static void n_Axis_Init(u8 nAxis)
{
//	TIM_TypeDef* 	TIM_Temp;
	u16 psc;
	
	// �ж��Ƿ�Ҫ�Ӽ���
	if(cmd_Plus_Data.clk[nAxis] > n_Axis_Min_Clk(nAxis))
	{
		nAxisClk_Cur[nAxis] = n_Axis_Min_Clk(nAxis);
	}
	else 			// ���Ӽ��٣�������ԭ�ٶ�
	{
		// �ٶ����� 
		correctNoAddSubClk(nAxis, cmd_Plus_Data.plusNum[nAxis]);
		nAxisClk_Cur[nAxis] = cmd_Plus_Data.clk[nAxis];
	}
	
	psc = calPSC(nAxisClk_Cur[nAxis]);
	
//	nAxisSetPWM(nAxis_TIM_Structure[nAxis].TIM_N, psc);  // ��һ�λ�ʧ��
	// PWM��ʼ�� 
	Tim_PWM_Init(&nAxis_TIM_Structure[nAxis], 0x04, 0x04, TIM_ARR, psc);
	
}

// �趨����
static void	setStepMotorDir(void)
{
	u8 i;
	for(i=0; i<AXIS_NUM; i++)
	{
		if(0 == cmd_Plus_Data.dir[i]) 								// ����
			Motor_Dir_Set(StepMotor_Dir+i, POS_DIR);
		else 																					// ����
			Motor_Dir_Set(StepMotor_Dir+i, NEG_DIR);
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

// ������Ƶ��
static u32 preProcMaxClk(void)
{
	volatile u8 i;
	u32 max_Clk = cmd_Plus_Data.clk[0];
	for(i=1; i<AXIS_NUM; i++)
	{
		max_Clk = max(max_Clk, cmd_Plus_Data.clk[i]);
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

// ��������Ƶ�������Ƶ������Ҫ���е�ʱ�䣨����ֵ��
// 0����Ƶ�ʣ� 0�����Ƶ�ʣ� ����ֵ��С������ٶȵ��м�Ƶ��
static void calMinSpeedTimeCnt(u8 nAxis)
{
	if(0 == cmd_Plus_Data.clk[nAxis])
		minSpeedTimeCnt[nAxis] = 0;
	else if(maxClk == cmd_Plus_Data.clk[nAxis])
		minSpeedTimeCnt[nAxis] = 0;
	else
		minSpeedTimeCnt[nAxis] = maxClk - cmd_Plus_Data.clk[nAxis];	
}

// ����ÿ�η�����������Ҫ��ʱ��_ ������ time��λΪus��������1e6
static u16 calPWM_TimeCnt(u16 clk, u16 time)
{
	return (1e6 / (clk * time));
}

// �޸��˷���1�����򷵻�0
static u8 if_PSC_Modify(u8 nAxis, u16 psc)				
{
	switch(nAxis)
	{
		case 0:
			if(X_PWM->PSC == psc)
				return 0;
			else 
				return 1;
		case 1:
			if(Y_PWM->PSC == psc)
				return 0;
			else 
				return 1;
		case 2:
			if(Z_PWM->PSC == psc)
				return 0;
			else 
				return 1;
		case 3:
			if(A_PWM->PSC == psc)
				return 0;
			else 
				return 1;
		case 4:
			if(B_PWM->PSC == psc)
				return 0;
			else 
				return 1;
		default :
			return 0;
	}
}

// ������ٽ׶���Ҫ��������(������)
static u32 calSubPlusCnt(u8 nAxis, u16 nAxisClk)
{
	u16 min_Clk;
	float timeAddSub;
	u16 addSubPlusCnt;
	
	min_Clk = n_Axis_Min_Clk(nAxis);
	timeAddSub = (nAxisClk - min_Clk) * TIM_OUT;		// ��λus
	addSubPlusCnt = (nAxisClk + min_Clk) / 2.0 * timeAddSub * 1e-6;		// 1e-6����Ϊ��us��s
	return addSubPlusCnt;
}

// ����ͬ���Ĳ���
static u16 correctSync(u32 nAxisMinClk, u8 nAxis)
{
	// ����ͬ���Ĳ���
	// �����ķ�ʽ����Ҫ�о�����ʱ��д��� byYJY
	if((AXIS_NUM > maxClkNum) && (nAxisMinClk != 0) && (maxClk != nAxisMinClk))
		return (nAxisMinClk*minSpeedTimeCnt[nAxis]*TIM_OUT/2e6 + 1);					// ��1����С��
	else
		return 0;									
}

// �������Ӽ��ٵ����Ƶ��
static void correctNoAddSubClk(u8 nAxis, u32 plusNum)
{
	u32 clk;
	float time = calTotalTime(maxClkNum);						// ��ʱ�������Ƶ�ʵ������
	if(0 == (int)time)
		return;
	
	clk = (int)((float)(1e6*plusNum) / time);
	if(0 == clk)
		return;
	
	if(cmd_Plus_Data.clk[nAxis] > clk)
		cmd_Plus_Data.clk[nAxis] = clk;
}

// ����һ֡��Ҫ��ʱ�䣬 ��λus
static float calTotalTime(u8 nAxis)
{
	u32 min_Clk;
	float timeAddSub;
	float plusNum;
	
	min_Clk = n_Axis_Min_Clk(nAxis);
	plusNum = cmd_Plus_Data.plusNum[nAxis];
	
	// �Ӽ�������ʱ�� us
	timeAddSub = (maxClk - min_Clk) * TIM_OUT;									
	return ((float)((float)1e6*plusNum - (maxClk+min_Clk)*timeAddSub)/
		(float)maxClk + 2 * timeAddSub);
	// ������timeAddSub����2��ʹ�ò���ͬ����ԭ��δ֪  byYJY
//	return ((float)(1e6*plusNum - (max_Clk+min_Clk)*timeAddSub/2)/(float)max_Clk + 2 * timeAddSub);
}
