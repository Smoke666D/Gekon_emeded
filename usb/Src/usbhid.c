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
#include "fix16.h"
/*----------------------- Structures ----------------------------------------------------------------*/
extern USBD_HandleTypeDef  hUsbDeviceFS;
/*----------------------- Constant ------------------------------------------------------------------*/

/*----------------------- Variables -----------------------------------------------------------------*/
static uint8_t      usbBuffer[USB_REPORT_SIZE];
static uint8_t      inputBuffer[USB_REPORT_SIZE];
static char         strBuffer[MAX_UNITS_LENGTH * 6U + 1U];
static osThreadId_t usbHandle;
/*----------------------- Functions -----------------------------------------------------------------*/
void               vUint32ToBytes( const uint32_t* input, uint8_t* output );
void               vUint16ToBytes( const uint16_t* input, uint8_t* output );
void               vUSBmakeReport( USB_REPORT* report );                          /* Convert report structure to the output buffer */
void               vUSBparseReport( USB_REPORT* report );                         /* Parse input buffer to the report structure */
void               vUSBresetControl( USB_ConfigControl* control );                /* Clean control structure */
void               vUSBConfigToReport( const eConfigReg* config, USB_REPORT* report );  /* Transfer configuration register to the report structure */
void               vUSBChartToReport( uint16_t adr, const eChartData* chart, USB_REPORT* report );
USB_Status         vUSBChartDotsToReport( uint16_t adr, const eChartData* chart, USB_REPORT* report );
USB_Status         eUSBReportToConfig( USB_REPORT* report );
USBD_StatusTypeDef eUSBwrite( uint8_t* data );                                    /* Send data via USB */
void               vUSBsendConfig( eConfigReg* config );                          /* Send configuration register via USB */
void               vUSBsendChart( uint16_t adr, const eChartData* chart );        /* Send chart data via USB */
void               vUSBgetConfig( USB_REPORT* report );                           /* Get configuration register via USB */
void               vUSBgetChart( USB_REPORT* report );                            /* Get chart data via USB */
void               vUSBParsingChart( uint16_t adr, const uint8_t* data );
void               vUSBParsingChartDots( uint16_t adr, uint8_t firstDot, uint8_t size, const uint8_t* data );
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void vUint32ToBytes( const uint32_t* input, uint8_t* output )
{
  for ( uint8_t i=0; i<4U; i++ ) output[i] = *( ( uint8_t* )( input ) + i );
  return;
}
void vUint16ToBytes( const uint16_t* input, uint8_t* output )
{
  for ( uint8_t i=0; i<2U; i++ ) output[i] = *( ( uint8_t* )( input ) + i );
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
void vUSBConfigToReport( const eConfigReg* config, USB_REPORT* report )
{
  uint8_t i     = 0U;
  uint8_t count = 0U;
  uint8_t shift = uEncodeURI( config->units, MAX_UNITS_LENGTH, strBuffer );

  report->cmd  = USB_GET_CONFIG_CMD;
  report->dir  = USB_OUTPUT;
  report->stat = USB_OK_STAT;
  report->adr  = config->adr;
  /*----------- Configuration value -----------*/
  for ( i=0U; i<config->len; i++ )
  {
    report->data[count + 2U*i]      = ( uint8_t )( ( config->value[i] >> 8U ) & 0x00FFU );
    report->data[count + 2U*i + 1U] = ( uint8_t )( config->value[i] & 0x00FFU );
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
void vUSBChartToReport( uint16_t adr, const eChartData* chart, USB_REPORT* report )
{
  uint8_t i     = 0U;
  uint8_t count = 0U;
  uint8_t shift = 0U;
  /*-------------------------------------------*/
  report->cmd    = USB_GET_CHART_CMD;
  report->dir    = USB_OUTPUT;
  report->stat   = USB_OK_STAT;
  report->adr    = adr;
  report->length = 18U + CHART_UNIT_LENGTH * 12U + 8U * chart->size;
  /*--------------- Chart data ----------------*/
  vUint32ToBytes( ( uint32_t* )( &chart->xmin ), &report->data[count] );
  count += 4U;
  vUint32ToBytes( ( uint32_t* )( &chart->xmax ), &report->data[count] );
  count += 4U;
  vUint32ToBytes( ( uint32_t* )( &chart->ymin ), &report->data[count] );
  count += 4U;
  vUint32ToBytes( ( uint32_t* )( &chart->ymax ), &report->data[count] );
  count += 4U;
  /*--------------- X unit --------------------*/
  shift = uEncodeURI( chart->xunit, CHART_UNIT_LENGTH, strBuffer );
  for ( i=0U; i<shift; i++ )
  {
    report->data[count++] = ( uint8_t )( strBuffer[i] );
  }
  for ( i=shift; i<( CHART_UNIT_LENGTH * 6U ); i++ )
  {
    report->data[count++] = 0U;
  }
  /*--------------- Y unit --------------------*/
  shift = uEncodeURI( chart->yunit, CHART_UNIT_LENGTH, strBuffer );
  for ( i=0U; i<shift; i++ )
  {
    report->data[count++] = ( uint8_t )( strBuffer[i] );
  }
  for ( i=shift; i<( CHART_UNIT_LENGTH * 6U ); i++ )
  {
    report->data[count++] = 0U;
  }
  /*----------------- Size --------------------*/
  vUint16ToBytes( &chart->size, &report->data[count] );
  count += 2U;
  /*-------------------------------------------*/
  return;
}
/*---------------------------------------------------------------------------------------------------*/
USB_Status vUSBChartDotsToReport( uint16_t adr, const eChartData* chart, USB_REPORT* report )
{
  USB_Status res    = USB_DONE;
  uint8_t    i      = 0U;
  uint8_t    count  = 0U;
  uint8_t    length = 0U;
  uint8_t    size   = 0U;
  static uint8_t currentChart   = 0xFFU;
  static uint8_t lastDotNumber  = 0U;
  /*-------------------------------------------*/
  report->cmd  = USB_GET_CHART_CMD;
  report->dir  = USB_OUTPUT;
  report->stat = USB_OK_STAT;
  report->adr  = adr;
  /*----------- Recalculation size ------------*/
  if ( currentChart != adr )
  {
    currentChart   = adr;
    lastDotNumber  = 0U;
  }
  if ( lastDotNumber >= chart->size )
  {
    res = USB_DONE;
    currentChart = 0xFFU;
  }
  else
  {
    res = USB_CONT;
    length = charts[adr]->size - lastDotNumber;
    if ( length < USB_DATA_SIZE )
    {
      size = length;
    }
    else
    {
      size = length - USB_DATA_SIZE * ( uint8_t )( charts[adr]->size / USB_DATA_SIZE );
    }
  /*----------------- Dots --------------------*/
    for ( i=0U; i<size; i++ )
    {
      vUint32ToBytes( ( uint32_t* )( &chart->dots[lastDotNumber + i].x ), &report->data[count] );
      count += 4U;
      vUint32ToBytes( ( uint32_t* )( &chart->dots[lastDotNumber + i].y ), &report->data[count] );
      count += 4U;
    }
    lastDotNumber += size;
  /*-------------------------------------------*/
  }

  return res;
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
    for ( i=0U; i<configReg[report->adr]->len; i++ )
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
/*---------------------------------------------------------------------------------------------------*/
void vUSBParsingChart( uint16_t adr, const uint8_t* data )
{
  uint8_t counter = 0U;

  charts[adr]->xmin = *( fix16_t* )( &data[counter] );
  counter += 4U;
  charts[adr]->xmax = *( fix16_t* )( &data[counter] );
  counter += 4U;
  charts[adr]->ymin = *( fix16_t* )( &data[counter] );
  counter += 4U;
  charts[adr]->ymax = *( fix16_t* )( &data[counter] );
  counter += 4U;
  vDecodeURI( ( char* )( &data[counter] ), charts[adr]->xunit, CHART_UNIT_LENGTH );
  counter += CHART_UNIT_LENGTH * 6U;
  vDecodeURI( ( char* )( &data[counter] ), charts[adr]->yunit, CHART_UNIT_LENGTH );
  counter += CHART_UNIT_LENGTH * 6U;
  charts[adr]->size = *( uint16_t* )( &data[counter] );
  counter += 2U;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBParsingChartDots( uint16_t adr, uint8_t firstDot, uint8_t size, const uint8_t* data )
{
  uint8_t i       = 0U;
  uint8_t counter = 0U;
  for ( i=0; i<size; i++ )
  {
    charts[adr]->dots[i + firstDot].x = *( fix16_t* )( &data[counter] );
    counter += 4U;
    charts[adr]->dots[i + firstDot].y = *( fix16_t* )( &data[counter] );
    counter += 4U;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
USB_Status eUSBReportToChart( USB_REPORT* report )
{
  static uint8_t currentChart   = 0xFFU;
  static uint8_t lastDotNumber  = 0U;
  USB_Status res    = USB_DONE;
  uint8_t    length = 0U;
  uint8_t    size   = 0U;
  /*----------- Chart data control ------------*/
  if ( report->length != 0U )
  {
  /*------------- Length control --------------*/
    length = 22U + CHART_UNIT_LENGTH * 12U;
    if ( report->length < length )
    {
      res = USB_ERROR_LENGTH;
    }
    else {
  /*------------- Multi message ---------------*/
      if ( currentChart != report->adr )
      {
        currentChart  = report->adr;
        lastDotNumber = 0U;
        vUSBParsingChart( report->adr, report->data );
      }
      else
      {
    	length = charts[report->adr]->size - lastDotNumber;
    	if ( length < USB_DATA_SIZE )
    	{
    	  size = length;
    	}
    	else
    	{
          size = length - USB_DATA_SIZE * ( uint8_t )( charts[report->adr]->size / USB_DATA_SIZE );
    	}
        vUSBParsingChartDots( report->adr, lastDotNumber, size, report->data );
        lastDotNumber += size;
        if ( lastDotNumber >= charts[report->adr]->size )
        {
          currentChart = 0xFFU;
        }
      }
    }
  /*-------------------------------------------*/
  }
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
  for( i=0U; i<report->length; i++)
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
  report->adr    = ( ( ( uint16_t )( report->buf[3U] ) ) << 8U ) | ( ( uint16_t )( report->buf[4U] ) );
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
void vUSBsendChart( uint16_t adr, const eChartData* chart )
{
  uint8_t    i      = 0U;
  USB_Status result = USB_DONE;
  USB_REPORT report =
  {
    .buf  = usbBuffer,
    .data = &usbBuffer[BUFFER_DATA_SHIFT],
  };


  // Нет длинны сообщения

  vUSBChartToReport( adr, chart, &report );
  vUSBmakeReport( &report );
  while ( eUSBwrite( report.buf ) == USBD_BUSY )
  {
    osDelay( 2U );
  }
  for ( i=0U; i<(CHART_CHART_MAX_SIZE/USB_DATA_SIZE + 1U); i++ )
  {
    result =  vUSBChartDotsToReport( adr, chart, &report ) == USB_DONE;
    vUSBmakeReport( &report );
    while ( eUSBwrite( report.buf ) == USBD_BUSY )
    {
      osDelay( 2U );
    }
    if ( result == USB_DONE ) break;
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

  if ( report->adr == 59U )
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
    .cmd    = report->cmd,
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
        case USB_GET_CHART_CMD:
        	if ( report.adr == BROADCAST_ADR )
        	{
        	  for ( i=0U; i<CHART_NUMBER; i++ )
        	  {
        	    vUSBsendChart( i, charts[i] );
        	  }
        	}
        	else if ( report.adr < CHART_NUMBER )
        	{
        	  vUSBsendChart( report.adr, charts[report.adr] );
        	}
          break;
        case USB_PUT_CHART_CMD:
            if ( report.adr < CHART_NUMBER )
            {
              i = 0U;
              vUSBgetChart( &report );
            }
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


