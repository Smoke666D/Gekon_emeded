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
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
/*------------------------ Define --------------------------------------*/
#define LCD_WIDTH            128U  /* LCD Size */
#define LCD_HEIGHT           64U
#define LCD_DIR              0U    /* LCD Direction 0:Horizontal 1:Vectrial */
//#define LCD_FMC_HANDLER hsram1 :unused
#define LCD_DMA2D_HANDLER    ( hdma2d )	/* Handler Definition */
#define LCD_DATA_BUFFER_SIZE 1024U
/*-------------------- Pin Definition ----------------------------------*/
#define LCD_RST_PORT 	  ( GPIOG )
#define LCD_BL_PORT 	  ( GPIOG )
#define LCD_RST           ( GPIO_PIN_3 )
#define LCD_BL            ( GPIO_PIN_4 )
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
#define SETCGR_ADR_CMD 0x40U
#define SETDDM_ADR_CMD 0x80U
/*----------------------------- Functions ------------------------------------*/
void LCD_Reset( void );
void vLCD_Init( void );
void IncData( void );
void LCD_WriteData( uint8_t data );
void LCD_WriteCommand( uint8_t data );
void LCD_SendData( uint8_t arg_int, uint8_t *arg_prt );
void LCD_Send16Data( uint8_t *arg_prt );
void LCD_Clear( void );
void LCD_Redraw( void );
void LCD_Delay( void );
void StartDelay( void );
void StartLcdRedrawTask( void *argument );
void vLCDInit( SemaphoreHandle_t temp );
/*----------------------------------------------------------------------------*/
#endif /* INC_LCD_H_ */
