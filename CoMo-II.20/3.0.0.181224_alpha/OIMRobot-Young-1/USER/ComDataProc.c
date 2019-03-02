/********************************************************************************************************
* ���ݴ������
********************************************************************************************************/
#include "ComDataProc.h"

extern		Flag_Structure 				flag_Struct;
extern 		vu32 									Offline_Data_Num;					// �ѻ��ӹ����ݵı�־
extern		Motion_Strcuture 			motion_Data_Pre;	

static Proc_Data proc_Data; 		// �������ݣ��г�Աָ��plus_Data
//Plus_Data plus_Data;		// �������ݣ����Ƶ���˶�

// �������ݽṹ��ĳ�ʼ��
void comData_Init(void)
{
	mymemset(&proc_Data, 0, sizeof(proc_Data));
	mymemset(&motion_Data_Pre, 0, sizeof(motion_Data_Pre));
}


// �ӻ������õ����ݲ����д���
void UsartDataProc(void)
{
	// ����Ϊ0
	comData_Init();
	
	if(IS_OK == bufData_Proc())
	{
		// ���ݺϸ�ͽ��и���
//		motion_Data_Pre.cmd_Datas = proc_Data;
		mymemcpy(&(motion_Data_Pre.cmd_Datas), &proc_Data, sizeof(proc_Data));
	}
	else 
	{
		respMsgError("�������ݽ��������Ϊ�գ�\r\n", 1);
	}
	
	// ���û�����
	buffer_Reset();
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
					posCur.end = buffer_Rec.data[DATA_LENGTH_POSITION]-2;
					if(FrameEnd == buffer_Rec.data[posCur.end + 1])
					{
						if(IS_OK == bufData_Proc_Region(posCur))
							return IS_OK;
						else
							return NOT_OK;
					}
//					while(posCur.end <= buffer_Rec.end)
//					{
//						if(FrameEnd == buffer_Rec.data[posCur.end])		// 0xFF
//						{
//							posCur.end--;
//							if(IS_OK == bufData_Proc_Region(posCur))
//								return IS_OK;
//							else
//								return NOT_OK;
//						} 
//						else 
//							posCur.end++;
//					}
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
void setRespStr(Proc_Data* pCmd, u8 resStr[], u16 length)
{
	resStr[0] = FrameB1;
	resStr[1] = FrameB2;
	
	resStr[2] = pCmd->cmd_Type;				// ֡��ʶ
	
	resStr[3] = (length & 0x00FF);												// ����
	resStr[4] = (length >> 8);
	
	resStr[5] = pCmd->resp_Excute;		// ��־��
	
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
				
				// ����ָ�����ж�ִ���ĸ�����
				switch(proc_Data.cmd_Excute)
				{
					case PLUS_DATA: 						// �������ݴ���
						plus_Data_Proc(posCur);
						return IS_OK;
					case OFFLINE_DATA:					// �ѻ��ӹ�
						offline_Data_Proc(posCur);
						return IS_OK;
					default:
						respMsgError("����ָ�����������\r\n", 1);
						return NOT_OK;
				}
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

// �ظ���λ���ĳ���
void respMsgError(const char str[], u8 status)
{
#if PRIN2DISP
	printf(str);
#endif
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

// �����������ݵĺ�����֡��ʶ0X0D��ָ����0X22
static void plus_Data_Proc(PosCur posCur)
{
	/* �������� */
				// X��
			proc_Data.plus_Datas.plusNum[0] = getSetDataPlusNum(posCur, 0);
				proc_Data.plus_Datas.clk[0] = getSetDataClk(posCur, 0);
			
				if((0 == proc_Data.plus_Datas.plusNum[0]) && (0 == proc_Data.plus_Datas.clk[0]))  	// �����趨
					proc_Data.plus_Datas.dir[0] = TBD_DIR;																	// ����δ��
				else if(0 == getSetDataDir(posCur, 0))
					proc_Data.plus_Datas.dir[0] = POS_DIR;																	// ����
				else
					proc_Data.plus_Datas.dir[0] = NEG_DIR;																	// ����
				
				// Y��
				proc_Data.plus_Datas.plusNum[1] = getSetDataPlusNum(posCur, 1);
				proc_Data.plus_Datas.clk[1] = getSetDataClk(posCur, 1);
				
				if((0 == proc_Data.plus_Datas.plusNum[1]) || (0 == proc_Data.plus_Datas.clk[1]))  	// �����趨
					proc_Data.plus_Datas.dir[1] = TBD_DIR;																	// ����δ��
				else if(0 == getSetDataDir(posCur, 1))
					proc_Data.plus_Datas.dir[1] = POS_DIR;																	// ����
				else
					proc_Data.plus_Datas.dir[1] = NEG_DIR;																	// ����
			
				// Z��
				proc_Data.plus_Datas.plusNum[2] = getSetDataPlusNum(posCur, 2);
				proc_Data.plus_Datas.clk[2] = getSetDataClk(posCur, 2);
				
				if((0 == proc_Data.plus_Datas.plusNum[2]) || (0 == proc_Data.plus_Datas.clk[2]))  	// �����趨
					proc_Data.plus_Datas.dir[2] = TBD_DIR;																	// ����δ��
				else if(0 == getSetDataDir(posCur, 2))
					proc_Data.plus_Datas.dir[2] = POS_DIR;																	// ����
				else
					proc_Data.plus_Datas.dir[2] = NEG_DIR;																	// ����
				
				// A��
				proc_Data.plus_Datas.plusNum[3] = getSetDataPlusNum(posCur, 3);
				proc_Data.plus_Datas.clk[3] = getSetDataClk(posCur, 3);
				
				if((0 == proc_Data.plus_Datas.plusNum[3]) || (0 == proc_Data.plus_Datas.clk[3]))  	// �����趨
					proc_Data.plus_Datas.dir[3] = TBD_DIR;																	// ����δ��
				else if(0 == getSetDataDir(posCur, 3))
					proc_Data.plus_Datas.dir[3] = POS_DIR;																	// ����
				else
					proc_Data.plus_Datas.dir[3] = NEG_DIR;																	// ����
				
				// B��
				proc_Data.plus_Datas.plusNum[4] = getSetDataPlusNum(posCur, 4);
				proc_Data.plus_Datas.clk[4] = getSetDataClk(posCur, 4);
				
				if((0 == proc_Data.plus_Datas.plusNum[4]) || (0 == proc_Data.plus_Datas.clk[4]))  	// �����趨
					proc_Data.plus_Datas.dir[4] = TBD_DIR;																	// ����δ��
				else if(0 == getSetDataDir(posCur, 4))
					proc_Data.plus_Datas.dir[4] = POS_DIR;																	// ����
				else
					proc_Data.plus_Datas.dir[4] = NEG_DIR;																	// ����
	
}

// �ѻ��ӹ� ֡��ʶ0X0D��ָ����0X25
// �磺 0E 09 0D 10 00 25 00 01 00 00 00 E2 E1 6A 0F FF
static void offline_Data_Proc(PosCur posCur)
{
	proc_Data.cmd_Value = (buffer_Rec.data[posCur.start+5]<<0) + 
		(buffer_Rec.data[posCur.start+6]<<8) + (buffer_Rec.data[posCur.start+7]<<16) + 
		(buffer_Rec.data[posCur.start+8]<<24);
	
	// ���ݳ�������
//	proc_Data.offline_length = LENGTH_OFFL;
	Offline_Data_Num = LENGTH_OFFL;
}


#if PRIN2DISP
#else
// �趨�˶����ݵĴ��ڷ�������ĸ�ʽ
void setRespStr_Motion(Proc_Data* pCmd, u8 respStr[], u16 length, u8 status)
{
	setRespStr(pCmd, respStr, RESP_MOTIONMSG_LENGTH);
	respStr[2] = DATAINFO;
	respStr[5] = PLUS_DATA;
	respStr[6] = 0x00;
	respStr[7] = status;
}
#endif


