/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          ԭ���������������
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F407ZG �����޸�оƬ������
*                                              
* File : Main.C
* By  : Young
* 2018-12-24
*
*��д��λ�����Ƴ��� 2018��12��24��15:45:07
********************************************************************************************************/
#include "main.h"

/* ȫ�ֱ��� (��ҪΪ���ֱ�־λ) */
Flag_Structure 	flag_Struct;
//volatile 	FlagStatus 	DMA_Out_Flag 	= RESET;								// DMA��������³�ʼ��
//volatile 	FlagStatus 	USART_IDLE_Flag 	= RESET; 						// ���ڿ����жϱ�־���������ݴ���
//volatile 	FunctionalState 	Cmd_Execute_Flag	= DISABLE;					// ָ�����ɱ�־
//volatile	FunctionalState 	Cmd_Copied_Falg = ENABLE; 			// ���������Ƿ�ɸ��Ƶı�ǩ������ִ�����ſ�)

volatile 	FunctionalState 	Offline_Work_Flag = DISABLE; 		// �����ѻ��ӹ��ı��
volatile 	u32 	Offline_Data_Num = 0;					// �ѻ��ӹ����ݵı��


#if _TEST_ON_ALIENTEK
void Delay(__IO uint32_t nCount);

void Delay(__IO uint32_t nCount)
{
  while(nCount--){}
}
#endif

int main(void)
{	
	u32 t=0;
	
	
	Sys_Init();		// ϵͳ��ʼ������
	Sys_Enable();	// ϵͳʹ�ܺ���
	
  while(1){
		/* ѭ��ɨ���Ƿ�����λ����  */
		procLimit();		
		
		/* �������ڿ����ж�ʱ����  */
		if(SET == flag_Struct.USART_IDLE_Flag)
		{	
			/* ��������  */
			if(RESET == flag_Struct.Cmd_ProcDone_Flag)
			{
				flag_Struct.USART_IDLE_Flag = RESET;
				
				
				UsartDataProc();
				
				CMD_Proc();
				
				flag_Struct.Cmd_ProcDone_Flag = SET;
			}
			
			/*  ִ������������  */
			if(RESET == flag_Struct.Cmd_Executing_Flag)
			{
				flag_Struct.Cmd_Execute_En = ENABLE;
			}	
		}		

		
		/* ִ������ */
		if(ENABLE == flag_Struct.Cmd_Execute_En)
		{
			flag_Struct.Cmd_Execute_En = DISABLE; 
			
			flag_Struct.Cmd_Executing_Flag = SET;						// ����ִ�����ǰ�������ٴ�ִ��
			
			CMD_Execute();
			
			flag_Struct.Cmd_ProcDone_Flag = RESET;			
		}
		
		/* �ѻ��ӹ� */
		if((0 != Offline_Data_Num) && (ENABLE == Offline_Work_Flag))
		{
			delay_ms(10);

#if PRIN2DISP			
			// test
			printf("t:%d\r\n", Offline_Data_Num);
#endif
			
 			Offline_Work_Flag = DISABLE;
 			Offline_Data_Num--;
			offlineWork(LENGTH_OFFL - Offline_Data_Num - 1);
			
		}
		
		
		
		
		// �������
//		if(t%2)
//			XAxisDir_Pos;
//		else
//			XAxisDir_Neg;
		
//    printf("t:%d\r\n",t);
//		delay_ms(500);
		t++;
		
//		TIM_Cmd(ADDSUB_TIMER, ENABLE);
//		if(X_P_Lim())
//			printf("û����λ\r\n");
//		else
//			printf("��λ\r\n");
		
//		LED0 = t%2;

	}
}


// ϵͳ��ʼ������ ����ʼ����������ֻ����һ�Σ���ΪҰָ��Ĵ��ڿ��ܵ���ϵͳ���⣩
void Sys_Init(void)
{
	Flag_Struct_Init();
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);					// �жϷ�������
	buffer_Init();					// ���ݻ�������ʼ��
	comData_Init();
	uart_init(BOUND_X);
	DMA_USART_Init();		// ���ڵ�DMA���䷽ʽ��ʼ��
	delay_init(_SYSCLK);
	LED_Init();
	limit_IO_Init();
	Timer_Init();	
	PWM_Init();
	EXTI_Config_Init();				// �ж�IO�ڳ�ʼ�� �ж�����ʼ��
	StepMotor_Init();				// �����������IO�ڳ�ʼ��
}

// ϵͳ������ʹ�ܺ�������LEDʹ�ܡ��ж�ʹ�ܵ�
void Sys_Enable(void)
{
//	USART_Cmd(USART_X, ENABLE);			// ����ʹ��	
	///////////// ���и����ж�ʹ��  byYJY ///////////////
	
	/* DMA ����ʹ�� */
	USART_DMACmd(USART_X, USART_DMAReq_Tx, ENABLE);  // ʹ�ܴ��ڵ�DMA����     
	USART_DMACmd(USART_X, USART_DMAReq_Rx, ENABLE);  // ʹ�ܴ��ڵ�DMA����
	
	/* �ж�ʹ�� */
	EXTI_Enable();
	
	/* ʹ�ܼӼ���ʱ�ӣ� �����ã���ɾ��  */
//	TIM_Cmd(ADDSUB_TIMER, ENABLE);
	
	/* ʹ��pwm�� �����ã���ɾ��  */
//	PWM_Cmd(X_PWM, ENABLE, X_CH_EXTI);
//	PWM_Cmd(X_PWM, ENABLE, X_CH_OUT);
//	
//	PWM_Cmd(Y_PWM, ENABLE, Y_CH_EXTI);
//	PWM_Cmd(Y_PWM, ENABLE, Y_CH_OUT);
//	
//	PWM_Cmd(Z_PWM, ENABLE, Z_CH_EXTI);
//	PWM_Cmd(Z_PWM, ENABLE, Z_CH_OUT); 
}


// �����ṹ���ʼ��
void Flag_Struct_Init(void)
{
	mymemset(&flag_Struct, 0, sizeof(flag_Struct));
//	flag_Struct.Cmd_Execute_Flag = ENABLE;
}






