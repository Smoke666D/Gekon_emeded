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
#include "adc.h"
#include "stdio.h"
#include "server.h"
#include "vrSensor.h"
/*------------------------ Define -------------------------------------------------------------------*/
#define NO_SELECT_D   0U
#define SELECT_D      1U
#define CHANGE_D      2U
/*----------------------- Variables -----------------------------------------------------------------*/
static u8g2_t*           u8g2             = NULL;
static osThreadId_t      xProccesToNotify = NULL;
static KeyEvent          TempEvent        = { 0U };
static KeyEvent          BufferEvent      = { 0U };
static uint8_t           temp_counter     = 0U;
static xScreenSetObject* pCurrMenu        = NULL;
static xScreenObjet*     pCurObject       = NULL;
static uint8_t           DownScreen       = 0U;
static QueueHandle_t     pKeyboard        = NULL;
static uint8_t           key              = 0U;
static xScreenObjet*     pCurDrawScreen   = NULL;
static uint8_t           Blink            = 0U;
static uint16_t          uiSetting        = 3U;
static uint8_t           ucActiveObject   = NO_SELECT_D;
static uint8_t           EXIT_KEY_F       = 0U;
/*----------------------- Functions -----------------------------------------------------------------*/
void xYesNoScreenKeyCallBack ( xScreenSetObject* menu, char key );
/*----------------------- Structures ----------------------------------------------------------------*/
xScreenSetObject xYesNoMenu =
{
  xYesNoScreens,
  ( YESNO_MENU_COUNT - 1U ),
  0U,
  ( void* )&xYesNoScreenKeyCallBack,
};
void EventScreenKeyCallBack( xScreenSetObject* menu, char key )
{
  switch ( key )
     {
       case KEY_EXIT:
         pCurrMenu = menu->pHomeMenu[menu->pCurrIndex].pUpScreenSet;
         vExitCurObject();
         DownScreen = 0U;
         uiSetting  = 0U;
         break;
       default:
         break;
     }


}


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
  if ( ucActiveObject != NO_SELECT_D )
  {
  	pCurObject->ObjectParamert[3U] = 0U;
  	if ( ucActiveObject == CHANGE_D )
  	{
  	  xYesNoMenu.pHomeMenu[0U].pUpScreenSet = pCurrMenu;
  	  pCurrMenu = &xYesNoMenu;
  	}
  	ucActiveObject = NO_SELECT_D;
  }
  return;
}

static uint8_t uSettingScreen = 0U;

void xSettingsScreenKeyCallBack( xScreenSetObject* menu, char key )
{

  if ( uSettingScreen == 0U )
  {
    switch ( key )
    {
      case KEY_STOP:
        if  ( ( ucActiveObject == NO_SELECT_D ) &&  ( uiSetting >= 1U ) )
        {
          uiSetting--;
        }
        if ( ucActiveObject != NO_SELECT_D )
        {
          ucActiveObject = CHANGE_D;
          eDATAAPIconfigValue( DATA_API_CMD_DEC, uiSetting, NULL );
        }
        break;
      case KEY_START:
        if ( ( ucActiveObject == NO_SELECT_D ) && ( uiSetting <= ( SETTING_REGISTER_NUMBER - 2U ) ) )
        {
          uiSetting++;
        }
        if ( ucActiveObject != NO_SELECT_D )
        {
          ucActiveObject = CHANGE_D;
          eDATAAPIconfigValue( DATA_API_CMD_INC, uiSetting, NULL );
        }
        break;
      case KEY_DOWN:
        if ( ( ucActiveObject == NO_SELECT_D) && ( uiSetting >= 10U ) )
        {
          uiSetting -= 10U;
        }
        if ( ucActiveObject != NO_SELECT_D )
        {
          ucActiveObject = CHANGE_D;
          for ( uint8_t i=0U; i<10U; i++ )
          {
            eDATAAPIconfigValue( DATA_API_CMD_DEC, uiSetting, NULL );
          }
        }
        break;
      case KEY_UP:
        if ( ( ucActiveObject == NO_SELECT_D)  && ( uiSetting <= ( SETTING_REGISTER_NUMBER - 12U ) ) )
        {
          uiSetting += 10U;
        }
        if ( ucActiveObject != NO_SELECT_D )
        {
          ucActiveObject = CHANGE_D;
          for ( uint8_t i=0U; i<10U; i++ )
          {
            eDATAAPIconfigValue( DATA_API_CMD_INC, uiSetting, NULL );
          }
        }
        break;
      case KEY_AUTO:
        if ( ucActiveObject == NO_SELECT_D )
        {
          ucActiveObject = SELECT_D;
          menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[5U].ObjectParamert[3U] = 1U;
        }
        else
        {
          pCurObject = &menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets[5U];
          vExitCurObject();
        }
        break;
      case KEY_EXIT:
        pCurrMenu = menu->pHomeMenu[menu->pCurrIndex].pUpScreenSet;
        vExitCurObject();
        DownScreen = 0U;
        uiSetting  = 0U;
        break;
      default:
        break;
    }
  }
  else if ( key == KEY_STOP_BREAK )
  {
    uSettingScreen = 0U;
  }
  else
  {

  }
  return;
}



/*---------------------------------------------------------------------------------------------------*/
/* Callback функция пролистывания информационных экранов.
 *
 */
void xInfoScreenCallBack ( xScreenSetObject* menu, char key )
{
  uint8_t           index = menu->pCurrIndex;
  xScreenSetObject* pMenu = menu;

  switch ( key )
  {
    case KEY_UP:
      uSettingScreen = 0U;
      if ( DownScreen > 0U )
      {
        DownScreen = 0U;
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
      break;
    case KEY_DOWN:
      if ( uSettingScreen == 1U )
      {
        pCurrMenu = &xSettingsMenu;
      }
      else
      {
        //Если нажата клавиша вниз, проверяем флаг, сигнализурующий что мы листаем
        //карусель вложенных экранов
        if ( DownScreen == 0U )
        {
          if ( menu->pHomeMenu[index].pDownScreenSet != NULL )
          {
            pCurrMenu             = menu->pHomeMenu[index].pDownScreenSet;
            DownScreen            = 1U;
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
    case KEY_EXIT:
      uSettingScreen = 0U;
      DownScreen     = 0U;
    break;
    case KEY_STOP:
      xTaskNotify( xProccesToNotify, ( uint32_t )HMI_CMD_STOP, eSetBits );
      uSettingScreen = 1U;
      break;
    case KEY_STOP_BREAK:
      uSettingScreen = 0U;
      break;
    case KEY_AUTO:
      xTaskNotify( xProccesToNotify, ( uint32_t )HMI_CMD_AUTO, eSetBits );
      uSettingScreen = 0U;
      break;
    case KEY_START:
      xTaskNotify( xProccesToNotify, ( uint32_t )HMI_CMD_START, eSetBits );
      uSettingScreen = 0U;
      break;
    default:
      break;
  }
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
  uint32_t ulNotifiedValue;
  xTaskNotifyWait( pdFALSE, 0xFFFFFFFU, &ulNotifiedValue, 200U );
  if ( ulNotifiedValue == 0x55U )
  {
     vLCDBrigthInit();
  }
  temp_counter++;
  //Блок отрисовки экранов
  if ( temp_counter == 2U )
  {
    vDrawObject( pCurrMenu->pHomeMenu[pCurrMenu->pCurrIndex].pScreenCurObjets );
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
        switch ( TempEvent.KeyCode )
        {
          case stop_key:
            key = KEY_STOP;
            break;
          case start_key:
            key = KEY_AUTO;
            break;
          case auto_key:
            key = KEY_START;
            break;
          case time_out:
            key = KEY_EXIT;
            break;
          default:
            break;
        }
      }
      if ( TempEvent.Status == BRAKECODE )
      {
        switch ( TempEvent.KeyCode )
        {
          case up_key:
            if ( EXIT_KEY_F == 0U )
            {
              key = KEY_UP;
            }
            else
            {
              EXIT_KEY_F = 0U;
            }
            break;
          case down_key:
            if ( EXIT_KEY_F == 0U )
            {
              key = KEY_DOWN;
            }
            else
            {
              EXIT_KEY_F = 0U;
            }
            break;
          case stop_key:
            key = KEY_STOP_BREAK;
            break;
          default:
            break;
        }
      }
    }
    if ( key > 0U )
    {
      pCurrMenu->pFunc( pCurrMenu, key );
      if ( TempEvent.KeyCode == time_out )
      {
        //pCurrMenu = &xMainMenu;
    	  //pCurrMenu->pCurrIndex = 0U;
      }
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vDrawObject( xScreenObjet * pScreenObjects)
{
  char* TEXT      = NULL;
  uint8_t  Insert    = 0U;
  uint8_t  Text[16U] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
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
        case LINE:
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
      if ( pScreenObjects[i].last > 0U )
      {
    	  break;
      }
    }
  }
  return;
}


void vUCTOSTRING ( uint8_t * str, uint8_t data )
{
  uint8_t fb = 0U;
  uint8_t i  = 0U;
  uint8_t DD = 100U;
  for ( uint8_t k=0U; k<3U; k++ )
  {
    if ( fb > 0U )
    {
      str[i++] = data / ( DD ) + '0';
    }
    else
    {
      if ( ( data / DD ) > 0U )
	    {
	      str[i++] = data / ( DD ) + '0';
	      fb       = 1U;
	    }
      data = data % ( DD );
      DD   = DD / 10U;
    }
  }
  if ( i == 0U )
  {
    str[i++] = '0';
  }
  str[i] = 0U;
  return;
}

void vITOSTRING ( uint8_t * str, uint16_t data )
{
  uint8_t  fb = 0U;
  uint8_t  i  = 0U;
  uint16_t DD = 10000U;
  for ( uint8_t k=0U; k<5U; k++ )
  {
    if ( fb > 0U )
    {
      str[i++] = data / ( DD ) + '0';
    }
    else
    {
      if ( ( data / DD ) > 0U )
      {
        str[i++] = data / ( DD ) + '0';
        fb       = 1U;
      }
      data = data % ( DD );
      DD   = DD / 10U;
    }
  }
  str[i] = 0U;
  return;
}
/*
 * Функция преобразования безнакового в строку
 */
void vUToStr( uint8_t* str, uint16_t data, signed char scale )
{
  uint8_t     fb     = 0U;
  uint8_t     i      = 0U;
  uint16_t    DD     = 10000U;
  signed char offset = 0U;
  uint8_t     point  = 0U;
  offset = scale;
  if ( offset & 0x80 )
  {
    offset = 1U;
    point  = 1U;
  }
  for ( uint8_t k=0U; k<(5U + offset); k++ )
  {
    if ( ( point == 0U ) && ( k >= 5U ) )  //Если scael был больше 0, то нужно домножить число, фактический добавить в вывод 0
    {
      str[i++] = '0';
    }
    else
    {
      if ( ( point == 1U ) && ( k == ( 6U + scale ) ) )
      {
        str[i++] = '.';
      }
      else
      {
        if ( ( point ==1U ) && ( k == ( 5 + scale ) ) && ( fb == 0U ) )
        {
          fb = 1U;
        }
        if ( ( fb == 1U ) || ( k == ( 5U + scale - 1U ) ) )
        {
          str[i++] = data / ( DD ) + '0';
        }
        else if ( ( data / DD ) > 0U )
        {
          str[i++] = data / ( DD ) + '0';
          fb       = 1U;
        }
        data = data % ( DD );
        DD   = DD / 10U;
      }
    }
  }
  str[i] = 0U;
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
      break;
    default:
      break;
  }
  return;
}

void vGetSettingsData ( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  eConfigAttributes xAtrib                  = { 0U };
  uint16_t          buff                    = 0U;
  int8_t            scale                   = 0U;
  uint16_t          units[MAX_UNITS_LENGTH] = { 0U };
  uint16_t          sbuff                   = 0U;
  Data[0U] = 0U;
  switch ( cmd )
  {
    case mREAD:
      eDATAAPIconfigAtrib( DATA_API_CMD_READ, uiSetting, &xAtrib );
      if ( xAtrib.bitMapSize == 0U )
      {
        if ( xAtrib.len == 1U )
        {
          switch ( xAtrib.type )
          {
            case 'U':
              eDATAAPIconfig( DATA_API_CMD_READ, uiSetting, &buff, &scale, units );
              vUToStr ( ( uint8_t* )Data, buff, scale );
              break;
            case 'S':
              eDATAAPIconfigValue( DATA_API_CMD_READ, uiSetting, &sbuff );
              //vITOSTRING( ( uint8_t* )Data, buff );
              break;
            case 'C':
              break;
            default:
              break;
          }
        }
      }
      break;
    default:
      break;
  }
  return;
}

void vGetSettingsNumber( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  if (cmd == mREAD)
  {
    vUCTOSTRING( ( uint8_t* )Data, (uint8_t) uiSetting);
  }
  return;
}

/*---------------------------------------------------------------------------------------------------*/
void vGetStatusData ( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  uint16_t buff;
  switch ( ID )
  {
    case DISPLAY_BRIGHTNES_LEVEL_ADR:
      switch ( cmd )
      {
        case mREAD:

	        eDATAAPIconfigValue( DATA_API_CMD_READ, displayBrightnesLevel.atrib->adr, &buff );
          vUCTOSTRING( ( uint8_t* )Data, (uint8_t) buff );
          break;
        case mINC:
          eDATAAPIconfigValue( DATA_API_CMD_INC, displayBrightnesLevel.atrib->adr, NULL );
          vLCDBrigthInit();
          break;
        case mDEC:
          eDATAAPIconfigValue( DATA_API_CMD_DEC, displayBrightnesLevel.atrib->adr, NULL );
          vLCDBrigthInit();
          break;
        case mSAVE:
          eDATAAPIconfigValue( DATA_API_CMD_SAVE, displayBrightnesLevel.atrib->adr, NULL );
          break;
        case mESC:
          eDATAAPIconfigValue( DATA_API_CMD_LOAD, displayBrightnesLevel.atrib->adr, NULL );
          vLCDBrigthInit();
  	       break;
        default:
          break;
      }
      break;
    default:
      break;
  }
  return;
}

void vMenuGetData( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  switch ( IP_ADRESS )
  {
    case 1:

      cSERVERgetStrIP( Data );
      break;
    default:
      break;

  }


}

void vGetDataForMenu( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  fix16_t temp;
  uint16_t utempdata;
  eConfigReg xtempCONF;
  uint16_t tt[10];
  uint16_t adr=0;
  char t[20];
  switch (ID)
  {
    case HW_VER:
    case SW_VER:
    case SERIAL:
      if (ID==SERIAL)
        adr =SERIAL_NUMBER_ADR;
      if (ID ==SW_VER)
        adr = VERSION_FIRMWARE_ADR;
      if (ID ==HW_VER)
       adr = VERSION_CONTROLLER_ADR;
      eDATAAPIconfigAtrib(DATA_API_CMD_READ,adr,&xtempCONF);
      eDATAAPIconfigValue(DATA_API_CMD_READ,adr,&tt);
      sprintf(Data,"%h",tt[0]);
      for (uint8_t i=1;i<xtempCONF.atrib->len;i++)
      {
        sprintf(t,"%h",tt[i]);
        strcat(Data,t);
      }
      break;
    case FUEL_LEVEL:
      eCHARTfunc(&fuelSensorChart,  xADCGetSFL() ,   &temp);
      fix16_to_str( temp, Data, 0U );
     break;
    case OIL_PRESSURE:
      eCHARTfunc(&oilSensorChart,  xADCGetSOP() ,   &temp);
      fix16_to_str( temp, Data, 2U );
      break;
    case  COOL_TEMP:
      eCHARTfunc(&coolantSensorChart, xADCGetSCT() ,   &temp);
      fix16_to_str( temp, Data, 0U );
      break;
    case  IN_VDD:
      fix16_to_str( xADCGetVDD(), Data, 2U );
      break;
    case GEN_F1_VDD:
       fix16_to_str( xADCGetGENL1(), Data, 0U );
       break;
    case GEN_F2_VDD:
       fix16_to_str(xADCGetGENL2(), Data, 0U );
       break;
    case GEN_F3_VDD:
       fix16_to_str( xADCGetGENL3(), Data, 0U );
       break;
    case GEN_FREQ:
       fix16_to_str( xADCGetGENLFreq(), Data, 1U );
       break;
    case GEN_F1_CUR:
       fix16_to_str(xADCGetGENL1Cur(), Data, 3U );
       break;
    case GEN_F2_CUR:
      fix16_to_str( xADCGetGENL2Cur(), Data, 3U );
      break;
    case GEN_F3_CUR:
      fix16_to_str( xADCGetGENL3Cur(), Data, 3U );
      break;

    case NET_F1_VDD:
      fix16_to_str( xADCGetNETL1(), Data, 0U );
      break;
    case NET_F2_VDD:
      fix16_to_str(xADCGetNETL2(), Data, 0U );
      break;
   case NET_F3_VDD:
      fix16_to_str( xADCGetNETL3(), Data, 0U );
      break;
   case NET_F1_L_VDD:
      fix16_to_str(xADCGetNETL1Lin(), Data, 0U );
      break;
    case NET_F2_L_VDD:
     fix16_to_str( xADCGetNETL2Lin(), Data, 0U );
     break;
    case NET_F3_L_VDD:
      fix16_to_str( xADCGetNETL3Lin(), Data, 0U );
      break;
    case GEN_F1_L_VDD:
      fix16_to_str( xADCGetGENL1Lin(), Data, 0U );
      break;
    case GEN_F2_L_VDD:
      fix16_to_str( xADCGetGENL2Lin(), Data, 0U );
      break;
   case GEN_F3_L_VDD:
      fix16_to_str( xADCGetGENL3Lin(), Data, 0U );
      break;
    case NET_FREQ:
     fix16_to_str( xADCGetNETLFreq(), Data, 1U );
     break;
    case NET_ROTATION:
             switch (uADCGetNetFaseRotation())
             {
               case B_C_ROTATION:
                 sprintf(Data,"L1-L2-L3");
                 break;
               case C_B_ROTATION:
                 sprintf(Data,"L1-L3-L2");
                 break;
               default:
                 sprintf(Data,"XX-XX-XX");
                 break;
             }
             break;
    case GEN_ROTATION:
         switch (uADCGetGenFaseRotation())
         {
            case B_C_ROTATION:
             sprintf(Data,"L1-L2-L3");
             break;
           case C_B_ROTATION:
             sprintf(Data,"L1-L3-L2");
             break;
           default:
            sprintf(Data,"XX-XX-XX");
            break;
         }
         break;
         case ENGINE_SPEED:
           fix16_to_str( fVRgetSpeed(), Data, 0U );
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
         case GEN_POWER:
           fix16_to_str(fix16_div(xADCGetPower(),fix16_from_int(1000)), Data, 2 );
           break;

    default:
     break;


  }


}


void vGetTestData( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  switch ( ID )
  {

    case 15:
      Data[0]='1';
      Data[1]='2';
      Data[2]='3';
      Data[3]=',';
      Data[4]='3';
      Data[5]='0';
      Data[6]=0;
      break;
    case 13:
      Data[0]='1';
      Data[1]='0';
      Data[2]='0';
      Data[3]=0;
      break;
    case 12:
      Data[0]='1';
      Data[1]='4';
      Data[2]=',';
      Data[3]='5';
      Data[4]=0;
      break;
    case 11:
      Data[0]='1';
      Data[1]='3';
      Data[2]=',';
      Data[3]='6';
      Data[4]=0;
      break;
    case 7:
      Data[0]='2';
      Data[1]='3';
      Data[2]='0';
      Data[3]=0;
      break;
    case 5:
      Data[0]='5';
      Data[1]='0';
      Data[2]=0;
      break;
    case 4:
      Data[0]='4';
      Data[1]='0';
      Data[2]='0';
      Data[3]=0;
      break;
    case 3:
      Data[0]='1';
      Data[1]='5';
      Data[2]='0';
      Data[3]='0';
      Data[4]=0;
      break;
    case 2:
      Data[0]='1';
      Data[1]='0';
      Data[2]='0';
      Data[3]=0;
      break;
    case 1:
      Data[0]='0';
      Data[1]=',';
      Data[2]='9';
      Data[3]=0;
      break;
    default:
      break;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void GetTime( char* Data )
{
  Data[0]='1';
  Data[1]='1';
  Data[2]=':';
  Data[3]='2';
  Data[4]='0';
  Data[5]=':';
  Data[6]='5';
  Data[7]='4';
  Data[8]=0;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
