/*
 * menu_data.c
 *
 *  Created on: Mar 2, 2020
 *      Author: igor.dymov
 */
/*----------------------- Includes ------------------------------------------------------------------*/


#include "menu_data.h"
/*------------------------ Extern -------------------------------------------------------------------*/



/*----------------------- Defines -----------------------------------------------------------------*/
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
static uint8_t InputParam[]   = { 0U, 1U, LEFT_ALIGN, 0U };
static uint8_t InputParam3[]   = { 0U, 1U, LEFT_ALIGN, 0U };
static uint8_t InputParam4[]   = { 0U, 1U, LEFT_ALIGN, 0U };
static uint8_t InputParam5[]   = { 0U, 1U, LEFT_ALIGN, 0U };
static uint8_t InputParam6[]   = { 0U, 1U, LEFT_ALIGN, 0U };
static uint8_t InputParam7[]   = { 0U, 1U, LEFT_ALIGN, 0U };
static uint8_t InputParam8[]   = { 0U, 1U, LEFT_ALIGN, 0U };
static uint8_t InputParam9[]   = { 0U, 1U, LEFT_ALIGN, 0U };

static uint8_t InputParam10[]   = { 0U, 1U, LEFT_ALIGN, 0U };
static uint8_t InputParam11[]   = { 0U, 1U, LEFT_ALIGN, 0U };
static uint8_t InputParam12[]   = { 0U, 1U, LEFT_ALIGN, 0U };
static uint8_t InputParam13[]   = { 0U, 1U, LEFT_ALIGN, 0U };
static uint8_t InputParam14[]   = { 0U, 1U, LEFT_ALIGN, 0U };
static uint8_t InputParam15[]   = { 0U, 1U, LEFT_ALIGN, 0U };
static uint8_t InputParam16[]   = { 0U, 1U, LEFT_ALIGN, 0U };
static uint8_t InputParam17[]   = { 0U, 1U, LEFT_ALIGN, 0U };
static uint8_t InputParam18[]   = { 0U, 1U, LEFT_ALIGN, 0U };
static uint8_t InputParam19[]   = { 0U, 1U, LEFT_ALIGN, 1U };
static uint8_t InputParam1[]  =  { 0U, 1U, CENTER_ALIGN, 1U };
static uint8_t InputParam2[]  =  { 0U, 1U, CENTER_ALIGN, 0U };


static xScreenObjet const EngineMainScreen[]=
{
  { 0U, LEFT_OFFSET, LINE1, 0U, 0U,                          TEXT_STRING, LeftText, "ДВИГАТЕЛЬ - Сводные", NULL, 0U },
  { 0U, 0U, ( LINE4_HIGTH + 1U ), 128U, ( LINE4_HIGTH + 1U ),H_LINE,Header, NULL, NULL, 0U },
  { 0U, LEFT_OFFSET, LINE2, 0U, 0U, TEXT_STRING, LeftText, "Давление", NULL, 0U },
  { 0U, LEFT_OFFSET, LINE3, 0U, 0U, TEXT_STRING, LeftText, "Температура", NULL, 0U},
  { 0U, LEFT_OFFSET, LINE4, 0U, 0U, TEXT_STRING, LeftText, "Скорость", NULL, 0U },
  { 0U, FONT_SIZE*14U, ( LINE4_HIGTH + 2U ), 40U, LINE4_HIGTH, HW_DATA, RigthText, NULL, (void*)vGetDataForMenu,  OIL_PRESSURE },
  { 0U, FONT_SIZE*14U, ( LINE2 + 3U ),       40U, LINE4_HIGTH, HW_DATA, RigthText, NULL, (void*)&vGetDataForMenu, COOL_TEMP},
  { 0U, FONT_SIZE*14U, ( LINE3 + 3U ),       40U, LINE4_HIGTH, HW_DATA, RigthText, NULL, (void*)&vGetDataForMenu, ENGINE_SPEED },
  {1U,FONT_SIZE*20U,LINE1-12U,5U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,ALARM_STATUS},
};

static xScreenObjet const Engine1Screen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ДВИГАТЕЛЬ",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),H_LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"Напряжение АКБ",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"Напряж.зар.ген.",NULL,0U},
  {0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"Уровень топлива",NULL,0U},
  {0U,FONT_SIZE*16U,(LINE4_HIGTH+2U),30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,IN_VDD},
  {0U,FONT_SIZE*16U,LINE2+3U,30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,IN_CAC},
  {0U,FONT_SIZE*16U,LINE3+3U,30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,FUEL_LEVEL},
  {1U,FONT_SIZE*20U,LINE1-12U,5U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,ALARM_STATUS},
};

static xScreenObjet const Engine2Screen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ДВИГАТЕЛЬ",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),H_LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"Наработка",NULL,0U},
  {0U,FONT_SIZE*14U,(LINE4_HIGTH+2U),30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu, ENGINE_WTIME},
  {0U,FONT_SIZE*20U,LINE2,0U,0U,TEXT_STRING,LeftText,"Ч",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"Кол-во запусков",NULL,0U},
  {0U,FONT_SIZE*16U,LINE2+3U,30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,ENGINE_SCOUNT},
  {1U,FONT_SIZE*20U,LINE1-12U,5U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,ALARM_STATUS},
};

static xScreenObjet const GeneratorMainScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ГЕНЕРАТОР",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),H_LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"Напряжение",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"Ток",NULL,0U},
  {0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"Частота",NULL,0U},
  {0U,FONT_SIZE*11U,LINE1+6U,60U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_AVER_V},
  {0U,FONT_SIZE*11U,LINE2+3U,60U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_AVER_A},
  {0U,FONT_SIZE*11U,LINE3+3U,60U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_FREQ},
  {1U,FONT_SIZE*20U,LINE1-12U,5U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,ALARM_STATUS},
};


static xScreenObjet const Generator2Screen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ГЕНЕРАТОР - U лин.",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),H_LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"L1-L2 -",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"L2-L3 -",NULL,0U},
  {0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"L3-L1 -",NULL,0U},
  {0U,FONT_SIZE*7U,LINE1+6U,40U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_L1_LINE_V},
  {0U,FONT_SIZE*7U,LINE2+3U,40U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_L2_LINE_V},
  {0U,FONT_SIZE*7U,LINE3+3U,40U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_L3_LINE_V},
  {1U,FONT_SIZE*20U,LINE1-12U,5U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,ALARM_STATUS},
};

static xScreenObjet const Generator1Screen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ГЕНЕРАТОР - U фазн.",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),H_LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"L1-N -",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"L2-N -",NULL,0U},
  {0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"L3-N -",NULL,0U},
  {0U,FONT_SIZE*6U,LINE1+6U,60U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_L1_FASE_V},
  {0U,FONT_SIZE*6U,LINE2+3U,60U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_L2_FASE_V},
  {0U,FONT_SIZE*6U,LINE3+3U,60U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_L3_FASE_V},
  {1U,FONT_SIZE*20U,LINE1-12U,5U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,ALARM_STATUS},
};

static xScreenObjet const Generator3Screen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ГЕНЕРАТОР - Ток",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),H_LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"L1 -",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"L2 -",NULL,0U},
  {0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"L3 -",NULL,0U},
  {0U,FONT_SIZE*5U,LINE1+6U,60U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_L1_CUR},
  {0U,FONT_SIZE*5U,LINE2+3U,60U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_L2_CUR},
  {0U,FONT_SIZE*5U,LINE3+3U,60U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,GEN_L3_CUR},
  {1U,FONT_SIZE*20U,LINE1-12U,5U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,ALARM_STATUS},
};

static xScreenObjet const Generator4Screen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ГЕНЕРАТОР - Мощность",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),H_LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"L1 -",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"L2 -",NULL,0U},
  {0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"L3 -",NULL,0U},
  {0U,FONT_SIZE*5U,LINE1+6U,40U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu, GEN_L1_APER_POWER},
  {0U,FONT_SIZE*5U,LINE2+3U,40U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu, GEN_L2_APER_POWER},
  {0U,FONT_SIZE*5U,LINE3+3U,40U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu, GEN_L3_APER_POWER},

  {1U,FONT_SIZE*20U,LINE1-12U,5U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,ALARM_STATUS},
};


static xScreenObjet const Generator5Screen[]=
{
   {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ГЕНЕРАТОР - Сводные",NULL,0U},
   {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),H_LINE,Header,NULL,NULL,0U},
   {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"Мощность",NULL,0U},
   {0U,FONT_SIZE*11U,LINE1+6U,60U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu, GEN_REACTIVE_POWER},
   {0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"cos f",NULL,0U},
   {0U,FONT_SIZE*11U,LINE3+3U,30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,COS_FI},
   {1U,FONT_SIZE*20U,LINE1-12U,5U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,ALARM_STATUS},
};


static xScreenObjet const Generator6Screen[]=
{
   {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ГЕНЕРАТОР",NULL,0U},
   {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),H_LINE,Header,NULL,NULL,0U},
   {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"Череование фаз:",NULL,0U},
   {0U,FONT_SIZE*11U,LINE3+3U,30U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu, GEN_ROTATION},
   {1U,FONT_SIZE*20U,LINE1-12U,5U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,ALARM_STATUS},
};


static xScreenObjet const NetMainScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"СЕТЬ - U лин.",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),H_LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"L1-N - ",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"L2-N - ",NULL,0U},
  {0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"L3-N - ",NULL,0U},
  {0U,FONT_SIZE*7U,LINE1+6U,40U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,NET_L1_LINE_V},
  {0U,FONT_SIZE*7U,LINE2+3U,40U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,NET_L2_LINE_V},
  {0U,FONT_SIZE*7U,LINE3+3U,40U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,NET_L3_LINE_V},
  {1U,FONT_SIZE*20U,LINE1-12U,5U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,ALARM_STATUS},
};

static xScreenObjet const Net1Screen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"СЕТЬ - U фазн.",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),H_LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"L1-L2 - ",NULL,0U},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"L2-L3 - ",NULL,0U},
  {0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"L3-L1 - ",NULL,0U},
  {0U,FONT_SIZE*8U,LINE1+6U,40U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,NET_L1_FASE_V},
  {0U,FONT_SIZE*8U,LINE2+3U,40U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,NET_L2_FASE_V},
  {0U,FONT_SIZE*8U,LINE3+3U,40U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,NET_L3_FASE_V},
  {1U,FONT_SIZE*20U,LINE1-12U,5U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,ALARM_STATUS},
};

static xScreenObjet const Net2Screen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"СЕТЬ - Частота.",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),H_LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"Частота - ",NULL,0U},
  {0U,FONT_SIZE*10U,LINE1+6U,60U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,NET_FREQ},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"Чередование фаз:",NULL,0U},
  {0U,LEFT_OFFSET,LINE3+3U,50U,LINE4_HIGTH,HW_DATA,LeftText,NULL,(void*)vGetDataForMenu,NET_ROTATION},
  {1U,FONT_SIZE*20U,LINE1-12U,5U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,ALARM_STATUS},
};


static xScreenObjet const AlarmMainScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ОШИБОК :",NULL,0U},
  {0U,FONT_SIZE*17U,LINE1-12U,15U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,ALARM_COUNT},
  {0U,LEFT_OFFSET,LINE1+6U,120U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,CURRENT_ALARM_TIME},
  {0U,LEFT_OFFSET,LINE2+3U,120U,LINE4_HIGTH,HW_DATA,LeftText,NULL,(void*)vGetAlarmForMenu,CURRENT_ALARM_T},
  {0U,LEFT_OFFSET,LINE3+3U,120U,LINE4_HIGTH,HW_DATA,LeftText,NULL,(void*)vGetAlarmForMenu,CURRENT_ALARM_ACTION},
  {1U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),H_LINE,Header,NULL,NULL,0U},

};


static xScreenObjet const EventMainScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ЖУРНАЛ:",NULL,0U},
  {0U,FONT_SIZE*17U,LINE1-12U,15U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,EVENT_COUNT},
  {0U,LEFT_OFFSET,LINE1+6U,120U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,CURRENT_EVENT_TIME},
  {0U,LEFT_OFFSET,LINE2+3U,120U,LINE4_HIGTH,HW_DATA,LeftText,NULL,(void*)vGetAlarmForMenu,CURRENT_EVENT_T},
  {0U,LEFT_OFFSET,LINE3+3U,120U,LINE4_HIGTH,HW_DATA,LeftText,NULL,(void*)vGetAlarmForMenu,CURRENT_EVENT_ACTION},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),H_LINE,Header,NULL,NULL,0U},
  {1U,FONT_SIZE*20U,LINE1-12U,5U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,ALARM_STATUS},
};




static xScreenObjet const InfoMainScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,128U,TEXT_STRING,Header,"О ПРИБОРЕ",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),H_LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE2,0U,0U,TEXT_STRING,LeftText,"Версия ПО:",NULL,0U},
  {0U,LEFT_OFFSET,LINE1+6U,110U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,SW_VER},
  {0U,LEFT_OFFSET,LINE3,0U,0U,TEXT_STRING,LeftText,"Версия прибора:",NULL,0U},
  {0U,LEFT_OFFSET,LINE2+3U,110U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,HW_VER},
  {0U,LEFT_OFFSET,LINE4,0U,0U,TEXT_STRING,LeftText,"IP",NULL,0U},
  {0U,FONT_SIZE*8U,LINE3+3U,20U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)&vGetDataForMenu,IP_ADRESS},
  {LO,FONT_SIZE*20U,LINE1-12U,5U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,ALARM_STATUS},
};


static xScreenObjet const SerialNumberScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,Header,"Cерийный номер",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),H_LINE,Header,NULL,NULL,0U},
  {0U,LEFT_OFFSET,LINE1+6U,110U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,SERIAL_L},
  {0U,LEFT_OFFSET,LINE2+3U,110U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetDataForMenu,SERIAL_H},
  {LO,FONT_SIZE*20U,LINE1-12U,5U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,ALARM_STATUS},
};

static xScreenObjet const LinkMainScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"Входы/выходы",NULL,0U},
  {0U,0U,(LINE4_HIGTH+1U),128U,(LINE4_HIGTH+1U),H_LINE,Header,NULL,NULL,0U},
  {1U,FONT_SIZE*20U,LINE1-12U,5U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,ALARM_STATUS},
};

static xScreenObjet const StatusMainScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"СТАТУС",NULL,0U},
  {0U, 0U, ( LINE4_HIGTH + 1U ), 128U, ( LINE4_HIGTH + 1U ), H_LINE, Header, NULL, NULL, 0U },
  {1U,FONT_SIZE*20U,LINE1-12U,5U,LINE4_HIGTH,HW_DATA,RigthText,NULL,(void*)vGetAlarmForMenu,ALARM_STATUS},
};


static xScreenObjet  const SettingsScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"НАСТРОЙКА :",NULL,0U},
  {0U, 0U, ( LINE4_HIGTH + 1U ), 128U, ( LINE4_HIGTH + 1U ), H_LINE, Header, NULL, NULL, 0U },
  {3U,FONT_SIZE*1U,  LINE2+3U,10U, LINE4_HIGTH, INPUT_HW_DATA, InputParam3,NULL,(void*)&vGetSettingsBitData, 16U},
  {3U,FONT_SIZE*2U,  LINE2+3U,10U, LINE4_HIGTH, INPUT_HW_DATA, InputParam4,NULL,(void*)&vGetSettingsBitData, 15U},
  {3U,FONT_SIZE*3U,  LINE2+3U,10U, LINE4_HIGTH, INPUT_HW_DATA, InputParam5,NULL,(void*)&vGetSettingsBitData, 14U},
  {3U,FONT_SIZE*4U,  LINE2+3U,10U, LINE4_HIGTH, INPUT_HW_DATA, InputParam6,NULL,(void*)&vGetSettingsBitData, 13U},
  {3U,FONT_SIZE*6U,  LINE2+3U,10U, LINE4_HIGTH, INPUT_HW_DATA, InputParam7,NULL,(void*)&vGetSettingsBitData, 12U},
  {3U,FONT_SIZE*7U,  LINE2+3U,10U, LINE4_HIGTH, INPUT_HW_DATA, InputParam8,NULL,(void*)&vGetSettingsBitData, 11U},
  {3U,FONT_SIZE*8U,  LINE2+3U,10U, LINE4_HIGTH, INPUT_HW_DATA, InputParam9,NULL,(void*)&vGetSettingsBitData, 10U},
  {3U,FONT_SIZE*9U,  LINE2+3U,10U, LINE4_HIGTH, INPUT_HW_DATA, InputParam10,NULL,(void*)&vGetSettingsBitData, 9U},
  {3U,FONT_SIZE*11U, LINE2+3U,10U, LINE4_HIGTH, INPUT_HW_DATA, InputParam11,NULL,(void*)&vGetSettingsBitData, 8U},
  {3U,FONT_SIZE*12U, LINE2+3U,10U, LINE4_HIGTH, INPUT_HW_DATA, InputParam12,NULL,(void*)&vGetSettingsBitData, 7U},
  {3U,FONT_SIZE*13U, LINE2+3U,10U, LINE4_HIGTH, INPUT_HW_DATA, InputParam13,NULL,(void*)&vGetSettingsBitData, 6U},
  {3U,FONT_SIZE*14U, LINE2+3U,10U, LINE4_HIGTH, INPUT_HW_DATA, InputParam14,NULL,(void*)&vGetSettingsBitData, 5U},
  {3U,FONT_SIZE*16U, LINE2+3U,10U, LINE4_HIGTH, INPUT_HW_DATA, InputParam15,NULL,(void*)&vGetSettingsBitData, 4U},
  {3U,FONT_SIZE*17U, LINE2+3U,10U, LINE4_HIGTH, INPUT_HW_DATA, InputParam16,NULL,(void*)&vGetSettingsBitData, 3U},
  {3U,FONT_SIZE*18U, LINE2+3U,10U, LINE4_HIGTH, INPUT_HW_DATA, InputParam17,NULL,(void*)&vGetSettingsBitData, 2U},
  {3U,FONT_SIZE*19U, LINE2+3U,10U, LINE4_HIGTH, INPUT_HW_DATA, InputParam18,NULL,(void*)&vGetSettingsBitData, 1U},
  {2U,FONT_SIZE*16U, LINE1+6U,40U, LINE4_HIGTH, HW_DATA,       InputParam,NULL,(void*)&vGetSettingsUnit,      0U},
  {2U,FONT_SIZE*5U, LINE1+6U,30U, LINE4_HIGTH,  INPUT_HW_DATA, InputParam,NULL,(void*)&vGetSettingsData,      0U},
  {LO,FONT_SIZE*13U, LINE1-11U,30U, LINE4_HIGTH, HW_DATA,       InputParam,NULL,(void*)&vGetSettingsNumber,    0U},

};

static xScreenObjet const xYesNoScreen[]=
{
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"ПРИМЕНИТЬ ИЗМЕНЕНИЯ",NULL,0U},
  {0U, 0U, ( LINE4_HIGTH + 1U ), 128U, ( LINE4_HIGTH + 1U ), H_LINE, Header, NULL, NULL, 0U },
  {0U,FONT_SIZE*3U,LINE2,30U,LINE4_HIGTH,DATA_STRING,InputParam1,"  ДА  ",NULL,0U},
  {1U,FONT_SIZE*13U,LINE2,30U,LINE4_HIGTH,DATA_STRING,InputParam2,"  НЕТ  ",NULL,0U},
};

static xScreenObjet const xPasswordScreen[]=
{
  {0U,FONT_SIZE*3U ,LINE2,10U,LINE4_HIGTH,INPUT_HW_DATA,InputParam19,NULL,(void*)&vGetPasswordData,1U},
  {0U,FONT_SIZE*7U ,LINE2,10U,LINE4_HIGTH,INPUT_HW_DATA,InputParam2,NULL,(void*)&vGetPasswordData,2U},
  {0U,FONT_SIZE*10U,LINE2,10U,LINE4_HIGTH,INPUT_HW_DATA,InputParam3,NULL,(void*)&vGetPasswordData,3U},
  {0U,FONT_SIZE*13U,LINE2,10U,LINE4_HIGTH,INPUT_HW_DATA,InputParam4,NULL,(void*)&vGetPasswordData,4U},
  {0U,LEFT_OFFSET,LINE1,0U,0U,TEXT_STRING,LeftText,"Пароль",NULL,0U},
  {1U, 0U, ( LINE4_HIGTH + 1U ), 128U, ( LINE4_HIGTH + 1U ), H_LINE, Header, NULL, NULL, 0U },

};

static xScreenObjet const xMessageScreen[]=
{
  {1U,FONT_SIZE*1U ,LINE2,100U,LINE4_HIGTH,HW_DATA,Header,NULL,(void*)&vGetMessageData,1U},
};

//Описание экранов главонго меню
xScreenType  xScreensLev1[MENU_LEVEL1_COUNT]=
{
  {StatusMainScreen,   &xMainMenu, NULL           ,0U,0U},
  {AlarmMainScreen,    &xMainMenu, &xAlarmMenu    ,0U,0U},
  {EngineMainScreen,   &xMainMenu, &xEngineMenu   ,0U,0U},
  {GeneratorMainScreen,&xMainMenu, &xGeneratorMenu,0U,0U},
  {NetMainScreen,      &xMainMenu, &xNetMenu      ,0U,0U},
  {EventMainScreen,    &xMainMenu, &xEventMenu    ,0U,0U},
  {InfoMainScreen,     &xMainMenu, &xAboutMenu    ,0U,0U},
};
//Описание вложеных экранов двигателя
xScreenType  xEngineScreens[ENGINE_MENU_COUNT]=
{
  {Engine1Screen,     &xMainMenu, NULL, 0U, 0U},
  {Engine2Screen,     &xMainMenu, NULL, 0U, 0U},
  {EngineMainScreen,  &xMainMenu, NULL, 0U, 0U},
};

xScreenType xGeneratorScreens[GENERATOR_MENU_COUNT]=
{
  {Generator1Screen,    &xMainMenu, NULL, 0U, 0U},
  {Generator2Screen,    &xMainMenu, NULL, 0U, 0U},
  {Generator3Screen,    &xMainMenu, NULL, 0U, 0U},
  {Generator4Screen,    &xMainMenu, NULL, 0U, 0U},
  {Generator5Screen,    &xMainMenu, NULL, 0U, 0U},
  {Generator6Screen,    &xMainMenu, NULL, 0U, 0U},
  {GeneratorMainScreen, &xMainMenu, NULL, 0U, 0U},
};

xScreenType xNetScreens[NET_MENU_COUNT]=
{
  { Net1Screen,       &xMainMenu, NULL,  0U, 0U},
  { Net2Screen,       &xMainMenu, NULL,  0U, 0U},
  { NetMainScreen,    &xMainMenu, NULL,  0U, 0U},
};


xScreenType  xAlarmScreens[ALARM_MENU_COUNT]=
{
  {AlarmMainScreen,   &xMainMenu, NULL, 0U, 0U},
};

xScreenType  xSettingsScreens[SETTINGS_MENU_COUNT]=
{
  { SettingsScreen, &xMainMenu, NULL,  0U, 0U},
};


xScreenType  xAboutScreens[ABOUT_MENU_COUNT]=
{
  {LinkMainScreen,      &xMainMenu, NULL, 0U, 0U},
  {SerialNumberScreen,  &xMainMenu, NULL, 0U, 0U},
  {InfoMainScreen,      &xMainMenu, NULL, 0U, 0U},
};


xScreenType   xEventScreens[EVENT_MENU_COUNT]=
{
  {EventMainScreen, &xMainMenu, NULL, 0U, 0U},
};


xScreenType   xYesNoScreens[SETTINGS_MENU_COUNT]=
{
  { xYesNoScreen, &xMainMenu, NULL,  0U, 0U},
};

xScreenType   xPasswordScreens[PASSWORD_MENU_COUNT]=
{
  { xPasswordScreen, &xMainMenu, NULL,  0U, 0U},
};

xScreenType   xMessageScreens[MESSAGE_MENU_COUNT]=
{
  { xMessageScreen, &xMainMenu, NULL,  0U, 0U},
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
  ( void* )&xInfoScreenCallBack,
};

xScreenSetObject xAboutMenu =
{
  xAboutScreens,
  ( ABOUT_MENU_COUNT - 1U ),
  0U,
  ( void* )&xInfoScreenCallBack,
};


xScreenSetObject xAlarmMenu =
{
  xAlarmScreens,
  ( ALARM_MENU_COUNT - 1U ),
  0U,
  ( void* )&xInfoScreenCallBack,
};

xScreenSetObject xPasswordMenu =
{
  xPasswordScreens,
  ( PASSWORD_MENU_COUNT - 1U ),
  0U,
  ( void* )&xPasswordScreenCallBack,
};


xScreenSetObject xMessageMenu =
{
  xMessageScreens,
  ( MESSAGE_MENU_COUNT - 1U ),
  0U,
  ( void* )&xMessageScreenCallBack,
};


xScreenSetObject xYesNoMenu =
{
  xYesNoScreens,
  ( YESNO_MENU_COUNT - 1U ),
  0U,
  ( void* )&xYesNoScreenKeyCallBack,
};

