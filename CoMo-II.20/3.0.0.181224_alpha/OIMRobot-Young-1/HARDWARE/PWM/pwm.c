//////////////////////////////////////////////////////////////////////////////////	 
//  PWM���ɳ���  2019��1��29��20:16:48 byYJY							  
////////////////////////////////////////////////////////////////////////////////// 

#include "pwm.h"

/* PWM ��ʼ���Ľṹ��  */
static  	GPIO_Structure_XX 		GPIO_PWM_Plus[AXIS_NUM];
// static 		AF_Structure_XX 			AF_PWM_Plus[AXIS_NUM];

// PWM��ʼ������
void PWM_Init(void)
{
	/*  ����PWM��ʼ��  */
	Tim_PWM_Init(GPIO_PWM_Plus+0, X_PWM, PWM_X_EXTI, 0x04, 0x04, TIM_ARR, TIM_PSC, X_CH_EXTI);
	Tim_PWM_Init(GPIO_PWM_Plus+0, X_PWM, PWM_X_OUT, 0x04, 0x04, TIM_ARR, TIM_PSC, X_CH_OUT);
	
	Tim_PWM_Init(GPIO_PWM_Plus+1, Y_PWM, PWM_Y_EXTI, 0x04, 0x04, TIM_ARR, TIM_PSC, Y_CH_EXTI);
	Tim_PWM_Init(GPIO_PWM_Plus+1, Y_PWM, PWM_Y_OUT, 0x04, 0x04, TIM_ARR, TIM_PSC, Y_CH_OUT);
	
	Tim_PWM_Init(GPIO_PWM_Plus+2, Z_PWM, PWM_Z_EXTI, 0x04, 0x04, TIM_ARR, TIM_PSC, Z_CH_EXTI);
	Tim_PWM_Init(GPIO_PWM_Plus+2, Z_PWM, PWM_Z_OUT, 0x04, 0x04, TIM_ARR, TIM_PSC, Z_CH_OUT);
	
	Tim_PWM_Init(GPIO_PWM_Plus+3, A_PWM, PWM_A_EXTI, 0x04, 0x04, TIM_ARR, TIM_PSC, A_CH_EXTI);
	Tim_PWM_Init(GPIO_PWM_Plus+3, A_PWM, PWM_A_OUT, 0x04, 0x04, TIM_ARR, TIM_PSC, A_CH_OUT);
	
	Tim_PWM_Init(GPIO_PWM_Plus+4, B_PWM, PWM_B_EXTI, 0x04, 0x04, TIM_ARR, TIM_PSC, B_CH_EXTI);
	Tim_PWM_Init(GPIO_PWM_Plus+4, B_PWM, PWM_B_OUT, 0x04, 0x04, TIM_ARR, TIM_PSC, B_CH_OUT);
	
	
}

// ��ʱ����ʼ�� 1��ch1��ch2ΪPWM���ͨ��(Ϊ0ɶ������) 2���ж����ȼ���Ч ��д0x04
static void Tim_PWM_Init(GPIO_Structure_XX *GPIO_Temp, TIM_TypeDef* TIM_N, const char Pin_PWM_str[], 
	uint8_t NVIC_IRQPreemptionPriority,	uint8_t NVIC_IRQSubPriority, u16 arr, u16 psc, u8 ch1)
{
	AF_Structure_XX 		TIM_AF_Structure;
	TIM_OCInitTypeDef  	TIM_OCInitStructure;
	
	// GPIO ��ʼ��
	GPIO_PWM_Init(GPIO_Temp, Pin_PWM_str); 	
	
	// ��ʱ����ʼ��
	Tim_Base_Init(TIM_N, arr, psc, DISABLE, 
		NVIC_IRQPreemptionPriority, NVIC_IRQSubPriority, &TIM_AF_Structure);
	
	// ����ӳ��
	GPIO_PinAFConfig(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_PinSource_N, 
		TIM_AF_Structure.GPIO_AF);
	
	// ��ʼ��PWM���ͨ��
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 								// ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 		// �Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 				// ������� ע��Ĭ������  byYJY
	
	// �������ͨ��
	Set_PWM_Channel(TIM_N, &TIM_OCInitStructure, ch1, (arr>>1));
	
	// �ر�PWM����ǿ������
	PWM_Cmd(TIM_N, DISABLE, ch1);	
}

// ��ʱ�����PIN�ų�ʼ��
static void GPIO_PWM_Init(GPIO_Structure_XX *GPIO_Temp, const char Pin_PWM_str[])
{
	GPIO_InitTypeDef 		GPIO_InitStructure;
	
	// GPIO�ṹ�幹��
	if(!GPIO_Structure_Make(Pin_PWM_str, GPIO_Temp))
		return;	
	
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
			TIM_OC1Init(TIM_N, TIM_OCInitStructure); 				 								// ����Tָ���Ĳ�����ʼ������
			TIM_OC1PreloadConfig(TIM_N, TIM_OCPreload_Enable); 								// ʹ��TIM��CCR�ϵ�Ԥװ�ؼĴ���
			TIM_ARRPreloadConfig(TIM_N, ENABLE);															// ARPEʹ�� 			
			// �趨ռ�ձ� ��50%
			TIM_SetCompare1(TIM_N, DC);				
			break;
		
		case 2:
			// ͨ��2
			TIM_OC2Init(TIM_N, TIM_OCInitStructure); 				 								// ����Tָ���Ĳ�����ʼ������
			TIM_OC2PreloadConfig(TIM_N, TIM_OCPreload_Enable); 								// ʹ��TIM��CCR�ϵ�Ԥװ�ؼĴ���
			TIM_ARRPreloadConfig(TIM_N, ENABLE);															// ARPEʹ�� 			
			// �趨ռ�ձ� ��50%
			TIM_SetCompare2(TIM_N, DC);		
			break;
		
		case 3:
			// ͨ��3
			TIM_OC3Init(TIM_N, TIM_OCInitStructure); 				 								// ����Tָ���Ĳ�����ʼ������
			TIM_OC3PreloadConfig(TIM_N, TIM_OCPreload_Enable); 								// ʹ��TIM��CCR�ϵ�Ԥװ�ؼĴ���
			TIM_ARRPreloadConfig(TIM_N, ENABLE);															// ARPEʹ�� 			
			// �趨ռ�ձ� ��50%
			TIM_SetCompare3(TIM_N, DC);		
			break;
		
		case 4:
			// ͨ��4
			TIM_OC4Init(TIM_N, TIM_OCInitStructure); 				 								// ����Tָ���Ĳ�����ʼ������
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
		PWM_Forced2High(TIM_N, DISABLE, ch);
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







