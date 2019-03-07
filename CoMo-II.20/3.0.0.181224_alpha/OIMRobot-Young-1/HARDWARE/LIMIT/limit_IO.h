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

#define 	SELFCHECK_OK					0x01		// 自检正常
#define 	SELFCHECK_URGENTSTOP 	0x03		// 急停按钮的值


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

uint8_t UrgentStop_Lim(void);

uint8_t MainMotor_Start(void);


// 查看是否有限位发生
// mode:0,不支持连续按;1,支持连续按;
static void LIMIT_Scan(void);				// u8 mode

// 循环查看是否有限位发生，并处理限位
void procLimit(void);

// 急停处理
void UrgentStop_Proc(u8 status);

// 自检的急停检查
u8 UrgentStopTest(void);

//  步进电机被限位的轴停止运动：限位触发时，将该轴的脉冲数、频率置零
void StepMotor_Limited(vu32 limitValue);






// 处理急停的函数 ，（注意：必须用宏）
#define UrgentStop_Proc_Macro			\
delay_ms(2);		\
urgentStopTemp1 = UrgentStop_Lim(); 			\
delay_ms(2);		\
urgentStopTemp2 = UrgentStop_Lim(); 			\
if(urgentStopTemp1 == urgentStopTemp2) 	\
{ 	\
	if(0 == urgentStopTemp1)	/* 急停按钮按下  */ \
	{	\
		flag_Struct.UrgentStop_Flag = UrgentStop_Down;	\
	}	\
	else	/* 急停按钮松开  */ \
	{	\
		flag_Struct.UrgentStop_Flag = UrgentStop_Up;	\
	}		\
	UrgentStop_Proc((u8)flag_Struct.UrgentStop_Flag);	\
	continue;		\
} else \
	flag_Struct.UrgentStop_Flag = UrgentStop_Init;	\












#endif 










