/********************************************************************************************************
*   ֱ���ͼӼ����㷨 2019��2��10��18:45:08 byYJY
********************************************************************************************************/

#include "AddSubSpeed_L.h"

extern Plus_Data cmd_Plus_Data;			// �������ݣ����Ƶ���˶�

static u32 maxClk, minClk;

// �Ӽ����㷨�ĳ�ʼ������ ϵͳ�˶�ǰ����
void addSubSpeed_L_Init(void)
{
	// ����Ƶ��
	adjustClk();
	
	
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




