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
#include "EEPROM.h"
#include "storage.h"
/*----------------------- Structures ----------------------------------------------------------------*/
extern USBD_HandleTypeDef  hUsbDeviceFS;
/*----------------------- Constant ------------------------------------------------------------------*/
/*----------------------- Variables -----------------------------------------------------------------*/
static uint8_t      usbBuffer[USB_REPORT_SIZE];
static uint8_t      inputBuffer[USB_REPORT_SIZE];
static char         strBuffer[( MAX_UNITS_LENGTH * 6U ) + 1U];
static osThreadId_t usbHandle;
/*----------------------- Functions -----------------------------------------------------------------*/
void               vUint32ToBytes ( uint32_t input, uint8_t* output );
void               vUint16ToBytes ( uint16_t input, uint8_t* output );
void               vUSBmakeReport ( USB_REPORT* report );                          /* Convert report structure to the output buffer */
void               vUSBparseReport ( USB_REPORT* report );                         /* Parse input buffer to the report structure */
void               vUSBresetControl ( USB_ConfigControl* control );                /* Clean control structure */
void               vUSBConfigToReport ( const eConfigReg* config, USB_REPORT* report );  /* Transfer configuration register to the report structure */
void               vUSBChartToReport ( uint16_t adr, const eChartData* chart, USB_REPORT* report );
USB_Status         vUSBChartDotsToReport ( uint16_t adr, const eChartData* chart, USB_REPORT* report );
USB_Status         eUSBReportToConfig ( USB_REPORT* report );
USBD_StatusTypeDef eUSBwrite ( uint8_t* data );                                    /* Send data via USB */
void               vUSBsendConfig ( eConfigReg* config );                          /* Send configuration register via USB */
void               vUSBsendChart ( uint16_t adr, const eChartData* chart );        /* Send chart data via USB */
void               vUSBgetConfig ( USB_REPORT* report );                           /* Get configuration register via USB */
void               vUSBgetChart ( USB_REPORT* report );                            /* Get chart data via USB */
void               vUSBgetEWA ( USB_REPORT* report );
void               vUSBParsingChart ( uint16_t adr, const uint8_t* data );
void               vUSBParsingChartDots ( uint16_t adr, uint8_t firstDot, uint8_t size, const uint8_t* data );
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void vUint32ToBytes ( uint32_t input, uint8_t* output )
{
  output[0U] = ( uint8_t )( ( input & 0x000000FFU ) );
  output[1U] = ( uint8_t )( ( input & 0x0000FF00U ) >> 8U );
  output[2U] = ( uint8_t )( ( input & 0x00FF0000U ) >> 16U );
  output[3U] = ( uint8_t )( ( input & 0xFF000000U ) >> 24U );
  return;
}
void vUint16ToBytes (  uint16_t input, uint8_t* output )
{
  output[0U] = ( uint8_t )( input );
  output[1U] = ( uint8_t )( input >> 8U );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Initialization device and control USB size of report descriptor
 * input:  task for handling input USB data
 * output: none
 */
void vUSBinit ( osThreadId_t taskHandle )
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
USB_StatusConnect eUSBgetStatus ( void )
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
USBD_StatusTypeDef eUSBwrite ( uint8_t* data )
{
  return USBD_CUSTOM_HID_SendReport( &hUsbDeviceFS, data, USB_REPORT_SIZE );;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Clean control structure
 * input:  control structure
 * output: none
 */
void vUSBresetControl ( USB_ConfigControl* control )
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
void vUSBConfigToReport ( const eConfigReg* config, USB_REPORT* report )
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
    report->data[count + ( 2U * i )]      = ( uint8_t )( ( config->value[i] >> 8U ) & 0x00FFU );
    report->data[count + ( 2U * i ) + 1U] = ( uint8_t )( config->value[i] & 0x00FFU );
  }
  count += 2U * config->len;
  /*----------- Configuration scale -----------*/
  report->data[count] = ( uint8_t )( config->scale );
  count++;
  /*----------- Configuration units -----------*/
  for( i=0U; i<shift; i++ )
  {
    report->data[count] = ( uint8_t )( strBuffer[i] );
    count++;
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
void vUSBChartToReport ( uint16_t adr, const eChartData* chart, USB_REPORT* report )
{
  uint8_t i     = 0U;
  uint8_t count = 0U;
  uint8_t shift = 0U;
  /*-------------------------------------------*/
  report->cmd    = USB_GET_CHART_CMD;
  report->dir    = USB_OUTPUT;
  report->stat   = USB_OK_STAT;
  report->adr    = adr;
  report->length = 18U + ( CHART_UNIT_LENGTH * 12U ) + ( 8U * chart->size );
  /*--------------- Chart data ----------------*/
  vUint32ToBytes( ( uint32_t )( &chart->xmin ), &report->data[count] );
  count += 4U;
  vUint32ToBytes( ( uint32_t )( &chart->xmax ), &report->data[count] );
  count += 4U;
  vUint32ToBytes( ( uint32_t )( &chart->ymin ), &report->data[count] );
  count += 4U;
  vUint32ToBytes( ( uint32_t )( &chart->ymax ), &report->data[count] );
  count += 4U;
  /*--------------- X unit --------------------*/
  shift = uEncodeURI( chart->xunit, CHART_UNIT_LENGTH, strBuffer );
  for ( i=0U; i<shift; i++ )
  {
    report->data[count] = ( uint8_t )( strBuffer[i] );
    count++;
  }
  for ( i=shift; i<( CHART_UNIT_LENGTH * 6U ); i++ )
  {
    report->data[count] = 0U;
    count++;
  }
  /*--------------- Y unit --------------------*/
  shift = uEncodeURI( chart->yunit, CHART_UNIT_LENGTH, strBuffer );
  for ( i=0U; i<shift; i++ )
  {
    report->data[count] = ( uint8_t )( strBuffer[i] );
    count++;
  }
  for ( i=shift; i<( CHART_UNIT_LENGTH * 6U ); i++ )
  {
    report->data[count] = 0U;
    count++;
  }
  /*----------------- Size --------------------*/
  vUint16ToBytes( chart->size, &report->data[count] );
  count += 2U;
  /*-------------------------------------------*/
  return;
}
/*---------------------------------------------------------------------------------------------------*/
USB_Status vUSBChartDotsToReport ( uint16_t adr, const eChartData* chart, USB_REPORT* report )
{
  USB_Status     res           = USB_DONE;
  uint8_t        i             = 0U;
  uint8_t        count         = 0U;
  uint8_t        length        = 0U;
  uint8_t        size          = 0U;
  static uint8_t currentChart  = 0xFFU;
  static uint8_t lastDotNumber = 0U;
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
    res          = USB_DONE;
    currentChart = 0xFFU;
  }
  else
  {
    res    = USB_CONT;
    length = charts[adr]->size - lastDotNumber;
    if ( length < USB_DATA_SIZE )
    {
      size = length;
    }
    else
    {
      size = length - ( USB_DATA_SIZE * ( uint8_t )( charts[adr]->size / USB_DATA_SIZE ) );
    }
  /*----------------- Dots --------------------*/
    for ( i=0U; i<size; i++ )
    {
      vUint32ToBytes( ( uint32_t )( &chart->dots[lastDotNumber + i].x ), &report->data[count] );
      count += 4U;
      vUint32ToBytes( ( uint32_t )( &chart->dots[lastDotNumber + i].y ), &report->data[count] );
      count += 4U;
    }
    lastDotNumber += size;
  /*-------------------------------------------*/
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_Status eUSBReportToConfig ( USB_REPORT* report )
{
  USB_Status res    = USB_DONE;
  uint8_t    count  = 0U;
  uint8_t    i      = 0U;
  uint8_t    length = 0U;
  /*------------- Length control --------------*/
  length = ( configReg[report->adr]->len * 2U ) + 1U + ( MAX_UNITS_LENGTH * 6U );
  if ( length < report->length ) {
  res = USB_ERROR_LENGTH;
  }
  else {
  /*----------- Configuration value -----------*/
    for ( i=0U; i<configReg[report->adr]->len; i++ )
    {
      configReg[report->adr]->value[i] = ( uint16_t )( report->data[count + ( 2U * i ) + 1U] ) | ( ( uint16_t )( report->data[count + ( 2U * i )] ) << 8U );
    }
    count += 2U * configReg[report->adr]->len;
  /*----------- Configuration scale -----------*/
    configReg[report->adr]->scale = ( signed char )( report->data[count] );
    count++;
  /*----------- Configuration units -----------*/
    vDecodeURI( ( char* )( &report->data[count] ), configReg[report->adr]->units, MAX_UNITS_LENGTH );
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBParsingChart( uint16_t adr, const uint8_t* data )
{
  uint8_t counter = 0U;
  char    chr     = NULL;
  charts[adr]->xmin = *( fix16_t* )( &data[counter] );
  counter += 4U;
  charts[adr]->xmax = *( fix16_t* )( &data[counter] );
  counter += 4U;
  charts[adr]->ymin = *( fix16_t* )( &data[counter] );
  counter += 4U;
  charts[adr]->ymax = *( fix16_t* )( &data[counter] );
  counter += 4U;
  chr = ( char )( data[counter] );
  vDecodeURI( &chr, charts[adr]->xunit, CHART_UNIT_LENGTH );
  counter += CHART_UNIT_LENGTH * 6U;
  chr = ( char )( data[counter] );
  vDecodeURI( &chr, charts[adr]->yunit, CHART_UNIT_LENGTH );
  counter += CHART_UNIT_LENGTH * 6U;
  charts[adr]->size = ( uint16_t )( data[counter] ) | ( ( uint16_t )( data[counter + 1U] ) << 8 );
  counter += 2U;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBParsingChartDots ( uint16_t adr, uint8_t firstDot, uint8_t size, const uint8_t* data )
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
USB_Status eUSBReportToChart ( USB_REPORT* report )
{
  static uint8_t currentChart   = 0xFFU;
  static uint8_t lastDotNumber  = 0U;
  USB_Status     res            = USB_DONE;
  uint8_t        length         = 0U;
  uint8_t        size           = 0U;
  /*----------- Chart data control ------------*/
  if ( report->length != 0U )
  {
  /*------------- Length control --------------*/
    length = 22U + ( CHART_UNIT_LENGTH * 12U );
    if ( report->length < length )
    {
      res = USB_ERROR_LENGTH;
    }
    /*------------- Multi message ---------------*/
    else
    {
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
          size = length - ( USB_DATA_SIZE * ( uint8_t )( charts[report->adr]->size / USB_DATA_SIZE ) );
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
USB_Status eUSBReportToEWA ( USB_REPORT* report )
{
  USB_Status      res    = USB_DONE;
  uint8_t         length = 0U;
  static uint32_t index  = 0U;
  uint32_t        adr    = 0U;
  EEPROM_STATUS   status = EEPROM_OK;

  if ( report->length > 0U )
  {
    if ( ( ( report->length - index ) / USB_DATA_SIZE ) > 0U )
    {
      length = USB_DATA_SIZE;
    }
    else if ( ( ( report->length - index ) % USB_DATA_SIZE ) > 0U )
    {
      length = ( uint8_t )( report->length - index );
    }
    else
    {
      length = 0U;
    }
    if ( length > 0U )
    {
      adr    = STORAGE_EWA_ADR + index ;
      status = eEEPROMWriteMemory( &adr, report->data, length );
      if ( status == EEPROM_OK )
      {
	uint8_t temp[length];
	uint8_t i =0;
	for(i=0;i<length;i++){temp[i]=0;}
	status = eEEPROMReadMemory( &adr, temp, length );
	if ( status == EEPROM_OK )
	{
	  for ( i=0;i<length;i++)
	  {
	      uint8_t a = report->data[i];
	      uint8_t b = temp[i];
	    if (report->data[i] != temp[i])
	      {
		i=2;
	      }
	  }
	} else {
	    i=1;
	}


        index += length;
        if ( index < report->length )
        {
          res = USB_CONT;
        }
        else if ( index == report->length )
        {
          index = 0U;
          res   = USB_DONE;
        }
        else
        {
 	  index = 0U;
	  res   = USB_ERROR_LENGTH;
        }
      }
      else
      {
	index = 0U;
	res = USB_STORAGE_ERROR;
      }
    }
    else
    {
      res = USB_ERROR_LENGTH;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Convert report structure to the output buffer
 * input:  report structure
 * output: none
 */
void vUSBmakeReport ( USB_REPORT* report )
{
  uint8_t i   = 0U;
  uint8_t len = USB_DATA_SIZE;
  report->buf[USB_DIR_BYTE]  = USB_GET_DIR_VAL( report->dir );
  report->buf[USB_CMD_BYTE]  = report->cmd;
  report->buf[USB_STAT_BYTE] = report->stat;
  report->buf[USB_ADR1_BYTE] = ( uint8_t )( report->adr >> 8U );
  report->buf[USB_ADR0_BYTE] = ( uint8_t )( report->adr );
  report->buf[USB_LEN2_BYTE] = ( uint8_t )( report->length >> 16U );
  report->buf[USB_LEN1_BYTE] = ( uint8_t )( report->length >> 8U );
  report->buf[USB_LEN0_BYTE] = ( uint8_t )( report->length );
  if ( report->length < USB_DATA_SIZE )
  {
    len = report->length;
  }
  for( i=0U; i<len; i++)
  {
    report->buf[USB_DATA_BYTE + i] = report->data[i];
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Parse input buffer to the report structure
 * input:  report structure
 * output: none
 */
void vUSBparseReport ( USB_REPORT* report )
{
  report->dir    = USB_GET_DIR( report->buf[USB_DIR_BYTE] );
  report->cmd    = report->buf[USB_CMD_BYTE];
  report->stat   = report->buf[USB_STAT_BYTE];
  report->adr    = ( ( ( uint16_t )( report->buf[USB_ADR1_BYTE] ) ) << 8U ) |
                     ( ( uint16_t )( report->buf[USB_ADR0_BYTE] ) );
  report->length = ( ( ( uint32_t )( report->buf[USB_LEN2_BYTE] ) ) << 16U ) |
                   ( ( ( uint32_t )( report->buf[USB_LEN1_BYTE] ) ) <<  8U ) |
		     ( ( uint32_t )( report->buf[USB_LEN0_BYTE] ) );
  report->data   = &( report->buf[USB_DATA_BYTE] );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Send configuration register via USB
 * input:  configuration register
 * output: none
 */
void vUSBsendConfig ( eConfigReg* config )
{
  USB_REPORT report =
  {
    .buf  = usbBuffer,
    .data = &usbBuffer[USB_DATA_BYTE],
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
void vUSBsendChart ( uint16_t adr, const eChartData* chart )
{
  uint8_t    i      = 0U;
  USB_Status result = USB_DONE;
  USB_REPORT report =
  {
    .buf  = usbBuffer,
    .data = &usbBuffer[USB_DATA_BYTE],
  };


  // Нет длинны сообщения

  vUSBChartToReport( adr, chart, &report );
  vUSBmakeReport( &report );
  while ( eUSBwrite( report.buf ) == USBD_BUSY )
  {
    osDelay( 2U );
  }
  for ( i=0U; i<( ( CHART_CHART_MAX_SIZE / USB_DATA_SIZE ) + 1U); i++ )
  {
    result =  vUSBChartDotsToReport( adr, chart, &report ) == USB_DONE;
    vUSBmakeReport( &report );
    while ( eUSBwrite( report.buf ) == USBD_BUSY )
    {
      osDelay( 2U );
    }
    if ( result == USB_DONE )
    {
      break;
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Get configuration register via USB
 * input:  report of transaction
 * output: none
 */
void vUSBgetConfig ( USB_REPORT* report )
{
  uint8_t    i        = 0U;
  USB_REPORT response =
  {
    .cmd    = USB_PUT_CONFIG_CMD,
    .stat   = USB_OK_STAT,
    .adr    = report->adr,
    .length = 0U,
    .buf    = usbBuffer,
    .data   = &usbBuffer[USB_DATA_BYTE],
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
void vUSBgetChart ( USB_REPORT* report )
{
  uint8_t    i        = 0U;
  USB_REPORT response =
  {
    .cmd    = report->cmd,
    .stat   = USB_OK_STAT,
    .adr    = report->adr,
    .length = 0U,
    .buf    = usbBuffer,
    .data   = &usbBuffer[USB_DATA_BYTE],
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
/*
 * Get EWA via USB
 * input:  report of transaction
 * output: none
 */
void vUSBgetEWA ( USB_REPORT* report )
{
  uint8_t    i        = 0U;
  USB_Status status   = USB_DONE;
  USB_REPORT response =
  {
    .cmd    = report->cmd,
    .stat   = USB_OK_STAT,
    .adr    = report->adr,
    .length = report->length,
    .buf    = usbBuffer,
    .data   = &usbBuffer[USB_DATA_BYTE],
  };

  status = eUSBReportToEWA( report );
  if ( ( status != USB_DONE ) && ( status != USB_CONT ) )
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
void StartUsbTask ( void *argument )
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
          else
          {
            break;
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
          else
          {
            break;
          }
          break;
        case USB_PUT_CHART_CMD:
            if ( report.adr < CHART_NUMBER )
            {
              i = 0U;
              vUSBgetChart( &report );
            }
          break;
        case USB_PUT_EWA_CMD:
          vUSBgetEWA( &report );
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



