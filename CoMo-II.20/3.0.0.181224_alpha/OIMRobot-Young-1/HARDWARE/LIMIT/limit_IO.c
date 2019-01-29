/********************************************************************************************************
*    限位IO口的相关程序
********************************************************************************************************/
#include "limit_IO.h"

static const u16 length = 9;

static u8 backResString[length]; 						// 回复的字符串

static u32 keyValue = 0;
static u32 keyValueOld = 0;

// 限位点IO口
static GPIO_Structure_XX x_p_limit;
static GPIO_Structure_XX x_n_limit;
static GPIO_Structure_XX y_p_limit;
static GPIO_Structure_XX y_n_limit;
static GPIO_Structure_XX z_p_limit;
static GPIO_Structure_XX z_n_limit;
static GPIO_Structure_XX a_limit;
static GPIO_Structure_XX b_limit;

// IO口初始化函数
void limit_IO_Init(void)
{
	// 初始化各限位点的IO口
	limitInit(&x_p_limit, X_P_LIMIT);
	limitInit(&x_n_limit, X_N_LIMIT);
	limitInit(&y_p_limit, Y_P_LIMIT);
	limitInit(&y_n_limit, Y_N_LIMIT);
	limitInit(&z_p_limit, Z_P_LIMIT);
	limitInit(&z_n_limit, Z_N_LIMIT);
	limitInit(&a_limit, A_LIMIT);
	limitInit(&b_limit, B_LIMIT);	
}

// 查看是否有限位发生
// mode:0,不支持连续按;1,支持连续按;
static void LIMIT_Scan(void)
{
	if((0==X_P_Lim()) || (0==X_N_Lim()) || (0==Y_P_Lim()) || (0==Y_N_Lim()) || 
			(0==Z_P_Lim()) || (0==Z_N_Lim()) || (0==A_Lim()) || (0==B_Lim()))				//  有按键按下
	{
//		delay_us(10);				// 按键去抖
		delay_ms(10);					// 按键去抖
		
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
					(1==Z_P_Lim()) && (1==Z_N_Lim()) && (1==A_Lim()) && (1==B_Lim()))		// 无按键按下
	{
		keyValue = 0;
	}
}

// 循环查看是否有限位发生，并处理限位
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
			
			backResString[3] = length;					// 长度
			backResString[4] = 0x00;
			
			backResString[8] = FrameEnd;
			
			/////////////注意标志位的设置//////////////////////// byYJY
			
		}
		
		/* X+ 限位 */
		if((keyValue>>0) & 0x0001)
		{
			//////////////////关闭电机在该方向上的运动 byYJY
			backResString[5] = X_P_MSG1;
			backResString[6] = 0x00;
			backResString[7] = X_P_MSG2;
			
#if PRIN2DISP
			// 串口回复
			backResString[0] = backResString[0]; 		// 避免warning
			respUsartMsg("LIMITx+\r\n", length);
#else
			respUsartMsg(backResString, length);
#endif			
		}
		
		/* X- 限位 */
		if((keyValue>>1) & 0x0001)
		{
			//////////////////关闭电机在该方向上的运动 byYJY
			backResString[5] = X_N_MSG1;
			backResString[6] = 0x00;
			backResString[7] = X_N_MSG2;
			
#if PRIN2DISP
			// 串口回复
			backResString[0] = backResString[0]; 		// 避免warning
			respUsartMsg("LIMITx-\r\n", length);
#else
			respUsartMsg(backResString, length);
#endif		
		}
		
		/* Y+ 限位 */
		if((keyValue>>2) & 0x0001)
		{
			//////////////////关闭电机在该方向上的运动 byYJY
			backResString[5] = Y_P_MSG1;
			backResString[6] = 0x00;
			backResString[7] = Y_P_MSG2;
			
#if PRIN2DISP
			// 串口回复
			backResString[0] = backResString[0]; 		// 避免warning
			respUsartMsg("LIMITy+\r\n", length);
#else
			respUsartMsg(backResString, length);
#endif		
		}
		
		/* Y- 限位 */
		if((keyValue>>3) & 0x0001)
		{
			//////////////////关闭电机在该方向上的运动 byYJY
			backResString[5] = Y_N_MSG1;
			backResString[6] = 0x00;
			backResString[7] = Y_N_MSG2;
			
#if PRIN2DISP
			// 串口回复
			backResString[0] = backResString[0]; 		// 避免warning
			respUsartMsg("LIMITy-\r\n", length);
#else
			respUsartMsg(backResString, length);
#endif		
		}
		
		/* Z+ 限位 */
		if((keyValue>>4) & 0x0001)
		{
			//////////////////关闭电机在该方向上的运动 byYJY
			backResString[5] = Z_P_MSG1;
			backResString[6] = 0x00;
			backResString[7] = Z_P_MSG2;
			
#if PRIN2DISP
			// 串口回复
			backResString[0] = backResString[0]; 		// 避免warning
			respUsartMsg("LIMITz+\r\n", length);
#else
			respUsartMsg(backResString, length);
#endif		
		}
		
		/* Z- 限位 */
		if((keyValue>>5) & 0x0001)
		{
			//////////////////关闭电机在该方向上的运动 byYJY
			backResString[5] = Z_N_MSG1;
			backResString[6] = 0x00;
			backResString[7] = Z_N_MSG2;
			
#if PRIN2DISP
			// 串口回复
			backResString[0] = backResString[0]; 		// 避免warning
			respUsartMsg("LIMITz-\r\n", length);
#else
			respUsartMsg(backResString, length);
#endif		
		}
		
		/* A 零位 */
		if((keyValue>>6) & 0x0001)
		{
			//////////////////关闭电机在该方向上的运动 byYJY
			backResString[5] = A_MSG1;
			backResString[6] = 0x00;
			backResString[7] = A_MSG2;
			
#if PRIN2DISP
			// 串口回复
			backResString[0] = backResString[0]; 		// 避免warning
			respUsartMsg("LIMITa0\r\n", length);
#else
			respUsartMsg(backResString, length);
#endif		
		}
		
		/* B 零位 */
		if((keyValue>>7) & 0x0001)
		{
			//////////////////关闭电机在该方向上的运动 byYJY
			backResString[5] = B_MSG1;
			backResString[6] = 0x00;
			backResString[7] = B_MSG1;	

#if PRIN2DISP
			// 串口回复
			backResString[0] = backResString[0]; 		// 避免warning
			respUsartMsg("LIMITb0\r\n", length);
#else
			respUsartMsg(backResString, length);
#endif		
		}		
	}	
}

// 初始化函数
static void limitInit(GPIO_Structure_XX *GPIO_Temp, const char str[])
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_Structure_Init(str, GPIO_Temp);
	
	RCC_AHB1PeriphClockCmd(GPIO_Temp->RCC_Periph_N, ENABLE);		// 使能时钟
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Temp->GPIO_Pin_N;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;  			//输入
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//100M
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  			//上拉输入
	
	GPIO_Init(GPIO_Temp->GPIO_Port, &GPIO_InitStructure);
}

// 读取IO口输出电平 0:低电平，触发限位 1：高电平，不触发
static uint8_t readOutput(GPIO_Structure_XX *GPIO_Temp)
{
	uint8_t bitStatus;
	bitStatus = GPIO_ReadInputDataBit(GPIO_Temp->GPIO_Port, GPIO_Temp->GPIO_Pin_N);
	
	return bitStatus;
}

// 0：触发限位，1：没有触发
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

