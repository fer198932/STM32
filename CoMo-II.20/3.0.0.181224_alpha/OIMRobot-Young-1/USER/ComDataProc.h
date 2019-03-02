#ifndef _COMDATAPROC_H
#define _COMDATAPROC_H
/*  ���ݴ������ */
#include "sys.h"
#include "usart.h"
#include "config.h"
#include "main.h"
#include "command.h"
#include "buffer.h"
#include "OffLine_Data.h"


#define  DATA_LENGTH_POSITION 	3 			// ���ݳ��ȶ�Ӧ��λ��

typedef enum { IS_OK = 0, NOT_OK = !IS_OK} IfOK_Status;


// ���ڽ���һ֡������ɵı�־
extern	 volatile 	IfOK_Status 	USARTRx_IfOK_Flag;

// ����DMA��������ı�־
extern volatile 	ErrorStatus 	USARTRx_DMAOut_Flag;

// �����Ƿ������
// extern	volatile	IfOK_Status 	DataPro_IsOK_Flag;

// λ���α�
typedef struct {
	u16 start;
	u16 end;
} PosCur;

// �������ݽṹ�� 
typedef struct {
	u32 				plusNum[AXIS_NUM];				// ������
	u32 				clk[AXIS_NUM];						// Ƶ�ʣ��������˶��ٶ�
	Motor_Dir	 	dir[AXIS_NUM];						// ����˶�����
} Plus_Data;

// �Ӵ��ڵõ��Ĵ���������
typedef struct {
	u8 						cmd_Type;  						// �������͡�֡��ʶ �磺#define SELFCHECK 		0x0B     //�Լ�
	u8 						cmd_Excute;						// ��Ҫִ�е����ָ����
	u32 					cmd_Value;						// ����ȡֵ
//	u32			offline_length;						// �ѻ��ӹ����ݵĳ���
	
	u8 						resp_Excute;					// �ظ�ָ���� ͨ��resp_Excute=cmd_Excute
	u8 						resp_Status;					// �ظ�״̬
//	void* 	cmd_Data; 								// ָ����������ݽṹ�壬�������ݵ��������Ҫ�õ� ��ʱû�õ������ã�
	Plus_Data			plus_Datas;						// �����˶�����
} Proc_Data; 



/* �Ӽ�����صĽṹ��  */
#define 	DATA_LENGTH				1024						// ��żӼ���pscֵ�Ŀռ䳤�ȣ������ɿ���תΪ��̬����

typedef enum {ADD_SPEED = 0, CONST_SPEED = 1, SUB_SPEED = 2} AddSubSpeedStatus;

// ����PSCֵ�Ľṹ��
typedef struct {
	u16 							length;
	u16 							psc_data[DATA_LENGTH];
	FunctionalState		enAddSubFlag; 												// ʹ�ܼӼ��ٵĿ���
	float 						addSpeed_NeedPlusNum; 			// ���ٽ׶���Ҫ��������
} PSC_Data_Array;

// �����ʼ������˶����ݵĽṹ�� ��ʼ��ʱĬ��Ϊ0
typedef struct {
	PSC_Data_Array 	PSC_Data[AXIS_NUM];
	
	// �Ӽ���״̬���
	AddSubSpeedStatus addSubSpeed_Status[AXIS_NUM];
	
	float 			addSubTime;											// ÿ���Ӽ��ٵ�ʱ�� ��ADDSUB_TIMER������ʱ�� ��λus
	u32 				maxClk;
	u32 				minClk;
	u32 				nAxisClk_Cur[AXIS_NUM];					// ��ǰƵ��
	u8 					maxClkNum;											// ���Ƶ�ʶ�Ӧ���� ����AXIS_NUM��ʾ�����δ����	

	Proc_Data		cmd_Datas;											// ��������
} Motion_Strcuture;

/* �Ӽ�����صĽṹ��  */


// �������ݽṹ��ĳ�ʼ��
void comData_Init(void);

// �ӻ������õ����ݲ����д���������ص���ĳ���
void UsartDataProc(void);

// �����������ݵĺ�����֡��ʶ0X0D��ָ����0X22
static void plus_Data_Proc(PosCur posCur);

// �ѻ��ӹ� ֡��ʶ0X0D��ָ����0X25
static void offline_Data_Proc(PosCur posCur);

// �����Ӧ�����ϵĻ���������
static IfOK_Status bufData_Proc_Region(PosCur posCur);

// ���ӻ�������ȡ�������Ƿ�ϸ�
static IfOK_Status bufData_Proc(void);

// �õ������趨�����ݳ���
static u16 getSetDataSize(PosCur posCur);

// �õ������趨�������� shift��ƫ���� 0-4 ��Ӧ5��
static u32 getSetDataPlusNum(PosCur posCur, u8 shift);

// �õ������趨��Ƶ�� shift��ƫ���� 0-4 ��Ӧ5��
static u32 getSetDataClk(PosCur posCur, u8 shift);

// �õ������趨�ķ��� shift��ƫ���� 0-4 ��Ӧ5��
static u8 getSetDataDir(PosCur posCur, u8 shift);

// ���OK���趨�������ݵĽṹ��
static IfOK_Status setCmdData(PosCur posCur);

// ���ûظ��ַ����ĸ�ʽ
void setRespStr(Proc_Data* pCmd, u8 resStr[], u16 length);

// �ظ���λ���ĳ��� �쳣�����
void respMsgError(const char str[], u8 status);

// �ظ�����������Ϣ
void respUsartMsg(const u8 backResString[], u16 length);

#if PRIN2DISP
#else
// �趨�˶����ݵĴ��ڷ�������ĸ�ʽ
void setRespStr_Motion(Proc_Data* pCmd, u8 respStr[], u16 length, u8 status);
#endif

#endif
