#include "StepMotor.h"			

//////////////////////////////////////////////////////////////////////////////////	 
// ����������Ƴ���
// 1��
// 2019��1��31��13:20:00 byYJY
//////////////////////////////////////////////////////////////////////////////////

// ָ�����趨���˶�����
extern Proc_Data cmd_Proc_Data; 		// �������ݣ��г�Աָ��plus_Data
extern Plus_Data cmd_Plus_Data;			// �������ݣ����Ƶ���˶�

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
	static volatile uint8_t dirOld = 0;  		// �ʼ�ķ���
	
	if(TBD_DIR == dir)					// ����δ�趨ʱ��ɶ������
		return;
	
	// ��Ҫ����
	if(dirOld != dir)
	{
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
		dirOld = dir;
	}
	
	// else ���û���
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
	// X��
	if((0 == cmd_Plus_Data.plusNum[0]) || (0 == cmd_Plus_Data.clk[0]))
	{
		nAxisStatus[0] = DISABLE;
	}
	else 
	{
		// �궨������
		nAxis_StepMotor_Start(X);
//		PWM_Cmd(X_PWM, ENABLE, X_CH_EXTI);
//		PWM_Cmd(X_PWM, ENABLE, X_CH_OUT);
	}
	
	// Y��
	if((0 == cmd_Plus_Data.plusNum[1]) || (0 == cmd_Plus_Data.clk[1]))
	{
		nAxisStatus[1] = DISABLE;
	}
	else 
	{
		nAxis_StepMotor_Start(Y);
//		PWM_Cmd(Y_PWM, ENABLE, Y_CH_EXTI);
//		PWM_Cmd(Y_PWM, ENABLE, Y_CH_OUT);	
	}
	
	// Z��
	if((0 == cmd_Plus_Data.plusNum[2]) || (0 == cmd_Plus_Data.clk[2]))
	{
		nAxisStatus[2] = DISABLE;
	}
	else 
	{
		nAxis_StepMotor_Start(Z);
//		PWM_Cmd(Z_PWM, ENABLE, Z_CH_EXTI);
//		PWM_Cmd(Z_PWM, ENABLE, Z_CH_OUT);
	}
	
	// A��
	if((0 == cmd_Plus_Data.plusNum[3]) || (0 == cmd_Plus_Data.clk[3]))
	{
		nAxisStatus[3] = DISABLE;
	}
	else 
	{
		nAxis_StepMotor_Start(A);
//		PWM_Cmd(A_PWM, ENABLE, A_CH_EXTI);
//		PWM_Cmd(A_PWM, ENABLE, A_CH_OUT);
	}
	
	// B��
	if((0 == cmd_Plus_Data.plusNum[4]) || (0 == cmd_Plus_Data.clk[4]))
	{
		nAxisStatus[4] = DISABLE;
	}
	else 
	{
		nAxis_StepMotor_Start(B);
//		PWM_Cmd(B_PWM, ENABLE, B_CH_EXTI);
//		PWM_Cmd(B_PWM, ENABLE, B_CH_OUT);
	}		
}

// �������ͬʱֹͣ��ͬ����
void StepMotor_Stop(void)
{
	nAxis_StepMotor_Stop(X);
	nAxis_StepMotor_Stop(Y);
	nAxis_StepMotor_Stop(Z);
	nAxis_StepMotor_Stop(A);
	nAxis_StepMotor_Stop(B);
}






