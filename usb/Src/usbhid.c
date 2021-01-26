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
#include "RTC.h"
#include "dataAPI.h"
#include "controller.h"
#include "measurement.h"
/*----------------------- Structures ----------------------------------------------------------------*/
extern USBD_HandleTypeDef  hUsbDeviceFS;
/*----------------------- Constant ------------------------------------------------------------------*/
/*----------------------- Variables -----------------------------------------------------------------*/
static uint8_t      outputBuffer[USB_REPORT_SIZE]             = { 0U };
static uint8_t      inputBuffer[USB_REPORT_SIZE]              = { 0U };
static char         strBuffer[( MAX_UNITS_LENGTH * 6U ) + 1U] = { 0U };
static osThreadId_t usbHandle                                 = NULL;
static AUTH_STATUS  usbAuthorization                          = AUTH_VOID;
/*----------------------- Functions -----------------------------------------------------------------*/

/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
void vUint32ToBytes ( uint32_t input, uint8_t* output )
{
  output[0U] = ( uint8_t )( ( input ) );
  output[1U] = ( uint8_t )( ( input ) >> 8U );
  output[2U] = ( uint8_t )( ( input ) >> 16U );
  output[3U] = ( uint8_t )( ( input ) >> 24U );
  return;
}
void vUint16ToBytes ( uint16_t input, uint8_t* output )
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
  if ( BUILD_BUG_OR_ZERO( USBD_CUSTOMHID_OUTREPORT_BUF_SIZE - USB_REPORT_SIZE ) == 0U ) /* Test size of report */
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
USB_CONN_STATUS eUSBgetStatus ( void )
{
  USB_CONN_STATUS res = USB_DISCONNECT;

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
  return USBD_CUSTOM_HID_SendReport( &hUsbDeviceFS, data, USB_REPORT_SIZE );
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBtimeToReport ( USB_REPORT* report, uint16_t adr )
{
  RTC_TIME time;

  eRTCgetTime( &time );
  report->stat     = USB_REPORT_STATE_OK;
  report->adr      = 0U;
  report->length   = sizeof( RTC_TIME );
  report->data[0U] = ( uint8_t )time.hour;
  report->data[1U] = ( uint8_t )time.min;
  report->data[2U] = ( uint8_t )time.sec;
  report->data[3U] = ( uint8_t )time.year;
  report->data[4U] = ( uint8_t )time.month;
  report->data[5U] = ( uint8_t )time.day;
  report->data[6U] = ( uint8_t )time.wday;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBfreeDataToReport ( USB_REPORT* report, uint16_t adr )
{
  report->stat     = USB_REPORT_STATE_OK;
  report->length   = 2U;
  report->data[0U] = ( uint8_t )( *freeDataArray[adr]       );
  report->data[1U] = ( uint8_t )( *freeDataArray[adr] >> 8U );
  return;
}

void vUSBlogToReport ( USB_REPORT* report, uint16_t adr )
{
  LOG_RECORD_TYPE record = { 0U };
  DATA_API_STATUS status = DATA_API_STAT_BUSY;

  report->stat   = USB_REPORT_STATE_BAD_REQ;
  report->length = 6U;

  while ( status == DATA_API_STAT_BUSY )
  {
    status = eDATAAPIlog( DATA_API_CMD_LOAD, &adr, &record );
  }
  if ( status == DATA_API_STAT_OK )
  {
    report->stat     = USB_REPORT_STATE_OK;
    report->data[0U] = ( uint8_t )( record.time         );
    report->data[1U] = ( uint8_t )( record.time >> 8U   );
    report->data[2U] = ( uint8_t )( record.time >> 16U  );
    report->data[3U] = ( uint8_t )( record.time >> 24U  );
    report->data[4U] = ( uint8_t )( record.event.type   );
    report->data[5U] = ( uint8_t )( record.event.action );
  }
  else
  {
    report->stat   = USB_REPORT_STATE_INTERNAL;
    report->length = 0U;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Transfer configuration register to the report structure
 * input:  config  - input configuration register
 *         report  - output structure
 * output: status of operation
 */
void vUSBConfigToReport ( USB_REPORT* report, uint16_t adr )
{
  uint8_t i     = 0U;
  uint8_t count = 0U;
  uint8_t shift = 0U;

  if ( adr < SETTING_REGISTER_NUMBER )
  {
    report->stat = USB_REPORT_STATE_OK;
    shift        = uEncodeURI( configReg[adr]->units, MAX_UNITS_LENGTH, strBuffer );
    /*----------- Configuration value -----------*/
    for ( i=0U; i<configReg[adr]->atrib->len; i++ )
    {
      vUint16ToBytes( configReg[adr]->value[i], &report->data[count + ( 2U * i )] );
    }
    count += 2U * configReg[adr]->atrib->len;
    /*----------- Configuration scale -----------*/
    report->data[count] = ( uint8_t )( configReg[adr]->scale );
    count++;
    /*----------- Configuration units -----------*/
    for ( i=0U; i<shift; i++ )
    {
      report->data[count] = ( uint8_t )( strBuffer[i] );
      count++;
    }
    /*-------------------------------------------*/
    report->length = count;
  }
  else
  {
    report->stat = USB_REPORT_STATE_NON_CON;
  }
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
void vUSBChartToReport ( uint16_t adr, USB_REPORT* report )
{
  uint8_t i     = 0U;
  uint8_t count = 0U;
  uint8_t shift = 0U;
  /*-------------------------------------------*/
  report->cmd    = USB_REPORT_CMD_GET_CHART;
  report->dir    = USB_REPORT_DIR_OUTPUT;
  report->stat   = USB_REPORT_STATE_OK;
  report->adr    = adr;
  report->length = 18U + ( CHART_UNIT_LENGTH * 12U ) + ( 8U * charts[adr]->size );
  /*--------------- Chart data ----------------*/
  vUint32ToBytes( ( uint32_t )( charts[adr]->xmin ), &report->data[count] );
  count += 4U;
  vUint32ToBytes( ( uint32_t )( charts[adr]->xmax ), &report->data[count] );
  count += 4U;
  vUint32ToBytes( ( uint32_t )( charts[adr]->ymin ), &report->data[count] );
  count += 4U;
  vUint32ToBytes( ( uint32_t )( charts[adr]->ymax ), &report->data[count] );
  count += 4U;
  /*--------------- X unit --------------------*/
  shift = uEncodeURI( charts[adr]->xunit, CHART_UNIT_LENGTH, strBuffer );
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
  shift = uEncodeURI( charts[adr]->yunit, CHART_UNIT_LENGTH, strBuffer );
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
  vUint16ToBytes( charts[adr]->size, &report->data[count] );
  count += 2U;
  /*-------------------------------------------*/
  return;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBChartDotsToReport ( uint16_t adr, const eChartData* chart, USB_REPORT* report )
{
  USB_STATUS     res           = USB_STATUS_DONE;
  uint8_t        i             = 0U;
  uint8_t        count         = 0U;
  uint8_t        length        = 0U;
  uint8_t        size          = 0U;
  static uint8_t currentChart  = 0xFFU;
  static uint8_t lastDotNumber = 0U;
  /*-------------------------------------------*/
  report->cmd  = USB_REPORT_CMD_GET_CHART;
  report->dir  = USB_REPORT_DIR_OUTPUT;
  report->stat = USB_REPORT_STATE_OK;
  report->adr  = adr;
  /*----------- Recalculation size ------------*/
  if ( currentChart != adr )
  {
    currentChart   = adr;
    lastDotNumber  = 0U;
  }
  if ( lastDotNumber >= chart->size )
  {
    res          = USB_STATUS_DONE;
    currentChart = 0xFFU;
  }
  else
  {
    res    = USB_STATUS_CONT;
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
      vUint32ToBytes( ( uint32_t )( chart->dots[lastDotNumber + i].x ), &report->data[count] );
      count += 4U;
      vUint32ToBytes( ( uint32_t )( chart->dots[lastDotNumber + i].y ), &report->data[count] );
      count += 4U;
    }
    lastDotNumber += size;
  /*-------------------------------------------*/
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBReportToTime ( const USB_REPORT* report )
{
  USB_STATUS res  = USB_STATUS_DONE;
  RTC_TIME   time;
  /*------------- Length control --------------*/
  if ( report->length >= sizeof( RTC_TIME ) )
  {
    time.hour  = report->data[0U];
    time.min   = report->data[1U];
    time.sec   = report->data[2U];
    time.year  = report->data[3U];
    time.month = report->data[4U];
    time.day   = report->data[5U];
    time.wday  = report->data[6U];
    if ( vRTCsetTime( &time ) != RTC_OK )
    {
      res = USB_STATUS_STORAGE_ERROR;
    }
  }
  else
  {
    res = USB_STATUS_ERROR_LENGTH;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBReportToPassword ( const USB_REPORT* report )
{
  USB_STATUS      res    = USB_STATUS_DONE;
  PASSWORD_TYPE   pass   = { 0U };
  DATA_API_STATUS status = DATA_API_STAT_BUSY;
  if ( report->length >= PASSWORD_SIZE )
  {
    pass.status = report->data[0U];
    pass.data   = ( ( ( uint16_t )report->data[1U] )       ) |
                  ( ( ( uint16_t )report->data[2U] ) << 8U );
    while ( status == DATA_API_STAT_BUSY )
    {
      status = eDATAAPIpassword( DATA_API_CMD_WRITE, &pass );
    }
    if ( status == DATA_API_STAT_OK )
    {
      status = DATA_API_STAT_BUSY;
      while ( status == DATA_API_STAT_BUSY )
      {
        status = eDATAAPIpassword( DATA_API_CMD_SAVE, NULL );
      }
      if ( status != DATA_API_STAT_OK )
      {
        res = USB_STATUS_STORAGE_ERROR;
      }
      else
      {
        res = USB_STATUS_STORAGE_ERROR;
      }
    }
    else
    {
      res = USB_STATUS_STORAGE_ERROR;
    }
  }
  else
  {
    res = USB_STATUS_ERROR_LENGTH;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBcheckupPassword ( const USB_REPORT* report )
{
  USB_STATUS      res    = USB_STATUS_DONE;
  uint16_t        input  = 0U;
  PASSWORD_TYPE   pass   = { 0U };
  DATA_API_STATUS status = DATA_API_STAT_BUSY;
  if ( report->length >= 2U )
  {
    input = ( ( ( uint16_t )report->data[0U] )       ) |
            ( ( ( uint16_t )report->data[1U] ) << 8U );
    while ( status == DATA_API_STAT_BUSY )
    {
      status = eDATAAPIpassword( DATA_API_CMD_READ, &pass );
    }
    if ( status == DATA_API_STAT_OK )
    {
      if ( ( ( pass.status == PASSWORD_SET ) && ( pass.data == input ) ) || ( pass.status == PASSWORD_RESET ) )
      {
        res              = USB_STATUS_DONE;
        usbAuthorization = AUTH_DONE;
      }
      else
      {
        res = USB_STATUS_UNAUTHORIZED_ERROR;
      }
    }
    else
    {
      res = USB_STATUS_STORAGE_ERROR;
    }
  }
  else
  {
    res = USB_STATUS_ERROR_LENGTH;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBReportToFreeData ( const USB_REPORT* report )
{
  USB_STATUS      res    = USB_STATUS_DONE;
  uint16_t        value  = 0U;
  DATA_API_STATUS status = DATA_API_STAT_BUSY;
  if ( report->length >= 2U )
  {
    if ( report->adr <= FREE_DATA_SIZE )
    {
      value = ( ( ( uint16_t )report->data[0U] )       ) |
              ( ( ( uint16_t )report->data[1U] ) << 8U );
      while ( status == DATA_API_STAT_BUSY )
      {
        status = eDATAAPIfreeData( DATA_API_CMD_WRITE, report->adr, &value );
      }
      if ( status == DATA_API_STAT_OK )
      {
        status = DATA_API_STAT_BUSY;
        while ( status == DATA_API_STAT_BUSY )
        {
          status = eDATAAPIfreeData( DATA_API_CMD_SAVE, report->adr, NULL );
        }
        if ( status != DATA_API_STAT_OK )
        {
          res = USB_STATUS_STORAGE_ERROR;
        }
      }
      else
      {
        res = USB_STATUS_STORAGE_ERROR;
      }
    }
    else
    {
      res = USB_STATUS_ERROR_ADR;
    }
  }
  else
  {
    res = USB_STATUS_ERROR_LENGTH;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBReportToConfig ( const USB_REPORT* report )
{
  USB_STATUS      res                        = USB_STATUS_DONE;
  uint8_t         count                      = 0U;
  uint8_t         i                          = 0U;
  uint8_t         length                     = 0U;
  uint16_t        valueBuf[MAX_VALUE_LENGTH] = { 0U };
  int8_t          scale                      = 0U;
  uint16_t        units[MAX_UNITS_LENGTH]    = { 0U };
  DATA_API_STATUS status                     = DATA_API_STAT_BUSY;
  /*------------- Length control --------------*/
  if ( report->adr < SETTING_REGISTER_NUMBER )
  {
    length = ( configReg[report->adr]->atrib->len * 2U ) + 1U + ( MAX_UNITS_LENGTH * 6U );
    if ( length < report->length )
    {
      res = USB_STATUS_ERROR_LENGTH;
    }
    else {
    /*----------- Configuration value -----------*/
      for ( i=0U; i<configReg[report->adr]->atrib->len; i++ )
      {
        valueBuf[i] = ( ( uint16_t )( report->data[count + ( 2U * i ) + 1U] ) << 8U ) |
                      ( ( uint16_t )( report->data[count + ( 2U * i )] ) );
      }
      count += 2U * configReg[report->adr]->atrib->len;
    /*----------- Configuration scale -----------*/
      scale = ( int8_t )( report->data[count] );
      count++;
    /*----------- Configuration units -----------*/
      vDecodeURI( ( char* )( &report->data[count] ), units, MAX_UNITS_LENGTH );
      while ( status == DATA_API_STAT_BUSY )
      {
        status = eDATAAPIconfig( DATA_API_CMD_WRITE, report->adr, valueBuf, &scale, units );
      }
      if ( status != DATA_API_STAT_OK )
      {
        res = USB_STATUS_STORAGE_ERROR;
      }
    }
  }
  else
  {
    res = USB_STATUS_ERROR_ADR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBParsingChart ( uint16_t adr, const uint8_t* data )
{
  uint8_t counter = 0U;

  while ( xSemaphoreTake( xCHARTgetSemophore(), SEMAPHORE_TAKE_DELAY ) != pdTRUE )
  {

  }
  charts[adr]->xmin = *( fix16_t* )( &data[counter] );
  counter   += 4U;
  charts[adr]->xmax = *( fix16_t* )( &data[counter] );
  counter   += 4U;
  charts[adr]->ymin = *( fix16_t* )( &data[counter] );
  counter   += 4U;
  charts[adr]->ymax = *( fix16_t* )( &data[counter] );
  counter   += 4U;
  vDecodeURI( ( ( char* )( &data[counter] ) ), charts[adr]->xunit, CHART_UNIT_LENGTH );
  counter   += CHART_UNIT_LENGTH * 6U;
  vDecodeURI( ( ( char* )( &data[counter] ) ), charts[adr]->yunit, CHART_UNIT_LENGTH );
  counter   += CHART_UNIT_LENGTH * 6U;
  charts[adr]->size = ( uint16_t )( data[counter] ) | ( ( uint16_t )( data[counter + 1U] ) << 8U );
  counter   += 2U;
  xSemaphoreGive( xCHARTgetSemophore() );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBParsingChartDots ( uint16_t adr, uint8_t firstDot, uint8_t size, const uint8_t* data )
{
  uint8_t i       = 0U;
  uint8_t counter = 0U;
  for ( i=0U; i<size; i++ )
  {
    charts[adr]->dots[i + firstDot].x = *( fix16_t* )( &data[counter] );
    counter += 4U;
    charts[adr]->dots[i + firstDot].y = *( fix16_t* )( &data[counter] );
    counter += 4U;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBReportToChart ( const USB_REPORT* report )
{
  static uint8_t currentChart   = 0xFFU;
  static uint8_t lastDotNumber  = 0U;
  USB_STATUS     res            = USB_STATUS_DONE;
  uint8_t        length         = 0U;
  uint8_t        size           = 0U;

  if (report->adr < CHART_NUMBER )
  {
  /*----------- Chart data control ------------*/
    if ( report->length != 0U )
    {
  /*------------- Length control --------------*/
      length = 22U + ( CHART_UNIT_LENGTH * 12U );
      if ( report->length < length )
      {
        res = USB_STATUS_ERROR_LENGTH;
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
    }
    else
    {
      res = USB_STATUS_DONE;
    }
  /*-------------------------------------------*/
  }
  else
  {
    res = USB_STATUS_ERROR_ADR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBReportToEWA ( const USB_REPORT* report )
{
  USB_STATUS      res           = USB_STATUS_DONE;
  uint8_t         length        = 0U;
  static uint32_t index         = 0U;
  DATA_API_STATUS status        = DATA_API_STAT_BUSY;
  uint16_t        i             = 0;
  uint32_t        checkAdr      = 0U;
  uint8_t         checkData[4U] = { 0U };


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
      while ( status == DATA_API_STAT_BUSY )
      {
        status = eDATAAPIewa( DATA_API_CMD_SAVE, ( STORAGE_EWA_DATA_ADR + index ), report->data, length );
      }
      if ( status == DATA_API_STAT_OK )
      {
	      checkAdr = STORAGE_EWA_DATA_ADR + index;
	      for ( i=0U; i<length; i++ )
	      {
	        status = DATA_API_STAT_BUSY;
	        while ( status == DATA_API_STAT_BUSY )
	        {
	          status = eDATAAPIewa( DATA_API_CMD_LOAD, checkAdr, checkData, 1U );
	        }
	        if ( ( report->data[i] != checkData[0U] ) || ( status != DATA_API_STAT_OK ) )
	        {
	          res = USB_STATUS_STORAGE_ERROR;
	          break;
	        }
	        checkAdr++;
	      }
        if ( res != USB_STATUS_STORAGE_ERROR )
        {
          index += length;
          if ( index < report->length )
          {
            res = USB_STATUS_CONT;
          }
          else if ( index == report->length )
          {
            index  = 0U;
            status = DATA_API_STAT_BUSY;
            while ( status == DATA_API_STAT_BUSY )
            {
              status = eDATAAPIewa( DATA_API_CMD_SAVE, STORAGE_EWA_ADR, &( report->buf[USB_LEN2_BYTE] ), EEPROM_LENGTH_SIZE );
            }
            if ( status == DATA_API_STAT_OK )
            {
              res = USB_STATUS_DONE;
            }
            else
            {
              res = USB_STATUS_STORAGE_ERROR;
            }
          }
          else
          {
 	          index = 0U;
	          res   = USB_STATUS_ERROR_LENGTH;
          }
        }
      }
      else
      {
	      index = 0U;
	      res = USB_STATUS_STORAGE_ERROR;
      }
    }
    else
    {
      res = USB_STATUS_ERROR_LENGTH;
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
  report->buf[USB_ADR0_BYTE] = ( uint8_t )( report->adr );
  report->buf[USB_ADR1_BYTE] = ( uint8_t )( report->adr >> 8U );
  report->buf[USB_LEN0_BYTE] = ( uint8_t )( report->length );
  report->buf[USB_LEN1_BYTE] = ( uint8_t )( report->length >> 8U );
  report->buf[USB_LEN2_BYTE] = ( uint8_t )( report->length >> 16U );

  if ( report->length < USB_DATA_SIZE )
  {
    len = report->length;
  }
  for ( i=0U; i<len; i++ )
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
		               ( ( ( uint32_t )( report->buf[USB_LEN0_BYTE] ) )        );
  report->data   = &( report->buf[USB_DATA_BYTE] );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBsendReport ( USB_REPORT* report )
{
  vUSBmakeReport( report );
  while ( eUSBwrite( report->buf ) == USBD_BUSY )
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
void vUSBsendChart ( const USB_REPORT* request )
{
  uint8_t    i      = 0U;
  USB_STATUS result = USB_STATUS_DONE;
  USB_REPORT report =
  {
    .cmd  = request->cmd,
    .adr  = request->adr,
    .buf  = outputBuffer,
    .data = &outputBuffer[USB_DATA_BYTE],
  };
  if ( usbAuthorization == AUTH_DONE )
  {
    if ( report.adr < CHART_NUMBER )
    {
      vUSBChartToReport( request->adr, &report );
      vUSBsendReport( &report );
      for ( i=0U; i<USB_REPORT_SIZE; i++ )
      {
        outputBuffer[i]=0U;
      }
      for ( i=0U; i<( ( CHART_DOTS_SIZE / USB_DATA_SIZE ) + 1U ); i++ )
      {
        result =  eUSBChartDotsToReport( request->adr, charts[request->adr], &report ) == USB_STATUS_DONE;
        vUSBsendReport( &report );
        if ( result == USB_STATUS_DONE )
        {
          break;
        }
      }
    }
  }
  else
  {
    report.stat = USB_REPORT_STATE_UNAUTHORIZED;
    for ( i=0U; i<USB_REPORT_SIZE; i++ )
    {
      outputBuffer[i] = 0U;
    }
    vUSBsendReport( &report );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBsaveConfigs ( const USB_REPORT* report )
{
  USB_STATUS      res    = USB_STATUS_DONE;
  DATA_API_STATUS status = DATA_API_STAT_BUSY;
  while ( status == DATA_API_STAT_BUSY )
  {
    status = eDATAAPIconfigValue( DATA_API_CMD_SAVE, 0U, NULL );
  }
  if ( status != DATA_API_STAT_OK )
  {
    res = USB_STATUS_STORAGE_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBsaveCharts ( const USB_REPORT* report )
{
  USB_STATUS      res    = USB_STATUS_DONE;
  DATA_API_STATUS status = DATA_API_STAT_BUSY;
  while ( status == DATA_API_STAT_BUSY )
  {
    status = eDATAAPIchart( DATA_API_CMD_SAVE, 0U, NULL );
  }
  if ( status != DATA_API_STAT_OK )
  {
    res = USB_STATUS_STORAGE_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBeraseLOG ( const USB_REPORT* report )
{
  USB_STATUS      res    = USB_STATUS_DONE;
  DATA_API_STATUS status = DATA_API_STAT_BUSY;
  while ( status == DATA_API_STAT_BUSY )
  {
    status = eDATAAPIlog( DATA_API_CMD_ERASE, NULL, NULL );
  }
  if ( status != DATA_API_STAT_OK )
  {
    res = USB_STATUS_STORAGE_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBerasePassword( const USB_REPORT* report )
{
  USB_STATUS      res    = USB_STATUS_DONE;
  DATA_API_STATUS status = DATA_API_STAT_BUSY;
  while ( status == DATA_API_STAT_BUSY )
  {
    status = eDATAAPIpassword( DATA_API_CMD_ERASE, NULL );
  }
  if ( status != DATA_API_STAT_OK )
  {
    res = USB_STATUS_STORAGE_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBmemorySizeToReport ( USB_REPORT* report, uint16_t adr )
{
  report->stat = USB_REPORT_STATE_NON_CON;
  if ( MEASUREMENT_ENB > 0U )
  {
    report->stat     = USB_REPORT_STATE_OK;
    report->length   = 4U;
    report->data[0U] = ( uint8_t )( STORAGE_MEASUREMENT_SIZE         );
    report->data[1U] = ( uint8_t )( STORAGE_MEASUREMENT_SIZE >> 8U   );
    report->data[2U] = ( uint8_t )( STORAGE_MEASUREMENT_SIZE >> 16U  );
    report->data[3U] = ( uint8_t )( STORAGE_MEASUREMENT_SIZE >> 24U  );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void eUSBmeasurementToReport ( USB_REPORT* report, uint16_t adr )
{
  uint8_t         i                                = 0U;
  uint16_t        data[MEASUREMENT_CHANNEL_NUMBER] = { 0U };
  DATA_API_STATUS status                           = DATA_API_STAT_BUSY;
  report->stat = USB_REPORT_STATE_NON_CON;
  if ( MEASUREMENT_ENB > 0U )
  {
    report->length = uMEASUREMENTgetSize() * 2U;
    while ( status == DATA_API_STAT_BUSY )
    {
      status = eDATAAPImeasurement( DATA_API_CMD_READ, &adr, uMEASUREMENTgetSize(), data );
    }
    if ( status == DATA_API_STAT_OK )
    {
      report->stat = USB_REPORT_STATE_OK;
      for ( i=0U; i<uMEASUREMENTgetSize(); i++ )
      {
        report->data[0U + i * 2U] = ( uint8_t )( data[i]       );
        report->data[1U + i * 2U] = ( uint8_t )( data[i] >> 8U );
      }
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void eUSBmeasurementLengthToReport ( USB_REPORT* report, uint16_t adr )
{
  DATA_API_STATUS status = DATA_API_STAT_BUSY;
  uint16_t        data   = 0U;
  report->stat   = USB_REPORT_STATE_NON_CON;
  report->length = 0U;
  if ( MEASUREMENT_ENB > 0U )
  {
    while ( status == DATA_API_STAT_BUSY )
    {
      status = eDATAAPImeasurement( DATA_API_CMD_COUNTER, &data, uMEASUREMENTgetSize(), NULL );
    }
    if ( status == DATA_API_STAT_OK )
    {
      report->stat     = USB_REPORT_STATE_OK;
      report->length   = 2U;
      report->data[0U] = ( uint8_t )( data         );
      report->data[1U] = ( uint8_t )( data >> 8U   );
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBeraseMeasurement ( const USB_REPORT* report )
{
  USB_STATUS      res    = USB_STATUS_FORBIDDEN;
  DATA_API_STATUS status = DATA_API_STAT_BUSY;
  if ( MEASUREMENT_ENB > 0U )
  {
    while ( status == DATA_API_STAT_BUSY )
    {
      status = eDATAAPImeasurement( DATA_API_CMD_ERASE, NULL, 0U, NULL );
    }
    if ( status != DATA_API_STAT_OK )
    {
      res = USB_STATUS_STORAGE_ERROR;
    }
    else
    {
      res = USB_STATUS_DONE;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBsend ( USB_REPORT* request, void ( *callback )( USB_REPORT*, uint16_t ) )
{
  uint16_t   i      = 0U;
  USB_REPORT report =
  {
    .dir  = USB_REPORT_DIR_OUTPUT,
    .cmd  = request->cmd,
    .adr  = request->adr,
    .buf  = outputBuffer,
    .data = &outputBuffer[USB_DATA_BYTE],
  };
  if ( usbAuthorization == AUTH_DONE )
  {
    callback( &report, request->adr );
    vUSBmakeReport( &report );
  }
  else
  {
    report.stat = USB_REPORT_STATE_UNAUTHORIZED;
    for ( i=0U; i<USB_REPORT_SIZE; i++ )
    {
      outputBuffer[i] = 0U;
    }
  }
  vUSBsendReport( &report );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBget ( USB_REPORT* report, USB_STATUS ( *callback )( const USB_REPORT* ) )
{
  uint16_t   i        = 0U;
  USB_STATUS res      = USB_STATUS_DONE;
  USB_REPORT response =
  {
    .cmd    = report->cmd,
    .stat   = USB_REPORT_STATE_OK,
    .adr    = report->adr,
    .length = 0U,
    .buf    = outputBuffer,
    .data   = &outputBuffer[USB_DATA_BYTE],
  };

  if ( ( usbAuthorization == AUTH_DONE ) || ( report->cmd == USB_REPORT_CMD_AUTHORIZATION ) )
  {
    if ( ( eCONTROLLERgetMode()   == CONTROLLER_MODE_MANUAL  ) ||
         ( eCONTROLLERgetStatus() == CONTROLLER_STATUS_IDLE  ) ||
         ( eCONTROLLERgetStatus() == CONTROLLER_STATUS_ALARM ) )
    {
      res = callback( report );
    }
    else
    {
      res = USB_STATUS_ENGINE_NON_STOP;
    }
  }
  else
  {
    res = USB_STATUS_UNAUTHORIZED_ERROR;
  }
  switch ( res )
  {
    case USB_STATUS_DONE:
      response.stat = USB_REPORT_STATE_OK;
      break;
    case USB_STATUS_CONT:
      response.stat = USB_REPORT_STATE_OK;
      break;
    case USB_STATUS_ERROR_LENGTH:
      response.stat = USB_REPORT_STATE_BAD_REQ;
      break;
    case USB_STATUS_ERROR_ADR:
      response.stat = USB_REPORT_STATE_NON_CON;
      break;
    case USB_STATUS_STORAGE_ERROR:
      response.stat = USB_REPORT_STATE_INTERNAL;
      break;
    case USB_STATUS_UNAUTHORIZED_ERROR:
      response.stat = USB_REPORT_STATE_UNAUTHORIZED;
      break;
    case USB_STATUS_ENGINE_NON_STOP:
      response.stat = USB_REPORT_STATE_FORBIDDEN;
      break;
    default:
      response.stat = USB_REPORT_STATE_BAD_REQ;
      break;
  }
  for ( i=0U; i<USB_REPORT_SIZE; i++ )
  {
    outputBuffer[i] = 0U;
  }
  vUSBsendReport( &response );
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
void vUSBreceiveHandler ( void )
{
  USBD_CUSTOM_HID_HandleTypeDef* hhid  = ( USBD_CUSTOM_HID_HandleTypeDef* )hUsbDeviceFS.pClassData;
  uint8_t                        i     = 0U;
  BaseType_t                     yield = pdFALSE;

  /* Copy input buffer to local buffer */
  for ( i=0U; i<USB_REPORT_SIZE; i++ )
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
void vUSBplugHandler ( void )
{
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBunplugHandler ( void )
{
  usbAuthorization = AUTH_VOID;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Processing USB input task
 * input:  none
 * output: none
 */
void vStartUsbTask ( void *argument )
{
  USB_REPORT report = {.buf = inputBuffer};
  for(;;)
  {
    if ( ulTaskNotifyTake( pdTRUE, portMAX_DELAY ) > 0U )
    {
      vUSBparseReport( &report );
      switch( report.cmd )
      {
        case USB_REPORT_CMD_GET_CONFIG:
          vUSBsend( &report, vUSBConfigToReport );
          break;
        case USB_REPORT_CMD_PUT_CONFIG:
          vUSBget( &report, eUSBReportToConfig );
          break;
        case USB_REPORT_CMD_GET_CHART:
          vUSBsendChart( &report );
          break;
        case USB_REPORT_CMD_PUT_CHART:
          vUSBget( &report, eUSBReportToChart );
          break;
        case USB_REPORT_CMD_PUT_EWA:
          vUSBget( &report, eUSBReportToEWA );
          break;
        case USB_REPORT_CMD_SAVE_CONFIG:
          vUSBget( &report, eUSBsaveConfigs );
          break;
        case USB_REPORT_CMD_SAVE_CHART:
          vUSBget( &report, eUSBsaveCharts );
          break;
        case USB_REPORT_CMD_GET_TIME:
          vUSBsend( &report, vUSBtimeToReport );
          break;
        case USB_REPORT_CMD_PUT_TIME:
          vUSBget( &report, eUSBReportToTime );
          break;
        case USB_REPORT_CMD_GET_FREE_DATA:             // PC->DEVICE
          vUSBsend( &report, vUSBfreeDataToReport );
          break;
        case USB_REPORT_CMD_PUT_FREE_DATA:
          vUSBget( &report, eUSBReportToFreeData );
          break;
        case USB_REPORT_CMD_GET_LOG:
          vUSBsend( &report, vUSBlogToReport );
          break;
        case USB_REPORT_CMD_ERASE_LOG:
          vUSBget( &report, eUSBeraseLOG );
          break;
        case USB_REPORT_CMD_PUT_PASSWORD:
          vUSBget( &report, eUSBReportToPassword );
          break;
        case USB_REPORT_CMD_AUTHORIZATION:
          vUSBget( &report, eUSBcheckupPassword );
          break;
        case USB_REPORT_CMD_ERASE_PASSWORD:
          vUSBget( &report, eUSBerasePassword );
          break;
        case USB_REPORT_CMD_GET_MEMORY_SIZE:
          vUSBsend( &report, vUSBmemorySizeToReport );
          break;
        case USB_REPORT_CMD_GET_MEASUREMENT:
          vUSBsend( &report, eUSBmeasurementToReport );
          break;
        case USB_REPORT_CMD_ERASE_MEASUREMENT:
          vUSBget( &report, eUSBeraseMeasurement );
          break;
        case USB_REPORT_CMD_GET_MEASUREMENT_LENGTH:
          vUSBsend( &report, eUSBmeasurementLengthToReport );
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
