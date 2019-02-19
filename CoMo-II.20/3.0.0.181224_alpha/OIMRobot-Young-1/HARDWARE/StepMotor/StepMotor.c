#include "StepMotor.h"			

//////////////////////////////////////////////////////////////////////////////////	 
// 步进电机控制程序
// 1、
// 2019年1月31日13:20:00 byYJY
//////////////////////////////////////////////////////////////////////////////////

// 指令中设定的运动参数
extern Proc_Data cmd_Proc_Data; 		// 命令数据，有成员指向plus_Data
extern Plus_Data cmd_Plus_Data;			// 脉冲数据，控制电机运动

// 各轴是否可以运动 
FunctionalState	 nAxisStatus[AXIS_NUM] = {DISABLE, DISABLE, DISABLE, DISABLE, DISABLE};

// 电机方向IO口
GPIO_Structure_XX  	StepMotor_Dir[AXIS_NUM];

// 步进电机初始化 主要是方向IO口的初始化
void StepMotor_Init(void)
{
	Motor_Dir_Init(StepMotor_Dir+0, DIR_X);
	Motor_Dir_Init(StepMotor_Dir+1, DIR_Y);
	Motor_Dir_Init(StepMotor_Dir+2, DIR_Z);
	Motor_Dir_Init(StepMotor_Dir+3, DIR_A);
	Motor_Dir_Init(StepMotor_Dir+4, DIR_B);
	
}

// 步进电机运动IO口方向初始化
static void Motor_Dir_Init(GPIO_Structure_XX *GPIO_Temp, const char str[])
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	// 构造GPIO
	if(!GPIO_Structure_Make(str, GPIO_Temp)) return;
	
	// 使能时钟
	RCC_AHB1PeriphClockCmd(GPIO_Temp->RCC_Periph_N, ENABLE);
	
	// GPIO配置
	GPIO_InitStructure.GPIO_Pin = GPIO_Temp->GPIO_Pin_N;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;						// 普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;					// 推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;			// 100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;						// 上拉 默认是正方向运动
  GPIO_Init(GPIO_Temp->GPIO_Port, &GPIO_InitStructure);									// 初始化GPIO	
}

// 电机运动方向设定 
void Motor_Dir_Set(GPIO_Structure_XX *GPIO_Temp, Motor_Dir dir)
{	
	static volatile uint8_t dirOld = 0;  		// 最开始的方向
	
	if(TBD_DIR == dir)					// 方向未设定时，啥都不干
		return;
	
	// 需要换向
	if(dirOld != dir)
	{
		if(POS_DIR == dir) 			// 正向
		{
			GPIO_SetBits(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N);
		}
		else      							// 负向
		{
			GPIO_ResetBits(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N);  
		}
		
//		while(GPIO_ReadOutputDataBit(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N) == dirOld) ; 	// 换向完成退出循环
		delay_us(DIR_EX_DALAY); 			// 一定延时
		dirOld = dir;
	}
	
	// else 不用换向
}

// 电机运动方向设定 备份1 
//void Motor_Dir_Set(GPIO_Structure_XX *GPIO_Temp, Motor_Dir dir)
//{	
//	volatile uint8_t dirOld;  		// 最开始的方向
//	
//	if(TBD_DIR == dir)					// 方向未设定时，啥都不干
//		return;
//	
//	dirOld = GPIO_ReadOutputDataBit(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N);
//	
//	// 需要换向
//	if(dirOld != dir)
//	{
//		if(POS_DIR == dir) 			// 正向
//		{
//			GPIO_SetBits(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N);
//		}
//		else      							// 负向
//		{
//			GPIO_ResetBits(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N);  
//		}
//		
//		while(GPIO_ReadOutputDataBit(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N) == dirOld) ; 	// 换向完成退出循环
//		delay_us(DIR_EX_DALAY); 			// 一定延时
//	}
//	
//	// else 不用换向
//}

// 步进电机同时开始运动（保证同步） 寄存器方式，提高效率
void StepMotor_Start(void)
{
	// X轴
	if((0 == cmd_Plus_Data.plusNum[0]) || (0 == cmd_Plus_Data.clk[0]))
	{
		nAxisStatus[0] = DISABLE;
	}
	else 
	{
		// 宏定义启动
		nAxis_StepMotor_Start(X);
//		PWM_Cmd(X_PWM, ENABLE, X_CH_EXTI);
//		PWM_Cmd(X_PWM, ENABLE, X_CH_OUT);
	}
	
	// Y轴
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
	
	// Z轴
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
	
	// A轴
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
	
	// B轴
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

// 步进电机同时停止（同步）
void StepMotor_Stop(void)
{
	nAxis_StepMotor_Stop(X);
	nAxis_StepMotor_Stop(Y);
	nAxis_StepMotor_Stop(Z);
	nAxis_StepMotor_Stop(A);
	nAxis_StepMotor_Stop(B);
}






