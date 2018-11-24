#ifndef _SysInit_H
#define _SysInit_H




//----------------------------------------------------------------------------
/*********����ͨ��Э��**********/
#define FrameB1 		0x0E     //����֡ͷ
#define FrameB2			0x09     //����֡ͷ
#define FrameEnd		0xFF     //����֡β
//---------------------------------------------------------------------------
/*********����ͨ������֡������**********/
#define SELFCHECK 		0x0B     //�Լ�
#define CONTROLINFO		0x0C     //
#define DATAINFO		0x0D     //
#define STATUSINFO		0x0E     //

//---------------------------------------------------------------------------

#define xAxisDIR_OFF 	GPIO_SetBits(GPIOF,GPIO_Pin_0)
#define xAxisDIR_ON 	GPIO_ResetBits(GPIOF,GPIO_Pin_0)
#define yAxisDIR_OFF 	GPIO_SetBits(GPIOF,GPIO_Pin_1)
#define yAxisDIR_ON 	GPIO_ResetBits(GPIOF,GPIO_Pin_1)
#define zAxisDIR_OFF 	GPIO_SetBits(GPIOF,GPIO_Pin_2)
#define zAxisDIR_ON 	GPIO_ResetBits(GPIOF,GPIO_Pin_2)
#define aAxisDIR_OFF 	GPIO_SetBits(GPIOF,GPIO_Pin_3)
#define aAxisDIR_ON 	GPIO_ResetBits(GPIOF,GPIO_Pin_3)
#define bAxisDIR_OFF 	GPIO_SetBits(GPIOF,GPIO_Pin_4)
#define bAxisDIR_ON 	GPIO_ResetBits(GPIOF,GPIO_Pin_4)

#define xAxisPlus1_LOW 		GPIO_SetBits(GPIOA,GPIO_Pin_6)
#define xAxisPlus1_HIGH 	GPIO_ResetBits(GPIOA,GPIO_Pin_6)
#define xAxisPlus2_LOW 		GPIO_SetBits(GPIOA,GPIO_Pin_7)
#define xAxisPlus2_HIGH 	GPIO_ResetBits(GPIOA,GPIO_Pin_7)

#define yAxisPlus_LOW 		GPIO_SetBits(GPIOA,GPIO_Pin_5)
#define yAxisPlus_HIGH 		GPIO_ResetBits(GPIOA,GPIO_Pin_5)

#define zAxisPlus1_LOW 		GPIO_SetBits(GPIOB,GPIO_Pin_8)
#define zAxisPlus1_HIGH 	GPIO_ResetBits(GPIOB,GPIO_Pin_8)
#define zAxisPlus2_LOW 		GPIO_SetBits(GPIOB,GPIO_Pin_9)
#define zAxisPlus2_HIGH 	GPIO_ResetBits(GPIOB,GPIO_Pin_9)

#define aAxisPlus1_LOW 		GPIO_SetBits(GPIOE,GPIO_Pin_5)
#define aAxisPlus1_HIGH 	GPIO_ResetBits(GPIOE,GPIO_Pin_5)
#define aAxisPlus2_LOW 		GPIO_SetBits(GPIOE,GPIO_Pin_6)
#define aAxisPlus2_HIGH 	GPIO_ResetBits(GPIOE,GPIO_Pin_6)

#define bAxisPlus1_LOW 		GPIO_SetBits(GPIOB,GPIO_Pin_14)
#define bAxisPlus1_HIGH 	GPIO_ResetBits(GPIOB,GPIO_Pin_14)
#define bAxisPlus2_LOW 		GPIO_SetBits(GPIOB,GPIO_Pin_15)
#define bAxisPlus2_HIGH 	GPIO_ResetBits(GPIOB,GPIO_Pin_15)

#define xAxis_REV GPIO_WriteBit(GPIOF, GPIO_Pin_0,(BitAction)(1-(GPIO_ReadOutputDataBit(GPIOF, GPIO_Pin_0))))

#define test_REV GPIO_WriteBit(GPIOC, GPIO_Pin_3,(BitAction)(1-(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_3))))


#endif

