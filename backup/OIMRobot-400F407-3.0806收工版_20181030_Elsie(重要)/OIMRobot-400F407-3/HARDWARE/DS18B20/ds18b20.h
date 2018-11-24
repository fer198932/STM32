#ifndef __DS18B20_H
#define __DS18B20_H 
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
* File : Ds18b20.h
* By  : Hiker Lee
* 2018-06-14
* 0614增加18B20；1、PD11主轴温度；2、PD12保护罩温度；

********************************************************************************************************/

//IO方向设置
#define DS18B20_IO_IN1()  {GPIOD->MODER&=~(3<<(11*2));GPIOD->MODER|=0<<11*2;}	//PD11输入模式
#define DS18B20_IO_OUT1() {GPIOD->MODER&=~(3<<(11*2));GPIOD->MODER|=1<<11*2;} 	//PD11输出模式

#define DS18B20_IO_IN2()  {GPIOD->MODER&=~(3<<(12*2));GPIOD->MODER|=0<<12*2;}	//PD11输入模式
#define DS18B20_IO_OUT2() {GPIOD->MODER&=~(3<<(12*2));GPIOD->MODER|=1<<12*2;} 	//PD11输出模式
 
////IO操作函数											   
#define	DS18B20_DQ_OUT1 PDout(11) 			//数据端口	PD11
#define	DS18B20_DQ_IN1  PDin(11)  			//数据端口	PD11 

#define	DS18B20_DQ_OUT2 PDout(12) 			//数据端口	PD12
#define	DS18B20_DQ_IN2  PDin(12)  			//数据端口	PD12 

   	
u8 DS18B20_Init1(void);					//初始化DS18B20
short DS18B20_Get_Temp1(void);			//获取温度
void DS18B20_Start1(void);				//开始温度转换
void DS18B20_Write_Byte1(u8 dat);		//写入一个字节
u8 DS18B20_Read_Byte1(void);				//读出一个字节
u8 DS18B20_Read_Bit1(void);				//读出一个位
u8 DS18B20_Check1(void);					//检测是否存在DS18B20
void DS18B20_Rst1(void);					//复位DS18B20    

u8 DS18B20_Init2(void);					//初始化DS18B20
short DS18B20_Get_Temp2(void);			//获取温度
void DS18B20_Start2(void);				//开始温度转换
void DS18B20_Write_Byte2(u8 dat);		//写入一个字节
u8 DS18B20_Read_Byte2(void);				//读出一个字节
u8 DS18B20_Read_Bit2(void);				//读出一个位
u8 DS18B20_Check2(void);					//检测是否存在DS18B20
void DS18B20_Rst2(void);					//复位DS18B20    

#endif















