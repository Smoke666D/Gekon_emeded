/*
 * menu_data.c
 *
 *  Created on: Mar 2, 2020
 *      Author: igor.dymov
 */
#include "menu.h"
#include "rtc.h"
#include "data_manager.h"

extern xScreenSetObject xGeneratorMenu;
extern xScreenSetObject xEngineMenu;

extern void xInputScreenKeyCallBack(xScreenSetObject * menu, char key);

#define MENU_LEVEL1_COUNT 10
#define MENU_LEVEL2_COUNT 7
#define MENU_ADD_COUNT 1
#define HEAD_STRINGS_COUNT MENU_LEVEL1_COUNT+MENU_LEVEL2_COUNT+MENU_ADD_COUNT
#define MAX_HEADER_STRING_SIZE 40

#define LINE4_HIGTH    64/4
#define LINE5_HIGTH    64/5

static uint8_t BufferString[30];
static uint8_t HeaderParam[]={0,0,CENTER_ALIGN,0};
static uint8_t HeaderParam1[]={0,0,RIGTH_ALIGN,0};
static uint8_t HeaderParam2[]={1,0,LEFT_ALIGN,0};
static uint8_t InputParam[]={0,1,CENTER_ALIGN,0};
static uint8_t InputParam1[]={0,1,CENTER_ALIGN,0};
static uint8_t InputParam2[]={0,1,CENTER_ALIGN,0};
static uint8_t InputParam3[]={0,1,CENTER_ALIGN,0};

extern void GetTime(char * Data);
extern void GetInt(char * Data);
extern void vdmGetData(uint8_t command, uint16_t DataID,uint8_t * pchTextString);

static uint8_t HEADERSTRINGS[HEAD_STRINGS_COUNT][MAX_HEADER_STRING_SIZE]=
{
		{"НАПРЯЖ. ГЕН. И СЕТИ"}, //1
		{"ПРИБОРЫ ГЕНЕРАТОРА"},  //2
		{"ПРИБОРЫ СЕТИ"},        //3
		{"ПРИБОРЫ НАГР. СЕТИ"},  //4
		{"ПРИБ. НАГР. И ТОКА"},  //5
		{"ПРИБ. ДВИГАТЕЛЯ"},     //6
		{"ИНФОРАЦИЯ О МОДУЛЕ"},  //7
		{"ДИАГНОСТ. ДВИГАТЕЛЯ"}, //8
		{"ЖУРНАЛ СОБЫТИЙ"},      //9
		{"ИНФ. О ДВИГАТЕЛЕ"},     //10
		{"CКОРОСТЬ ДВИГАТЕЛЯ"},  //11
		{"ВРЕМЯ РАБОТЫ ДВИГ"},   //12
		{"НАПРЯЖЕНИЕ АКК"},      //13
		{"НАПРЯЖЕНИЕ ГЕНЕР."},   //14
		{"ТЕМ. ОХЛ. ЖИДКОСТИ"},  //15
		{"ДАВЛЕНИЕ МАСЛА"},      //16
		{"УРОВЕНЬ ТОПЛИВА"},     //17
		{"уровень топлива"},	 //18
};

static xScreenObjet ScreenLev1_1[]=
{  {0,0,0,128,LINE4_HIGTH,STRING,HeaderParam,HEADERSTRINGS[0],NULL,0},
   {0,0,LINE4_HIGTH+1,128,LINE4_HIGTH,HW_DATA,HeaderParam,NULL,&vRTCGetTime,0},
   {0,10,2*(LINE4_HIGTH+1),15,LINE4_HIGTH,INPUT_HW_DATA,InputParam1,NULL,&vRTCCorrectTime,RTC_HOUR},
   {0,25,2*(LINE4_HIGTH+1),9,LINE4_HIGTH,STRING,InputParam,":",NULL,0},
   {0,35,2*(LINE4_HIGTH+1),15,LINE4_HIGTH,INPUT_HW_DATA,InputParam2,NULL,&vRTCCorrectTime,RTC_MIN},
   {0,50,2*(LINE4_HIGTH+1),9,LINE4_HIGTH,STRING,InputParam,":",NULL,0},
   {0,60,2*(LINE4_HIGTH+1),15,LINE4_HIGTH,INPUT_HW_DATA,InputParam3,NULL,&vRTCCorrectTime,RTC_SEC},
   {1,0,0,0,0,STRING,NULL,NULL}	};

static xScreenObjet ScreenLev1_2[]=
{
   {0,0,0,128,LINE5_HIGTH,STRING,HeaderParam,HEADERSTRINGS[17],NULL,0},
   {0,0,LINE5_HIGTH+1,128,LINE5_HIGTH,STRING,HeaderParam,HEADERSTRINGS[17],NULL,0},
   {0,0,2*(LINE5_HIGTH+1),128,LINE5_HIGTH,STRING,HeaderParam,HEADERSTRINGS[17],NULL,0},
   {0,0,3*(LINE5_HIGTH+1),128,LINE5_HIGTH,STRING,HeaderParam,HEADERSTRINGS[17],NULL,0},
   {0,0,4*(LINE5_HIGTH+1),128,LINE5_HIGTH,STRING,HeaderParam,HEADERSTRINGS[17],NULL,0},
   {1,0,0,0,0,STRING,NULL,NULL}	};

static xScreenObjet ScreenLev1_3[]=
{  {0,0,0,128,LINE4_HIGTH,STRING,HeaderParam,HEADERSTRINGS[2],NULL,0},
   {0,0,2*(LINE4_HIGTH+1),128,LINE4_HIGTH,INPUT_DATA_STRING,InputParam,BufferString,&vdmGetData,ID_TEST_DATA },
   {1,0,0,0,0,STRING,NULL,NULL}	};

static xScreenObjet ScreenLev1_4[]=
{  {0,0,0,128,16,STRING,HeaderParam,HEADERSTRINGS[3],NULL,0},
   {1,0,0,0,0,STRING,NULL,NULL}	};

static xScreenObjet ScreenLev1_5[]=
{  {0,0,0,128,16,STRING,HeaderParam,HEADERSTRINGS[4],NULL,0},
   {1,0,0,0,0,STRING,NULL,NULL}	};

static xScreenObjet ScreenLev1_6[]=
{  {0,0,0,128,16,STRING,HeaderParam,HEADERSTRINGS[5],NULL,0},
   {1,0,0,0,0,STRING,NULL,NULL}	};

static xScreenObjet ScreenLev1_7[]=
{  {0,0,0,128,16,STRING,HeaderParam,HEADERSTRINGS[6],NULL,0},
   {1,0,0,0,0,STRING,NULL,NULL}	};

static xScreenObjet ScreenLev1_8[]=
{  {0,0,0,128,16,STRING,HeaderParam,HEADERSTRINGS[7],NULL,0},
   {1,0,0,0,0,STRING,NULL,NULL}	};

static xScreenObjet ScreenLev1_9[]=
{  {0,0,0,128,16,STRING,HeaderParam,HEADERSTRINGS[8],NULL,0},
   {1,0,0,0,0,STRING,NULL,NULL}	};

static xScreenObjet ScreenLev1_10[]=
{  {0,0,0,128,16,STRING,HeaderParam,HEADERSTRINGS[9],NULL,0},
   {1,0,0,0,0,STRING,NULL,NULL}	};

static xScreenObjet ScreenLev2_1[]=
{  {0,0,0,128,16,STRING,HeaderParam,HEADERSTRINGS[10],NULL,0},
   {1,0,0,0,0,STRING,NULL,NULL}	};

static xScreenObjet ScreenLev2_2[]=
{  {0,0,0,128,16,STRING,HeaderParam,HEADERSTRINGS[11],NULL,0},
   {1,0,0,0,0,STRING,NULL,NULL}	};

static xScreenObjet ScreenLev2_3[]=
{  {0,0,0,128,16,STRING,HeaderParam,HEADERSTRINGS[12],NULL,0},
   {1,0,0,0,0,STRING,NULL,NULL}	};

static xScreenObjet ScreenLev2_4[]=
{  {0,0,0,128,16,STRING,HeaderParam,HEADERSTRINGS[13],NULL,0},
   {1,0,0,0,0,STRING,NULL,NULL}	};

static xScreenObjet ScreenLev2_5[]=
{  {0,0,0,128,16,STRING,HeaderParam,HEADERSTRINGS[14],NULL,0},
   {1,0,0,0,0,STRING,NULL,NULL}	};

static xScreenObjet ScreenLev2_6[]=
{  {0,0,0,128,16,STRING,HeaderParam,HEADERSTRINGS[15],NULL,0},
   {1,0,0,0,0,STRING,NULL,NULL}	};

static xScreenObjet ScreenLev2_7[]=
{  {0,0,0,128,16,STRING,HeaderParam,HEADERSTRINGS[16],NULL,0},
   {1,0,0,0,0,STRING,NULL,NULL}	};


static xScreenType  xScreensLev1[MENU_LEVEL1_COUNT]=
{
		{ScreenLev1_1,NULL,NULL,NOT_ACTIVE,0,0,&xInputScreenKeyCallBack},
		{ScreenLev1_2,NULL,NULL,PASSIVE,0,0,NULL},
		{ScreenLev1_3,NULL,NULL,PASSIVE,0,0,NULL},
		{ScreenLev1_4,NULL,NULL,PASSIVE,0,0,NULL},
		{ScreenLev1_5,NULL,NULL,PASSIVE,0,0,NULL},
		{ScreenLev1_6,(void *)&xGeneratorMenu,(void *)&xEngineMenu,PASSIVE,0,0,NULL},
		{ScreenLev1_7,NULL,NULL,PASSIVE,0,0,NULL},
		{ScreenLev1_8,NULL,NULL,PASSIVE,0,0,NULL},
		{ScreenLev1_9,NULL,NULL,PASSIVE,0,0,NULL},
		{ScreenLev1_10,NULL,NULL,PASSIVE,0,0,NULL},
};

static xScreenType  xScreensLev2_1[MENU_LEVEL2_COUNT]=
{
		{ScreenLev2_1,&xGeneratorMenu,NULL,PASSIVE,0,0,NULL},
		{ScreenLev2_2,&xGeneratorMenu,NULL,PASSIVE,0,0,NULL},
		{ScreenLev2_3,&xGeneratorMenu,NULL,PASSIVE,0,0,NULL},
		{ScreenLev2_4,&xGeneratorMenu,NULL,PASSIVE,0,0,NULL},
		{ScreenLev2_5,&xGeneratorMenu,NULL,PASSIVE,0,0,NULL},
		{ScreenLev2_6,&xGeneratorMenu,NULL,PASSIVE,0,0,NULL},
		{ScreenLev2_7,&xGeneratorMenu,NULL,PASSIVE,0,0,NULL},

};




