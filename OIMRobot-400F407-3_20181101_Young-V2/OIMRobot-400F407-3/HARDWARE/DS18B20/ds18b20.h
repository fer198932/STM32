#ifndef __DS18B20_H
#define __DS18B20_H 
#include "sys.h"   
/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          ԭ���������������
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : Ds18b20.h
* By  : Hiker Lee
* 2018-06-14
* 0614����18B20��1��PD11�����¶ȣ�2��PD12�������¶ȣ�

********************************************************************************************************/

//IO��������
#define DS18B20_IO_IN1()  {GPIOD->MODER&=~(3<<(11*2));GPIOD->MODER|=0<<11*2;}	//PD11����ģʽ
#define DS18B20_IO_OUT1() {GPIOD->MODER&=~(3<<(11*2));GPIOD->MODER|=1<<11*2;} 	//PD11���ģʽ

#define DS18B20_IO_IN2()  {GPIOD->MODER&=~(3<<(12*2));GPIOD->MODER|=0<<12*2;}	//PD11����ģʽ
#define DS18B20_IO_OUT2() {GPIOD->MODER&=~(3<<(12*2));GPIOD->MODER|=1<<12*2;} 	//PD11���ģʽ
 
////IO��������											   
#define	DS18B20_DQ_OUT1 PDout(11) 			//���ݶ˿�	PD11
#define	DS18B20_DQ_IN1  PDin(11)  			//���ݶ˿�	PD11 

#define	DS18B20_DQ_OUT2 PDout(12) 			//���ݶ˿�	PD12
#define	DS18B20_DQ_IN2  PDin(12)  			//���ݶ˿�	PD12 

   	
u8 DS18B20_Init1(void);					//��ʼ��DS18B20
short DS18B20_Get_Temp1(void);			//��ȡ�¶�
void DS18B20_Start1(void);				//��ʼ�¶�ת��
void DS18B20_Write_Byte1(u8 dat);		//д��һ���ֽ�
u8 DS18B20_Read_Byte1(void);				//����һ���ֽ�
u8 DS18B20_Read_Bit1(void);				//����һ��λ
u8 DS18B20_Check1(void);					//����Ƿ����DS18B20
void DS18B20_Rst1(void);					//��λDS18B20    

u8 DS18B20_Init2(void);					//��ʼ��DS18B20
short DS18B20_Get_Temp2(void);			//��ȡ�¶�
void DS18B20_Start2(void);				//��ʼ�¶�ת��
void DS18B20_Write_Byte2(u8 dat);		//д��һ���ֽ�
u8 DS18B20_Read_Byte2(void);				//����һ���ֽ�
u8 DS18B20_Read_Bit2(void);				//����һ��λ
u8 DS18B20_Check2(void);					//����Ƿ����DS18B20
void DS18B20_Rst2(void);					//��λDS18B20    

#endif















