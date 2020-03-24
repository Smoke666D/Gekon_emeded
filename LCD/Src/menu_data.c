/*
 * menu_data.c
 *
 *  Created on: Mar 2, 2020
 *      Author: igor.dymov
 */
#include "menu.h"
#include "rtc.h"
#include "data_manager.h"

extern xScreenSetObject xMainMenu;
extern xScreenSetObject xGeneratorMenu;
extern xScreenSetObject xEngineMenu;
extern xScreenSetObject xNetMenu;

extern void xInfoScreenCallBack(xScreenSetObject * menu, char key);
extern void xInputScreenKeyCallBack(xScreenSetObject * menu, char key);



static char EventLog[][44]=
{
  "Вспомог. входы",
  "Аналог. вход",
  "Аналог. вход высокий",
  "Аналог. вход низкий",
  "Размык. цепи",
  "Ошибка запуска",
  "Масло низк. дав.",
  "Высокая темп. двиг.",
  "Понижена скорость",
  "Повышеная скорость",
  "Сбой зарядки",
  "Низка скорость вент.",
  "Топливо низк. уров.",
  "Топливо высок. уров.",
  "Напряж. ген. низк",
  "Напряж. ген. высок",
  "Напряжение ген.",

};




#define MENU_LEVEL1_COUNT    8
#define ENGINE_MENU_COUNT 	 3
#define NET_MENU_COUNT	 	 3
#define GENERATOR_MENU_COUNT 7
#define MENU_ADD_COUNT 		 1
#define MAX_HEADER_STRING_SIZE 40

#define LINE4_HIGTH    64/4
#define LINE5_HIGTH    64/5


#define LEFT_OFFSET  2
#define LINE1  12
#define LINE2  30
#define LINE3  45
#define LINE4  60
#define FONT_SIZE 6


static uint8_t Header[]={0,1,CENTER_ALIGN};
static uint8_t RigthText[]={0,1,RIGTH_ALIGN};
static uint8_t LeftText[]= {0,1,LEFT_ALIGN};
static uint8_t HeaderParam[]={1,0,CENTER_ALIGN,0};
static uint8_t HeaderParam1[]={0,0,RIGTH_ALIGN,0};
static uint8_t HeaderParam2[]={0,1,LEFT_ALIGN,0};
static uint8_t InputParam[]={0,1,CENTER_ALIGN,0};
static uint8_t InputParam1[]={0,1,CENTER_ALIGN,0};
static uint8_t InputParam2[]={0,1,CENTER_ALIGN,0};
static uint8_t InputParam3[]={0,1,CENTER_ALIGN,0};

extern void GetTime(char * Data);
extern void GetInt(char * Data);
extern void vdmGetData(uint8_t command, uint16_t DataID,uint8_t * pchTextString);
extern void vGetTestData(DATA_COMMNAD_TYPE cmd, char * Data, uint8_t ID);

/*

static xScreenObjet ScreenLev1_1[]=
{  {0,0,0,128,LINE4_HIGTH,STRING,HeaderParam,HEADERSTRINGS[0],NULL,0},
   {0,0,LINE4_HIGTH+1,128,LINE4_HIGTH,HW_DATA,HeaderParam,NULL,(void*)&vRTCGetTime,0},
   {0,10,2*(LINE4_HIGTH+1),15,LINE4_HIGTH,INPUT_HW_DATA,InputParam1,NULL,(void*)&vRTCCorrectTime,RTC_HOUR},
   {0,25,2*(LINE4_HIGTH+1),9,LINE4_HIGTH,STRING,InputParam,":",NULL,0},
   {0,35,2*(LINE4_HIGTH+1),15,LINE4_HIGTH,INPUT_HW_DATA,InputParam2,NULL,&vRTCCorrectTime,RTC_MIN},
   {0,50,2*(LINE4_HIGTH+1),9,LINE4_HIGTH,STRING,InputParam,":",NULL,0},
   {0,60,2*(LINE4_HIGTH+1),15,LINE4_HIGTH,INPUT_HW_DATA,InputParam3,NULL,&vRTCCorrectTime,RTC_SEC},
   {1,0,0,0,0,STRING,NULL,NULL}	};
*/

//
static xScreenObjet EngineMainScreen[]=
{
   {0,LEFT_OFFSET,LINE1,0,0,TEXT_STRING,LeftText,"ДВИГАТЕЛЬ - Сводные",NULL,0},
   {0,0,(LINE4_HIGTH+1),128,(LINE4_HIGTH+1),LINE,Header,NULL,NULL,0},
   {0,LEFT_OFFSET,LINE2,0,0,TEXT_STRING,LeftText,"Давление",NULL,0},
   {0,67,(LINE4_HIGTH+2),30,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,1},
   {0,FONT_SIZE*17,LINE2,0,0,TEXT_STRING,LeftText,"Бар",NULL,0},
   {0,LEFT_OFFSET,LINE3,0,0,TEXT_STRING,LeftText,"Температура",NULL,0},
   {0,67,LINE2+2,30,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,2},
   {0,FONT_SIZE*17,LINE3,0,0,TEXT_STRING,LeftText,"гр.С",NULL,0},
   {0,LEFT_OFFSET,LINE4,0,0,TEXT_STRING,LeftText,"Скорость",NULL,0},
   {0,67,LINE3+3,30,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,3},
   {1,FONT_SIZE*17,LINE4,0,0,TEXT_STRING,LeftText,"об/м",NULL,0}

};

static xScreenObjet Engine1Screen[]=
{
   {0,LEFT_OFFSET,LINE1,0,0,TEXT_STRING,LeftText,"ДВИГАТЕЛЬ",NULL,0},
   {0,0,(LINE4_HIGTH+1),128,(LINE4_HIGTH+1),LINE,Header,NULL,NULL,0},
   {0,LEFT_OFFSET,LINE2,0,0,TEXT_STRING,LeftText,"Напряжение АКБ",NULL,0},
   {0,FONT_SIZE*15,(LINE4_HIGTH+2),30,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,11},
   {0,FONT_SIZE*20,LINE2,0,0,TEXT_STRING,LeftText,"В",NULL,0},
   {0,LEFT_OFFSET,LINE3,0,0,TEXT_STRING,LeftText,"Напряж.зар.ген.",NULL,0},
   {0,FONT_SIZE*15,LINE2+3,30,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,12},
   {0,FONT_SIZE*20,LINE3,0,0,TEXT_STRING,LeftText,"В",NULL,0},
   {0,LEFT_OFFSET,LINE4,0,0,TEXT_STRING,LeftText,"Уровень топлива",NULL,0},
   {0,FONT_SIZE*16,LINE3+3,20,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,13},
   {1,FONT_SIZE*20,LINE4,0,0,TEXT_STRING,LeftText,"%",NULL,0}

};

static xScreenObjet Engine2Screen[]=
{
   {0,LEFT_OFFSET,LINE1,0,0,TEXT_STRING,LeftText,"ДВИГАТЕЛЬ",NULL,0},
   {0,0,(LINE4_HIGTH+1),128,(LINE4_HIGTH+1),LINE,Header,NULL,NULL,0},
   {0,LEFT_OFFSET,LINE2,0,0,TEXT_STRING,LeftText,"Наработка",NULL,0},
   {0,FONT_SIZE*14,(LINE4_HIGTH+2),30,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,15},
   {0,FONT_SIZE*20,LINE2,0,0,TEXT_STRING,LeftText,"Ч",NULL,0},
   {0,LEFT_OFFSET,LINE3,0,0,TEXT_STRING,LeftText,"Кол-во запусков",NULL,0},
   {0,FONT_SIZE*16,LINE2+3,30,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,14},
   {0,LEFT_OFFSET,LINE4,0,0,TEXT_STRING,LeftText,"Уровень топлива",NULL,0},
   {0,FONT_SIZE*16,LINE3+3,20,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,13},
   {1,FONT_SIZE*20,LINE4,0,0,TEXT_STRING,LeftText,"%",NULL,0}

};



static xScreenObjet GeneratorMainScreen[]=
{
   {0,LEFT_OFFSET,LINE1,0,0,TEXT_STRING,LeftText,"ГЕНЕРАТОР",NULL,0},
   {0,0,(LINE4_HIGTH+1),128,(LINE4_HIGTH+1),LINE,Header,NULL,NULL,0},
   {0,LEFT_OFFSET,LINE2,0,0,TEXT_STRING,LeftText,"Напряжение",NULL,0},
   {0,FONT_SIZE*11,LINE1+6,30,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4},
   {0,FONT_SIZE*17,LINE2,0,0,TEXT_STRING,LeftText,"В",NULL,0},
   {0,LEFT_OFFSET,LINE3,0,0,TEXT_STRING,LeftText,"Ток",NULL,0},
   {0,FONT_SIZE*11,LINE2+3,30,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,2},
   {0,FONT_SIZE*17,LINE3,0,0,TEXT_STRING,LeftText,"А",NULL,0},
   {0,LEFT_OFFSET,LINE4,0,0,TEXT_STRING,LeftText,"Частота",NULL,0},
   {0,FONT_SIZE*11,LINE3+3,30,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,5},
   {1,FONT_SIZE*17,LINE4,0,0,TEXT_STRING,LeftText,"Гц",NULL,0}

};


static xScreenObjet Generator1Screen[]=
{
   {0,LEFT_OFFSET,LINE1,0,0,TEXT_STRING,LeftText,"ГЕНЕРАТОР - U лин.",NULL,0},
   {0,0,(LINE4_HIGTH+1),128,(LINE4_HIGTH+1),LINE,Header,NULL,NULL,0},
   {0,LEFT_OFFSET,LINE2,0,0,TEXT_STRING,LeftText,"L1-L2 -",NULL,0},
   {0,FONT_SIZE*7,LINE1+6,20,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4},
   {0,FONT_SIZE*12,LINE2,0,0,TEXT_STRING,LeftText,"В",NULL,0},
   {0,LEFT_OFFSET,LINE3,0,0,TEXT_STRING,LeftText,"L2-L3 -",NULL,0},
   {0,FONT_SIZE*7,LINE2+3,20,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4},
   {0,FONT_SIZE*12,LINE3,0,0,TEXT_STRING,LeftText,"В",NULL,0},
   {0,LEFT_OFFSET,LINE4,0,0,TEXT_STRING,LeftText,"L3-L1 -",NULL,0},
   {0,FONT_SIZE*7,LINE3+3,20,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4},
   {1,FONT_SIZE*12,LINE4,0,0,TEXT_STRING,LeftText,"В",NULL,0}
};

static xScreenObjet Generator2Screen[]=
{
  {0,LEFT_OFFSET,LINE1,0,0,TEXT_STRING,LeftText,"ГЕНЕРАТОР - U фазн.",NULL,0},
  {0,0,(LINE4_HIGTH+1),128,(LINE4_HIGTH+1),LINE,Header,NULL,NULL,0},
  {0,LEFT_OFFSET,LINE2,0,0,TEXT_STRING,LeftText,"L1-N -",NULL,0},
  {0,FONT_SIZE*6,LINE1+6,30,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4},
  {0,FONT_SIZE*12,LINE2,0,0,TEXT_STRING,LeftText,"В",NULL,0},
  {0,LEFT_OFFSET,LINE3,0,0,TEXT_STRING,LeftText,"L2-N -",NULL,0},
  {0,FONT_SIZE*6,LINE2+3,30,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4},
  {0,FONT_SIZE*12,LINE3,0,0,TEXT_STRING,LeftText,"В",NULL,0},
  {0,LEFT_OFFSET,LINE4,0,0,TEXT_STRING,LeftText,"L3-N -",NULL,0},
  {0,FONT_SIZE*6,LINE3+3,30,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4},
  {1,FONT_SIZE*12,LINE4,0,0,TEXT_STRING,LeftText,"В",NULL,0}

};

static xScreenObjet Generator3Screen[]=
{
  {0,LEFT_OFFSET,LINE1,0,0,TEXT_STRING,LeftText,"ГЕНЕРАТОР - Ток",NULL,0},
  {0,0,(LINE4_HIGTH+1),128,(LINE4_HIGTH+1),LINE,Header,NULL,NULL,0},
  {0,LEFT_OFFSET,LINE2,0,0,TEXT_STRING,LeftText,"L1 -",NULL,0},
  {0,FONT_SIZE*5,LINE1+6,20,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4},
  {0,FONT_SIZE*9,LINE2,0,0,TEXT_STRING,LeftText,"А",NULL,0},
  {0,LEFT_OFFSET,LINE3,0,0,TEXT_STRING,LeftText,"L2 -",NULL,0},
  {0,FONT_SIZE*5,LINE2+3,20,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4},
  {0,FONT_SIZE*9,LINE3,0,0,TEXT_STRING,LeftText,"А",NULL,0},
  {0,LEFT_OFFSET,LINE4,0,0,TEXT_STRING,LeftText,"L3 -",NULL,0},
  {0,FONT_SIZE*5,LINE3+3,20,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4},
  {1,FONT_SIZE*9,LINE4,0,0,TEXT_STRING,LeftText,"А",NULL,0}

};

static xScreenObjet Generator4Screen[]=
{
  {0,LEFT_OFFSET,LINE1,0,0,TEXT_STRING,LeftText,"ГЕНЕРАТОР - Мощность",NULL,0},
  {0,0,(LINE4_HIGTH+1),128,(LINE4_HIGTH+1),LINE,Header,NULL,NULL,0},
  {0,LEFT_OFFSET,LINE2,0,0,TEXT_STRING,LeftText,"L1 -",NULL,0},
  {0,FONT_SIZE*5,LINE1+6,20,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4},
  {0,FONT_SIZE*9,LINE2,0,0,TEXT_STRING,LeftText,"кВт",NULL,0},
  {0,LEFT_OFFSET,LINE3,0,0,TEXT_STRING,LeftText,"L2 -",NULL,0},
  {0,FONT_SIZE*5,LINE2+3,20,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4},
  {0,FONT_SIZE*9,LINE3,0,0,TEXT_STRING,LeftText,"кВт",NULL,0},
  {0,LEFT_OFFSET,LINE4,0,0,TEXT_STRING,LeftText,"L3 -",NULL,0},
  {0,FONT_SIZE*5,LINE3+3,20,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4},
  {1,FONT_SIZE*9,LINE4,0,0,TEXT_STRING,LeftText,"кВт",NULL,0}
};


static xScreenObjet Generator5Screen[]=
{
   {0,LEFT_OFFSET,LINE1,0,0,TEXT_STRING,LeftText,"ГЕНЕРАТОР - Сводные",NULL,0},
   {0,0,(LINE4_HIGTH+1),128,(LINE4_HIGTH+1),LINE,Header,NULL,NULL,0},
   {0,LEFT_OFFSET,LINE2,0,0,TEXT_STRING,LeftText,"Мощность",NULL,0},
   {0,FONT_SIZE*11,LINE1+6,30,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4},
   {0,FONT_SIZE*17,LINE2,0,0,TEXT_STRING,LeftText,"кВт",NULL,0},
   {0,FONT_SIZE*11,LINE2+3,30,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,2},
   {0,FONT_SIZE*17,LINE3,0,0,TEXT_STRING,LeftText,"кВт",NULL,0},
   {0,LEFT_OFFSET,LINE4,0,0,TEXT_STRING,LeftText,"cos f",NULL,0},
   {1,FONT_SIZE*11,LINE3+3,30,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,3}

};


static xScreenObjet Generator6Screen[]=
{
   {0,LEFT_OFFSET,LINE1,0,0,TEXT_STRING,LeftText,"ГЕНЕРАТОР",NULL,0},
   {0,0,(LINE4_HIGTH+1),128,(LINE4_HIGTH+1),LINE,Header,NULL,NULL,0},
   {0,LEFT_OFFSET,LINE3,0,0,TEXT_STRING,LeftText,"Череование фаз:",NULL,0},
   {1,FONT_SIZE*11,LINE3+3,30,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,16},
};



static xScreenObjet NetMainScreen[]=
{
   {0,LEFT_OFFSET,LINE1,0,0,TEXT_STRING,LeftText,"СЕТЬ - U лин.",NULL,0},
   {0,0,(LINE4_HIGTH+1),128,(LINE4_HIGTH+1),LINE,Header,NULL,NULL,0},
   {0,LEFT_OFFSET,LINE2,0,0,TEXT_STRING,LeftText,"L1-N - ",NULL,0},
   {0,FONT_SIZE*7,LINE1+6,20,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,7},
   {0,FONT_SIZE*11,LINE2,0,0,TEXT_STRING,LeftText,"В",NULL,0},
   {0,LEFT_OFFSET,LINE3,0,0,TEXT_STRING,LeftText,"L2-N - ",NULL,0},
   {0,FONT_SIZE*7,LINE2+3,20,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,7},
   {0,FONT_SIZE*11,LINE3,0,0,TEXT_STRING,LeftText,"В",NULL,0},
   {0,LEFT_OFFSET,LINE4,0,0,TEXT_STRING,LeftText,"L3-N - ",NULL,0},
   {0,FONT_SIZE*7,LINE3+3,20,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,7},
   {1,FONT_SIZE*11,LINE4,0,0,TEXT_STRING,LeftText,"В",NULL,0}

};

static xScreenObjet Net1Screen[]=
{
   {0,LEFT_OFFSET,LINE1,0,0,TEXT_STRING,LeftText,"СЕТЬ - U фазн.",NULL,0},
   {0,0,(LINE4_HIGTH+1),128,(LINE4_HIGTH+1),LINE,Header,NULL,NULL,0},
   {0,LEFT_OFFSET,LINE2,0,0,TEXT_STRING,LeftText,"L1-L2 - ",NULL,0},
   {0,FONT_SIZE*8,LINE1+6,20,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4},
   {0,FONT_SIZE*12,LINE2,0,0,TEXT_STRING,LeftText,"В",NULL,0},
   {0,LEFT_OFFSET,LINE3,0,0,TEXT_STRING,LeftText,"L2-L3 - ",NULL,0},
   {0,FONT_SIZE*8,LINE2+3,20,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4},
   {0,FONT_SIZE*12,LINE3,0,0,TEXT_STRING,LeftText,"В",NULL,0},
   {0,LEFT_OFFSET,LINE4,0,0,TEXT_STRING,LeftText,"L3-L1 - ",NULL,0},
   {0,FONT_SIZE*8,LINE3+3,20,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4},
   {1,FONT_SIZE*12,LINE4,0,0,TEXT_STRING,LeftText,"В",NULL,0}
};

static xScreenObjet Net2Screen[]=
{
   {0,LEFT_OFFSET,LINE1,0,0,TEXT_STRING,LeftText,"СЕТЬ - Частота.",NULL,0},
   {0,0,(LINE4_HIGTH+1),128,(LINE4_HIGTH+1),LINE,Header,NULL,NULL,0},
   {0,LEFT_OFFSET,LINE2,0,0,TEXT_STRING,LeftText,"Частота - ",NULL,0},
   {0,FONT_SIZE*10,LINE1+6,15,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,5},
   {0,FONT_SIZE*13,LINE2,0,0,TEXT_STRING,LeftText,"Гц",NULL,0},
   {0,LEFT_OFFSET,LINE3,0,0,TEXT_STRING,LeftText,"Чередование фаз:",NULL,0},
   {1,LEFT_OFFSET,LINE3+3,50,LINE4_HIGTH,HW_DATA,LeftText,NULL,(void*)&vGetTestData,16}
};



static xScreenObjet LinkMainScreen[]=
{
   {0,LEFT_OFFSET,LINE1,0,0,TEXT_STRING,LeftText,"СВЯЗЬ Ethernet",NULL,0},
   {0,0,(LINE4_HIGTH+1),128,(LINE4_HIGTH+1),LINE,Header,NULL,NULL,0},
   {0,LEFT_OFFSET,LINE2,0,0,TEXT_STRING,LeftText,"IP ",NULL,0},
   {0,FONT_SIZE*6,LINE1+6,100,LINE4_HIGTH,HW_DATA,LeftText,NULL,(void*)&vGetTestData,8},
   {0,LEFT_OFFSET,LINE3,0,0,TEXT_STRING,LeftText,"Маска",NULL,0},
   {0,FONT_SIZE*6,LINE2+3,100,LINE4_HIGTH,HW_DATA,LeftText,NULL,(void*)&vGetTestData,9},
   {0,LEFT_OFFSET,LINE4,0,0,TEXT_STRING,LeftText,"Шлюз",NULL,0},
   {1,FONT_SIZE*6,LINE3+3,100,LINE4_HIGTH,HW_DATA,LeftText,NULL,(void*)&vGetTestData,10}

};

static xScreenObjet AlarmMainScreen[]=
{
   {0,LEFT_OFFSET,LINE1,0,0,TEXT_STRING,LeftText,"АВАРИИ",NULL,0},
   {1,0,(LINE4_HIGTH+1),128,(LINE4_HIGTH+1),LINE,Header,NULL,NULL,0}
};

static xScreenObjet EventMainScreen[]=
{
   {0,LEFT_OFFSET,LINE1,0,0,TEXT_STRING,LeftText,"ЖУРНАЛ СОБЫТИЙ №",NULL,0},
   {1,0,(LINE4_HIGTH+1),128,(LINE4_HIGTH+1),LINE,Header,NULL,NULL,0}

};

static xScreenObjet InfoMainScreen[]=
{
   {0,LEFT_OFFSET,LINE1,0,0,TEXT_STRING,LeftText,"ВЕРСИЯ",NULL,0},
   {0,0,(LINE4_HIGTH+1),128,(LINE4_HIGTH+1),LINE,Header,NULL,NULL,0},
   {0,LEFT_OFFSET,LINE2,0,0,TEXT_STRING,LeftText,"ID",NULL,0},
   {0,LEFT_OFFSET,LINE3,0,0,TEXT_STRING,LeftText,"Тех.поддержка",NULL,0},
   {1,LEFT_OFFSET,LINE4,0,0,TEXT_STRING,LeftText,"www.energan.ru",NULL,0}

};

static xScreenObjet StatusMainScreen[]=
{
   {0,LEFT_OFFSET,LINE1,0,0,TEXT_STRING,LeftText,"СТАТУС",NULL,0},
   {1,0,(LINE4_HIGTH+1),128,(LINE4_HIGTH+1),LINE,Header,NULL,NULL,0}

};


static xScreenType  xScreensLev1[MENU_LEVEL1_COUNT]=
{
	//	{ScreenLev1_1,NULL,NULL,NOT_ACTIVE,0,0,&xInputScreenKeyCallBack},
		{EngineMainScreen,&xMainMenu,&xEngineMenu,PASSIVE,0,0,NULL},
		{GeneratorMainScreen,&xMainMenu,&xGeneratorMenu,PASSIVE,0,0,NULL},
		{NetMainScreen,&xMainMenu,&xNetMenu,PASSIVE,0,0,NULL},
		{LinkMainScreen,NULL,NULL,PASSIVE,0,0,NULL},
		{AlarmMainScreen,NULL,NULL,PASSIVE,0,0,NULL},
		{EventMainScreen,NULL,NULL,PASSIVE,0,0,NULL},
		{InfoMainScreen,NULL,NULL,PASSIVE,0,0,NULL},
		{StatusMainScreen,NULL,NULL,PASSIVE,0,0,NULL},
};

static xScreenType  xEngineScreens[ENGINE_MENU_COUNT]=
{
		{Engine1Screen,&xMainMenu,NULL,PASSIVE,0,0,NULL},
		{Engine2Screen,&xMainMenu,NULL,PASSIVE,0,0,NULL},
		{EngineMainScreen,(void*)&xMainMenu,NULL,PASSIVE,0,0,NULL},

};

static xScreenType  xGeneratorScreens[GENERATOR_MENU_COUNT]=
{
		{Generator1Screen,&xMainMenu,NULL,PASSIVE,0,0,NULL},
		{Generator2Screen,&xMainMenu,NULL,PASSIVE,0,0,NULL},
		{Generator3Screen,&xMainMenu,NULL,PASSIVE,0,0,NULL},
		{Generator4Screen,&xMainMenu,NULL,PASSIVE,0,0,NULL},
		{Generator5Screen,&xMainMenu,NULL,PASSIVE,0,0,NULL},
		{Generator6Screen,&xMainMenu,NULL,PASSIVE,0,0,NULL},
		{GeneratorMainScreen,(void*)&xGeneratorMenu,NULL,PASSIVE,0,0,NULL},

};

static xScreenType  xNetScreens[NET_MENU_COUNT]=
{
		{Net1Screen,&xMainMenu,NULL,PASSIVE,0,0,NULL},
		{Net2Screen,&xMainMenu,NULL,PASSIVE,0,0,NULL},
		{NetMainScreen,(void*)&xMainMenu,NULL,PASSIVE,0,0,NULL},

};


