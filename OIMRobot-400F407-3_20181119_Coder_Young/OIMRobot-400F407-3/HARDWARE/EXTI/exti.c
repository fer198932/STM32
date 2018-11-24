/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          ԭ���������������
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : Exit.C
* By  : Hiker Lee
* 2018-04-10
* 20180725�жϹܽŶ����޸�:PA1-->PB1;PA0-->PB0;
********************************************************************************************************/


#include "exti.h"
#include "delay.h" 
#include "led.h" 
#include "key.h"
#include "ChangeTools.h"		// �����˻��������Ľṹ��
#include "ComDataProc.h"

////////////////////////////////////////////////////////////////////////////////// 

extern u32 xAxisPlusNum,yAxisPlusNum,zAxisPlusNum,aAxisPlusNum,bAxisPlusNum;

extern nAxisSetData allAxisSetData[3];			// X��Y��Z���趨���˶�����
extern u8 setedDataResFlag;

u32 pwmNumTim12=0;
u32 pwmNumTim9=0;
u32 pwmNumTim3=0;
u32 pwmNumTim4=0;
u32 pwmNumTim2=0;			//Y

extern u8 xAxisStepFlag; 
extern u8 yAxisStepFlag; 
extern u8 zAxisStepFlag; 
extern u8 aAxisStepFlag; 
extern u8 bAxisStepFlag; 

u8 urgentStopFlag = -1; 
// u8 i;

//�ⲿ�ж�0�������
void EXTI0_IRQHandler(void)
{
	 EXTI_ClearITPendingBit(EXTI_Line0); 	//���LINE0�ϵ��жϱ�־λ 
	 pwmNumTim2++;
	 if(pwmNumTim2>=yAxisPlusNum)
	 	{
			TIM2_PWM_Stop();
			pwmNumTim2 = 0;
			yAxisStepFlag = 2;
			
			// ��¼������е�ʱ��
			allAxisSetData[1].subLastTime = TIM7->CNT;
			TIM_Cmd(TIM7,DISABLE);												// �ر�TIM7��ʱ
			setedDataResFlag = 0;													// �˶�ֹͣ�������λ
	 	}
}

//�ⲿ�ж�1�������
void EXTI1_IRQHandler(void)
{
	 EXTI_ClearITPendingBit(EXTI_Line1); 	//���LINE1�ϵ��жϱ�־λ 
	 pwmNumTim3++;
	 if(pwmNumTim3>=xAxisPlusNum)
	 	{
			TIM3_PWM_Stop();
			pwmNumTim3 = 0;					//180612 test
			xAxisStepFlag = 2;
			
			// ��¼������е�ʱ��
			allAxisSetData[0].subLastTime = TIM7->CNT;
			TIM_Cmd(TIM7,DISABLE);												// �ر�TIM7��ʱ
			setedDataResFlag = 0;													// �˶�ֹͣ�������λ
	 	}
}	

//�ⲿ�ж�2�������
void EXTI2_IRQHandler(void)
{
if(EXTI_GetITStatus(EXTI_Line2)!=RESET)		//�ж�ĳ�����ϵ��ж��Ƿ��� 
	{
	EXTI_ClearITPendingBit(EXTI_Line2);    //���LINE2�ϵ��жϱ�־λ 
	pwmNumTim4++;
	if(pwmNumTim4>=zAxisPlusNum)
	   {
		   	TIM4_PWM_Stop();
		    pwmNumTim4 = 0;
		   	zAxisStepFlag = 2;
			 
			 // ��¼������е�ʱ��
			allAxisSetData[2].subLastTime = TIM7->CNT;
			TIM_Cmd(TIM7,DISABLE);												// �ر�TIM7��ʱ
			setedDataResFlag = 0;													// �˶�ֹͣ�������λ
	   }
	}
}
//�ⲿ�ж�3�������
void EXTI3_IRQHandler(void)
{
	 EXTI_ClearITPendingBit(EXTI_Line3);  	//���LINE3�ϵ��жϱ�־λ  
	 pwmNumTim12++;
	 if(pwmNumTim12>=bAxisPlusNum)
	 	{
			TIM12_PWM_Stop();
			pwmNumTim12 = 0;
			bAxisStepFlag = 2;
	 	}
}
//�ⲿ�ж�4�������
void EXTI4_IRQHandler(void)
{ 
	 EXTI_ClearITPendingBit(EXTI_Line4);	//���LINE4�ϵ��жϱ�־λ  
	 pwmNumTim9++;
	 if(pwmNumTim9>=aAxisPlusNum)
	 	{
			TIM9_PWM_Stop();
			pwmNumTim9 = 0;
			aAxisStepFlag = 2;
	 	}
}

u8 UrgentStopTest(void)
{
	u8 temp,temp2;
	delay_ms(3);
	temp = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_5);
	delay_ms(2);
	temp2 = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_5);		//180625

	if(temp == temp2)
	{
		if(temp == 0)		//��������		
		{
			return 0;
		}
		else
			return 1;		//��������
	}
	else
		return 2;			//״̬�仯�������¼�⣻
	
}
//�ⲿ�ж�5�������
void EXTI9_5_IRQHandler(void)
{ 
	u8 temp,temp2;
	if(EXTI_GetITStatus(EXTI_Line5)!=RESET) 		//��ͣ�ź�
	{
		EXTI_ClearITPendingBit(EXTI_Line5); 		//����ж�
		delay_ms(3);
		temp = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_5);
		delay_ms(2);
		temp2 = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_5);		//180625
		if(temp == temp2)
		{
			if(temp == 0)		//��������			
			{
				mainMotorStop();
				
				TIM_Cmd(TIM3, DISABLE); 	//ʹ��x
				TIM_Cmd(TIM4, DISABLE); 	//ʹ��z
				TIM_Cmd(TIM9, DISABLE); 	//ʹ��a
				TIM_Cmd(TIM12, DISABLE);	//ʹ��b
				TIM_Cmd(TIM5, DISABLE);		//ʹ��y
				TIM_Cmd(TIM2, DISABLE);
				
				urgentStopFlag = 1;
			}
			else			//��������		//urgentStopFlag = 3;			//�������º������
			{
				if(urgentStopFlag == 3)
				urgentStopFlag = 0;
			}
		}
		
	}	
	if(EXTI_GetITStatus(EXTI_Line6)!=RESET) 		//�����ź�
	{
		EXTI_ClearITPendingBit(EXTI_Line6); 		//����ж�
		delay_us(100);
		temp = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_6);
		delay_us(1000);
		temp2 = GPIO_ReadInputDataBit(GPIOG,GPIO_Pin_6);
		
		if(temp == temp2)
		{
		if(temp == 0)
			{
			mainMotorDecode();
			delay_us(100);			//�ֶ����룬��λ������������ͣ��
			mainMotorWork();		//hiker 0522 test
			}
		}
	}
	
	if(EXTI_GetITStatus(EXTI_Line7)!=RESET) 		
	{
		EXTI_ClearITPendingBit(EXTI_Line7); 		//����ж�
	}	
	if(EXTI_GetITStatus(EXTI_Line8)!=RESET) 		
	{
		EXTI_ClearITPendingBit(EXTI_Line8); 		//����ж�
	}	
	if(EXTI_GetITStatus(EXTI_Line9)!=RESET) 		
	{
		EXTI_ClearITPendingBit(EXTI_Line9); 		//����ж�
	}	
}

void EXTIIO_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);		//ʹ��GPIOEʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;				//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//����
  GPIO_Init(GPIOE, &GPIO_InitStructure);					//��ʼ��GPIOE0,1,2,3,4

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);		//ʹ��GPIOAʱ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;					//x���ж�����
  GPIO_Init(GPIOB, &GPIO_InitStructure);					//��ʼ��GPIOA1

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);		//ʹ��GPIOGʱ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;			//5��ͣ�ź�,6�����ź�
  GPIO_Init(GPIOG, &GPIO_InitStructure);						//��ʼ��GPIOG5,6
} 

void motorDirIO_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);		//ʹ��GPIOFʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//����
  GPIO_Init(GPIOF, &GPIO_InitStructure);					//��ʼ��GPIOF0,1,2,3,4

} 
	   
//�ⲿ�жϳ�ʼ������
//��ʼ��PE0~4Ϊ�ж�����.
void EXTIX_Init(void)
{
	NVIC_InitTypeDef   NVIC_InitStructure;
	EXTI_InitTypeDef   EXTI_InitStructure;
	
	EXTIIO_Init(); 	//������Ӧ��IO�ڳ�ʼ��
 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);		//ʹ��SYSCFGʱ��
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);��
	
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);		//PB0 ���ӵ��ж���0
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);		//PB1 ���ӵ��ж���1
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);		//PE2 ���ӵ��ж���2
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);		//PE3 ���ӵ��ж���3
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);		//PE4 ���ӵ��ж���4

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource5);		//PG5 ���ӵ��ж���5
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOG, EXTI_PinSource6);		//PG6 ���ӵ��ж���6
		
	// ����EXTI_Line0,1,2,3,4 
	EXTI_InitStructure.EXTI_Line = EXTI_Line0 | EXTI_Line1 | EXTI_Line2 | EXTI_Line3 | EXTI_Line4| EXTI_Line6;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;		//�ж��¼�
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½��ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;				//�ж���ʹ��
    EXTI_Init(&EXTI_InitStructure);							//����

	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;				//�ж��¼�
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; 	//�½��ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;						//�ж���ʹ��
    EXTI_Init(&EXTI_InitStructure);									//����
 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//�ⲿ�ж�0
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);								//����

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//�ⲿ�ж�0			x
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);								//����

	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//�ⲿ�ж�2			z
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�3
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x03;		//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);								//����
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;			//�ⲿ�ж�3			b
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);								//����
	
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;			//�ⲿ�ж�4			a
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�1
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;		//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);								//����

	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;			//�ⲿ�ж�4
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		//�����ȼ�2
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 			//ʹ���ⲿ�ж�ͨ��
	NVIC_Init(&NVIC_InitStructure); 							//����

}


void disableZaxisCountINT()
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//�ⲿ�ж�2
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�3
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;		//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;			//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);								//����

}

void enableZaxisCountINT()
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;			//�ⲿ�ж�2
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�3
 	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;		//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);								//����

}

void disableXaxisCountINT()
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//�ⲿ�ж�0
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;			//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);								//����

}

void enableXaxisCountINT()
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;			//�ⲿ�ж�0
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;//��ռ���ȼ�0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;		//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);								//����

}

void disableYaxisCountINT()
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//�ⲿ�ж�0
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;			//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);								//����

}

void enableYaxisCountINT()
{
	NVIC_InitTypeDef   NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;			//�ⲿ�ж�0
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;//��ռ���ȼ�0
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x02;		//�����ȼ�2
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;				//ʹ���ⲿ�ж�ͨ��
  	NVIC_Init(&NVIC_InitStructure);								//����

}




