/*
 * usbhid.h
 *
 *  Created on: 2 апр. 2020 г.
 *      Author: photo_Mickey
 */

#ifndef INC_USBHID_H_
#define INC_USBHID_H_
/*----------------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "usbd_customhid.h"
#include "usbd_def.h"
#include "config.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
/*------------------------------ Enum ----------------------------------------*/
typedef enum
{
  USB_DISCONNECT,
  USB_CONNECT,
} USB_StatusConnect;

typedef enum
{
  USB_TransactionDone,
  USB_TransactionContin,
} USB_StatusTransaction;

typedef enum
{
  USB_OUTPUT,
  USB_INPUT,
} USB_ReportDir;

typedef enum
{
  USB_OK_STAT      = 0x01U,     /* 200 analog - all OK                            */
  USB_BAD_REQ_STAT = 0x02U,     /* 400 analog - request is distorted              */
  USB_NON_CON_STAT = 0x03U,     /* 404 analog - requesting a nonexistent resource */
} USB_ReportStat;

typedef enum
{
  USB_GET_CMD = 0x01U,
  USB_PUT_CMD = 0x02U,
} USB_ReportCmd;
/*------------------------------ Default -------------------------------------*/
#define USB_REPORT_SIZE       8U
#define USB_QUEUE_SIZE        10U
/*------------------------------ Report --------------------------------------*/
/*
 * | 0  |  1  |  2   |  3   |  4   |  5   |   6   |   7   |
 * | ID | CMD | STAT | FILD | ADR1 | ADR0 | DATA1 | DATA0 |
 */
#define USB_INPUT_REPORT_ID   0x01U    /* First byte in report */
#define USB_OUTPUT_REPORT_ID  0x02U    /* First byte in report */
/*------------------------------ Macros --------------------------------------*/
#define USB_GET_DIR_VAL(e)    ( e == USB_INPUT ) ? USB_INPUT_REPORT_ID : USB_OUTPUT_REPORT_ID
#define USB_GET_DIR(e)        ( e == USB_INPUT_REPORT_ID ) ? USB_INPUT : USB_OUTPUT
/*---------------------------- Structures ------------------------------------*/
typedef struct
{
  uint8_t* data;
} USBEvent;

typedef struct
{
  uint8_t      count;
  uint8_t      len;
  CONFIG_FILD  fild;
} USB_ConfigControl;

typedef struct
{
  USB_ReportDir  dir;
  USB_ReportCmd  cmd;
  USB_ReportStat stat;
  CONFIG_FILD    fild;
  uint16_t       adr;
  uint16_t       data;
  uint8_t*       buf;
} USB_REPORT;
/*------------------------------ Extern --------------------------------------*/

/*----------------------------- Functions ------------------------------------*/
void               vUSBinit( osThreadId_t taskHandle );
void               StartUsbTask( void *argument );

USB_StatusConnect  eUSBgetStatus( void );
void               vUSBreceiveHandler( void );
/*----------------------------------------------------------------------------*/
#endif /* INC_USBHID_H_ */
