/********************************************************************************************************
* 回零程序 
* 2019年3月8日09:29:46 byYJY
*
*
********************************************************************************************************/
#include "Return2Zero.h"

extern 		FunctionalState	 				nAxisStatus[AXIS_NUM];  						// 各轴是否可以运动的标志
extern 		vu32 										plusNumPWM[AXIS_NUM]; 							// 脉冲数的中断计数
extern 		NAxis_TIM_Structure			nAxis_TIM_Structure[AXIS_NUM];  		// PWM 初始化的结构体 
extern 		Flag_Structure 					flag_Struct;


static	 DesPosition 	workPos;			// 回零前的工作位置

// 回到零点，限位触发
void return2Zero(void)
{
/*Motor_Dir		dir2Limit[] = {NEG_DIR, POS_DIR, POS_DIR, TBD_DIR, TBD_DIR};		// 注释别删！ */
	Motor_Dir		dir2Far[] = {POS_DIR, NEG_DIR, NEG_DIR, TBD_DIR, TBD_DIR};	
	
	/* 应该在相关电机都停止运动后才回零 */
	// 等待步进电机停止运动
	StepMotor_Move_Done();	
	delay_ms(10);
	
	// 设置标志位
	///////////////////////
	
	// 主轴电机停止转动
	mainMotorStop();
	delay_ms(500);	
	
	// 初始化相关参数值
	mymemset(&workPos, 0, sizeof(workPos));	
	
	// Z轴向上进入限位位置
	workPos2Limit(Z_Axis, Z_AXIS_MAX_CLK>>2, POS_DIR, Z_P_Lim);
	delay_ms(10);	
	workPos.posPlusNum[Z_Axis] = plusNumPWM[Z_Axis];		// 保存原来的工作位置
	// X轴向左进入限位位置
	workPos2Limit(X_Axis, X_AXIS_MAX_CLK>>2, NEG_DIR, X_N_Lim);
	delay_ms(10);	
	workPos.posPlusNum[X_Axis] = plusNumPWM[X_Axis];		// 保存原来的工作位置
	// Y轴向后进入限位位置
	workPos2Limit(Y_Axis, Y_AXIS_MAX_CLK>>2, POS_DIR, Y_P_Lim);
	delay_ms(10);
	workPos.posPlusNum[Y_Axis] = plusNumPWM[Y_Axis];		// 保存原来的工作位置
	
	// 适当离开限位位置
	StepMotor_All_Move(StepMotor_MinClk, MIN_DIS*SUBDIV_NUM, dir2Far);
	StepMotor_Move_Done();
	delay_ms(10);
	
	// 精准定位
	// Z轴向上进入限位位置
	workPos2Limit(Z_Axis, Z_AXIS_MIN_CLK, POS_DIR, Z_P_Lim);
	delay_ms(10);	
	workPos.posPlusNum[Z_Axis] = workPos.posPlusNum[Z_Axis] - 
			MIN_DIS*SUBDIV_NUM + plusNumPWM[Z_Axis];		// 保存原来的工作位置
	// X轴向左进入限位位置
	workPos2Limit(X_Axis, X_AXIS_MIN_CLK, NEG_DIR, X_N_Lim);
	delay_ms(10);	
	workPos.posPlusNum[X_Axis] = workPos.posPlusNum[X_Axis] - 
			MIN_DIS*SUBDIV_NUM + plusNumPWM[X_Axis];		// 保存原来的工作位置
	// Y轴向后进入限位位置
	workPos2Limit(Y_Axis, Y_AXIS_MIN_CLK, POS_DIR, Y_P_Lim);
	delay_ms(10);
	workPos.posPlusNum[Y_Axis] = workPos.posPlusNum[Y_Axis] - 
			MIN_DIS*SUBDIV_NUM + plusNumPWM[Y_Axis];		// 保存原来的工作位置
			
	mymemcpy(&(workPos.posDir), dir2Far, sizeof(dir2Far));
	
	
	// 重置标志位
	//////////////////////
	
	// 清空串口数据，避免执行命令时有串口命令进来
	if(SET == flag_Struct.USART_IDLE_Flag)
	{
		flag_Struct.USART_IDLE_Flag = RESET;
		respMsgError("回零执行中，不接收串口命令！\r\n", 1);					
		// 重置缓冲区
		buffer_Reset();		
	}
}

// 指定轴从工作位置移动到限位点
static void workPos2Limit(N_Axis n, u32 _clk, Motor_Dir _dir, u8 (*fuc_limit)())
{
	StepMotor_Move(n, _clk, MAX_DIS*SUBDIV_NUM, _dir);
	while(0 != nAxis_Motion_Flag)
	{
		if(0 == (*fuc_limit)())		// 限位发生
		{
			nAxis_StepMotor_Stop_MACRO(nAxis_TIM_Structure[n].TIM_N, nAxis_TIM_Structure[n].ch_exti, 
																	nAxis_TIM_Structure[n].ch_out);
			
			nAxisStatus[n] = DISABLE;
			
			DIS_ADDSUB_TIMER;
			
//			workPos.posPlusNum[n] = plusNumPWM[n];		// 保存原来的工作位置
			
			break;
		}
	}
}	


// 从零点到指定位置，按设定的值运动
static void zeroBack2Pos(DesPosition* pDesPos)
{	
	// X、Y轴运动
	StepMotor_Move(X_Axis, StepMotor_MaxClk>>2, pDesPos->posPlusNum[X_Axis], pDesPos->posDir[X_Axis]);		
	StepMotor_Move(Y_Axis, StepMotor_MaxClk>>2, pDesPos->posPlusNum[Y_Axis], pDesPos->posDir[Y_Axis]);
	StepMotor_Move_Done();
	delay_ms(10);
	
	// z轴运动
	StepMotor_Move(Z_Axis, StepMotor_MaxClk>>2, pDesPos->posPlusNum[Z_Axis], pDesPos->posDir[Z_Axis]);
	StepMotor_Move_Done();
	delay_ms(10);
}





























