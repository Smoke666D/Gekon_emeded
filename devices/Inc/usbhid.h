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
#include "usbd_custom_hid_if.h"
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
  USB_STATUS_DONE,               /* 0 Operation done */
  USB_STATUS_CONT,               /* 1 Message is not end. Wait for new package */
  USB_STATUS_ERROR_LENGTH,       /* 2 Error in declared and received length of data*/
  USB_STATUS_ERROR_ADR,          /* 3 Wrong address */
  USB_STATUS_ERROR_DATA,         /* 4 Wrong data field */
  USB_STATUS_ERROR_COMMAND,      /* 5 Wrong command */
  USB_STATUS_STORAGE_ERROR,      /* 6 Error in storage process ( EEPROM or RTC )  */
  USB_STATUS_UNAUTHORIZED_ERROR, /* 7 Non unauthorized request */
  USB_STATUS_ENGINE_NON_STOP,    /* 8 Access forbidden - engine is non stop */
  USB_STATUS_FORBIDDEN,          /* 9 Access forbidden */
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
  USB_REPORT_STATE_AUTO_MODE    = 0x07U
} USB_REPORT_STATE;

typedef enum
{
  USB_REPORT_CMD_NULL              = 0x00U,
  USB_REPORT_CMD_GET_CONFIG        = 0x01U,
  USB_REPORT_CMD_PUT_CONFIG        = 0x02U,
  USB_REPORT_CMD_GET_CHART_OIL     = 0x03U,
  USB_REPORT_CMD_PUT_CHART_OIL     = 0x04U,
  USB_REPORT_CMD_GET_CHART_COOLANT = 0x05U,
  USB_REPORT_CMD_PUT_CHART_COOLANT = 0x06U,
  USB_REPORT_CMD_GET_CHART_FUEL    = 0x07U,
  USB_REPORT_CMD_PUT_CHART_FUEL    = 0x08U,
  USB_REPORT_CMD_RESERVED_0        = 0x09U,
  USB_REPORT_CMD_SAVE_CONFIG       = 0x0AU,
  USB_REPORT_CMD_SAVE_CHART        = 0x0BU,
  USB_REPORT_CMD_GET_TIME          = 0x0CU,
  USB_REPORT_CMD_PUT_TIME          = 0x0DU,
  USB_REPORT_CMD_GET_FREE_DATA     = 0x0EU,
  USB_REPORT_CMD_PUT_FREE_DATA     = 0x0FU,
  USB_REPORT_CMD_GET_LOG           = 0x10U,
  USB_REPORT_CMD_ERASE_LOG         = 0x11U,
  USB_REPORT_CMD_PUT_PASSWORD      = 0x12U,
  USB_REPORT_CMD_AUTHORIZATION     = 0x13U,
  USB_REPORT_CMD_ERASE_PASSWORD    = 0x14U,
  USB_REPORT_CMD_RESERVED_1        = 0x15U,
  USB_REPORT_CMD_RESERVED_2        = 0x16U,
  USB_REPORT_CMD_RESERVED_3        = 0x17U,
  USB_REPORT_CMD_GET_SD            = 0x18U,
  USB_REPORT_CMD_GET_OUTPUT        = 0x19U,
  USB_REPORT_CMD_PUT_OUTPUT        = 0x1AU
} USB_REPORT_CMD;
/*
 * |  0  |  1  |  2   |  3   |  4   |  ...  | ... |  64 |
 * | DIR | CMD | STAT | ADR0 | LEN0 | DATA  | ... | ... |
 */
typedef enum
{
  USB_DIR_BYTE,  /* 0 */
  USB_CMD_BYTE,  /* 1 */
  USB_STAT_BYTE, /* 2 */
  USB_ADR0_BYTE, /* 3 */
  USB_LEN0_BYTE, /* 4 */
  USB_DATA_BYTE, /* 5 */
} USB_BYTES;
typedef enum
{
  USB_INPUT_REPORT_ID  = 0x01U,    /* Direction - first byte in report */
  USB_OUTPUT_REPORT_ID = 0x02U     /* Direction - first byte in report */
} USB_REPORT_ID;
/*------------------------------ Default -------------------------------------*/
#define USB_REPORT_SIZE ( REPORT_COUNT + 1U )
#define USB_DATA_SIZE   ( USB_REPORT_SIZE - USB_DATA_BYTE )
/*------------------------------ Macros --------------------------------------*/
#define USB_GET_DIR_VAL( e ) ( e == USB_REPORT_DIR_INPUT ) ? ( USB_INPUT_REPORT_ID ) : ( USB_OUTPUT_REPORT_ID )
#define USB_GET_DIR( e )     ( e == USB_INPUT_REPORT_ID ) ? ( USB_REPORT_DIR_INPUT ) : ( USB_REPORT_DIR_OUTPUT )
#if ( USBD_CUSTOMHID_OUTREPORT_BUF_SIZE < ( REPORT_COUNT + 1U ) )
  #error "Wrong USB buffer size"
#endif
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
  uint8_t          adr;     /* Address of register */
  uint8_t*         data;    /* Data array */
  uint8_t          length;  /* Length of data array */
  uint8_t*         buf;     /* Pointer to the IO buffer */
  uint8_t          multi;   /* Number of USB messages for one report*/
} USB_REPORT;

typedef USB_STATUS ( *dataCallBack )( USB_REPORT* report );  /* callback for data operations */
/*------------------------------ Extern --------------------------------------*/

/*----------------------------- Functions ------------------------------------*/
#if defined( UNIT_TEST )
  void       vUint32ToBytes ( uint32_t input, uint8_t* output );
  void       vUint24ToBytes ( uint32_t input, uint8_t* output );
  void       vUint16ToBytes ( uint16_t input, uint8_t* output );
  uint16_t   uByteToUnit16 ( const uint8_t* data );
  uint32_t   uByteToUint24 ( const uint8_t* data );
  uint32_t   uByteToUint32 ( const uint8_t* data );
  void       vUSBtimeToReport ( USB_REPORT* report );
  void       vUSBfreeDataToReport ( USB_REPORT* report );
  void       vUSBlogToReport ( USB_REPORT* report );
  void       vUSBconfigToReport ( USB_REPORT* report );
  void       vUSBchartToReport ( USB_REPORT* report );
  USB_STATUS eUSBreportToTime ( const USB_REPORT* report );
  USB_STATUS eUSBreportToPassword ( const USB_REPORT* report );
  USB_STATUS eUSBcheckupPassword ( const USB_REPORT* report );
  USB_STATUS eUSBreportToFreeData ( const USB_REPORT* report );
  USB_STATUS eUSBreportToConfig ( const USB_REPORT* report );
  USB_STATUS eUSBreportToChart ( const USB_REPORT* report );
  void       vUSBmakeReport ( USB_REPORT* report );
  void       vUSBparseReport ( USB_REPORT* report );
  USB_STATUS eUSBsaveConfigs ( const USB_REPORT* report );
  USB_STATUS eUSBsaveCharts ( const USB_REPORT* report );
  USB_STATUS eUSBeraseLOG ( const USB_REPORT* report );
  USB_STATUS eUSBerasePassword ( const USB_REPORT* report );
  void       vUSBmeasurementToReport ( USB_REPORT* report );
  void       vUSBmeasurementLengthToReport ( USB_REPORT* report );
  void       vUSBoutputToReport ( USB_REPORT* report );
  USB_STATUS eUSBreportToOutput ( const USB_REPORT* report );
#endif
void            vUSBinit ( osThreadId_t taskHandle );  /* Initialization device and control USB size of report descriptor */
void            vStartUsbTask ( void *argument );      /* Processing USB input task */
USB_CONN_STATUS eUSBgetStatus ( void );                /* Get connection status of USB device */
void            vUSBreceiveHandler ( void );           /* Handler of USB input interrupt */
void            vUSBplugHandler ( void );
void            vUSBunplugHandler ( void );
/*----------------------------------------------------------------------------*/
#endif /* INC_USBHID_H_ */
