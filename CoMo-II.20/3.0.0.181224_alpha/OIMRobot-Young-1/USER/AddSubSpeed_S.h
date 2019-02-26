/********************************************************************************************************
*    S�ͼӼ����㷨 2019��2��13��09:02:11 byYJY
********************************************************************************************************/

#ifndef __ADDSUBSPEED_S_H
#define __ADDSUBSPEED_S_H 

#include "sys.h"
#include <math.h>
#include "pwm.h"


// ���ø���������Ƶ�ʣ���Ƶ�����˶������С����궨��ȷ��
#define X_AXIS_MIN_CLK 		500
#define Y_AXIS_MIN_CLK 		500
#define Z_AXIS_MIN_CLK 		500
#define A_AXIS_MIN_CLK 		500
#define B_AXIS_MIN_CLK 		500

#define X_AXIS_MAX_CLK 		10000
#define Y_AXIS_MAX_CLK 		10000
#define Z_AXIS_MAX_CLK 		10000
#define A_AXIS_MAX_CLK 		10000
#define B_AXIS_MAX_CLK 		10000


#define 	S_FLEXIBLE 				4								// S���ߵ�ѹ�������Խ������Խб��ԽСԽ�ӽ��ȼ��٣� ����ȡֵ4-6
#define 	DATA_LENGTH				1024						// ��żӼ���pscֵ�Ŀռ䳤�ȣ������ɿ���תΪ��̬����
#define 	AddSub_Time_MAX		200							// �Ӽ��ٽ׶����ʱ�䣬��λms����ӦƵ�ʴ�0~10000�����
#define		AddSub_Step_DIV		((float)10)			// ���Ƶ�ʰ���ֵ���зֶ� ��0~10000ʱ������Ϊ1000��
#define 	ConstS_NeedPlus		5								// ��������Ҫ��������������ֵ����������

#define 	MIN_STEP_NUM			((float)5.0) 		// С�ڸ�ֵ�Ĳ�����Ϊ������
	
#define 	MIN_STEP_LENGTH__INIT_VAULE		0xFFFF	// ��С�������ĳ�ʼֵ����������²�����ָ�ֵ

typedef enum {ADD_SPEED = 0, CONST_SPEED = 1, SUB_SPEED = 2} AddSubSpeedStatus;

// ����PSCֵ�Ľṹ��
typedef struct {
	u16 							length;
	u16 							psc_data[DATA_LENGTH];
	FunctionalState		enAddSubFlag; 												// ʹ�ܼӼ��ٵĿ���
	float 						addSpeed_NeedPlusNum; 			// ���ٽ׶���Ҫ��������
} PSC_Data_Array;


/* 	����S�ͼ������ߵĺ��� �ο���https://blog.csdn.net/weixin_40567368/article/details/81742938
*  	fre: Ƶ������-Ƶ��ֵ����ʱ����
*  	period[] : ��ʱ��װ�ص�PSCֵ
*	 	len : �Ӽ��ٵĲ���
*  	fre_max : ����ʱ���Ƶ��
*		fre_min : ��ʼƵ��
* 	flexible : S���ߵ�ѹ�������Խ������Խб��ԽСԽ�ӽ��ȼ��٣� ����ȡֵ4-6
*/
static FunctionalState calSModelLine(float fre, u16 period[], float len, 
	float fre_max, float fre_min, float flexible);

// �Ӽ�����ز������õĳ�ʼ��
void AddSubSpeed_Init(void);

// �趨�˶�����
static void	setStepMotorDir(void);

// Ƶ�ʵ�����ʹ��Ƶ��λ�ں��ʷ�Χ�ڡ�0������ok��1���������������Ƶ������
static u8 adjustClk(void);

// ������Ƶ��
static u32 preProcMaxClk(void);

// ��÷������СƵ�� �������ֵ�ٵ���
static u32 preProcMinClk(u32 max_Clk);

// ��Ƶ���о��ȶ�ʧ
static void reduceClk(void);
// ��Ƶ���ٶ�̫��ʱʹ�ã����ȼ���
static void boostClk(void);

// ���ص�ǰ������������СƵ��
u32 n_Axis_Max_Clk(u8 i);
u32 n_Axis_Min_Clk(u8 i);

// �Ƿ������Ƶ���ж� 
static FunctionalState ifBoostClkOK(void);

// ����Ӽ�����ηֶ�
static void cal_AddSubSpeed_Div(void);

// �������S�ͼ�������
static void cal_S_Line(void);


// ���㵱ǰ�����£���ɼ�������Ҫ��������
// ��ע���£� https://blog.csdn.net/gtkknd/article/details/52199963 byYJY
static float calAddSpeed_NeedPlusNum(u16 period[], float len);

// ����ָ������������Ҫ��ʱ�� ��λus
static float calTotalNeedTime(u8 nAxis);

// ��������˶�����
static void resetMotionData(void);

// ������˶�״̬��ʼ��
static void	nAxisMotion_Init(void);

// ������С�Ĳ�����
//static u16 calMinStepNumLength(PSC_Data_Array	Psc_Data_Cur[]);


// �򿪻�رռӼ��ٹ��ܵı��
static void En_ASS_Flag(FunctionalState status, N_Axis n_Axis);

// ����ָ�����Ƶ�ʣ�����������ʼƵ��
static ErrorStatus CorrectClk(u8 nAxis, float needTime);


#endif
