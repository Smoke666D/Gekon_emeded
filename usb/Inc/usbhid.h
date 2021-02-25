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
} USB_CONN_STATUS;

typedef enum
{
  USB_STATUS_DONE,               /* Operation done */
  USB_STATUS_CONT,               /* Message is not end. Wait for new package */
  USB_STATUS_ERROR_LENGTH,       /* Error in declared and received length of data*/
  USB_STATUS_ERROR_ADR,          /* Wrong address */
  USB_STATUS_STORAGE_ERROR,      /* Error in storage process ( EEPROM or RTC )  */
  USB_STATUS_UNAUTHORIZED_ERROR, /* Non unauthorized request */
  USB_STATUS_ENGINE_NON_STOP,    /* Access forbidden - engine is non stop */
  USB_STATUS_FORBIDDEN,          /* Access forbidden */
} USB_STATUS;

typedef enum
{
  USB_REPORT_DIR_OUTPUT,
  USB_REPORT_DIR_INPUT,
} USB_REPORT_DIR;

typedef enum
{
  USB_REPORT_STATE_OK           = 0x01U, /* 200 analog - all OK                             */
  USB_REPORT_STATE_BAD_REQ      = 0x02U, /* 400 analog - request is distorted               */
  USB_REPORT_STATE_NON_CON      = 0x03U, /* 404 analog - requesting a nonexistent resource  */
  USB_REPORT_STATE_UNAUTHORIZED = 0x04U, /* 401 analog - unauthorized request               */
  USB_REPORT_STATE_FORBIDDEN    = 0x05U, /* 403 analog - machine not stop, access forbidden */
  USB_REPORT_STATE_INTERNAL     = 0x06U, /* 500 analog - internal server error */
} USB_REPORT_STATE;

typedef enum
{
  USB_REPORT_CMD_GET_CONFIG             = 0x01U,
  USB_REPORT_CMD_PUT_CONFIG             = 0x02U,
  USB_REPORT_CMD_GET_CHART_OIL          = 0x03U,
  USB_REPORT_CMD_PUT_CHART_OIL          = 0x04U,
  USB_REPORT_CMD_GET_CHART_COOLANT      = 0x05U,
  USB_REPORT_CMD_PUT_CHART_COOLANT      = 0x06U,
  USB_REPORT_CMD_GET_CHART_FUEL         = 0x07U,
  USB_REPORT_CMD_PUT_CHART_FUEL         = 0x08U,
  USB_REPORT_CMD_PUT_EWA                = 0x09U,
  USB_REPORT_CMD_SAVE_CONFIG            = 0x0AU,
  USB_REPORT_CMD_SAVE_CHART             = 0x0BU,
  USB_REPORT_CMD_GET_TIME               = 0x0CU,
  USB_REPORT_CMD_PUT_TIME               = 0x0DU,
  USB_REPORT_CMD_GET_FREE_DATA          = 0x0EU,
  USB_REPORT_CMD_PUT_FREE_DATA          = 0x0FU,
  USB_REPORT_CMD_GET_LOG                = 0x10U,
  USB_REPORT_CMD_ERASE_LOG              = 0x11U,
  USB_REPORT_CMD_PUT_PASSWORD           = 0x12U,
  USB_REPORT_CMD_AUTHORIZATION          = 0x13U,
  USB_REPORT_CMD_ERASE_PASSWORD         = 0x14U,
  USB_REPORT_CMD_GET_MEMORY_SIZE        = 0x15U,
  USB_REPORT_CMD_GET_MEASUREMENT        = 0x16U,
  USB_REPORT_CMD_ERASE_MEASUREMENT      = 0x17U,
  USB_REPORT_CMD_GET_MEASUREMENT_LENGTH = 0x18U,
} USB_REPORT_CMD;
/*------------------------------ Default -------------------------------------*/
#define USB_REPORT_SIZE       65U
#define USB_QUEUE_SIZE        10U
/*------------------------------ Report --------------------------------------*/
/*
 * |  0  |  1  |  2   |  3   |  4   |  5   |  6   |  7   |  ...  | ... |  64 |
 * | DIR | CMD | STAT | ADR1 | ADR0 | LEN2 | LEN1 | LEN0 | DATA  | ... | ... |
 */
#define USB_DIR_BYTE          0x00U
#define USB_CMD_BYTE          0x01U
#define USB_STAT_BYTE         0x02U
#define USB_ADR1_BYTE         0x03U
#define USB_ADR0_BYTE         0x04U
#define USB_LEN2_BYTE         0x05U
#define USB_LEN1_BYTE         0x06U
#define USB_LEN0_BYTE         0x07U
#define USB_DATA_BYTE         0x08U

#define USB_DATA_SIZE         ( USB_REPORT_SIZE - USB_DATA_BYTE )

#define USB_INPUT_REPORT_ID   0x01U    /* Direction - first byte in report */
#define USB_OUTPUT_REPORT_ID  0x02U    /* Direction - first byte in report */
/*------------------------------ Macros --------------------------------------*/
#define USB_GET_DIR_VAL(e)    ( e == USB_REPORT_DIR_INPUT ) ? ( USB_INPUT_REPORT_ID ) : ( USB_OUTPUT_REPORT_ID )
#define USB_GET_DIR(e)        ( e == USB_INPUT_REPORT_ID ) ? ( USB_REPORT_DIR_INPUT ) : ( USB_REPORT_DIR_OUTPUT )
/*---------------------------- Structures ------------------------------------*/
typedef struct __packed
{
  uint8_t      count;    /* */
  uint8_t      len;      /* */
  CONFIG_FILD  fild;     /* */
} USB_ConfigControl;

typedef struct __packed
{
  USB_REPORT_DIR   dir;     /* Direction of transaction */
  USB_REPORT_CMD   cmd;     /* Command */
  USB_REPORT_STATE stat;    /* Status of transaction */
  uint16_t         adr;     /* Address of register */
  uint8_t*         data;    /* Data array */
  uint32_t         length;  /* Length of data array */
  uint8_t*         buf;     /* Pointer to the IO buffer */
  uint8_t          multi;   /* Number of USB messages for one report*/
} USB_REPORT;

typedef USB_STATUS ( *dataCallBack )( USB_REPORT* report );  /* callback for data operations */
/*------------------------------ Extern --------------------------------------*/

/*----------------------------- Functions ------------------------------------*/
void            vUSBinit ( osThreadId_t taskHandle );  /* Initialization device and control USB size of report descriptor */
void            vStartUsbTask ( void *argument );      /* Processing USB input task */
USB_CONN_STATUS eUSBgetStatus ( void );                /* Get connection status of USB device */
void            vUSBreceiveHandler ( void );           /* Handler of USB input interrupt */
void            vUSBplugHandler ( void );
void            vUSBunplugHandler ( void );
/*----------------------------------------------------------------------------*/
#endif /* INC_USBHID_H_ */
