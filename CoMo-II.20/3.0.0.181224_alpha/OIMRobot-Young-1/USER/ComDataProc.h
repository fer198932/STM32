#ifndef _COMDATAPROC_H
#define _COMDATAPROC_H
/*  ���ݴ������ */
#include "sys.h"
#include "usart.h"
#include "config.h"
#include "main.h"
#include "command.h"
#include "buffer.h"


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

// �Ӵ��ڵõ��Ĵ���������
typedef struct {
	u8 cmd_Type;  					// �������͡�֡��ʶ �磺#define SELFCHECK 		0x0B     //�Լ�
	u8 cmd_Excute;					// ��Ҫִ�е����ָ����
	u8 cmd_Value;						// ����ȡֵ
	
	u8 resp_Excute;					// �ظ�ָ���� ͨ��resp_Excute=cmd_Excute
	u8 resp_Status;					// �ظ�״̬
	void* cmd_Data; 				// ָ����������ݽṹ�壬�������ݵ��������Ҫ�õ�
} Proc_Data; 

// �������ݽṹ�� 
typedef struct {
	u32 				plusNum[AXIS_NUM];				// ������
	u32 				clk[AXIS_NUM];						// Ƶ�ʣ��������˶��ٶ�
	Motor_Dir	 	dir[AXIS_NUM];						// ����˶�����
} Plus_Data;

// �������ݽṹ��ĳ�ʼ��
void comData_Init(void);

// �ӻ������õ����ݲ����д���������ص���ĳ���
void UsartDataProc(void);

// �����Ӧ�����ϵĻ���������
static IfOK_Status bufData_Proc_Region(PosCur posCur);

// proc�ṹ��ָ���Ӧ���ݽṹ��
static void proc_pt_motionData(void* _data);

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
void setRespStr(u8 resStr[], u16 length);

// �ظ���λ���ĳ��� �쳣�����
void respMsgError(const char str[], u8 status);

// �ظ�����������Ϣ
void respUsartMsg(const u8 backResString[], u16 length);

#endif
