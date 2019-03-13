#ifndef __CONFIG_H
#define __CONFIG_H
/* 该文件包括了一些常用的控制系统配置选项，自己写的代码都应该包括该头文件 */

#include "macro_operate.h"				// 宏操作


/********************************************************功能开关*************************************************************************/
/* 系统功能开关： 1--打开测试代码，0--关闭 */
// 测试用
#define 	PRIN2DISP 	1   			// 打印提示信息，而不是发回控制命令
// 开发板上的测试代码，方便在探索者板子上验证代码
#define 	_TEST_ON_ALIENTEK 	1


#define 	BOARD			1     	// 0：无板子，1：探索者开发板，2：旧的主控板，3：新的主控板
/*****************************************************************************************************************************************/






/*********************************************************系统配置************************************************************************/
/*  系统时钟 */
#define _SYSCLK	168								// 系统时钟168MHz



/* 间隙补偿：单位um */
#define 	BacklashCompensation				40		
/*****************************************************************************************************************************************/





/*********************************************************IO口配置************************************************************************/
////////////////////////////////////////////////////////探索者开发板//////////////////////////////////////////////////////////////
#if (1 == BOARD)			// 探索者开发板


/* 串口设置 USART1 USART2 USART3  */
#define 	USART_EN 		1						// 使能串口1
#define 	BOUND_X			115200

/* LED IO口 */
#define 	GPIO_LED				"PF9"

/* 限位 IO口 */
#define 	X_P_LIMIT 			"PE2"						// X+限位
#define 	X_N_LIMIT 			"PF6"						// X-限位 兼用为X零位
#define 	Y_P_LIMIT 			"PF7"						// Y+限位	"PF7"
#define 	Y_N_LIMIT 			"PA11"					// Y-限位 兼用为Y零位
#define 	Z_P_LIMIT 			"PF10"					// Z+限位
#define 	Z_N_LIMIT 			"PA11"					// Z-限位 兼用为Z零位
#define 	A_LIMIT 				"PA11"					// A零位
#define 	B_LIMIT 				"PA11"					// B零位

/* 刀具电磁铁IO口 */
#define		TOOLS_ELECTROMAGNET			"PF12"			// 电磁铁IO口

/* 主轴电机IO口 */
#define		EXTI_LINE_MAINMOTOR			4															// 中断线
#define		EXTI_MAINMOTOR_START		"PE4"													// 主轴电机启动IO口，带手动解码
#define 	PWM_MAINMOTOR_OUT				"PA7"			/* TIM14_CH1 */
#define 	PWM_MAINMOTOR_DIR				"XXX"

/******* 中断IO口  注意：如PA11只能连接到中断线11 ***********/
/* 步进电机的相关IO口  方向、中断及输出IO口 注：PWM的IO口不能随便选择！ */
#define		EXTI_LINE_X							3															// 中断线，与下文的中断IO口对应  如PC4->中断线4
#define 	EXTI_X_PLUS							"PB3" 		/* 中断线3 */				// 接收PWM的中断IO口
#define 	PWM_X_EXTI							"PB5"			/* TIM3_CH2 */			// 输出PWM到中断的IO口   	
#define 	PWM_X_OUT								"PB4"  		/* TIM3_CH1 */			// 输出PWM到电机的IO口		 探索者板子上用
#define 	DIR_X										"PA4" 		/*          */ 			// 电机运动方向

#if _TEST_ON_ALIENTEK
	#define		EXTI_LINE_Y						2															// 中断线，与下文的中断IO口对应  如PC4->中断线4
	#define 	EXTI_Y_PLUS						"PA2"			/* 中断线2  */			// 接收PWM的中断IO口  探索者板子上不能用中断线1，初始化时会被打断
#else
	#define		EXTI_LINE_Y						1															// 中断线，与下文的中断IOkou对应  如PC4->中断线4
	#define 	EXTI_Y_PLUS						"PA1"			/* 中断线1  */			// 接收PWM的中断IO口  
#endif
#define 	PWM_Y_EXTI							"PA3"			/* TIM2_CH4 */			// 输出PWM到中断的IO口   	
#define 	PWM_Y_OUT								"PA5"  		/* TIM2_CH1 */			// 输出PWM到电机的IO口
#define 	DIR_Y										"PC3" 		/*          */ 			// 电机运动方向

#define		EXTI_LINE_Z							6															// 中断线，与下文的中断IO口对应  如PC4->中断线4
#define 	EXTI_Z_PLUS							"PB6"			/* 中断线6 */				// 接收PWM的中断IO口
#define 	PWM_Z_EXTI							"PB8"			/* TIM4_CH3 */			// 输出PWM到中断的IO口
#define 	PWM_Z_OUT								"PB9"  		/* TIM4_CH4 */			// 输出PWM到电机的IO口
#define 	DIR_Z										"PB7" 		/*          */ 			// 电机运动方向

#define		EXTI_LINE_A							14															// 中断线，与下文的中断IO口对应  如PC4->中断线4
#define 	EXTI_A_PLUS							"XXX"			/* 中断线11 */			// 接收PWM的中断IO口
#define 	PWM_A_EXTI							"PE6"			/* TIM3_CH1 */			// 输出PWM到中断的IO口
#define 	PWM_A_OUT								"XXX"  		/* TIM3_CH1 */			// 输出PWM到电机的IO口
#define 	DIR_A										"XXX" 		/*          */ 			// 电机运动方向

#define		EXTI_LINE_B							15															// 中断线，与下文的中断IO口对应  如PC4->中断线4
#define 	EXTI_B_PLUS							"XXX"			/* 中断线11 */			// 接收PWM的中断IO口
#define 	PWM_B_EXTI							"PB14"		/* TIM3_CH1 */			// 输出PWM到中断的IO口
#define 	PWM_B_OUT								"XXX"  		/* TIM3_CH1 */			// 输出PWM到电机的IO口
#define 	DIR_B										"XXX" 		/*          */ 			// 电机运动方向

#define 	EXTI_LINE_UrgentStop		5					/* 中断线5  */		// 中断线，急停按钮的中断线
#define		EXTI_UrgentStop_IO			"PF5"												// 急停的中断IO口，对应探索者板子上的KEY_UP			

// 测试按钮中断 可以删除
#define 	_KEY1 						"XXX" 		// 按下后拉高
//#define 	_KEY1 						"PE3" 		// 按下后拉低

/******* 中断IO口  注意：如PA11只能连接到中断线11 ***********/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////旧的主控板////////////////////////////////////////////////////////////////
#elif (2 == BOARD)

/* 串口设置 USART1 USART2 USART3  */
#define 	USART_EN 			3						 			// 使能串口3
#define 	BOUND_X				115200

/* LED IO口 */
#define 	GPIO_LED								"PD6"

/* 限位 IO口 */
#define 	X_P_LIMIT 							"PF5"						// X+限位
#define 	X_N_LIMIT 							"PF6"						// X-限位 兼用为X零位
#define 	Y_P_LIMIT 							"PF7"						// Y+限位
#define 	Y_N_LIMIT 							"PF8"						// Y-限位 兼用为Y零位
#define 	Z_P_LIMIT 							"PF10"					// Z+限位
#define 	Z_N_LIMIT 							"PF13"					// Z-限位 兼用为Z零位
#define 	A_LIMIT 								"PF14"					// A零位
#define 	B_LIMIT 								"PF15"					// B零位

/* 刀具电磁铁IO口 */
#define		TOOLS_ELECTROMAGNET			"PB12"					// 电磁铁IO口

/* 主轴电机IO口 */
#define		EXTI_LINE_MAINMOTOR			6															// 中断线
#define		EXTI_MAINMOTOR_START		"PG6"													// 主轴电机启动IO口，带手动解码
#define 	PWM_MAINMOTOR_OUT				"PF9"			/* TIM14_CH1 */
//#define 	PWM_MAINMOTOR_DIR				"XXX"

/******* 中断IO口  注意：如PA11只能连接到中断线11 ***********/
/* 步进电机的相关IO口  方向、中断及输出IO口 注：PWM的IO口不能随便选择！ */
#define		EXTI_LINE_X							1															// 中断线，与下文的中断IO口对应  如PC4->中断线4
#define 	EXTI_X_PLUS							"PB1" 		/* 中断线1 */				// 接收PWM的中断IO口
#define 	PWM_X_EXTI							"PB5"			/* TIM3_CH2 */			// 输出PWM到中断的IO口  	
#define 	PWM_X_OUT								"PA6"  		/* TIM3_CH1 */			// 输出PWM到电机的IO口		 探索者板子上用
#define 	DIR_X										"PF0" 		/*          */ 			// 电机运动方向

#define		EXTI_LINE_Y							0															// 中断线，与下文的中断IO口对应  如PC4->中断线4
#define 	EXTI_Y_PLUS							"PB0"			/* 中断线0  */			// 接收PWM的中断IO口  探索者板子上不能用中断线1，初始化时会被打断
#define 	PWM_Y_EXTI							"PA3"			/* TIM2_CH4 */			// 输出PWM到中断的IO口   	
#define 	PWM_Y_OUT								"PA5"  		/* TIM2_CH1 */			// 输出PWM到电机的IO口
#define 	DIR_Y										"PF1" 		/*          */ 			// 电机运动方向

#define		EXTI_LINE_Z							2															// 中断线，与下文的中断IO口对应  如PC4->中断线4
#define 	EXTI_Z_PLUS							"PE2"			/* 中断线2 */				// 接收PWM的中断IO口
#define 	PWM_Z_EXTI							"PB8"			/* TIM4_CH3 */			// 输出PWM到中断的IO口
#define 	PWM_Z_OUT								"PB9"  		/* TIM4_CH4 */			// 输出PWM到电机的IO口
#define 	DIR_Z										"PF2" 		/*          */ 			// 电机运动方向

#define		EXTI_LINE_A							4															// 中断线，与下文的中断IO口对应  如PC4->中断线4
#define 	EXTI_A_PLUS							"PE4"			/* 中断线4  */			// 接收PWM的中断IO口
#define 	PWM_A_EXTI							"PE6"			/* TIM9_CH2 */			// 输出PWM到中断的IO口
#define 	PWM_A_OUT								"PE5"  		/* TIM9_CH1 */			// 输出PWM到电机的IO口
#define 	DIR_A										"PF3" 		/*          */ 			// 电机运动方向

#define		EXTI_LINE_B							3															// 中断线，与下文的中断IO口对应  如PC4->中断线4
#define 	EXTI_B_PLUS							"PE3"			/* 中断线3  */			// 接收PWM的中断IO口
#define 	PWM_B_EXTI							"PB14"		/* TIM12_CH1*/			// 输出PWM到中断的IO口
#define 	PWM_B_OUT								"PB15"  	/* TIM12_CH2*/			// 输出PWM到电机的IO口
#define 	DIR_B										"PF4" 		/*          */ 			// 电机运动方向

#define 	EXTI_LINE_UrgentStop		5					/* 中断线5  */			// 中断线，急停按钮的中断线
#define		EXTI_UrgentStop_IO			"PG5"													// 急停的中断IO口，对应探索者板子上的KEY_UP			

/******* 中断IO口  注意：如PA11只能连接到中断线11 ***********/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////新的主控板////////////////////////////////////////////////////////////////
#elif (3 == BOARD)

/* 串口设置 USART1 USART2 USART3  */
#define 	USART_EN 			3						 			// 使能串口3
#define 	BOUND_X				9600

/* LED IO口 */
#define 	GPIO_LED								"PF14"

/* 限位 IO口 */
#define 	X_P_LIMIT 							"PD11"						// X+限位
#define 	X_N_LIMIT 							"PD12"						// X-限位 兼用为X零位
#define 	Y_P_LIMIT 							"PC6"						// Y+限位
#define 	Y_N_LIMIT 							"PC7"						// Y-限位 兼用为Y零位
#define 	Z_P_LIMIT 							"PF13"					// Z+限位
#define 	Z_N_LIMIT 							"PG1"					// Z-限位 兼用为Z零位
#define 	A_LIMIT 								"PG2"					// A零位
#define 	B_LIMIT 								"PG4"					// B零位

/* 刀具电磁铁IO口 */
#define		TOOLS_ELECTROMAGNET			"PB12"					// 电磁铁IO口

/* 主轴电机IO口 */
#define		EXTI_LINE_MAINMOTOR			6															// 中断线
#define		EXTI_MAINMOTOR_START		"PG6"													// 主轴电机启动IO口，带手动解码
#define 	PWM_MAINMOTOR_OUT				"PF9"			/* TIM14_CH1 */
//#define 	PWM_MAINMOTOR_DIR				"XXX"

/******* 中断IO口  注意：如PA11只能连接到中断线11 ***********/
/* 步进电机的相关IO口  方向、中断及输出IO口 注：PWM的IO口不能随便选择！ */
#define		EXTI_LINE_X							3															// 中断线，与下文的中断IO口对应  如PC4->中断线4
#define 	EXTI_X_PLUS							"PE3" 		/* 中断线3 */				// 接收PWM的中断IO口
#define 	PWM_X_EXTI							"PB5"			/* TIM3_CH2 */			// 输出PWM到中断的IO口  	
#define 	PWM_X_OUT								"PA6"  		/* TIM3_CH1 */			// 输出PWM到电机的IO口		 探索者板子上用
#define 	DIR_X										"PF0" 		/*          */ 			// 电机运动方向

#define		EXTI_LINE_Y							0															// 中断线，与下文的中断IO口对应  如PC4->中断线4
#define 	EXTI_Y_PLUS							"PB0"			/* 中断线0  */			// 接收PWM的中断IO口  探索者板子上不能用中断线1，初始化时会被打断
#define 	PWM_Y_EXTI							"PA3"			/* TIM2_CH4 */			// 输出PWM到中断的IO口   	
#define 	PWM_Y_OUT								"PA5"  		/* TIM2_CH1 */			// 输出PWM到电机的IO口
#define 	DIR_Y										"PF1" 		/*          */ 			// 电机运动方向

#define		EXTI_LINE_Z							2															// 中断线，与下文的中断IO口对应  如PC4->中断线4
#define 	EXTI_Z_PLUS							"PE2"			/* 中断线2 */				// 接收PWM的中断IO口
#define 	PWM_Z_EXTI							"PB8"			/* TIM4_CH3 */			// 输出PWM到中断的IO口
#define 	PWM_Z_OUT								"PB9"  		/* TIM4_CH4 */			// 输出PWM到电机的IO口
#define 	DIR_Z										"PF2" 		/*          */ 			// 电机运动方向

#define		EXTI_LINE_A							4															// 中断线，与下文的中断IO口对应  如PC4->中断线4
#define 	EXTI_A_PLUS							"PE4"			/* 中断线4  */			// 接收PWM的中断IO口
#define 	PWM_A_EXTI							"PE6"			/* TIM9_CH2 */			// 输出PWM到中断的IO口
#define 	PWM_A_OUT								"PE5"  		/* TIM9_CH1 */			// 输出PWM到电机的IO口
#define 	DIR_A										"PF3" 		/*          */ 			// 电机运动方向

#define		EXTI_LINE_B							1															// 中断线，与下文的中断IO口对应  如PC4->中断线4
#define 	EXTI_B_PLUS							"PB1"			/* 中断线3  */			// 接收PWM的中断IO口
#define 	PWM_B_EXTI							"PB14"		/* TIM12_CH1*/			// 输出PWM到中断的IO口
#define 	PWM_B_OUT								"PB15"  	/* TIM12_CH2*/			// 输出PWM到电机的IO口
#define 	DIR_B										"PF4" 		/*          */ 			// 电机运动方向

#define 	EXTI_LINE_UrgentStop		5					/* 中断线5  */			// 中断线，急停按钮的中断线
#define		EXTI_UrgentStop_IO			"PG5"													// 急停的中断IO口，对应探索者板子上的KEY_UP			

/******* 中断IO口  注意：如PA11只能连接到中断线11 ***********/
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
/*****************************************************************************************************************************************/









/*********************************************************通讯协议*************************************************************************/
/*********串口通信协议**********/
#define FrameB1 		0x0E     //数据帧头
#define FrameB2			0x09     //数据帧头
#define FrameEnd		0xFF     //数据帧尾

/*********串口通信数据帧控制字**********/
#define SELFCHECK 		0x0B     	// 自检
#define CONTROLINFO		0x0C     	// 控制
#define DATAINFO			0x0D     	// 数据
#define STATUSINFO		0x0E     	// 状态
#define INVALID_CMD 	0x00 			// 无效命令，默认状态或没有新命令

/* 	0x0D // 数据 : 对应指令码	 */
#define PLUS_DATA				0x22			// 脉冲数据处理
#define OFFLINE_DATA		0x25			// 脱机加工指令码

/*  0x0E // 状态 : 对应指令码	 */
#define 	HARDWARE_URGENTSTOP				0x3B			// 硬件急停
#define 	HARDWARE_URGENTCANCEL			0x41			// 硬件急停

/*  0x0C // 控制 : 对应指令码	 */
#define 	MAINAXIS_WORK							0x08			// 主轴工作
#define 	MAINAXIS_STOP							0x01			// 主轴停止
#define		CHANGE_TOOL1							0x04			// 换刀1
#define		CHANGE_TOOL2							0x05			// 换刀2
#define		CHANGE_TOOL3							0x06			// 换刀3
#define		CHANGE_TOOL4							0x07			// 换刀4
#define		CLEAN_TOOLS								0x010			// 脱刀
#define		CALIBRATE_TOOL_POS				0x014			// 校准刀具位置
/*****************************************************************************************************************************************/







/********************************************************其它****************************************************************************/
/* 运动系统的运动轴数量  */
#define AXIS_NUM 		5     	// 5轴系统 顺序：X、Y、Z、A、B
/*****************************************************************************************************************************************/





// 尺寸链包含有问题，暂时未解决，先放在这
typedef enum {TBD_DIR = 0, POS_DIR = 1, NEG_DIR = 2} Motor_Dir; 		// 电机运动方向


#endif
