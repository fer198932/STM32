#include "StepMotor.h"			

//////////////////////////////////////////////////////////////////////////////////	 
// 步进电机控制程序
// 1、
// 2019年1月31日13:20:00 byYJY
//////////////////////////////////////////////////////////////////////////////////

// 指令中设定的运动参数
extern 	volatile 	FunctionalState 		Offline_Work_Flag; 						// 进入脱机加工的标记
extern 	Flag_Structure 				flag_Struct;
extern	Motion_Strcuture 			motion_Data;	
extern	Motion_Strcuture 			motion_Data_Pre;	


// 换向间隙相关
const	 u32	 Backlash_PlusNum = BacklashCompensation * SUBDIV_NUM / (1e3);
BacklashCompensation_Structure		backlashCompen;


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
//	static volatile uint8_t dirOld = 0;  		// 最开始的方向
	
	if(TBD_DIR == dir)					// 方向未设定时，啥都不干
		return;
	
	if(POS_DIR == dir) 			// 正向
	{
		GPIO_SetBits(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N);
	}
	else      							// 负向
	{
		GPIO_ResetBits(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N);  
	}
	
//		while(GPIO_ReadOutputDataBit(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N) == dirOld) ; 	// 换向完成退出循环
//	delay_us(DIR_EX_DALAY); 			// 一定延时

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
	// 限位触发时，将该轴的脉冲数、频率置零
	StepMotor_Limited(flag_Struct.Limiti_Flag);
	
	// X轴
	if(ENABLE == nAxisStatus[0])
	{
		// 宏定义启动
		nAxis_StepMotor_Start(X);
//		PWM_Cmd(X_PWM, ENABLE, X_CH_EXTI);
//		PWM_Cmd(X_PWM, ENABLE, X_CH_OUT);
	}
	
	// Y轴
	if(ENABLE == nAxisStatus[1])
	{
		nAxis_StepMotor_Start(Y);
//		PWM_Cmd(Y_PWM, ENABLE, Y_CH_EXTI);
//		PWM_Cmd(Y_PWM, ENABLE, Y_CH_OUT);	
	}
	
	// Z轴
	if(ENABLE == nAxisStatus[2])
	{
		nAxis_StepMotor_Start(Z);
//		PWM_Cmd(Z_PWM, ENABLE, Z_CH_EXTI);
//		PWM_Cmd(Z_PWM, ENABLE, Z_CH_OUT);
	}
	
	// A轴
	if(ENABLE == nAxisStatus[3])
	{
		nAxis_StepMotor_Start(A);
//		PWM_Cmd(A_PWM, ENABLE, A_CH_EXTI);
//		PWM_Cmd(A_PWM, ENABLE, A_CH_OUT);
	}
	
	// B轴
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
		
		
		// 回复上位机
#if PRIN2DISP
//		respUsartMsg("PWM_Cmd\r\n", 10);
#else
//		respUsartMsg(backResString_Motion, RESP_MOTIONMSG_LENGTH);
#endif	
	}
}

// 步进电机同时停止（同步） 没有强制拉高
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

// 步进电机运行设定距离
void StepMotor_Move(N_Axis n_axis, u32 Clk, u32 PlusNum, Motor_Dir Dir)
{		
	if(n_axis >= ALL_Axis)
	{
		respMsgError("设定电机运动错误\r\n", 1);
		return;
	}

	/* 各轴初始化 */
	// 设定运动参数
	mymemset(&motion_Data_Pre, 0, sizeof(motion_Data_Pre));	
	
	motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[n_axis] = PlusNum;
	motion_Data_Pre.cmd_Datas.plus_Datas.clk[n_axis] = Clk;
	motion_Data_Pre.cmd_Datas.plus_Datas.dir[n_axis] = Dir;
	
	
	// 加减速初始化
	AddSubSpeedInit_Pre();		// 注意这里可能会对设定的频率进行调整
	
	// 运动状态初始化
	motion_Init();
	delay_us(MOTION_START_DALAY); 			// 一定延时
	
	// 步进电机同时开始运动
	StepMotor_Start();
	
	// 加减速定时器开启
	EN_ADDSUB_TIMER;
}

// 所有轴的步进电机运行设定距离
void StepMotor_All_Move(u32 Clk, u32 PlusNum, Motor_Dir Dir[])
{		
	u8 i;
	
	mymemset(&motion_Data_Pre, 0, sizeof(motion_Data_Pre));
	
	for(i=0; i<AXIS_NUM; i++)			// 暂时取消A、B轴的运动  byYJY
	{
		motion_Data_Pre.cmd_Datas.plus_Datas.plusNum[i] = PlusNum;
		motion_Data_Pre.cmd_Datas.plus_Datas.clk[i] = Clk;
		motion_Data_Pre.cmd_Datas.plus_Datas.dir[i] = Dir[i];
	}
	
	// 加减速初始化
	AddSubSpeedInit_Pre();		// 注意这里可能会对设定的频率进行调整
	
	// 运动状态初始化
	motion_Init();
	delay_us(MOTION_START_DALAY); 			// 一定延时
	
	// 步进电机同时开始运动
	StepMotor_Start();

	// 加减速定时器开启
	EN_ADDSUB_TIMER;
}

// 初始化时候的间隙补偿 0.1mm
void Move2CompensateBacklash(void)
{	
	Motor_Dir posDir[AXIS_NUM] = {POS_DIR, POS_DIR, POS_DIR, TBD_DIR, TBD_DIR};
	Motor_Dir negDir[AXIS_NUM] = {NEG_DIR, NEG_DIR, NEG_DIR, TBD_DIR, TBD_DIR};
	
	StepMotor_All_Move(StepMotor_MinClk, SUBDIV_NUM/10, posDir);		// 0.1mm
	delay_ms(100);
	StepMotor_All_Move(StepMotor_MinClk, SUBDIV_NUM/10, negDir);
	delay_ms(100);
	
//	backlashCompen.motorDirOld[i] = NEG_DIR;
	mymemset((void*)backlashCompen.motorDirOld, NEG_DIR, sizeof(FlagStatus) * AXIS_NUM);
	
#if BACKLASH_COMPENSATION
	backlashCompen.DirChange_En = ENABLE;
#endif
}



// 运动停止前，会死循环在该函数里
void StepMotor_Move_Done(void)
{
	while(0 != nAxis_Motion_Flag) ;
}






