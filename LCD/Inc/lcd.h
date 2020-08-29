/*
 * lcd.h
 *
 *  Created on: Feb 5, 2020
 *      Author: igor.dymov
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "main.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "u8g2.h"
#include "menu.h"
#include "config.h"
/*------------------------ Define --------------------------------------*/
#define LCD_WIDTH            128U  /* LCD Size */
#define LCD_HEIGHT           64U
#define LCD_DIR              0U    /* LCD Direction 0:Horizontal 1:Vectrial */
#define LCD_DMA2D_HANDLER    ( hdma2d )	/* Handler Definition */
#define LCD_DATA_BUFFER_SIZE 1024U

/*---------------------- ST7920 ----------------------------------------*/
#define CLEAR_CMD      0x01U
#define HOME_CMD       0x02U
#define MODE_CMD       0x04U
#define RIGTH_SHIFT    0x03U
#define LEFT_SHIFT     0x01U
#define ONOFF_CMD      0x08U
#define DISPALY_ON     0x04U
#define CURSOR_ON      0x02U
#define BLINK_ON       0x01U
#define CURSOR_CMD     0x10U
#define FUNCTION_CMD   0x20U
#define BIT8BUS        0x10U
#define RE             0x04U
#define G_ON	       0x02U
#define SETCGR_ADR_CMD 0x40U
#define SETDDM_ADR_CMD 0x80U
/*----------------------------- Functions ------------------------------------*/

void vLCDBrigthInit(void);
void vLCD_Init( void );
void vLCDRedraw( void );
void vLCDDelay( void );
void vST7920init(void);
uint8_t ucLCDGetLedBrigth(void);
void vLCDSetLedBrigth(uint8_t brigth);
void vLCDInit( SemaphoreHandle_t temp );
/*----------------------------------------------------------------------------*/
#endif /* INC_LCD_H_ */
