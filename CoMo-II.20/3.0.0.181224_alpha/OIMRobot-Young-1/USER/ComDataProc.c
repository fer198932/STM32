/********************************************************************************************************
* ���ݴ������
********************************************************************************************************/
#include "ComDataProc.h"

// �ظ���λ���ĳ���
void respMsgError(u8 status)
{
	printf("ERROR");
	
}

// �ظ�����������Ϣ
void respUsartMsg(const u8 backResString[], u16 length)
{
	u16 i;
	for(i=0;i<length;i++)
	{
		USART_SendData(USART_X, backResString[i]); 
		while(USART_GetFlagStatus(USART_X,USART_FLAG_TC)!=SET);		//�ȴ����ͽ���
	}	
}



