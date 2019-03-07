/********************************************************************************************************
*    ��λIO�ڵ���س���
********************************************************************************************************/
#include "limit_IO.h"

extern 		Flag_Structure 								flag_Struct;
extern 		RespMsgArray									respMsgStr;										// �ظ���λ������Ϣ����	

extern		GPIO_Structure_XX							EXTI_UrgentStop;							// ��ͣ�ж�
extern		GPIO_Structure_XX							EXTI_MainMotor_Start;					// ��ͣ�ж�

extern		Motion_Strcuture 							motion_Data;	
extern 		FunctionalState	 							nAxisStatus[AXIS_NUM];  	// �����Ƿ�����˶��ı�־


static const u16 length = 9;

// static ErrorStatus limit_Status[8]; 				// �жϸ�����λIO���Ƿ��ʼ��

static u8 backResString[length]; 						// �ظ����ַ���

static u32 keyValue = 0;
static u32 keyValueOld = 0;

// ��λ��IO��
static 	GPIO_Structure_XX 	x_p_limit;
static 	GPIO_Structure_XX 	x_n_limit;
static 	GPIO_Structure_XX 	y_p_limit;
static 	GPIO_Structure_XX 	y_n_limit;
static 	GPIO_Structure_XX 	z_p_limit;
static 	GPIO_Structure_XX 	z_n_limit;
static 	GPIO_Structure_XX 	a_limit;
static 	GPIO_Structure_XX 	b_limit;

// IO�ڳ�ʼ������
void limit_IO_Init(void)
{
	// ��ʼ������λ���IO��
	limitInit(&x_p_limit, X_P_LIMIT);
	limitInit(&x_n_limit, X_N_LIMIT);
	limitInit(&y_p_limit, Y_P_LIMIT);
	limitInit(&y_n_limit, Y_N_LIMIT);
	limitInit(&z_p_limit, Z_P_LIMIT);
	limitInit(&z_n_limit, Z_N_LIMIT);
	limitInit(&a_limit, A_LIMIT);
	limitInit(&b_limit, B_LIMIT);	
}

// �鿴�Ƿ�����λ����
// mode:0,��֧��������;1,֧��������;
static void LIMIT_Scan(void)
{
//	u8 UrgentStop_Temp1, UrgentStop_Temp2;
	
	if((0==X_P_Lim()) || (0==X_N_Lim()) || (0==Y_P_Lim()) || (0==Y_N_Lim()) || 
			(0==Z_P_Lim()) || (0==Z_N_Lim()) || (0==A_Lim()) || (0==B_Lim()))				//  �а�������
	{
//		delay_us(10);				// ����ȥ��
		delay_ms(10);					// ����ȥ��
		
		keyValue = 0;
		
		if(0==X_P_Lim())  keyValue=keyValue | 1;
		if(0==X_N_Lim())  keyValue=keyValue | 2;
		if(0==Y_P_Lim())  keyValue=keyValue | 4;
		if(0==Y_N_Lim())  keyValue=keyValue | 8;
		if(0==Z_P_Lim())  keyValue=keyValue | 16;
		if(0==Z_N_Lim())  keyValue=keyValue | 32;
		if(0==A_Lim())   	keyValue=keyValue | 64;
		if(0==B_Lim())   	keyValue=keyValue | 128;
		
	}
	else if((1==X_P_Lim()) && (1==X_N_Lim()) && (1==Y_P_Lim()) && (1==Y_N_Lim()) &&
					(1==Z_P_Lim()) && (1==Z_N_Lim()) && (1==A_Lim()) && (1==B_Lim()))		// �ް�������
	{
		keyValue = 0;
	}
	
	//  �趨��־λ
	flag_Struct.Limiti_Flag = keyValue;
}

// ѭ���鿴�Ƿ�����λ��������������λ
void procLimit(void)
{
	LIMIT_Scan();
	
	if(keyValue != keyValueOld)
	{
		keyValueOld = keyValue;
		
		if(keyValue != 0)
		{
			backResString[0] = FrameB1;
			backResString[1] = FrameB2;
			backResString[2] = STATUSINFO;
			
			backResString[3] = length;					// ����
			backResString[4] = 0x00;
			
			backResString[8] = FrameEnd;
			
			/////////////ע���־λ������//////////////////////// byYJY
			
		}
		
		/* X+ ��λ */
		if((keyValue>>0) & 0x0001)
		{
			//////////////////�رյ���ڸ÷����ϵ��˶� byYJY
			backResString[5] = X_P_MSG1;
			backResString[6] = 0x00;
			backResString[7] = X_P_MSG2;
			
#if PRIN2DISP
			// ���ڻظ�
			backResString[0] = backResString[0]; 		// ����warning
			respUsartMsg("LIMITx+\r\n", length);
#else
			respUsartMsg(backResString, length);
#endif			
		}
		
		/* X- ��λ */
		if((keyValue>>1) & 0x0001)
		{
			//////////////////�رյ���ڸ÷����ϵ��˶� byYJY
			backResString[5] = X_N_MSG1;
			backResString[6] = 0x00;
			backResString[7] = X_N_MSG2;
			
#if PRIN2DISP
			// ���ڻظ�
			backResString[0] = backResString[0]; 		// ����warning
			respUsartMsg("LIMITx-\r\n", length);
#else
			respUsartMsg(backResString, length);
#endif		
		}
		
		/* Y+ ��λ */
		if((keyValue>>2) & 0x0001)
		{
			//////////////////�رյ���ڸ÷����ϵ��˶� byYJY
			backResString[5] = Y_P_MSG1;
			backResString[6] = 0x00;
			backResString[7] = Y_P_MSG2;
			
#if PRIN2DISP
			// ���ڻظ�
			backResString[0] = backResString[0]; 		// ����warning
			respUsartMsg("LIMITy+\r\n", length);
#else
			respUsartMsg(backResString, length);
#endif		
		}
		
		/* Y- ��λ */
		if((keyValue>>3) & 0x0001)
		{
			//////////////////�رյ���ڸ÷����ϵ��˶� byYJY
			backResString[5] = Y_N_MSG1;
			backResString[6] = 0x00;
			backResString[7] = Y_N_MSG2;
			
#if PRIN2DISP
			// ���ڻظ�
			backResString[0] = backResString[0]; 		// ����warning
			respUsartMsg("LIMITy-\r\n", length);
#else
			respUsartMsg(backResString, length);
#endif		
		}
		
		/* Z+ ��λ */
		if((keyValue>>4) & 0x0001)
		{
			//////////////////�رյ���ڸ÷����ϵ��˶� byYJY
			backResString[5] = Z_P_MSG1;
			backResString[6] = 0x00;
			backResString[7] = Z_P_MSG2;
			
#if PRIN2DISP
			// ���ڻظ�
			backResString[0] = backResString[0]; 		// ����warning
			respUsartMsg("LIMITz+\r\n", length);
#else
			respUsartMsg(backResString, length);
#endif		
		}
		
		/* Z- ��λ */
		if((keyValue>>5) & 0x0001)
		{
			//////////////////�رյ���ڸ÷����ϵ��˶� byYJY
			backResString[5] = Z_N_MSG1;
			backResString[6] = 0x00;
			backResString[7] = Z_N_MSG2;
			
#if PRIN2DISP
			// ���ڻظ�
			backResString[0] = backResString[0]; 		// ����warning
			respUsartMsg("LIMITz-\r\n", length);
#else
			respUsartMsg(backResString, length);
#endif		
		}
		
		/* A ��λ */
		if((keyValue>>6) & 0x0001)
		{
			//////////////////�رյ���ڸ÷����ϵ��˶� byYJY
			backResString[5] = A_MSG1;
			backResString[6] = 0x00;
			backResString[7] = A_MSG2;
			
#if PRIN2DISP
			// ���ڻظ�
			backResString[0] = backResString[0]; 		// ����warning
			respUsartMsg("LIMITa0\r\n", length);
#else
			respUsartMsg(backResString, length);
#endif		
		}
		
		/* B ��λ */
		if((keyValue>>7) & 0x0001)
		{
			//////////////////�رյ���ڸ÷����ϵ��˶� byYJY
			backResString[5] = B_MSG1;
			backResString[6] = 0x00;
			backResString[7] = B_MSG1;	

#if PRIN2DISP
			// ���ڻظ�
			backResString[0] = backResString[0]; 		// ����warning
			respUsartMsg("LIMITb0\r\n", length);
#else
			respUsartMsg(backResString, length);
#endif		
		}		
	}	
}

// ��ʼ������
static void limitInit(GPIO_Structure_XX *GPIO_Temp, const char str[])
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(ERROR == GPIO_Structure_Make(str, GPIO_Temp))
		return;
	
	RCC_AHB1PeriphClockCmd(GPIO_Temp->RCC_Periph_N, ENABLE);		// ʹ��ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Temp->GPIO_Pin_N;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  			//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  			//��������
	
	GPIO_Init(GPIO_Temp->GPIO_Port, &GPIO_InitStructure);
}

// ��ȡIO�������ƽ 0:�͵�ƽ��������λ 1���ߵ�ƽ��������
static uint8_t readOutput(GPIO_Structure_XX *GPIO_Temp)
{
	uint8_t bitStatus;
	bitStatus = GPIO_ReadInputDataBit(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N);
	
	return bitStatus;
}

// 0��������λ��1��û�д���
uint8_t X_P_Lim(void)
{
	return readOutput(&x_p_limit);
}
uint8_t X_N_Lim(void)
{
	return readOutput(&x_n_limit);
}
uint8_t Y_P_Lim(void)
{
	return readOutput(&y_p_limit);
}
uint8_t Y_N_Lim(void)
{
	return readOutput(&y_n_limit);
}
uint8_t Z_P_Lim(void)
{
	return readOutput(&z_p_limit);
}
uint8_t Z_N_Lim(void)
{
	return readOutput(&z_n_limit);
}
uint8_t A_Lim(void)
{
	return readOutput(&a_limit);
}
uint8_t B_Lim(void)
{
	return readOutput(&b_limit);
}

uint8_t UrgentStop_Lim(void)
{
	return readOutput(&EXTI_UrgentStop);
}

uint8_t MainMotor_Start(void)
{
	return readOutput(&EXTI_MainMotor_Start);
}

// ��ͣ����
void UrgentStop_Proc(u8 status)
{
	if(UrgentStop_Down == status)			// ���¼�ͣ
	{
		// ������ֹͣת��
		mainMotorStop();
		
		// ���ֹͣ�˶�
		StepMotor_Stop_Macro();
		DIS_ADDSUB_TIMER;
		delay_ms(1);
		PWM_Forced2High_All();
		
		// �ظ���λ��
		setRespStr_UrgentStop(respMsgStr.respMsg_UrgentStopDown, RespMsg_UrgentStopDown_Length, 1);
		respUsartMsg(respMsgStr.respMsg_UrgentStopDown, RespMsg_UrgentStopDown_Length);
		
		// ���λ���³�ʼ��
		Flag_Struct_Init();		
		flag_Struct.UrgentStop_Flag = UrgentStop_Down;
	}
	else if(UrgentStop_Up == status) 	// �ɿ���ͣ
	{
		//  �ظ���λ��
		setRespStr_UrgentStop(respMsgStr.respMsg_UrgentStopUp, RespMsg_UrgentStopUp_Length, 1);
		respUsartMsg(respMsgStr.respMsg_UrgentStopUp, RespMsg_UrgentStopUp_Length);
		
		// ���ñ��λ
		flag_Struct.UrgentStop_Flag = UrgentStop_Init;		
	}
}

// �Լ�ļ�ͣ���
u8 UrgentStopTest(void)
{
	u8 UrgentStop_Temp1, UrgentStop_Temp2;
	
	/* ��ͣ��ť�Ƿ��� */
	if(0 == UrgentStop_Lim())
	{
		UrgentStop_Temp1 = UrgentStop_Lim();
		delay_ms(2);
		UrgentStop_Temp2 = UrgentStop_Lim();
		if(UrgentStop_Temp1 == UrgentStop_Temp2)
		{
			if(0 == UrgentStop_Temp1)
			{
				flag_Struct.UrgentStop_Flag = UrgentStop_Locked;
			}
		}
		return SELFCHECK_URGENTSTOP;		
	}
	
	return SELFCHECK_OK;
}

//  �����������λ����ֹͣ�˶�����λ����ʱ�����������������Ƶ������
void StepMotor_Limited(vu32 limitValue)
{
	u8 i;
	
	for(i=0; i<AXIS_NUM; i++)
	{
		if((0 == motion_Data.cmd_Datas.plus_Datas.plusNum[i]) || (0 == motion_Data.cmd_Datas.plus_Datas.clk[i]))
		{
			nAxisStatus[i] = DISABLE;
		}
		else if(i<3)		// X��Y��Z���з���
		{
			if(POS_DIR == motion_Data.cmd_Datas.plus_Datas.dir[i])
			{
				if(0 != ((flag_Struct.Limiti_Flag>>(2*i)) & 0x0001))			// ����λ
				{
					nAxisStatus[i] = DISABLE;
				}
			}
			else if(NEG_DIR == motion_Data.cmd_Datas.plus_Datas.dir[i])
			{
				if(0 != ((flag_Struct.Limiti_Flag>>(2*i+1)) & 0x0001))			// ����λ
				{
					nAxisStatus[i] = DISABLE;
				}
			}			
		}
		else						// A��B��
		{
			if(0 != ((flag_Struct.Limiti_Flag>>(i+3)) & 0x0001))			// ����λ
			{
				nAxisStatus[i] = DISABLE;
			}
		}
	}
}










