#ifndef _KEY_H
#define _KEY_H
#include "sys.h"

/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          原能智能制造机器人
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : Key.h
* By  : Hiker Lee
* 2018-05-02
********************************************************************************************************/

// 原各轴限位IO口
//#define KEYXa	PFin(5)			
//#define KEYXb	PFin(6) 		
//#define KEYYa	PFin(7)			
//#define KEYYb	PFin(8) 		
//#define KEYZa	PFin(10)			
//#define KEYZb	PFin(13) 		
//#define KEYA	PFin(14)			
//#define KEYB	PFin(15) 

// 注意是否初始化IO口
#define KEYXa	PDin(11)			
#define KEYXb	PDin(12) 		
#define KEYYa	PCin(6)			
#define KEYYb	PCin(7) 		
#define KEYZa	PFin(13)			
#define KEYZb	PGin(1) 		
#define KEYA	PGin(2)			
#define KEYB	PGin(4)

#define KEYTool1	PGin(0) 		
#define KEYTool2	PGin(1) 		
#define KEYTool3	PGin(2) 		
#define KEYTool4	PGin(3) 		

#define KEYToolLimit	PGin(4) 		

#define KEYXa_PRES	1
#define KEYXb_PRES 	2
#define KEYYa_PRES	3
#define KEYYb_PRES	4
#define KEYZa_PRES	5
#define KEYZb_PRES 	6
#define KEYA_PRES	7
#define KEYB_PRES	8

#define KEYTool1_POS	9
#define KEYTool2_POS	10
#define KEYTool3_POS	11
#define KEYTool4_POS	12

#define KEYToolLimit_POS	13

#endif 
