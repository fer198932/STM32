#include "dma.h"			

//////////////////////////////////////////////////////////////////////////////////	 
// DMA����
// 1��Ӧ���ڴ���ͨ��
// 2019��1��25��17:23:04 byYJY 
////////////////////////////////////////////////////////////////////////////////// 	 

// u8 DMA_Rec[BUF_SIZE];

// DMA���ó���
// DMA_Streamx:DMA������,
// chx:DMAͨ��ѡ��
// p_ar:�����ַ
// m_ar:�洢����ַ
// amount:���ݴ�����  
// dir:���䷽��
// DMA_Mode_X: ��ͨģʽ  ����ѭ��ģʽ
void DMA_Config(DMA_Stream_TypeDef *DMA_Streamx, uint32_t chx, uint32_t p_ar, 
	uint32_t m_ar, uint32_t amount, uint32_t dir, uint32_t DMA_Mode_X)
{
	DMA_InitTypeDef DMA_InitStructure;
	
	if((u32)DMA_Streamx > (u32)DMA2) 					// �жϵ�ǰstream������DMA2����DMA1
	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE); 		// ʹ��DMA2ʱ��
	}
	else
	{
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE); 		// ʹ��DMA1ʱ��
	}
	DMA_DeInit(DMA_Streamx); 			// ϵͳĬ�ϵ�DMA���ò���
	
	while(DMA_GetCmdStatus(DMA_Streamx) != DISABLE) {}  		// �ȴ�DMA������

	/* ���� DMA ������ */
	DMA_InitStructure.DMA_Channel = chx; 		// ͨ��ѡ��
	DMA_InitStructure.DMA_PeripheralBaseAddr = p_ar; // DMA�����ַ
	DMA_InitStructure.DMA_Memory0BaseAddr = m_ar;			// �洢����ַ
	DMA_InitStructure.DMA_DIR = dir; // ���䷽�� �� DMA_DIR_MemoryToPeripheral �� �洢��������ģʽ
	DMA_InitStructure.DMA_BufferSize = amount; // ��������
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; // ���������ģʽ
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable; // �洢������ģʽ
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; // �������ݳ���:8λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; // �洢�����ݳ���:8λ 
	DMA_InitStructure.DMA_Mode = DMA_Mode_X; 	// ѭ��ģʽ����ͨģʽ �磺DMA_Mode_Normal ��ͨģʽ 
	DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; // �е����ȼ� ���ȼ��� byYJY
	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;  // ��ʹ��FIFO
	DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full; 
	DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single; // �洢�� ���δ���
	DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; // ����ͻ�����δ���
	
	DMA_Init(DMA_Streamx, &DMA_InitStructure); 	// ��ʼ��DMA������
}

// ����һ��DMA����ĳ���
void DMA_Restart(DMA_Stream_TypeDef *DMA_Streamx, uint32_t amount)
{
	DMA_Cmd(DMA_Streamx, DISABLE); 											// �ر�DMA����
	
	while(DMA_GetCmdStatus(DMA_Streamx) != DISABLE) {}  // �ȴ�DMA������
	
	DMA_SetCurrDataCounter(DMA_Streamx, amount); 				// ���ݴ�����		
	
	DMA_Cmd(DMA_Streamx, ENABLE);                      	// ����DMA���� 
}

// ����ʹ��DMA��ʽ�ĳ�ʼ������
void DMA_USART_Init(void)
{
	/* DMA���ͳ�ʼ��  */
	DMA_Config(DMA_Stream_Tx, DMA_Channel_Tx, (u32)&USART1->DR, (u32)buffer_Trans.data,
		BUF_SIZE, DMA_DIR_MemoryToPeripheral, DMA_Mode_Normal);
	/* DMA���ճ�ʼ�� */
	DMA_Config(DMA_Stream_Rx, DMA_Channel_Rx, (u32)&USART1->DR, (u32)buffer_Rec.data,
		BUF_SIZE, DMA_DIR_PeripheralToMemory, DMA_Mode_Circular);
	DMA_Cmd(DMA_Stream_Rx, DISABLE);					// ����һ�£������һ���������
	DMA_SetCurrDataCounter(DMA_Stream_Rx, BUF_SIZE);
	DMA_Cmd(DMA_Stream_Rx, ENABLE);
}

















