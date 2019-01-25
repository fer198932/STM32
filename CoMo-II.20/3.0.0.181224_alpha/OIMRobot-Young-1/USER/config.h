#ifndef __CONFIG_H
#define __CONFIG_H
/* 该文件包括了一些常用的控制系统配置选项，自己写的代码都应该包括该头文件 */

#include "macro_operate.h"				// 宏操作

/*  系统时钟 */
#define _SYSCLK	168								// 系统时钟168MHz

/* 串口设置 USART1 USART2 USART3  */
#define 	EN_USART1 	1						// 如使能串口2： EN_USART2 	1
#define 	BOUND_X			115200

/* LED IO口 */
#define 	GPIO_LED		"PF9"

/* 限位 IO口 */
#define X_P_LIMIT 		"PE2"					// X+限位
#define X_N_LIMIT 		"PE3"					// X-限位 兼用为X零位
#define Y_P_LIMIT 		"PE4"					// Y+限位
#define Y_N_LIMIT 		"PE5"					// Y-限位 兼用为Y零位
#define Z_P_LIMIT 		"PE6"					// Z+限位
#define Z_N_LIMIT 		"PE7"					// Z-限位 兼用为Z零位
#define A_LIMIT 			"PE8"					// A零位
#define B_LIMIT 			"PE9"					// B零位




/*********串口通信协议**********/
#define FrameB1 		0x0E     //数据帧头
#define FrameB2			0x09     //数据帧头
#define FrameEnd		0xFF     //数据帧尾

/*********串口通信数据帧控制字**********/
#define SELFCHECK 		0x0B     //自检
#define CONTROLINFO		0x0C     //控制
#define DATAINFO		0x0D     //数据
#define STATUSINFO		0x0E     //状态

/* 限位回复信息 */
#define X_P_MSG1  	0x31			// X+ 限位
#define X_P_MSG2  	0x01			// X+ 限位

#define X_N_MSG1  	0x31			// X- 限位
#define X_N_MSG2  	0x02			// X- 限位

#define Y_P_MSG1  	0x32			// Y+ 限位
#define Y_P_MSG2  	0x01			// Y+ 限位

#define Y_N_MSG1  	0x32			// Y- 限位
#define Y_N_MSG2  	0x02			// Y- 限位

#define Z_P_MSG1  	0x33			// Z+ 限位
#define Z_P_MSG2  	0x01			// Z+ 限位

#define Z_N_MSG1  	0x33			// Z- 限位
#define Z_N_MSG2  	0x02			// Z- 限位

#define A_MSG1  		0x35			// A 零位
#define A_MSG2  		0x03			// A 零位

#define B_MSG1  		0x36			// B 零位
#define B_MSG2  		0x03			// B 零位



#endif
