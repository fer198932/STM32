/********************************************************************************************************
*       �жϵ�IO�ڴ������  2019��1��29��09:26:51 byYJY
********************************************************************************************************/

#include "exti.h"

extern volatile 	FunctionalState 		Offline_Work_Flag; 		// �����ѻ��ӹ��ı��

static GPIO_Structure_XX GPIO_EXTI_Plus[AXIS_NUM]; 			// ���������PWM�����ж� ˳��X��Y��Z��A��B

#if PRIN2DISP
#else
static u8 backResString[RESP_MOTIONMSG_LENGTH];										// �������飬��ʱ�˶���  byYJY
#endif

#if _TEST	
	/* ��ť�жϣ�������  */
static	GPIO_Structure_XX 	GPIO_Key1;
#endif	


// ָ�����趨���˶�����
extern Proc_Data cmd_Proc_Data; 		// �������ݣ��г�Աָ��plus_Data
extern Plus_Data cmd_Plus_Data;			// �������ݣ����Ƶ���˶�

// �洢PSCֵ�Ľṹ������
extern PSC_Data_Array	Psc_Data_Cur[AXIS_NUM];
extern AddSubSpeedStatus addSubSpeed_Status[AXIS_NUM];		// �Ӽ���״̬���
extern FunctionalState	 		nAxisStatus[AXIS_NUM];  	// �����Ƿ�����˶��ı�־


// ���������жϼ���
u32 pluNumPWM[AXIS_NUM] = {0, 0, 0, 0, 0};


// �ⲿ�жϳ�ʼ��		 	
void EXTI_Config_Init(void)
{
	/* ���PWM��������IO�ڳ�ʼ�� �ж���0-4 ��ռ���ȼ�Ϊ0x0-��� */
	RCC_Periph_N(GPIO_EXTI_Plus+0, EXTI_X_PLUS, EXTI_Trigger_Rising, 0x00, 0x00);
//	delay_ms(5);
	RCC_Periph_N(GPIO_EXTI_Plus+1, EXTI_Y_PLUS, EXTI_Trigger_Rising, 0x00, 0x00);
//	delay_ms(5);
	RCC_Periph_N(GPIO_EXTI_Plus+2, EXTI_Z_PLUS, EXTI_Trigger_Rising, 0x00, 0x01);
//	delay_ms(5);
	RCC_Periph_N(GPIO_EXTI_Plus+3, EXTI_A_PLUS, EXTI_Trigger_Rising, 0x00, 0x02);
//	delay_ms(5);
	RCC_Periph_N(GPIO_EXTI_Plus+4, EXTI_B_PLUS, EXTI_Trigger_Rising, 0x00, 0x02);
//	delay_ms(5);

#if _TEST	
	/* ��ť�жϣ�������  */
	RCC_Periph_N(&GPIO_Key1, _KEY1, EXTI_Trigger_Rising, 0x01, 0x00);
#endif	
	
#if PRIN2DISP
#else
	setRespStr_Motion(backResString, RESP_MOTIONMSG_LENGTH, 0x01);
#endif	
	
}

// �жϷ������
void EXTI0_IRQHandler(void)
{ 	
	EXTI_ClearITPendingBit(EXTI_Line0);			// ����жϱ�־λ
	
//	EXTI_IRQ_PWM(2, Z_PWM, Z);
	
	EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
}

// �жϷ������
void EXTI1_IRQHandler(void)
{ 	
	EXTI_ClearITPendingBit(EXTI_Line1);			// ����жϱ�־λ
	
#if _TEST_ON_ALIENTEK
	/* ɶ������ */
#else
	EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#endif
	
//	EXTI_IRQ_PWM(1, Y_PWM, Y);
	
	
}

// �жϷ������
void EXTI2_IRQHandler(void)
{ 
	EXTI_ClearITPendingBit(EXTI_Line2);			// ����жϱ�־λ
	
#if _TEST_ON_ALIENTEK	
	EXTI_IRQ_PWM_MACRO(1, Y_PWM, Y_CH_EXTI, Y_CH_OUT);
#else
	/* ɶ������ */
#endif
	
//	EXTI_IRQ_PWM(2, Z_PWM, Z);
}

// �жϷ������
void EXTI3_IRQHandler(void)
{ 
	EXTI_ClearITPendingBit(EXTI_Line3);			// ����жϱ�־λ
	
//	EXTI_IRQ_PWM(3, A_PWM, A);
}

// �жϷ������
void EXTI4_IRQHandler(void)
{ 
	EXTI_ClearITPendingBit(EXTI_Line4);			// ����жϱ�־λ
	
	EXTI_IRQ_PWM_MACRO(0, X_PWM, X_CH_EXTI, X_CH_OUT); 
	
//	/* �жϷ������ */
//	EXTI->PR = EXTI_Line4; 	/* ����ж� */
//	
//	pluNumPWM[0]++;
//	
//	/* ������ٽ׶� */
//	if((SUB_SPEED != addSubSpeed_Status[0]) && 
//		(Psc_Data_Cur[0].addSpeed_NeedPlusNum > (cmd_Plus_Data.plusNum[0] - pluNumPWM[0])))
//	{
////		ADDSUB_TIMER->CNT = 0;
//		addSubSpeed_Status[0] = SUB_SPEED; 
//	}
//	
//	/*  �˶���� �ر�PWM */
//	if(cmd_Plus_Data.plusNum[0] < pluNumPWM[0])
//	{
//		/* nAxis_StepMotor_Stop_MACRO(TIM_N, ch_exti, ch_out); 		*/
//		// ͨ��2 ch_exti
//		X_PWM->CR1 &= (uint16_t)~TIM_CR1_CEN;															// ֹͣPWm����
//		X_PWM->CCMR1 = ((X_PWM->CCMR1) & ((uint16_t)~TIM_CCMR1_OC2M)) | 
//										((uint16_t)(TIM_ForcedAction_Active << 8)); 			// PWM��ƽǿ������		
//		// ͨ��1 ch_out
//		X_PWM->CR1 &= (uint16_t)~TIM_CR1_CEN;															// ֹͣPWm����	
//		X_PWM->CCMR1 = ((X_PWM->CCMR1) & ((uint16_t)~TIM_CCMR1_OC1M)) | 
//										((uint16_t)(TIM_ForcedAction_Active)); 			// PWM��ƽǿ������		
//		
//		nAxisStatus[0] = DISABLE;
//		
//		/* respUsartMsg("PWM_EXTI\r\n", 10); */
//		
//		/* ���е��ᶼֹͣ��ֹͣ ADDSUB_TIMER  */
//		if(0 == nAxis_Motion_Flag)
//		{
//#if OFFLINE_WORK
//			Offline_Work_Flag = ENABLE;
//#endif
//			/* TIM_Cmd(ADDSUB_TIMER, DISABLE); */
//			X_PWM->CR1 &= (uint16_t)~TIM_CR1_CEN;
//		}
//	}	
}

// �жϷ������
void EXTI9_5_IRQHandler(void)
{ 
//	EXTI_ClearITPendingBit(EXTI_Line1);			// ����жϱ�־λ
	
	if(EXTI_GetITStatus(EXTI_Line5) != RESET)		// KEY1����
	{
		EXTI_ClearITPendingBit(EXTI_Line5);				// ����жϱ�־λ
		respUsartMsg("KEY1\r\n", 6);							// ��ӡ
		
	}
	
	if(EXTI_GetITStatus(EXTI_Line6) != RESET)		// KEY1����
	{
		EXTI_ClearITPendingBit(EXTI_Line6);				// ����жϱ�־λ
		
		EXTI_IRQ_PWM_MACRO(2, Z_PWM, Z_CH_EXTI, Z_CH_OUT);
		
		
		
//		respUsartMsg("EXTI_6\r\n", 8);							// ��ӡ		
	}

}

// �жϷ������
void EXTI15_10_IRQHandler(void)
{ 
//	EXTI_ClearITPendingBit(EXTI_Line1);			// ����жϱ�־λ
	
}



// ��Ӧ��IO���жϳ�ʼ��
static void RCC_Periph_N(GPIO_Structure_XX *GPIO_Temp, const char str[], EXTITrigger_TypeDef EXTI_Trigger_N, 
	uint8_t NVIC_IRQPreemptionPriority,	uint8_t NVIC_IRQSubPriority)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	/* ��Ӧ��GPIO�ڳ�ʼ�� */
	EXTI_GPIO_Init(GPIO_Temp, str);	
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);		// ʹ��SYSCFGʱ�� �ж�ʹ��
	
	SYSCFG_EXTILineConfig(GPIO_Temp->EXTI_PortGPIO_N, GPIO_Temp->EXTI_PinSource_N);		// ���ӵ��ж��� 
	
	// �����ж��� 
	EXTI_InitStructure.EXTI_Line = GPIO_Temp->EXTI_Line_N;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//�ж��¼�
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_N; 	// ������ʽ ע�⿼�Ǵ�����ʽ byYJY
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//�ж���ʹ��
	EXTI_Init(&EXTI_InitStructure);							//����
	
	// �����ж� ///////
	NVIC_InitStructure.NVIC_IRQChannel = GPIO_Temp->NVIC_IRQ_N;			//�ⲿ�ж�0
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = NVIC_IRQPreemptionPriority;//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = NVIC_IRQSubPriority;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure);								//����	
	
	// �ر��ж�
	EXTI_Cmd(GPIO_Temp->EXTI_Line_N, DISABLE);
}

// IO�ڵ�GPIO��ʼ��
static void EXTI_GPIO_Init(GPIO_Structure_XX *GPIO_Temp, const char str[])
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(!GPIO_Structure_Make(str, GPIO_Temp))
		return;
	
	RCC_AHB1PeriphClockCmd(GPIO_Temp->RCC_Periph_N, ENABLE);		// ʹ��ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Temp->GPIO_Pin_N;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  			//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//100M
	
#if _TEST	
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;  			//��������  ���԰����ϵ�KEY1ʱ�ã���ɾ��
//	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  			// ��������  ���԰����ϵ�KEY1ʱ�ã���ɾ��
#else
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  			//�������� 
#endif
	
	GPIO_Init(GPIO_Temp->GPIO_Port, &GPIO_InitStructure);	
}

// �ж�ʹ�ܻ�ر�
static void EXTI_Cmd(uint32_t EXTI_Line_N, FunctionalState state)
{
	if(ENABLE == state)
	{
		EXTI->IMR |= EXTI_Line_N;					// ʹ���ⲿ�ж�
	}
	else 
	{
		EXTI->IMR &= ~(EXTI_Line_N); 			// �����ⲿ�ж�
	}
}

// �����ж���ʹ��
void EXTI_Enable(void)
{
	EXTI_Cmd((GPIO_EXTI_Plus+0)->EXTI_Line_N, ENABLE);
	EXTI_Cmd((GPIO_EXTI_Plus+1)->EXTI_Line_N, ENABLE);
	EXTI_Cmd((GPIO_EXTI_Plus+2)->EXTI_Line_N, ENABLE);
	EXTI_Cmd((GPIO_EXTI_Plus+3)->EXTI_Line_N, ENABLE);
	EXTI_Cmd((GPIO_EXTI_Plus+4)->EXTI_Line_N, ENABLE);

#if _TEST	
	EXTI_Cmd(GPIO_Key1.EXTI_Line_N, ENABLE);	
#endif
	
//	EXTI_Cmd(EXTI_Line4, ENABLE);
}

// �����õĸ���ֹͣ����
static void nAxis_StepMotor_Stop_MACRO(TIM_TypeDef* TIM_N, u8 ch_exti, u8 ch_out)
{
	PWM_Cmd(TIM_N, DISABLE, ch_exti); 	
	PWM_Cmd(TIM_N, DISABLE, ch_out); 	
}

// �����õ��жϷ������
static void EXTI_IRQ_PWM_MACRO(u8 n, TIM_TypeDef *TIM_N, u8 ch_exti, u8 ch_out)
{	
	pluNumPWM[n]++;
	
	/* ������ٽ׶� */
	if((CONST_SPEED == addSubSpeed_Status[n]) && 
		(Psc_Data_Cur[n].addSpeed_NeedPlusNum > (cmd_Plus_Data.plusNum[n] - pluNumPWM[n])))
	{
//		ADDSUB_TIMER->CNT = 0;
		addSubSpeed_Status[n] = SUB_SPEED; 
	}
	
	/*  �˶���� �ر�PWM */
#if _TEST_ON_ALIENTEK
	if(cmd_Plus_Data.plusNum[n] <= pluNumPWM[n])
#else
	if(cmd_Plus_Data.plusNum[n] <= (pluNumPWM[n]-1))
#endif
	{
		nAxis_StepMotor_Stop_MACRO(TIM_N, ch_exti, ch_out); 		//  ע�������ú궨��
		
		nAxisStatus[n] = DISABLE;
		
		/* ���е��ᶼֹͣ��ֹͣ ADDSUB_TIMER  */
		if(0 == nAxis_Motion_Flag)
		{
#if OFFLINE_WORK
			Offline_Work_Flag = ENABLE;
#endif
//			TIM_Cmd(ADDSUB_TIMER, DISABLE);
			DIS_ADDSUB_TIMER;
#if PRIN2DISP					
			respUsartMsg("PWM_EXTI\r\n", 10);
#else
			respUsartMsg(backResString, RESP_MOTIONMSG_LENGTH);
#endif
		}
	}
}

#if PRIN2DISP
#else
// �趨�˶����ݵĴ��ڷ�������ĸ�ʽ
static void setRespStr_Motion(u8 respStr[], u16 length, u8 status)
{
	setRespStr(backResString, RESP_MOTIONMSG_LENGTH);
	backResString[2] = DATAINFO;
	backResString[5] = PLUS_DATA;
	backResString[6] = 0x00;
	backResString[7] = status;
}
#endif


