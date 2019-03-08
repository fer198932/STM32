/********************************************************************************************************
* ������� 
* 2019��3��8��09:29:46 byYJY
*
*
********************************************************************************************************/
#include "Return2Zero.h"

extern 		FunctionalState	 				nAxisStatus[AXIS_NUM];  						// �����Ƿ�����˶��ı�־
extern 		vu32 										plusNumPWM[AXIS_NUM]; 							// ���������жϼ���
extern 		NAxis_TIM_Structure			nAxis_TIM_Structure[AXIS_NUM];  		// PWM ��ʼ���Ľṹ�� 
extern 		Flag_Structure 					flag_Struct;


static	 DesPosition 	workPos;			// ����ǰ�Ĺ���λ��

// �ص���㣬��λ����
void return2Zero(void)
{
/*Motor_Dir		dir2Limit[] = {NEG_DIR, POS_DIR, POS_DIR, TBD_DIR, TBD_DIR};		// ע�ͱ�ɾ�� */
	Motor_Dir		dir2Far[] = {POS_DIR, NEG_DIR, NEG_DIR, TBD_DIR, TBD_DIR};	
	
	/* Ӧ������ص����ֹͣ�˶���Ż��� */
	// �ȴ��������ֹͣ�˶�
	StepMotor_Move_Done();	
	delay_ms(10);
	
	// ���ñ�־λ
	///////////////////////
	
	// ������ֹͣת��
	mainMotorStop();
	delay_ms(500);	
	
	// ��ʼ����ز���ֵ
	mymemset(&workPos, 0, sizeof(workPos));	
	
	// Z�����Ͻ�����λλ��
	workPos2Limit(Z_Axis, Z_AXIS_MAX_CLK>>2, POS_DIR, Z_P_Lim);
	delay_ms(10);	
	workPos.posPlusNum[Z_Axis] = plusNumPWM[Z_Axis];		// ����ԭ���Ĺ���λ��
	// X�����������λλ��
	workPos2Limit(X_Axis, X_AXIS_MAX_CLK>>2, NEG_DIR, X_N_Lim);
	delay_ms(10);	
	workPos.posPlusNum[X_Axis] = plusNumPWM[X_Axis];		// ����ԭ���Ĺ���λ��
	// Y����������λλ��
	workPos2Limit(Y_Axis, Y_AXIS_MAX_CLK>>2, POS_DIR, Y_P_Lim);
	delay_ms(10);
	workPos.posPlusNum[Y_Axis] = plusNumPWM[Y_Axis];		// ����ԭ���Ĺ���λ��
	
	// �ʵ��뿪��λλ��
	StepMotor_All_Move(StepMotor_MinClk, MIN_DIS*SUBDIV_NUM, dir2Far);
	StepMotor_Move_Done();
	delay_ms(10);
	
	// ��׼��λ
	// Z�����Ͻ�����λλ��
	workPos2Limit(Z_Axis, Z_AXIS_MIN_CLK, POS_DIR, Z_P_Lim);
	delay_ms(10);	
	workPos.posPlusNum[Z_Axis] = workPos.posPlusNum[Z_Axis] - 
			MIN_DIS*SUBDIV_NUM + plusNumPWM[Z_Axis];		// ����ԭ���Ĺ���λ��
	// X�����������λλ��
	workPos2Limit(X_Axis, X_AXIS_MIN_CLK, NEG_DIR, X_N_Lim);
	delay_ms(10);	
	workPos.posPlusNum[X_Axis] = workPos.posPlusNum[X_Axis] - 
			MIN_DIS*SUBDIV_NUM + plusNumPWM[X_Axis];		// ����ԭ���Ĺ���λ��
	// Y����������λλ��
	workPos2Limit(Y_Axis, Y_AXIS_MIN_CLK, POS_DIR, Y_P_Lim);
	delay_ms(10);
	workPos.posPlusNum[Y_Axis] = workPos.posPlusNum[Y_Axis] - 
			MIN_DIS*SUBDIV_NUM + plusNumPWM[Y_Axis];		// ����ԭ���Ĺ���λ��
			
	mymemcpy(&(workPos.posDir), dir2Far, sizeof(dir2Far));
	
	
	// ���ñ�־λ
	//////////////////////
	
	// ��մ������ݣ�����ִ������ʱ�д����������
	if(SET == flag_Struct.USART_IDLE_Flag)
	{
		flag_Struct.USART_IDLE_Flag = RESET;
		respMsgError("����ִ���У������մ������\r\n", 1);					
		// ���û�����
		buffer_Reset();		
	}
}

// ָ����ӹ���λ���ƶ�����λ��
static void workPos2Limit(N_Axis n, u32 _clk, Motor_Dir _dir, u8 (*fuc_limit)())
{
	StepMotor_Move(n, _clk, MAX_DIS*SUBDIV_NUM, _dir);
	while(0 != nAxis_Motion_Flag)
	{
		if(0 == (*fuc_limit)())		// ��λ����
		{
			nAxis_StepMotor_Stop_MACRO(nAxis_TIM_Structure[n].TIM_N, nAxis_TIM_Structure[n].ch_exti, 
																	nAxis_TIM_Structure[n].ch_out);
			
			nAxisStatus[n] = DISABLE;
			
			DIS_ADDSUB_TIMER;
			
//			workPos.posPlusNum[n] = plusNumPWM[n];		// ����ԭ���Ĺ���λ��
			
			break;
		}
	}
}	


// ����㵽ָ��λ�ã����趨��ֵ�˶�
static void zeroBack2Pos(DesPosition* pDesPos)
{	
	// X��Y���˶�
	StepMotor_Move(X_Axis, StepMotor_MaxClk>>2, pDesPos->posPlusNum[X_Axis], pDesPos->posDir[X_Axis]);		
	StepMotor_Move(Y_Axis, StepMotor_MaxClk>>2, pDesPos->posPlusNum[Y_Axis], pDesPos->posDir[Y_Axis]);
	StepMotor_Move_Done();
	delay_ms(10);
	
	// z���˶�
	StepMotor_Move(Z_Axis, StepMotor_MaxClk>>2, pDesPos->posPlusNum[Z_Axis], pDesPos->posDir[Z_Axis]);
	StepMotor_Move_Done();
	delay_ms(10);
}





























