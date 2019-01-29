#include "dma.h"			

//////////////////////////////////////////////////////////////////////////////////	 
// DMA程序
// 1、应用在串口通信
// 2019年1月25日17:23:04 byYJY 
////////////////////////////////////////////////////////////////////////////////// 	 

// u8 DMA_Rec[BUF_SIZE];

// DMA配置程序
// DMA_Streamx:DMA数据流,
// chx:DMA通道选择
// p_ar:外设地址
// m_ar:存储器地址
// amount:数据传输量  
// dir:传输方向
// DMA_Mode_X: 普通模式  还是循环模式
void DMA_Config(DMA_Stream_TypeDef *DMA_Streamx, uint32_t chx, uint32_t p_ar, 
	uint32_t m_ar, uint32_t amount, uint32_t dir, uint32_t DMA_Mode_X)
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
	DMA_InitStructure.DMA_Mode = DMA_Mode_X; 	// 循环模式或普通模式 如：DMA_Mode_Normal 普通模式 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; // 中等优先级 优先级： byYJY
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;  // 不使用FIFO
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full; 
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single; // 存储器 单次传输
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; // 外设突发单次传输
	
	DMA_Init(DMA_Streamx, &DMA_InitStructure); 	// 初始化DMA数据流
}

// 开启一次DMA传输的程序
void DMA_Restart(DMA_Stream_TypeDef *DMA_Streamx, uint32_t amount)
{
	DMA_Cmd(DMA_Streamx, DISABLE); 											// 关闭DMA传输
	
	while(DMA_GetCmdStatus(DMA_Streamx) != DISABLE) {}  // 等待DMA可配置
	
	DMA_SetCurrDataCounter(DMA_Streamx, amount); 				// 数据传输量		
	
	DMA_Cmd(DMA_Streamx, ENABLE);                      	// 开启DMA传输 
}

// 串口使用DMA方式的初始化函数
void DMA_USART_Init(void)
{
	/* DMA发送初始化  */
	DMA_Config(DMA_Stream_Tx, DMA_Channel_Tx, (u32)&USART1->DR, (u32)buffer_Trans.data,
		BUF_SIZE, DMA_DIR_MemoryToPeripheral, DMA_Mode_Normal);
	/* DMA接收初始化 */
	DMA_Config(DMA_Stream_Rx, DMA_Channel_Rx, (u32)&USART1->DR, (u32)buffer_Rec.data,
		BUF_SIZE, DMA_DIR_PeripheralToMemory, DMA_Mode_Circular);
	DMA_Cmd(DMA_Stream_Rx, DISABLE);					// 调用一下，否则第一条不会接收
	DMA_SetCurrDataCounter(DMA_Stream_Rx, BUF_SIZE);
	DMA_Cmd(DMA_Stream_Rx, ENABLE);
}

















