/********************************************************************************************************
*    ��λIO�ڵ���س���
********************************************************************************************************/
#include "limit_IO.h"

static const u16 length = 9;

static u8 backResString[length]; 						// �ظ����ַ���

static u32 keyValue = 0;
static u32 keyValueOld = 0;

// ��λ��IO��
static GPIO_XX x_p_limit;
static GPIO_XX x_n_limit;
static GPIO_XX y_p_limit;
static GPIO_XX y_n_limit;
static GPIO_XX z_p_limit;
static GPIO_XX z_n_limit;
static GPIO_XX a_limit;
static GPIO_XX b_limit;

// IO�ڳ�ʼ������
void limit_IO_Init(void)
{
	// ��ʼ������λ���IO��
	limitInit(&x_p_limit, X_P_LIMIT);
	limitInit(&x_n_limit, X_N_LIMIT);
	limitInit(&y_p_limit, Y_P_LIMIT);
	limitInit(&y_n_limit, Y_N_LIMIT);
	limitInit(&z_p_limit, Z_P_LIMIT);
	limitInit(&z_n_limit, Z_N_LIMIT);
	limitInit(&a_limit, A_LIMIT);
	limitInit(&b_limit, B_LIMIT);	
}

// �鿴�Ƿ�����λ����
// mode:0,��֧��������;1,֧��������;
static void LIMIT_Scan(void)
{
	if((0==X_P_Lim()) || (0==X_N_Lim()) || (0==Y_P_Lim()) || (0==Y_N_Lim()) || 
			(0==Z_P_Lim()) || (0==Z_N_Lim()) || (0==A_Lim()) || (0==B_Lim()))				//  �а�������
	{
		delay_us(10);				// ����ȥ��
		
		keyValue = 0;
		
		if(0==X_P_Lim())  keyValue=keyValue | 1;
		if(0==X_N_Lim())  keyValue=keyValue | 2;
		if(0==Y_P_Lim())  keyValue=keyValue | 4;
		if(0==Y_N_Lim())  keyValue=keyValue | 8;
		if(0==Z_P_Lim())  keyValue=keyValue | 16;
		if(0==Z_N_Lim())  keyValue=keyValue | 32;
		if(0==A_Lim())   	keyValue=keyValue | 64;
		if(0==B_Lim())   	keyValue=keyValue | 128;
		
	}
	else if((1==X_P_Lim()) && (1==X_N_Lim()) && (1==Y_P_Lim()) && (1==Y_N_Lim()) &&
					(1==Z_P_Lim()) && (1==Z_N_Lim()) && (1==A_Lim()) && (1==B_Lim()))		// �ް�������
	{
		keyValue = 0;
	}
}

// ѭ���鿴�Ƿ�����λ��������������λ
void procLimit(void)
{
	LIMIT_Scan();
	
	if(keyValue != keyValueOld)
	{
		keyValueOld = keyValue;
		
		if(keyValue != 0)
		{
			backResString[0] = FrameB1;
			backResString[1] = FrameB2;
			backResString[2] = STATUSINFO;
			
			backResString[3] = length;					// ����
			backResString[4] = 0x00;
			
			backResString[8] = FrameEnd;
			
			/////////////ע���־λ������//////////////////////// byYJY
			
		}
		
		/* X+ ��λ */
		if((keyValue>>0) & 0x0001)
		{
			//////////////////�رյ���ڸ÷����ϵ��˶� byYJY
			backResString[5] = X_P_MSG1;
			backResString[6] = 0x00;
			backResString[7] = X_P_MSG2;
			
			respUsartMsg(backResString, length);
		}
		
		/* X- ��λ */
		if((keyValue>>1) & 0x0001)
		{
			//////////////////�رյ���ڸ÷����ϵ��˶� byYJY
			backResString[5] = X_N_MSG1;
			backResString[6] = 0x00;
			backResString[7] = X_N_MSG2;
			
			respUsartMsg(backResString, length);
		}
		
		/* Y+ ��λ */
		if((keyValue>>2) & 0x0001)
		{
			//////////////////�رյ���ڸ÷����ϵ��˶� byYJY
			backResString[5] = Y_P_MSG1;
			backResString[6] = 0x00;
			backResString[7] = Y_P_MSG2;
			
			respUsartMsg(backResString, length);
		}
		
		/* Y- ��λ */
		if((keyValue>>3) & 0x0001)
		{
			//////////////////�رյ���ڸ÷����ϵ��˶� byYJY
			backResString[5] = Y_N_MSG1;
			backResString[6] = 0x00;
			backResString[7] = Y_N_MSG2;
			
			respUsartMsg(backResString, length);
		}
		
		/* Z+ ��λ */
		if((keyValue>>4) & 0x0001)
		{
			//////////////////�رյ���ڸ÷����ϵ��˶� byYJY
			backResString[5] = Z_P_MSG1;
			backResString[6] = 0x00;
			backResString[7] = Z_P_MSG2;
			
			respUsartMsg(backResString, length);
		}
		
		/* Z- ��λ */
		if((keyValue>>5) & 0x0001)
		{
			//////////////////�رյ���ڸ÷����ϵ��˶� byYJY
			backResString[5] = Z_N_MSG1;
			backResString[6] = 0x00;
			backResString[7] = Z_N_MSG2;
			
			respUsartMsg(backResString, length);
		}
		
		/* A ��λ */
		if((keyValue>>6) & 0x0001)
		{
			//////////////////�رյ���ڸ÷����ϵ��˶� byYJY
			backResString[5] = A_MSG1;
			backResString[6] = 0x00;
			backResString[7] = A_MSG2;
			
			respUsartMsg(backResString, length);
		}
		
		/* B ��λ */
		if((keyValue>>7) & 0x0001)
		{
			//////////////////�رյ���ڸ÷����ϵ��˶� byYJY
			backResString[5] = B_MSG1;
			backResString[6] = 0x00;
			backResString[7] = B_MSG1;
			
			respUsartMsg(backResString, length);
		}		
	}	
}

// ��ʼ������
static void limitInit(GPIO_XX *GPIO_Temp, const char str[])
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	*GPIO_Temp = GPIO_XX_Init(str);
	
	RCC_AHB1PeriphClockCmd(GPIO_Temp->RCC_Periph_X, ENABLE);		// ʹ��ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Temp->GPIO_Pin_X;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  			//����
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  			//��������
	
	GPIO_Init(GPIO_Temp->GPIO_Base, &GPIO_InitStructure);
}

// ��ȡIO�������ƽ 0:�͵�ƽ��������λ 1���ߵ�ƽ��������
static uint8_t readOutput(GPIO_XX *GPIO_Temp)
{
	uint8_t bitStatus;
	bitStatus = GPIO_ReadInputDataBit(GPIO_Temp->GPIO_Base, GPIO_Temp->GPIO_Pin_X);
	
	return bitStatus;
}

// 0��������λ��1��û�д���
uint8_t X_P_Lim(void)
{
	return readOutput(&x_p_limit);
}
uint8_t X_N_Lim(void)
{
	return readOutput(&x_n_limit);
}
uint8_t Y_P_Lim(void)
{
	return readOutput(&y_p_limit);
}
uint8_t Y_N_Lim(void)
{
	return readOutput(&y_n_limit);
}
uint8_t Z_P_Lim(void)
{
	return readOutput(&z_p_limit);
}
uint8_t Z_N_Lim(void)
{
	return readOutput(&z_n_limit);
}
uint8_t A_Lim(void)
{
	return readOutput(&a_limit);
}
uint8_t B_Lim(void)
{
	return readOutput(&b_limit);
}

