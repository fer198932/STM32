/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          ԭ���������������
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : Pwmint.C
* By  : Hiker Lee
* 2018-04-26
********************************************************************************************************/

#include "pwmint.h"
#include "led.h"
#include "usart.h"

//TIM12 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM12_PWM_Init(u32 arr,u32 psc)			//b��
{		 					 
	//�˲������ֶ��޸�IO������
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM_Cmd(TIM12, DISABLE);  			//ʹ��TIM9

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM12,ENABLE);  	//TIM14ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//ʹ��PORTFʱ��	
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_TIM12); //GPIOF9����Ϊ��ʱ��14
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_TIM12); //GPIOF9����Ϊ��ʱ��14
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;        //����
	GPIO_Init(GPIOB,&GPIO_InitStructure);              	//��ʼ��PF9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM12,&TIM_TimeBaseStructure);//��ʼ����ʱ��14
	
	//��ʼ��TIM14 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 		//�������:TIM����Ƚϼ��Ե�
	TIM_OC1Init(TIM12, &TIM_OCInitStructure); 				 		//����Tָ���Ĳ�����ʼ������TIM1 4OC1
	TIM_OC1PreloadConfig(TIM12, TIM_OCPreload_Enable); 				//ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���

 
 	TIM_OC2Init(TIM12, &TIM_OCInitStructure);			//����Tָ���Ĳ�����ʼ������TIM1 4OC1
 	TIM_OC2PreloadConfig(TIM12, TIM_OCPreload_Enable);  //ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���

  	TIM_ARRPreloadConfig(TIM12,ENABLE);		//ARPEʹ�� 
  	
	TIM_SetCompare1(TIM12,(arr>>1));		//�޸ıȽ�ֵ���޸�ռ�ձ�
	TIM_SetCompare2(TIM12,(arr>>1));		//�޸ıȽ�ֵ���޸�ռ�ձ�
	
	TIM_Cmd(TIM12, DISABLE);  				//ʹ��TIM12
 
										  
}  
void TIM12_PWM_Stop(void)
{
	TIM_Cmd(TIM12, DISABLE);  				//ʹ��TIM12
}



//TIM9 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM9_PWM_Init(u32 arr,u32 psc)			//a��
{		 					 
	//�˲������ֶ��޸�IO������
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM_Cmd(TIM9, DISABLE);  			//ʹ��TIM9
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM9,ENABLE);  	//TIM9ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 	//ʹ��PORTFʱ��	
	
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource5,GPIO_AF_TIM9); 	//GPIOF9����Ϊ��ʱ��9
	GPIO_PinAFConfig(GPIOE,GPIO_PinSource6,GPIO_AF_TIM9); 	//GPIOF9����Ϊ��ʱ��9
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_6;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;        //����				//180612 test
	GPIO_Init(GPIOE,&GPIO_InitStructure);              	//��ʼ��PF9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  					//��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 	//���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   					//�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM9,&TIM_TimeBaseStructure);				//��ʼ����ʱ��9
	
	//��ʼ��TIM14 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 					//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;	 	//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 			//�������:TIM����Ƚϼ��Ե�
	TIM_OC1Init(TIM9, &TIM_OCInitStructure);  							//����Tָ���Ĳ�����ʼ������TIM1 4OC1
	TIM_OC1PreloadConfig(TIM9, TIM_OCPreload_Enable);  					//ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���

 
 	TIM_OC2Init(TIM9, &TIM_OCInitStructure);			//����Tָ���Ĳ�����ʼ������TIM1 4OC1
 	TIM_OC2PreloadConfig(TIM9, TIM_OCPreload_Enable);  	//ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���

  	TIM_ARRPreloadConfig(TIM9,ENABLE);	//ARPEʹ�� 

	TIM_SetCompare1(TIM9,(arr>>1));		//�޸ıȽ�ֵ���޸�ռ�ձ�
	TIM_SetCompare2(TIM9,(arr>>1));		//�޸ıȽ�ֵ���޸�ռ�ձ�
	
	TIM_Cmd(TIM9, DISABLE);  			//ʹ��TIM9
 
										  
}  

void TIM9_PWM_Stop(void)
{
	TIM_Cmd(TIM9, DISABLE);  			//ʹ��TIM9
}


//TIM3 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM3_PWM_Init(u32 arr,u32 psc)		//X��
{		 					 
	//�˲������ֶ��޸�IO������
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM_Cmd(TIM3, DISABLE); 
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  	//TIM3ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//ʹ��PORTFʱ��	
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_TIM3); //GPIOF9����Ϊ��ʱ��14
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;        //����
	GPIO_Init(GPIOA,&GPIO_InitStructure);              	//��ʼ��PF9

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//ʹ��PORTFʱ��	
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource5,GPIO_AF_TIM3); //GPIOF9����Ϊ��ʱ��14
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;        //����
	GPIO_Init(GPIOB,&GPIO_InitStructure);              	//��ʼ��PF9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   //�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);//��ʼ����ʱ��14
	
	//��ʼ��TIM14 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 		//�������:TIM����Ƚϼ��Ե�
	TIM_OC1Init(TIM3, &TIM_OCInitStructure); 				 		//����Tָ���Ĳ�����ʼ������TIM1 4OC1
	TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable); 				//ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���

 
 	TIM_OC2Init(TIM3, &TIM_OCInitStructure);			//����Tָ���Ĳ�����ʼ������TIM1 4OC1
 	TIM_OC2PreloadConfig(TIM3, TIM_OCPreload_Enable);  //ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���

  	TIM_ARRPreloadConfig(TIM3,ENABLE);		//ARPEʹ�� 
  	
	TIM_SetCompare1(TIM3,(arr>>1));		//�޸ıȽ�ֵ���޸�ռ�ձ�
	TIM_SetCompare2(TIM3,(arr>>1));		//�޸ıȽ�ֵ���޸�ռ�ձ�
	
	TIM_Cmd(TIM3, DISABLE);  				//ʹ��TIM12
										  
}  

void TIM3_PWM_Stop(void)
{
	TIM_Cmd(TIM3, DISABLE);  			//ʹ��TIM9
}



//TIM4 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM4_PWM_Init(u32 arr,u32 psc)			//z��
{		 					 
	//�˲������ֶ��޸�IO������
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM_Cmd(TIM4, DISABLE);  				//ʹ��TIM12
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);  	//TIM3ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE); 	//ʹ��PORTFʱ��	
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_TIM4); //GPIOF9����Ϊ��ʱ��14
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_TIM4); //GPIOF9����Ϊ��ʱ��14
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;        //����
	GPIO_Init(GPIOB,&GPIO_InitStructure);              	//��ʼ��PF9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  			//��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   			//�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);//��ʼ����ʱ��14
	
	//��ʼ��TIM14 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 		//�������:TIM����Ƚϼ��Ե�
	TIM_OC3Init(TIM4, &TIM_OCInitStructure); 				 		//����Tָ���Ĳ�����ʼ������TIM1 4OC1
	TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable); 				//ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���

 
 	TIM_OC4Init(TIM4, &TIM_OCInitStructure);			//����Tָ���Ĳ�����ʼ������TIM1 4OC1
 	TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable);  //ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���

  	TIM_ARRPreloadConfig(TIM4,ENABLE);		//ARPEʹ�� 
  	
	TIM_SetCompare3(TIM4,(arr>>1));		//�޸ıȽ�ֵ���޸�ռ�ձ�
	TIM_SetCompare4(TIM4,(arr>>1));		//�޸ıȽ�ֵ���޸�ռ�ձ�
//	TIM_SetCompare3(TIM4,150); 	//�޸ıȽ�ֵ���޸�ռ�ձ�
//	TIM_SetCompare4(TIM4,150); 	//�޸ıȽ�ֵ���޸�ռ�ձ�
	
	TIM_Cmd(TIM4, DISABLE);  				//ʹ��TIM12
										  
}  


void TIM4_PWM_Stop(void)
{
	TIM_Cmd(TIM4, DISABLE);  			//ʹ��TIM9
}




//TIM4 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM2_PWM_Init(u32 arr,u32 psc)			//Y
{		 					 
	//�˲������ֶ��޸�IO������
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	TIM_Cmd(TIM2, DISABLE);  				//ʹ��TIM2
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);  	//TIM2ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE); 	//ʹ��PORTFʱ��	
	
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_TIM2); //GPIOF9����Ϊ��ʱ��14
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_TIM2); //GPIOF9����Ϊ��ʱ��14
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_3;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;        //����
	GPIO_Init(GPIOA,&GPIO_InitStructure);              	//��ʼ��PF9
 
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  			//��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   			//�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);//��ʼ����ʱ��14
	
	//��ʼ��TIM14 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 		//�������:TIM����Ƚϼ��Ե�
	TIM_OC4Init(TIM2, &TIM_OCInitStructure); 				 		//����Tָ���Ĳ�����ʼ������TIM1 4OC1
	TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable); 				//ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���

 
 	TIM_OC1Init(TIM2, &TIM_OCInitStructure);			//����Tָ���Ĳ�����ʼ������TIM1 4OC1
 	TIM_OC1PreloadConfig(TIM2, TIM_OCPreload_Enable);  //ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���

  	TIM_ARRPreloadConfig(TIM2,ENABLE);		//ARPEʹ�� 
  	
	TIM_SetCompare1(TIM2,(arr>>1));		//�޸ıȽ�ֵ���޸�ռ�ձ�
	TIM_SetCompare4(TIM2,(arr>>1));		//�޸ıȽ�ֵ���޸�ռ�ձ�
	
	TIM_Cmd(TIM2, DISABLE);  				//ʹ��TIM12
										  
}  


void TIM2_PWM_Stop(void)
{
	TIM_Cmd(TIM2, DISABLE);  			//ʹ��TIM9
}

u16 xAxisClkSet(u16 xAxisClk)	// �޸ģ����ڼ�¼����  byYJY
{
	u16 nPDTemp;
	nPDTemp = 300000UL/xAxisClk;
	TIM3_PWM_Init(nPDTemp-1,280-1);			//84M/2800=30khz�ļ���Ƶ��,��װ��ֵ1000������PWMƵ��Ϊ 30k/1000=30hz.
	return xAxisClk;
}

u16 yAxisClkSet(u16 yAxisClk)	// �޸ģ����ڼ�¼����  byYJY
{
	u16 nPDTemp;
	nPDTemp = 300000UL/yAxisClk;
	TIM2_PWM_Init(nPDTemp-1,280-1);			//84M/2800=30khz�ļ���Ƶ��,��װ��ֵ1000������PWMƵ��Ϊ 30k/1000=30hz.
	return yAxisClk;	
}


u16 zAxisClkSet(u16 zAxisClk) // �޸ģ����ڼ�¼����  byYJY
{
	u16 nPDTemp;
	nPDTemp = 300000UL/zAxisClk;
	TIM4_PWM_Init(nPDTemp-1,280-1);			//84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ500������PWMƵ��Ϊ 1M/500=2Khz. 	
//	TIM4_PWM_Init(300-1,84-1);			//84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ500������PWMƵ��Ϊ 1M/500=2Khz. 	
	return zAxisClk;
}

void aAxisClkSet(u16 aAxisClk)
{
	u16 nPDTemp;
	nPDTemp = 600000UL/aAxisClk;
	TIM9_PWM_Init(nPDTemp-1,280-1);			//42M/2800=15khz�ļ���Ƶ��,��װ��ֵ1000������PWMƵ��Ϊ 30k/1000=30hz. 
}

void bAxisClkSet(u16 bAxisClk)
{
	u16 nPDTemp;
	nPDTemp = 300000UL/bAxisClk;
	TIM12_PWM_Init(nPDTemp-1,280-1);			//84M/84=1Mhz�ļ���Ƶ��,��װ��ֵ500������PWMƵ��Ϊ 1M/500=2Khz. 	
}


/////////////////////////////////////// ����������	/////////////////////////////////////
//TIM14 PWM���ֳ�ʼ�� 
//PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM14_PWM_Init(u32 arr,u32 psc)
{		 					 
	//�˲������ֶ��޸�IO������
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;

	TIM_Cmd(TIM14, DISABLE);  

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);  	//TIM14ʱ��ʹ��    
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); 	//ʹ��PORTFʱ��	
	
	GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,GPIO_AF_TIM14); //GPIOF9����Ϊ��ʱ��14
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;           //GPIOF9
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//�ٶ�100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //����
	GPIO_Init(GPIOF,&GPIO_InitStructure);              	//��ʼ��PF9
	  
	TIM_TimeBaseStructure.TIM_Prescaler=psc;  			//��ʱ����Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	TIM_TimeBaseStructure.TIM_Period=arr;   			//�Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM14,&TIM_TimeBaseStructure);		//��ʼ����ʱ��14
	
	//��ʼ��TIM14 Channel1 PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 				//ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 	//�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; 		//�������:TIM����Ƚϼ��Ե�   //TIM_OCPolarity_Low
	TIM_OC1Init(TIM14, &TIM_OCInitStructure);  						//����Tָ���Ĳ�����ʼ������TIM1 4OC1

	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);  			//ʹ��TIM14��CCR1�ϵ�Ԥװ�ؼĴ���
 
  	TIM_ARRPreloadConfig(TIM14,ENABLE);								//ARPEʹ�� 
	
	TIM_Cmd(TIM14, DISABLE);  										//ʹ��TIM14
 
										  
}  

void mainMotorDecode(void)
{
	TIM14_PWM_Init(200-1,8400-1); 		//84M/8400=10Khz�ļ���Ƶ��,��װ��ֵ200������PWMƵ��Ϊ 10Khz/200=50hz.   
	TIM_Cmd(TIM14, ENABLE);  			//ʹ��TIM14
	TIM_SetCompare1(TIM14,20);			//�޸ıȽ�ֵ���޸�ռ�ձ� 10%
	delay_ms(2000);	 

	TIM_SetCompare1(TIM14,10);			//�޸ıȽ�ֵ���޸�ռ�ձ� 10%
	delay_ms(1000);	 
	
	TIM_Cmd(TIM14, DISABLE);  										
}

void mainMotorWork(void)
{
	TIM14_PWM_Init(200-1,8400-1); 		//84M/8400=10Khz�ļ���Ƶ��,��װ��ֵ200������PWMƵ��Ϊ 10Khz/200=50hz.   
	TIM_Cmd(TIM14, ENABLE);  			//ʹ��TIM14
	TIM_SetCompare1(TIM14,24);			//�޸ıȽ�ֵ���޸�ռ�ձ� 12%
}

void mainMotorStop(void)
{
	TIM_Cmd(TIM14, DISABLE);  										
}

