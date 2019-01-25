/********************************************************************************************************
* 数据处理程序
********************************************************************************************************/
#include "ComDataProc.h"

// 回复上位机的程序
void respMsgError(u8 status)
{
	printf("ERROR");
	
}

// 回复串口数据信息
void respUsartMsg(const u8 backResString[], u16 length)
{
	u16 i;
	for(i=0;i<length;i++)
	{
		USART_SendData(USART_X, backResString[i]); 
		while(USART_GetFlagStatus(USART_X,USART_FLAG_TC)!=SET);		//等待发送结束
	}	
}



