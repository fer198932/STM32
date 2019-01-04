/********************************************************************************************************
*                                 Original intelligent manufacturing robot
*                                          Ô­ÄÜÖÇÄÜÖÆÔì»úÆ÷ÈË
*
*                                 (c) Copyright 2018, DOYO JONO, CQ
*                                          All Rights Reserved
*
*                                            STM32F103ZET6
*                                              
* File : ChangeTools.C
* By  : Hiker Lee
* 2018-05-07
* ÖØĞ´»»µ¶Âß¼­£¬ÒòÎª×ø±êÏµ±äÁË£»20180601
* ÉÏµ¶»ú¹¹ĞŞ¸Ä£¬³ÌĞòµ÷Õû£»20180720
* Ôö¼Óµ½µ½Áãµã¼ÆÊıµÄ¹¦ÄÜ byYJY 2018Äê11ÔÂ12ÈÕ14:13:00
********************************************************************************************************/
#include "ChangeTools.h"
#include "ComDataProc.h"

#define zZeroFlag GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_10)				//0:µ½Î»£»1:Î´µ½Î»£»
#define yZeroFlag GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_7)
#define xZeroFlag GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_6)

#define toolY1Flag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_0)
#define toolY2Flag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_1)
#define toolY3Flag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_2)				// Ã»ÓÃÁË£¬1¡Á4Ê±ÓÃµÄ
#define toolY4Flag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_3)				// Ã»ÓÃÁË£¬1¡Á4Ê±ÓÃµÄ

#define toolX1Flag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_4)
#define toolX2Flag GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_11)

#define toolZFlag GPIO_ReadInputDataBit(GPIOF, GPIO_Pin_13)				// ¸ÄÎªPF13 byYJY

#define mainMoterToolsFlag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_7)		//0:ÓĞµ¶£»	1:ÎŞµ¶£»	
#define unClampFlag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_8)			//0:ËÉ¿ªµ½Î»£»	1:ËÉ¿ªÎ´µ½Î»£»
#define clampFlag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_10)				//0:¼Ğ½ôµ½Î»£»	1:¼Ğ½ôÎ´µ½Î»£»

#define sheelOpenCloseFlag GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_9)		//0:Íâ¿Ç´ò¿ª£»	1:Íâ¿Ç¹Ø±Õ£»

u8 targeToolNo=0;
u8 usingToolNo=0;

extern u8 xAxisStepFlag;
extern u8 yAxisStepFlag;
extern u8 zAxisStepFlag; 
extern unsigned char backResString[20];
extern unsigned char  xAxisDir,yAxisDir,zAxisDir;

extern u32 pwmNumTim2;
extern u32 pwmNumTim3;
extern u32 pwmNumTim4;

// ÉèÖÃ±êÖ¾Î»
u8 stepsFlag[5] = {0, 0, 0, 0, 0}; 				// ´ò¿ªX¡¢Y¡¢Z¡¢A¡¢BµÄ¼ÆÊı 1£º´ò¿ª£» 0£º¹Ø±Õ

ChangeToolsSavedSteps changeToolsSavedSteps;	// ÓÉÉÏÎ»»úÊı¾İ³õÊ¼»¯
ChangeToolsSavedSteps stepsTemp;				// ÁÙÊ±±£´æ×ß¹ıµÄ²½Êı
// ¸ù¾İÉÏÎ»»ú·¢ËÍµÄÊı¾İ£¬¶Ôµ¶¾ßÎ»ÖÃµÈ½øĞĞ³õÊ¼»¯
void savedStepsInit(void)
{
	// ³õÊ¼»¯¹¤¼şÓëÁãµãµÄÎ»ÖÃ¹ØÏµ
	changeToolsSavedSteps.xSteps[0] = 0;
	changeToolsSavedSteps.ySteps[0] = 0;
	changeToolsSavedSteps.zSteps[0] = 0;
	
	// ³õÊ¼»¯µ¶¾ßÓëÁãµãÖ®¼äµÄÎ»ÖÃ¹ØÏµ£ºÓÉÉÏÎ»»ú´«À´£¬ÔİÊ±Ğ´¸ö¹Ì¶¨Öµ
	changeToolsSavedSteps.xSteps[1] = 0xf5ab;//0xf555;//0xf548;	 // µ¶¾ß1µÄÎ»ÖÃ
	changeToolsSavedSteps.ySteps[1] = 0x276a;//0x254e;//0x2540;
	changeToolsSavedSteps.zSteps[1] = 0x7327;//0x73ac;//0x3f2f;
	
	changeToolsSavedSteps.xSteps[2] = 10000;	// µ¶¾ß2µÄÎ»ÖÃ
	changeToolsSavedSteps.ySteps[2] = 10000;
	changeToolsSavedSteps.zSteps[2] = 10000;
	
	changeToolsSavedSteps.xSteps[3] = 10000;	// µ¶¾ß3µÄÎ»ÖÃ
	changeToolsSavedSteps.ySteps[3] = 10000;
	changeToolsSavedSteps.zSteps[3] = 10000;
	
	changeToolsSavedSteps.xSteps[4] = 10000;	// µ¶¾ß4µÄÎ»ÖÃ
	changeToolsSavedSteps.ySteps[4] = 10000;
	changeToolsSavedSteps.zSteps[4] = 10000;
	
}

void mainToolFlagIOInit(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure;

  //ÒÔÏÂÊ¹ÄÜÎ»ÖÃ´«¸ĞÆ÷£»
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);		//Ê¹ÄÜGPIOGÊ±ÖÓ
 
  GPIO_InitStructure.GPIO_Pin=GPIO_Pin_7;	  				//µ¶1-4£¬ÍÑµ¶µç»úÏŞÎ»
  GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN;			  	//ÊäÈë
  GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;			  	//ÉÏÀ­ÊäÈë
  GPIO_Init(GPIOG,&GPIO_InitStructure); 				  	//³õÊ¼»¯GPIOG
} 


void delay_s(u8 delay)
{
	u8 i;
	for(i=0;i<delay;i++)
	{
		delay_ms(1000);
	}
}

void axis3Back2ORG(void)
{
	//	ZÖá¹éÁã
	// zAxisDIR_ON;				//z¹éÁã·½Ïò
	zAxisDIR_OFF;					// ²âÊÔÓÃ
	disableZaxisCountINT(); 	//½ûÖ¹zÖá¼ÆÊıÖĞ¶Ï£»
	zAxisClkSet(Z_AXIS_RST_CLK);			//z¹éÁãÆµÂÊÉè¶¨£»
	TIM_Cmd(TIM4, ENABLE);		//Ê¹ÄÜz¹éÁã¶¯×÷£»	
	while(zZeroFlag==1);		//¹éÁãÍê³É£»
	TIM_Cmd(TIM4, DISABLE); 	//¹éÁãÍê³É£¬PWMÍ£Ö¹£»
	enableZaxisCountINT();		//¿ªÆôzÖá¼ÆÊıÖĞ¶Ï£»
	delay_s(2);

	//	YÖá¹éÁã
	yAxisDIR_OFF;				//y¹éÁã·½Ïò
	disableYaxisCountINT(); 	//½ûÖ¹yÖá¼ÆÊıÖĞ¶Ï£»
	yAxisClkSet(Y_AXIS_RST_CLK);			//y¹éÁãÆµÂÊÉè¶¨£»
	TIM_Cmd(TIM2, ENABLE);		//Ê¹ÄÜy¹éÁã¶¯×÷£»	
//	while(yZeroFlag==1);		//¹éÁãÍê³É£»
	while(toolY1Flag==1);			// ²âÊÔÓÃ
	TIM_Cmd(TIM2, DISABLE); 	//¹éÁãÍê³É£¬PWMÍ£Ö¹£»
	enableYaxisCountINT();		//¿ªÆôyÖá¼ÆÊıÖĞ¶Ï£»
	delay_s(2);
	
	//	XÖá¹éÁã
	xAxisDIR_ON;				//x¹éÁã·½Ïò   //xAxisDIR_ON
	disableXaxisCountINT(); 	//½ûÖ¹xÖá¼ÆÊıÖĞ¶Ï£»
	xAxisClkSet(X_AXIS_RST_CLK);			//x¹éÁãÆµÂÊÉè¶¨£»
	TIM_Cmd(TIM3, ENABLE);		//Ê¹ÄÜx¹éÁã¶¯×÷£»
//	while(xZeroFlag==1);		//¹éÁãÍê³É£»
	while(toolX2Flag==1);			// ²âÊÔÓÃ
	TIM_Cmd(TIM3, DISABLE); 	//¹éÁãÍê³É£¬PWMÍ£Ö¹£
	enableXaxisCountINT();		//¿ªÆôxÖá¼ÆÊıÖĞ¶Ï£»	
	delay_s(2);
}

// ÄÜ¼Æ²½ÊıµÄ»Øµ½Ô­µã TEST_STEPSNUM´ò¿ªµÄÇé¿öÏÂµ÷ÓÃ¸Ãº¯Êı
void axis3Back2ORGWithCnt(void)
{
	// ²»ÔÊĞí´ò¶Ï
	xAxisStepFlag = 0;
	yAxisStepFlag = 0;
	zAxisStepFlag = 0;
	
	//	ZÖá¹éÁã ////////////////////////////////////////////////
	zAxisDIR_ON;				//z¹éÁã·½Ïò
//	disableZaxisCountINT(); 	//½ûÖ¹zÖá¼ÆÊıÖĞ¶Ï£» ½ûÖ¹ºóÎŞ·¨¼ÆÊı
	
	// Ö»ÓĞÎŞ¼Ó¼õËÙÔË¶¯
	zAxisClkSet(Z_AXIS_RST_CLK>>1);			//zÖáÏÂÒÆµÄËÙ¶È£»
	
	// ¿ªÊ¼¶¯×÷²¢¼ÆÊı
	stepsFlag[2] = 1;					// ÉèÖÃ±êÖ¾Î»£¬zÖá¿ªÊ¼¼ÆÊı 1:¼ÆÊı£» 0£ºÍ£Ö¹¼ÆÊı
	TIM_Cmd(TIM4, ENABLE);		//Ê¹ÄÜz¹éÁã¶¯×÷£»
	
	while(zZeroFlag==1);		//¹éÁãÍê³É£»
	
	TIM_Cmd(TIM4, DISABLE); 	//¹éÁãÍê³É£¬PWMÍ£Ö¹£»
	stepsFlag[2] = 0;					//  ¹Ø±Õ¼ÆÊı
//	enableZaxisCountINT();		//¿ªÆôzÖá¼ÆÊıÖĞ¶Ï£»
	delay_s(1);

	//	YÖá¹éÁã  ///////////////////////////////////////////////////
	yAxisDIR_ON;				//y¹éÁã·½Ïò
//	disableYaxisCountINT(); 	//½ûÖ¹yÖá¼ÆÊıÖĞ¶Ï£»
	
	// Ö»ÓĞÎŞ¼Ó¼õËÙÔË¶¯
	yAxisClkSet(Y_AXIS_RST_CLK>>1);			//yÖáÏÂÒÆµÄËÙ¶È£»

	// ¿ªÊ¼¶¯×÷²¢¼ÆÊı
	stepsFlag[1] = 1;					// ÉèÖÃ±êÖ¾Î»£¬yÖá¿ªÊ¼¼ÆÊı 1:¼ÆÊı£» 0£ºÍ£Ö¹¼ÆÊı
	TIM_Cmd(TIM2, ENABLE);		//Ê¹ÄÜy¹éÁã¶¯×÷£»
	
	while(yZeroFlag==1);		//¹éÁãÍê³É£»
//	while(toolY1Flag==1);			// ²âÊÔÓÃ
	
	TIM_Cmd(TIM2, DISABLE); 	//¹éÁãÍê³É£¬PWMÍ£Ö¹£»
	stepsFlag[1] = 0;
//	enableYaxisCountINT();		//¿ªÆôyÖá¼ÆÊıÖĞ¶Ï£»
	delay_s(1);
	
	//	XÖá¹éÁã  /////////////////////////////////////////////////
	xAxisDIR_OFF;				//x¹éÁã·½Ïò 
//	disableXaxisCountINT(); 	//½ûÖ¹xÖá¼ÆÊıÖĞ¶Ï£»
	
	// Ö»ÓĞÎŞ¼Ó¼õËÙÔË¶¯
	xAxisClkSet(X_AXIS_RST_CLK>>1);			//xÖáÏÂÒÆµÄËÙ¶È£»
	
	// ¿ªÊ¼¶¯×÷²¢¼ÆÊı
	stepsFlag[0] = 1;					// ÉèÖÃ±êÖ¾Î»£¬xÖá¿ªÊ¼¼ÆÊı 1:¼ÆÊı£» 0£ºÍ£Ö¹¼ÆÊı
	TIM_Cmd(TIM3, ENABLE);		//Ê¹ÄÜx¹éÁã¶¯×÷£»
	
	while(xZeroFlag==1);		//¹éÁãÍê³É£»
//	while(toolX2Flag==1);			// ²âÊÔÓÃ
	
	TIM_Cmd(TIM3, DISABLE); 	//¹éÁãÍê³É£¬PWMÍ£Ö¹£
	stepsFlag[0] = 0;
//	enableXaxisCountINT();		//¿ªÆôxÖá¼ÆÊıÖĞ¶Ï£»
	delay_s(1);
	
	// ËùÓĞ¶¯×÷Íê³É
	stepMotor5AxisStop();
}

void dataFromTemp2Saved(u8 num)		// ½«±àºÅµÄtempÊı¾İ½øĞĞ±£´æ£º0£º¼Ó¹¤µãµ½Áãµã 1-4£ºÁãµãµ½µ¶¾ß
{
	changeToolsSavedSteps.xSteps[num] = stepsTemp.xSteps[0];
	changeToolsSavedSteps.ySteps[num] = stepsTemp.ySteps[0];
	changeToolsSavedSteps.zSteps[num] = stepsTemp.zSteps[0];
}

void toolsFromORG2Start(void)	// ´ÓÁãµã·µ»ØÔ­À´¹¤¼şµÄÎ»ÖÃ
{
	//	xÖáÕıÏòÒÆ£¬µ½¹¤¼şÎ»ÖÃ£» ///////////////////////	
	xAxisPlusNum = changeToolsSavedSteps.xSteps[0];		//xÖáÒÆ¶¯µÄ²½Êı£»
	pwmNumTim3 = 0; 
	xAxisStepFlag = 0;	

#ifndef IF_ADDSUB_WORK_CHANGETOOLS		
	// ÎŞ¼Ó¼õËÙÔË¶¯
	xAxisClkSet(X_AXIS_RST_CLK);			//xÖáÏÂÒÆµÄËÙ¶È£»
	xAxisDIR_ON;				// xÖáÒÆ¶¯·½Ïò
	TIM_Cmd(TIM3, ENABLE);		//¿ªÊ¼¶¯×÷£»	
#else
	// ÓĞ¼Ó¼õËÙÔË¶¯
	xAxisDir = 0;				// xÖáÒÆ¶¯·½Ïò
	xAxisClk = X_AXIS_RST_CLK;
	setSpeedRST_Seg(X_AXIS_RST_CLK, 0);	
#endif
	
	while(xAxisStepFlag!=2);	//¶¯×÷Íê³É£»
	xAxisStepFlag = 0;
	delay_s(1);
	
	//	yÖáÕıÏòÒÆ£¬µ½¹¤¼şÎ»ÖÃ£» ///////////////////////
	yAxisPlusNum = changeToolsSavedSteps.ySteps[0];		//yÖáÒÆ¶¯µÄ²½Êı£»
	pwmNumTim2 = 0; 
	yAxisStepFlag = 0;	

#ifndef IF_ADDSUB_WORK_CHANGETOOLS	
	// ÎŞ¼Ó¼õËÙÔË¶¯
	yAxisClkSet(Y_AXIS_RST_CLK);			//yÖáÏÂÒÆµÄËÙ¶È£»
	yAxisDIR_OFF;				// yÖáÒÆ¶¯·½Ïò
	TIM_Cmd(TIM2, ENABLE);		//¿ªÊ¼¶¯×÷£»	
#else	
	// ÓĞ¼Ó¼õËÙÔË¶¯
	yAxisDir = 1;
	yAxisClk = Y_AXIS_RST_CLK;
	setSpeedRST_Seg(Y_AXIS_RST_CLK, 1);	
#endif

	while(yAxisStepFlag!=2);	//¶¯×÷Íê³É£»
	yAxisStepFlag = 0;
	delay_s(1);
	
	//	zÖáÏÂÒÆ£¬µ½¹¤¼şÎ»ÖÃ£» ////////////////////////////////
	zAxisPlusNum = changeToolsSavedSteps.zSteps[0];		//zÖáÒÆ¶¯µÄ²½Êı£»
	pwmNumTim4 = 0; 	
	zAxisStepFlag = 0;	

#ifndef IF_ADDSUB_WORK_CHANGETOOLS	
	// ÎŞ¼Ó¼õËÙÔË¶¯
	zAxisClkSet(Z_AXIS_RST_CLK);			//zÖáÏÂÒÆµÄËÙ¶È£»
	zAxisDIR_OFF;				// zÖáÒÆ¶¯·½Ïò
	TIM_Cmd(TIM4, ENABLE);		//¿ªÊ¼¶¯×÷£»	
#else	
	// ÓĞ¼Ó¼õËÙÔË¶¯
	zAxisDir = 1;
	zAxisClk = Z_AXIS_RST_CLK;
	setSpeedRST_Seg(Z_AXIS_RST_CLK, 2);	
#endif

	while(zAxisStepFlag!=2);	//¶¯×÷Íê³É£»
	zAxisStepFlag = 0;
	delay_s(1);
	
	// ËùÓĞ¶¯×÷Íê³É
	stepMotor5AxisStop();
}

void fromORG2Tools(u8 toolNo)	// ´ÓÁãµãµ½µ¶¾ßÎ»ÖÃ
{	
	//	xÖáÕıÏòÒÆ£¬µ½µ¶¾ßÎ»ÖÃ£»
	xAxisPlusNum = changeToolsSavedSteps.xSteps[toolNo];		//xÖáÒÆ¶¯µÄ²½Êı£»
	pwmNumTim3 = 0; 		

#ifndef IF_ADDSUB_WORK_CHANGETOOLS		
	// ÎŞ¼Ó¼õËÙÔË¶¯
	xAxisClkSet(X_AXIS_RST_CLK);			//xÖáÏÂÒÆµÄËÙ¶È£»
	xAxisDIR_OFF;				// xÖáÒÆ¶¯·½Ïò
	TIM_Cmd(TIM3, ENABLE);		//¿ªÊ¼¶¯×÷£»	
#else
	// ÓĞ¼Ó¼õËÙÔË¶¯
	xAxisDir = 1;
	xAxisClk = X_AXIS_RST_CLK;
	setSpeedRST_Seg(X_AXIS_RST_CLK, 0);	
#endif		
	
	while(xAxisStepFlag!=2);	//¶¯×÷Íê³É£»
	xAxisStepFlag = 0;
	delay_s(2);
	
	//	yÖáÕıÏòÒÆ£¬µ½¹¤¼şÎ»ÖÃ£»
	yAxisPlusNum = changeToolsSavedSteps.ySteps[toolNo];		//yÖáÒÆ¶¯µÄ²½Êı£»
	pwmNumTim2 = 0; 		

#ifndef IF_ADDSUB_WORK_CHANGETOOLS		
	// ÎŞ¼Ó¼õËÙÔË¶¯
	yAxisClkSet(Y_AXIS_RST_CLK);			//yÖáÏÂÒÆµÄËÙ¶È£»
	yAxisDIR_ON;				// yÖáÒÆ¶¯·½Ïò
	TIM_Cmd(TIM2, ENABLE);		//¿ªÊ¼¶¯×÷£»	
#else
	// ÓĞ¼Ó¼õËÙÔË¶¯
	yAxisDir = 0;
	yAxisClk = Y_AXIS_RST_CLK;
	setSpeedRST_Seg(Y_AXIS_RST_CLK, 1);		
#endif
	
	while(yAxisStepFlag!=2);	//¶¯×÷Íê³É£»
	yAxisStepFlag = 0;
	delay_s(2);
	
	//	zÖáÏÂÒÆ£¬µ½¹¤¼şÎ»ÖÃ£»
	zAxisPlusNum = changeToolsSavedSteps.zSteps[toolNo];		//zÖáÒÆ¶¯µÄ²½Êı£»
	pwmNumTim4 = 0; 	
	
#ifndef IF_ADDSUB_WORK_CHANGETOOLS		
	// ÎŞ¼Ó¼õËÙÔË¶¯
	zAxisClkSet(Z_AXIS_RST_CLK);			//zÖáÏÂÒÆµÄËÙ¶È£»
	zAxisDIR_ON;				// zÖáÒÆ¶¯·½Ïò
	TIM_Cmd(TIM4, ENABLE);		//¿ªÊ¼¶¯×÷£»	
#else
	// ÓĞ¼Ó¼õËÙÔË¶¯
	zAxisDir = 0;
	zAxisClk = Z_AXIS_RST_CLK;
	setSpeedRST_Seg(Z_AXIS_RST_CLK, 2);	
#endif

	while(zAxisStepFlag!=2);	//¶¯×÷Íê³É£»
	zAxisStepFlag = 0;
	delay_s(2);
}

void xFromORG2ToolsXaPOS(void)
{
	//	XÖáÒÆ¶¯µ½È¡µ¶Î»ÖÃµÄXA
	xAxisDIR_OFF;				//xÖáÏòµ¶¼ÜÒÆ¶¯
	disableXaxisCountINT(); 	//½ûÖ¹xÖá¼ÆÊıÖĞ¶Ï£»
	xAxisClkSet(X_AXIS_RST_CLK);			//x¹éÁãÆµÂÊÉè¶¨£»
	TIM_Cmd(TIM3, ENABLE);  	//Ê¹ÄÜx
	
	while(toolX1Flag==1);		//¹éÁãÍê³É£»
	
	TIM_Cmd(TIM3, DISABLE); 	//¹éÁãÍê³É£¬PWMÍ£Ö¹£»
	enableXaxisCountINT();		//¿ªÆôxÖá¼ÆÊıÖĞ¶Ï£»
}

void xFromORG2ToolsXbPOS(void)
{
	//	XÖáÒÆ¶¯µ½È¡µ¶Î»ÖÃµÄXB
	xAxisDIR_OFF;				//xÖáÏòµ¶¼ÜÒÆ¶¯£¬ON¸ÄÎªOFF
	disableXaxisCountINT(); 	//½ûÖ¹xÖá¼ÆÊıÖĞ¶Ï£»
	xAxisClkSet(X_AXIS_RST_CLK);			//x¹éÁãÆµÂÊÉè¶¨£»
	TIM_Cmd(TIM3, ENABLE);  	//Ê¹ÄÜx
	
	while(toolX2Flag==1);		//¹éÁãÍê³É£»
	TIM_Cmd(TIM3, DISABLE); 	//¹éÁãÍê³É£¬PWMÍ£Ö¹£»
	enableXaxisCountINT();		//¿ªÆôxÖá¼ÆÊıÖĞ¶Ï£»
}

void yFromORG2ToolsPOS(u8 dir,u8 pos)
{
	//	YÖáÒÆ¶¯µ½È¡µ¶Î»ÖÃµÄY
	if(dir==1)
		yAxisDIR_OFF;				//y¹éÁã·½Ïò
	else
		yAxisDIR_ON;
	
	disableYaxisCountINT(); 	//½ûÖ¹yÖá¼ÆÊıÖĞ¶Ï£»
	yAxisClkSet(Y_AXIS_RST_CLK);			//y¹éÁãÆµÂÊÉè¶¨£»
	TIM_Cmd(TIM2, ENABLE);
	
	if(pos == 1)
		{
			while(toolY1Flag==1);		//µ½µ¶1Î»ÖÃ£»
		}
	else if(pos == 2)
		{
			while(toolY2Flag==1);		//µ½µ¶2Î»ÖÃ£»
		}
	
	else if(pos == 3)
		{
			while(toolY3Flag==1);		//µ½µ¶3Î»ÖÃ£»
		}
	
	else if(pos == 4)
		{
			while(toolY4Flag==1);		//µ½µ¶4Î»ÖÃ£»
		}
	TIM_Cmd(TIM2, DISABLE); 			// ¹Ø±Õ¶¨Ê±Æ÷ 
	enableYaxisCountINT();				//¿ªÆôyÖá¼ÆÊıÖĞ¶Ï£»
}

void xAxisUpNStep(u32 steps)
{
	//	xÖáÕıÏòÒÆ£¬µ½µ¶¾ßÎ»ÖÃ£»
	xAxisDIR_ON;				// xÖáÒÆ¶¯·½Ïò
	xAxisPlusNum = steps;		//xÖáÏÂÒÆµÄ²½Êı£»
	pwmNumTim3 = 0; 
	yAxisStepFlag	= 0;	
	xAxisClkSet(X_AXIS_RST_CLK);			//xÖáÏÂÒÆµÄËÙ¶È£»
	TIM_Cmd(TIM3, ENABLE);		//¿ªÊ¼¶¯×÷£»
	
	while(xAxisStepFlag!=2);	//¶¯×÷Íê³É£»
	xAxisStepFlag = 0;
}

void xAxisDownNStep(u32 steps)
{
	//	xÖá¸ºÏòÒÆ¶¯£¬µ½µ¶¾ßÎ»ÖÃ£»
	xAxisDIR_ON;				// xÖáÒÆ¶¯·½Ïò
	xAxisPlusNum = steps;		//xÖáÏÂÒÆµÄ²½Êı£»
	pwmNumTim3 = 0; 		
	xAxisClkSet(X_AXIS_RST_CLK);			//xÖáÏÂÒÆµÄËÙ¶È£»
	TIM_Cmd(TIM3, ENABLE);		//¿ªÊ¼¶¯×÷£»
	
	while(xAxisStepFlag!=2);	//¶¯×÷Íê³É£»
	xAxisStepFlag = 0;
}

void yAxisUpNStep(u32 steps)
{
	//	yÖáÕıÏòÒÆ£¬µ½µ¶¾ßÎ»ÖÃ£»
	yAxisDIR_OFF;				// yÖáÒÆ¶¯·½Ïò
	yAxisPlusNum = steps;		//yÖáÏÂÒÆµÄ²½Êı£»
	pwmNumTim2 = 0; 	
	yAxisStepFlag	= 0;
	yAxisClkSet(Y_AXIS_RST_CLK);			//yÖáÏÂÒÆµÄËÙ¶È£»
	TIM_Cmd(TIM2, ENABLE);		//¿ªÊ¼¶¯×÷£»
	
	while(yAxisStepFlag!=2);	//¶¯×÷Íê³É£»
	yAxisStepFlag = 0;
}

void zAxisDownNStep(u32 steps)
{
	//	ZÖáÏÂÒÆ£¬ÉÏµ¶£»
	zAxisDIR_ON;				//z »»ÁË·½Ïò£¬ZÖá·½Ïò²»¶Ô£¬ byYJY 2018Äê11ÔÂ9ÈÕ16:11:39
	zAxisPlusNum = steps;		//ZÖáÏÂÒÆµÄ²½Êı£»
	pwmNumTim4 = 0; 		
	zAxisClkSet(Z_AXIS_RST_CLK);			//zÖáÏÂÒÆµÄËÙ¶È£»
	TIM_Cmd(TIM4, ENABLE);		//¿ªÊ¼¶¯×÷£»
	
	while(zAxisStepFlag!=2);	//¶¯×÷Íê³É£»
	zAxisStepFlag = 0;
}

void zAxisDown2toolZFlag(void)
{
	//	ZÖáÏÂÒÆ£¬ÉÏµ¶£»
	zAxisDIR_ON;				//z »»ÁË·½Ïò£¬ZÖá·½Ïò²»¶Ô£¬ byYJY 2018Äê11ÔÂ9ÈÕ16:11:39
	disableZaxisCountINT(); 	//½ûÖ¹zÖá¼ÆÊıÖĞ¶Ï£»
	zAxisClkSet(Z_AXIS_RST_CLK);			//z¹éÁãÆµÂÊÉè¶¨£»
	TIM_Cmd(TIM4, ENABLE);		//Ê¹ÄÜz¹éÁã¶¯×÷£»
	
	while(toolZFlag==1);		//¹éÁãÍê³É£»
	TIM_Cmd(TIM4, DISABLE); 	//¹éÁãÍê³É£¬PWMÍ£Ö¹£»
	enableZaxisCountINT();		//¿ªÆôzÖá¼ÆÊıÖĞ¶Ï£»
}

void zAxisUpNStep(u32 steps)
{
	//	ZÖáÉÏÒÆ£¬ÅĞ¶ÏÊÇ·ñÉÏµ¶³É¹¦£»
	zAxisDIR_OFF;				//z »»ÁË·½Ïò£¬ZÖá·½Ïò²»¶Ô£¬ byYJY 2018Äê11ÔÂ9ÈÕ16:11:39
	zAxisPlusNum = steps;		//ZÖáÏÂÒÆµÄ²½Êı£»
	pwmNumTim4 = 0; 

	// ÎŞ¼Ó¼õËÙÔË¶¯
	zAxisClkSet(Z_AXIS_RST_CLK);			//zÖáÏÂÒÆµÄËÙ¶È£»
	
	TIM_Cmd(TIM4, ENABLE);		//¿ªÊ¼¶¯×÷£»
	
	while(zAxisStepFlag!=2);	//¶¯×÷Íê³É£»
	zAxisStepFlag = 0;
}

void zAxisUp2zZeroFlag(void)
{
	//	ZÖáÉÏÒÆ£¬ÅĞ¶ÏÊÇ·ñÉÏµ¶³É¹¦£»
	zAxisDIR_OFF;				//z »»ÁË·½Ïò£¬ZÖá·½Ïò²»¶Ô£¬ byYJY 2018Äê11ÔÂ9ÈÕ16:11:39
	disableZaxisCountINT(); 	//½ûÖ¹zÖá¼ÆÊıÖĞ¶Ï£»
	zAxisClkSet(Z_AXIS_RST_CLK);			//z¹éÁãÆµÂÊÉè¶¨£»
	TIM_Cmd(TIM4, ENABLE);		//Ê¹ÄÜz¹éÁã¶¯×÷£»
	
	while(zZeroFlag==1);		//¹éÁãÍê³É£»
	TIM_Cmd(TIM4, DISABLE); 	//¹éÁãÍê³É£¬PWMÍ£Ö¹£»
	enableZaxisCountINT();		//¿ªÆôzÖá¼ÆÊıÖĞ¶Ï£»
}

void sendDataFillIn(u8 value)
{
	//»»µ¶³É¹¦·¢ËÍĞÅÏ¢£»
	backResString[0] = FrameB1;
	backResString[1] = FrameB2;
	backResString[2] = CONTROLINFO;
	
	backResString[3] = 9;
	backResString[4] = 0x00;
	
	backResString[5] = 3+targeToolNo;
	backResString[6] = 0x00;
	
	backResString[7] = value;
	
	backResString[8] = FrameEnd;
}

void sendMessage2PC(void)
{
	u8 i=0;
	for(i=0;i<9;i++)
		{
			USART_SendData(USART3,backResString[i]); 
			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)!=SET);		//µÈ´ı·¢ËÍ½áÊø
		}
}

// ÍÑµ¶
u8 cleanTools(u8 toolNo)
{
	if(mainMoterToolsFlag==0)		//0:ÓĞµ¶£»	1:ÎŞµ¶£» ÕâÀïĞèÒªÅĞ¶ÏÓĞÎŞµ¶
	{		
		sendDataFillIn(0x02);
		sendMessage2PC();
		return 1;
	}
	else if(mainMoterToolsFlag==1)
	{
		mainMotorStop();	// µ¶ÖáÍ£Ö¹×ª¶¯
		delay_s(2);

		// ´Ó¹¤¼Ó¹¤µãµ½Áãµã
		stepsTemp.xSteps[0] = 0;					// ¼ÆÊıÖÃÁã
		stepsTemp.ySteps[0] = 0;
		stepsTemp.zSteps[0] = 0;					
		axis3Back2ORGWithCnt();									// »Øµ½Áãµã£¬ ´ø¼Æ²½ÊıµÄ¹¦ÄÜ	
		dataFromTemp2Saved(0);									// Êı¾İ±£´æ	
		delay_s(2);
		delay_s(5);	// ·½±ã¹Û²ì
		
		// ´ÓÁãµãµ½µ¶¾ßÎ»ÖÃ
		GPIO_SetBits(GPIOB,GPIO_Pin_12);			// ËÉ¿ªµ¶¾ß
		delay_s(2);
		
		fromORG2Tools(usingToolNo);		// Áãµãµ½µ¶¾ß
		delay_s(2);
		delay_s(5);	// ·½±ã¹Û²ì
		
		/* ÉÏµ¶¾ß£º¿ÉÄÜĞèÒªµ¶ÖáÖ÷»úÊÊµ±Âı×ªÒ»ÏÂ²ÅÄÜ¶ÔÎ»£¬ZÖá¿ÉÄÜĞèÒªÍùÏÂ×ßÒ»µã£¬ byYJY */	
		
		GPIO_ResetBits(GPIOB,GPIO_Pin_12);			// ¼Ğ½ôµ¶¾ß£¬ÒÔ±ãÍÑµ¶
		delay_s(2);
		
		// ZÖáÉÏÒÆÒ»¶Î¾àÀë£¬ÔÙÅĞ¶ÏÊÇ·ñÓĞµ¶¾ßÔÚ
		zAxisUpNStep(10000);									// ÕâÀïÓ¦¸Ã°Î³öµ¶¾ß£¬ÍÑµ¶³É¹¦
		delay_s(2);		
		delay_s(5);	// ·½±ã¹Û²ì
		
		if( 1 /*mainMoterToolsFlag == 0*/)		//ÉÏµ¶³É¹¦	ÔİÊ±Ä¬ÈÏÉÏµ¶¶¼³É¹¦ byYJY
		{
			// ·µ»ØÔ­µã£¬¸ù¾İ¼ÇÂ¼µÄÂö³åÔ­Â··µ»Ø£¬ĞèÔö¼Óº¯Êı 
			#if TEST_STEPSNUM
				stepsTemp.xSteps[targeToolNo] = 0;	// ¼ÆÊıÖÃÁã
				stepsTemp.ySteps[targeToolNo] = 0;	// ¼ÆÊıÖÃÁã
				stepsTemp.zSteps[targeToolNo] = 0;	// ¼ÆÊıÖÃÁã
				axis3Back2ORGWithCnt();
				dataFromTemp2Saved(targeToolNo);									// Êı¾İ±£´æ	
			#else
				axis3Back2ORG();
			#endif
			delay_s(2);			
			delay_s(5);	// ·½±ã¹Û²ì
			
			sendDataFillIn(0x01);
			sendMessage2PC();
			
			// ·µ»Ø¹¤¼şµÄÎ»ÖÃ
			toolsFromORG2Start();
			delay_s(2);			
			
			// ÉèÖÃ±êÖ¾Î»
			xAxisStepFlag = 2;
			yAxisStepFlag = 2;
			zAxisStepFlag = 2;
			
			return 0;
		}
		else 							//ÉÏµ¶Ê§°Ü	1  ÔÙĞı×ª×Å»ºÂıÍùÏÂ£¬ÔÙ´ÎÊÔ×ÅÉÏµ¶
		{			
			if(mainMoterToolsFlag == 0) //ÉÏµ¶³É¹¦
			{
				/* ²»·µ»ØÔ­µã£¬¸ù¾İ¼ÇÂ¼µÄÂö³åÔ­Â··µ»Ø£¬ĞèÔö¼Óº¯Êı */
				
				sendDataFillIn(0x01);
				sendMessage2PC();
				return 0;
			}
			else						//ÉÏµ¶Ê§°Ü	2
			{
				if(mainMoterToolsFlag == 0) //ÉÏµ¶³É¹¦
				{
					/* ²»·µ»ØÔ­µã£¬¸ù¾İ¼ÇÂ¼µÄÂö³åÔ­Â··µ»Ø£¬ĞèÔö¼Óº¯Êı */
			
					sendDataFillIn(0x01);
					sendMessage2PC();
					return 0;
				}
				else						//ÉÏµ¶Ê§°Ü	3
				{
					sendDataFillIn(0x02);
					sendMessage2PC();
					return 1;
				}
			}
		}		
	}
}

// ×°µ¶
u8 assemTools(u8 toolNo)
{
	if(mainMoterToolsFlag==0)		//0:ÓĞµ¶£»	1:ÎŞµ¶£»
	{		
		sendDataFillIn(0x02);
		sendMessage2PC();
		return 1;
	}
	else if(mainMoterToolsFlag==1)
	{
		mainMotorStop();	// µ¶ÖáÍ£Ö¹×ª¶¯
		delay_s(2);

		/* ĞÂµÄ»»µ¶Á÷³Ì£¬byYJY 2018Äê11ÔÂ9ÈÕ13:02:23  */	
		// ´Ó¹¤¼Ó¹¤µãµ½Áãµã
		stepsTemp.xSteps[0] = 0;					// ¼ÆÊıÖÃÁã
		stepsTemp.ySteps[0] = 0;
		stepsTemp.zSteps[0] = 0;					
		axis3Back2ORGWithCnt();									// »Øµ½Áãµã£¬ ´ø¼Æ²½ÊıµÄ¹¦ÄÜ	
		dataFromTemp2Saved(0);									// Êı¾İ±£´æ	
		delay_s(2);
		delay_s(5);	// ·½±ã¹Û²ì
		// ´ÓÁãµãµ½µ¶¾ßÎ»ÖÃ
//		stepsTemp.xSteps[targeToolNo] = changeToolsSavedSteps.xSteps[targeToolNo];	// ¶Á³öÉèÖÃµÄÁãµãµ½µ¶¾ßµÄ²½Êı
//		stepsTemp.ySteps[targeToolNo] = changeToolsSavedSteps.xSteps[targeToolNo];
//		stepsTemp.zSteps[targeToolNo] = changeToolsSavedSteps.xSteps[targeToolNo];		
//		// XÖáÒÆ¶¯¹Ì¶¨²½Êıµ½µ¶¾ßÎ»ÖÃ
//		xAxisUpNStep(stepsTemp.xSteps[targeToolNo]);	
//		yAxisUpNStep(stepsTemp.ySteps[targeToolNo]);
//		zAxisDownNStep(stepsTemp.zSteps[targeToolNo]);									// ZÖáÏÂÒÆ£¬ºóĞø¿ÉÄÜĞèÒªÂı×ªÍùÏÂ
		// µ½µ¶¾ßÎ»ÖÃÖ®Ç°¾Í´ò¿ªµ¶¾ß¼Ğ×ùµÄµç´ÅÌú
		GPIO_SetBits(GPIOB,GPIO_Pin_12);			// ËÉ¿ªµ¶¾ß
		delay_s(2);
		
		fromORG2Tools(targeToolNo);
		delay_s(2);
		delay_s(5);	// ·½±ã¹Û²ì
		
		/* ÉÏµ¶¾ß£º¿ÉÄÜĞèÒªµ¶ÖáÖ÷»úÊÊµ±Âı×ªÒ»ÏÂ²ÅÄÜ¶ÔÎ»£¬ZÖá¿ÉÄÜĞèÒªÍùÏÂ×ßÒ»µã£¬ byYJY */	
			

		// Ôö¼Óµ¶¾ß×óÒÆ¡¢ÓÒÒÆ0.5mmµÄ¹¦ÄÜ
//		xAxisUpNStep(200);										// 400¸öÂö³å×ß1mm
//		delay_s(2);
//		xAxisDownNStep(200);
//		delay_s(2);
		
		// ZÖáÉÏÒÆÒ»¶Î¾àÀë£¬ÔÙÅĞ¶ÏÊÇ·ñÓĞµ¶¾ßÔÚ
		zAxisUpNStep(10000);
		delay_s(2);		
		delay_s(5);	// ·½±ã¹Û²ì
		
		if( 1 /*mainMoterToolsFlag == 0*/)		//ÉÏµ¶³É¹¦	ÔİÊ±Ä¬ÈÏÉÏµ¶¶¼³É¹¦ byYJY
		{
			// ·µ»ØÔ­µã£¬¸ù¾İ¼ÇÂ¼µÄÂö³åÔ­Â··µ»Ø£¬ĞèÔö¼Óº¯Êı 
			#if TEST_STEPSNUM
				stepsTemp.xSteps[targeToolNo] = 0;	// ¼ÆÊıÖÃÁã
				stepsTemp.ySteps[targeToolNo] = 0;	// ¼ÆÊıÖÃÁã
				stepsTemp.zSteps[targeToolNo] = 0;	// ¼ÆÊıÖÃÁã
				axis3Back2ORGWithCnt();
				dataFromTemp2Saved(targeToolNo);									// Êı¾İ±£´æ	
			#else
				axis3Back2ORG();
			#endif
			delay_s(2);			
			delay_s(5);	// ·½±ã¹Û²ì
			
			GPIO_ResetBits(GPIOB,GPIO_Pin_12);			// ¼Ğ½ôÊ£Óàµ¶¾ß
			sendDataFillIn(0x01);
			sendMessage2PC();
			
			// ·µ»Ø¹¤¼şµÄÎ»ÖÃ
			toolsFromORG2Start();
			delay_s(2);			
			
			// ÉèÖÃ±êÖ¾Î»
			xAxisStepFlag = 2;
			yAxisStepFlag = 2;
			zAxisStepFlag = 2;
			
			// ²âÊÔÓÃ£¬½øÈëÍÑµ¶Ñ­»·
//			cleanTools();
			
			return 0;
		}
		else 							//ÉÏµ¶Ê§°Ü	1  ÔÙĞı×ª×Å»ºÂıÍùÏÂ£¬ÔÙ´ÎÊÔ×ÅÉÏµ¶
		{			
			if(mainMoterToolsFlag == 0) //ÉÏµ¶³É¹¦
			{
				/* ²»·µ»ØÔ­µã£¬¸ù¾İ¼ÇÂ¼µÄÂö³åÔ­Â··µ»Ø£¬ĞèÔö¼Óº¯Êı */
			
				sendDataFillIn(0x01);
				sendMessage2PC();
				return 0;
			}
			else						//ÉÏµ¶Ê§°Ü	2
			{
				if(mainMoterToolsFlag == 0) //ÉÏµ¶³É¹¦
				{
					/* ²»·µ»ØÔ­µã£¬¸ù¾İ¼ÇÂ¼µÄÂö³åÔ­Â··µ»Ø£¬ĞèÔö¼Óº¯Êı */
			
					sendDataFillIn(0x01);
					sendMessage2PC();
					return 0;
				}
				else						//ÉÏµ¶Ê§°Ü	3
				{
					sendDataFillIn(0x02);
					sendMessage2PC();
					return 1;
				}
			}
		}		
	}
}

// »»µ¶1
u8 changeToolsStep1(void)
{
	// ÍÑµ¶
	cleanTools(usingToolNo);
}

// »»µ¶2
u8 changeToolsStep2(void)
{
	// ×°µ¶
	assemTools(targeToolNo);
}

// ·Ö¶ÎÊ½¼Ó¼õËÙ-»»µ¶¼°»ØÁãÓÃ£¬ nAxis ±íÊ¾Ä³Ò»Öá 0-x, 1-y, 2-z
void setSpeedRST_Seg(u16 clk, u8 nAxis)
{
	// ÆäÓàÖáÖÃÁã
	if(0 != nAxis)
	{
		xAxisPlusNum = 0;
		xAxisClk = 0;
	} 
	else if(1 != nAxis) {
		yAxisPlusNum = 0;
		yAxisClk = 0;
	} 
	else if(2 != nAxis) {
		zAxisPlusNum = 0;
		zAxisClk = 0;
	} 
	
	addSubSpeed_Cur();
}

