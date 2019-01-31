#ifndef _COMMAND_H
#define _COMMAND_H
/*  数据处理程序 */
#include "sys.h"
#include "config.h"
#include "main.h"

// 执行命令
void CMD_Execute(void);

// 将串口发来的数据复制到本地的结构体中 
static void usartData2cmd(void);

// 自检程序
void selfCheckFunc(void);

// 运动数据处理程序
void motionDataProc(void);



#endif
