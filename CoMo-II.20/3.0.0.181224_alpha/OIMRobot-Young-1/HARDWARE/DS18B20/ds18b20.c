/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          原能智能制造机器人
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : Ds18b20.c
* By  : Hiker Lee
* 2018-06-14
* 0614增加18B20；1、PD11主轴温度；2、PD12保护罩温度；
********************************************************************************************************/

#include "ds18b20.h"
#include "delay.h"	


//复位DS18B20
void DS18B20_Rst1(void)	   
{                 
	DS18B20_IO_OUT1(); //SET PG11 OUTPUT
  	DS18B20_DQ_OUT1=0; //拉低DQ
  	delay_us(750);    //拉低750us
  	DS18B20_DQ_OUT1=1; //DQ=1 
	delay_us(15);     //15US
}

void DS18B20_Rst2(void)	   
{                 
	DS18B20_IO_OUT2(); //SET PG11 OUTPUT
  	DS18B20_DQ_OUT2=0; //拉低DQ
  	delay_us(750);    //拉低750us
  	DS18B20_DQ_OUT2=1; //DQ=1 
	delay_us(15);     //15US
}

//等待DS18B20的回应
//返回1:未检测到DS18B20的存在
//返回0:存在
u8 DS18B20_Check1(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN1();		//SET PG11 INPUT	 
    while (DS18B20_DQ_IN1&&retry<200)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!DS18B20_DQ_IN1&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}
u8 DS18B20_Check2(void) 	   
{   
	u8 retry=0;
	DS18B20_IO_IN2();		//SET PG12 INPUT	 
    while (DS18B20_DQ_IN2&&retry<200)
	{
		retry++;
		delay_us(1);
	};	 
	if(retry>=200)return 1;
	else retry=0;
    while (!DS18B20_DQ_IN2&&retry<240)
	{
		retry++;
		delay_us(1);
	};
	if(retry>=240)return 1;	    
	return 0;
}

//从DS18B20读取一个位
//返回值：1/0
u8 DS18B20_Read_Bit1(void) 			 // read one bit
{
  	u8 data;
	DS18B20_IO_OUT1();//SET PG11 OUTPUT
  	DS18B20_DQ_OUT1=0; 
	delay_us(2);
  	DS18B20_DQ_OUT1=1; 
	DS18B20_IO_IN1();//SET PG11 INPUT
	delay_us(12);
	if(DS18B20_DQ_IN1)data=1;
  	else data=0;	 
  	delay_us(50);           
  	return data;
}
u8 DS18B20_Read_Bit2(void) 			 // read one bit
{
  	u8 data;
	DS18B20_IO_OUT2();//SET PG11 OUTPUT
  	DS18B20_DQ_OUT2=0; 
	delay_us(2);
  	DS18B20_DQ_OUT2=1; 
	DS18B20_IO_IN2();//SET PG11 INPUT
	delay_us(12);
	if(DS18B20_DQ_IN2)data=1;
  	else data=0;	 
  	delay_us(50);           
  	return data;
}

//从DS18B20读取一个字节
//返回值：读到的数据
u8 DS18B20_Read_Byte1(void)    // read one byte
{        
    u8 i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit1();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}
u8 DS18B20_Read_Byte2(void)    // read one byte
{        
    u8 i,j,dat;
    dat=0;
	for (i=1;i<=8;i++) 
	{
        j=DS18B20_Read_Bit2();
        dat=(j<<7)|(dat>>1);
    }						    
    return dat;
}

//写一个字节到DS18B20
//dat：要写入的字节
void DS18B20_Write_Byte1(u8 dat)     
 {             
    u8 j;
    u8 testb;
	  DS18B20_IO_OUT1();//SET PG11 OUTPUT;
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_DQ_OUT1=0;// Write 1
            delay_us(2);                            
            DS18B20_DQ_OUT1=1;
            delay_us(60);             
        }
        else 
        {
            DS18B20_DQ_OUT1=0;// Write 0
            delay_us(60);             
            DS18B20_DQ_OUT1=1;
            delay_us(2);                          
        }
    }
}
void DS18B20_Write_Byte2(u8 dat)     
 {             
    u8 j;
    u8 testb;
	DS18B20_IO_OUT2();//SET PG11 OUTPUT;
    for (j=1;j<=8;j++) 
	{
        testb=dat&0x01;
        dat=dat>>1;
        if (testb) 
        {
            DS18B20_DQ_OUT2=0;// Write 1
            delay_us(2);                            
            DS18B20_DQ_OUT2=1;
            delay_us(60);             
        }
        else 
        {
            DS18B20_DQ_OUT2=0;// Write 0
            delay_us(60);             
            DS18B20_DQ_OUT2=1;
            delay_us(2);                          
        }
    }
}

//开始温度转换
void DS18B20_Start1(void)// ds1820 start convert
{   						               
    DS18B20_Rst1();	   
	DS18B20_Check1();	 
    DS18B20_Write_Byte1(0xcc);// skip rom
    DS18B20_Write_Byte1(0x44);// convert
} 
void DS18B20_Start2(void)// ds1820 start convert
{   						               
    DS18B20_Rst2();	   
	DS18B20_Check2();	 
    DS18B20_Write_Byte2(0xcc);// skip rom
    DS18B20_Write_Byte2(0x44);// convert
} 

//初始化DS18B20的IO口 DQ 同时检测DS的存在
//返回1:不存在
//返回0:存在    	 
u8 DS18B20_Init1(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOG时钟

  //GPIOG9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
 
 	DS18B20_Rst1();
	return DS18B20_Check1();
}  
u8 DS18B20_Init2(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//使能GPIOG时钟

  //GPIOG9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化
 
 	DS18B20_Rst2();
	return DS18B20_Check2();
}  

//从ds18b20得到温度值
//精度：0.1C
//返回值：温度值 （-550~1250） 
short DS18B20_Get_Temp1(void)
{
    u8 temp;
    u8 TL,TH;
	short tem;
	
    DS18B20_Start1();                    // ds1820 start convert
    DS18B20_Rst1();
    DS18B20_Check1();	 
    DS18B20_Write_Byte1(0xcc);// skip rom
    DS18B20_Write_Byte1(0xbe);// convert	    
    TL=DS18B20_Read_Byte1(); // LSB   
    TH=DS18B20_Read_Byte1(); // MSB   
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;//温度为负  
    }else temp=1;//温度为正	  	  
    tem=TH; //获得高八位
    tem<<=8;    
    tem+=TL;//获得底八位
    tem=(double)tem*0.625;//转换     
	if(temp)return tem; //返回温度值
	else return -tem;    
}

short DS18B20_Get_Temp2(void)
{
    u8 temp;
    u8 TL,TH;
	short tem;
	
    DS18B20_Start2();                    // ds1820 start convert
    DS18B20_Rst2();
    DS18B20_Check2();	 
    DS18B20_Write_Byte2(0xcc);			// skip rom
    DS18B20_Write_Byte2(0xbe);			// convert	    
    TL=DS18B20_Read_Byte2(); 			// LSB   
    TH=DS18B20_Read_Byte2(); 			// MSB   
    if(TH>7)
    {
        TH=~TH;
        TL=~TL; 
        temp=0;							//温度为负  
    }else temp=1;						//温度为正	  	  
    tem=TH; 							//获得高八位
    tem<<=8;    
    tem+=TL;				//获得底八位
    tem=(double)tem*0.625;	//转换     
	if(temp)return tem; 	//返回温度值
	else return -tem;    
}
















