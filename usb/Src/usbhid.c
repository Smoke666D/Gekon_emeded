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
static uint8_t           inputBuffer[USB_REPORT_SIZE];
static char              strBuffer[MAX_UNITS_LENGTH * 6U + 1U];
static osThreadId_t      usbHandle;
/*----------------------- Functions -----------------------------------------------------------------*/
void               vUSBmakeReport( USB_REPORT* report );                          /* Convert report structure to the output buffer */
void               vUSBparseReport( USB_REPORT* report );                         /* Parse input buffer to the report structure */
void               vUSBresetControl( USB_ConfigControl* control );                /* Clean control structure */
void               eUSBConfigToReport( eConfigReg* config, USB_REPORT* report );  /* Transfer configuration register to the report structure */
USBD_StatusTypeDef eUSBwrite( uint8_t* data );                                    /* Send data via USB */
void               vUSBsendConfig( eConfigReg* config );                          /* Send configuration register via USB */
/*---------------------------------------------------------------------------------------------------*/
/*
 * Initialization device and control USB size of report descriptor
 * input:  task for handling input USB data
 * output: none
 */
void vUSBinit( osThreadId_t taskHandle )
{
  if( BUILD_BUG_OR_ZERO( USBD_CUSTOMHID_OUTREPORT_BUF_SIZE - USB_REPORT_SIZE ) == 0U ) /* Test size of report */
  {
    usbHandle = taskHandle;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Get connection status of USB device
 * input:  output
 * output: status
 */
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
/*
 * Send data via USB
 * input:  array of data
 * output: status of transmission
 */
USBD_StatusTypeDef eUSBwrite( uint8_t* data )
{
  return USBD_CUSTOM_HID_SendReport( &hUsbDeviceFS, data, USB_REPORT_SIZE );;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Clean control structure
 * input:  control structure
 * output: none
 */
void vUSBresetControl( USB_ConfigControl* control )
{
  control->count = 0U;
  control->len   = 0U;
  control->fild  = CONFIG_NO;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Transfer configuration register to the report structure
 * input:  config  - input configuration register
 *         report  - output structure
 * output: status of operation
 */
void eUSBConfigToReport( eConfigReg* config, USB_REPORT* report )
{
  uint8_t               i     = 0U;
  uint8_t               count = 0U;
  uint8_t               shift = uEncodeURI( strBuffer, config->units, MAX_UNITS_LENGTH );

  report->cmd  = USB_GET_CMD;
  report->dir  = USB_OUTPUT;
  report->stat = USB_OK_STAT;
  report->adr  = config->adr;
  /*----------- Configuration value -----------*/
  for ( i=0U; i<config->len; i++ )
  {
    report->data[count + 2U*i]      = ( uint8_t )( ( config->value[i] >> 8U ) & 0x00FF );
    report->data[count + 2U*i + 1U] = ( uint8_t )( config->value[i] & 0x00FF );
  }
  count += 2U * config->len;
  /*----------- Configuration scale -----------*/
  report->data[count++] = ( uint8_t )( config->scale );
  /*----------- Configuration units -----------*/
  for( i=0U; i<shift; i++ )
  {
    report->data[count + i] = ( uint8_t )( strBuffer[i] );
	//report->data[count + i] = 0x20;
  }
  count += shift;
  /*-------------------------------------------*/
  report->length = count;

  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Convert report structure to the output buffer
 * input:  report structure
 * output: none
 */
void vUSBmakeReport( USB_REPORT* report )
{
  uint8_t i = 0U;
  report->buf[0U] = USB_GET_DIR_VAL( report->dir );
  report->buf[1U] = report->cmd;
  report->buf[2U] = report->stat;
  report->buf[3U] = ( uint8_t )( report->adr >> 8U );
  report->buf[4U] = ( uint8_t )( report->adr );
  report->buf[5U] = report->length;
  for( i=0; i<report->length; i++)
  {
    report->buf[6U + i] = report->data[i];
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Parse input buffer to the report structure
 * input:  report structure
 * output: none
 */
void vUSBparseReport( USB_REPORT* report )
{
  report->dir    = USB_GET_DIR( report->buf[0U] );
  report->cmd    = report->buf[1U];
  report->stat   = report->buf[2U];
  report->adr    = ( ( ( uint16_t )( report->buf[3U] ) ) << 8 ) | ( ( uint16_t )( report->buf[4U] ) );
  report->length = report->buf[5U];
  report->data   = &(report->buf[6U]);
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Send configuration register via USB
 * input:  configuration register
 * output: none
 */
void vUSBsendConfig( eConfigReg* config )
{
  USB_REPORT report =
  {
    .buf  = usbBuffer,
	.data = &usbBuffer[6U],
  };

  eUSBConfigToReport( config, &report );
  vUSBmakeReport( &report );
  while ( eUSBwrite( report.buf ) == USBD_BUSY )
  {
    osDelay( 2U );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*
 * Handler of USB input interrupt
 * input:  none
 * output: none
 */
void vUSBreceiveHandler( void )
{
  USBD_CUSTOM_HID_HandleTypeDef *hhid  = (USBD_CUSTOM_HID_HandleTypeDef*)hUsbDeviceFS.pClassData;
  uint8_t                        i     = 0U;
  BaseType_t                     yield = pdFALSE;

  /* Copy input buffer to local buffer */
  for ( i=0U; i<USB_REPORT_SIZE; i++)
  {
    inputBuffer[i] = hhid->Report_buf[i];
  }
  /* Start USB processing task */
  if ( usbHandle != NULL )
  {
    vTaskNotifyGiveFromISR( usbHandle, &yield );
  	portYIELD_FROM_ISR ( yield );
  }

  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Processing USB input task
 * input:  none
 * output: none
 */
void StartUsbTask( void *argument )
{
  USB_REPORT report = {.buf = inputBuffer};
  uint16_t   i      = 0U;
  for(;;)
  {
    if ( ulTaskNotifyTake( pdTRUE, portMAX_DELAY ) > 0U )
    {
      vUSBparseReport( &report );
      switch( report.cmd )
      {
        case USB_GET_CMD:
          if ( report.adr == BROADCAST_ADR )
          {
        	for ( i=0U; i<SETTING_REGISTER_NUMBER; i++ )
        	{
              vUSBsendConfig( configReg[i] );
        	}
          }
          else if ( report.adr < SETTING_REGISTER_NUMBER )
          {
            vUSBsendConfig( configReg[report.adr] );
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



