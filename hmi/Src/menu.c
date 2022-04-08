/*
 * menu.c
 *
 *  Created on: 21 февр. 2020 г.
 *      Author: igor.dymov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "../../hmi/Inc/menu.h"

#include "controllerTypes.h"
//#include "stdio.h"
#include "server.h"
#include "keyboard.h"
#include "lcd.h"
#include "menu_data.h"
#include "engine.h"

/*------------------------ Define -------------------------------------------------------------------*/
#define NO_SELECT_D   0U
#define SELECT_D      1U
#define CHANGE_D      2U
#define VIEW_DELAY    10U

/*----------------------- Variables -----------------------------------------------------------------*/
static u8g2_t*           u8g2             = NULL;
static osThreadId_t      xProccesToNotify = NULL;
static KeyEvent          TempEvent        = { 0U };
static KeyEvent          BufferEvent      = { 0U };
static uint8_t           temp_counter     = 0U;
static xScreenSetObject* pCurrMenu        = NULL;
static xScreenObjet*     pCurObject       = NULL;
static QueueHandle_t     pKeyboard        = NULL;
static uint8_t           key              = 0U;
static xScreenObjet*     pCurDrawScreen   = NULL;
static uint8_t           Blink            = 0U;
static uint16_t          uiSetting        = FIRST_SETTING ;
static uint8_t           ucActiveObject   = NO_SELECT_D;
static uint8_t           EXIT_KEY_F       = 0U;
static char              TempArray[70];
static uint8_t           uDataType        = 0U;
static uint16_t          uCurrentAlarm    = 0U;
static uint8_t           password[]       = { 0U, 0U, 0U ,0U };
static FLAG              fAlarmFlag       = FLAG_RESET;
static FLAG              fDownScreen      = FLAG_RESET;
static uint8_t           uCurrentObject   = 0U;
static FLAG              fPassowordCorrect= FLAG_RESET;
static FLAG              fTimeEdit        = FLAG_RESET;
static RTC_TIME          buftime;

/*----------------------- Functions -----------------------------------------------------------------*/

void vMenuGotoAlarmScreen( void);

/*----------------------- Structures ----------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------*/
/*
 * Функции обработки клавишей меню да-нет?
 */
#define     YES_MASK   0x01
#define     NO_MASK    0x02

void vYesNoMenu(xScreenSetObject* menu,uint8_t mask)
{
  menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[2U].ObjectParamert[3U] = mask & 0x01;
  menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[3U].ObjectParamert[3U] = (mask >> 1) & 0x01;
  return;
}

void xYesNoScreenKeyCallBack ( xScreenSetObject* menu, char key )
{
    switch ( key )
    {
      case KEY_STOP:  //Если клавиша стоп, то подсвечиваем объект "ДА"
        vYesNoMenu(menu,YES_MASK);
        break;
      case KEY_START://Если клавиша старт, то подсвечиваем объект "НЕТ"
        vYesNoMenu(menu,NO_MASK);
      break;
      case KEY_AUTO:
        //Если каливаша AUTO то проверяем объеты меню, если выбран ДА.
        if ( menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[2U].ObjectParamert[3U] == 1U )
        {
          if (uDataType ==  DATE_TYPE)
          {
            eRTCsetTime (&buftime);
            fTimeEdit = FLAG_RESET;
          }
          else
          {
            eDATAAPIconfigValue(DATA_API_CMD_SAVE,uiSetting,NULL);
          }
        }
        else
        {
          if (uiSetting >= FIRST_VALID_SETTING)
          {
            eDATAAPIconfigValue(DATA_API_CMD_LOAD,uiSetting,NULL);
          }
        }
        vYesNoMenu(menu,YES_MASK); //Подсвечиваем объект "ДА"
        pCurrMenu = xYesNoMenu.pHomeMenu[0U].pUpScreenSet;
        break;
      case KEY_EXIT:  // Если прилетела команда выход, что также может быть и таймаутом,
                      // выходим каскадом в предидущие меню
        vYesNoMenu(menu,YES_MASK);
        pCurrMenu = xYesNoMenu.pHomeMenu[0U].pUpScreenSet;
        pCurrMenu->pFunc( pCurrMenu, KEY_EXIT );
        break;
      default:
        break;
    }
  return;
}
/***********************************************************************   Функции экранов редактирования уставок*********************************************************************/

#define        HOUR_OBJECT      1U
#define        MINUTE_OBJECT    2U
#define        DAY_OBJECT       3U
#define        MONTH_OBJECT     4U
#define        YEAR_OBJECT      5U

/*
 * Сервисная функция вывода в строку заданного бита уставки типа bitmap
 */
void vGetSettingsBitData( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  eConfigAttributes xAtrib    = { 0U };
  uint16_t          buff      =  0U;
  if ( cmd == mREAD)
  {
      eDATAAPIconfigAtrib( DATA_API_CMD_READ, uiSetting, &xAtrib );
      if ( xAtrib.bitMapSize != 0U )
      {
        eDATAAPIconfigValue( DATA_API_CMD_READ, uiSetting, &buff );
        Data[0]   = ( ( buff >> ( ID-1 ) ) & 0x01 ) + '0';
        Data[1]   = 0;
        uDataType = BITMAP_TYPE;
      }
      else
      {
        Data[0]=0;
      }
  }
  return;
}

/*
 *  Сервисная функция вывода данных численного формата
 */

void vGetSettingsData ( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  eConfigAttributes xAtrib                  = { 0U };
  uint16_t          buff                    = 0U;
  int8_t            scale                   = 0U;
  uint16_t          units[MAX_UNITS_LENGTH] = { 0U };
  char              cbuf[17]                = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
  Data[0U] = 0U;
  if ( cmd == mREAD )
  {
      eDATAAPIconfigAtrib( DATA_API_CMD_READ, uiSetting, &xAtrib );
      if ( xAtrib.bitMapSize == 0U )
      {
         if ( xAtrib.len == 1U )
         {
             if (ID!=10)
             {
                 switch ( xAtrib.type )
                 {
                     case 'U':
                         eDATAAPIconfig( DATA_API_CMD_READ, uiSetting, &buff, &scale, units );
                         vUToStr ( Data, buff, scale );
                         uDataType = 2;
                         break;
                     case 'S':

                       break;
                     default:
                       break;
                 }
             }
        }
        else
        {
          if ( (ID==10) && ( xAtrib.type == 'C') )
          {
              eDATAAPIconfigValue( DATA_API_CMD_READ, uiSetting,(uint16_t *) &cbuf );
              vStrCopy(Data,cbuf);
          }
        }
      }
  }
  return;
}


/*
 * Сервисная функция вывода в номера редактируемой настройки
 */
void vGetSettingsNumber( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  if (cmd == mREAD)
  {
    vUToStr(Data, uiSetting,0);
  }
  return;
}


/*
 * Сервисная функция вывода в строку размерности насртокйки
 */

void vGetSettingsUnit ( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  eConfigAttributes xAtrib                  = { 0U };
  uint8_t           k                       = 0U;
  uint16_t          buff                    = 0U;
  int8_t            scale                   = 0U;
  uint16_t          units[MAX_UNITS_LENGTH] = { 0U };
  if ( cmd == mREAD)
  {
      eDATAAPIconfigAtrib( DATA_API_CMD_READ, uiSetting, &xAtrib );
      if (( xAtrib.bitMapSize == 0U ) && ( xAtrib.len == 1U ) )
      {
          eDATAAPIconfig( DATA_API_CMD_READ, uiSetting, &buff, &scale, units );
          for (uint8_t i=0; i< MAX_UNITS_LENGTH; i++ )
          {
            if ( ( units[i] >> 8U) != 0U )
            {
              Data[k++] = units[i] >> 8U;
            }
            Data[k++] = units[i] & 0x00FFU;
          }
          Data[k] = 0U;
      }
      else
      {
        vStrCopy(Data,"    ");
      }
  }
  return;
}


/*
 * Сервисная функция для корретироаки времени в меню на строек. Проверяет корректность измененных данных
 */
void vMenuTimeChange(int8_t offset)
{
  ucActiveObject = CHANGE_D;
  fTimeEdit      = FLAG_SET;
  switch (uCurrentObject)
  {
        case HOUR_OBJECT:
           if ( ( buftime.hour + offset ) <= RTC_HOUR_MAX )
           {
              buftime.hour = buftime.hour + offset;
           }
           break;
        case MINUTE_OBJECT:
           if ( ( buftime.min + offset ) <= RTC_MIN_MAX )
           {
              buftime.min = buftime.min + offset;
           }
           break;
        case DAY_OBJECT:
           if ( ( buftime.day + offset <= RTC_DAY_MAX ) && ( buftime.day + offset >= RTC_DAY_MIN ) )
           {
              buftime.day = buftime.day + offset;
           }
           break;
        case MONTH_OBJECT:
           if ( ( buftime.month + offset <= RTC_MONTH_MAX ) && ( buftime.month + offset >= RTC_MONTH_MIN ) )
           {
              buftime.month = buftime.month + offset;
           }
           break;
        case YEAR_OBJECT:
           if ( ( buftime.year + offset ) <= RTC_YEAR_MAX )
           {
              buftime.year = buftime.year + offset;
           }
           break;
         default:
           break;
   }
}

uint8_t vSelectLastObject( xScreenSetObject* menu, uint8_t i)
{
  if (menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[i].last == LO)
  {
      return ( 1U );
  }
  if (menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[i].last == uDataType )
  {
      menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].ObjectParamert[3U] = 0U;
      uCurrentObject = i;
      menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].ObjectParamert[3U] = 1U;
  }
  return ( 0U );
}

uint8_t vSelectNewObject( xScreenSetObject* menu, uint8_t i)
{
  if (menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[i].last == LO)
  {
    return ( 1U );
  }
  if (menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[i].last == uDataType )
  {
    menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].ObjectParamert[3U] = 0U;
    uCurrentObject = i;
    menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].ObjectParamert[3U] = 1U;
    return ( 1U );
  }

return ( 0U );
}




void xSettingsScreenKeyCallBack( xScreenSetObject* menu, char key )
{

    uint16_t data =0;
    if (key == (KEY_EXIT))
    {
      fPassowordCorrect = FLAG_RESET;
      menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].ObjectParamert[3U] = 0U;

      pCurrMenu = menu->pHomeMenu[menu->pCurrIndex].pUpScreenSet;

      fDownScreen = FLAG_RESET;
      ucActiveObject = NO_SELECT_D;
      fTimeEdit   = FLAG_RESET;
      if (uiSetting >= FIRST_VALID_SETTING)
      {
        eDATAAPIconfigValue(DATA_API_CMD_LOAD,uiSetting,NULL);
      }
      uiSetting  = FIRST_SETTING ;
      return;
    }
    if ( ( ucActiveObject !=NO_SELECT_D) && (uDataType == BITMAP))
    {
        switch (key)
        {
          case KEY_DOWN:
          case KEY_UP:
                ucActiveObject = CHANGE_D;
                eDATAAPIconfigValue( DATA_API_CMD_READ, uiSetting, &data );
                if (data & (0x01<<(menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].DataID-1)))
                    data &= ~(0x01<<(menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].DataID-1));
                else
                    data |= (0x01<<(menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].DataID-1));
                eDATAAPIconfigValue( DATA_API_CMD_WRITE, uiSetting, &data );
                break;
          case KEY_STOP:
                 for (uint8_t i=uCurrentObject-1; ((i>0) && ( vSelectNewObject(menu,i) != 1) );i--);
                 break;
          case KEY_START:
                  for (uint8_t i=(uCurrentObject+1); ( ( i < MAX_SCREEN_OBJECT ) && ( vSelectNewObject(menu,i) != 1) );i++);
                  break;
          case KEY_AUTO:
                 pCurObject =  (xScreenObjet *)&menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject];
                 vExitCurObject();
                 return;
          default:
              break;
        }
      }
  if ((uDataType ==  NUMBER) && ( ucActiveObject !=NO_SELECT_D))
  {
     switch (key)
    {
         case KEY_STOP:
         case KEY_START:
              ucActiveObject = CHANGE_D;
              eDATAAPIconfigValue(key == KEY_STOP ? DATA_API_CMD_DEC :DATA_API_CMD_INC , uiSetting, NULL );
              break;
         case KEY_DOWN:
         case KEY_UP:
              ucActiveObject = CHANGE_D;
              for ( uint8_t i=0U; i<10U; i++ )
              {
                 eDATAAPIconfigValue( key == KEY_STOP ? DATA_API_CMD_DEC :DATA_API_CMD_INC , uiSetting, NULL );
              }
              break;
         case KEY_AUTO:
              pCurObject =  (xScreenObjet *)&menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject];
              vExitCurObject();
              return;
         default:
           break;
       }
    }
     //Блок редактирования данныт типа даты и времекни
     if ( ( uDataType ==  DATE_TYPE ) && ( ucActiveObject != NO_SELECT_D ) )
     {
      switch (key)
      {
           case KEY_DOWN:
           case KEY_UP:
                vMenuTimeChange ( ( key ==  KEY_DOWN) ? -1 : 1 );
                break;
           case KEY_STOP:
                for (uint8_t i = uCurrentObject-1; ( ( i>0 ) && ( vSelectNewObject(menu,i) != 1)) ;i--);
                break;
           case KEY_START:
                for (uint8_t i=(uCurrentObject+1); ( ( i<MAX_SCREEN_OBJECT ) &&  ( vSelectNewObject(menu,i) != 1) ) ; i++ );
                break;
           case KEY_AUTO:
                fTimeEdit =  FLAG_RESET;
                pCurObject =  (xScreenObjet *)&menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject];
                vExitCurObject();
                return;
           default:
                break;
        }
     }
    //Обащя навигация по меню, есди не ничего для рекатирования не выбратно
    if ( ucActiveObject == NO_SELECT_D )
    {
        switch(key)
       {
           case KEY_STOP:
            uiSetting       -= ( uiSetting > FIRST_VALID_SETTING   )  ? 1U : 0U;     //Уменьшаем номер текущей уставки
            menu->pCurrIndex = ( uiSetting < FIRST_SETTING )          ? 1U : 0U;     //Если перешли к окну времени
            break;
          case KEY_START:
            uiSetting        += ( uiSetting <= ( SETTING_REGISTER_NUMBER - 2U ) ) ? 1U : 0U; //Увиличиваем номер текущей уставки
            menu->pCurrIndex  = ( uiSetting >= FIRST_SETTING )                    ? 0U : 1U; //Если перескакивам из окна ред. времени обранто
            break;
          case KEY_DOWN:
            uiSetting       -= ( uiSetting >= 10U )        ? 10U : 0U ;             //Уменьшаем номер текущей уставки на 10
            menu->pCurrIndex = (uiSetting < FIRST_SETTING) ? 1U : 0U;               //Если перешли к окну времени
            break;
          case KEY_UP:
            uiSetting       += (  uiSetting <= ( SETTING_REGISTER_NUMBER - 12U )  ) ? 10U : 0U; //Увиличиваем номер текущей уставки на 10
            menu->pCurrIndex =  (uiSetting >=FIRST_SETTING) ? 0U : 1U;                          //Если перескакивам из окна ред. времени обранто
            break;
          case KEY_AUTO:
            if ((fPassowordCorrect == FLAG_SET) || (systemPassword.status == PASSWORD_RESET) )
            {
              ucActiveObject = SELECT_D;
              for (uint8_t i=0; ( ( i < MAX_SCREEN_OBJECT ) &&  ( vSelectLastObject(menu,i) != 1) );i++);
            }
            else
            {
              xPasswordMenu.pHomeMenu[0U].pUpScreenSet = pCurrMenu;
              pCurrMenu = &xPasswordMenu;
            }
            break;
           default:
             break;
         }
      }
  return;
}

static uint8_t CurPassDigitSelect = 0;

void vPasswordHide(xScreenSetObject* menu)
{
  password[0] = 0U;
  password[1] = 0U;
  password[2] = 0U;
  password[3] = 0U;
  menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[CurPassDigitSelect].ObjectParamert[3U] = 0U;
  menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[0].ObjectParamert[3U] = 1U;
  pCurrMenu = pCurrMenu->pHomeMenu[0U].pUpScreenSet;
  CurPassDigitSelect = 0;
}


void xPasswordScreenCallBack ( xScreenSetObject* menu, char key )
{
  switch (key)
  {
    case KEY_START:
         if (CurPassDigitSelect < 3)
         {
           menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[CurPassDigitSelect].ObjectParamert[3U] = 0U;
           CurPassDigitSelect++;
           menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[CurPassDigitSelect].ObjectParamert[3U] = 1U;
         }
         break;
    case KEY_STOP:
         if   (CurPassDigitSelect > 0)
         {
           menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[CurPassDigitSelect].ObjectParamert[3U] = 0U;
           CurPassDigitSelect--;
           menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[CurPassDigitSelect].ObjectParamert[3U] = 1U;
         }
         break;
    case KEY_UP:
         if ( password[CurPassDigitSelect] < 9 )
         {
           password[CurPassDigitSelect]++;
         }
         break;
    case KEY_DOWN:
          if ( password[CurPassDigitSelect] > 0 )
          {
             password[CurPassDigitSelect]--;
          }
          break;
    case KEY_AUTO:
          menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[CurPassDigitSelect].ObjectParamert[3U] = 0U;
          menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[0].ObjectParamert[3U] = 1U;
          CurPassDigitSelect = 0;
          if ((password[0]*1000 + password[1]*100 + password[2]*10 + password[3]) == systemPassword.data)
          {
            fPassowordCorrect = FLAG_SET;
          }
          vPasswordHide(menu);
          ( fPassowordCorrect == FLAG_SET ) ? pCurrMenu->pFunc( pCurrMenu, KEY_AUTO ) : vMenuMessageShow("Неверный пароль!");
         break;
    case KEY_EXIT:
          vPasswordHide(menu);
          pCurrMenu->pFunc( pCurrMenu, KEY_EXIT );
         break;
    default:
         break;
  }
  return;
}

static  uint8_t   key_ready =0;
/*---------------------------------------------------------------------------------------------------*/
//Функция обработки нажатий в базовых меню

void xInfoScreenCallBack ( xScreenSetObject* menu, char key )
{
  uint8_t           index = menu->pCurrIndex;
  xScreenSetObject* pMenu = menu;


  switch ( key )
  {
    case KEY_UP:
      key_ready|= SET_MENU_READY;
      if  ( (key_ready & (SET_MENU_READY | STOP_KEY_READY )) ==(SET_MENU_READY | STOP_KEY_READY ))
      {
         //Переход в меню
         pCurrMenu = &xSettingsMenu;
         pCurrMenu->pCurrIndex = 0U;
         key_ready =0;
      }
      break;
    case KEY_UP_BREAK:
      //Смотрим, не находимся ли мы в экранах нижнего уровня
      if ( fDownScreen == FLAG_SET )
      {
        fDownScreen = FLAG_RESET;
        if ( menu->pHomeMenu[index].pUpScreenSet != NULL )
        {
          pCurrMenu = menu->pHomeMenu[index].pUpScreenSet;
          pMenu     = pCurrMenu;
        }
      }
      pMenu->pCurrIndex = ( pMenu->pCurrIndex == pMenu->pMaxIndex ) ? 0U : pMenu->pCurrIndex + 1;
      uCurrentAlarm =0;
      key_ready &= ~SET_MENU_READY;
      break;
    case KEY_DOWN_BREAK:
        //Если нажата клавиша вниз, проверяем флаг, сигнализурующий что мы листаем
        //карусель вложенных экранов
        if ((menu->pHomeMenu[index].pDownScreenSet == &xAlarmMenu) || (menu->pHomeMenu[index].pDownScreenSet == &xEventMenu))
        {
          uCurrentAlarm++;
        }
        else
        {
            if ( fDownScreen == FLAG_RESET )
            {
              if ( menu->pHomeMenu[index].pDownScreenSet != NULL )
              {
                pCurrMenu             = menu->pHomeMenu[index].pDownScreenSet;
                fDownScreen            = FLAG_SET;
                pCurrMenu->pCurrIndex = 0U;
              }
            }
            else
            {
              pMenu->pCurrIndex = ( pMenu->pCurrIndex == pMenu->pMaxIndex ) ? 0U : pMenu->pCurrIndex + 1;
            }
        }
       break;
    case KEY_STOP:
      if ((key_ready & STOP_KEY_READY )==0U)
      {
        key_ready |= STOP_KEY_READY;
        xTaskNotify( xProccesToNotify, ( uint32_t )HMI_CMD_STOP, eSetBits );

      }
      break;
    case KEY_AUTO:
      if (( key_ready & AUTO_KEY_READY)==0U)
      {
        key_ready |=AUTO_KEY_READY;
        xTaskNotify( xProccesToNotify, ( uint32_t )HMI_CMD_AUTO, eSetBits );
      }
      break;
    case KEY_START:
      if ((key_ready &  START_KEY_READY)==0U)
      {
          key_ready |= START_KEY_READY;
          xTaskNotify( xProccesToNotify, ( uint32_t )HMI_CMD_START, eSetBits );
      }
      break;
    case KEY_AUTO_BREAK:
      key_ready &= ~AUTO_KEY_READY;
      break;
    case KEY_START_BREAK:
      key_ready &= ~START_KEY_READY;
      break;
    case KEY_STOP_BREAK:
      key_ready &=  ~STOP_KEY_READY;
      break;
    case KEY_EXIT:
      fDownScreen   = FLAG_RESET;
      break;
    default:
      break;
  }
  return;
}

/*
 *  Функция обработки экрана подтверждения
 */
void vExitCurObject ( void )
{

  pCurObject->ObjectParamert[3U] = 0U;
  if ( ucActiveObject == CHANGE_D )
  {
      xYesNoMenu.pHomeMenu[0U].pUpScreenSet = pCurrMenu;
      pCurrMenu = &xYesNoMenu;
  }
  ucActiveObject = NO_SELECT_D;
  return;
}

/*---------------------------------------------------------------------------------------------------*/
void vMenuInit ( u8g2_t* temp )
{
  u8g2      = temp;
  pCurrMenu = &xMainMenu;
  pKeyboard = pGetKeyboardQueue();
  return;
}

void vMenuMessageInit ( osThreadId_t xmainprocess )
{
  xProccesToNotify = xmainprocess;
}



static FLAG xTimeOutFlag = FLAG_RESET;
static FLAG xKeyIgnore   = FLAG_RESET;
/*---------------------------------------------------------------------------------------------------*/
void vMenuTask ( void )
{



  //Если установлен флаг реинициализации
  if ( ( xEventGroupGetBits( xDATAAPIgetEventGroup() ) &    DATA_API_FLAG_LCD_TASK_CONFIG_REINIT ) > 0U )
  {
     vLCDBrigthInit();
     xEventGroupClearBits( xDATAAPIgetEventGroup(),    DATA_API_FLAG_LCD_TASK_CONFIG_REINIT );
  }

  osDelay(100);
  temp_counter++;
  //Блок отрисовки экранов
  if ( temp_counter == 2U )
  {
    vDrawObject( ( xScreenObjet * )pCurrMenu->pHomeMenu[pCurrMenu->pCurrIndex].pScreenCurObjets );
    vLCDRedraw();
    temp_counter = 0U;
  }

  if ( xQueueReceive( pKeyboard, &TempEvent, 0U ) == pdPASS )
  {
    key = 0U;
    if ( ( TempEvent.Status == MAKECODE ) && ( BufferEvent.Status == MAKECODE ) &&
         ( ( TempEvent.KeyCode | BufferEvent.KeyCode) == 0x03U ) )
    {
      key        = KEY_EXIT;
      EXIT_KEY_F = 0x03U;
    }
    else
    {
       BufferEvent = TempEvent;
      //Если зафиксировано нажатие клавиши
      if ( TempEvent.Status == MAKECODE )
      {
          key = TempEvent.KeyCode | MAKECODE;
      }
      else
      {
        switch ( TempEvent.KeyCode )
        {
          case up_key:
            if ( EXIT_KEY_F == 0U )
            {
              key = KEY_UP_BREAK;
            }
            else
            {
              EXIT_KEY_F = 0U;
            }
            break;
          case down_key:
            if ( EXIT_KEY_F == 0U )
            {
              key = KEY_DOWN_BREAK;
            }
            else
            {
              EXIT_KEY_F = 0U;
            }
            break;
          case stop_key:
          case start_key:
          case auto_key:
            key = TempEvent.KeyCode | BRAKECODE;
            break;
          default:
            break;
        }
      }
    }
    if ( key > 0U )
    {

      //Если возник таймаут нажатия на клавишу
      if ( TempEvent.KeyCode == time_out )
      {
	/*
	 * Если есть нет активных ошибок, то переходим на домашний экран и гасим индикатора
	 */
        if (fAlarmFlag == FLAG_RESET)
        {
          xMainMenu.pCurrIndex = HOME_MENU;
          pCurrMenu = &xMainMenu;
          xTimeOutFlag = FLAG_SET;
          vLCDSetLedBrigth(0);
        }
        /*
         * Если есть активные ошибки, то переходим на экран активных ошибок
         */
        else
        {
          vMenuGotoAlarmScreen();
        }
      }
      else
      {
	/*
	 * Если нажаите клавиши происходит после таймаута, то зажигаем индикатор
	 */
        if (xTimeOutFlag == FLAG_SET)
        {
           if (xKeyIgnore == FLAG_RESET)
           {
               xKeyIgnore   = FLAG_SET;
               vLCDBrigthInit();
           }
           if ((xKeyIgnore == FLAG_SET) && (key & BRAKECODE))
	   {
               xKeyIgnore  = FLAG_RESET;
               xTimeOutFlag = FLAG_RESET;
	   }
           return 0;
        }
        pCurrMenu->pFunc( pCurrMenu, key );
      }
    }
  }
  return;
}

void vMenuGotoAlarmScreen( void)
{
      static LOG_RECORD_TYPE  xrecord;
      static uint16_t         utemp;
      pCurrMenu = &xMainMenu;
      eLOGICERactiveErrorList( ERROR_LIST_CMD_COUNTER,&xrecord,(uint8_t *)&utemp);
      if  (utemp>0)
      {
	  pCurrMenu->pCurrIndex = ALARM_MENU;
      }
      else
      {
	  pCurrMenu->pCurrIndex = HOME_MENU;
      }
      vLCDBrigthInit();
      return;
}


uint8_t GetNumber(char temp)
{
  uint8_t res;
  switch (temp)
  {
    case '0':
     res =0;
     break;
    case '1':
     res = 1;
     break;
    case '2':
     res = 2;
     break;
    case '3':
     res = 3;
     break;
    case '4':
     res = 4;
     break;
    case '5':
     res =5;
     break;
    case '6':
     res = 6;
     break;
    case '7':
     res = 7;
     break;
    case '8':
     res = 8;
     break;
    case '9':
     res = 9;
     break;
  }
  return res;
}

/*---------------------------------------------------------------------------------------------------*/
void vDrawObject( xScreenObjet * pScreenObjects)
{
  char* TEXT      = NULL;
  uint8_t sTEXT[2]={0,0,0};
  uint8_t  Insert    = 0U;
  char  Text[40U] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',' ',' ',' ',' '};
  uint8_t  i         = 0U;
  uint8_t  x_offset  = 0U;
  uint8_t  y_offset  = 0U;
  uint8_t  Redraw    = 0U;

  //Проверяем, какой экран необходимо перерисовать
  if ( pCurDrawScreen != pScreenObjects )  //Если экран изменился
  {
    pCurDrawScreen = pScreenObjects;
    Redraw         = 1U;
  }
  else                     //Если тот же самый экран
  {
    for ( i=0U; i<MAX_SCREEN_OBJECT; i++ ) //Проверяем есть ли на экране динамические объекты
    {
      switch ( pScreenObjects[i].xType )
      {
        case HW_DATA:
        case INPUT_HW_DATA:
        case DATA_STRING:
          Redraw = 1U;
          break;
        default:
          break;
      }
      if ( ( pScreenObjects[i].last > 0U ) || ( Redraw != 0U ) )
      {
        break;
      }
    }
  }
  if ( Redraw > 0U )   //Если экран нужно перерисовывать
  {
    u8g2_ClearBuffer( u8g2 );
    for ( i=0U; i<MAX_SCREEN_OBJECT; i++ )
    {
      Insert = 0U;
      switch ( pScreenObjects[i].xType )
      {
        //Если текущий объект - строка
        case H_LINE:
          u8g2_SetDrawColor( u8g2, pScreenObjects[i].ObjectParamert[1U] );
          u8g2_DrawLine( u8g2, pScreenObjects[i].x, pScreenObjects[i].y, pScreenObjects[i].Width, pScreenObjects[i].Height );
          break;
        case STRING:
          break;
        case INPUT_HW_DATA:
        case HW_DATA:
        case TEXT_STRING:
        case DATA_STRING:
          if ( ( pScreenObjects[i].xType == INPUT_HW_DATA ) || ( pScreenObjects[i].xType ==DATA_STRING ) )
          {
            if ( pScreenObjects[i].ObjectParamert[3U] > 0U )
            {
              Insert = 1U;
              Blink = ( Blink > 0U ) ? 0U : 1U;
            }
            if ( !Insert )
            {
              u8g2_SetDrawColor( u8g2, pScreenObjects[i].ObjectParamert[1U]?0U:1U );
            }
            else
            {
              u8g2_SetDrawColor( u8g2, Blink?0U:1U );
            }
            u8g2_DrawBox( u8g2, pScreenObjects[i].x, pScreenObjects[i].y, pScreenObjects[i].Width, pScreenObjects[i].Height );
          }
          u8g2_SetFontMode( u8g2, pScreenObjects[i].ObjectParamert[0U] );
          if ( !Insert )
          {
            u8g2_SetDrawColor( u8g2, pScreenObjects[i].ObjectParamert[1U] );
          }
          else
          {
            u8g2_SetDrawColor( u8g2, Blink );
          }
          switch ( pScreenObjects[i].xType )
          {
            case STRING:
              break;
            case TEXT_STRING:
            case DATA_STRING:
              TEXT = pScreenObjects[i].pStringParametr;
              break;
            case HW_DATA:
            case INPUT_HW_DATA:
              ( pScreenObjects[i].DataID > 0U ) ? pScreenObjects[i].GetDtaFunction( mREAD, &Text, pScreenObjects[i].DataID ) : pScreenObjects[i].GetDtaFunction( mREAD, &Text );
              TEXT = (char*)Text;
              break;
            default:
              break;
          }
          u8g2_SetFont( u8g2, FONT_TYPE4 );
          if ( pScreenObjects[i].xType != TEXT_STRING )
          {
            switch ( pScreenObjects[i].ObjectParamert[2U] )
            {
              case RIGTH_ALIGN:
                x_offset =  pScreenObjects[i].x + ( pScreenObjects[i].Width - u8g2_GetUTF8Width( u8g2, TEXT ) ) - 1U;
                break;
              case LEFT_ALIGN:
                x_offset =  pScreenObjects[i].x + 1U;
                break;
              default:
                x_offset =  pScreenObjects[i].x + ( pScreenObjects[i].Width - u8g2_GetUTF8Width( u8g2, TEXT ) ) / 2U;
                break;
            }
            y_offset =  pScreenObjects[i].y + ( pScreenObjects[i].Height / 2U ) + ( u8g2_GetAscent( u8g2 ) / 2U );
            u8g2_DrawUTF8( u8g2,x_offset, y_offset, TEXT );
          }
          else
          {
            if (TEXT[0]!='$')
            {
              u8g2_DrawUTF8( u8g2, pScreenObjects[i].x, pScreenObjects[i]. y, TEXT );
            }
            else
            {

              u8g2_SetFont( u8g2, FONT_TYPES );
              sTEXT[0] =GetNumber(TEXT[1])*100+GetNumber(TEXT[2])*10+GetNumber(TEXT[3]);
              u8g2_DrawStr( u8g2, pScreenObjects[i].x, pScreenObjects[i]. y, sTEXT );
            }
          }
          break;
        default:
           break;
      }
      if ( pScreenObjects[i].last == LAST_OBJECT )
      {
    	  break;
      }
    }
  }
  return;
}


static uint8_t  StringShift   = 0;
static uint8_t  StringShift1  = 0;
static uint8_t  BufferAlarm   = 0;
static uint8_t  ScrollDelay   = 0;
static uint8_t  BufAlarmCount = 0;
static uint16_t  uCurPointer   = 0;
 /* Вспомогательная функция для vGetAlarmForMenu. Предназначена для вывода текущей отображаемой ошибки или события в форматие x/общее количество событий
 *
 */
void vEventCountPrintFunction(uint16_t  utemp,char * Data )
{
     uint16_t pointer;
     char   TS[6];
     if (uCurrentAlarm >= utemp)
     {
       uCurrentAlarm=0U;
     }
     if ((BufAlarmCount!=utemp) || ( uCurrentAlarm != BufferAlarm ) )
     {
       BufAlarmCount   = utemp;
       BufferAlarm     = uCurrentAlarm;
       eDATAAPIlogPointer(DATA_API_CMD_READ_CASH,&pointer);
       if (utemp < LOG_SIZE )
       {
         uCurPointer = pointer -uCurrentAlarm -1U;
       }
       else
       {
         if ((pointer -uCurrentAlarm-1) >= 0 )
         {
           uCurPointer = pointer -uCurrentAlarm -1;
         }
         else
         {
           uCurPointer = (LOG_SIZE -1) - (uCurrentAlarm -pointer  );
         }
       }
       StringShift     = 0U;
       StringShift1    = 0U;
     }
     if (utemp >0)
     {
       vUToStr(Data,uCurrentAlarm+1,0);
       vStrAdd(Data," / ");
       vUToStr(TS,utemp,0);
       vStrAdd(Data,TS);
     }
     else
     {
       vStrCopy(Data,"0 / 0");
     }
     if (++ScrollDelay>VIEW_DELAY)
     {
       ScrollDelay = VIEW_DELAY -1U;
     }
}
/*
 * Вспомогательная функция для vGetAlarmForMenu.
 *
 */
char * vScrollFunction(uint16_t utemp, uint8_t  * shift)
{
  char * StartArray;
  if (utemp> 39U)
  {
     StartArray =&TempArray[*shift];
     if ((utemp-(*shift))<39)
     {
        StartArray[utemp-(*shift)]=0U;
     }
     else
     {
        StartArray[39]=0U;
     }
     if ( ScrollDelay >=VIEW_DELAY)
     {
        *shift=*shift+1;
        if ((*shift) >= (utemp-38U))
        {
          *shift=0;
          ScrollDelay =0;
        }
     }
  }
  else
  {
     *shift =0U;
     StartArray= TempArray;
  }

   return StartArray;
}



/*
 * Функция для отображения списка текущх ошибок или списка событий
 * !!!Важно.Для кооректого исполнения комманд, первой должна обязатаельнос вывполнятся команда ALARM_COUNT или EVENT_COUNT
 * В связи с тем, что функция писалась под конкретную сруктуру вывода в меню, для сокращения обращений к памяти, текущая запись кэшируется при выволненении команды ALARM_COUNT или EVENT_COUNT
 */
static uint16_t         utemp;
static LOG_RECORD_TYPE  xrecord;
static uint8_t          ALD   = 0;

void vGetAlarmForMenu( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  char   TS[6];
  vStrCopy(Data," ");
  switch (ID)
  {
    case ALARM_STATUS:
      //Это команда объекта, выводящего статус в угулу экрана, помимо этого, объект контролирует возниконвение алармов
      eLOGICERactiveErrorList( ERROR_LIST_CMD_COUNTER,&xrecord,(uint8_t *)&utemp);    //Полосучаем кол-во актвиных алармов
      if (utemp >0)   //Если есть активные алармы
      {
         if ( fAlarmFlag == FLAG_RESET ) //И если их до этого не было, т.е. аларм возник прямо сейчас
         {
             fAlarmFlag = FLAG_SET;     //Ставим флаг
             vMenuGotoAlarmScreen();
             pCurrMenu->pFunc( pCurrMenu, KEY_EXIT ); //И отпраляем в текущуе меню команды выхода, переход на экран алармов осуществет обработчик текущего меню
         }
         if ( ++ALD > BLINK_TIME )
         {
           vStrCopy(Data,"О");
           if (ALD > (BLINK_TIME*2))
           {
             ALD=0;
           }
         }
      }
      else
      {
        fAlarmFlag = FLAG_RESET;
      }
      break;
    case ALARM_COUNT:
      eLOGICERactiveErrorList( ERROR_LIST_CMD_COUNTER,&xrecord,(uint8_t *)&utemp);
      vEventCountPrintFunction(utemp,Data);
      eLOGICERactiveErrorList(ERROR_LIST_CMD_READ,&xrecord,(uint8_t *)&uCurrentAlarm);
      break;
    case EVENT_COUNT:
      eDATAAPIlog(DATA_API_CMD_COUNTER,&utemp,&xrecord);
      vEventCountPrintFunction(utemp,Data);
      eDATAAPIlog(DATA_API_CMD_READ_CASH,&uCurPointer,&xrecord);
      break;
    case CURRENT_EVENT_TIME:
    case CURRENT_ALARM_TIME:
      if (uCurrentAlarm < BufAlarmCount)
       {
                  vUNToStr( Data, (int)GET_LOG_DAY( xrecord.time ),2);
                  vStrAdd(Data,":");
                  vUNToStr( TS, (int)GET_LOG_MONTH( xrecord.time ),2);
                  vStrAdd(Data,TS);
                  vStrAdd(Data,":");
                  vUNToStr( TS,(int) LOG_START_YEAR + (int)GET_LOG_YEAR(xrecord.time) ,2);
                  vStrAdd(Data,TS);
                  vStrAdd(Data,"  ");
                  vUNToStr( TS, (int)GET_LOG_HOUR(xrecord.time),2);
                  vStrAdd(Data,TS);
                  vStrAdd(Data,":");
                  vUNToStr( TS, (int)GET_LOG_MIN( xrecord.time ),2);
                  vStrAdd(Data,TS);
                  vStrAdd(Data,":");
                  vUNToStr( TS, (int)GET_LOG_SEC( xrecord.time ) ,2);
                  vStrAdd(Data,TS);
       }
      break;
    case CURRENT_ALARM_T:
    case CURRENT_EVENT_T:
      if (uCurrentAlarm < BufAlarmCount)
      {
        switch (xrecord.event.type)
        {
          case EVENT_USER_FUNCTION_A:
            eDATAAPIconfigValue( DATA_API_CMD_READ, DIA_MESSAGE0_ADR, (uint16_t*) &Data[0U] );
            eDATAAPIconfigValue( DATA_API_CMD_READ, DIA_MESSAGE1_ADR, (uint16_t*) &Data[4U] );
            eDATAAPIconfigValue( DATA_API_CMD_READ, DIA_MESSAGE2_ADR, (uint16_t*) &Data[8U] );
            eDATAAPIconfigValue( DATA_API_CMD_READ, DIA_MESSAGE3_ADR, (uint16_t*) &Data[12U] );
            vStrCopy(Data,vScrollFunction(strlen(TempArray), &StringShift));
            break;
          case EVENT_USER_FUNCTION_B:
            eDATAAPIconfigValue( DATA_API_CMD_READ, DIB_MESSAGE0_ADR,(uint16_t *) &Data[0U] );
            eDATAAPIconfigValue( DATA_API_CMD_READ, DIB_MESSAGE1_ADR,(uint16_t *) &Data[4U] );
            eDATAAPIconfigValue( DATA_API_CMD_READ, DIB_MESSAGE2_ADR,(uint16_t *) &Data[8U] );
            eDATAAPIconfigValue( DATA_API_CMD_READ, DIB_MESSAGE3_ADR,(uint16_t *) &Data[12U] );
            vStrCopy(Data,vScrollFunction(strlen(TempArray), &StringShift));
            break;
          case EVENT_USER_FUNCTION_C:
            eDATAAPIconfigValue( DATA_API_CMD_READ, DIC_MESSAGE0_ADR,(uint16_t *) &Data[0U] );
            eDATAAPIconfigValue( DATA_API_CMD_READ, DIC_MESSAGE1_ADR,(uint16_t *) &Data[4U] );
            eDATAAPIconfigValue( DATA_API_CMD_READ, DIC_MESSAGE2_ADR,(uint16_t *) &Data[8U] );
            eDATAAPIconfigValue( DATA_API_CMD_READ, DIC_MESSAGE3_ADR,(uint16_t *) &Data[12U] );
            vStrCopy(Data,vScrollFunction(strlen(TempArray), &StringShift));
            break;
          case EVENT_USER_FUNCTION_D:
            eDATAAPIconfigValue( DATA_API_CMD_READ, DID_MESSAGE0_ADR,(uint16_t *) &Data[0U] );
            eDATAAPIconfigValue( DATA_API_CMD_READ, DID_MESSAGE1_ADR,(uint16_t *) &Data[4U] );
            eDATAAPIconfigValue( DATA_API_CMD_READ, DID_MESSAGE2_ADR,(uint16_t *) &Data[8U] );
            eDATAAPIconfigValue( DATA_API_CMD_READ, DID_MESSAGE3_ADR,(uint16_t *) &Data[12U] );
            vStrCopy(Data,vScrollFunction(strlen(TempArray), &StringShift));
            break;
          default:
            vStrCopy(TempArray,(char*)logTypesDictionary[xrecord.event.type]);
            vStrCopy(Data,vScrollFunction(strlen(TempArray), &StringShift));
            break;
        }
      }
      else
      {
        vStrCopy(Data,"ОШИБОК НЕТ");
      }
      break;
    case CURRENT_ALARM_ACTION:
    case CURRENT_EVENT_ACTION:
      if (uCurrentAlarm < BufAlarmCount)
      {
         vStrCopy(TempArray,(char*)logActionsDictionary[xrecord.event.action]);
         vStrCopy(Data,vScrollFunction(strlen(TempArray), &StringShift1));
      }
      break;
    default:
      break;
  }
 return;
}

void vGetPasswordData( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  Data[0]=password[ID-1]+'0';
  Data[1]=0;
}



void vGetTIMEForMenu( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  Data[1] = 0;
  if ( fTimeEdit ==  FLAG_RESET)
  {
    vRTCgetCashTime (&buftime );
  }
  uDataType = 4;
  switch (ID)
  {
    case HOUR:
      vUNToStr(Data, buftime.hour,2);
      break;
    case MINUTE:
      vUNToStr(Data, buftime.min,2);
      break;
    case DAY:
      vUNToStr(Data, buftime.day,2);
      break;
    case MOUNTH:
      vUNToStr(Data, buftime.month,2);
      break;
    case YEAR:
      vUNToStr(Data, buftime.year,2);
      break;
    default:
      break;
  }
  return;
}


void vGetFPIForMenu( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  TRIGGER_STATE  DS;
  DS = eFPIgetState( ID-1 );
  Data[0] = ( DS == TRIGGER_IDLE ) ? '0' : '1' ;
  Data[1] = 0;
  return;
}

void vGetFPOForMenu( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  TRIGGER_STATE  DS;
  DS = eFPOgetState ( ID-1 );
  Data[0] = ( DS == TRIGGER_IDLE ) ? '0' : '1' ;
  Data[1] = 0;
  return;
}

void vGetControllerStatus( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
   DEVICE_INFO xStatus;
   RTC_TIME    time;
   char        TS[6];

   if (cmd != mREAD ) return;

   switch (ID)
   {
     case  CONTROLER_STATUS:
       vSTATUSget(&xStatus);
       vStrCopy(Data,(char *)cSTATUSgetString(xStatus.status));
       break;
     case  STATUS_TIME:
       vSTATUSget(&xStatus);
       ( xStatus.timer ==PERMISSION_ENABLE ) ? vUToStr( Data, xStatus.time,0) : vStrCopy(Data,"  ");
       break;
     case  TIME_DATE:
       vRTCgetCashTime (&time );
       vUNToStr( Data, time.day,2);
       vStrAdd(Data,":");
       vUNToStr( TS, time.month,2);
       vStrAdd(Data,TS);
       vStrAdd(Data,":");
       vUNToStr( TS, time.year,2);
       vStrAdd(Data,TS);
       vStrAdd(Data,"  ");
       vUNToStr( TS, time.hour,2);
       vStrAdd(Data,TS);
       vStrAdd(Data,":");
       vUNToStr( TS, time.min,2);
       vStrAdd(Data,TS);
       break;
     default:
       break;
   }
  return;
}
/***********************************************************************************вывод в меню данных контроллера*************************************************************************************/
/*
 * Сервисная функция вывода в строку серийного номера
 */
void vMenuPrintSerial(char * str, uint16_t * serial, uint8_t ofs)
{
  for (uint8_t i=0;i<3;i++)
  {
       str[i*6]  = cHexToChar( (serial[ i + ofs ] >> 12)  & 0x0F);
       str[i*6+1]= cHexToChar( (serial[ i + ofs ] >> 8 )  & 0xF );
       str[i*6+2]= ':';
       str[i*6+3]= cHexToChar( (serial[ i + ofs ] >> 4 )  & 0xF );
       str[i*6+4]= cHexToChar( (serial[ i + ofs ])        & 0xF );
       str[i*6+5]=':';
  }
  str[17]=0;
  return;
}
/*
 *  Функция выврла сосиряния аналоговых входов
 */
void vPrintAinData(char* Data, uint8_t ID)
{
  fix16_t temp;
  switch (ID)
  {
    case FUEL_LEVEL:
      switch (xADCGetFLChType())
      {
        case SENSOR_TYPE_RESISTIVE:
           eCHARTfunc(&fuelSensorChart,  xADCGetSFL() ,   &temp);
           fix16_to_str( temp, Data, 0U );
           vStrAdd(Data,"%");;
           break;
        case SENSOR_TYPE_NORMAL_OPEN:
        case SENSOR_TYPE_NORMAL_CLOSE:
           vStrCopy(Data, eENGINEgetFuelSensorState() == TRIGGER_SET ? "Активен" : "Не актив." );
           break;
        default:
           Data[0]=0;
           break;
      }
      break;
     case OIL_PRESSURE:
       switch(xADCGetxOPChType())
       {
         case SENSOR_TYPE_RESISTIVE:
            eCHARTfunc(charts[OIL_CHART_ADR],  xADCGetSOP() ,   &temp);
            fix16_to_str( temp, Data, 2U );
            vStrAdd(Data," Бар");
            break;
         case SENSOR_TYPE_NORMAL_OPEN:
         case SENSOR_TYPE_NORMAL_CLOSE:
            vStrCopy(Data, eENGINEgetOilSensorState() == TRIGGER_SET ? "Активен" : "Не актив." );
            break;
         default:
            Data[0]=0;
            break;
         }
         break;
      case  COOL_TEMP:
         switch (xADCGetxCTChType())
         {
           case SENSOR_TYPE_RESISTIVE:
             eCHARTfunc(charts[COOLANT_CHART_ADR], xADCGetSCT() ,   &temp);
             fix16_to_str( temp, Data, 0U );
             vStrAdd(Data,"    С");
             break;
          case SENSOR_TYPE_NORMAL_OPEN:
          case SENSOR_TYPE_NORMAL_CLOSE:
             vStrCopy(Data, eENGINEgetCoolantSensorState() == TRIGGER_SET ? "Активен" : "Не актив." );
             break;
           default:
             Data[0]=0;
             break;
          }
         break;
        default:
          break;
  }
}


/*
 * Функция вывода данных
 */
void vGetDataForMenu( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{

  uint16_t utempdata;
  xADCRotatinType xfase;
  uint16_t tt[6]={0,0,0,0,0,0};
  char TS[6];
  eConfigAttributes ATR;
  if (xADCGetScheme()==ELECTRO_SCHEME_SINGLE_PHASE)
  {
     switch (ID)
     {
       case NET_L2_LINE_V:
       case NET_L3_LINE_V:
       case NET_L2_FASE_V:
       case NET_L3_FASE_V:
       case GEN_L2_LINE_V:
       case GEN_L3_LINE_V:
       case GEN_L2_FASE_V:
       case GEN_L3_FASE_V:
       case GEN_L2_ACTIVE_POWER:
       case GEN_L3_ACTIVE_POWER:
       case GEN_L2_REAL_POWER:
       case GEN_L3_REAL_POWER:
       case GEN_L2_REAC_POWER:
       case GEN_L3_REAC_POWER:
       case GEN_L2_CUR:
       case GEN_L3_CUR:
       case NET_ROTATION:
       case GEN_ROTATION:
         Data[0]=0;
         return;
       default:
         break;
     }
  }
     switch (ID)
     {
       case  IP_ADRESS:
            uSERVERgetStrIP( Data );
             break;
       case HW_VER:
       case SW_VER:
            eDATAAPIconfigAtrib(DATA_API_CMD_READ, ( ID == HW_VER ) ? VERSION_CONTROLLER_ADR : VERSION_FIRMWARE_ADR ,&ATR);
            if (ATR.len ==3 )
            {
                eDATAAPIconfigValue(DATA_API_CMD_READ, (ID == HW_VER ) ? VERSION_CONTROLLER_ADR : VERSION_FIRMWARE_ADR ,(uint16_t*)&tt);
                vUToStr(TS,tt[0]/10,0);
                vStrCopy(Data,TS);
                vUToStr(TS,tt[0]%10,0);
                 vStrAdd(Data,TS);
                vStrAdd(Data,".");
                vUToStr(TS,tt[1]/10,0);
                vStrAdd(Data,TS);
                vUToStr(TS,tt[1]%10,0);
                vStrAdd(Data,TS);
                vStrAdd(Data,".");
                vUToStr(TS,tt[2]/10,0);
                vStrAdd(Data,TS);
                vUToStr(TS,tt[2]%10,0);
                vStrAdd(Data,TS);
            }
            break;
       case SERIAL_L:
       case SERIAL_H:
            eDATAAPIconfigValue(DATA_API_CMD_READ,SERIAL_NUMBER0_ADR, (uint16_t*)&tt[0U]);
            eDATAAPIconfigValue(DATA_API_CMD_READ,SERIAL_NUMBER1_ADR, (uint16_t*)&tt[3U]);
            vMenuPrintSerial(Data,(uint16_t*) &tt , ID == SERIAL_L ? 0 : 3);
            break;
      case COOL_TEMP   :
      case FUEL_LEVEL  :
      case OIL_PRESSURE:
           vPrintAinData(Data, ID);
           break;
      case  IN_VDD:
           fix16_to_str( xADCGetVDD(), Data, 2U );
           vStrAdd(Data,"В");
           break;
      case IN_CAC:
           fix16_to_str( xADCGetCAC(), Data, 2U );
           vStrAdd(Data,"В");
           break;
      case NET_L2_LINE_V:
      case NET_L3_LINE_V:
      case NET_L2_FASE_V:
      case NET_L3_FASE_V:
      case GEN_L2_LINE_V:
      case GEN_L3_LINE_V:
      case GEN_L2_FASE_V:
      case GEN_L3_FASE_V:
      case NET_L1_FASE_V:
      case GEN_L1_LINE_V:
      case NET_L1_LINE_V:
      case GEN_L1_FASE_V:
      case GEN_AVER_V:
           fix16_to_str(  xADCGetREG(ID), Data, 0U );
           vStrAdd(Data," В");
           break;
      case NET_FREQ :
      case GEN_FREQ :
           fix16_to_str(  xADCGetREG(ID), Data, 2U );
           vStrAdd(Data," Гц");
           break;
      case GEN_L2_CUR:
      case GEN_L3_CUR:
      case GEN_L1_CUR:
      case GEN_AVER_A:
           fix16_to_str(  xADCGetREG(ID), Data, 2U );
           vStrAdd(Data," А");
           break;
      case GEN_L2_REAC_POWER:
      case GEN_L3_REAC_POWER:
      case GEN_L1_REAC_POWER:
      case GEN_REACTIVE_POWER:
            fix16_to_str(  xADCGetREG(ID), Data, 2U );
           vStrAdd(Data," кВАр");
           break;
      case GEN_L2_REAL_POWER:
      case GEN_L3_REAL_POWER:
      case GEN_L1_REAL_POWER:
      case GEN_REAL_POWER:
           fix16_to_str(  xADCGetREG(ID), Data, 2U );
           vStrAdd(Data," кВА");
           break;
      case GEN_L2_ACTIVE_POWER:
      case GEN_L3_ACTIVE_POWER:
      case GEN_L1_ACTIVE_POWER:
      case GEN_ACTIVE_POWER:
           fix16_to_str(  xADCGetREG(ID), Data, 2U );
           vStrAdd(Data," кВт");
           break;
      case NET_ROTATION:
      case GEN_ROTATION:
           xfase = (ID ==NET_ROTATION) ? xADCGetNetFaseRotation() : xADCGetGenFaseRotation() ;
           switch (xfase)
           {
             case B_C_ROTATION:
              vStrCopy(Data,"L1-L2-L3");
              break;
            case C_B_ROTATION:
              vStrCopy(Data,"L1-L3-L2");
              break;
            default:
              vStrCopy(Data,"XX-XX-XX");
              break;
           }
           break;
      case ENGINE_SPEED:
           fix16_to_str( fENGINEgetSpeed(), Data, 0U );
           vStrAdd( Data, " об/м" );
           break;
      case ENGINE_SCOUNT:
      case ENGINE_WTIME:
           eDATAAPIfreeData(DATA_API_CMD_READ, ( ID == ENGINE_SCOUNT ) ? ENGINE_STARTS_NUMBER_ADR : ENGINE_WORK_TIME_ADR ,&utempdata);
           vUToStr(Data,utempdata,0);
           break;
      case COS_FI:
           fix16_to_str( xADCGetCOSFi(), Data, 2 );
           break;
      default:
           break;
    }

 return;
}

/*
 *  Вывод пользовательского сообщения на экран
 */
static char MessageData[17];

void vGetMessageData( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  Data[0]=0;
  vStrCopy(Data,MessageData);
  return;
}
/*
 *  Функция обработки нажатия клавиш в окне сообщений. Реагируем только на нажатия, что бы можно было
 *  привязывать вывод сообщения к нажатию на кнопку. В противном случае, окон будет закрываться сразу
 *  же полсе отпускания кноки.
 */
void xMessageScreenCallBack ( xScreenSetObject* menu, char key )
{
  if ( (key & MAKECODE) !=0 )
  {
     vMenuMessageHide();
     if (key == KEY_EXIT)
     {
        pCurrMenu->pFunc( pCurrMenu, KEY_EXIT );
     }
  }
  return;
}
/*
 *  Функция активации окна пользовательского сообщения.
 */
void vMenuMessageShow(char * mes)
{
  xMessageMenu.pHomeMenu[0U].pUpScreenSet = pCurrMenu;
  pCurrMenu = &xMessageMenu;
  vStrCopy(MessageData,mes);
  return;
}
/*
 *  Функция закрытия окна сообщения
 */
void vMenuMessageHide(void)
{
    pCurrMenu = xMessageMenu.pHomeMenu[0U].pUpScreenSet;
    return;
}

