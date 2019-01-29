#include "usart.h"	
////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
// ���ڳ���
////////////////////////////////////////////////////////////////////////////////// 	  

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{ 	
	while((USART_X->SR&0X40)==0);//ѭ������,ֱ���������   
	USART_X->DR = (u8) ch;      
	return ch;
}
#endif

#if EN_USART1
	#define USART_X USART1
#elif EN_USART2
	#define USART_X USART2
#elif EN_USART3
	#define USART_X USART3
#else
#endif

static 	GPIO_Structure_XX 	GPIO_AF_Usart_Tx;						// GPIO����Ϊ���ڷ���
static 	GPIO_Structure_XX 	GPIO_AF_Usart_Rx;						// GPIO����Ϊ���ڽ���
static 	AF_Structure_XX 		USART_AF_Structure; 				// ���ڵĸ������

// ��ʼ��GPIO�ʹ��ڵ��������ݽṹ��
static void GPIO_AF_Usart_Init(USART_TypeDef* USARTx, 
	const char Pin_Tx_str[], const char Pin_Rx_str[])
{
	GPIO_AF_Usart_Tx.AF_Structure = &USART_AF_Structure; 			// GPIO�봮�ڵĸ���ӳ��
	GPIO_AF_Usart_Rx.AF_Structure = &USART_AF_Structure; 			// GPIO�봮�ڵĸ���ӳ��
	
	GPIO_Structure_Init(Pin_Tx_str, &GPIO_AF_Usart_Tx);
	GPIO_Structure_Init(Pin_Rx_str, &GPIO_AF_Usart_Rx);	
	
#if EN_USART1
	USART_AF_Structure.RCC_Periph_N = RCC_APB2Periph_USART1;
	USART_AF_Structure.GPIO_AF = GPIO_AF_USART1;
	USART_AF_Structure.IRQ_Channel_N = USART1_IRQn;	
#elif EN_USART2
	USART_AF_Structure.RCC_Periph_N = RCC_APB1Periph_USART2;
	USART_AF_Structure.GPIO_AF = GPIO_AF_USART2;
	USART_AF_Structure.IRQ_Channel_N = USART2_IRQn;
#elif EN_USART3
	USART_AF_Structure.RCC_Periph_N = RCC_APB1Periph_USART3;
	USART_AF_Structure.GPIO_AF = GPIO_AF_USART3;
	USART_AF_Structure.IRQ_Channel_N = USART3_IRQn;
#else
#endif
	USART_AF_Structure.IRQ_Type = USART_IT_IDLE; 					// ���ڿ����ж�	
}

//��ʼ��IO ����
//bound:������
void uart_init(u32 bound){
	// GPIO�����ڡ��ж�
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// ��ʼ���������ݽṹ��
	GPIO_AF_Usart_Init(USART_X, USART_TX, USART_RX);
	
	// ʱ��ʹ�� 1������1�봮��2��3��ʱ���߲�һ���� 2��GPIO����ͬһ���˿ڣ��ó��������
#if EN_USART1
	RCC_APB2PeriphClockCmd(USART_AF_Structure.RCC_Periph_N, ENABLE); 		// ʹ��USART1ʱ��
#else
	RCC_APB1PeriphClockCmd(USART_AF_Structure.RCC_Periph_N, ENABLE); 		// ʹ�ܴ���ʱ��
#endif
	RCC_AHB1PeriphClockCmd(GPIO_AF_Usart_Tx.RCC_Periph_N, ENABLE);  	// ʹ��GPIOʱ��
	
	// ���ڶ�Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIO_AF_Usart_Tx.GPIO_Port, 
	GPIO_AF_Usart_Tx.GPIO_PinSource_N, USART_AF_Structure.GPIO_AF);
	GPIO_PinAFConfig(GPIO_AF_Usart_Rx.GPIO_Port, 
	GPIO_AF_Usart_Rx.GPIO_PinSource_N, USART_AF_Structure.GPIO_AF);
	
	// GPIO����
	GPIO_InitStructure.GPIO_Pin = GPIO_AF_Usart_Tx.GPIO_Pin_N | GPIO_AF_Usart_Rx.GPIO_Pin_N;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 			// ���ù���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	// �ٶ�50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 		// ���츴�����
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 			// ����
	GPIO_Init(GPIO_AF_Usart_Tx.GPIO_Port, &GPIO_InitStructure); 	// ��ʼ��
	
	// ���ڳ�ʼ��
	USART_InitStructure.USART_BaudRate = bound; 		// ����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 	// �ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	// һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No; 		// ����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 	// ��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	 	// �շ�ģʽ
  USART_Init(USART_X, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART_X, ENABLE);  // ʹ�ܴ���
	
	//USART_ClearFlag(USART_X, USART_FLAG_TC);	

	USART_ITConfig(USART_X, USART_AF_Structure.IRQ_Type, ENABLE); 	// ��������ж� 

	// Usart NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART_AF_Structure.IRQ_Channel_N;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����	
	
////////// ���ӹرմ����жϵĺ����������ʼ��δ��ɾʹ��  byYJY  /////////

}


#if EN_USART1
void USART1_IRQHandler(void)                	//����1�жϷ������
{
//	volatile static u16 endPosOld = 0; 					// endָ���ǰһ��λ�ã��ж��Ƿ���
	volatile u8 temp;												// ���־λ��
#if SYSTEM_SUPPORT_OS 		/*���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS. */		
	OSIntEnter();    	
#endif	
	
	USART_IRQ;
	
//	if(USART_GetITStatus(USART_X, USART_IT_IDLE) != RESET)  /* �����ж� */ \
//	{ \
//		temp = USART_X->SR;    /* ͨ����SR(״̬�Ĵ���)��DR(���ݼĴ���)������ж� */   \
//		temp = USART_X->DR;		\
//		/* DMA���շ�ʽ */ 		\
//		/* endָ���λ��=���õĽ��ճ���-ʣ���DMA�����С ��ʼ�յ��ڣ� */
//		buffer_Rec.end = BUF_SIZE - DMA_GetCurrDataCounter(DMA_Stream_Rx);		
//		if(buffer_Rec.start <= buffer_Rec.end)  /* ���� */
//		{
//			/* ���ʣ��Ŀռ�С��RESERVED_SIZE���ر�DMA ��ע����������֮��Ӧ���¿�����  */
//			if((BUF_SIZE + buffer_Rec.start - buffer_Rec.end) < RESERVED_SIZE)
//			{
//				DMA_Cmd(DMA_Stream_Rx, DISABLE);
//				respMsgError("DMA���\r\n", 1);
//			}
//		}
//		else 																		/* ���� */
//		{
//			/* ���ʣ��Ŀռ�С��RESERVED_SIZE���ر�DMA ��ע����������֮��Ӧ���¿�����  */
//			if((buffer_Rec.start - buffer_Rec.end) < RESERVED_SIZE)
//			{
//				DMA_Cmd(DMA_Stream_Rx, DISABLE);
//				respMsgError("DMA���\r\n", 1);
//			}
//		}

//		
////		if(endPosOld < buffer_Rec.end) 		/* endû�������� */ 
////			
////		
////		/* ���յ��ַ�������=���õĽ��ճ���-ʣ���DMA�����С */ 	
////		buffer_Rec.end = BUF_SIZE - DMA_GetCurrDataCounter(DMA_Stream_Rx); 	
////		/* ʣ���DMA�����С  */
//////		length = DMA_GetCurrDataCounter(DMA_Stream_Rx);
////		if(buffer_Rec.bufStatus) /* ����start>end */
////		{
////			
////		}
////		else 										/* ����start<end */
////		{
//////			buffer_Rec.end += 
//////			if()
////		}
////		
////		
//////		mymemcpy((buffer_Rec.data+buffer_Rec.end), DMA_Rec, length); 		
//////		DMA_Stream_Rx->NDTR = BUF_SIZE;		/* ��0λ�ÿ�ʼ����װ�� */ 		
////		/* ���ô������ݳ��� */ 			
//////		DMA_SetCurrDataCounter(DMA_Stream_Rx, BUF_SIZE); 			
//////		DMA_Cmd(DMA_Stream_Rx, ENABLE); 		
//	} 	 
	
#if SYSTEM_SUPPORT_OS 	/* ���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS. */ 
	OSIntExit();  											 
#endif 	
} 	

#elif EN_USART2
void USART2_IRQHandler(void)                	//����2�жϷ������
{
	volatile u8 temp;												// ���־λ��
	
#if SYSTEM_SUPPORT_OS 		/*���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS. */		
	OSIntEnter();    	
#endif	
	
	 USART_IRQ;
	
#if SYSTEM_SUPPORT_OS 	/* ���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS. */ 
	OSIntExit();  											 
#endif 	
} 

#elif EN_USART3
void USART3_IRQHandler(void)                	//����3�жϷ������
{
	volatile u8 temp;												// ���־λ��
	
#if SYSTEM_SUPPORT_OS 		/*���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS. */		
	OSIntEnter();    	
#endif	
	
	 USART_IRQ;
	
#if SYSTEM_SUPPORT_OS 	/* ���SYSTEM_SUPPORT_OSΪ�棬����Ҫ֧��OS. */ 
	OSIntExit();  											 
#endif 	
} 

#endif	



 



