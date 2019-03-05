#include "StepMotor.h"			

//////////////////////////////////////////////////////////////////////////////////	 
// ����������Ƴ���
// 1��
// 2019��1��31��13:20:00 byYJY
//////////////////////////////////////////////////////////////////////////////////

// ָ�����趨���˶�����
extern 	volatile 	FunctionalState 		Offline_Work_Flag; 						// �����ѻ��ӹ��ı��
extern 	Flag_Structure 				flag_Struct;
extern	Motion_Strcuture 			motion_Data;	
extern	Motion_Strcuture 			motion_Data_Pre;	


// �����϶���
const	 u32	 Backlash_PlusNum = BacklashCompensation * SUBDIV_NUM / (1e3);
BacklashCompensation_Structure		backlashCompen;


// �����Ƿ�����˶� 
FunctionalState	 nAxisStatus[AXIS_NUM] = {DISABLE, DISABLE, DISABLE, DISABLE, DISABLE};

// �������IO��
GPIO_Structure_XX  	StepMotor_Dir[AXIS_NUM];

// ���������ʼ�� ��Ҫ�Ƿ���IO�ڵĳ�ʼ��
void StepMotor_Init(void)
{
	Motor_Dir_Init(StepMotor_Dir+0, DIR_X);
	Motor_Dir_Init(StepMotor_Dir+1, DIR_Y);
	Motor_Dir_Init(StepMotor_Dir+2, DIR_Z);
	Motor_Dir_Init(StepMotor_Dir+3, DIR_A);
	Motor_Dir_Init(StepMotor_Dir+4, DIR_B);
	
}

// ��������˶�IO�ڷ����ʼ��
static void Motor_Dir_Init(GPIO_Structure_XX *GPIO_Temp, const char str[])
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	// ����GPIO
	if(!GPIO_Structure_Make(str, GPIO_Temp)) return;
	
	// ʹ��ʱ��
	RCC_AHB1PeriphClockCmd(GPIO_Temp->RCC_Periph_N, ENABLE);
	
	// GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Temp->GPIO_Pin_N;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;						// ��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					// �������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			// 100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						// ���� Ĭ�����������˶�
  GPIO_Init(GPIO_Temp->GPIO_Port, &GPIO_InitStructure);									// ��ʼ��GPIO	
}

// ����˶������趨 
void Motor_Dir_Set(GPIO_Structure_XX *GPIO_Temp, Motor_Dir dir)
{	
//	static volatile uint8_t dirOld = 0;  		// �ʼ�ķ���
	
	if(TBD_DIR == dir)					// ����δ�趨ʱ��ɶ������
		return;
	
	if(POS_DIR == dir) 			// ����
	{
		GPIO_SetBits(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N);
	}
	else      							// ����
	{
		GPIO_ResetBits(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N);  
	}
	
//		while(GPIO_ReadOutputDataBit(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N) == dirOld) ; 	// ��������˳�ѭ��
	delay_us(DIR_EX_DALAY); 			// һ����ʱ

}

// ����˶������趨 ����1 
//void Motor_Dir_Set(GPIO_Structure_XX *GPIO_Temp, Motor_Dir dir)
//{	
//	volatile uint8_t dirOld;  		// �ʼ�ķ���
//	
//	if(TBD_DIR == dir)					// ����δ�趨ʱ��ɶ������
//		return;
//	
//	dirOld = GPIO_ReadOutputDataBit(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N);
//	
//	// ��Ҫ����
//	if(dirOld != dir)
//	{
//		if(POS_DIR == dir) 			// ����
//		{
//			GPIO_SetBits(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N);
//		}
//		else      							// ����
//		{
//			GPIO_ResetBits(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N);  
//		}
//		
//		while(GPIO_ReadOutputDataBit(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N) == dirOld) ; 	// ��������˳�ѭ��
//		delay_us(DIR_EX_DALAY); 			// һ����ʱ
//	}
//	
//	// else ���û���
//}

// �������ͬʱ��ʼ�˶�����֤ͬ���� �Ĵ�����ʽ�����Ч��
void StepMotor_Start(void)
{
	// ��λ����ʱ�����������������Ƶ������
	StepMotor_Limited(flag_Struct.Limiti_Flag);
	
	// X��
	if(ENABLE == nAxisStatus[0])
	{
		// �궨������
		nAxis_StepMotor_Start(X);
//		PWM_Cmd(X_PWM, ENABLE, X_CH_EXTI);
//		PWM_Cmd(X_PWM, ENABLE, X_CH_OUT);
	}
	
	// Y��
	if(ENABLE == nAxisStatus[1])
	{
		nAxis_StepMotor_Start(Y);
//		PWM_Cmd(Y_PWM, ENABLE, Y_CH_EXTI);
//		PWM_Cmd(Y_PWM, ENABLE, Y_CH_OUT);	
	}
	
	// Z��
	if(ENABLE == nAxisStatus[2])
	{
		nAxis_StepMotor_Start(Z);
//		PWM_Cmd(Z_PWM, ENABLE, Z_CH_EXTI);
//		PWM_Cmd(Z_PWM, ENABLE, Z_CH_OUT);
	}
	
	// A��
	if(ENABLE == nAxisStatus[3])
	{
		nAxis_StepMotor_Start(A);
//		PWM_Cmd(A_PWM, ENABLE, A_CH_EXTI);
//		PWM_Cmd(A_PWM, ENABLE, A_CH_OUT);
	}
	
	// B��
	if(ENABLE == nAxisStatus[4])
	{
		nAxis_StepMotor_Start(B);
//		PWM_Cmd(B_PWM, ENABLE, B_CH_EXTI);
//		PWM_Cmd(B_PWM, ENABLE, B_CH_OUT);
	}		

	if(0 == nAxis_Motion_Flag)
	{
		flag_Struct.Cmd_Executing_Flag = RESET;
		
#if OFFLINE_WORK
		Offline_Work_Flag = ENABLE;
#else
#endif	
		
		
		// �ظ���λ��
#if PRIN2DISP
//		respUsartMsg("PWM_Cmd\r\n", 10);
#else
//		respUsartMsg(backResString_Motion, RESP_MOTIONMSG_LENGTH);
#endif	
	}
}

// �������ͬʱֹͣ��ͬ���� û��ǿ������
void StepMotor_Stop_Macro(void)
{
	TIM_Cmd(X_PWM, DISABLE);
	TIM_Cmd(Y_PWM, DISABLE);
	TIM_Cmd(Z_PWM, DISABLE);
	TIM_Cmd(A_PWM, DISABLE);
	TIM_Cmd(B_PWM, DISABLE);
//	nAxis_StepMotor_Stop(X);
//	nAxis_StepMotor_Stop(Y);
//	nAxis_StepMotor_Stop(Z);
//	nAxis_StepMotor_Stop(A);
//	nAxis_StepMotor_Stop(B);
}

// ������������趨����
void StepMotor_Move(N_Axis n_axis, u32 Clk, u32 PlusNum, Motor_Dir Dir)
{		
	// �趨����
//	Motor_Dir_Set(StepMotor_Dir+n_axis, Dir);

	/* �����ʼ�� */
	// �趨�˶�����
	mymemset(&motion_Data_Pre, 0, sizeof(motion_Data_Pre));
	motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[n_axis] = PlusNum;
	motion_Data_Pre.cmd_Datas.plus_Datas.clk[n_axis] = Clk;
	motion_Data_Pre.cmd_Datas.plus_Datas.dir[n_axis] = Dir;
	
	// �Ӽ��ٳ�ʼ��
	AddSubSpeedInit_Pre();		// ע��������ܻ���趨��Ƶ�ʽ��е���
	
	// �˶�״̬��ʼ��
	motion_Init();
	delay_us(MOTION_START_DALAY); 			// һ����ʱ
	
	// �������ͬʱ��ʼ�˶�
	StepMotor_Start();
	
#if NO_ADDSUBSPEED
	/* �رռӼ��� */
#else		
	// �Ӽ��ٶ�ʱ������
	EN_ADDSUB_TIMER;
#endif
}

// ��ʼ��ʱ��ļ�϶���� 0.1mm
void Move2CompensateBacklash(void)
{
	u8 i;
	
	for(i=0; i<AXIS_NUM; i++)
	{
		StepMotor_Move((N_Axis)i, n_Axis_Min_Clk(i), SUBDIV_NUM/10, POS_DIR);		// 0.1mm
		delay_ms(100);
		StepMotor_Move((N_Axis)i, n_Axis_Min_Clk(i), SUBDIV_NUM/10, NEG_DIR);
		delay_ms(100);
		backlashCompen.motorDirOld[i] = NEG_DIR;
	}
	
	backlashCompen.DirChange_En = ENABLE;
}






