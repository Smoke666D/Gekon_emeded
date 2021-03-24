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
#include "config.h"
#include "dataProces.h"
#include "OutputData.h"
#include "outputProcessing.h"
/*----------------------- Structures ----------------------------------------------------------------*/
extern USBD_HandleTypeDef  hUsbDeviceFS;
/*----------------------- Constant ------------------------------------------------------------------*/
/*----------------------- Variables -----------------------------------------------------------------*/
static uint8_t      outputBuffer[USB_REPORT_SIZE] = { 0U };
static uint8_t      inputBuffer[USB_REPORT_SIZE]  = { 0U };
static osThreadId_t usbHandle                     = NULL;
static AUTH_STATUS  usbAuthorization              = AUTH_VOID;
/*----------------------- Functions -----------------------------------------------------------------*/
void     vUint32ToBytes ( uint32_t input, uint8_t* output );
void     vUint16ToBytes ( uint16_t input, uint8_t* output );
uint16_t uBytesToUnit16 ( const uint8_t* data );
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
/*---------------------------------------------------------------------------------------------------*/
void vUint24ToBytes ( uint32_t input, uint8_t* output )
{
  output[0U] = ( uint8_t )( ( input ) );
  output[1U] = ( uint8_t )( ( input ) >> 8U );
  output[2U] = ( uint8_t )( ( input ) >> 16U );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUint16ToBytes ( uint16_t input, uint8_t* output )
{
  output[0U] = ( uint8_t )( input );
  output[1U] = ( uint8_t )( input >> 8U );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
uint16_t uBytesToUnit16 ( const uint8_t* data )
{
  return ( ( uint16_t )data[0U] ) | ( ( ( uint16_t )data[1U] ) << 8U );
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uByteToUint24 ( const uint8_t* data )
{
  return ( ( ( uint32_t )( data[2U] ) ) << 16U ) |
         ( ( ( uint32_t )( data[1U] ) ) <<  8U ) |
           ( ( uint32_t )( data[0U] ) );
}
/*---------------------------------------------------------------------------------------------------*/
uint32_t uByteToUint32 ( const uint8_t* data )
{
  return ( ( ( uint32_t )( data[3U] ) ) << 24U ) |
         ( ( ( uint32_t )( data[2U] ) ) << 16U ) |
         ( ( ( uint32_t )( data[1U] ) ) <<  8U ) |
           ( ( uint32_t )( data[0U] ) );
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
void vUSBtimeToReport ( USB_REPORT* report )
{
  RTC_TIME time;
  eRTCgetTime( &time );
  report->stat     = USB_REPORT_STATE_OK;
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
void vUSBfreeDataToReport ( USB_REPORT* report )
{
  report->stat     = USB_REPORT_STATE_OK;
  report->length   = 2U;
  vUint16ToBytes( *freeDataArray[report->adr], report->data );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBlogToReport ( USB_REPORT* report )
{
  LOG_RECORD_TYPE record = { 0U };
  DATA_API_STATUS status = DATA_API_STAT_BUSY;
  report->stat   = USB_REPORT_STATE_BAD_REQ;
  report->length = 6U;
  while ( status == DATA_API_STAT_BUSY )
  {
    status = eDATAAPIlog( DATA_API_CMD_LOAD, &report->adr, &record );
  }
  if ( status == DATA_API_STAT_OK )
  {
    report->stat     = USB_REPORT_STATE_OK;
    vUint32ToBytes( record.time, report->data );
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
void vUSBconfigToReport ( USB_REPORT* report )
{
  uint8_t i     = 0U;
  if ( report->adr < SETTING_REGISTER_NUMBER )
  {
    for ( i=0U; i<configReg[report->adr]->atrib->len; i++ )
    {
      vUint16ToBytes( configReg[report->adr]->value[i], &report->data[ 2U * i ] );
    }
    report->length = 2U * configReg[report->adr]->atrib->len;
    report->stat   = USB_REPORT_STATE_OK;
  }
  else
  {
    report->length = 0U;
    report->stat   = USB_REPORT_STATE_NON_CON;
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
void vUSBchartToReport ( USB_REPORT* report )
{
  uint8_t chartAdr = 0U;
  report->dir      = USB_REPORT_DIR_OUTPUT;
  report->stat     = USB_REPORT_STATE_OK;
  switch ( report->cmd )
  {
    case USB_REPORT_CMD_GET_CHART_OIL:
      chartAdr = OIL_CHART_ADR;
      break;
    case USB_REPORT_CMD_GET_CHART_COOLANT:
      chartAdr = COOLANT_CHART_ADR;
      break;
    case USB_REPORT_CMD_GET_CHART_FUEL:
      chartAdr = FUEL_CHART_ADR;
      break;
    default:
      break;
  }
  if ( report->adr == 0U )
  {
    report->length   = 2U;
    vUint16ToBytes( charts[chartAdr]->size, &report->data[0U] );
  }
  else if ( ( report->adr - 1U ) <= CHART_DOTS_SIZE )
  {
    report->length = 8U;
    vUint32ToBytes( ( uint32_t )( charts[chartAdr]->dots[report->adr - 1U].x ), &report->data[0U] );
    vUint32ToBytes( ( uint32_t )( charts[chartAdr]->dots[report->adr - 1U].y ), &report->data[4U] );
  }
  else
  {
    report->stat = USB_STATUS_ERROR_ADR;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBreportToTime ( const USB_REPORT* report )
{
  USB_STATUS res  = USB_STATUS_DONE;
  RTC_TIME   time;
  /*------------- Length control --------------*/
  if ( report->length == sizeof( RTC_TIME ) )
  {
    time.hour  = report->data[0U];
    time.min   = report->data[1U];
    time.sec   = report->data[2U];
    time.year  = report->data[3U];
    time.month = report->data[4U];
    time.day   = report->data[5U];
    time.wday  = report->data[6U];
    if ( eRTCsetTime( &time ) != RTC_OK )
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
USB_STATUS eUSBreportToPassword ( const USB_REPORT* report )
{
  USB_STATUS      res    = USB_STATUS_DONE;
  PASSWORD_TYPE   pass   = { 0U };
  DATA_API_STATUS status = DATA_API_STAT_BUSY;
  uint16_t        buf    = getUintValue( configReg[MODULE_SETUP_ADR] );
  if ( report->length == PASSWORD_SIZE )
  {
    pass.status = report->data[0U];
    pass.data   = uBytesToUnit16( &report->data[1U] );
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
      if ( status == DATA_API_STAT_OK )
      {
        if ( pass.status == PASSWORD_SET )
        {
          buf |= moduleSetup.atrib->bitMap[PASSWORD_ENB_ADR].mask;
        }
        else
        {
          buf &= ~moduleSetup.atrib->bitMap[PASSWORD_ENB_ADR].mask;
        }
        status = eDATAAPIconfigValue( DATA_API_CMD_WRITE, MODULE_SETUP_ADR, &buf );
        if ( status == DATA_API_STAT_OK )
        {
          status = eDATAAPIconfigValue( DATA_API_CMD_SAVE, 0U, NULL );
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
  if ( report->length == 2U )
  {
    input = uBytesToUnit16( &report->data[0U] );
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
USB_STATUS eUSBreportToFreeData ( const USB_REPORT* report )
{
  USB_STATUS      res    = USB_STATUS_DONE;
  uint16_t        value  = 0U;
  DATA_API_STATUS status = DATA_API_STAT_BUSY;
  if ( report->length == 2U )
  {
    if ( report->adr <= FREE_DATA_SIZE )
    {
      value = uBytesToUnit16( &report->data[0U] );
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
USB_STATUS eUSBreportToConfig ( const USB_REPORT* report )
{
  USB_STATUS      res                        = USB_STATUS_DONE;
  uint8_t         count                      = 0U;
  uint8_t         i                          = 0U;
  uint8_t         length                     = 0U;
  uint16_t        valueBuf[MAX_VALUE_LENGTH] = { 0U };
  DATA_API_STATUS status                     = DATA_API_STAT_BUSY;
  /*------------- Length control --------------*/
  if ( report->adr < SETTING_REGISTER_NUMBER )
  {
    length = configReg[report->adr]->atrib->len * 2U;
    if ( length == report->length )
    {
      /*----------- Configuration value -----------*/
      for ( i=0U; i<configReg[report->adr]->atrib->len; i++ )
      {
        valueBuf[i] = uBytesToUnit16( &report->data[count + ( 2U * i )]  );
      }
      count += 2U * configReg[report->adr]->atrib->len;
      while ( status == DATA_API_STAT_BUSY )
      {
        status = eDATAAPIconfig( DATA_API_CMD_WRITE, report->adr, valueBuf, NULL, NULL );
      }
      if ( status != DATA_API_STAT_OK )
      {
        res = USB_STATUS_STORAGE_ERROR;
      }
    }
    else
    {
      res = USB_STATUS_ERROR_LENGTH;
    }

  }
  else
  {
    res = USB_STATUS_ERROR_ADR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBreportToChart ( const USB_REPORT* report )
{
  uint8_t    chartAdr = 0U;
  USB_STATUS res      = USB_STATUS_DONE;
  switch ( report->cmd )
  {
    case USB_REPORT_CMD_PUT_CHART_OIL:
      chartAdr = OIL_CHART_ADR;
      break;
    case USB_REPORT_CMD_PUT_CHART_COOLANT:
      chartAdr = COOLANT_CHART_ADR;
      break;
    case USB_REPORT_CMD_PUT_CHART_FUEL:
      chartAdr = FUEL_CHART_ADR;
      break;
    default:
      break;
  }
  if ( report->adr == 0U )
  {
    if ( report->length == 2U )
    {
      while ( xSemaphoreTake( xCHARTgetSemophore(), SEMAPHORE_TAKE_DELAY ) != pdTRUE )
      {

      }
      charts[chartAdr]->size  = uBytesToUnit16( &report->data[0U] );
      xSemaphoreGive( xCHARTgetSemophore() );
    }
    else
    {
      res = USB_STATUS_ERROR_LENGTH;
    }
  }
  else if ( ( report->adr ) <= CHART_DOTS_SIZE )
  {
    if ( report->length == 8U )
    {
      while ( xSemaphoreTake( xCHARTgetSemophore(), SEMAPHORE_TAKE_DELAY ) != pdTRUE )
      {

      }
      charts[chartAdr]->dots[report->adr - 1U].x = *( fix16_t* )( &report->data[0U] );
      charts[chartAdr]->dots[report->adr - 1U].y = *( fix16_t* )( &report->data[4U] );
      xSemaphoreGive( xCHARTgetSemophore() );
    }
    else
    {
      res = USB_STATUS_ERROR_LENGTH;
    }
  }
  else
  {
    res = USB_STATUS_ERROR_ADR;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBreportToEWA ( const USB_REPORT* report )
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
    if ( ( report->length - index ) >= USB_DATA_SIZE )
    {
      length = USB_DATA_SIZE;
    }
    else
    {
      length = ( uint8_t )( report->length - index );
    }
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
  report->adr    = uBytesToUnit16( &report->buf[USB_ADR0_BYTE] );
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
void vUSBmemorySizeToReport ( USB_REPORT* report )
{
  report->stat = USB_REPORT_STATE_NON_CON;
  if ( MEASUREMENT_ENB > 0U )
  {
    report->stat  = USB_REPORT_STATE_OK;
    report->length = 4U;
    vUint32ToBytes( STORAGE_MEASUREMENT_SIZE, report->data );
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBmeasurementToReport ( USB_REPORT* report )
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
      status = eDATAAPImeasurement( DATA_API_CMD_READ, &report->adr, uMEASUREMENTgetSize(), data );
    }
    if ( status == DATA_API_STAT_OK )
    {
      report->stat = USB_REPORT_STATE_OK;
      for ( i=0U; i<uMEASUREMENTgetSize(); i++ )
      {
        vUint16ToBytes( data[i], &report->data[i * 2U] );
      }
    }
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBmeasurementLengthToReport ( USB_REPORT* report )
{
  DATA_API_STATUS status = DATA_API_STAT_BUSY;
  uint16_t        data   = 0U;
  report->stat   = USB_REPORT_STATE_NON_CON;
  report->length = 0U;
  if ( MEASUREMENT_ENB > 0U )
  {
    while ( status == DATA_API_STAT_BUSY )
    {
      status = eDATAAPImeasurement( DATA_API_CMD_COUNTER, &data, 0U, NULL );
    }
    if ( status == DATA_API_STAT_OK )
    {
      report->stat   = USB_REPORT_STATE_OK;
      report->length = 2U;
      vUint16ToBytes( data, report->data );
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
void vUSBoutputToReport ( USB_REPORT* report )
{
  if ( report->adr < OUTPUT_DATA_REGISTER_NUMBER )
  {
    vOUTPUTupdate( report->adr );
    vUint16ToBytes( outputDataReg[report->adr]->value[0U], &report->data[0U] );
    report->length = 2U;
    report->stat   = USB_REPORT_STATE_OK;
  }
  else
  {
    report->length = 0U;
    report->stat   = USB_REPORT_STATE_NON_CON;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
USB_STATUS eUSBreportToOutput ( const USB_REPORT* report )
{
  USB_STATUS    res  = USB_STATUS_DONE;
  OUTPUT_STATUS stat = OUTPUT_STATUS_OK;
  stat = vOUTPUTwrite( report->adr, *( uint16_t* )report->data );
  switch ( stat )
  {
    case OUTPUT_STATUS_OK:
      res = USB_STATUS_DONE;
      vOUTPUTupdate( report->adr );
      break;
    case OUTPUT_STATUS_BUSY:
      res = USB_STATUS_FORBIDDEN;
      break;
    case OUTPUT_STATUS_INIT_ERROR:
      res = USB_STATUS_STORAGE_ERROR;
      break;
    case OUTPUT_STATUS_ACCESS_DENIED:
      res = USB_STATUS_ERROR_ADR;
      break;
    default:
      break;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
void vUSBsend ( USB_REPORT* request, void ( *callback )( USB_REPORT* ) )
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
    callback( &report );
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
         ( eCONTROLLERgetStatus() == CONTROLLER_STATUS_ERROR ) )
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
          vUSBsend( &report, vUSBconfigToReport );
          break;
        case USB_REPORT_CMD_PUT_CONFIG:
          vUSBget( &report, eUSBreportToConfig );
          break;
        case USB_REPORT_CMD_GET_CHART_OIL:
          vUSBsend( &report, vUSBchartToReport );
          break;
        case USB_REPORT_CMD_PUT_CHART_OIL:
          vUSBget( &report, eUSBreportToChart );
          break;
        case USB_REPORT_CMD_GET_CHART_COOLANT:
          vUSBsend( &report, vUSBchartToReport );
          break;
        case USB_REPORT_CMD_PUT_CHART_COOLANT:
          vUSBget( &report, eUSBreportToChart );
          break;
        case USB_REPORT_CMD_GET_CHART_FUEL:
          vUSBsend( &report, vUSBchartToReport );
          break;
        case USB_REPORT_CMD_PUT_CHART_FUEL:
          vUSBget( &report, eUSBreportToChart );
          break;
        case USB_REPORT_CMD_PUT_EWA:
          vUSBget( &report, eUSBreportToEWA );
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
          vUSBget( &report, eUSBreportToTime );
          break;
        case USB_REPORT_CMD_GET_FREE_DATA:             // PC->DEVICE
          vUSBsend( &report, vUSBfreeDataToReport );
          break;
        case USB_REPORT_CMD_PUT_FREE_DATA:
          vUSBget( &report, eUSBreportToFreeData );
          break;
        case USB_REPORT_CMD_GET_LOG:
          vUSBsend( &report, vUSBlogToReport );
          break;
        case USB_REPORT_CMD_ERASE_LOG:
          vUSBget( &report, eUSBeraseLOG );
          break;
        case USB_REPORT_CMD_PUT_PASSWORD:
          vUSBget( &report, eUSBreportToPassword );
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
          vUSBsend( &report, vUSBmeasurementToReport );
          break;
        case USB_REPORT_CMD_ERASE_MEASUREMENT:
          vUSBget( &report, eUSBeraseMeasurement );
          break;
        case USB_REPORT_CMD_GET_MEASUREMENT_LENGTH:
          vUSBsend( &report, vUSBmeasurementLengthToReport );
          break;
        case USB_REPORT_CMD_GET_OUTPUT:
          vUSBsend( &report, vUSBoutputToReport );
          break;
        case USB_REPORT_CMD_PUT_OUTPUT:
          vUSBget( &report, eUSBreportToOutput );
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
