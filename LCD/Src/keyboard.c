/*
 * keyboard.c
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: igor.dymov
 */
#include "keyboard.h"
#include "main.h"

static xKeyPortStruct xKeyPortMass[KEYBOARD_COUNT]={
		{SW0_GPIO_Port,SW0_Pin},
		{SW1_GPIO_Port,SW1_Pin},
		{SW2_GPIO_Port,SW2_Pin},
		{SW3_GPIO_Port,SW3_Pin},
		{SW4_GPIO_Port,SW4_Pin},
};


static unsigned char STATUS[KEYBOARD_COUNT]={0,0,0,0,0};
static unsigned int COUNTERS[KEYBOARD_COUNT]={0,0,0,0,0};
static unsigned char CODES[KEYBOARD_COUNT]={up_key,down_key,left_key,enter_key,rigth_key};

static  unsigned long KeyNorPressTimeOut=0;
static unsigned long KEY_TIME_OUT  = 60000;
static char cKeyDelay =0;


static StaticQueue_t xKeyboardQueue;

uint8_t KeyboardBuffer[ 16 * sizeof( KeyEvent ) ];

static QueueHandle_t pKeyboardQueue;

static EventGroupHandle_t pxKeyStatusFLag;

void SetupKeyboard()
{

  pxKeyStatusFLag = xEventGroupCreate();
  pKeyboardQueue =xQueueCreateStatic(16,sizeof(KeyEvent),KeyboardBuffer,&xKeyboardQueue);
  vKeyboardInit(KEY_ON_MESSAGE);
}

QueueHandle_t GetKeyboardQueue()
{
	return pKeyboardQueue;

}

void vKeyboardInit(  uint32_t Message)
{
  switch (Message)
  {
    case KEY_ON_MESSAGE:
        cKeyDelay =0;
        xQueueReset(pKeyboardQueue);
        xEventGroupSetBits(pxKeyStatusFLag,KEY_READY);
        break;
    case KEY_OFF_MESSAGE:
    default:
        xEventGroupClearBits(pxKeyStatusFLag,KEY_READY);
        xQueueReset(pKeyboardQueue);
        break;
  }
}

//Задача обработки клавиш
void vKeyboardTask(void const * argument)
{
  KeyEvent TEvent;
  GPIO_PinState TK[KEYBOARD_COUNT];

 for(;;)
 {
  vTaskDelay(KEY_PEREOD/ portTICK_RATE_MS );
  //Задача ждет флага готовности KEY_READY,
  xEventGroupWaitBits(pxKeyStatusFLag,KEY_READY,pdFALSE,pdTRUE,portMAX_DELAY);

  //Считываем текущее состояние портов клавиатуры
  for (uint8_t k=0;k<KEYBOARD_COUNT;k++)
  {
	TK[k]=  HAL_GPIO_ReadPin(xKeyPortMass[k].KeyPort,xKeyPortMass[k].KeyPin);
  }
  //Анализируем клавиутру
  for (uint8_t i=0;i<KEYBOARD_COUNT;i++)
  {
	//Если текущие состояние порта ВЫКЛ, а предидущие состояние было ВКЛ,
	//Фиксируем отжатие клавищи (BRAKECODE)
    if (STATUS[i] && (TK[i]==KEY_OFF_STATE))
    {
      STATUS[i] =KEY_OFF; //Состоянии клавиши ВЫКЛ
      COUNTERS[i]=0;      //Сбрасываем счетчик
      TEvent.KeyCode =CODES[i];
      TEvent.Status = BRAKECODE;
      xQueueReset(pKeyboardQueue);
      xQueueSend(pKeyboardQueue, &TEvent, portMAX_DELAY );
      KeyNorPressTimeOut =0;
    }
    else
    //Если текущие состояние потрта ВКЛ, а предидущие было ВЫКЛ
    //то запускаме счеткик нажатий
    if ( !STATUS[i]  && (TK[i]==KEY_ON_STATE) )
    {
      COUNTERS[i]++;
      //если счетчик превысил значение SWITCHONDELAY то фиксируем нажатие
      if (COUNTERS[i]>=SWITCHONDELAY/KEY_PEREOD)
      {
        COUNTERS[i]=0;
        STATUS[i] = KEY_ON;
        TEvent.KeyCode =CODES[i];
        TEvent.Status = MAKECODE;
        xQueueSend(pKeyboardQueue, &TEvent, portMAX_DELAY );
        KeyNorPressTimeOut =0;
     }
   }
   else
   if ( STATUS[i] &&  (TK[i]==KEY_ON_STATE))
    {

      COUNTERS[i]++;
      switch  ( STATUS[i] )
      {
        case KEY_ON:
          if (COUNTERS[i]>=(DefaultDelay/KEY_PEREOD ))
          {
             STATUS[i]=KEY_ON_REPEAT;   //?????? ????? ???????
             COUNTERS[i]=0; //?????????? ???????
             TEvent.KeyCode =CODES[i];
             TEvent.Status = MAKECODE;
             xQueueSend(pKeyboardQueue, &TEvent, portMAX_DELAY );             //?????????? MAKE CODR
             KeyNorPressTimeOut =0;
          }
          break;
       case KEY_ON_REPEAT:

          if (COUNTERS[i]>=(DefaultRepeatRate/KEY_PEREOD ))
          {
             COUNTERS[i]=0;
             TEvent.KeyCode =CODES[i];
             TEvent.Status = MAKECODE;
             xQueueSend(pKeyboardQueue, &TEvent, portMAX_DELAY );
             KeyNorPressTimeOut =0;
          }
        break;
       default:
    	break;
      }
    }
  }
  KeyNorPressTimeOut++;
  if (KeyNorPressTimeOut>=(KEY_TIME_OUT/(KEY_PEREOD / portTICK_RATE_MS)))
  {
    KeyNorPressTimeOut=0;
   //	 vMenuStop(cKeyDelay);
    cKeyDelay++;
  }
}



}


unsigned long GetKeyTimeOut()
{
  return KEY_TIME_OUT;
}
void SetKeyTimeOut(unsigned long data)
{
  KEY_TIME_OUT =data;
}




