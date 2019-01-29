/********************************************************************************************************
* 数据处理程序
********************************************************************************************************/
#include "ComDataProc.h"

Proc_Data proc_Data; 		// 命令数据，有成员指向plus_Data
Plus_Data plus_Data;		// 脉冲数据，控制电机运动

// 命令数据结构体的初始化
void comData_Init(void)
{
	proc_Data.cmd_Type = INVALID_CMD;				// 初始化为无效命令 全局变量默认初始化为0;
	proc_pt_motionData(NULL);								// 默认情况
}


// 从缓冲区得到数据并进行处理、启动相关电机的程序
void procDataStep(void)
{
	if(IS_OK == bufData_Proc())
	{
		switch(proc_Data.cmd_Type)
		{
			case 0x0B:
				selfCheckFunc();
				break;
			case 0x0C:
				
				break;
			case 0x0D:
				
				break;
			case 0x0E:
				
				break;
			default:
				respMsgError("命令解析后运行时错误！\r\n", 1);
				break;			
		}
	}
	else
	{
		respMsgError("命令数据解析有误或为空！\r\n", 1);
	}
}

// 检查从缓冲区读取的数据是否合格
static BufData_Status bufData_Proc(void)
{
	PosCur posCur;				// 位置游标
	posCur.start = buffer_Rec.start;
	posCur.end = posCur.start;
	
	if(buffer_Rec.start <  buffer_Rec.end) 					// 缓冲区正序
	{
		while(posCur.start != buffer_Rec.end)			
		{
			/* 帧头判断  */
			if(FrameB1 == buffer_Rec.data[posCur.start++])			// 0x0E
				if(FrameB2 == buffer_Rec.data[posCur.start++])		// 0x09
				{
					posCur.end = posCur.start;
					buffer_Rec.start = posCur.start-2;					// 缓冲区指向新的0x0E
					/*  帧尾判断  */
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
		buffer_Rec.start = posCur.start-1;					// 缓冲区指向新的end位置后面一位
		return NOT_OK;		
	}
	else if (buffer_Rec.start > buffer_Rec.end) 		// 缓冲区逆序
	{
		/////////////// 
		/* 使用了缓冲区将要溢出，重置缓冲区的方法 */		
		return NOT_OK;
	}
	else																						// 为空
	{
		return NOT_OK;
	}
}

// 处理对应区间上的缓冲区数据
static BufData_Status bufData_Proc_Region(PosCur posCur)
{
	u16 dataSize;
	if(posCur.start <  posCur.end) 					// 缓冲区正序
	{
		dataSize = posCur.end - posCur.start + 3 + 1; 
		if(getSetDataSize(posCur) == dataSize)
		{			
			return setCmdData(posCur);
		}
		else
			return NOT_OK;
	}
	else if (posCur.start > posCur.end) 		// 缓冲区逆序
	{
		///////////////
		/* 使用了缓冲区将要溢出，重置缓冲区的方法 */
		return NOT_OK;
	}
	else																						// 为空
	{
		return NOT_OK;
	}
}

// 设置回复字符串的格式
void setRespStr(u8 resStr[], u16 length)
{
	resStr[0] = FrameB1;
	resStr[1] = FrameB2;
	
	resStr[2] = proc_Data.cmd_Type;				// 帧标识
	
	resStr[3] = (length & 0x00FF);												// 长度
	resStr[4] = (length >> 8);
	
	resStr[5] = proc_Data.resp_Excute;		// 标志码
	
	resStr[length-1] = FrameEnd;
}

// 得到命令设定的数据长度
static u16 getSetDataSize(PosCur posCur)
{
	return ((buffer_Rec.data[posCur.start+2]<<8) + (buffer_Rec.data[posCur.start+1]<<0));
}

// 检查OK后，设定命令数据的结构体
static BufData_Status setCmdData(PosCur posCur)
{
	buffer_Rec.start = (++posCur.end);			// 截掉已经处理的数据段
	
	if(posCur.start <  posCur.end) 					// 缓冲区正序
	{
		proc_Data.cmd_Type = buffer_Rec.data[posCur.start];
		switch(proc_Data.cmd_Type)
		{
			case 0x0B: 			// 自检
				proc_Data.cmd_Excute = buffer_Rec.data[posCur.start+3]; 
				proc_Data.resp_Excute = proc_Data.cmd_Excute;
				return IS_OK;
//				break;
			case 0x0C:			// 控制
				
				return IS_OK;
//				break;
			case 0x0D: 			// 数据
				
				return IS_OK;
//				break;
			case 0x0E: 			// 状态
				
				return IS_OK;
//				break;
			default:
				respMsgError("命令解析错误！\r\n", 1);
				return NOT_OK;
//				break;
		}
	}
	else if (posCur.start > posCur.end) 		// 缓冲区逆序
	{
		///////////////
		/* 使用了缓冲区将要溢出，重置缓冲区的方法 */
		return NOT_OK;
	}
	else																						// 为空
	{
		return NOT_OK;
	}
}

// proc结构体指向对应数据结构体
static void proc_pt_motionData(void* _data)
{
	proc_Data.cmd_Data = _data;
}

// 回复上位机的程序
void respMsgError(const char str[], u8 status)
{
	printf(str);
//	mymemcpy(&buffer_Trans, (void*)"ERROR", 5);
//	DMA_Restart(DMA_Stream_Tx, 5);
}

// 回复串口数据信息
void respUsartMsg(const u8 backResString[], u16 length)
{
	/* DMA式串口传送 */
	static u8 firstTxFlag = 0; 					// 第一次发送时，无法清除标志位
	
	mymemcpy(&buffer_Trans, (void*)backResString, length);
	
	// 清DMA标志位
	if(firstTxFlag)
	{
		while(1)
		{
			if(DMA_GetFlagStatus(DMA_Stream_Tx, DMA_FLAG_Tx) != RESET) 			// 等待DMA发送完成
			{
				DMA_ClearFlag(DMA_Stream_Tx, DMA_FLAG_Tx); 										// 清除DMA_Steam传输完成标志
				break;
			}
		}
	} 
	else
	{
		firstTxFlag = 1;
	}
	
	// 打开一次DMA传输
	DMA_Restart(DMA_Stream_Tx, length);
	
	/* 中断式串口传送 */
//	u16 i;
//	for(i=0;i<length;i++)
//	{
//		USART_SendData(USART_X, backResString[i]); 
//		while(USART_GetFlagStatus(USART_X,USART_FLAG_TC)!=SET);		//等待发送结束
//	}	
}



