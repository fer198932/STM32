/********************************************************************************************************
*   通过宏定义构造初始化用的结构体
********************************************************************************************************/
#include "macro_operate.h"

// 根据字符串构造初始化的GPIO结构体
GPIO_XX GPIO_XX_Init(const char str[])
{
	GPIO_XX GPIO_Temp;
	
	if('P' != str[0])
	{
		respMsgError(1);
		return GPIO_Temp;
	}
	
	switch(str[1])
	{
		case 'E':
			GPIO_Temp.GPIO_Base = GPIOE;
			GPIO_Temp.RCC_Periph_X = RCC_AHB1Periph_GPIOE;
			break;
		
		case 'F':
			GPIO_Temp.GPIO_Base = GPIOF;
			GPIO_Temp.RCC_Periph_X = RCC_AHB1Periph_GPIOF;
			break;
		
		default:
			respMsgError(1);
			break;
		
	}
	
	switch(str[2])
	{		
		case '1':
			if('\0' == str[3])
				GPIO_Temp.GPIO_Pin_X = GPIO_Pin_1;
			else
			{
				switch(str[3])
				{
					case '0':
						GPIO_Temp.GPIO_Pin_X = GPIO_Pin_10;
						break;
					
					default:
						respMsgError(1);
						break;
					
				}
				
			}
			break;
			
		case '2':
			GPIO_Temp.GPIO_Pin_X = GPIO_Pin_2;
			break;
		
		case '3':
			GPIO_Temp.GPIO_Pin_X = GPIO_Pin_3;
			break;
		
		case '4':
			GPIO_Temp.GPIO_Pin_X = GPIO_Pin_4;
			break;
			
		case '9':
			GPIO_Temp.GPIO_Pin_X = GPIO_Pin_9;
			break;
		
		default:
			respMsgError(1);
			break;		
	}
	
	
	return GPIO_Temp;
}





