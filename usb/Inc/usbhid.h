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
#include "chart.h"
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
  USB_DONE,
  USB_CONT,
  USB_ERROR_LENGTH,
} USB_Status;

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
  USB_GET_CONFIG_CMD = 0x01U,
  USB_PUT_CONFIG_CMD = 0x02U,
  USB_GET_CHART_CMD  = 0x03U,
  USB_PUT_CHART_CMD  = 0x04U,
} USB_ReportCmd;
/*------------------------------ Default -------------------------------------*/
#define USB_REPORT_SIZE       65U
#define USB_QUEUE_SIZE        10U
#define	BUFFER_DATA_SHIFT     6U
#define USB_DATA_SIZE         ( USB_REPORT_SIZE - BUFFER_DATA_SHIFT )
/*------------------------------ Report --------------------------------------*/
/*
 * | 0  |  1  |  2   |  4   |  5   |  6  | ...  | ... |  64 |
 * | ID | CMD | STAT | ADR1 | ADR0 | LEN | DATA | ... | ... |
 */
#define USB_INPUT_REPORT_ID   0x01U    /* Direction - first byte in report */
#define USB_OUTPUT_REPORT_ID  0x02U    /* Direction - first byte in report */
/*------------------------------ Macros --------------------------------------*/
#define USB_GET_DIR_VAL(e)    ( e == USB_INPUT ) ? ( USB_INPUT_REPORT_ID ) : ( USB_OUTPUT_REPORT_ID )
#define USB_GET_DIR(e)        ( e == USB_INPUT_REPORT_ID ) ? ( USB_INPUT ) : ( USB_OUTPUT )
/*---------------------------- Structures ------------------------------------*/
typedef struct
{
  uint8_t      count;    /* */
  uint8_t      len;      /* */
  CONFIG_FILD  fild;     /* */
} USB_ConfigControl;

typedef struct
{
  USB_ReportDir  dir;     /* Direction of transaction */
  USB_ReportCmd  cmd;     /* Command */
  USB_ReportStat stat;    /* Status of transaction */
  uint16_t       adr;     /* Address of register */
  uint8_t*       data;    /* Data array */
  uint8_t        length;  /* Length of data array */
  uint8_t*       buf;     /* Pointer to the IO buffer */
  uint8_t        multi;   /* Number of USB messages for one report*/
} USB_REPORT;
/*------------------------------ Extern --------------------------------------*/

/*----------------------------- Functions ------------------------------------*/
void               vUSBinit( osThreadId_t taskHandle );  /* Initialization device and control USB size of report descriptor */
void               StartUsbTask( void *argument );       /* Processing USB input task */
USB_StatusConnect  eUSBgetStatus( void );                /* Get connection status of USB device */
void               vUSBreceiveHandler( void );           /* Handler of USB input interrupt */
/*----------------------------------------------------------------------------*/
#endif /* INC_USBHID_H_ */
