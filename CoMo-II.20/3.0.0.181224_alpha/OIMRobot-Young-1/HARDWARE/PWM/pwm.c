//////////////////////////////////////////////////////////////////////////////////	 
//  PWM���ɳ���  2019��1��29��20:16:48 byYJY							  
////////////////////////////////////////////////////////////////////////////////// 

#include "pwm.h"

// ���ص�ǰ������������СƵ��
extern u32 n_Axis_Max_Clk(u8 i);
extern u32 n_Axis_Min_Clk(u8 i);

/* PWM ��ʼ���Ľṹ��  */
NAxis_TIM_Structure 				nAxis_TIM_Structure[AXIS_NUM];

// PWM��ʼ������
void PWM_Init(void)
{
	u8 i;
	for(i=0; i<AXIS_NUM; i++)
	{
		/*  ����PIN����TIM֮���ӳ���ϵ  */
		nAxis2TIM(i, &nAxis_TIM_Structure[i]);
		
		/*  ����PWM��ʼ��  */
		Tim_PWM_Init(&nAxis_TIM_Structure[i], 0x04, 0x04, TIM_ARR, TIM_PSC, PWM_EXTI);
//		delay_us(5);
		Tim_PWM_Init(&nAxis_TIM_Structure[i], 0x04, 0x04, TIM_ARR, TIM_PSC, PWM_OUT);
//		delay_us(5);
	}
}

// ���ָ������TIM֮���ӳ���ϵ
void nAxis2TIM(u8 nAxis, NAxis_TIM_Structure* 	nAxis_TIM_Structure)
{
//	TIM_Temp = TIM_Temp;		// ����warning �������Σ�
	switch(nAxis)
	{
		case 0: 								// X��
			nAxis_TIM_Structure->nAxis = nAxis;
			nAxis_TIM_Structure->TIM_N = X_PWM;
			nAxis_TIM_Structure->ch_exti = X_CH_EXTI;
			nAxis_TIM_Structure->ch_out = X_CH_OUT;
			if(ERROR == GPIO_Structure_Make(PWM_X_EXTI, &(nAxis_TIM_Structure->GPIO_PWM_EXTI)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			if(ERROR == GPIO_Structure_Make(PWM_X_OUT, &(nAxis_TIM_Structure->GPIO_PWM_OUT)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			break;
		case 1:									// Y��
			nAxis_TIM_Structure->nAxis = nAxis;
			nAxis_TIM_Structure->TIM_N = Y_PWM;
			nAxis_TIM_Structure->ch_exti = Y_CH_EXTI;
			nAxis_TIM_Structure->ch_out = Y_CH_OUT;
			if(ERROR == GPIO_Structure_Make(PWM_Y_EXTI, &(nAxis_TIM_Structure->GPIO_PWM_EXTI)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			if(ERROR == GPIO_Structure_Make(PWM_Y_OUT, &(nAxis_TIM_Structure->GPIO_PWM_OUT)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			break;
		case 2:									// Z��
			nAxis_TIM_Structure->nAxis = nAxis;
			nAxis_TIM_Structure->TIM_N = Z_PWM;
			nAxis_TIM_Structure->ch_exti = Z_CH_EXTI;
			nAxis_TIM_Structure->ch_out = Z_CH_OUT;
			if(ERROR == GPIO_Structure_Make(PWM_Z_EXTI, &(nAxis_TIM_Structure->GPIO_PWM_EXTI)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			if(ERROR == GPIO_Structure_Make(PWM_Z_OUT, &(nAxis_TIM_Structure->GPIO_PWM_OUT)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			break;
		case 3:									// A��
			nAxis_TIM_Structure->nAxis = nAxis;
			nAxis_TIM_Structure->TIM_N = A_PWM;
			nAxis_TIM_Structure->ch_exti = A_CH_EXTI;
			nAxis_TIM_Structure->ch_out = A_CH_OUT;
			if(ERROR == GPIO_Structure_Make(PWM_A_EXTI, &(nAxis_TIM_Structure->GPIO_PWM_EXTI)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			if(ERROR == GPIO_Structure_Make(PWM_A_OUT, &(nAxis_TIM_Structure->GPIO_PWM_OUT)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			break;
		case 4:									// B��
			nAxis_TIM_Structure->nAxis = nAxis;
			nAxis_TIM_Structure->TIM_N = B_PWM;
			nAxis_TIM_Structure->ch_exti = B_CH_EXTI;
			nAxis_TIM_Structure->ch_out = B_CH_OUT;
			if(ERROR == GPIO_Structure_Make(PWM_B_EXTI, &(nAxis_TIM_Structure->GPIO_PWM_EXTI)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			if(ERROR == GPIO_Structure_Make(PWM_B_OUT, &(nAxis_TIM_Structure->GPIO_PWM_OUT)))
				nAxis_TIM_Structure->nAxis = GPIO_ERROR;
			break;
		default:
			respMsgError("PWM���趨����������\r\n", 1);
	}
}




// ��ʱ����ʼ�� 1��PWM_chΪPWM���ͨ�� 2���ж����ȼ���Ч ��д0x04
void Tim_PWM_Init(const NAxis_TIM_Structure* 	nAxis_TIM_Structure, uint8_t NVIC_IRQPreemptionPriority, 
	uint8_t NVIC_IRQSubPriority, u16 arr, u16 psc, PWM_Channel PWM_ch)
{
	AF_Structure_XX 		TIM_AF_Structure;
	TIM_OCInitTypeDef  	TIM_OCInitStructure;
	
	// PIN����������ֱ���˳���ʼ��
	if(GPIO_ERROR == nAxis_TIM_Structure->nAxis)
		return;
	
	// ��ʱ����ʼ��
	Tim_Base_Init(nAxis_TIM_Structure->TIM_N, arr, psc, DISABLE, 
		NVIC_IRQPreemptionPriority, NVIC_IRQSubPriority, &TIM_AF_Structure);
	
	// GPIO ��ʼ��
	if(PWM_EXTI == PWM_ch)  // EXTI
		GPIO_PWM_Init(&nAxis_TIM_Structure->GPIO_PWM_EXTI); 
	else 										// OUT
		GPIO_PWM_Init(&nAxis_TIM_Structure->GPIO_PWM_OUT); 	
	
	// ����ӳ��
	if(PWM_EXTI == PWM_ch) 	// EXTI
		GPIO_PinAFConfig(nAxis_TIM_Structure->GPIO_PWM_EXTI.GPIO_Port, 
			nAxis_TIM_Structure->GPIO_PWM_EXTI.GPIO_PinSource_N, TIM_AF_Structure.GPIO_AF);
	else										// OUT
		GPIO_PinAFConfig(nAxis_TIM_Structure->GPIO_PWM_OUT.GPIO_Port, 
			nAxis_TIM_Structure->GPIO_PWM_OUT.GPIO_PinSource_N, TIM_AF_Structure.GPIO_AF);
	
	// ��ʼ��PWM���ͨ��
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 								// ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 		// �Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 				// ������� ע��Ĭ������  byYJY
	
	// �������ͨ��
	if(PWM_EXTI == PWM_ch) 	// EXTI
		Set_PWM_Channel(nAxis_TIM_Structure->TIM_N, &TIM_OCInitStructure, 
			nAxis_TIM_Structure->ch_exti, (arr>>1));
	else										// OUT
		Set_PWM_Channel(nAxis_TIM_Structure->TIM_N, &TIM_OCInitStructure, 
			nAxis_TIM_Structure->ch_out, (arr>>1));
	
	// �ر�PWM����ǿ������
	if(PWM_EXTI == PWM_ch) 	// EXTI
		PWM_Cmd(nAxis_TIM_Structure->TIM_N, DISABLE, nAxis_TIM_Structure->ch_exti);	
	else										// OUT
		PWM_Cmd(nAxis_TIM_Structure->TIM_N, DISABLE, nAxis_TIM_Structure->ch_out);	
}

// ��ʱ�����PIN�ų�ʼ��
static void GPIO_PWM_Init(const GPIO_Structure_XX *GPIO_Temp)
{
	GPIO_InitTypeDef 		GPIO_InitStructure;
	
	// GPIO�ṹ�幹��
//	if(!GPIO_Structure_Make(Pin_PWM_str, GPIO_Temp))
//		return;	
	
	// ʹ��ʱ��
	RCC_AHB1PeriphClockCmd(GPIO_Temp->RCC_Periph_N, ENABLE);
	
	// GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_Temp->GPIO_Pin_N;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;  			// �������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	// 100M
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 			// ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  			// ���� 
	
	GPIO_Init(GPIO_Temp->GPIO_Port, &GPIO_InitStructure);
}

// ���ͨ������ DC��duty cycle ռ�ձ�
static void Set_PWM_Channel(TIM_TypeDef* TIM_N, TIM_OCInitTypeDef*  TIM_OCInitStructure, u8 ch1, u16 DC)
{
	switch(ch1)
	{
		case 0:
			// ���趨
			break;
		
		case 1:
			// ͨ��1
			TIM_OC1Init(TIM_N, TIM_OCInitStructure); 				 									// ����Tָ���Ĳ�����ʼ������
			TIM_OC1PreloadConfig(TIM_N, TIM_OCPreload_Enable); 								// ʹ��TIM��CCR�ϵ�Ԥװ�ؼĴ���
			TIM_ARRPreloadConfig(TIM_N, ENABLE);															// ARPEʹ�� 			
			// �趨ռ�ձ� ��50%
			TIM_SetCompare1(TIM_N, DC);				
			break;
		
		case 2:
			// ͨ��2
			TIM_OC2Init(TIM_N, TIM_OCInitStructure); 				 									// ����Tָ���Ĳ�����ʼ������
			TIM_OC2PreloadConfig(TIM_N, TIM_OCPreload_Enable); 								// ʹ��TIM��CCR�ϵ�Ԥװ�ؼĴ���
			TIM_ARRPreloadConfig(TIM_N, ENABLE);															// ARPEʹ�� 			
			// �趨ռ�ձ� ��50%
			TIM_SetCompare2(TIM_N, DC);		
			break;
		
		case 3:
			// ͨ��3
			TIM_OC3Init(TIM_N, TIM_OCInitStructure); 				 									// ����Tָ���Ĳ�����ʼ������
			TIM_OC3PreloadConfig(TIM_N, TIM_OCPreload_Enable); 								// ʹ��TIM��CCR�ϵ�Ԥװ�ؼĴ���
			TIM_ARRPreloadConfig(TIM_N, ENABLE);															// ARPEʹ�� 			
			// �趨ռ�ձ� ��50%
			TIM_SetCompare3(TIM_N, DC);		
			break;
		
		case 4:
			// ͨ��4
			TIM_OC4Init(TIM_N, TIM_OCInitStructure); 				 									// ����Tָ���Ĳ�����ʼ������
			TIM_OC4PreloadConfig(TIM_N, TIM_OCPreload_Enable); 								// ʹ��TIM��CCR�ϵ�Ԥװ�ؼĴ���
			TIM_ARRPreloadConfig(TIM_N, ENABLE);															// ARPEʹ�� 			
			// �趨ռ�ձ� ��50%
			TIM_SetCompare4(TIM_N, DC);		
			break;
		
		default :
			respMsgError("PWMͨ���趨����\r\n", 1);
			break;
	}
}

// PWM �رջ�ʹ��
void PWM_Cmd(TIM_TypeDef* TIM_N, FunctionalState state, u8 ch)
{
	if(ENABLE == state) 		// ����
	{
		PWM_Forced2High(TIM_N, DISABLE, ch); 		// ȡ��ǿ������
		delay_us(5);
		TIM_Cmd(TIM_N, ENABLE);
	}		
	else 										// �ر�
	{
		TIM_Cmd(TIM_N, DISABLE);  							// �ر�TIM
		delay_us(5);
		PWM_Forced2High(TIM_N, ENABLE, ch);  		// ǿ������
	}
}

// PWM �رջ�ʹ�� �궨��汾
//#define	PWM_Cmd(TIM_N, state, ch)  \
//do 	\
//{ \
//	if(ENABLE == state) 		/* ����  */ \
//	{ 	\
//		PWM_Forced2High(TIM_N, DISABLE, ch); \
//		delay_us(5); \
//		TIM_Cmd(TIM_N, ENABLE); \
//	}		\
//	else 										/* �ر�  */ \
//	{ \
//		TIM_Cmd(TIM_N, DISABLE);  							/* �ر�TIM */ \
//		delay_us(5); \
//		PWM_Forced2High(TIM_N, ENABLE, ch);  		/* ǿ������ */ \
//	}	 \
//} while(0)

// PWM ǿ�����߻�ȡ�� ch:���ͨ��
static void PWM_Forced2High(TIM_TypeDef* TIM_N, FunctionalState state, u8 ch)
{
	if(ENABLE == state) 		// ǿ������
	{
		switch(ch)
		{
			case 1:
				TIM_ForcedOC1Config(TIM_N, TIM_ForcedAction_Active); 		// ǿ������
				break;
			
			case 2:
				TIM_ForcedOC2Config(TIM_N, TIM_ForcedAction_Active); 		// ǿ������
				break;
			
			case 3:
				TIM_ForcedOC3Config(TIM_N, TIM_ForcedAction_Active); 		// ǿ������
				break;
			
			case 4:
				TIM_ForcedOC4Config(TIM_N, TIM_ForcedAction_Active); 		// ǿ������
				break;
			
			default:
				respMsgError("PWMǿ��������������\r\n", 1);
				break;
		}
	}		
	else 										// ȡ��ǿ������
	{
		switch(ch)
		{
			case 1:
				TIM_SelectOCxM(TIM_N, TIM_Channel_1, TIM_OCMode_PWM1);  // ע�ⶨʱ��ģʽTIM_OCMode_PWM1
				TIM_CCxCmd(TIM_N, TIM_Channel_1, TIM_CCx_Enable);				
				break;
			
			case 2:
				TIM_SelectOCxM(TIM_N, TIM_Channel_2, TIM_OCMode_PWM1);  // ע�ⶨʱ��ģʽTIM_OCMode_PWM1
				TIM_CCxCmd(TIM_N, TIM_Channel_2, TIM_CCx_Enable);				
				break;
			
			case 3:
				TIM_SelectOCxM(TIM_N, TIM_Channel_3, TIM_OCMode_PWM1);  // ע�ⶨʱ��ģʽTIM_OCMode_PWM1
				TIM_CCxCmd(TIM_N, TIM_Channel_3, TIM_CCx_Enable);				
				break;
			
			case 4:
				TIM_SelectOCxM(TIM_N, TIM_Channel_4, TIM_OCMode_PWM1);  // ע�ⶨʱ��ģʽTIM_OCMode_PWM1
				TIM_CCxCmd(TIM_N, TIM_Channel_4, TIM_CCx_Enable);				
				break;
			
			default:
				respMsgError("PWMȡ��ǿ��������������\r\n", 1);
				break;
		}
	}	
}

// ͨ��CLKƵ�ʼ���Ԥ��Ƶϵ�� PSC ע��Ĵ����������
u16 calPSC(u32 clk, u8 nAxis)
{
	u16 psc;
	if((clk >= n_Axis_Min_Clk(nAxis)) && (clk <= n_Axis_Max_Clk(nAxis)))
	{
		psc = PSC_CLK / clk;
	}
	else
	{
		psc = PSC_CLK / n_Axis_Min_Clk(nAxis);
	}
	return psc;
}

// ͨ��PSC����Ƶ�ʣ� ע��Ĵ����������  ��λHZ
u32 calClk_PSC(u16 psc)
{
	u32 clk;
	clk = PSC_CLK / psc;
	return clk;
}



