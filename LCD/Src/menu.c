/*
 * menu.c
 *
 *  Created on: 21 февр. 2020 г.
 *      Author: igor.dymov
 */

#include "menu.h"
#include "main.h"
#include "lcd.h"
#include "u8g2.h"
#include "keyboard.h"
#include "data_type.h"
#include "menu_data.c"

#define XRESULURION       128U
#define YRESOLUTION       64U
#define FONT_TYPE         ( u8g2_font_6x13_t_cyrillic )
#define FONT_X_SIZE       6U
#define FONT_TYPE_NUMBER  ( u8g2_font_5x8_mf )
#define FONT_TYPE4        ( u8g2_font_6x13_t_cyrillic )
#define FONT_X_SIZE4      6U
#define FONT_TYPE_NUMBER4 ( u8g2_font_5x8_mf )
#define MAX_KEY_PRESS     10U

u8g2_t*  u8g2;
KeyEvent TempEvent;
KeyEvent BufferEvent = { 0U, 0U };
uint8_t  temp_counter = 0U;
static xScreenSetObject* pCurrMenu      = NULL;
static xScreenObjet*     pCurObject     = NULL;
static uint8_t           CurObjectIndex = 0U;
static uint8_t           DownScreen     = 0U;
static QueueHandle_t     pKeyboard;
static uint8_t           key            = 0U;
static xScreenObjet*     pCurDrawScreen = NULL;
static uint8_t           Blink          = 0U;

void xInputScreenKeyCallBack(xScreenSetObject* menu, char key)
{
  xScreenObjet* pObjects          = menu->pHomeMenu[menu->pCurrIndex].pScreenCurObjets;
  uint8_t       ActiveObjectCount = 0U;
  uint8_t       i                 = 0U;
  switch (key)
  {
    case KEY_STOP:
      pCurObject->GetDtaFunction(INC, NULL,pCurObject->DataID);
      break;
    case KEY_START:
      pCurObject->GetDtaFunction(DEC, NULL,pCurObject->DataID);
      break;
    case KEY_UP:
      if (menu->pHomeMenu[menu->pCurrIndex].pCurrIndex <  menu->pHomeMenu[menu->pCurrIndex].pMaxIndex)
      {
        menu->pHomeMenu[menu->pCurrIndex].pCurrIndex++;
        for (uint8_t i=0;i<MAX_SCREEN_OBJECT;i++) //Проверяем есть ли на экране динамические объекты
        {
          if (pObjects[i].xType == INPUT_HW_DATA)
          {
            ActiveObjectCount++;
            if ( ActiveObjectCount == ( menu->pHomeMenu[menu->pCurrIndex].pCurrIndex - 1U ) )
            {
              pObjects[i].ObjectParamert[3U] = 0U;
            }
            if ( ActiveObjectCount == menu->pHomeMenu[menu->pCurrIndex].pCurrIndex )
            {
              pObjects[i].ObjectParamert[3U] = 1U;
              pCurObject = &pObjects[i];
              break;
            }
          }
        }
      }
      break;
    case KEY_DOWN:
      if ( menu->pHomeMenu[menu->pCurrIndex].pCurrIndex > 1U )
      {
        menu->pHomeMenu[menu->pCurrIndex].pCurrIndex--;
        for ( i=0U; i<MAX_SCREEN_OBJECT; i++ ) //Проверяем есть ли на экране динамические объекты
        {
          if (pObjects[i].xType == INPUT_HW_DATA)
          {
            ActiveObjectCount++;
            if ( ActiveObjectCount == ( menu->pHomeMenu[menu->pCurrIndex].pCurrIndex + 1U ) )
            {
              pObjects[i].ObjectParamert[3U] = 0U;
              break;
            }
            if ( ActiveObjectCount == menu->pHomeMenu[menu->pCurrIndex].pCurrIndex )
            {
              pObjects[i].ObjectParamert[3U] = 1U;
              pCurObject = &pObjects[i];
            }
          }
        }
      }
      break;
    case KEY_AUTO:
      //Если на экране есть объект с редактируемым полем, то оправлем команду на запись текущего значения
      pCurObject->GetDtaFunction( SAVE, NULL, pCurObject->DataID );
      //Если на экране только один объект редактирования или его вообще нет, то выполняются дейтсвия по выходу из экрана
      if ( menu->pHomeMenu[menu->pCurrIndex].pMaxIndex > 1U ) break;
    case KEY_EXIT:
      //Выходи из активного экрана, для этого меняем его статус и также освобождаем редактируемый объект
      menu->pHomeMenu[menu->pCurrIndex].xScreenStatus=NOT_ACTIVE;
      pCurObject->ObjectParamert[3]=0;
      if ( menu->pHomeMenu[menu->pCurrIndex].pMaxIndex) pCurObject->GetDtaFunction( ESC, NULL);
      break;
    default:
      break;
  }
  return;
}

void xLineScreenKeyCallBack( xScreenSetObject* menu, char key )
{
  uint8_t index = menu->pCurrIndex;
  uint8_t i     = 0U;

  if ( menu->pHomeMenu[index].xScreenStatus != ACTIVE )
  {
    switch ( key )
    {
      case KEY_UP:
        if ( menu->pCurrIndex == menu->pMaxIndex )
          menu->pCurrIndex=0;
        else
          menu->pCurrIndex++;
        break;
      case KEY_DOWN:
        if ( menu->pCurrIndex == 0U )
          menu->pCurrIndex = menu->pMaxIndex;
        else
          menu->pCurrIndex--;
        break;
      case KEY_AUTO:
          //Если текущий Screen являтется ScreenSet то переводи глобальный указатель на меню нижнего уровня
            if ( menu->pHomeMenu[index].pDownScreenSet != NULL )
              pCurrMenu = menu->pHomeMenu[index].pDownScreenSet;
            else
            {
              //Есди текущий Screen конечный, то делаем его активным, это позволит перенапрявлять назажите клавиш в обработчик экрана
              if ( menu->pHomeMenu[index].xScreenStatus == NOT_ACTIVE )
              {
                menu->pHomeMenu[index].xScreenStatus = ACTIVE;
                menu->pHomeMenu[index].pCurrIndex    = 0U;
                //Для ускорения работы выясняем кол-во редактируемых объектов на экране, если кол-во не задана при инициализации
                if ( menu->pHomeMenu[index].pMaxIndex == 0U )
                for ( i=0U; i<MAX_SCREEN_OBJECT; i++ )
                {
                    switch ( menu->pHomeMenu[index].pScreenCurObjets[i].xType )
                  {
                    case INPUT_HW_DATA:
                      menu->pHomeMenu[index].pMaxIndex++;
                       break;
                    default:
                        break;
                   }
                    if  ( menu->pHomeMenu[index].pScreenCurObjets[i].last ) break;
                }
                if ( menu->pHomeMenu[index].pMaxIndex > 1U)
                {
                  for ( uint8_t i=0U; i<MAX_SCREEN_OBJECT; i++ )
                  {

                    if  ( menu->pHomeMenu[index].pScreenCurObjets[i].xType == INPUT_HW_DATA )
                    {
                      menu->pHomeMenu[index].pScreenCurObjets[i].ObjectParamert[3U] = 1U;
                      CurObjectIndex = i;
                      pCurObject = &menu->pHomeMenu[index].pScreenCurObjets[i];
                      break;
                    }
                    if ( menu->pHomeMenu[index].pScreenCurObjets[i].last ) break;
                  }
                }
              }
            }
            break;
      case KEY_EXIT:
          //Если текущий Screen являтется ScreenSet то переводи глобальный указатель на меню нижнего уровня
           if ( menu->pHomeMenu[index].pUpScreenSet != NULL )
               pCurrMenu = menu->pHomeMenu[index].pUpScreenSet;
           else
           //Есди текущий Screen конечный, то делаем его активным
             if ( menu->pHomeMenu[index].xScreenStatus == ACTIVE )
             {
               menu->pHomeMenu[index].xScreenStatus = NOT_ACTIVE;
             }
          break;
      default:
          break;
    }
  }
  else
  {
    if ( menu->pHomeMenu[index].pFunc != NULL )
      menu->pHomeMenu[index].pFunc( menu, key );
  }
  return;
}

void xInfoScreenCallBack( xScreenSetObject* menu, char key )
{
  uint8_t           index = menu->pCurrIndex;
  xScreenSetObject* pMenu = menu;

  switch ( key )
  {
    case KEY_UP:
      if ( DownScreen )
      {
        DownScreen = 0U;
        if ( menu->pHomeMenu[index].pUpScreenSet != NULL )
        {
          pCurrMenu = menu->pHomeMenu[index].pUpScreenSet;
          pMenu     = pCurrMenu;
        }
      }
      if ( pMenu->pCurrIndex == pMenu->pMaxIndex )
        pMenu->pCurrIndex = 0U;
      else
        pMenu->pCurrIndex++;
      break;
    case KEY_DOWN:
      if ( DownScreen == 0U )
      {
        if ( menu->pHomeMenu[index].pDownScreenSet != NULL )
        {
          pCurrMenu  = menu->pHomeMenu[index].pDownScreenSet;
          DownScreen = 1U;
          pCurrMenu->pCurrIndex = 0U;
        }
      }
      else
      {
        if ( menu->pCurrIndex == menu->pMaxIndex )
          menu->pCurrIndex = 0U;
        else
          menu->pCurrIndex++;
      }
      break;
    default:
      break;
  }
  return;
}

void InitMenu( void )
{
  return;
}

void vMenuInit( u8g2_t* temp )
{
  u8g2      = temp;
  pCurrMenu = &xMainMenu;
  pKeyboard = GetKeyboardQueue();
  return;
}

void vMenuTask( void )
{
    //Блок обработки нажатий на клавиши
  osDelay( 200U );
  temp_counter++;
  //Блок отрисовки экранов
  if ( temp_counter == 2U )
  {
    DrawObject( pCurrMenu->pHomeMenu[pCurrMenu->pCurrIndex].pScreenCurObjets );
    LCD_Redraw();
    temp_counter = 0U;
  }
  if ( xQueueReceive( pKeyboard, &TempEvent, 0U ) == pdPASS )
  {
    key = 0U;
    if ( TempEvent.KeyCode == time_out )
    {
      pCurrMenu = &xMainMenu;
      pCurrMenu->pCurrIndex = 0U;
    }
    else
    {
      if ( ( TempEvent.Status == MAKECODE ) && ( BufferEvent.Status == MAKECODE ) &&
           ( ( TempEvent.KeyCode | BufferEvent.KeyCode) == 0x03U ) )
      {
        key = KEY_EXIT;
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
            case up_key:
              key = KEY_UP;
              break;
            case down_key:
              key = KEY_DOWN;
              break;
            case start_key:
              key = KEY_START;
              break;
            case auto_key:
              key = KEY_AUTO;
              break;
          }
        }
      }
      if ( key ) pCurrMenu->pFunc( pCurrMenu, key );
    }
  }
  return;
}

void DrawObject( xScreenObjet * pScreenObjects)
{
  uint8_t* TEXT;
  uint8_t  Insert    = 0U;
  uint8_t  Text[16U] = {' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' '};
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
      if  ( pScreenObjects[i].last > 0U )
      {
    	break;
      }
      switch ( pScreenObjects[i].xType )
      {
        case HW_DATA:
          break;
        case INPUT_HW_DATA:
          Redraw = 1U;
          break;
        default:
          break;
      }
      if ( Redraw > 0U )
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
        case INPUT_HW_DATA:
          if ( pScreenObjects[i].ObjectParamert[3U] > 0U )
          {
            Insert = 1U;
            if ( Blink )
            {
              Blink = 0U;
            }
            else
            {
              Blink = 1U;
            }
          }
          break;
        case STRING:
          break;
        case HW_DATA:
          if ( !Insert > 0U )
          {
            u8g2_SetDrawColor( u8g2, pScreenObjects[i].ObjectParamert[1U]?0U:1U );
          }
          else
          {
            u8g2_SetDrawColor( u8g2, Blink?0U:1U );
          }
          u8g2_DrawBox( u8g2, pScreenObjects[i].x, pScreenObjects[i].y, pScreenObjects[i].Width, pScreenObjects[i].Height );
          /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
          /* Если поставить сюда break - не выводяться цифры*/
          /*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
        case TEXT_STRING:
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
              TEXT = pScreenObjects[i].pStringParametr;
              break;
            case HW_DATA:
            	/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
            	/* Если поставить сюда break - не выводяться цифры*/
            	/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
            case INPUT_HW_DATA:
              if ( pScreenObjects[i].DataID > 0U )
              {
                pScreenObjects[i].GetDtaFunction( READ, &Text, pScreenObjects[i].DataID );
              }
              else
              {
                pScreenObjects[i].GetDtaFunction( READ, &Text );
              }
              TEXT = Text;
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
            y_offset =  pScreenObjects[i].y + pScreenObjects[i].Height / 2U + u8g2_GetAscent( u8g2 ) / 2U;
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
      if  ( pScreenObjects[i].last > 0U )
      {
    	break;
      }
    }
  }
  return;
}


//Обявдение объека-карусели экранов верхнего уровня
xScreenSetObject xMainMenu =
{
  xScreensLev1,
  ( MENU_LEVEL1_COUNT - 1U ),
  0U,
  ( void* )&xInfoScreenCallBack,
};

xScreenSetObject xEngineMenu =
{
  xEngineScreens,
  ( ENGINE_MENU_COUNT - 1U ),
  0U,
  ( void* )&xInfoScreenCallBack,
};

xScreenSetObject xGeneratorMenu =
{
  xGeneratorScreens,
  ( GENERATOR_MENU_COUNT - 1U ),
  0U,
  ( void* )&xInfoScreenCallBack,
};

xScreenSetObject xNetMenu =
{
  xNetScreens,
  ( NET_MENU_COUNT - 1U ),
  0U,
  ( void* )&xInfoScreenCallBack,
};

void vGetTestData( DATA_COMMNAD_TYPE cmd, char* Data, uint8_t ID )
{
  switch ( ID )
  {
    case 16U:
      Data[0]='L';
      Data[1]='1';
      Data[2]='-';
      Data[3]='L';
      Data[4]='2';
      Data[5]='-';
      Data[6]='L';
      Data[7]='3';
      Data[8]=0;
      break;
    case 15:
      Data[0]='1';
      Data[1]='2';
      Data[2]='3';
      Data[3]=',';
      Data[4]='3';
      Data[5]='0';
      Data[6]=0;
      break;
    case 14:
      Data[0]='2';
      Data[1]='1';
      Data[2]='5';
      Data[3]=0;
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
    case 10:
      Data[0]='1';
      Data[1]='9';
      Data[2]='2';
      Data[3]='.';
      Data[4]='1';
      Data[5]='6';
      Data[6]='8';
      Data[7]='.';
      Data[8]='1';
      Data[9]='0';
      Data[10]='0';
      Data[11]='.';
      Data[12]='5';
      Data[13]=0;
      break;
    case 9:
      Data[0]='2';
      Data[1]='5';
      Data[2]='5';
      Data[3]='.';
      Data[4]='2';
      Data[5]='5';
      Data[6]='5';
      Data[7]='.';
      Data[8]='2';
      Data[9]='5';
      Data[10]='5';
      Data[11]='.';
      Data[12]='0';
      Data[13]=0;
      break;
    case 8:
      Data[0]='1';
      Data[1]='9';
      Data[2]='2';
      Data[3]='.';
      Data[4]='1';
      Data[5]='6';
      Data[6]='8';
      Data[7]='.';
      Data[8]='1';
      Data[9]='0';
      Data[10]='0';
      Data[11]='.';
      Data[12]='1';
      Data[13]='2';
      Data[14]='1';
      Data[15]=0;
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
