#ifndef _LIMIT_IO_H
#define _LIMIT_IO_H
#include "sys.h"
#include "config.h"
#include "delay.h"
#include "ComDataProc.h"
#include "main.h"
/********************************************************************************************************
*    限位IO口的相关程序
********************************************************************************************************/

// IO口初始化函数
void limit_IO_Init(void);
// 初始化函数
static void limitInit(GPIO_Structure_XX *GPIO_Temp, const char str[]);

// 读取IO口输出电平
static uint8_t readOutput(GPIO_Structure_XX *GPIO_Temp);

// 0：触发限位，1：没有触发
uint8_t X_P_Lim(void);
uint8_t X_N_Lim(void);
uint8_t Y_P_Lim(void);
uint8_t Y_N_Lim(void);
uint8_t Z_P_Lim(void);
uint8_t Z_N_Lim(void);
uint8_t A_Lim(void);
uint8_t B_Lim(void);

// 查看是否有限位发生
// mode:0,不支持连续按;1,支持连续按;
static void LIMIT_Scan(void);				// u8 mode

// 循环查看是否有限位发生，并处理限位
void procLimit(void);

#endif 

