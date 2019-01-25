#ifndef __MACRO_OPERATE_H
#define __MACRO_OPERATE_H
#include "sys.h"
#include "ComDataProc.h"
/* 宏操作函数 */



typedef struct {
	uint16_t GPIO_Pin_X;							// pin脚
	GPIO_TypeDef *GPIO_Base;					// 端口号
	uint32_t RCC_Periph_X;						// 时钟线
//	uint32_t GPIOx_ODR_Addr;					// 输出地址映射
} GPIO_XX;

// 获得GPIO的pin脚
// #define GPIO_Pin_X(GPIO_Pin) 		GPIO_Pin_9

// 根据字符串构造初始化的GPIO结构体
GPIO_XX GPIO_XX_Init(const char str[]);


#endif
