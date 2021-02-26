/*
 * menu.c
 *
 *  Created on: 21 февр. 2020 г.
 *      Author: igor.dymov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "menu.h"
#include "lcd.h"
#include "keyboard.h"
#include "menu_data.h"
#include "controllerTypes.h"
#include "stdio.h"
#include "server.h"
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


/*----------------------- Functions -----------------------------------------------------------------*/

void vMenuGotoAlarmScreen( void);

/*----------------------- Structures ----------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------*/
/*
 * Функция обработки клавишей меню да-нет?
 */
void xYesNoScreenKeyCallBack ( xScreenSetObject* menu, char key )
{
    switch ( key )
    {
      case KEY_STOP:  //Если клавиша стоп, то подсвечиваем объект "ДА"
        menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[2U].ObjectParamert[3U] = 1U;
        menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[3U].ObjectParamert[3U] = 0U;
        break;
      case KEY_START://Если клавиша старт, то подсвечиваем объект "НЕТ"
        menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[2U].ObjectParamert[3U] = 0U;
        menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[3U].ObjectParamert[3U] = 1U;
      break;
      case KEY_AUTO:
        //Если каливаша AUTO то проверяем объеты меню, если выбран ДА.
        if ( menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[2U].ObjectParamert[3U] == 1U )
        {
          eDATAAPIconfigValue(DATA_API_CMD_SAVE,uiSetting,NULL);
        }
        else
        {
          menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[2U].ObjectParamert[3U] =1U;
          menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[3U].ObjectParamert[3U] =0U;
        }
        eDATAAPIconfigValue(DATA_API_CMD_LOAD,uiSetting,NULL);
        pCurrMenu = xYesNoMenu.pHomeMenu[0U].pUpScreenSet;
        break;
      case KEY_EXIT:  // Если прилетела команда выход, что также может быть и таймаутом,
                      // выходим каскадом в предидущие меню
        menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[2U].ObjectParamert[3U] =1U;
        menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[3U].ObjectParamert[3U] =0U;
        eDATAAPIconfigValue(DATA_API_CMD_LOAD,uiSetting,NULL);
        break;
      default:
        break;
    }
  return;
}



void xSettingsScreenKeyCallBack( xScreenSetObject* menu, char key )
{
  static FLAG              fFirstEnter   = FLAG_SET;
  uint16_t data =0;
  if ( fFirstEnter == FLAG_RESET )
  {
    if (key == (KEY_EXIT))
    {
      menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].ObjectParamert[3U] = 0U;
      pCurrMenu = menu->pHomeMenu[menu->pCurrIndex].pUpScreenSet;
      fDownScreen = FLAG_RESET;
      fFirstEnter = FLAG_SET;
      if (uiSetting >= FIRST_VALID_SETTING)
      {
        eDATAAPIconfigValue(DATA_API_CMD_LOAD,uiSetting,NULL);
      }
      uiSetting  = FIRST_SETTING ;
    }
    if ( ( ucActiveObject !=NO_SELECT_D) && (uDataType == BITMAP))
    {
        switch (key)
        {
          case KEY_DOWN_BREAK:
                  ucActiveObject = CHANGE_D;
                  eDATAAPIconfigValue( DATA_API_CMD_READ, uiSetting, &data );
                  if (data & (0x01<<(menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].DataID-1)))
                      data &= ~(0x01<<(menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].DataID-1));
                  else
                     data |= (0x01<<(menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].DataID-1));
                  eDATAAPIconfigValue( DATA_API_CMD_WRITE, uiSetting, &data );
                  break;

             break;
          case KEY_UP_BREAK:
                   ucActiveObject = CHANGE_D;
                   eDATAAPIconfigValue( DATA_API_CMD_READ, uiSetting, &data );
                   if (data & (0x01<<(menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].DataID-1)))
                       data &= ~(0x01<<(menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].DataID-1));
                   else
                     data |= (0x01<<(menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].DataID-1));
                   eDATAAPIconfigValue( DATA_API_CMD_WRITE, uiSetting, &data );

                 break;
          case KEY_STOP:
                   for (uint8_t i=uCurrentObject-1; i>0;i--)
                   {
                      if (menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[i].last == uDataType )
                      {
                           menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].ObjectParamert[3U] = 0U;
                           uCurrentObject = i;
                           menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].ObjectParamert[3U] = 1U;
                           break;
                      }
                   }
                 break;
          case KEY_START:
                   for (uint8_t i=(uCurrentObject+1); i<MAX_SCREEN_OBJECT;i++)
                   {
                      if (menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[i].last == LO)
                                   break;
                      if (menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[i].last == uDataType )
                      {
                            menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].ObjectParamert[3U] = 0U;
                            uCurrentObject = i;
                            menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].ObjectParamert[3U] = 1U;
                            break;
                        }
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
  if ((uDataType ==  NUMBER) && ( ucActiveObject !=NO_SELECT_D))
  {
     switch (key)
    {
         case KEY_STOP:
                ucActiveObject = CHANGE_D;
                eDATAAPIconfigValue( DATA_API_CMD_DEC, uiSetting, NULL );
                break;
         case KEY_START:
                ucActiveObject = CHANGE_D;
                eDATAAPIconfigValue( DATA_API_CMD_INC, uiSetting, NULL );
                break;
         case KEY_DOWN_BREAK:
               ucActiveObject = CHANGE_D;
               for ( uint8_t i=0U; i<10U; i++ )
               {
                  eDATAAPIconfigValue( DATA_API_CMD_DEC, uiSetting, NULL );
               }
               break;
         case KEY_UP_BREAK:
               ucActiveObject = CHANGE_D;
               for ( uint8_t i=0U; i<10U; i++ )
               {
                  eDATAAPIconfigValue( DATA_API_CMD_INC, uiSetting, NULL );
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
     if ((uDataType ==  DATE_TYPE) && ( ucActiveObject !=NO_SELECT_D))
     {
      switch (key)
                {
                  case KEY_DOWN_BREAK:
                          ucActiveObject = CHANGE_D;
  /*                        eDATAAPIconfigValue( DATA_API_CMD_READ, uiSetting, &data );
                          if (data & (0x01<<(menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].DataID-1)))
                              data &= ~(0x01<<(menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].DataID-1));
                          else
                             data |= (0x01<<(menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].DataID-1));
                          eDATAAPIconfigValue( DATA_API_CMD_WRITE, uiSetting, &data );
                          break;
*/
                     break;
                  case KEY_UP_BREAK:
                           ucActiveObject = CHANGE_D;
                          /* eDATAAPIconfigValue( DATA_API_CMD_READ, uiSetting, &data );
                           if (data & (0x01<<(menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].DataID-1)))
                               data &= ~(0x01<<(menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].DataID-1));
                           else
                             data |= (0x01<<(menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].DataID-1));
                           eDATAAPIconfigValue( DATA_API_CMD_WRITE, uiSetting, &data );
*/
                         break;
                  case KEY_STOP:
                           for (uint8_t i=uCurrentObject-1; i>0;i--)
                           {
                              if (menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[i].last == uDataType )
                              {
                                   menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].ObjectParamert[3U] = 0U;
                                   uCurrentObject = i;
                                   menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].ObjectParamert[3U] = 1U;
                                   break;
                              }
                           }
                         break;
                  case KEY_START:
                           for (uint8_t i=(uCurrentObject+1); i<MAX_SCREEN_OBJECT;i++)
                           {
                              if (menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[i].last == LO)
                                           break;
                              if (menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[i].last == uDataType )
                              {
                                    menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].ObjectParamert[3U] = 0U;
                                    uCurrentObject = i;
                                    menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].ObjectParamert[3U] = 1U;
                                    break;
                                }
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

    //Обащя навигация, в не зависимости от типа данных
    if ( ucActiveObject == NO_SELECT_D )
    {
        switch(key)
       {
           case KEY_STOP:
            if ( uiSetting > FIRST_VALID_SETTING   )
            {
                   uiSetting--;
                   if (uiSetting < FIRST_SETTING)
                   {
                       menu->pCurrIndex = 1;
                   }
            }
            break;
          case KEY_START:
            if ( uiSetting <= ( SETTING_REGISTER_NUMBER - 2U ) )
                  uiSetting++;
            if (uiSetting >=FIRST_SETTING)
              menu->pCurrIndex = 0;
            break;
          case KEY_DOWN_BREAK:
            if ( uiSetting >= 10U )
            {
                uiSetting -= 10U;
                if (uiSetting < FIRST_SETTING)
                {
                  menu->pCurrIndex = 1;
                }
            }
            break;
          case KEY_UP_BREAK:
            if (  uiSetting <= ( SETTING_REGISTER_NUMBER - 12U )  )
                uiSetting += 10U;
            if (uiSetting >=FIRST_SETTING)
               menu->pCurrIndex = 0;
            break;
          case KEY_AUTO:
            if ((fPassowordCorrect == FLAG_SET) || (systemPassword.status == PASSWORD_RESET))
            {
              ucActiveObject = SELECT_D;
              for (uint8_t i=0; i<MAX_SCREEN_OBJECT;i++)
              {
                if (menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[i].last == 1)
                         break;
                if (menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[i].last == uDataType )
                {
                    uCurrentObject = i;
                }
              }
              menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[uCurrentObject].ObjectParamert[3U] = 1U;
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
  }
  else
  {
    //Обработка клавиш начинается только после того, как будут отпущены клавиши UP и BREAK
    fPassowordCorrect = FLAG_RESET;
    password[0] = 0;
    password[1] = 0;
    password[2] = 0;
    password[3] = 0;
    if (key == (KEY_STOP_BREAK))
    {
      fFirstEnter = FLAG_RESET;
    }
  }
  return;
}

static uint8_t CurPassDigitSelect = 0;

void xPasswordScreenCallBack ( xScreenSetObject* menu, char key )
{

   if  ( (key== KEY_START) && (CurPassDigitSelect < 3))
   {
     menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[CurPassDigitSelect].ObjectParamert[3U] = 0U;
     CurPassDigitSelect++;
     menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[CurPassDigitSelect].ObjectParamert[3U] = 1U;
   }
   if  ( (key==KEY_STOP) && (CurPassDigitSelect > 0))
   {
     menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[CurPassDigitSelect].ObjectParamert[3U] = 0U;
     CurPassDigitSelect--;
     menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[CurPassDigitSelect].ObjectParamert[3U] = 1U;
   }
   if ((key == KEY_UP) && ( password[CurPassDigitSelect] < 9 ))
   {
     password[CurPassDigitSelect]++;
   }
   if ((key == KEY_DOWN) && ( password[CurPassDigitSelect] > 0 ))
   {
     password[CurPassDigitSelect]--;
   }
   if ((key == KEY_AUTO) || (key ==KEY_EXIT))
   {
     if (((password[0]*1000 + password[1]*100 + password[2]*10 + password[0]) == systemPassword.data) && (key == KEY_AUTO))
     {
       fPassowordCorrect = FLAG_SET;
     }
     else
     {
       fPassowordCorrect = FLAG_SET;
     }
      menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[CurPassDigitSelect].ObjectParamert[3U] = 0U;
      menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[0].ObjectParamert[3U] = 1U;
      pCurrMenu = xPasswordMenu.pHomeMenu[0U].pUpScreenSet;
      if (key == KEY_EXIT)
      {
         pCurrMenu->pFunc( pCurrMenu, KEY_EXIT );
      }
   }

}


/*---------------------------------------------------------------------------------------------------*/
//Функция обработки нажатий в базовых меню

void xInfoScreenCallBack ( xScreenSetObject* menu, char key )
{
  uint8_t           index = menu->pCurrIndex;
  xScreenSetObject* pMenu = menu;
  static  uint8_t   key_ready =0;

  switch ( key )
  {
    case KEY_UP:
      key_ready|= SET_MENU_READY;
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
      if ( pMenu->pCurrIndex == pMenu->pMaxIndex )
      {
        pMenu->pCurrIndex = 0U;
      }
      else
      {
        pMenu->pCurrIndex++;
      }
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
              if ( menu->pCurrIndex == menu->pMaxIndex )
              {
                menu->pCurrIndex = 0U;
              }
              else
              {
                menu->pCurrIndex++;
              }
            }
        }
       break;
    case KEY_STOP:
      if ((key_ready & STOP_KEY_READY )==0U)
      {
        key_ready |= STOP_KEY_READY;
        xTaskNotify( xProccesToNotify, ( uint32_t )HMI_CMD_STOP, eSetBits );
        if  ((key_ready & SET_MENU_READY)!=0)
        {
          //Переход в меню
            pCurrMenu = &xSettingsMenu;
            pCurrMenu->pCurrIndex = 0U;
        }
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




/*---------------------------------------------------------------------------------------------------*/
void vMenuTask ( void )
{
    //Блок обработки нажатий на клавиши

  static FLAG xTimeOutFlag = FLAG_RESET;
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
      pCurrMenu->pFunc( pCurrMenu, key );
      //Если возник таймаут гашения экрана.
      if ( TempEvent.KeyCode == time_out )
      {
        if (fAlarmFlag == FLAG_RESET)
        {
          pCurrMenu = &xMainMenu;
          xTimeOutFlag = FLAG_SET;
          pCurrMenu->pCurrIndex = HOME_MENU;
          vLCDSetLedBrigth(1);

        }
        else
        {
          vMenuGotoAlarmScreen();
        }
      }
      else
      {
        if (xTimeOutFlag == FLAG_SET)
        {
           xTimeOutFlag =FLAG_RESET;
           vLCDBrigthInit();
        }
      }
    }
  }
  return;
}

void vMenuGotoAlarmScreen( void)
{
      pCurrMenu = &xMainMenu;
      pCurrMenu->pCurrIndex = ALARM_MENU;
      vLCDBrigthInit();
      return;
}

/*---------------------------------------------------------------------------------------------------*/
void vDrawObject( xScreenObjet * pScreenObjects)
{
  char* TEXT      = NULL;
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
              if ( Blink > 0U )
              {
                Blink = 0U;
              }
              else
              {
                Blink = 1U;
              }
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
              if ( pScreenObjects[i].DataID > 0U )
              {
                pScreenObjects[i].GetDtaFunction( mREAD, &Text, pScreenObjects[i].DataID );
              }
              else
              {
                pScreenObjects[i].GetDtaFunction( mREAD, &Text );
              }
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
            u8g2_DrawUTF8( u8g2, pScreenObjects[i].x, pScreenObjects[i]. y, TEXT );
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


void vGetSettingsUnit ( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  eConfigAttributes xAtrib                  = { 0U };
  uint8_t           k                       = 0U;
  uint16_t          buff                    = 0U;
  uint8_t           i                       = 0U;
  int8_t            scale                   = 0U;
  uint16_t          units[MAX_UNITS_LENGTH] = { 0U };
  Data[0U] = 0U;
  switch ( cmd )
  {
    case mREAD:
      eDATAAPIconfigAtrib( DATA_API_CMD_READ, uiSetting, &xAtrib );
      if ( xAtrib.bitMapSize == 0U )
      {
        if ( xAtrib.len == 1U )
        {
          eDATAAPIconfig( DATA_API_CMD_READ, uiSetting, &buff, &scale, units );

          for ( i=0; i<MAX_UNITS_LENGTH; i++ )
          {
            if ( ( units[i] >> 8U) != 0U )
            {
              Data[k++] = units[i] >> 8U;
            }
            Data[k++] = units[i] & 0x00FFU;
          }
          Data[k] = 0U;
        }
      }
      else
      {
        vStrCopy(Data,"    ");
      }
      break;
    default:
      break;
  }
  return;
}




void vGetSettingsBitData( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  eConfigAttributes xAtrib                  = { 0U };
  uint16_t          buff                    =  0U;
  switch ( cmd )
  {
    case  mREAD:
      eDATAAPIconfigAtrib( DATA_API_CMD_READ, uiSetting, &xAtrib );
      if ( xAtrib.bitMapSize != 0U )
      {
        eDATAAPIconfigValue( DATA_API_CMD_READ, uiSetting, &buff );
        Data[0]=((buff>>(ID-1)) & 0x01)+'0';
        Data[1]=0;
        uDataType = 3;
      }
      else
        Data[0]=0;
      break;
    default:
      break;
  }
}

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
                       //    eDATAAPIconfigValue( DATA_API_CMD_READ, uiSetting, &sbuff );
                       //vITOSTRING( ( uint8_t* )Data, buff );
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



void vGetSettingsNumber( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  if (cmd == mREAD)
  {
    vUToStr(Data, uiSetting,0);
    //vUCTOSTRING( ( uint8_t* )Data, (uint8_t) uiSetting);
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

void vGetAlarmForMenu( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  static LOG_RECORD_TYPE  xrecord;

  static uint8_t          ALD   = 0;
  static uint16_t         utemp;
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
         if ( ++ALD>BLINK_TIME )
         {
           vStrCopy(Data,"О");
           if (ALD ==(BLINK_TIME*2))
           ALD=0;
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
            eDATAAPIconfigValue( DATA_API_CMD_READ, DIA_MESSAGE_ADR,(uint16_t *) &Data );
            vStrCopy(Data,vScrollFunction(strlen(TempArray), &StringShift));
            break;
          case EVENT_USER_FUNCTION_B:
            eDATAAPIconfigValue( DATA_API_CMD_READ, DIB_MESSAGE_ADR,(uint16_t *) &Data );
            vStrCopy(Data,vScrollFunction(strlen(TempArray), &StringShift));
            break;
          case EVENT_USER_FUNCTION_C:
            eDATAAPIconfigValue( DATA_API_CMD_READ, DIC_MESSAGE_ADR,(uint16_t *) &Data );
            vStrCopy(Data,vScrollFunction(strlen(TempArray), &StringShift));
            break;
          case EVENT_USER_FUNCTION_D:
            eDATAAPIconfigValue( DATA_API_CMD_READ, DID_MESSAGE_ADR,(uint16_t *) &Data );
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

void vGetFPOForMenu( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  TRIGGER_STATE  DS;
  DS = eFPOgetState ( ID-1 );
  if (DS==TRIGGER_IDLE)
  {
     Data[0] = '0';
  }
  else
  {
     Data[0] = '1';
  }
  Data[1] = 0;

  return;

}

void vGetTIMEForMenu( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  RTC_TIME time;
  Data[1] = 0;
  vRTCgetCashTime (&time );
  uDataType = 4;
  switch (ID)
  {
    case HOUR:
      vUNToStr(Data, time.hour,2);
      break;
    case MINUTE:
      vUNToStr(Data, time.min,2);
      break;
    case DAY:
      vUNToStr(Data, time.day,2);
      break;
    case MOUNTH:
      vUNToStr(Data, time.month,2);
      break;
    case YEAR:
      vUNToStr(Data, time.year,2);
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
  if (DS==TRIGGER_IDLE)
  {
     Data[0] = '0';
  }
  else
  {
     Data[0] = '1';
  }
  Data[1] = 0;

  return;

}


void vGetControllerStatus( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
   DEVICE_INFO xStatus;
   RTC_TIME    time;
   char        TS[6];

   if (cmd !=mREAD ) return;

   switch (ID)
   {
     case  CONTROLER_STATUS:
       vSTATUSget(&xStatus);
       vStrCopy(Data,(char *)cSTATUSgetString(xStatus.status));
       break;
     case  STATUS_TIME:
       vSTATUSget(&xStatus);
       if (xStatus.timer ==PERMISSION_ENABLE)
       {
         vUToStr( Data, xStatus.time,0);
       }
       else
       {
         vStrCopy(Data,"  ");
       }
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

void vGetDataForMenu( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  fix16_t temp;
  uint16_t utempdata;
  xADCRotatinType xfase;
  uint16_t tt[6]={0,0,0,0,0,0};
  eConfigAttributes ATR;
  switch (ID)
  {
    case  IP_ADRESS:
      cSERVERgetStrIP( Data );
      break;
    case HW_VER:
      eDATAAPIconfigAtrib(DATA_API_CMD_READ,VERSION_CONTROLLER_ADR ,&ATR);
      if (ATR.len ==1 )
      {
          eDATAAPIconfigValue(DATA_API_CMD_READ,VERSION_CONTROLLER_ADR ,(uint16_t*)&tt);
          sprintf(Data,"%i.%i",tt[0]/1000,tt[0]%1000);
      }
      break;
    case SW_VER:
      eDATAAPIconfigAtrib(DATA_API_CMD_READ,VERSION_FIRMWARE_ADR,&ATR);
      if (ATR.len ==1 )
      {
        eDATAAPIconfigValue(DATA_API_CMD_READ,VERSION_FIRMWARE_ADR, (uint16_t*)&tt);
        sprintf(Data,"%i.%i",tt[0]/1000,tt[0]%1000);
      }
      break;
    case SERIAL_L:
      eDATAAPIconfigValue(DATA_API_CMD_READ,SERIAL_NUMBER_ADR, (uint16_t*)&tt);
      for (uint8_t i=0;i<3;i++)
      {
           Data[i*6]  = cHexToChar((tt[i]>>12)  & 0x0F );
           Data[i*6+1]= cHexToChar((tt[i]>>8)   & 0xF);
           Data[i*6+2]= ':';
           Data[i*6+3]= cHexToChar((tt[i]>>4)   & 0xF);
           Data[i*6+4]= cHexToChar((tt[i])      & 0xF);
           Data[i*6+5]=':';
      }
      Data[17]=0;
      break;
    case SERIAL_H:
      eDATAAPIconfigValue(DATA_API_CMD_READ,SERIAL_NUMBER_ADR,(uint16_t*)&tt);
      for (uint8_t i=0;i<3;i++)
          {
            Data[i*6]  = cHexToChar((tt[i+3]>>12) & 0x0F );
            Data[i*6+1]= cHexToChar((tt[i+3]>>8)   & 0xF);
            Data[i*6+2]=':';
            Data[i*6+3]= cHexToChar((tt[i+3]>>4)   & 0xF);
            Data[i*6+4]= cHexToChar((tt[i+3])   & 0xF);
            Data[i*6+5]=':';
          }
        Data[17]=0;

      break;
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
             if (eENGINEgetFuelSensorState()==TRIGGER_SET)
                vStrCopy(Data,"Активен");
              else
                vStrCopy(Data,"Не актив.");
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
            eCHARTfunc(&oilSensorChart,  xADCGetSOP() ,   &temp);
            fix16_to_str( temp, Data, 2U );
            vStrAdd(Data," Бар");
            break;
        case SENSOR_TYPE_NORMAL_OPEN:
        case SENSOR_TYPE_NORMAL_CLOSE:
            if (eENGINEgetOilSensorState()==TRIGGER_SET)
              vStrCopy(Data,"Активен");
            else
              vStrCopy(Data,"Не актив.");
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
            eCHARTfunc(&coolantSensorChart, xADCGetSCT() ,   &temp);
            fix16_to_str( temp, Data, 0U );
            vStrAdd(Data," гр.С");
            break;
        case SENSOR_TYPE_NORMAL_OPEN:
        case SENSOR_TYPE_NORMAL_CLOSE:
            if (eENGINEgetCoolantSensorState()==TRIGGER_SET)
              vStrCopy(Data,"Активен");
            else
              vStrCopy(Data,"Не актив.");
            break;
        default:
           Data[0]=0;
           break;
      }
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
      if (xADCGetScheme()==ELECTRO_SCHEME_SINGLE_PHASE)
      {
        Data[0]=0;
      }
      else
      {
        fix16_to_str(  xADCGetREG(ID), Data, 0U );
        vStrAdd(Data," В");
      }
      break;
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
        if (xADCGetScheme()==ELECTRO_SCHEME_SINGLE_PHASE)
        {
              Data[0]=0;
         }
        else
        {
          fix16_to_str(  xADCGetREG(ID), Data, 2U );
           vStrAdd(Data," А");
        }
        break;
    case GEN_L1_CUR:
    case GEN_AVER_A:
      fix16_to_str(  xADCGetREG(ID), Data, 2U );
      vStrAdd(Data," А");
       break;
    case GEN_L2_REAC_POWER:
    case GEN_L3_REAC_POWER:
    if (xADCGetScheme()==ELECTRO_SCHEME_SINGLE_PHASE)
      {
         Data[0]=0;
      }
       else
       {
             fix16_to_str( fix16_div(xADCGetREG(ID),fix16_from_int(1000)), Data, 2U );
             vStrAdd(Data," кВАр");
        }
        break;
     case GEN_L1_REAC_POWER:
         fix16_to_str( fix16_div(xADCGetREG(ID),fix16_from_int(1000)), Data, 2U );
         vStrAdd(Data," кВАр");
         break;
     case GEN_L2_REAL_POWER:
     case GEN_L3_REAL_POWER:
         if (xADCGetScheme()==ELECTRO_SCHEME_SINGLE_PHASE)
           {
              Data[0]=0;
           }
            else
            {
                  fix16_to_str( fix16_div(xADCGetREG(ID),fix16_from_int(1000)), Data, 2U );
                  vStrAdd(Data," кВА");
             }
             break;
      case GEN_L1_REAL_POWER:
              fix16_to_str( fix16_div(xADCGetREG(ID),fix16_from_int(1000)), Data, 2U );
              vStrAdd(Data," кВА");
              break;
   case GEN_L2_ACTIVE_POWER:
   case GEN_L3_ACTIVE_POWER:
       if (xADCGetScheme()==ELECTRO_SCHEME_SINGLE_PHASE)
       {
           Data[0]=0;
       }
       else
       {
         fix16_to_str( fix16_div(xADCGetREG(ID),fix16_from_int(1000)), Data, 2U );
         vStrAdd(Data," кВт");
       }
       break;
   case GEN_REACTIVE_POWER:
   case GEN_L1_ACTIVE_POWER:
     fix16_to_str( fix16_div(xADCGetREG(ID),fix16_from_int(1000)), Data, 2U );
     vStrAdd(Data," кВт");
     break;
    case NET_ROTATION:
    case GEN_ROTATION:
         if (xADCGetScheme()==ELECTRO_SCHEME_SINGLE_PHASE)
         {
             Data[0]=0;
             break;
         }
        if (ID ==NET_ROTATION)
          xfase=xADCGetNetFaseRotation();
        else
          xfase=xADCGetGenFaseRotation();
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
           fix16_to_str( fENGINEspeedGet(), Data, 0U );
           vStrAdd( Data, " об/м" );
           break;
     case  ENGINE_SCOUNT:
           eDATAAPIfreeData(DATA_API_CMD_READ,ENGINE_STARTS_NUMBER_ADR,&utempdata);
           sprintf(Data,"%u",utempdata);
           break;
     case ENGINE_WTIME:
           eDATAAPIfreeData(DATA_API_CMD_READ,ENGINE_WORK_TIME_ADR,&utempdata);
           sprintf(Data,"%u",utempdata);
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
}


void xMessageScreenCallBack ( xScreenSetObject* menu, char key )
{
  static FLAG fKey = FLAG_RESET;
  if (key!=0)
  {
    if (fKey == FLAG_RESET )
    {
      fKey = FLAG_SET;
    }
    else
    {
      fKey = FLAG_RESET;
      vMenuMessageHide();
    }
  }

}

void vMenuMessageShow(char * mes)
{
  xMessageMenu.pHomeMenu[0U].pUpScreenSet = pCurrMenu;
  pCurrMenu = &xMessageMenu;
  pCurrMenu->pCurrIndex = 0U;
  vStrCopy(MessageData,mes);
}

void vMenuMessageHide(void)
{
    pCurrMenu = xMessageMenu.pHomeMenu[0U].pUpScreenSet;
}

