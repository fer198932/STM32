/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          ԭ���������������
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : Ds18b20.c
* By  : Hiker Lee
* 2018-06-14
* 0614����18B20��1��PD11�����¶ȣ�2��PD12�������¶ȣ�
********************************************************************************************************/

#include "ds18b20.h"
#include "delay.h"	


//��λDS18B20
void DS18B20_Rst1(void)	   
{                 
	DS18B20_IO_OUT1(); //SET PG11 OUTPUT
  	DS18B20_DQ_OUT1=0; //����DQ
  	delay_us(750);    //����750us
  	DS18B20_DQ_OUT1=1; //DQ=1 
	delay_us(15);     //15US
}

void DS18B20_Rst2(void)	   
{                 
	DS18B20_IO_OUT2(); //SET PG11 OUTPUT
  	DS18B20_DQ_OUT2=0; //����DQ
  	delay_us(750);    //����750us
  	DS18B20_DQ_OUT2=1; //DQ=1 
	delay_us(15);     //15US
}

//�ȴ�DS18B20�Ļ�Ӧ
//����1:δ��⵽DS18B20�Ĵ���
//����0:����
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

//��DS18B20��ȡһ��λ
//����ֵ��1/0
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

//��DS18B20��ȡһ���ֽ�
//����ֵ������������
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

//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
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

//��ʼ�¶�ת��
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

//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������
//����0:����    	 
u8 DS18B20_Init1(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIOGʱ��

  //GPIOG9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��
 
 	DS18B20_Rst1();
	return DS18B20_Check1();
}  
u8 DS18B20_Init2(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��GPIOGʱ��

  //GPIOG9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//50MHz
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
  GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��
 
 	DS18B20_Rst2();
	return DS18B20_Check2();
}  

//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250�� 
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
        temp=0;//�¶�Ϊ��  
    }else temp=1;//�¶�Ϊ��	  	  
    tem=TH; //��ø߰�λ
    tem<<=8;    
    tem+=TL;//��õװ�λ
    tem=(double)tem*0.625;//ת��     
	if(temp)return tem; //�����¶�ֵ
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
        temp=0;							//�¶�Ϊ��  
    }else temp=1;						//�¶�Ϊ��	  	  
    tem=TH; 							//��ø߰�λ
    tem<<=8;    
    tem+=TL;				//��õװ�λ
    tem=(double)tem*0.625;	//ת��     
	if(temp)return tem; 	//�����¶�ֵ
	else return -tem;    
}
















