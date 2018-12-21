/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          ԭ���������������
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : Key.C
* By  : Hiker Lee
* 2018-05-02
********************************************************************************************************/

#include "key.h"
#include "delay.h"
#include "SysInit.h"

extern unsigned char backResString[20];

extern u8 xAxisStepFlag; 
extern u8 yAxisStepFlag; 
extern u8 zAxisStepFlag; 
extern u8 aAxisStepFlag; 
extern u8 bAxisStepFlag; 

u8 bAxis2ZeroFlag=0; 

u32 keyValue=0;
u32 keyValueOld=0;


//������ʼ������
void KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF|RCC_AHB1Periph_GPIOG,ENABLE);  	//ʹ��GPIOA GPIOEʱ��
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_10|GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;  				//5�Ṳ8����������
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;  			//����
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;  			//��������
	GPIO_Init(GPIOF,&GPIO_InitStructure); 					//��ʼ��GPIOE
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4;  	//��1-4���ѵ������λ
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;  			//����
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;  			//��������
	GPIO_Init(GPIOG,&GPIO_InitStructure);     				//��ʼ��GPIOG

	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11|GPIO_Pin_12;  	//��1-4���ѵ������λ
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;  			//����
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;  			//��������
	GPIO_Init(GPIOA,&GPIO_InitStructure);     				//��ʼ��GPIOA
}

//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����
//3��KEY2���� 
//4��WKUP���� WK_UP
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>WK_UP!!

void KEY_Scan5axis(u8 mode)
{
	static u8 key_up=1;   				//�����ɿ���־
	if(mode) key_up=1;  				//֧������
	if((KEYXa==0||KEYXb==0||KEYYa==0||KEYYb==0||KEYZa==0||KEYZb==0||KEYA==0||KEYB==0)) //�а�������
	{
		delay_us(10);  //����ȥ��
	//	key_up=0;
	
		keyValue = 0;
	
		if(KEYXa==0)  keyValue=keyValue | 1;
		if(KEYXb==0)  keyValue=keyValue | 2;
		if(KEYYa==0)  keyValue=keyValue | 4;
		if(KEYYb==0)  keyValue=keyValue | 8;
		if(KEYZa==0)  keyValue=keyValue | 16;
		if(KEYZb==0)  keyValue=keyValue | 32;
		if(KEYA==0)   keyValue=keyValue | 64;
		if(KEYB==0)   keyValue=keyValue | 128;
	//	return keyValue;
	}
	else if(KEYXa==1&&KEYXb==1&&KEYYa==1&&KEYYb==1&&KEYZa==1&&KEYZb==1&&KEYA==1&&KEYB==1)
		keyValue = 0;
	//	key_up=1; 
//	return 0; 		//�ް�������
}


void key_Proc5axisComm(void *pdata)
{
	u8 i; 
	
	KEY_Scan5axis(0);
	if(keyValue !=keyValueOld)
	{
		keyValueOld = keyValue;
	
		if(keyValue != 0)
		{
			backResString[0] = FrameB1;
			backResString[1] = FrameB2;
			backResString[2] = STATUSINFO;
			
			backResString[3] = 9;
			backResString[4] = 0x00;
			
			backResString[8] = FrameEnd;
			
			xAxisStepFlag = 2;
			yAxisStepFlag = 2;
			zAxisStepFlag = 2;
			aAxisStepFlag = 2;
			bAxisStepFlag = 2;
		}
	
		if(keyValue&0x0001)	//KEYXa_PRES
		{
			TIM_Cmd(TIM3, DISABLE); 	//Xa��λ
			backResString[5] = 0x31;
			backResString[6] = 0x00;
			backResString[7] = 0x01;

			for(i=0;i<9;i++)
				{
					USART_SendData(USART3,backResString[i]); 
					while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//�ȴ����ͽ���
				}
		}
		if((keyValue>>1)&0x0001)	//KEYXb_PRES
		{
			TIM_Cmd(TIM3, DISABLE); 	//Xb��λ
			backResString[5] = 0x31;
			backResString[6] = 0x00;
			backResString[7] = 0x02;

			for(i=0;i<9;i++)
				{
					USART_SendData(USART3,backResString[i]); 
					while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//�ȴ����ͽ���
				}
		}
		if((keyValue>>2)&0x0001)	//KEYYa_PRES
		{
			TIM_Cmd(TIM2, DISABLE);		//Ya��λ
			backResString[5] = 0x32;
			backResString[6] = 0x00;
			backResString[7] = 0x01;

			for(i=0;i<9;i++)
				{
					USART_SendData(USART3,backResString[i]); 
					while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//�ȴ����ͽ���
				}
		}
	
		if((keyValue>>3)&0x0001)	//KEYYb_PRES
		{
			TIM_Cmd(TIM2, DISABLE);		//Yb��λ
			backResString[5] = 0x32;
			backResString[6] = 0x00;
			backResString[7] = 0x02;

			for(i=0;i<9;i++)
				{
					USART_SendData(USART3,backResString[i]); 
					while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//�ȴ����ͽ���
				}
		}
	
		if((keyValue>>4)&0x0001)	//KEYZa_PRES
		{
			TIM_Cmd(TIM4, DISABLE); 	//Za��λ
			backResString[5] = 0x33;
			backResString[6] = 0x00;
			backResString[7] = 0x01;

			for(i=0;i<9;i++)
				{
					USART_SendData(USART3,backResString[i]); 
					while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//�ȴ����ͽ���
				}
		}

		if((keyValue>>5)&0x0001)	//KEYZb_PRES
		{
			TIM_Cmd(TIM4, DISABLE); 	//Zb��λ
			backResString[5] = 0x33;
			backResString[6] = 0x00;
			backResString[7] = 0x02;

			for(i=0;i<9;i++)
				{
					USART_SendData(USART3,backResString[i]); 
					while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//�ȴ����ͽ���
				}
		}
	
		if((keyValue>>6)&0x0001)	//KEYA_PRES
		{
		//	TIM_Cmd(TIM9, DISABLE); 	//A��λ
			backResString[5] = 0x35;
			backResString[6] = 0x00;
			backResString[7] = 0x03;

			for(i=0;i<9;i++)
				{
					USART_SendData(USART3,backResString[i]); 
					while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//�ȴ����ͽ���
				}
		}

		if((keyValue>>7)&0x0001)	//KEYB_PRES
		{
			if(bAxis2ZeroFlag == 0)
			{
				TIM_Cmd(TIM12, DISABLE);	//B��λ
			}
			backResString[5] = 0x36;
			backResString[6] = 0x00;
			backResString[7] = 0x03;

			for(i=0;i<9;i++)
				{
					USART_SendData(USART3,backResString[i]); 
					while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//�ȴ����ͽ���
				}
		}
	}
}
