#ifndef __EXTI_H
#define __EXTI_H	 
#include "sys.h" 
#include "config.h"
#include "ComDataProc.h"
#include "main.h"

/********************************************************************************************************
*       中断的IO口处理程序  2019年1月29日09:26:51 byYJY
********************************************************************************************************/

#define 	RESP_MOTIONMSG_LENGTH 	9




// 中断线与GPIO映射关系



// 外部中断初始化		 	
void EXTI_Config_Init(void);		


// 中断服务程序
void EXTI0_IRQHandler(void);
void EXTI1_IRQHandler(void);
void EXTI2_IRQHandler(void);
void EXTI3_IRQHandler(void);
void EXTI4_IRQHandler(void);
void EXTI9_5_IRQHandler(void);
void EXTI15_10_IRQHandler(void);

// 对应的IO口中断初始化
// 对应的IO口中断初始化
static void RCC_Periph_N(GPIO_Structure_XX *GPIO_Temp, const char str[], EXTITrigger_TypeDef EXTI_Trigger_N, 
	uint8_t NVIC_IRQPreemptionPriority,	uint8_t NVIC_IRQSubPriority);

// IO口的GPIO初始化
static void EXTI_GPIO_Init(GPIO_Structure_XX *GPIO_Temp, const char str[]);

// 中断使能或关闭
static void EXTI_Cmd(uint32_t EXTI_Line_N, FunctionalState state);

// 所有中断线使能
void EXTI_Enable(void);

/* 为脉冲服务的中断程序 */
//#define EXTI_IRQ_PWM(EXTI_Line_n, n, TIM_N, nAxis)  			\
//do 		\
//{ \
//	EXTI->PR = EXTI_Line_n; 	/* 清除中断 */
//	
//	pluNumPWM[n]++;
//	
//	/* 进入减速阶段 */
//	if((SUB_SPEED != addSubSpeed_Status[n]) && 
//		(Psc_Data_Cur[n].addSpeed_NeedPlusNum > (cmd_Plus_Data.plusNum[n] - pluNumPWM[n])))
//	{
////		ADDSUB_TIMER->CNT = 0;
//		addSubSpeed_Status[n] = SUB_SPEED; 
//	}
//	
//	/*  运动完成 关闭PWM */
//	if(cmd_Plus_Data.plusNum[n] < pluNumPWM[n])
//	{
//		/* nAxis_StepMotor_Stop_MACRO(TIM_N, ch_exti, ch_out); 		*/
//		/* 手写才行 */
//		
//		nAxisStatus[n] = DISABLE;
//		
//		/* respUsartMsg("PWM_EXTI\r\n", 10); */
//		
//		/* 所有的轴都停止才停止 ADDSUB_TIMER  */
//		if(0 == nAxis_Motion_Flag)
//		{
//#if OFFLINE_WORK
//			Offline_Work_Flag = ENABLE;
//#endif
//			/* TIM_Cmd(ADDSUB_TIMER, DISABLE); */
//			TIM_N->CR1 &= (uint16_t)~TIM_CR1_CEN;
//		}
//	}
//	
//	\
//} while(0)


// 测试用的中断服务程序
static void EXTI_IRQ_PWM_MACRO(u8 n, TIM_TypeDef *TIM_N, u8 ch_exti, u8 ch_out);

// 测试用的各轴停止函数
static void nAxis_StepMotor_Stop_MACRO(TIM_TypeDef* TIM_N, u8 ch_exti, u8 ch_out);


#if PRIN2DISP
#else
// 设定运动数据的串口反馈数组的格式
static void setRespStr_Motion(u8 respStr[], u16 length, u8 status);
#endif






#endif

























