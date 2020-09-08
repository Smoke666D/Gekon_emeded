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
/*----------------------- Structures ----------------------------------------------------------------*/
extern USBD_HandleTypeDef  hUsbDeviceFS;
/*----------------------- Constant ------------------------------------------------------------------*/
/*----------------------- Variables -----------------------------------------------------------------*/
static uint8_t              usbBuffer[USB_REPORT_SIZE];
static uint8_t              inputBuffer[USB_REPORT_SIZE];
static char                 strBuffer[( MAX_UNITS_LENGTH * 6U ) + 1U];
static osThreadId_t         usbHandle;
static AUTHORIZATION_STATUS usbAuthorization = AUTHORIZATION_VOID;
/*----------------------- Functions -----------------------------------------------------------------*/
void               vUint32ToBytes ( uint32_t input, uint8_t* output );
void               vUint16ToBytes ( uint16_t input, uint8_t* output );
void               vUSBmakeReport ( USB_REPORT* report );                          /* Convert report structure to the output buffer */
void               vUSBparseReport ( USB_REPORT* report );                         /* Parse input buffer to the report structure */
void               vUSBresetControl ( USB_ConfigControl* control );                /* Clean control structure */
void               vUSBConfigToReport ( const eConfigReg* config, USB_REPORT* report );  /* Transfer configuration register to the report structure */
void               vUSBChartToReport ( uint16_t adr, const eChartData* chart, USB_REPORT* report );
USB_STATUS         vUSBChartDotsToReport ( uint16_t adr, const eChartData* chart, USB_REPORT* report );
USB_STATUS         eUSBReportToConfig ( USB_REPORT* report );
USBD_StatusTypeDef eUSBwrite ( uint8_t* data );                                    /* Send data via USB */
void               vUSBsendChart ( uint16_t adr, const eChartData* chart );        /* Send chart data via USB */
void               vUSBParsingChart ( uint16_t adr, const uint8_t* data );
void               vUSBParsingChartDots ( uint16_t adr, uint8_t firstDot, uint8_t size, const uint8_t* data );


void vUSBget ( USB_REPORT* report, USB_STATUS ( *callback )( USB_REPORT* ) );
void vUSBsend ( void* data, void ( *callback )( USB_REPORT*, void* ) );
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
void vUSBtimeToReport ( const RTC_TIME* time, USB_REPORT* report )
{
  report->cmd      = USB_GET_TIME;
  report->dir      = USB_OUTPUT;
  report->stat     = USB_OK_STAT;
  report->adr      = 0U;
  report->length   = sizeof( RTC_TIME );
  report->data[0U] = ( uint8_t )time->hour;
  report->data[1U] = ( uint8_t )time->min;
  report->data[2U] = ( uint8_t )time->sec;
  report->data[3U] = ( uint8_t )time->year;
  report->data[4U] = ( uint8_t )time->month;
  report->data[5U] = ( uint8_t )time->day;
  report->data[6U] = ( uint8_t )time->wday;
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBfreeDataToReport ( uint16_t adr, USB_REPORT* report )
{
  report->cmd      = USB_GET_FREE_DATA;
  report->dir      = USB_OUTPUT;
  report->stat     = USB_OK_STAT;
  report->adr      = adr;
  report->length   = 2U;
  report->data[0U] = ( uint8_t )( *freeDataArray[adr] << 8U );
  report->data[1U] = ( uint8_t )( *freeDataArray[adr] );
  return;
}

void vUSBlogToReport ( uint16_t adr, USB_REPORT* report )
{
  LOG_RECORD_TYPE record;

  report->cmd      = USB_GET_LOG;
  report->dir      = USB_OUTPUT;
  report->stat     = USB_BAD_REQ_STAT;
  report->adr      = adr;
  report->length   = 6U;
  if ( eDATAAPIlog( DATA_API_CMD_LOAD, adr, &record ) == DATA_API_STAT_OK )
  {
    report->stat     = USB_OK_STAT;
    report->data[0U] = ( uint8_t )( record.time >> 24U );
    report->data[1U] = ( uint8_t )( record.time >> 16U );
    report->data[2U] = ( uint8_t )( record.time >> 8U );
    report->data[3U] = ( uint8_t )( record.time );
    report->data[4U] = ( uint8_t )( record.event.type );
    report->data[5U] = ( uint8_t )( record.event.action );
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
void vUSBConfigToReport ( const eConfigReg* config, USB_REPORT* report )
{
  uint8_t i     = 0U;
  uint8_t count = 0U;
  uint8_t shift = uEncodeURI( config->units, MAX_UNITS_LENGTH, strBuffer );

  report->cmd  = USB_GET_CONFIG_CMD;
  report->dir  = USB_OUTPUT;
  report->stat = USB_OK_STAT;
  report->adr  = config->atrib->adr;
  /*----------- Configuration value -----------*/
  for ( i=0U; i<config->atrib->len; i++ )
  {
    report->data[count + ( 2U * i )]      = ( uint8_t )( ( config->value[i] >> 8U ) & 0x00FFU );
    report->data[count + ( 2U * i ) + 1U] = ( uint8_t )( config->value[i] & 0x00FFU );
  }
  count += 2U * config->atrib->len;
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
  vUint32ToBytes( ( uint32_t )( chart->xmin ), &report->data[count] );
  count += 4U;
  vUint32ToBytes( ( uint32_t )( chart->xmax ), &report->data[count] );
  count += 4U;
  vUint32ToBytes( ( uint32_t )( chart->ymin ), &report->data[count] );
  count += 4U;
  vUint32ToBytes( ( uint32_t )( chart->ymax ), &report->data[count] );
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
USB_STATUS vUSBChartDotsToReport ( uint16_t adr, const eChartData* chart, USB_REPORT* report )
{
  USB_STATUS     res           = USB_DONE;
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
USB_STATUS eUSBReportToTime ( USB_REPORT* report )
{
  USB_STATUS res  = USB_DONE;
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
      res = USB_STORAGE_ERROR;
    }
  }
  else
  {
    res = USB_ERROR_LENGTH;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBReportToPassword ( USB_REPORT* report )
{
  USB_STATUS    res  = USB_DONE;
  PASSWORD_TYPE pass =
  {
    .data   = 0U,
    .status = PASSWORD_RESET,
  };
  if ( report->length >= PASSWORD_SIZE )
  {
    pass.status = report->data[0U];
    pass.data   = ( ( ( uint16_t )report->data[1U] ) << 8U ) | ( uint16_t )report->data[2U];
    if ( eDATAAPIpassword( DATA_API_CMD_WRITE, &pass ) == DATA_API_STAT_OK )
    {
      if ( eDATAAPIpassword( DATA_API_CMD_SAVE, NULL ) != DATA_API_STAT_OK )
      {
        res = USB_STORAGE_ERROR;
      }
    }
    else
    {
      res = USB_STORAGE_ERROR;
    }
  }
  else
  {
    res = USB_ERROR_LENGTH;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBcheckupPassword ( USB_REPORT* report )
{
  USB_STATUS    res   = USB_DONE;
  uint16_t      input = 0U;
  PASSWORD_TYPE pass  =
  {
    .data   = 0U,
    .status = PASSWORD_RESET,
  };

  if ( report->length >= 2U )
  {
    input = ( ( ( uint16_t )report->data[0U] ) << 8U ) | ( ( uint16_t )report->data[1U] );
    if ( eDATAAPIpassword( DATA_API_CMD_READ, &pass ) == DATA_API_STAT_OK )
    {
      if ( ( ( pass.status == PASSWORD_SET ) && ( pass.data == input ) ) || ( pass.status == PASSWORD_RESET ) )
      {
        res              = USB_DONE;
        usbAuthorization = AUTHORIZATION_DONE;
      }
      else
      {
        res = USB_UNAUTHORIZED_ERROR;
      }
    }
    else
    {
      res = USB_STORAGE_ERROR;
    }
  }
  else
  {
    res = USB_ERROR_LENGTH;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBReportToFreeData ( USB_REPORT* report )
{
  USB_STATUS res   = USB_DONE;
  uint16_t   value = 0U;
  if ( report->length >= 2U )
  {
    if ( report->adr <= FREE_DATA_SIZE )
    {
      value = ( ( ( uint16_t )report->data[0U] ) << 8U ) | ( ( uint16_t )report->data[1U] );
      if ( eDATAAPIfreeData( DATA_API_CMD_WRITE, report->adr, &value ) == DATA_API_STAT_OK )
      {
        if ( eDATAAPIfreeData( DATA_API_CMD_SAVE,  report->adr, NULL ) != DATA_API_STAT_OK )
        {
          res = USB_STORAGE_ERROR;
        }
      }
      else
      {
        res = USB_STORAGE_ERROR;
      }
    }
    else
    {
      res = USB_ERROR_ADR;
    }
  }
  else
  {
    res = USB_ERROR_LENGTH;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBReportToConfig ( USB_REPORT* report )
{
  USB_STATUS    res    = USB_DONE;
  uint8_t       count  = 0U;
  uint8_t       i      = 0U;
  uint8_t       length = 0U;
  uint16_t      valueBuf[MAX_VALUE_LENGTH];
  signed char   scale  = 0U;
  uint16_t      units[MAX_UNITS_LENGTH];
  eConfigBitMap bitMap[MAX_BIT_MAP_LENGTH];
  /*------------- Length control --------------*/
  if ( report->adr < SETTING_REGISTER_NUMBER )
  {
    length = ( configReg[report->adr]->atrib->len * 2U ) + 1U + ( MAX_UNITS_LENGTH * 6U );
    if ( length < report->length )
    {
      res = USB_ERROR_LENGTH;
    }
    else {
    /*----------- Configuration value -----------*/
      for ( i=0U; i<configReg[report->adr]->atrib->len; i++ )
      {
        valueBuf[i] = ( uint16_t )( report->data[count + ( 2U * i ) + 1U] ) | ( ( uint16_t )( report->data[count + ( 2U * i )] ) << 8U );
      }
      count += 2U * configReg[report->adr]->atrib->len;
    /*----------- Configuration scale -----------*/
      scale = ( signed char )( report->data[count] );
      count++;
    /*----------- Configuration units -----------*/
      vDecodeURI( ( char* )( &report->data[count] ), units, MAX_UNITS_LENGTH );
      if ( eDATAAPIconfig( DATA_API_CMD_WRITE, report->adr, valueBuf, &scale, units, bitMap ) != DATA_API_STAT_OK )
      {
        res = USB_STORAGE_ERROR;
      }
    }
  }
  else
  {
    res = USB_ERROR_ADR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBParsingChart( uint16_t adr, const uint8_t* data )
{
  uint8_t    counter = 0U;
  eChartData chart;

  chart.xmin = *( fix16_t* )( &data[counter] );
  counter   += 4U;
  chart.xmax = *( fix16_t* )( &data[counter] );
  counter   += 4U;
  chart.ymin = *( fix16_t* )( &data[counter] );
  counter   += 4U;
  chart.ymax = *( fix16_t* )( &data[counter] );
  counter   += 4U;
  vDecodeURI( ( ( char* )( &data[counter] ) ), chart.xunit, CHART_UNIT_LENGTH );
  counter   += CHART_UNIT_LENGTH * 6U;
  vDecodeURI( ( ( char* )( &data[counter] ) ), chart.yunit, CHART_UNIT_LENGTH );
  counter   += CHART_UNIT_LENGTH * 6U;
  chart.size = ( uint16_t )( data[counter] ) | ( ( uint16_t )( data[counter + 1U] ) << 8U );
  counter   += 2U;
  eDATAAPIchart( DATA_API_CMD_WRITE, adr, &chart );
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
USB_STATUS eUSBReportToChart ( USB_REPORT* report )
{
  static uint8_t currentChart   = 0xFFU;
  static uint8_t lastDotNumber  = 0U;
  USB_STATUS     res            = USB_DONE;
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
    }
    else
    {
      res = USB_ERROR_LENGTH;
    }
  /*-------------------------------------------*/
  }
  else
  {
    res = USB_ERROR_ADR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBReportToEWA ( USB_REPORT* report )
{
  USB_STATUS      res           = USB_DONE;
  uint8_t         length        = 0U;
  static uint32_t index         = 0U;
  DATA_API_STATUS status        = DATA_API_STAT_OK;
  uint16_t        i             = 0;
  uint32_t        checkAdr      = 0U;
  uint8_t         checkData[4U] = {0U};


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
      status = eDATAAPIewa( DATA_API_CMD_SAVE, ( STORAGE_EWA_DATA_ADR + index ), report->data, length );
      if ( status == DATA_API_STAT_OK )
      {
	      checkAdr = STORAGE_EWA_DATA_ADR + index;
	      for ( i=0; i<length; i++ )
	      {
	        status = eDATAAPIewa( DATA_API_CMD_LOAD, checkAdr, checkData, 1U );
	        if ( ( report->data[i] != checkData[0U] ) || ( status != DATA_API_STAT_OK ) )
	        {
	          res = USB_STORAGE_ERROR;
	          break;
	        }
	        checkAdr++;
	      }
        if ( res != USB_STORAGE_ERROR )
        {
          index += length;
          if ( index < report->length )
          {
            res = USB_CONT;
          }
          else if ( index == report->length )
          {
            index  = 0U;
            status = eDATAAPIewa( DATA_API_CMD_SAVE, STORAGE_EWA_ADR, &( report->buf[USB_LEN2_BYTE] ), EEPROM_LENGTH_SIZE );
            if ( status == DATA_API_STAT_OK )
            {
              res = USB_DONE;
            }
            else
            {
              res = USB_STORAGE_ERROR;
            }
          }
          else
          {
 	          index = 0U;
	          res   = USB_ERROR_LENGTH;
          }
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
void vUSBsendConfig ( USB_REPORT* request, eConfigReg* config )
{
  uint16_t   i      = 0U;
  USB_REPORT report =
  {
    .cmd  = request->cmd,
    .adr  = request->adr,
    .buf  = usbBuffer,
    .data = &usbBuffer[USB_DATA_BYTE],
  };
  if ( usbAuthorization == AUTHORIZATION_DONE )
  {
    if ( report.adr < SETTING_REGISTER_NUMBER )
    {
      vUSBConfigToReport( config, &report );
      vUSBmakeReport( &report );
      while ( eUSBwrite( report.buf ) == USBD_BUSY )
      {
        osDelay( 2U );
      }
    }
  }
  else
  {
    report.stat = USB_STAT_UNAUTHORIZED;
    for ( i=0U; i<USB_REPORT_SIZE; i++ )
    {
      usbBuffer[i] = 0U;
    }
    vUSBmakeReport( &report );
    while ( eUSBwrite( report.buf ) == USBD_BUSY )
    {
      osDelay( 2U );
    }
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
  USB_STATUS result = USB_DONE;
  USB_REPORT report =
  {
    .buf  = usbBuffer,
    .data = &usbBuffer[USB_DATA_BYTE],
  };
  if ( usbAuthorization == AUTHORIZATION_DONE )
  {
    if ( report.adr < CHART_NUMBER )
    {
      vUSBChartToReport( adr, chart, &report );
      vUSBmakeReport( &report );
      while ( eUSBwrite( report.buf ) == USBD_BUSY )
      {
        osDelay( 2U );
      }
      for ( i=0U; i<USB_REPORT_SIZE; i++ )
      {
        usbBuffer[i]=0U;
      }
      for ( i=0U; i<( ( CHART_DOTS_SIZE / USB_DATA_SIZE ) + 1U); i++ )
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
    }
  }
  else
  {
    report.stat = USB_STAT_UNAUTHORIZED;
    for ( i=0U; i<USB_REPORT_SIZE; i++ )
    {
      usbBuffer[i] = 0U;
    }
    vUSBmakeReport( &report );
    while ( eUSBwrite( report.buf ) == USBD_BUSY )
    {
      osDelay( 2U );
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBsendTime ()
{
  RTC_TIME   time;
  uint16_t   i      = 0U;
  USB_REPORT report =
  {
    .buf  = usbBuffer,
    .data = &usbBuffer[USB_DATA_BYTE],
  };
  if ( usbAuthorization == AUTHORIZATION_DONE )
  {
    eRTCgetTime( &time );
    vUSBtimeToReport( &time, &report );
  }
  else
  {
    report.stat = USB_STAT_UNAUTHORIZED;
    for ( i=0U; i<USB_REPORT_SIZE; i++ )
    {
      usbBuffer[i] = 0U;
    }
  }
  vUSBmakeReport( &report );
  while ( eUSBwrite( report.buf ) == USBD_BUSY )
  {
    osDelay( 2U );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBsendFreeData ( uint16_t adr )
{
  uint16_t   i      = 0U;
  USB_REPORT report =
  {
    .buf  = usbBuffer,
    .data = &usbBuffer[USB_DATA_BYTE],
  };
  if ( usbAuthorization == AUTHORIZATION_DONE )
  {
    vUSBfreeDataToReport( adr, &report );
  }
  else
  {
    report.stat = USB_STAT_UNAUTHORIZED;
    for ( i=0U; i<USB_REPORT_SIZE; i++ )
    {
      usbBuffer[i] = 0U;
    }
  }
  vUSBmakeReport( &report );
  while ( eUSBwrite( report.buf ) == USBD_BUSY )
  {
    osDelay( 2U );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBsendLog( uint16_t adr )
{
  uint16_t   i      = 0U;
  USB_REPORT report =
  {
    .buf  = usbBuffer,
    .data = &usbBuffer[USB_DATA_BYTE],
  };
  if ( usbAuthorization == AUTHORIZATION_DONE )
  {
    vUSBlogToReport( adr, &report );
  }
  else
  {
    report.stat = USB_STAT_UNAUTHORIZED;
    for ( i=0U; i<USB_REPORT_SIZE; i++ )
    {
      usbBuffer[i] = 0U;
    }
  }
  vUSBmakeReport( &report );
  while ( eUSBwrite( report.buf ) == USBD_BUSY )
  {
    osDelay( 2U );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS vUSBsaveConfigs ( USB_REPORT* report )
{
  USB_STATUS res = USB_DONE;
  if ( eDATAAPIconfigValue( DATA_API_CMD_SAVE, 0U, NULL ) != DATA_API_STAT_OK )
  {
    res = USB_STORAGE_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS vUSBsaveCharts ( USB_REPORT* report )
{
  USB_STATUS res = USB_DONE;
  if ( eDATAAPIchart( DATA_API_CMD_SAVE, 0U, NULL ) != DATA_API_STAT_OK )
  {
    res = USB_STORAGE_ERROR;
  }
  return res;
}
USB_STATUS vUSBeraseLOG ( USB_REPORT* report )
{
  USB_STATUS res = USB_DONE;
  if ( eDATAAPIlog( DATA_API_CMD_ERASE, 0U, NULL ) != DATA_API_STAT_OK )
  {
    res = USB_STORAGE_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS vUSBerasePassword( USB_REPORT* report )
{
  USB_STATUS res = USB_DONE;
  if ( eDATAAPIpassword( DATA_API_CMD_ERASE, NULL ) != DATA_API_STAT_OK )
  {
    res = USB_STORAGE_ERROR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBsend ( void* data, void ( *callback )( USB_REPORT*, void* ) )
{
  USB_REPORT report =
  {
    .buf  = usbBuffer,
    .data = &usbBuffer[USB_DATA_BYTE],
  };
  if ( report.adr < SETTING_REGISTER_NUMBER )
  {
    callback( &report, data );
    vUSBmakeReport( &report );
    while ( eUSBwrite( report.buf ) == USBD_BUSY )
    {
      osDelay( 2U );
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBget ( USB_REPORT* report, USB_STATUS ( *callback )( USB_REPORT* ) )
{
  uint16_t   i        = 0U;
  USB_STATUS res      = USB_DONE;
  USB_REPORT response =
  {
    .cmd    = report->cmd,
    .stat   = USB_OK_STAT,
    .adr    = report->adr,
    .length = 0U,
    .buf    = usbBuffer,
    .data   = &usbBuffer[USB_DATA_BYTE],
  };
  if ( ( usbAuthorization == AUTHORIZATION_DONE ) || ( report->cmd == USB_AUTHORIZATION ) )
  {
    res = callback( report );
  }
  else
  {
    res = USB_STAT_UNAUTHORIZED;
  }
  switch ( res )
  {
    case USB_DONE:
      response.stat = USB_OK_STAT;
      break;
    case USB_CONT:
      response.stat = USB_OK_STAT;
      break;
    case USB_ERROR_LENGTH:
      response.stat = USB_BAD_REQ_STAT;
      break;
    case USB_ERROR_ADR:
      response.stat = USB_BAD_REQ_STAT;
      break;
    case USB_STORAGE_ERROR:
      response.stat = USB_NON_CON_STAT;
      break;
    case USB_UNAUTHORIZED_ERROR:
      response.stat = USB_STAT_UNAUTHORIZED;
      break;
    default:
      response.stat = USB_BAD_REQ_STAT;
      break;
  }
  for ( i=0U; i<USB_REPORT_SIZE; i++ )
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
  usbAuthorization = AUTHORIZATION_VOID;
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
  uint16_t   i      = 0U;
  for(;;)
  {
    if ( ulTaskNotifyTake( pdTRUE, portMAX_DELAY ) > 0U )
    {
      vUSBparseReport( &report );
      switch( report.cmd )
      {
        case USB_GET_CONFIG_CMD:
          vUSBsendConfig( &report, configReg[report.adr] );
          break;
        case USB_PUT_CONFIG_CMD:
          vUSBget( &report, eUSBReportToConfig );
          break;
        case USB_GET_CHART_CMD:
          if ( report.adr == BROADCAST_ADR )
          {
            for ( i=0U; i<CHART_NUMBER; i++ )
            {
              vUSBsendChart( i, charts[i] );
            }
          }
          else
          {
            vUSBsendChart( report.adr, charts[report.adr] );
          }
          break;
        case USB_PUT_CHART_CMD:
          vUSBget( &report, eUSBReportToChart );
          break;
        case USB_PUT_EWA_CMD:
          vUSBget( &report, eUSBReportToEWA );
          break;
        case USB_SAVE_CONFIG_CMD:
          vUSBget( &report, vUSBsaveConfigs );
          break;
        case USB_SAVE_CHART_CMD:
          vUSBget( &report, vUSBsaveCharts );
          break;
        case USB_GET_TIME:
          vUSBsendTime();
          break;
        case USB_PUT_TIME:
          vUSBget( &report, eUSBReportToTime );
          break;
        case USB_GET_FREE_DATA:             // PC->DEVICE
          vUSBsendFreeData( report.adr );
          break;
        case USB_PUT_FREE_DATA:
          vUSBget( &report, eUSBReportToFreeData );
          break;
        case USB_GET_LOG:
          vUSBsendLog( report.adr );
          break;
        case USB_ERASE_LOG:
          vUSBget( &report, vUSBeraseLOG );
          break;
        case USB_PUT_PASSWORD:
          vUSBget( &report, eUSBReportToPassword );
          break;
        case USB_AUTHORIZATION:
          vUSBget( &report, eUSBcheckupPassword );
          break;
        case USB_ERASE_PASSWORD:
          vUSBget( &report, vUSBerasePassword );
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



