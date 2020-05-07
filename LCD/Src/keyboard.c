/*
 * keyboard.c
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: igor.dymov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "keyboard.h"
#include "main.h"
/*----------------------- Structures ----------------------------------------------------------------*/
static xKeyPortStruct xKeyPortMass[KEYBOARD_COUNT]={
		{SW0_GPIO_Port,SW0_Pin},
		{SW1_GPIO_Port,SW1_Pin},
		{SW2_GPIO_Port,SW2_Pin},
		{SW3_GPIO_Port,SW3_Pin},
		{SW4_GPIO_Port,SW4_Pin},
};
static StaticQueue_t      xKeyboardQueue;
static QueueHandle_t      pKeyboardQueue;
static EventGroupHandle_t pxKeyStatusFLag;
/*----------------------- Variables -----------------------------------------------------------------*/
static unsigned char STATUS[KEYBOARD_COUNT]   = { 0U, 0U, 0U, 0U, 0U };
static unsigned int  COUNTERS[KEYBOARD_COUNT] = { 0U, 0U, 0U, 0U, 0U };
static unsigned char CODES[KEYBOARD_COUNT]    = { up_key, down_key, stop_key, auto_key, start_key };
static unsigned long KeyNorPressTimeOut       = 0U;
static unsigned long KEY_TIME_OUT             = 6000U;
static char          cKeyDelay                = 0U;
uint8_t KeyboardBuffer[ 16 * sizeof( KeyEvent ) ];
/*---------------------------------------------------------------------------------------------------*/
void SetupKeyboard( void )
{
  pxKeyStatusFLag = xEventGroupCreate();
  pKeyboardQueue  = xQueueCreateStatic( 16U, sizeof( KeyEvent ), KeyboardBuffer, &xKeyboardQueue );
  vKeyboardInit( KEY_ON_MESSAGE );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
QueueHandle_t GetKeyboardQueue( void )
{
  return pKeyboardQueue;
}

void vKeyboardInit(  uint32_t message )
{
  switch ( message )
  {
    case KEY_ON_MESSAGE:
      cKeyDelay =0;
      xQueueReset(pKeyboardQueue);
      xEventGroupSetBits(pxKeyStatusFLag,KEY_READY);
      break;
    case KEY_OFF_MESSAGE:
      break;
    default:
      xEventGroupClearBits(pxKeyStatusFLag,KEY_READY);
      xQueueReset(pKeyboardQueue);
      break;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Задача обработки клавиш
 * */
void vKeyboardTask( void const * argument )
{
  KeyEvent      TEvent;
  GPIO_PinState TK[KEYBOARD_COUNT];
  uint8_t       k = 0U;
  uint8_t       i = 0U;

  for(;;)
  {
    vTaskDelay(KEY_PEREOD/ portTICK_RATE_MS );
    xEventGroupWaitBits(pxKeyStatusFLag,KEY_READY,pdFALSE,pdTRUE,portMAX_DELAY); /* Задача ждет флага готовности KEY_READY, */
    for ( k=0U; k<KEYBOARD_COUNT; k++ )                                          /* Считываем текущее состояние портов клавиатуры */
    {
	  TK[k]=  HAL_GPIO_ReadPin( xKeyPortMass[k].KeyPort, xKeyPortMass[k].KeyPin );
    }
    for ( i=0U; i<KEYBOARD_COUNT; i++ )                                          /* Анализируем клавиутру */
    {
      //Если текущие состояние порта ВЫКЛ, а предидущие состояние было ВКЛ,
	  //Фиксируем отжатие клавищи (BRAKECODE)
      if ( STATUS[i] && ( TK[i] == KEY_OFF_STATE ) )
      {
        STATUS[i]      = KEY_OFF; //Состоянии клавиши ВЫКЛ
        COUNTERS[i]    = 0U;      //Сбрасываем счетчик
        TEvent.KeyCode = CODES[i];
        TEvent.Status  = BRAKECODE;
        xQueueReset( pKeyboardQueue );
        xQueueSend( pKeyboardQueue, &TEvent, portMAX_DELAY );
        KeyNorPressTimeOut = 0U;
      }
      else
      {
        //Если текущие состояние потрта ВКЛ, а предидущие было ВЫКЛ
        //то запускаме счеткик нажатий
        if ( !STATUS[i] && ( TK[i] == KEY_ON_STATE ) )
        {
          COUNTERS[i]++;
          //если счетчик превысил значение SWITCHONDELAY то фиксируем нажатие
          if ( COUNTERS[i] >= ( SWITCHONDELAY / KEY_PEREOD ) )
          {
            COUNTERS[i]    = 0U;
            STATUS[i]      = KEY_ON;
            TEvent.KeyCode = CODES[i];
            TEvent.Status  = MAKECODE;
            xQueueSend( pKeyboardQueue, &TEvent, portMAX_DELAY );
            KeyNorPressTimeOut = 0U;
          }
        }
        else if ( STATUS[i] && ( TK[i] == KEY_ON_STATE ) )
        {
          COUNTERS[i]++;
          switch ( STATUS[i] )
          {
            case KEY_ON:
              if ( COUNTERS[i] >= ( DefaultDelay / KEY_PEREOD ) )
              {
                STATUS[i]      = KEY_ON_REPEAT;   //?????? ????? ???????
                COUNTERS[i]    = 0U; //?????????? ???????
                TEvent.KeyCode = CODES[i];
                TEvent.Status  = MAKECODE;
                xQueueSend( pKeyboardQueue, &TEvent, portMAX_DELAY );             //?????????? MAKE CODR
                KeyNorPressTimeOut = 0U;
              }
              break;
            case KEY_ON_REPEAT:
              if ( COUNTERS[i] >= ( DefaultRepeatRate / KEY_PEREOD ) )
              {
                COUNTERS[i]    = 0U;
                TEvent.KeyCode = CODES[i];
                TEvent.Status  = MAKECODE;
                xQueueSend( pKeyboardQueue, &TEvent, portMAX_DELAY );
                KeyNorPressTimeOut = 0U;
              }
              break;
            default:
    	      break;
          }
        }
      }
    }
    KeyNorPressTimeOut++;
    if ( KeyNorPressTimeOut >= ( KEY_TIME_OUT / ( KEY_PEREOD / portTICK_RATE_MS ) ) )
    {
      KeyNorPressTimeOut = 0U;
      TEvent.KeyCode     = time_out;
      TEvent.Status      = MAKECODE;
      xQueueSend( pKeyboardQueue, &TEvent, portMAX_DELAY );
      //	 vMenuStop(cKeyDelay);
      cKeyDelay++;
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
unsigned long GetKeyTimeOut( void )
{
  return KEY_TIME_OUT;
}
/*---------------------------------------------------------------------------------------------------*/
void SetKeyTimeOut( unsigned long data )
{
  KEY_TIME_OUT = data;
}




