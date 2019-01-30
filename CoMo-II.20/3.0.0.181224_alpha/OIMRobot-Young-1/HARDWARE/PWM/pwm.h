#ifndef _PWM_H
#define _PWM_H
#include "sys.h"
#include "timer.h"

//////////////////////////////////////////////////////////////////////////////////	 
//  PWM生成程序  2019年1月29日20:16:48 byYJY							  
////////////////////////////////////////////////////////////////////////////////// 	




// PWM初始化函数
void PWM_Init(void);


// 定时器输出PIN脚初始化
static void GPIO_PWM_Init(GPIO_Structure_XX *GPIO_Temp, const char Pin_PWM_str[]);


#endif
