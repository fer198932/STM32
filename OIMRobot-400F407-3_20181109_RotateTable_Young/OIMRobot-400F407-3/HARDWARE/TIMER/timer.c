/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          ԭ���������������
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : Timer.C
* By  : Hiker Lee
* 2018-05-03
********************************************************************************************************/

#include "timer.h"
#include "led.h"
#include "lwip_comm.h"
#include "SysInit.h"
#include "ChangeTools.h"		// �����˻��������Ľṹ��

// �߾��Ȼ�ת̨������
extern u8 ii;
extern u8 addSubCnt;				// ��Ϊ4��
extern u16 clkTemp;					// ϸ����8ʱ��Ƶ��6666��ӦF1000��250(r/min)
extern u8 rotateTableFlag;
extern u8 nAxis;

extern unsigned char recvFrameFlag;
extern unsigned char recvFrameState;

extern u8 addSubStepSum;
extern u8 addSubStepNo;
extern u8 addSubFlag;			//0:��Ҫ��ʱ�����üӼ��٣�1:����Ҫ��
extern u32 workTimeMSSum;
extern u32 workedTimeMS;

extern u32 xAxisPlusNum,yAxisPlusNum,zAxisPlusNum,aAxisPlusNum,bAxisPlusNum;
extern u8 xAxisStepFlag; 
extern u8 yAxisStepFlag; 
extern u8 zAxisStepFlag; 
extern u8 aAxisStepFlag; 
extern u8 bAxisStepFlag; 

u16 temp1,temp2,temp3,temp4,temp5;
u16 testTemp[12][5];
u8 i=0;

extern u32 pwmNumTim12;
extern u32 pwmNumTim9;
extern u32 pwmNumTim3;
extern u32 pwmNumTim4;
extern u32 pwmNumTim2;

extern u8 dataPreProcFlag;		//0:�����Ԥ�������ڶ�������1:�ȴ���һ֡���ݵ�Ԥ����
extern u32 maxPlusNum;

extern u8 temCollectionFlag;

// ������������������
extern ChangeToolsSavedSteps stepsTemp;
extern u8 stepsFlag[5];

extern u8 addtime;
extern u32 CountNeedAdd;
extern u32 xAxisPlusNum,yAxisPlusNum,zAxisPlusNum,aAxisPlusNum,bAxisPlusNum;
extern u32 maxPlusNum;
u32 testNum;
void testIO_Init(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);		//ʹ��GPIOFʱ��
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;				//��ͨ����ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;			//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;		//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;				//����
  GPIO_Init(GPIOC, &GPIO_InitStructure);					//��ʼ��GPIOF0,1,2,3,4

} 

//ͨ�ö�ʱ��6�жϳ�ʼ��
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//��ʱ�����ʱ����㷽��:Tout=((arr+1)*(psc+1))/Ft us.
//Ft=��ʱ������Ƶ��,��λ:Mhz
//����ʹ�õ��Ƕ�ʱ��6!

void TIM6_Base_Init(u16 arr,u16 psc)
{
	  TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	  NVIC_InitTypeDef NVIC_InitStructure;
	  
	  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);  			///ʹ��TIM6ʱ��
	  
	  TIM_TimeBaseInitStructure.TIM_Period = arr;   				//�Զ���װ��ֵ
	  TIM_TimeBaseInitStructure.TIM_Prescaler=psc;					//��ʱ����Ƶ
	  TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
	  TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	  
	  TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStructure);			//��ʼ��TIM6
	  
	  TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE); 						//����ʱ��6�����ж�
	  
	  NVIC_InitStructure.NVIC_IRQChannel=TIM6_DAC_IRQn; 			//��ʱ��6�ж�
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 	//��ռ���ȼ�1
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; 			//�����ȼ�3
	  NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	  NVIC_Init(&NVIC_InitStructure);

	  TIM_Cmd(TIM6,DISABLE); 										//ʹ�ܶ�ʱ��3
}

void TIM6_DAC_IRQHandler(void)    						//  TIM6 �ж�
{
	if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) 		// ���ָ����TIM6�жϷ������
		{
			TIM_ClearITPendingBit(TIM6, TIM_IT_Update  );  	//����ж�
			recvFrameFlag = 0;								// ���ڽ��������־
			recvFrameState = 0;

		/*	xAxisStepFlag = 2;								//5��δ��ɶ�������������������
			yAxisStepFlag = 2;
			zAxisStepFlag = 2;
			aAxisStepFlag = 2;
			bAxisStepFlag = 2;
		//*/
		
			dataPreProcFlag = 1; 	//0:�����Ԥ�������ڶ�������1:�ȴ���һ֡���ݵ�Ԥ����

			test_REV;
		}
}

void TIM7_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7,ENABLE);  			///ʹ��TIM7ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  					//��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 	//���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_Period=arr;   					//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM7,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM7,TIM_IT_Update,ENABLE); 						//����ʱ��7�����ж�
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM7_IRQn; 					//��ʱ��7�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; 		//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; 			//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM7,DISABLE); 											//ʹ�ܶ�ʱ��7
}

//��ʱ��7�жϷ�����
void TIM7_IRQHandler(void)						//40ms
{
	if(TIM_GetITStatus(TIM7,TIM_IT_Update)==SET) 		//����ж�
	{
		TIM_ClearITPendingBit(TIM7,TIM_IT_Update);			//����жϱ�־λ
		
		if(1 == rotateTableFlag)
		{
			switch(nAxis)
			{
				case 0:																			// X��
					// ֻ���X��
					if(ii < (addSubCnt-1))
					{
						ii++;
						xAxisClkSet(clkTemp>>(addSubCnt-ii-1));
					} else 
					{
//						xAxisClkSet(clkTemp);
					}
				break;
				case 1:																			// Y��
					// ֻ���Y��
					if(ii < (addSubCnt-1))
					{
						ii++;
						yAxisClkSet(clkTemp>>(addSubCnt-ii-1));
					} else 
					{
//						yAxisClkSet(clkTemp);
					}
				break;
				case 2:																			// Z��
					// ֻ���Z��
					if(ii < (addSubCnt-1))
					{
						ii++;
						zAxisClkSet(clkTemp>>(addSubCnt-ii-1));
					} else 
					{
//						zAxisClkSet(clkTemp);
					}
				break;
				default:
				break;
			}
			motor5workstart();
		}	else 
		{
	
		if(workedTimeMS == 0)
			{
				i=0;
				memset(testTemp, 0, sizeof(u16)*12*5);
			}
		if(workedTimeMS < (0xffffffff-addtime))
			workedTimeMS = workedTimeMS + addtime;
		
		if(workedTimeMS < (addSubStepSum+3)*addtime)
		{
			if((addSubFlag == 0)&&(addSubStepNo<addSubStepSum))		//addSubStepNo = addSubStepSum;
				{
					addSubStepNo = addSubStepNo + 1; 
				/*	
					if(addSubStepNo == addSubStepSum)
					{
						temp1 = 0;	//just test
					}
					//*/
					speedReSet(addSubStepSum,addSubStepNo);
					
					//////////////////////////////////
				/*	
					temp1 = pwmNumTim3;
					temp2 = pwmNumTim2;
					temp3 = pwmNumTim4;
					temp4 = pwmNumTim9;
					temp5 = pwmNumTim12;
				
					testTemp[i][0] = temp1;
					testTemp[i][1] = temp2;
					testTemp[i][2] = temp3;
					testTemp[i][3] = temp4;
					testTemp[i][4] = temp5;

					i++;
				//*/	
					motor5workstart();
						
					//////////////////////////////////
					
				}
		}
//		else if(workedTimeMS > (workTimeMSSum - 500))
//		{
//		/*
//			temp1 = pwmNumTim3;
//			temp2 = pwmNumTim2;
//			temp3 = pwmNumTim4;
//			temp4 = pwmNumTim9;
//			temp5 = pwmNumTim12;
//		//*/

//			if((pwmNumTim3 > maxPlusNum-315)||(pwmNumTim2 > maxPlusNum-315)||(pwmNumTim4 > maxPlusNum-315)||(pwmNumTim9 > maxPlusNum-315)||(pwmNumTim12 > maxPlusNum-315))
//			{
//					if((addSubFlag == 0)&&(addSubStepNo>1)) 			//addSubStepNo = 1;
//						{
//							addSubStepNo--; 
//							speedReSet(addSubStepSum,addSubStepNo);
//							
//							//////////////////////////////////
//						/*	
//							testTemp[i][0] = temp1;
//							testTemp[i][1] = temp2;
//							testTemp[i][2] = temp3;
//							testTemp[i][3] = temp4;
//							testTemp[i][4] = temp5;
//				
//							i++;
//						//*/	
//							motor5workstart();
//								
//							//////////////////////////////////
//							
//				//			if(addSubStepNo==1)
//				//				TIM_Cmd(TIM7,DISABLE);			//			���ڵ���Ӽ��ٴ���			
//						}

//			}
//		}
		else
		{
				testNum=0;
				if(xAxisPlusNum==maxPlusNum)
					testNum=pwmNumTim3;
				else if(yAxisPlusNum==maxPlusNum)
					testNum=pwmNumTim2;
				else if(zAxisPlusNum==maxPlusNum)
					testNum=pwmNumTim4;
				else if(aAxisPlusNum==maxPlusNum)
					testNum=pwmNumTim9;
				else if(bAxisPlusNum==maxPlusNum)
					testNum=pwmNumTim12;
				
				if(testNum>=maxPlusNum-CountNeedAdd)
				{
					if((addSubFlag == 0)&&(addSubStepNo>1)) 			//addSubStepNo = 1;
					{
						addSubStepNo--; 
						speedReSet(addSubStepSum,addSubStepNo);
						motor5workstart();
					}	
				}
		}
	//	test_REV;
	}
}
}


void TIM14_Int_Init(u16 arr,u16 psc)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);  			///ʹ��TIM7ʱ��
	
	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  					//��ʱ����Ƶ
	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; 	//���ϼ���ģʽ
	TIM_TimeBaseInitStructure.TIM_Period=arr;   					//�Զ���װ��ֵ
	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	
	TIM_TimeBaseInit(TIM14,&TIM_TimeBaseInitStructure);
	
	TIM_ITConfig(TIM14,TIM_IT_Update,ENABLE); 						//����ʱ��7�����ж�
	
	NVIC_InitStructure.NVIC_IRQChannel=TIM8_TRG_COM_TIM14_IRQn; 					//��ʱ��7�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x03; 		//��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x02; 			//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	TIM_Cmd(TIM14,DISABLE); 											//ʹ�ܶ�ʱ��7
}

void TIM8_TRG_COM_TIM14_IRQHandler(void)    						//  TIM14 �ж�
{
	if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) 		// ���ָ����TIM14�жϷ������
		{
			TIM_ClearITPendingBit(TIM14,TIM_IT_Update);			//����жϱ�־λ
//			if(stepsFlag[0])
//				stepsTemp.xSteps[0]++;														// x�����
//			if(stepsFlag[1])
//				stepsTemp.ySteps[0]++;														// y�����
//			if(stepsFlag[2])
//				stepsTemp.zSteps[0]++;														// z�����
		}
}

/* ԭ�¶Ȳɼ��жϣ�ע�͵�
void TIM8_TRG_COM_TIM14_IRQHandler(void)    						//  TIM14 �ж�
{
	if (TIM_GetITStatus(TIM14, TIM_IT_Update) != RESET) 		// ���ָ����TIM14�жϷ������
		{
			TIM_ClearITPendingBit(TIM14,TIM_IT_Update);			//����жϱ�־λ
			temCollectionFlag = 1;
			test_REV;
		}
}
*/

