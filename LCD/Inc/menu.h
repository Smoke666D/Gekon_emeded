/*
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
/*------------------------ Define --------------------------------------*/
#define SET_PARAMETR_SCREEN 0U
#define CENTER_ALIGN        1U
#define RIGTH_ALIGN         2U
#define LEFT_ALIGN          3U
#define NO_ALIGN            0U
/* Определение виртуальных клавиш, которые могу как повторять клавиши клавиатуры, так и быть их комбинацие */
#define KEY_UP              1U
#define KEY_DOWN            2U
#define KEY_STOP            3U
#define KEY_AUTO            4U
#define KEY_START           5U
#define KEY_EXIT            6U
#define MAX_SCREEN_COUNT    3U
#define MAX_SCREEN_OBJECT   20U
#define XRESULURION         128U
#define YRESOLUTION         64U
#define FONT_TYPE           ( u8g2_font_6x13_t_cyrillic )
#define FONT_X_SIZE         6U
#define FONT_TYPE_NUMBER    ( u8g2_font_5x8_mf )
#define FONT_TYPE4          ( u8g2_font_6x13_t_cyrillic )
#define FONT_X_SIZE4        6U
#define FONT_TYPE_NUMBER4   ( u8g2_font_5x8_mf )
#define MAX_KEY_PRESS       10U
/*------------------------------ Enum ----------------------------------------*/
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
/*---------------------------- Structures --------------------------------------*/
typedef struct
{
  uint8_t     last;
  uint8_t     x;
  uint8_t     y;
  uint8_t     Width;
  uint8_t     Height;
  OBJECT_TYPE xType;
  uint8_t*    ObjectParamert;
  char*       pStringParametr;
  void        (*GetDtaFunction)();
  uint16_t    DataID;
} xScreenObjet;

typedef struct
{
  xScreenObjet* pScreenCurObjets;
  void*         pUpScreenSet;
  void*         pDownScreenSet;
  SCREEN_STATUS xScreenStatus;
  uint8_t       pCurrIndex;
  uint8_t       pMaxIndex;
  void          (*pFunc)(void *,char);
} xScreenType;

typedef struct
{
  xScreenType*  pHomeMenu;               /* Указатель на массив экранов верхнего уровня. */
  uint8_t	    pMaxIndex;	             /* Индекс домашнего экрана в маасиве экранов верхнего уровня */
  uint8_t	    pCurrIndex;	             /* Индексе текущего экрана */
  void          (* pFunc)(void *,char);  /* Функция обработки нажатий */
} xScreenSetObject;
/*----------------------------- Functions ------------------------------------*/
void DrawMenu( uint8_t temp, uint8_t* data );
void vMenuInit( u8g2_t* temp );
void vMenuTask( void );
void DrawObject( xScreenObjet* pScreenObjects );
/*----------------------------------------------------------------------------*/
#endif /* INC_MENU_H_ */
