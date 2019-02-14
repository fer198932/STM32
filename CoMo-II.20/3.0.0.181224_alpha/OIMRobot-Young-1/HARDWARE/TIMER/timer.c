//////////////////////////////////////////////////////////////////////////////////	 
// 定时器 2019年1月29日20:43:08 byYJY
//////////////////////////////////////////////////////////////////////////////////

#include "timer.h"


extern 	PSC_Data_Array				Psc_Data_Cur[AXIS_NUM];							// 存储PSC值的结构体数组
extern 	NAxis_TIM_Structure		nAxis_TIM_Structure[AXIS_NUM];  		// PWM 初始化的结构体  
extern 	AddSubSpeedStatus addSubSpeed_Status[AXIS_NUM];		// 加减速状态标记


// 记录加减速定时器运行的步数
u16 addSubSpeed_StepNum[AXIS_NUM] = {0, 0, 0, 0, 0};


// 定时器中断服务函数
void TIM7_IRQHandler(void)						
{
//	static u32 temp = 0; // 测试用，可删除
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)==SET) 		// 溢出中断
	{
		TIM_ClearITPendingBit(TIM7,TIM_IT_Update);			// 清除中断标志位
		TIM_IRQ_ADDSUB;
	}
}

// 定时器初始化
void Timer_Init(void)
{
	AF_Structure_XX TIM_AF_Structure;
	
	/* 加减速定时器初始化 */
	Tim_Base_Init(ADDSUB_TIMER, TIM_ARR, TIM_PSC, ENABLE, 0x01, 0x01, &TIM_AF_Structure);	
}

// 定时器初始化函数 注：arr可以为u32
void Tim_Base_Init(TIM_TypeDef* TIM_N, u16 arr, u16 psc, FunctionalState Enable_IRQ,
	uint8_t IRQPreempPriority, uint8_t IRQSubPriority, AF_Structure_XX* TIM_AF_Structure)
{
	TIM_TimeBaseInitTypeDef	 	TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef 					NVIC_InitStructure;
	
	// 构造TIM结构体
	if(!TIM_Structure_Make(TIM_N, TIM_AF_Structure))
		return;
	
	// 使能时钟
	if((u32)TIM1 > (u32)TIM_N) 			// 时钟线 TIM2、3、4、5、6、7、12、13、14
		RCC_APB1PeriphClockCmd(TIM_AF_Structure->RCC_Periph_N, ENABLE);
	else 														// 时钟线 TIM1、8、9、10、11
		RCC_APB2PeriphClockCmd(TIM_AF_Structure->RCC_Periph_N, ENABLE);
	
	// 配置时钟
	TIM_TimeBaseInitStructure.TIM_Period = arr-1;													// 重装载值
	TIM_TimeBaseInitStructure.TIM_Prescaler = psc-1;											// 预分频系数
	TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up; 		// 向上计数模式
	TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1; 					
	
	TIM_TimeBaseInit(TIM_N, &TIM_TimeBaseInitStructure);
	
	if(ENABLE == Enable_IRQ) 			// 开启定时器中断
	{
		TIM_ITConfig(TIM_N, TIM_IT_Update, ENABLE);		// 定时器更新中断
		
		NVIC_InitStructure.NVIC_IRQChannel = TIM_AF_Structure->IRQ_Channel_N; 				//定时器中断服务程序
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = IRQPreempPriority; 	//抢占优先级1
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = IRQSubPriority; 						//子优先级3
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
		NVIC_Init(&NVIC_InitStructure);
	}
		
	// 关闭定时器，需要时再打开
	TIM_Cmd(TIM_N, DISABLE);
}

// 加减速定时器的中断服务程序
static void addSubTime_IRQ_MARCO(void)
{
	u8 i;
	
	for(i=0; i<AXIS_NUM; i++)
	{
		/* 步数递增超出范围，不进行动作  */
		if((ADD_SPEED == addSubSpeed_Status[i]) && 
			(addSubSpeed_StepNum[i] >=  (int)Psc_Data_Cur[i].length))
		{
			addSubSpeed_Status[i] = CONST_SPEED; 			// 匀速阶段
			continue;
		}
		
		/* 该轴未开启加减速 */
		if(DISABLE == Psc_Data_Cur[i].enAddSubFlag) 
			continue;
		
		/* 加速阶段  */
		if(ADD_SPEED == addSubSpeed_Status[i])
		{
			nAxisSetPWM(nAxis_TIM_Structure[i].TIM_N, 
				Psc_Data_Cur[i].psc_data[addSubSpeed_StepNum[i]++]); 		
			continue;
		}
		
		/* 减速阶段  */
		if(SUB_SPEED == addSubSpeed_Status[i])
		{			
			nAxisSetPWM(nAxis_TIM_Structure[i].TIM_N, 
				Psc_Data_Cur[i].psc_data[--addSubSpeed_StepNum[i]]);
		}
	}
}







