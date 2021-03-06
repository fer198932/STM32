
#include "usart.h"	

////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif
//////////////////////////////////////////////////////////////////////////////////	 
// 串口程序
////////////////////////////////////////////////////////////////////////////////// 	  

//extern volatile 	u8 	USART_IDLE_Flag; 							// 串口空闲中断标志，串口数据处理 
extern	Flag_Structure 	flag_Struct;
extern volatile 	FlagStatus 	DMA_Out_Flag;									// DMA溢出，重新初始化

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
_sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{ 	
	while((USART_X->SR&0X40)==0);//循环发送,直到发送完毕   
	USART_X->DR = (u8) ch;      
	return ch;
}
#endif


static 	GPIO_Structure_XX 	GPIO_AF_Usart_Tx;						// GPIO复用为串口发送
static 	GPIO_Structure_XX 	GPIO_AF_Usart_Rx;						// GPIO复用为串口接收
static 	AF_Structure_XX 		USART_AF_Structure; 				// 串口的复用情况

// 初始化GPIO和串口的配置数据结构体
static void GPIO_AF_Usart_Init(USART_TypeDef* USARTx, 
	const char Pin_Tx_str[], const char Pin_Rx_str[])
{
	GPIO_AF_Usart_Tx.AF_Structure = &USART_AF_Structure; 			// GPIO与串口的复用映射
	GPIO_AF_Usart_Rx.AF_Structure = &USART_AF_Structure; 			// GPIO与串口的复用映射
	
	if(ERROR == GPIO_Structure_Make(Pin_Tx_str, &GPIO_AF_Usart_Tx))
		return;
	if(ERROR == GPIO_Structure_Make(Pin_Rx_str, &GPIO_AF_Usart_Rx))
		return;	
	
#if (1 == USART_EN)
	USART_AF_Structure.RCC_Periph_N = RCC_APB2Periph_USART1;
	USART_AF_Structure.GPIO_AF = GPIO_AF_USART1;
	USART_AF_Structure.IRQ_Channel_N = USART1_IRQn;	
#elif (2 == USART_EN)
	USART_AF_Structure.RCC_Periph_N = RCC_APB1Periph_USART2;
	USART_AF_Structure.GPIO_AF = GPIO_AF_USART2;
	USART_AF_Structure.IRQ_Channel_N = USART2_IRQn;
#elif (3 == USART_EN)
	USART_AF_Structure.RCC_Periph_N = RCC_APB1Periph_USART3;
	USART_AF_Structure.GPIO_AF = GPIO_AF_USART3;
	USART_AF_Structure.IRQ_Channel_N = USART3_IRQn;
#else
#endif
	USART_AF_Structure.IRQ_Type = USART_IT_IDLE; 					// 串口空闲中断	
}

//初始化IO 串口
//bound:波特率
void uart_init(u32 bound){
	// GPIO、串口、中断
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	// 初始化配置数据结构体
	GPIO_AF_Usart_Init(USART_X, USART_TX, USART_RX);
	
	// 时钟使能 1、串口1与串口2、3的时钟线不一样； 2、GPIO需在同一个端口（该程序的需求）
#if (1 == USART_EN)
	RCC_APB2PeriphClockCmd(USART_AF_Structure.RCC_Periph_N, ENABLE); 		// 使能USART1时钟
#else
	RCC_APB1PeriphClockCmd(USART_AF_Structure.RCC_Periph_N, ENABLE); 		// 使能串口时钟
#endif
	RCC_AHB1PeriphClockCmd(GPIO_AF_Usart_Tx.RCC_Periph_N, ENABLE);  	// 使能GPIO时钟
	
	// 串口对应引脚复用映射
	GPIO_PinAFConfig(GPIO_AF_Usart_Tx.GPIO_Port, 
	GPIO_AF_Usart_Tx.GPIO_PinSource_N, USART_AF_Structure.GPIO_AF);
	GPIO_PinAFConfig(GPIO_AF_Usart_Rx.GPIO_Port, 
	GPIO_AF_Usart_Rx.GPIO_PinSource_N, USART_AF_Structure.GPIO_AF);
	
	// GPIO配置
	GPIO_InitStructure.GPIO_Pin = GPIO_AF_Usart_Tx.GPIO_Pin_N | GPIO_AF_Usart_Rx.GPIO_Pin_N;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 			// 复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	// 速度50MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 		// 推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 			// 上拉
	GPIO_Init(GPIO_AF_Usart_Tx.GPIO_Port, &GPIO_InitStructure); 	// 初始化
	
	// 串口初始化
	USART_InitStructure.USART_BaudRate = bound; 		// 波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 	// 字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 	// 一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No; 		// 无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; 	// 无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	 	// 收发模式
  USART_Init(USART_X, &USART_InitStructure); //初始化串口1
	
  USART_Cmd(USART_X, ENABLE);  // 使能串口
	
	//USART_ClearFlag(USART_X, USART_FLAG_TC);	

	USART_ITConfig(USART_X, USART_AF_Structure.IRQ_Type, ENABLE); 	// 开启相关中断 

	// Usart NVIC 配置
  NVIC_InitStructure.NVIC_IRQChannel = USART_AF_Structure.IRQ_Channel_N;//串口1中断通道
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、	
	
////////// 增加关闭串口中断的函数，避免初始化未完成就打断  byYJY  /////////

}

// 串口空闲中断服务程序
void USART_IRQ_Macro(volatile u8 temp)
{
	if(USART_GetITStatus(USART_X, USART_IT_IDLE) != RESET)  /* 空闲中断 */ \
	{ \
		temp = temp;					/* 清除 warning */				\
		/* DMA接收方式 */ 		\
		temp = USART_X->SR;    /* 通过读SR(状态寄存器)和DR(数据寄存器)清空闲中断 */   \
		temp = USART_X->DR;		\
			\
		/* end指向的位置=设置的接收长度-剩余的DMA缓存大小 （始终等于） */ 	\
		buffer_Rec.end = BUF_SIZE - DMA_GetCurrDataCounter(DMA_Stream_Rx);		\
			\
//		if(RESET == flag_Struct.USART_IDLE_Flag) 	/* 如果指令处理完成 */		
			flag_Struct.USART_IDLE_Flag = SET; 			/* 接收到一帧数据 */ 			\
	} 	
}


#if (1 == USART_EN)
void USART1_IRQHandler(void)                	//串口1中断服务程序
{
//	volatile static u16 endPosOld = 0; 					// end指向的前一个位置，判断是否倒置
	volatile u8 temp;												// 清标志位的
//#if SYSTEM_SUPPORT_OS 		/*如果SYSTEM_SUPPORT_OS为真，则需要支持OS. */		
//	OSIntEnter();    	
//#endif	
	
	USART_IRQ_Macro(temp); 
	
//#if SYSTEM_SUPPORT_OS 	/* 如果SYSTEM_SUPPORT_OS为真，则需要支持OS. */ 
//	OSIntExit();  											 
//#endif 	
} 	

#elif (2 == USART_EN)
void USART2_IRQHandler(void)                	//串口2中断服务程序
{
	volatile u8 temp;												// 清标志位的
	
	 USART_IRQ_Macro(temp);
	
} 

#elif (3 == USART_EN)
void USART3_IRQHandler(void)                	//串口3中断服务程序
{
	volatile u8 temp;												// 清标志位的
	
	 USART_IRQ_Macro(temp);
	
} 

#endif	


// 锁定串口，不接收命令（发送的命令全部丢掉） 后续可以考虑直接关闭串口
void lockUsart(const char str[])
{
	if(SET == flag_Struct.USART_IDLE_Flag)
	{
		flag_Struct.USART_IDLE_Flag = RESET;
		respMsgError(str, 1);					
		// 重置缓冲区
		buffer_Reset();		
	}	
}



 



