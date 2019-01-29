#include "led.h"
#include "config.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
// LED 程序  
////////////////////////////////////////////////////////////////////////////////// 	
  
//LED IO初始化
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_Structure_XX GPIO_led;
	GPIO_Structure_Init(GPIO_LED, &GPIO_led);
	
	RCC_AHB1PeriphClockCmd(GPIO_led.RCC_Periph_N,ENABLE); //使能GPIOD的时钟
	
	GPIO_InitStructure.GPIO_Pin=GPIO_led.GPIO_Pin_N;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//输出
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //推挽输出
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;  //上拉
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //高速GPIO
	GPIO_Init(GPIO_led.GPIO_Port,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIO_led.GPIO_Port,GPIO_led.GPIO_Pin_N); //GPIOF9,10高电平
	
	// 灯亮
	Set_Led(GPIO_led, 0);
}

void Set_Led(GPIO_Structure_XX GPIO_led, u8 status)		// 0:灯亮 1：灯灭
{
	if(0 == status)
		GPIO_ResetBits(GPIO_led.GPIO_Port, GPIO_led.GPIO_Pin_N);
	else
		GPIO_SetBits(GPIO_led.GPIO_Port, GPIO_led.GPIO_Pin_N);
}




