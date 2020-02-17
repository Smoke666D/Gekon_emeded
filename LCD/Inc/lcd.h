/*
 * lcd.h
 *
 *  Created on: Feb 5, 2020
 *      Author: igor.dymov
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_

#include "stm32f2xx_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

//LCD Size
#define LCD_WIDTH 128
#define LCD_HEIGHT 64

//LCD Direction
#define LCD_DIR 0 //0:Horizontal 1:Vectrial

//Handler Definition
//#define LCD_FMC_HANDLER hsram1 :unused
#define LCD_DMA2D_HANDLER hdma2d

//Pin Definition
#define LCD_RST_PORT GPIOG
#define LCD_BL_PORT GPIOG
#define LCD_RST GPIO_PIN_3
#define LCD_BL GPIO_PIN_4


//Маски команд ST7920
#define CLEAR_CMD    0x01
#define HOME_CMD     0x02
#define MODE_CMD     0x04
		#define RIGTH_SHIFT 0x03
		#define LEFT_SHIFT  0x01
#define ONOFF_CMD    0x08
		#define DISPALY_ON 0x04
		#define CURSOR_ON  0x02
		#define BLINK_ON 0x01
#define CURSOR_CMD   0x10
#define FUNCTION_CMD 0x20
		#define BIT8BUS  0x10
		#define RE       0x04
#define SETCGR_ADR_CMD 0x40
#define SETDDM_ADR_CMD 0x80



//Function declaration
void LCD_Reset(void);
void vLCD_Init(void);

void IncData();
void LCD_WriteData(uint8_t data);
void LCD_WriteCommand(uint8_t data);
void LCD_SendData(uint8_t arg_int, uint8_t *arg_prt);
void LCD_Send16Data( uint8_t *arg_prt);
void LCD_Clear();
void LCD_Redraw();
void LCD_Delay();
void StartDelay();
void StartLcdRedrawTask(void *argument);
void vLCDInit(SemaphoreHandle_t temp);

#endif /* INC_LCD_H_ */
