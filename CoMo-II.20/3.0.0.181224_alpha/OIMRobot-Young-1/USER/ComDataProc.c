/********************************************************************************************************
* ���ݴ������
********************************************************************************************************/
#include "ComDataProc.h"

// ���ڽ���һ֡������ɵı�־
// volatile 	IfOK_Status 	USARTRx_IfOK_Flag 	= 	NOT_OK;

// ����DMA��������ı�־
// volatile 	ErrorStatus 	USARTRx_DMAOut_Flag 	= 	SUCCESS;

// �����Ƿ������
// volatile IfOK_Status 	DataPro_IsOK_Flag = IS_OK;

Proc_Data proc_Data; 		// �������ݣ��г�Աָ��plus_Data
Plus_Data plus_Data;		// �������ݣ����Ƶ���˶�

// �������ݽṹ��ĳ�ʼ��
void comData_Init(void)
{
	proc_Data.cmd_Type = INVALID_CMD;				// ��ʼ��Ϊ��Ч���� ȫ�ֱ���Ĭ�ϳ�ʼ��Ϊ0;
	proc_pt_motionData(NULL);								// Ĭ�����
}


// �ӻ������õ����ݲ����д���
void UsartDataProc(void)
{
	if(IS_OK == bufData_Proc())
	{
		
	}
	else 
	{
		respMsgError("�������ݽ��������Ϊ�գ�\r\n", 1);
	}
		
	
	
//	DataPro_IsOK_Flag = NOT_OK;		// ������־λ�������ݴ���
	
//	if(IS_OK == bufData_Proc())
//	{
//		switch(proc_Data.cmd_Type)
//		{
//			case 0x0B:
//				selfCheckFunc();
//				break;
//			case 0x0C:
//				
//				break;
//			case 0x0D:
//				motionDataProc();
//				break;
//			case 0x0E:
//				
//				break;
//			default:
//				respMsgError("�������������ʱ����\r\n", 1);
//				break;			
//		}
//	}
//	else
//	{
//		respMsgError("�������ݽ��������Ϊ�գ�\r\n", 1);
//	}
	
//	DataPro_IsOK_Flag = IS_OK; 		// ������һ��ָ����ٴ򿪴�����һ��
}

// ���ӻ�������ȡ�������Ƿ�ϸ�
static IfOK_Status bufData_Proc(void)
{
	PosCur posCur;				// λ���α�
	posCur.start = buffer_Rec.start;
	posCur.end = posCur.start;
	
	if(buffer_Rec.start <  buffer_Rec.end) 					// ����������
	{
		while(posCur.start != buffer_Rec.end)			
		{
			/* ֡ͷ�ж�  */
			if(FrameB1 == buffer_Rec.data[posCur.start++])			// 0x0E
				if(FrameB2 == buffer_Rec.data[posCur.start++])		// 0x09
				{
					posCur.end = posCur.start;
					buffer_Rec.start = posCur.start-2;					// ������ָ���µ�0x0E
					/*  ֡β�ж�  */
					while(posCur.end <= buffer_Rec.end)
					{
						if(FrameEnd == buffer_Rec.data[posCur.end])		// 0xFF
						{
							posCur.end--;
							if(IS_OK == bufData_Proc_Region(posCur))
								return IS_OK;
							else
								return NOT_OK;
						} 
						else 
							posCur.end++;
					}
					return NOT_OK;
				}
		}
		buffer_Rec.start = posCur.start-1;					// ������ָ���µ�endλ�ú���һλ
		return NOT_OK;		
	}
	else if (buffer_Rec.start > buffer_Rec.end) 		// ����������
	{
		/////////////// 
		/* ʹ���˻�������Ҫ��������û������ķ��� */		
		return NOT_OK;
	}
	else																						// Ϊ��
	{
		return NOT_OK;
	}
}

// �����Ӧ�����ϵĻ���������
static IfOK_Status bufData_Proc_Region(PosCur posCur)
{
	u16 dataSize;
	if(posCur.start <  posCur.end) 					// ����������
	{
		dataSize = posCur.end - posCur.start + 3 + 1; 
		if(getSetDataSize(posCur) == dataSize)
		{			
			return setCmdData(posCur);
		}
		else
			return NOT_OK;
	}
	else if (posCur.start > posCur.end) 		// ����������
	{
		///////////////
		/* ʹ���˻�������Ҫ��������û������ķ��� */
		return NOT_OK;
	}
	else																						// Ϊ��
	{
		return NOT_OK;
	}
}

// ���ûظ��ַ����ĸ�ʽ
void setRespStr(u8 resStr[], u16 length)
{
	resStr[0] = FrameB1;
	resStr[1] = FrameB2;
	
	resStr[2] = proc_Data.cmd_Type;				// ֡��ʶ
	
	resStr[3] = (length & 0x00FF);												// ����
	resStr[4] = (length >> 8);
	
	resStr[5] = proc_Data.resp_Excute;		// ��־��
	
	resStr[length-1] = FrameEnd;
}

// �õ������趨�����ݳ���
static u16 getSetDataSize(PosCur posCur)
{
	return ((buffer_Rec.data[posCur.start+2]<<8) + (buffer_Rec.data[posCur.start+1]<<0));
}

// �õ������趨�������� shift��ƫ���� 0-4 ��Ӧ5��
static u32 getSetDataPlusNum(PosCur posCur, u8 shift)
{
	shift *= 6;
	return ((buffer_Rec.data[posCur.start+7+shift]<<16) + 
		(buffer_Rec.data[posCur.start+6+shift]<<8) + 
		(buffer_Rec.data[posCur.start+5+shift]<<0));
}

// �õ������趨��Ƶ�� shift��ƫ���� 0-4 ��Ӧ5��
static u32 getSetDataClk(PosCur posCur, u8 shift)
{
	shift *= 6;
	return (((buffer_Rec.data[posCur.start+10+shift]<<16)&0x7F) + 
		(buffer_Rec.data[posCur.start+9+shift]<<8) + 
		(buffer_Rec.data[posCur.start+8+shift]<<0));
}

// �õ������趨�ķ��� shift��ƫ���� 0-4 ��Ӧ5��
static u8 getSetDataDir(PosCur posCur, u8 shift)
{
	shift *= 6;
	return 	(buffer_Rec.data[posCur.start+10+shift]>>7);
}

// ���OK���趨�������ݵĽṹ��
static IfOK_Status setCmdData(PosCur posCur)
{
	buffer_Rec.start = (++posCur.end);			// �ص��Ѿ���������ݶ�
	
	if(posCur.start <  posCur.end) 					// ����������
	{
		proc_Data.cmd_Type = buffer_Rec.data[posCur.start];
		
		switch(proc_Data.cmd_Type)
		{
			case 0x0B: 			// �Լ�
				proc_Data.cmd_Excute = (buffer_Rec.data[posCur.start+4]<<8) + buffer_Rec.data[posCur.start+3]; 
				proc_Data.resp_Excute = proc_Data.cmd_Excute;
				return IS_OK;
//				break;
			case 0x0C:			// ����
				
				return IS_OK;
//				break;
			case 0x0D: 			// ����
				proc_Data.cmd_Excute = (buffer_Rec.data[posCur.start+4]<<8) + (buffer_Rec.data[posCur.start+3]<<0);
				proc_Data.resp_Excute = proc_Data.cmd_Excute;
				
				/* �������� */
				// X��
				plus_Data.plusNum[0] = getSetDataPlusNum(posCur, 0);
				plus_Data.clk[0] = getSetDataClk(posCur, 0);
				plus_Data.dir[0] = getSetDataDir(posCur, 0);
				
				// Y��
				plus_Data.plusNum[1] = getSetDataPlusNum(posCur, 1);
				plus_Data.clk[1] = getSetDataClk(posCur, 1);
				plus_Data.dir[1] = getSetDataDir(posCur, 1);
			
				// Z��
				plus_Data.plusNum[2] = getSetDataPlusNum(posCur, 2);
				plus_Data.clk[2] = getSetDataClk(posCur, 2);
				plus_Data.dir[2] = getSetDataDir(posCur, 2);
				
				// A��
				plus_Data.plusNum[3] = getSetDataPlusNum(posCur, 3);
				plus_Data.clk[3] = getSetDataClk(posCur, 3);
				plus_Data.dir[3] = getSetDataDir(posCur, 3);
				
				// B��
				plus_Data.plusNum[4] = getSetDataPlusNum(posCur, 4);
				plus_Data.clk[4] = getSetDataClk(posCur, 4);
				plus_Data.dir[4] = getSetDataDir(posCur, 4);
				
				return IS_OK;
//				break;
			case 0x0E: 			// ״̬
				
				return IS_OK;
//				break;
			default:
				respMsgError("�����������\r\n", 1);
				return NOT_OK;
//				break;
		}
	}
	else if (posCur.start > posCur.end) 		// ����������
	{
		///////////////
		/* ʹ���˻�������Ҫ��������û������ķ��� */
		return NOT_OK;
	}
	else																						// Ϊ��
	{
		return NOT_OK;
	}
}

// proc�ṹ��ָ���Ӧ���ݽṹ��
static void proc_pt_motionData(void* _data)
{
	proc_Data.cmd_Data = _data;
}

// �ظ���λ���ĳ���
void respMsgError(const char str[], u8 status)
{
	printf(str);
//	mymemcpy(&buffer_Trans, (void*)"ERROR", 5);
//	DMA_Restart(DMA_Stream_Tx, 5);
}

// �ظ�����������Ϣ
void respUsartMsg(const u8 backResString[], u16 length)
{
	/* DMAʽ���ڴ��� */
	static u8 firstTxFlag = 0; 					// ��һ�η���ʱ���޷������־λ
	
	mymemcpy(&buffer_Trans, (void*)backResString, length);
	
	// ��DMA��־λ
	if(firstTxFlag)
	{
		while(1)
		{
			if(DMA_GetFlagStatus(DMA_Stream_Tx, DMA_FLAG_Tx) != RESET) 			// �ȴ�DMA�������
			{
				DMA_ClearFlag(DMA_Stream_Tx, DMA_FLAG_Tx); 										// ���DMA_Steam������ɱ�־
				break;
			}
		}
	} 
	else
	{
		firstTxFlag = 1;
	}
	
	// ��һ��DMA����
	DMA_Restart(DMA_Stream_Tx, length);
	
	/* �ж�ʽ���ڴ��� */
//	u16 i;
//	for(i=0;i<length;i++)
//	{
//		USART_SendData(USART_X, backResString[i]); 
//		while(USART_GetFlagStatus(USART_X,USART_FLAG_TC)!=SET);		//�ȴ����ͽ���
//	}	
}



