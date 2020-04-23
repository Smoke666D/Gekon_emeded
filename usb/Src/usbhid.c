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
void               vUint32ToBytes( uint32_t input, uint8_t* output );
void               vUint16ToBytes( uint16_t input, uint8_t* output );
void               vUSBmakeReport( USB_REPORT* report );                          /* Convert report structure to the output buffer */
void               vUSBparseReport( USB_REPORT* report );                         /* Parse input buffer to the report structure */
void               vUSBresetControl( USB_ConfigControl* control );                /* Clean control structure */
void               vUSBConfigToReport( eConfigReg* config, USB_REPORT* report );  /* Transfer configuration register to the report structure */
void               vUSBChartToReport( uint16_t adr, eChartData* chart, USB_REPORT* report );
USB_Status         eUSBReportToConfig( USB_REPORT* report );
USBD_StatusTypeDef eUSBwrite( uint8_t* data );                                    /* Send data via USB */
void               vUSBsendConfig( eConfigReg* config );                          /* Send configuration register via USB */
void               vUSBsendChart( uint16_t adr, eChartData* chart );              /* Send chart data via USB */
void               vUSBgetConfig( USB_REPORT* report );                           /* Get configuration register via USB */
void               vUSBgetChart( USB_REPORT* report );                            /* Get chart data via USB */
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void vUint32ToBytes( uint32_t input, uint8_t* output )
{
  output[0] = ( input & 0xFF000000U ) >> 24;
  output[1] = ( input & 0x00FF0000U ) >> 16;
  output[2] = ( input & 0x0000FF00U ) >> 8;
  output[3] = input & 0x000000FFU;
  return;
}
void vUint16ToBytes( uint16_t input, uint8_t* output )
{
  output[0] = ( input & 0xFF00U ) >> 8;
  output[1] = input & 0x00FFU;
  return;
}
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
void vUSBConfigToReport( eConfigReg* config, USB_REPORT* report )
{
  uint8_t i     = 0U;
  uint8_t count = 0U;
  uint8_t shift = uEncodeURI( strBuffer, config->units, MAX_UNITS_LENGTH );

  report->cmd  = USB_GET_CONFIG_CMD;
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
    report->data[count++] = ( uint8_t )( strBuffer[i] );
  }
  /*-------------------------------------------*/
  report->length = count;

  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Transfer chart data to the report structure
 * input:  adr    - adress of chart in memory map
 *         chart  - input chart data
 *         report - output structure
 * output: status of operation
 */
void vUSBChartToReport( uint16_t adr, eChartData* chart, USB_REPORT* report )
{
  uint8_t i     = 0;
  uint8_t count = 0;
  uint8_t shift = 0;

  report->cmd  = USB_GET_CHART_CMD;
  report->dir  = USB_OUTPUT;
  report->stat = USB_OK_STAT;
  report->adr  = adr;
  /*--------------- Chart data ----------------*/
  vUint32ToBytes( ( uint32_t )( chart->xmin ), &report->data[count] );
  count += 4U;
  vUint32ToBytes( ( uint32_t )( chart->xmax ), &report->data[count] );
  count += 4U;
  vUint32ToBytes( ( uint32_t )( chart->ymin ), &report->data[count] );
  count += 4U;
  vUint32ToBytes( ( uint32_t )( chart->ymax ), &report->data[count] );
  count += 4U;
  /*--------------- X unit --------------------*/
  shift = uEncodeURI( strBuffer, chart->xunit, CHART_UNIT_LENGTH );
  for ( i=0; i<shift; i++ )
  {
    report->data[count++] = ( uint8_t )( strBuffer[i] );
  }
  for ( i=shift; i<( CHART_UNIT_LENGTH * 6 ); i++ )
  {
    report->data[count++] = 0x00U;
  }
  /*--------------- Y unit --------------------*/
  shift = uEncodeURI( strBuffer, chart->yunit, CHART_UNIT_LENGTH );
  for ( i=0; i<shift; i++ )
  {
    report->data[count++] = ( uint8_t )( strBuffer[i] );
  }
  for ( i=shift; i<( CHART_UNIT_LENGTH * 6 ); i++ )
  {
    report->data[count++] = 0x00U;
  }
  /*----------------- Size --------------------*/
  vUint16ToBytes( chart->size, &report->data[count] );
  count += 2U;
  /*----------------- Dots --------------------*/
  for ( i=0U; i<chart->size; i++ )
  {
    vUint32ToBytes( ( uint32_t )( chart->dots[i].x ), &report->data[count] );
    count += 4U;
    vUint32ToBytes( ( uint32_t )( chart->dots[i].y ), &report->data[count] );
    count += 4U;
  }

  /*-------------------------------------------*/
  return;
}
/*---------------------------------------------------------------------------------------------------*/
USB_Status eUSBReportToConfig( USB_REPORT* report )
{
  USB_Status res    = USB_DONE;
  uint8_t    count  = 0U;
  uint8_t    i      = 0U;
  uint8_t    length = 0U;
  /*------------- Length control --------------*/
  length = configReg[report->adr]->len * 2U + 1U + MAX_UNITS_LENGTH * 6U;
  if ( length < report->length ) {
	res = USB_ERROR_LENGTH;
  }
  else {
  /*----------- Configuration value -----------*/
    for ( i=0; i<configReg[report->adr]->len; i++ )
    {
      configReg[report->adr]->value[i] = ( uint16_t )( report->data[count + 2U * i + 1U] ) | ( ( uint16_t )( report->data[count + 2U * i] ) << 8U );
    }
    count += 2U * configReg[report->adr]->len;
  /*----------- Configuration scale -----------*/
    configReg[report->adr]->scale = ( signed char )( report->data[count++] );
  /*----------- Configuration units -----------*/
    vDecodeURI( ( char* )( &report->data[count] ), configReg[report->adr]->units, MAX_UNITS_LENGTH );
  }
  return res;
}

USB_Status eUSBReportToChart( USB_REPORT* report )
{
  USB_Status res    = USB_DONE;
  uint8_t    count  = 0U;
  uint8_t    i      = 0U;
  uint8_t    length = 0U;
  /**/
  res = USB_ERROR_LENGTH;
  /**/
  return res;
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
    report->buf[BUFFER_DATA_SHIFT + i] = report->data[i];
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
  report->data   = &(report->buf[BUFFER_DATA_SHIFT]);
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
	.data = &usbBuffer[BUFFER_DATA_SHIFT],
  };

  vUSBConfigToReport( config, &report );
  vUSBmakeReport( &report );
  while ( eUSBwrite( report.buf ) == USBD_BUSY )
  {
    osDelay( 2U );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Send chart data via USB
 * input:  configuration register
 * output: none
 */
void vUSBsendChart( uint16_t adr, eChartData* chart )
{
  USB_REPORT report =
  {
    .buf  = usbBuffer,
    .data = &usbBuffer[BUFFER_DATA_SHIFT],
  };

  vUSBChartToReport( adr, chart, &report );
  vUSBmakeReport( &report );
  while ( eUSBwrite( report.buf ) == USBD_BUSY )
  {
    osDelay( 2U );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Get configuration register via USB
 * input:  report of transaction
 * output: none
 */
void vUSBgetConfig( USB_REPORT* report )
{
  uint8_t i = 0U;
  USB_REPORT response =
  {
    .cmd    = USB_PUT_CONFIG_CMD,
	.stat   = USB_OK_STAT,
	.adr    = report->adr,
	.length = 0U,
    .buf    = usbBuffer,
    .data   = &usbBuffer[BUFFER_DATA_SHIFT],
  };

  if ( report->adr == 59 )
  {
	i = 0U;
  }

  if ( eUSBReportToConfig( report ) != USB_DONE )
  {
	response.stat = USB_BAD_REQ_STAT;
  }
  for( i=0U; i<USB_REPORT_SIZE; i++ )
  {
    usbBuffer[i] = 0U;
  }
  vUSBmakeReport( &response );
  while ( eUSBwrite( response.buf ) == USBD_BUSY )
  {
    osDelay( 2U );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Get chart data via USB
 * input:  report of transaction
 * output: none
 */
void vUSBgetChart( USB_REPORT* report )
{
  uint8_t i = 0U;
  USB_REPORT response =
  {
    .cmd    = USB_PUT_CONFIG_CMD,
    .stat   = USB_OK_STAT,
    .adr    = report->adr,
    .length = 0U,
    .buf    = usbBuffer,
    .data   = &usbBuffer[BUFFER_DATA_SHIFT],
  };
  if ( eUSBReportToChart( report ) != USB_DONE )
  {
    response.stat = USB_BAD_REQ_STAT;
  }
  for( i=0U; i<USB_REPORT_SIZE; i++ )
  {
    usbBuffer[i] = 0U;
  }
  vUSBmakeReport( &response );
  while ( eUSBwrite( response.buf ) == USBD_BUSY )
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
        case USB_GET_CONFIG_CMD:
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
        case USB_PUT_CONFIG_CMD:
          if ( report.adr < SETTING_REGISTER_NUMBER )
          {
            vUSBgetConfig( &report );
          }
          break;
          /*
        case USB_GET_CHART_CMD:
        	if ( report.adr == BROADCAST_ADR )
        	{
        	  for ( i=0U; i<CHART_NUMBER; i++ )
        	  {
        	    //vUSBsendChart( i, charts[i] );
        	  }
        	}
        	else if ( report.adr < CHART_NUMBER )
        	{
        	  //vUSBsendChart( report.adr, charts[report.adr] );
        	}
          break;
        case USB_PUT_CHART_CMD:
            if ( report.adr < CHART_NUMBER )
            {
              //vUSBgetChart( &report );
            }
          break;
          */
        default:
          break;
      }
    }
  }
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/



