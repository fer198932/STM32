//////////////////////////////////////////////////////////////////////////////////	 
// ��ʱ�� 2019��1��29��20:43:08 byYJY
//////////////////////////////////////////////////////////////////////////////////

#include "timer.h"


// ��ʱ���жϷ�����
void TIM7_IRQHandler(void)						
{
	static u32 temp = 0; // �����ã���ɾ��
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)==SET) 		// ����ж�
	{
		TIM_ClearITPendingBit(TIM7,TIM_IT_Update);			// ����жϱ�־λ
		TIM_IRQ_ADDSUB;
	}
}

// ��ʱ����ʼ��
void Timer_Init(void)
{
	AF_Structure_XX TIM_AF_Structure;
	
	/* �Ӽ��ٶ�ʱ����ʼ�� */
	Tim_Base_Init(ADDSUB_TIMER, TIM_ARR, TIM_PSC, ENABLE, 0x01, 0x01, &TIM_AF_Structure);	
}

// ��ʱ����ʼ������ ע��arr����Ϊu32
void Tim_Base_Init(TIM_TypeDef* TIM_N, u16 arr, u16 psc, FunctionalState Enable_IRQ,
	uint8_t IRQPreempPriority, uint8_t IRQSubPriority, AF_Structure_XX* TIM_AF_Structure)
{
	TIM_TimeBaseInitTypeDef	 	TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef 					NVIC_InitStructure;
	
	// ����TIM�ṹ��
	if(!TIM_Structure_Make(TIM_N, TIM_AF_Structure))
		return;
	
	// ʹ��ʱ��
	if((u32)TIM1 > (u32)TIM_N) 			// ʱ���� TIM2��3��4��5��6��7��12��13��14
		RCC_APB1PeriphClockCmd(TIM_AF_Structure->RCC_Periph_N, ENABLE);
	else 														// ʱ���� TIM1��8��9��10��11
		RCC_APB2PeriphClockCmd(TIM_AF_Structure->RCC_Periph_N, ENABLE);
	
	// ����ʱ��
	TIM_TimeBaseInitStructure.TIM_Period = arr-1;													// ��װ��ֵ
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc-1;											// Ԥ��Ƶϵ��
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; 		// ���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 					
	
	TIM_TimeBaseInit(TIM_N, &TIM_TimeBaseInitStructure);
	
	if(ENABLE == Enable_IRQ) 			// ������ʱ���ж�
	{
		TIM_ITConfig(TIM_N, TIM_IT_Update, ENABLE);		// ��ʱ�������ж�
		
		NVIC_InitStructure.NVIC_IRQChannel = TIM_AF_Structure->IRQ_Channel_N; 				//��ʱ���жϷ������
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = IRQPreempPriority; 	//��ռ���ȼ�1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = IRQSubPriority; 						//�����ȼ�3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
		NVIC_Init(&NVIC_InitStructure);
	}
		
	// �رն�ʱ������Ҫʱ�ٴ�
	TIM_Cmd(TIM_N, DISABLE);
}









