#include "MainAxisMotor.h"			

//////////////////////////////////////////////////////////////////////////////////	 
// ���������Ƴ���
// 2019��3��6��13:42:10 byYJY
////////////////////////////////////////////////////////////////////////////////// 


extern 		NAxis_TIM_Structure					mainMotor_TIM_Structure;



// �����ʼ������Ҫ�ǵ������
void mainMotor_Init(void)
{
	PWM_Forced2High(mainMotor_TIM_Structure.TIM_N, DISABLE, mainMotor_TIM_Structure.ch_out);	// ȡ��ǿ������
	
	// Ԥ���ķ���IO�ڳ�ʼ��
}

// �������
static void mainMotorDecode(void)
{
	nAxisSetPWM(mainMotor_TIM_Structure.TIM_N, MAINMOTOR_PSC);
	TIM_GenerateEvent(mainMotor_TIM_Structure.TIM_N, TIM_EventSource_Update);			// ���������¼������¼Ĵ���
	
	TIM_Cmd(mainMotor_TIM_Structure.TIM_N, ENABLE);
//	PWM_Cmd(mainMotor_TIM_Structure.TIM_N, ENABLE, mainMotor_TIM_Structure.ch_out);		// ��ǿ������
	
	// ����1
	setMainMotor_DC(mainMotor_TIM_Structure.ch_out, MAINMOTOR_DECODE_DC1);
	delay_ms(MAINMOTOR_DECODE_DELAY1);
	
	// ����2 
	setMainMotor_DC(mainMotor_TIM_Structure.ch_out, MAINMOTOR_DECODE_DC2);
	delay_ms(MAINMOTOR_DECODE_DELAY2);
	
	TIM_Cmd(mainMotor_TIM_Structure.TIM_N, DISABLE);	
//	PWM_Cmd(mainMotor_TIM_Structure.TIM_N, DISABLE, mainMotor_TIM_Structure.ch_out);		// ��ȡ��ǿ������
}


// ���Ṥ��
void mainMotorWork(Decode_Mode mode)
{
	u8 temp1, temp2;
	
	if(HM_Decode == mode)
	{
		delay_ms(1);
		temp1 = MainMotor_Start();
		delay_ms(1);
		temp2 = MainMotor_Start();
		
		if(temp1 == temp2)
		{
			if(0 == temp1)
			{
				mainMotorDecode();
				delay_us(100);
			}
		}
	}
	
	nAxisSetPWM(mainMotor_TIM_Structure.TIM_N, MAINMOTOR_PSC);
	TIM_GenerateEvent(mainMotor_TIM_Structure.TIM_N, TIM_EventSource_Update);			// ���������¼������¼Ĵ���
	
	// �趨ռ�ձ�
	setMainMotor_DC(mainMotor_TIM_Structure.ch_out, MAINMOTOR_DC);
	
	TIM_Cmd(mainMotor_TIM_Structure.TIM_N, ENABLE);
//	PWM_Cmd(mainMotor_TIM_Structure.TIM_N, ENABLE, mainMotor_TIM_Structure.ch_out);		// ��ǿ������
}


// ����ֹͣ
void mainMotorStop(void)
{
	TIM_Cmd(mainMotor_TIM_Structure.TIM_N, DISABLE);
//	PWM_Cmd(mainMotor_TIM_Structure.TIM_N, DISABLE, mainMotor_TIM_Structure.ch_out);		// ��ȡ��ǿ������
}

// �趨��������ռ�ձȣ�dc��ռ�ձ� ��λ%  
void setMainMotor_DC(u8 ch, u8 dc)
{
	u32 compareTemp;
	
	compareTemp = dc * MAINMOTOR_ARR / 100;
	
	if(1 > compareTemp) 
	{
		compareTemp = 1;
		respMsgError("������������������\r\n", 1);
	}
	else if(TIM_ARR-1 < compareTemp)
	{
		compareTemp = TIM_ARR-1;
		respMsgError("������������������\r\n", 1);
	}
	
	switch(ch)
	{
		case 1:
			TIM_SetCompare1(mainMotor_TIM_Structure.TIM_N, compareTemp);
			break;
		
		case 2:
			TIM_SetCompare2(mainMotor_TIM_Structure.TIM_N, compareTemp);
			break;
		
		case 3:
			TIM_SetCompare3(mainMotor_TIM_Structure.TIM_N, compareTemp);
			break;
		
		case 4:
			TIM_SetCompare4(mainMotor_TIM_Structure.TIM_N, compareTemp);
			break;
		
		default:
			respMsgError("������������������\r\n", 1);
			break;
	}
	
}







