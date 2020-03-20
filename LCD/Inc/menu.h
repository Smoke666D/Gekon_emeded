/*
 * menu.h
 *
 *  Created on: 21 февр. 2020 г.
 *      Author: igor.dymov
 */

#ifndef INC_MENU_H_
#define INC_MENU_H_



#include "u8g2.h"

void DrawMenu(uint8_t temp, uint8_t * data);
void vMenuInit(u8g2_t * temp);
void vMenuTask();

#define  SET_PARAMETR_SCREEN 0

#define CENTER_ALIGN		 1
#define RIGTH_ALIGN 		 2
#define LEFT_ALIGN        	 3
#define NO_ALIGN     		 0

//Определение виртуальных клавиш, которые могу как повторять клавиши клавиатуры, так и быть их комбинацие
#define KEY_UP    1
#define KEY_DOWN   2
#define KEY_STOP   3
#define KEY_AUTO   4
#define KEY_START   5
#define KEY_EXIT    6




typedef enum
{
	ACTIVE,
	NOT_ACTIVE,
	PASSIVE,

} SCREEN_STATUS;

typedef enum
{
	ICON,
	TEXT_STRING,
	STRING,
	LINE,
	NEGATIVE_STRING,
	DATA_STRING,
	INPUT_DATA_STRING,
	HW_DATA,
	INPUT_HW_DATA,
} OBJECT_TYPE;

typedef struct
{
  uint8_t last;
  uint8_t x;
  uint8_t y;
  uint8_t Width;
  uint8_t Height;
  OBJECT_TYPE xType;
  uint8_t * ObjectParamert;
  uint8_t * pStringParametr;
  void (*GetDtaFunction)();
  uint16_t DataID;
} xScreenObjet;


typedef struct
{
  xScreenObjet * pScreenCurObjets;
  void * pUpScreenSet;
  void * pDownScreenSet;
  SCREEN_STATUS xScreenStatus;
  uint8_t pCurrIndex;
  uint8_t pMaxIndex;
  void (*pFunc)(void *,char);
} xScreenType;




typedef struct
{
  xScreenType * pHomeMenu;    //Указатель на массив экранов верхнего уровня.
  uint8_t	    pMaxIndex;	  //Индекс домашнего экрана в маасиве экранов верхнего уровня
  uint8_t	    pCurrIndex;	  //Индексе текущего экрана
  void (* pFunc)(void *,char);	          //Функция обработки нажатий
} xScreenSetObject;





#define MAX_SCREEN_COUNT 3
#define MAX_SCREEN_OBJECT 20


void DrawObject( xScreenObjet * pScreenObjects);

#endif /* INC_MENU_H_ */
