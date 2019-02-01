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
volatile 	FunctionalState	 nAxisStatus[AXIS_NUM] = {ENABLE, ENABLE, ENABLE, ENABLE, ENABLE};

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
	volatile uint8_t dirOld;  		// �ʼ�ķ���
	dirOld = GPIO_ReadOutputDataBit(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N);
	
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
		
		while(GPIO_ReadOutputDataBit(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N) == dirOld) ; 	// ��������˳�ѭ��
		delay_us(DIR_EX_DALAY); 			// һ����ʱ
	}
	
	// else ���û���
}

// �������ͬʱ��ʼ�˶�����֤ͬ���� �Ĵ�����ʽ�����Ч��
void StepMotor_Start(void)
{
	// X��
	if((0 == cmd_Plus_Data.plusNum[0]) || (0 == cmd_Plus_Data.clk[0]))
	{
		nAxisStatus[0] = ENABLE;
	}
	else 
	{
		nAxis_StepMotor_Start(X_PWM);
	}
	
	// Y��
	if((0 == cmd_Plus_Data.plusNum[1]) || (0 == cmd_Plus_Data.clk[1]))
	{
		nAxisStatus[1] = ENABLE;
	}
	else 
	{
		nAxis_StepMotor_Start(Y_PWM);
	}
	
	// Z��
	if((0 == cmd_Plus_Data.plusNum[2]) || (0 == cmd_Plus_Data.clk[2]))
	{
		nAxisStatus[2] = ENABLE;
	}
	else 
	{
		nAxis_StepMotor_Start(Z_PWM);
	}
	
	// A��
	if((0 == cmd_Plus_Data.plusNum[3]) || (0 == cmd_Plus_Data.clk[3]))
	{
		nAxisStatus[3] = ENABLE;
	}
	else 
	{
		nAxis_StepMotor_Start(A_PWM);
	}
	
	// B��
	if((0 == cmd_Plus_Data.plusNum[4]) || (0 == cmd_Plus_Data.clk[4]))
	{
		nAxisStatus[4] = ENABLE;
	}
	else 
	{
		nAxis_StepMotor_Start(B_PWM);
	}		
}

// �������ͬʱֹͣ��ͬ����
void StepMotor_Stop(void)
{
	nAxis_StepMotor_Stop(X_PWM);
	nAxis_StepMotor_Stop(Y_PWM);
	nAxis_StepMotor_Stop(Z_PWM);
	nAxis_StepMotor_Stop(A_PWM);
	nAxis_StepMotor_Stop(B_PWM);
}






