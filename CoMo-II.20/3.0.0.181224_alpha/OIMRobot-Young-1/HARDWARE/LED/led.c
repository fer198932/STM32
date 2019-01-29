#include "led.h"
#include "config.h"
#include "delay.h"
//////////////////////////////////////////////////////////////////////////////////	 
// LED ����  
////////////////////////////////////////////////////////////////////////////////// 	
  
//LED IO��ʼ��
void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_Structure_XX GPIO_led;
	GPIO_Structure_Init(GPIO_LED, &GPIO_led);
	
	RCC_AHB1PeriphClockCmd(GPIO_led.RCC_Periph_N,ENABLE); //ʹ��GPIOD��ʱ��
	
	GPIO_InitStructure.GPIO_Pin=GPIO_led.GPIO_Pin_N;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;//���
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;  //�������
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_NOPULL;  //����
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_100MHz; //����GPIO
	GPIO_Init(GPIO_led.GPIO_Port,&GPIO_InitStructure);
	
	GPIO_SetBits(GPIO_led.GPIO_Port,GPIO_led.GPIO_Pin_N); //GPIOF9,10�ߵ�ƽ
	
	// ����
	Set_Led(GPIO_led, 0);
}

void Set_Led(GPIO_Structure_XX GPIO_led, u8 status)		// 0:���� 1������
{
	if(0 == status)
		GPIO_ResetBits(GPIO_led.GPIO_Port, GPIO_led.GPIO_Pin_N);
	else
		GPIO_SetBits(GPIO_led.GPIO_Port, GPIO_led.GPIO_Pin_N);
}




