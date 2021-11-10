#/*
 * menu.h
 *
 *  Created on: 21 февр. 2020 г.
 *      Author: igor.dymov
 */
/*----------------------------------------------------------------------*/
#ifndef INC_MENU_H_
#define INC_MENU_H_
/*----------------------- Includes -------------------------------------*/
#include "u8g2.h"
#include "config.h"
#include "../../hmi/Inc/data_type.h"
#include "dataAPI.h"
#include "adc.h"
#include "RTC.H"
#include "journal.h"
#include "fpi.h"
#include "fpo.h"
#include "alarm.h"
#include "controllerTypes.h"
#include "string.h"
#include "../../hmi/Inc/utils.h"

/*------------------------ Define --------------------------------------*/
#define   SET_PARAMETR_SCREEN 0U
#define   CENTER_ALIGN        1U
#define   RIGTH_ALIGN         2U
#define   LEFT_ALIGN          3U
#define   NO_ALIGN            0U

#define   BLINK_TIME          2U
#define   BITMAP              3U
#define   NUMBER              2U
#define   DATE_TYPE           4U
#define   HOME_MENU           0U
#define   ALARM_MENU          1U

#define   FIRST_VALID_SETTING 2U
#define   FIRST_SETTING       3U

#define LAST_OBJECT  1U
/* Определение виртуальных клавиш, которые могу как повторять клавиши клавиатуры, так и быть их комбинацие */
#define KEY_UP_BREAK        (up_key | BRAKECODE) //1U
#define KEY_UP              (up_key | MAKECODE) //2U
#define KEY_DOWN_BREAK      (down_key | BRAKECODE) //3U
#define KEY_DOWN            (down_key | MAKECODE)  //4U
#define KEY_STOP            (stop_key | MAKECODE)  //5U
#define KEY_STOP_BREAK      (stop_key | BRAKECODE) // 6U
#define KEY_AUTO            (auto_key | MAKECODE)  //9U
#define KEY_AUTO_BREAK      (auto_key | BRAKECODE)//7U
#define KEY_START           (start_key | MAKECODE) //10U
#define KEY_START_BREAK     (start_key | BRAKECODE)// 8U
#define KEY_EXIT            (time_out | MAKECODE)  //11U

/**************Типы данны****************************************************************************************/
#define BITMAP_TYPE         3U

#define MAX_SCREEN_COUNT    3U
#define MAX_SCREEN_OBJECT   25U
#define XRESULURION         128U
#define YRESOLUTION         64U
#define FONT_TYPE           ( u8g2_font_6x13_t_cyrillic )
#define FONT_X_SIZE         6U
#define FONT_TYPE_NUMBER    ( u8g2_font_5x8_mf )
#define FONT_TYPE4          ( u8g2_font_6x13_t_cyrillic )
#define FONT_X_SIZE4        6U
#define FONT_TYPE_NUMBER4   ( u8g2_font_5x8_mf )
#define MAX_KEY_PRESS

#define ALARM_COUNT           1U
#define EVENT_COUNT           2U
#define CURRENT_ALARM_TIME    3U
#define CURRENT_EVENT_TIME    4U
#define CURRENT_ALARM_T       5U
#define CURRENT_EVENT_T       6U
#define CURRENT_ALARM_ACTION  7U
#define CURRENT_EVENT_ACTION  8U
#define ALARM_STATUS          9U

#define AUTO_KEY_READY   0x01U
#define START_KEY_READY  0x02U
#define STOP_KEY_READY   0x04U
#define SET_MENU_READY   0x08U


#define SW_VER         54U
#define HW_VER         55U
#define SERIAL_L       56U
#define SERIAL_H       57U
#define ADC_FREQ       62U
#define ADC_TEMP       63U
#define NET_ROTATION   68U
#define GEN_ROTATION   69U
#define FUEL_LEVEL     79U
#define OIL_PRESSURE   80U
#define COOL_TEMP      81U
#define ENGINE_SPEED   82U
#define ENGINE_SCOUNT  83U
#define ENGINE_WTIME   84U
#define COS_FI         85U
#define IN_CAC         86U
#define IP_ADRESS      87U
#define TIME_DATE      88U
#define FPO_S            89U
#define FPI_S            90U
#define CONTROLER_STATUS 91U
#define STATUS_TIME      92U
#define HOUR            93U
#define MINUTE          94U
#define DAY             95U
#define MOUNTH           96U
#define YEAR            97U


/*------------------------------ Enum ----------------------------------------*/



typedef enum
{
  ICON,
  TEXT_STRING,
  STRING,
  H_LINE,
  NEGATIVE_STRING,
  DATA_STRING,
  INPUT_DATA_STRING,
  HW_DATA,
  INPUT_HW_DATA,
} OBJECT_TYPE;

typedef enum
{
  FLAG_RESET,
  FLAG_SET,
} FLAG;
/*---------------------------- Structures --------------------------------------*/
typedef struct __packed
{
  uint8_t     last;
  uint8_t     x;
  uint8_t     y;
  uint8_t     Width;
  uint8_t     Height;
  OBJECT_TYPE xType;
  uint8_t*    ObjectParamert;
  char*       pStringParametr;
  void        ( *GetDtaFunction )();
  uint16_t    DataID;
} xScreenObjet;

typedef struct __packed
{
  const xScreenObjet*  pScreenCurObjets;
   void*         pUpScreenSet;
   void*         pDownScreenSet;
  uint8_t       pCurrIndex;
  uint8_t       pMaxIndex;
 // void          ( *pFunc )( void*, char );
} xScreenType;

typedef struct __packed
{
  xScreenType*  pHomeMenu;               /* Указатель на массив экранов верхнего уровня. */
  uint8_t       pMaxIndex;	             /* Индекс домашнего экрана в маасиве экранов верхнего уровня */
  uint8_t       pCurrIndex;	             /* Индексе текущего экрана */
  void          (* pFunc)(void *,char);  /* Функция обработки нажатий */
} xScreenSetObject;
/*----------------------------- Functions ------------------------------------*/

void vMenuInit( u8g2_t* temp );
void vMenuTask( void );
void vDrawObject( xScreenObjet* pScreenObjects )  __attribute__((optimize("-O3")));
void vGetSettingsData( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );
void vGetSettingsUnit( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );
void vGetSettingsNumber( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );
void vGetSettingsBitData( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );
void vGetPasswordData( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );
void vMenuMessageInit( osThreadId_t xmainprocess );
void vGetMessageData( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );
void vGetDataForMenu( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );
void vGetControllerStatus( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );
void vGetAlarmForMenu( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );
void vGetFPOForMenu( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );
void vGetFPIForMenu( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );
void vGetTIMEForMenu( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID );
void vExitCurObject ( void );


//Обработчики нажатий клавиш
void xYesNoScreenKeyCallBack    ( xScreenSetObject * menu, char key );
void xSettingsScreenKeyCallBack ( xScreenSetObject * menu, char key );
void xPasswordScreenCallBack    ( xScreenSetObject * menu, char key );
void EventScreenKeyCallBack     ( xScreenSetObject * menu, char key );
void xInfoScreenCallBack        ( xScreenSetObject * menu, char key );
void xInputScreenKeyCallBack    ( xScreenSetObject * menu, char key );
void xMessageScreenCallBack     ( xScreenSetObject * menu, char key );


void vMenuMessageShow(char * mes);
void vMenuMessageHide(void);
/*----------------------------------------------------------------------------*/
#endif /* INC_MENU_H_ */
