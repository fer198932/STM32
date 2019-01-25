#include "dma.h"																	   	  
#include "delay.h"		 
//////////////////////////////////////////////////////////////////////////////////	 
// DMA程序
// 1、应用在串口通信
// 2019年1月25日17:23:04 byYJY 
////////////////////////////////////////////////////////////////////////////////// 	 

#define SEND_BUF_SIZE 8200	//发送数据长度,最好等于sizeof(TEXT_TO_SEND)+2的整数倍.

u8 SendBuff[SEND_BUF_SIZE];	//发送数据缓冲区
u8 TEXT_TO_SEND[]={"ALIENTEK Explorer STM32F4 DMA 串口实验"};	

// DMA配置程序
// DMA_Streamx:DMA数据流,
// chx:DMA通道选择
// p_ar:外设地址
// m_ar:存储器地址
// amount:数据传输量  
// dir:传输方向
void DMA_Config(DMA_Stream_TypeDef *DMA_Streamx, uint32_t chx, 
	uint32_t p_ar, uint32_t m_ar, uint32_t amount, uint32_t dir)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	if((u32)DMA_Streamx > (u32)DMA2) 					// 判断当前stream是属于DMA2还是DMA1
	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); 		// 使能DMA2时钟
	}
	else
	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 		// 使能DMA1时钟
	}
	DMA_DeInit(DMA_Streamx); 			// 系统默认的DMA设置参数
	
	while(DMA_GetCmdStatus(DMA_Streamx) != DISABLE) {}  		// 等待DMA可配置

	/* 配置 DMA 数据流 */
	DMA_InitStructure.DMA_Channel = chx; 		// 通道选择
	DMA_InitStructure.DMA_PeripheralBaseAddr = p_ar; // DMA外设地址
	DMA_InitStructure.DMA_Memory0BaseAddr = m_ar;			// 存储器地址
	DMA_InitStructure.DMA_DIR = dir; // 传输方向， 如 DMA_DIR_MemoryToPeripheral → 存储器到外设模式
	DMA_InitStructure.DMA_BufferSize = amount; // 传输数量
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // 外设非增量模式
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // 存储器增量模式
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // 外设数据长度:8位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // 存储器数据长度:8位 
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal; // 使用普通模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; // 中等优先级 优先级： byYJY
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;  // 不使用FIFO
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full; 
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single; // 存储器 单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; // 外设突发单次传输
	
	DMA_Init(DMA_Streamx, &DMA_InitStructure); 	// 初始化DMA数据流
}

// 开启一次DMA传输的程序
void DMA_Enable(DMA_Stream_TypeDef *DMA_Streamx, uint32_t amount)
{
	DMA_Cmd(DMA_Streamx, DISABLE); 											// 关闭DMA传输
	
	while(DMA_GetCmdStatus(DMA_Streamx) != DISABLE) {}  // 等待DMA可配置
	
	DMA_SetCurrDataCounter(DMA_Streamx, amount); 				// 数据传输量		
	
	DMA_Cmd(DMA_Streamx, ENABLE);                      	// 开启DMA传输 
}

// 串口使用DMA方式的初始化函数
void DMA_USART_Init(void)
{
	DMA_Config(DMA2_Stream7, DMA_Channel_4, (u32)&USART1->DR, (u32)TEXT_TO_SEND,
		SEND_BUF_SIZE, DMA_DIR_MemoryToPeripheral);
	
	
}

















