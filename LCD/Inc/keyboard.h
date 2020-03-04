/*
 * keyboard.h
 *
 *  Created on: 25 февр. 2020 г.
 *      Author: igor.dymov
 */

#ifndef INC_KEYBOARD_H_
#define INC_KEYBOARD_H_

#include "main.h"
#include "stm32f2xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "event_groups.h"


//Константа определяющая количесвто клавиш, которые обрабатываем драйвер
#define KEYBOARD_COUNT  5

#define KEY_READY	  0x0001

#define KEY_ON_MESSAGE 0x0001
#define KEY_OFF_MESSAGE 0x0002

#define KEY_ON_STATE  GPIO_PIN_RESET
#define KEY_OFF_STATE GPIO_PIN_SET

#define KEY_OFF        0x00
#define KEY_ON    	   0X01
#define KEY_ON_REPEAT  0x02

#define KEY_PEREOD         25
#define SWITCHONDELAY      100
#define DefaultDelay       600
#define DefaultRepeatRate  400
#define BRAKECODE 2
#define MAKECODE 1
#define LINE  4
#define ROW   5

#define up_key  1
#define down_key 2
#define enter_key 3
#define exit_key 4
#define left_key 5
#define rigth_key 6


typedef struct
{
	GPIO_TypeDef * KeyPort;
	uint16_t KeyPin;
} xKeyPortStruct;

typedef struct
{
  unsigned int KeyCode;
  unsigned char Status;
} KeyEvent;
typedef struct
{
  unsigned int sKeyCode;
  unsigned char cStatus;
} xKeyEvent;

unsigned long GetKeyTimeOut();
void SetKeyTimeOut(unsigned long data);
void vKeyboardTask(void const * argument);
void SetupKeyboard(QueueHandle_t KeyboardQueue);
void vKeyboardInit(  uint32_t Message);



#endif /* INC_KEYBOARD_H_ */
