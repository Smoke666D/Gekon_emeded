/*
 * usbhid.c
 *
 *  Created on: 2 апр. 2020 г.
 *      Author: photo_Mickey
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "usbhid.h"
#include "usbd_conf.h"
#include "common.h"
#include "usb_device.h"
/*----------------------- Structures ----------------------------------------------------------------*/
extern USBD_HandleTypeDef  hUsbDeviceFS;
/*----------------------- Constant ------------------------------------------------------------------*/

/*----------------------- Variables -----------------------------------------------------------------*/
static uint8_t           usbBuffer[USB_REPORT_SIZE];
static USB_ConfigControl configControl;
static char              strBuffer[MAX_UNITS_LENGTH * 3U + 1U];

static uint8_t           inputBuffer[USB_REPORT_SIZE];

static osThreadId_t      usbHandle;
/*----------------------- Functions -----------------------------------------------------------------*/
void                  vUSBmakeReport( USB_REPORT* report );
void                  vUSBparseReport( USB_REPORT* report );
void                  vUSBresetControl( USB_ConfigControl* control );
USB_StatusTransaction eUSBConfigToReport( eConfigReg* config, USB_ConfigControl control,  USB_REPORT* report );
USBD_StatusTypeDef    eUSBwrite( uint8_t* data );
void                  vUSBsendConfig( eConfigReg* config );
/*---------------------------------------------------------------------------------------------------*/
void vUSBinit( osThreadId_t taskHandle )
{
  if( BUILD_BUG_OR_ZERO( USBD_CUSTOMHID_OUTREPORT_BUF_SIZE - USB_REPORT_SIZE ) == 0U ) /* Test size of report */
  {
    usbHandle = taskHandle;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
USB_StatusConnect eUSBgetStatus( void )
{
  USB_StatusConnect res = USB_DISCONNECT;

  if ( hUsbDeviceFS.dev_state == USBD_STATE_CONFIGURED )
  {
	res = USB_CONNECT;
  }

  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USBD_StatusTypeDef eUSBwrite( uint8_t* data )
{
  return USBD_CUSTOM_HID_SendReport( &hUsbDeviceFS, data, USB_REPORT_SIZE );;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBresetControl( USB_ConfigControl* control )
{
  control->count = 0U;
  control->len   = 0U;
  control->fild  = CONFIG_NO;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
USB_StatusTransaction eUSBConfigToReport( eConfigReg* config, USB_ConfigControl control,  USB_REPORT* report )
{
  USB_StatusTransaction res = USB_TransactionContin;

  if ( ( control.count >= control.len ) || ( control.fild == CONFIG_NO ) )
  {
    report->cmd   = USB_GET_CMD;
    report->dir   = USB_OUTPUT;
    report->stat  = USB_OK_STAT;
    report->adr   = config->adr;
	control.len   = 0U;
	control.count = 0U;
	control.fild++;
	if ( control.fild >= FILDS_TO_WRITE_NUMBER )
	{
	  res = USB_TransactionDone;
	}
  }
  switch (control.fild)
  {
    case CONFIG_VALUE:
      if ( control.len == 0U )
      {
    	control.len = config->len;
      }
      report->data = config->value[control.count];
      control.count++;
      break;
    case CONFIG_SCALE:
      if ( control.len == 0U )
      {
    	control.len = 1U;
      }
      report->data = ( ( uint16_t )( config->scale ) & 0x00FF );
      control.count++;
      break;
    case CONFIG_UNITS:
      if ( control.len == 0U )
      {
    	 control.len = uEncodeURI( strBuffer, config->units, MAX_UNITS_LENGTH );
      }
      report->data = ( uint16_t )( strBuffer[control.count] );
      control.count++;
      break;
    default:
      break;
  }

  return res;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBmakeReport( USB_REPORT* report )
{
  report->buf[0U] = USB_GET_DIR_VAL( report->dir );
  report->buf[1U] = report->cmd;
  report->buf[2U] = report->stat;
  report->buf[3U] = report->fild;
  report->buf[4U] = ( uint8_t )( report->adr >> 8U );
  report->buf[5U] = ( uint8_t )( report->adr );
  report->buf[6U] = ( uint8_t )( report->data >> 8U );
  report->buf[7U] = ( uint8_t )( report->data );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBparseReport( USB_REPORT* report )
{
  report->dir = USB_GET_DIR( report->buf[0U] );
  report->cmd  = report->buf[1U];
  report->stat = report->buf[2U];
  report->fild = report->buf[3U];
  report->adr  = ( ( ( uint16_t )( report->buf[4U] ) ) << 8 ) | ( ( uint16_t )( report->buf[5U] ) );
  report->data = ( ( ( uint16_t )( report->buf[6U] ) ) << 8 ) | ( ( uint16_t )( report->buf[7U] ) );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBsendConfig( eConfigReg* config )
{
  USB_REPORT report;

  report.buf = usbBuffer;
  vUSBresetControl( &configControl );
  while ( eUSBConfigToReport( config, configControl, &report ) == USB_TransactionContin )
  {
    vUSBmakeReport( &report );
    while ( eUSBwrite( report.buf ) == USBD_BUSY )
    {
      osDelay( 10U );
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void vUSBreceiveHandler( void )
{
  USBD_CUSTOM_HID_HandleTypeDef *hhid = (USBD_CUSTOM_HID_HandleTypeDef*)hUsbDeviceFS.pClassData;
  uint8_t      i     = 0U;
  BaseType_t   yield = pdFALSE;

  for ( i=0U; i<USB_REPORT_SIZE; i++)
  {
    inputBuffer[i] = hhid->Report_buf[i];
  }

  if ( usbHandle != NULL )
  {
    vTaskNotifyGiveFromISR( usbHandle, &yield );
  	portYIELD_FROM_ISR ( yield );
  }

  return;
}
/*---------------------------------------------------------------------------------------------------*/
void StartUsbTask( void *argument )
{
  USB_REPORT report;
  uint16_t   i      = 0U;
  for(;;)
  {
    if ( ulTaskNotifyTake( pdTRUE, portMAX_DELAY ) > 0U )
    {
      vUSBparseReport( &report );
      switch( report->cmd )
      {
        case USB_GET_CMD:
          if ( report->adr == BROADCAST_ADR )
          {
        	for ( i=0U; i<SETTING_REGISTER_NUMBER; i++ )
        	{
              vUSBsendConfig( configReg[i] );
        	}
          }
          else if ( report->adr < SETTING_REGISTER_NUMBER )
          {
            vUSBsendConfig( configReg[report->adr] );
          }
          break;
        case USB_PUT_CMD:
          break;
        default:
          break;
      }
    }
  }
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/



