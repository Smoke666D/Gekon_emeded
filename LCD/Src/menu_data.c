/*
 * menu_data.c
 *
 *  Created on: Mar 2, 2020
 *      Author: igor.dymov
 */
/*----------------------- Includes ------------------------------------------------------------------*/


#include "menu_data.h"
/*------------------------ Extern -------------------------------------------------------------------*/
extern xScreenSetObject xMainMenu;
extern xScreenSetObject xGeneratorMenu;
extern xScreenSetObject xEngineMenu;
extern xScreenSetObject xNetMenu;
extern xScreenSetObject xSettingsMenu;


/*----------------------- Variables -----------------------------------------------------------------*/
static char EventLog[][44U]=
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



#define MAX_HEADER_STRING_SIZE 40U
#define LINE4_HIGTH            ( 64U / 4U )
#define LINE5_HIGTH            ( 64U / 5U )

#define BRIGTH_ID    3U
#define SETTING_ID   1U
#define LEFT_OFFSET  2U
#define LINE1        12U
#define LINE2        30U
#define LINE3        45U
#define LINE4        60U
#define FONT_SIZE    6U


static uint8_t Header[]       = { 0U, 1U, CENTER_ALIGN };
static uint8_t RigthText[]    = { 0U, 1U, RIGTH_ALIGN};
static uint8_t LeftText[]     = { 0U, 1U, LEFT_ALIGN };
static uint8_t HeaderParam[]  = { 1U, 0U, CENTER_ALIGN, 0U };
static uint8_t InputParam[]   = { 0U, 1U, LEFT_ALIGN, 0U };
static uint8_t InputParam1[]  = { 0U, 1U, CENTER_ALIGN, 1U };
static uint8_t InputParam2[]  = { 0U, 1U, CENTER_ALIGN, 0U };

extern void GetTime( char* Data );
extern void GetInt( char* Data );
extern void vdmGetData( uint8_t command, uint16_t DataID, uint8_t* pchTextString );
extern void vGetTestData( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );



//
static xScreenObjet const EngineMainScreen[]=
{
  { 0U, LEFT_OFFSET, LINE1, 0U, 0U, TEXT_STRING, LeftText, "ДВИГАТЕЛЬ - Сводные", NULL, 0U },
  { 0U, 0U, ( LINE4_HIGTH + 1U ), 128U, ( LINE4_HIGTH + 1U ), LINE,Header, NULL, NULL, 0U },
  { 0U, LEFT_OFFSET,LINE2, 0U, 0U, TEXT_STRING, LeftText, "Давление", NULL, 0U },
  { 0U, 67U, ( LINE4_HIGTH + 2U ), 30U, LINE4_HIGTH, HW_DATA, RigthText, NULL, (void*)vGetDataForMenu,OIL_PRESSURE },
  { 0U, ( FONT_SIZE * 17U ), LINE2, 0U, 0U, TEXT_STRING, LeftText, "Бар", NULL, 0U},
  { 0U, LEFT_OFFSET, LINE3, 0U, 0U, TEXT_STRING, LeftText, "Температура", NULL, 0U},
  { 0U, 67U, ( LINE2 + 2U ), 30U, LINE4_HIGTH, HW_DATA, RigthText, NULL, (void*)&vGetDataForMenu, COOL_TEMP},
  { 0U, ( FONT_SIZE * 17U ), LINE3, 0U, 0U, TEXT_STRING, LeftText, "гр.С", NULL, 0U },
  { 0U, LEFT_OFFSET, LINE4, 0U, 0U, TEXT_STRING, LeftText, "Скорость", NULL, 0U },
  { 0U, 67U, ( LINE3 + 3U ), 30U, LINE4_HIGTH, HW_DATA, RigthText, NULL, (void*)&vGetDataForMenu, ENGINE_SPEED },
  { 1U, ( FONT_SIZE * 17U ), LINE4, 0U, 0U, TEXT_STRING, LeftText, "об/м", NULL, 0U }
};

static xScreenObjet const Engine1Screen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ДВИГАТЕЛЬ",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"Напряжение АКБ",NULL,0U},
  {0U,FONT_SIZE*15U,(LINE4_HIGTH+2U),30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,IN_VDD},
  {0U,FONT_SIZE*20U,LINE2,0U,0U,TEXT_STRING,LeftText,"В",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"Напряж.зар.ген.",NULL,0U},
  {0U,FONT_SIZE*15U,LINE2+3U,30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,12U},
  {0U,FONT_SIZE*20U,LINE3,0U,0U,TEXT_STRING,LeftText,"В",NULL,0U},
  {0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"Уровень топлива",NULL,0U},
  {0U,FONT_SIZE*16U,LINE3+3U,20U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,FUEL_LEVEL},
  {1U,FONT_SIZE*20U,LINE4,0U,0U,TEXT_STRING,LeftText,"%",NULL,0U}
};

static xScreenObjet const Engine2Screen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ДВИГАТЕЛЬ",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"Наработка",NULL,0U},
  {0U,FONT_SIZE*14U,(LINE4_HIGTH+2U),30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu, ENGINE_WTIME},
  {0U,FONT_SIZE*20U,LINE2,0U,0U,TEXT_STRING,LeftText,"Ч",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"Кол-во запусков",NULL,0U},
  {1U,FONT_SIZE*16U,LINE2+3U,30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,ENGINE_SCOUNT},
  //{0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"Уровень топлива",NULL,0U},
  //{0U,FONT_SIZE*16U,LINE3+3U,20U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,13U},
  //{1U,FONT_SIZE*20U,LINE4,0U,0U,TEXT_STRING,LeftText,"%",NULL,0U}
};



static xScreenObjet const GeneratorMainScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ГЕНЕРАТОР",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"Напряжение",NULL,0U},
  {0U,FONT_SIZE*11U,LINE1+6U,30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4U},
  {0U,FONT_SIZE*17U,LINE2,0U,0U,TEXT_STRING,LeftText,"В",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"Ток",NULL,0U},
  {0U,FONT_SIZE*11U,LINE2+3U,30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,2U},
  {0U,FONT_SIZE*17U,LINE3,0U,0U,TEXT_STRING,LeftText,"А",NULL,0U},
  {0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"Частота",NULL,0U},
  {0U,FONT_SIZE*11U,LINE3+3U,30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_FREQ},
  {1U,FONT_SIZE*17U,LINE4,0U,0U,TEXT_STRING,LeftText,"Гц",NULL,0U}
};


static xScreenObjet const Generator2Screen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ГЕНЕРАТОР - U лин.",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"L1-L2 -",NULL,0U},
  {0U,FONT_SIZE*7U,LINE1+6U,20U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_F1_L_VDD},
  {0U,FONT_SIZE*12U,LINE2,0U,0U,TEXT_STRING,LeftText,"В",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"L2-L3 -",NULL,0U},
  {0U,FONT_SIZE*7U,LINE2+3U,20U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_F2_L_VDD},
  {0U,FONT_SIZE*12U,LINE3,0U,0U,TEXT_STRING,LeftText,"В",NULL,0U},
  {0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"L3-L1 -",NULL,0U},
  {0U,FONT_SIZE*7U,LINE3+3U,20U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_F3_L_VDD},
  {1U,FONT_SIZE*12U,LINE4,0U,0U,TEXT_STRING,LeftText,"В",NULL,0U}
};

static xScreenObjet const Generator1Screen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ГЕНЕРАТОР - U фазн.",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"L1-N -",NULL,0U},
  {0U,FONT_SIZE*6U,LINE1+6U,30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_F1_VDD},
  {0U,FONT_SIZE*12U,LINE2,0U,0U,TEXT_STRING,LeftText,"В",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"L2-N -",NULL,0U},
  {0U,FONT_SIZE*6U,LINE2+3U,30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_F2_VDD},
  {0U,FONT_SIZE*12U,LINE3,0U,0U,TEXT_STRING,LeftText,"В",NULL,0U},
  {0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"L3-N -",NULL,0U},
  {0U,FONT_SIZE*6U,LINE3+3U,30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_F3_VDD},
  {1U,FONT_SIZE*12U,LINE4,0U,0U,TEXT_STRING,LeftText,"В",NULL,0U}
};

static xScreenObjet const Generator3Screen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ГЕНЕРАТОР - Ток",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"L1 -",NULL,0U},
  {0U,FONT_SIZE*5U,LINE1+6U,30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_F1_CUR},
  {0U,FONT_SIZE*10U,LINE2,0U,0U,TEXT_STRING,LeftText,"А",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"L2 -",NULL,0U},
  {0U,FONT_SIZE*5U,LINE2+3U,30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_F2_CUR},
  {0U,FONT_SIZE*10U,LINE3,0U,0U,TEXT_STRING,LeftText,"А",NULL,0U},
  {0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"L3 -",NULL,0U},
  {0U,FONT_SIZE*5U,LINE3+3U,30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_F3_CUR},
  {1U,FONT_SIZE*10U,LINE4,0U,0U,TEXT_STRING,LeftText,"А",NULL,0U}

};

static xScreenObjet const Generator4Screen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ГЕНЕРАТОР - Мощность",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"L1 -",NULL,0U},
  {0U,FONT_SIZE*5U,LINE1+6U,20U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4U},
  {0U,FONT_SIZE*9U,LINE2,0U,0U,TEXT_STRING,LeftText,"кВт",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"L2 -",NULL,0U},
  {0U,FONT_SIZE*5U,LINE2+3U,20U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4U},
  {0U,FONT_SIZE*9U,LINE3,0U,0U,TEXT_STRING,LeftText,"кВт",NULL,0U},
  {0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"L3 -",NULL,0U},
  {0U,FONT_SIZE*5U,LINE3+3U,20U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetTestData,4U},
  {1U,FONT_SIZE*9U,LINE4,0U,0U,TEXT_STRING,LeftText,"кВт",NULL,0U}
};


static xScreenObjet const Generator5Screen[]=
{
   {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ГЕНЕРАТОР - Сводные",NULL,0U},
   {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),LINE,Header,NULL,NULL,0U},
   {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"Мощность",NULL,0U},
   {0U,FONT_SIZE*11U,LINE1+6U,30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_POWER},
   {0U,FONT_SIZE*17U,LINE2,0U,0U,TEXT_STRING,LeftText,"кВт",NULL,0U},
   {0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"cos f",NULL,0U},
   {1U,FONT_SIZE*11U,LINE3+3U,30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,COS_FI}

};


static xScreenObjet const Generator6Screen[]=
{
   {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ГЕНЕРАТОР",NULL,0U},
   {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),LINE,Header,NULL,NULL,0U},
   {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"Череование фаз:",NULL,0U},
   {1U,FONT_SIZE*11U,LINE3+3U,30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu, GEN_ROTATION},
};



static xScreenObjet const NetMainScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"СЕТЬ - U лин.",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"L1-N - ",NULL,0U},
  {0U,FONT_SIZE*7U,LINE1+6U,20U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,NET_F1_VDD},
  {0U,FONT_SIZE*15U,LINE1+6U,20U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetADCDC,NET_FREQ+1},
  {0U,FONT_SIZE*11U,LINE2,0U,0U,TEXT_STRING,LeftText,"В",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"L2-N - ",NULL,0U},
  {0U,FONT_SIZE*7U,LINE2+3U,20U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,NET_F2_VDD},
  {0U,FONT_SIZE*15U,LINE2+3U,20U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetADCDC,ADC_FREQ+1},
  {0U,FONT_SIZE*11U,LINE3,0U,0U,TEXT_STRING,LeftText,"В",NULL,0U},
  {0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"L3-N - ",NULL,0U},
  {0U,FONT_SIZE*7U,LINE3+3U,20U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,NET_F3_VDD},
  {0U,FONT_SIZE*15U,LINE3+3U,20U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetADCDC,ADC_TEMP+1},
  {1U,FONT_SIZE*11U,LINE4,0U,0U,TEXT_STRING,LeftText,"В",NULL,0U}
};

static xScreenObjet const Net1Screen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"СЕТЬ - U фазн.",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"L1-L2 - ",NULL,0U},
  {0U,FONT_SIZE*8U,LINE1+6U,20U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,NET_F1_L_VDD},
  {0U,FONT_SIZE*12U,LINE2,0U,0U,TEXT_STRING,LeftText,"В",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"L2-L3 - ",NULL,0U},
  {0U,FONT_SIZE*8U,LINE2+3U,20U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,NET_F2_L_VDD},
  {0U,FONT_SIZE*12U,LINE3,0,0,TEXT_STRING,LeftText,"В",NULL,0U},
  {0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"L3-L1 - ",NULL,0U},
  {0U,FONT_SIZE*8U,LINE3+3U,20U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,NET_F3_L_VDD},
  {1U,FONT_SIZE*12U,LINE4,0U,0U,TEXT_STRING,LeftText,"В",NULL,0U}
};

static xScreenObjet const Net2Screen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"СЕТЬ - Частота.",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"Частота - ",NULL,0U},
  {0U,FONT_SIZE*10U,LINE1+6U,15U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,NET_FREQ},
  {0U,FONT_SIZE*13U,LINE2,0U,0U,TEXT_STRING,LeftText,"Гц",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"Чередование фаз:",NULL,0U},
  {1U,LEFT_OFFSET,LINE3+3U,50U,LINE4_HIGTH,HW_DATA,LeftText,NULL,(void*)vGetDataForMenu,NET_ROTATION}
};



static xScreenObjet const LinkMainScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"СВЯЗЬ Ethernet",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"IP ",NULL,0U},
  {01U,FONT_SIZE*6U,LINE1+6U,100U,LINE4_HIGTH,HW_DATA,LeftText,NULL,(void*)&vMenuGetData,IP_ADRESS},
};

static xScreenObjet const AlarmMainScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"АВАРИИ",NULL,0U},
  {1U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),LINE,Header,NULL,NULL,0U}
};

static xScreenObjet const EventMainScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ЖУРНАЛ СОБЫТИЙ №",NULL,0U},
  {1U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),LINE,Header,NULL,NULL,0U}
};

static xScreenObjet const EventScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"СОБЫТИE",NULL,0U},
  {1U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),LINE,Header,NULL,NULL,0U}
};






static xScreenObjet const InfoMainScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ВЕРСИЯ",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"ID",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"Тех.поддержка",NULL,0U},
  {1U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"www.energan.ru",NULL,0U}
};

static xScreenObjet const StatusMainScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"СТАТУС",NULL,0U},
  {1U, 0U, ( LINE4_HIGTH + 1U ), 128U, ( LINE4_HIGTH + 1U ), LINE, Header, NULL, NULL, 0U },
};


static xScreenObjet  const SettingsScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"НАСТРОЙКА :",NULL,0U},
  {0U, 0U, ( LINE4_HIGTH + 1U ), 128U, ( LINE4_HIGTH + 1U ), LINE, Header, NULL, NULL, 0U },
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText," ",NULL,0U},
  {0U,FONT_SIZE*13U, LINE1-11U,30U, LINE4_HIGTH, HW_DATA, InputParam,NULL,(void*)&vGetSettingsNumber, 0U},
  {0U,FONT_SIZE*16U, LINE1+6U,40U, LINE4_HIGTH, HW_DATA, InputParam,NULL,(void*)&vGetSettingsUnit, 0U},
  {1U,FONT_SIZE*5U, LINE1+6U,40U, LINE4_HIGTH, INPUT_HW_DATA, InputParam,NULL,(void*)&vGetSettingsData, 0U},
};


static xScreenObjet const SettingsMainScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"НАСТРОЙКИ",NULL,0U},
  {1U, 0U, ( LINE4_HIGTH + 1U ), 128U, ( LINE4_HIGTH + 1U ), LINE, Header, NULL, NULL, 0U },
};

static xScreenObjet const xYesNoScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ПРИМЕНИТЬ ИЗМЕНЕНИЯ",NULL,0U},
  {0U, 0U, ( LINE4_HIGTH + 1U ), 128U, ( LINE4_HIGTH + 1U ), LINE, Header, NULL, NULL, 0U },
  {0U,FONT_SIZE*3U,LINE2,30U,LINE4_HIGTH,DATA_STRING,InputParam1,"  ДА  ",NULL,0U},
  {1U,FONT_SIZE*13U,LINE2,30U,LINE4_HIGTH,DATA_STRING,InputParam2,"  НЕТ  ",NULL,0U},
};


xScreenType  xScreensLev1[MENU_LEVEL1_COUNT]=
{
  {GeneratorMainScreen,&xMainMenu,&xGeneratorMenu,0U,0U},
  {NetMainScreen,&xMainMenu,&xNetMenu,0U,0U},
  {StatusMainScreen,NULL,NULL,0U,0U},
  {EngineMainScreen,&xMainMenu,&xEngineMenu,0U,0U},
  {LinkMainScreen,NULL,NULL,0U,0U},
  {AlarmMainScreen,NULL,NULL,0U,0U},
  {EventMainScreen,&xMainMenu,NULL,0U,0U},
  {InfoMainScreen,NULL,NULL,0U,0U},
};

xScreenType  xEngineScreens[ENGINE_MENU_COUNT]=
{
  {Engine1Screen,&xMainMenu,NULL,0U,0U},
  {Engine2Screen,&xMainMenu,NULL,0U,0U},
  {EngineMainScreen,(void*)&xMainMenu,NULL,0U,0U},
};

xScreenType xGeneratorScreens[GENERATOR_MENU_COUNT]=
{
  {Generator1Screen,&xMainMenu,NULL,0U,0U},
  {Generator2Screen,&xMainMenu,NULL,0U,0U},
  {Generator3Screen,&xMainMenu,NULL,0U,0U},
  {Generator4Screen,&xMainMenu,NULL,0U,0U},
  {Generator5Screen,&xMainMenu,NULL,0U,0U},
  {Generator6Screen,&xMainMenu,NULL,0U,0U},
  {GeneratorMainScreen,(void*)&xGeneratorMenu,NULL,0U,0U},
};

xScreenType xNetScreens[NET_MENU_COUNT]=
{
  { Net1Screen, &xMainMenu, NULL,  0U, 0U},
  { Net2Screen, &xMainMenu, NULL,  0U, 0U },
  { NetMainScreen, (void*)&xMainMenu, NULL,  0U, 0U},
};



xScreenType   xEventScreens[EVENT_MENU_COUNT]=
{
  { EventScreen, &xMainMenu, NULL,  0U, 0U},
};


xScreenType  xSettingsScreens[SETTINGS_MENU_COUNT]=
{
  { SettingsScreen, (void*)&xMainMenu, NULL,  0U, 0U},
};

xScreenType   xYesNoScreens[SETTINGS_MENU_COUNT]=
{
  { xYesNoScreen, &xMainMenu, NULL,  0U, 0U},
};


/*---------------------------------------------------------------------------------------------------*/
/*
 * Обявдение объека-карусели экранов верхнего уровня
 */
xScreenSetObject xMainMenu =
{
  xScreensLev1,
  ( MENU_LEVEL1_COUNT - 1U ),
  0U,
  ( void* )&xInfoScreenCallBack,
};
/*---------------------------------------------------------------------------------------------------*/
xScreenSetObject xEngineMenu =
{
  xEngineScreens,
  ( ENGINE_MENU_COUNT - 1U ),
  0U,
  ( void* )&xInfoScreenCallBack,
};
/*---------------------------------------------------------------------------------------------------*/
xScreenSetObject xGeneratorMenu =
{
  xGeneratorScreens,
  ( GENERATOR_MENU_COUNT - 1U ),
  0U,
  ( void* )&xInfoScreenCallBack,
};
/*---------------------------------------------------------------------------------------------------*/
xScreenSetObject xNetMenu =
{
  xNetScreens,
  ( NET_MENU_COUNT - 1U ),
  0U,
  ( void* )&xInfoScreenCallBack,
};

xScreenSetObject xSettingsMenu =
{
  xSettingsScreens,
  ( SETTINGS_MENU_COUNT - 1U ),
  0U,
  ( void* )&xSettingsScreenKeyCallBack,
};

xScreenSetObject xEventMenu =
{
  xEventScreens,
  ( EVENT_MENU_COUNT  - 1U ),
  0U,
  ( void* )&EventScreenKeyCallBack,
};

