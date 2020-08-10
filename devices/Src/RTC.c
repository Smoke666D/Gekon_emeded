/*
 * RTC3231.c
 *
 *  Created on: 8 июн. 2020 г.
 *      Author: mikhail.mikhailov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "RTC.h"
#include "cmsis_os2.h"
/*------------------------- Define ------------------------------------------------------------------*/
#define  RTC_TIME_SIZE        7U
#define  RTC_MEMORY_SIZE      19U
/*----------------------- Structures ----------------------------------------------------------------*/
static   I2C_HandleTypeDef*   i2c = NULL;
/*----------------------- Constant ------------------------------------------------------------------*/
/*----------------------- Variables -----------------------------------------------------------------*/
/*----------------------- Functions -----------------------------------------------------------------*/
RTC_STATUS  eRTCwrite( uint8_t adr, uint8_t* data, uint8_t size );
RTC_STATUS  eRTCread( uint8_t adr, uint8_t* data, uint8_t size );
uint8_t     bcdToDec( uint8_t num );
uint8_t     decToBcd( uint8_t num );
RTC_STATUS  uRTCpoolSRUntil( uint8_t target );
RTC_STATUS  eVarifyTime( RTC_TIME* time );
RTC_STATUS  eVerifyAlarm( RTC_ALARM* alarm );
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PRIVATE -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*
 * Send data to RTC via I2C
 * Input:  adr  - start address of register in memory
 *         data - pointer to data array
 *         size - size of data array
 * Output: status of operation
 */
RTC_STATUS eRTCwrite( uint8_t adr, uint8_t* data, uint8_t size )
{
  HAL_StatusTypeDef hal = HAL_OK;
  RTC_STATUS        res = RTC_OK;
  uint8_t           i   = 0U;
  uint8_t           buffer[RTC_MEMORY_SIZE + 1U];
  if ( i2c != NULL)
  {
    if ( adr != 0U )
    {
      buffer[0] = adr;
      for ( i=0U; i<size; i++ )
      {
        buffer[i + 1U] = data[i];
      }
      do
      {
        hal = HAL_I2C_Master_Transmit( i2c, ( uint16_t )RTC_DEVICE_ADR, buffer, ( uint16_t )size, ( uint32_t )RTC_TIMEOUT );
        if ( ( hal == HAL_BUSY ) || ( hal == HAL_TIMEOUT ) )
        {
          osDelay( 10U );
        }
        if ( hal == HAL_ERROR )
        {
          res = RTC_ERROR;
        }
      } while ( ( hal != HAL_OK ) || ( hal != HAL_ERROR ) );
    }
    else
    {
      do
      {
        hal = HAL_I2C_Master_Transmit( i2c, ( uint16_t )RTC_DEVICE_ADR, data, ( uint16_t )size, ( uint32_t )RTC_TIMEOUT );
        if ( ( hal == HAL_BUSY ) || ( hal == HAL_TIMEOUT ) )
        {
          osDelay( 10U );
        }
        if ( hal == HAL_ERROR )
        {
          res = RTC_ERROR;
        }
      } while ( ( hal != HAL_OK ) || ( hal != HAL_ERROR ) );
    }
  }
  else
  {
    res = RTC_INIT_ERROR;
  }
  return res;
}
/*
 * Get data from RTC via I2C
 * Input:  adr  - start address of register in memory
 *         data - pointer to data array
 *         size - size of data array
 * Output: status of operation
 */
RTC_STATUS eRTCread( uint8_t adr, uint8_t* data, uint8_t size )
{
  RTC_STATUS        res = RTC_OK;
  HAL_StatusTypeDef hal = HAL_OK;
  if ( i2c != NULL)
  {
    if ( adr != 0U )
    {
      do
      {
        hal = HAL_I2C_Master_Transmit( i2c, ( uint16_t )RTC_DEVICE_ADR, &adr, 0x0001U, ( uint32_t )RTC_TIMEOUT );
        if ( ( hal == HAL_BUSY ) || ( hal == HAL_TIMEOUT ) )
        {
          osDelay( 10U );
        }
        if ( hal == HAL_ERROR )
        {
          res = RTC_ERROR;
        }
      } while ( ( hal != HAL_OK ) || ( hal != HAL_ERROR ) );
    }
    do
    {
      hal = HAL_I2C_Master_Receive( i2c, ( uint16_t )RTC_DEVICE_ADR, data, ( uint16_t )size, ( uint32_t )RTC_TIMEOUT );
      if ( ( hal == HAL_BUSY ) || ( hal == HAL_TIMEOUT ) )
      {
        osDelay( 10U );
      }
      if ( hal == HAL_ERROR )
      {
        res = RTC_ERROR;
      }
    } while ( ( hal != HAL_OK ) || ( hal != HAL_ERROR ) );
  }
  else
  {
    res = RTC_INIT_ERROR;
  }
  return res;
}
/*
 * Convert BCD number to decimal
 * Input:  BCD  data for conversion
 * Output: Decimal number
 */
uint8_t bcdToDec( uint8_t num )
{
  return ( ( num >> 4U ) * 10U ) + ( num & 0x0FU );
}
/*
 * Convert decimal number to BCD
 * Input:  Decimal data for conversion
 * Output: BCD number
 */
uint8_t decToBcd( uint8_t num )
{
  return ( ( ( num / 10U ) << 4U ) + ( num % 10U ) );
}

RTC_STATUS eVarifyTime( RTC_TIME* time )
{
  RTC_STATUS res = RTC_OK;
  if ( ( time->sec > RTC_SEC_MAX )     ||
       ( time->min > RTC_MIN_MAX )     ||
       ( time->hour > RTC_HOUR_MAX )   ||
       ( time->wday < RTC_WDAY_MIN )   || ( time->wday > RTC_WDAY_MAX )   ||
       ( time->day < RTC_DAY_MIN )     || ( time->day > RTC_DAY_MAX )     ||
       ( time->month < RTC_MONTH_MIN ) || ( time->month > RTC_MONTH_MAX ) ||
       ( time->year < RTC_YEAR_MIN )   || ( time->year > RTC_YEAR_MAX ) )
  {
    res = RTC_FORMAT_ERROR;
  }
  return res;
}

RTC_STATUS eVerifyAlarm( RTC_ALARM* alarm )
{
  RTC_STATUS res = RTC_OK;
  if ( ( alarm->sec > RTC_SEC_MAX )   ||
       ( alarm->min > RTC_MIN_MAX )   ||
       ( alarm->hour > RTC_HOUR_MAX ) ||
       ( alarm->wday < RTC_WDAY_MIN ) || ( alarm->wday > RTC_WDAY_MAX ) ||
       ( alarm->day < RTC_DAY_MIN )   || ( alarm->day > RTC_DAY_MAX ) )
  {
    res = RTC_FORMAT_ERROR;
  }
  return res;
}

RTC_STATUS uRTCpoolSRUntil( uint8_t target )
{
  uint8_t    status = 0U;
  RTC_STATUS res    = RTC_OK;
  while ( ( status != target ) && ( res != RTC_OK ) )
  {
    res = eRTCread( RTC_SR, &status, 1U );
    osDelay( 10U );
    if ( res == RTC_ERROR )
    {
      break;
    }
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*----------------------- PABLICK -------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*
 * Installation of RTC
 * Input:  Pointer to I2C device structure
 * Output: none
 */
void vInitRTC( I2C_HandleTypeDef* hi2c )
{
  i2c = hi2c;
  return;
}
/*
 * Read time from the RTC
 * Input:  Structure of time
 * Output: Status of operation
 */
RTC_STATUS eRTCgetTime( RTC_TIME* time )
{
  uint8_t    buffer[RTC_TIME_SIZE];
  RTC_STATUS res = eRTCread( RTC_SECONRTC, buffer, RTC_TIME_SIZE );
  if ( res == RTC_OK )
  {
    time->sec   = bcdToDec( buffer[RTC_SECONRTC] & RTC_SEC_MSK );
    time->min   = bcdToDec( buffer[RTC_MINUTES] & RTC_MIN_MSK );
    time->hour  = bcdToDec( buffer[RTC_HOURS] );
    time->wday  = buffer[RTC_WDAY] & RTC_WDAY_MSK;
    time->day   = bcdToDec( buffer[RTC_DAY] & RTC_DAY_MSK );
    time->month = bcdToDec( buffer[RTC_MONTH] & RTC_MONTH_MSK );
    time->year  = bcdToDec( buffer[RTC_YEAR] & RTC_YEAR_MSK );
  }
  return res;
}
/*
 * Write time to the RTC
 * Input:  Structure of time
 * Output: Status of operation
 */
RTC_STATUS vRTCsetTime( RTC_TIME* time )
{
  RTC_STATUS res = eVarifyTime( time );
  uint8_t    buffer[RTC_TIME_SIZE];
  if ( res == RTC_OK )
  {
    buffer[RTC_SECONRTC] = decToBcd( time->sec );
    buffer[RTC_MINUTES] = decToBcd( time->min );
    buffer[RTC_HOURS]   = decToBcd( time->hour );
    buffer[RTC_WDAY]    = ( uint8_t )time->wday;
    buffer[RTC_DAY]     = decToBcd( time->day );
    buffer[RTC_MONTH]   = decToBcd( ( uint8_t )time->month );
    buffer[RTC_YEAR]    = decToBcd( time->year );
    res = eRTCwrite( RTC_SECONRTC, buffer, RTC_TIME_SIZE );
  }
  return res;
}
/*
 * Write calibration value to the RTC
 * Input:  Signed calibration value
 * Output: Status of operation
 */
RTC_STATUS vRTCsetCalibration( signed char value )
{
  RTC_FREQ   freq   = RTC_FREQ_1HZ;
  uint8_t    start  = 0U;
  RTC_STATUS res    = eRTCreadFreq( &freq );
  uint8_t    buffer = ( uint8_t )value;
  uint8_t    status = 0x00U;
  if ( res == RTC_OK )
  {
	res = eRTCread( RTC_SR, &status, 1U );
    if ( ( status & RTC_SR_OSF ) > 0U )
    {
      res   = eRTCsetOscillator( 0U, freq );
      start = 1U;
    }
    if ( res == RTC_OK )
    {
      res = uRTCpoolSRUntil( status & ~( RTC_SR_BSY | RTC_SR_OSF ) );
      if ( res == RTC_OK )
      {
        res = eRTCwrite( RTC_AO, &buffer, 1U );
        if ( ( start > 0U ) && ( res == RTC_OK) )
        {
          res   = eRTCsetOscillator( 1U, freq );
        }
      }
    }
  }
  return res;
}
/*
 * Read calibration value from the RTC
 * Input:  Signed calibration value
 * Output: Status of operation
 */
RTC_STATUS eRTCgetCalibration( signed char* value )
{
  uint8_t    buffer = 0x00U;
  RTC_STATUS res    = eRTCread( RTC_AO, &buffer, 1U );
  if ( res == RTC_OK )
  {
    *value = ( signed char )buffer;
  }
  return res;
}
/*
 * Read temperature from the RTC
 * Input:  Float temperature
 * Output: Status of operation
 */
RTC_STATUS eRTCgetTemperature( float* data )
{
  uint8_t    buffer[2U] = { 0x00U, 0x00U };
  RTC_STATUS res        = eRTCread( RTC_UTR, buffer, 2U );
  if ( res == RTC_OK )
  {
    *data = RTC_GET_TEMP( buffer[0U], buffer[1U] );
  }
  return res;
}
/*
 * Setup temperature compensation oscillator
 * Input:  enb - 0 - disable
 *               1 - enable
 * Output: status of operation
 */
RTC_STATUS eRTCsetTemperatureCompensation( uint8_t enb )
{
  RTC_FREQ   freq     = RTC_FREQ_1HZ;
  uint8_t    start    = 0U;
  uint8_t    data[2U] = { 0x00U, 0x00U };
  RTC_STATUS res      = eRTCread( RTC_CR, data, 2U );
  if ( res == RTC_OK )
  {
	res = eRTCreadFreq( &freq );
	if ( res == RTC_OK )
	{
	  if ( ( data[1U] & RTC_SR_OSF ) > 0U )
	  {
	    res   = eRTCsetOscillator( 0U, freq );
	    start = 1U;
	  }
	  if ( res == RTC_OK )
	  {
	    res = uRTCpoolSRUntil( data[1U] & ~( RTC_SR_BSY | RTC_SR_OSF ) );
	    if ( res == RTC_OK )
        {
	      if ( enb > 0U )
	      {
	        data[0U] |= RTC_CR_CONV;
	      }
	      if ( enb == 0U )
	      {
	        data[0U] &= ~RTC_CR_CONV;
	      }
	      res = eRTCwrite( RTC_CR, data, 1U );
	      if ( ( res == RTC_OK ) && ( start > 0U ) )
	      {
            res = eRTCsetOscillator( 1U, freq );
	      }
	    }
	  }
    }
  }
  return res;
}
/*
 * Setup alarm of RTC
 * Input:  n     - number of alarm ( 1 or 2 )
 *         alarm - structure of alarm
 * Output: Status of operation
 */
RTC_STATUS eRTCsetAlarm( uint8_t n, RTC_ALARM* alarm )
{
  RTC_STATUS res        = eVerifyAlarm( alarm );
  uint8_t    buffer[5U] = { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U };
  if ( res == RTC_OK )
  {
    res = eRTCread( RTC_CR, &buffer[4U], 1U );
    if ( res == RTC_OK )
    {
      buffer[0U] = decToBcd( alarm->sec )  | ( ( alarm->rate & 0x01U ) << 7U );
      buffer[1U] = decToBcd( alarm->min )  | ( ( alarm->rate & 0x02U ) << 6U );
      buffer[2U] = decToBcd( alarm->hour ) | ( ( alarm->rate & 0x04U ) << 5U );
      if ( alarm->type == ALARM_ON_DATE )
      {
        buffer[3U] = decToBcd( alarm->day ) | ( ( alarm->rate & 0x08U ) << 4U );
      }
      else
      {
        buffer[3U] = alarm->wday | RTC_ALARMn_DYDT | ( ( alarm->rate & 0x08U ) << 4U );
      }
      switch ( n )
      {
        case 1:
          if ( alarm->ie > 0U )
          {
            buffer[4U] |= RTC_CR_A1IE;
          }
          else
          {
            buffer[4U] &= ~RTC_CR_A1IE;
          }
          res = eRTCwrite( RTC_ALARM1_SECONRTC, &buffer[0U], 5U );
          break;
        case 2:
          if ( alarm->ie > 0U )
          {
            buffer[4U] |= RTC_CR_A2IE;
          }
          else
          {
            buffer[4U] &= ~RTC_CR_A2IE;
          }
          res = eRTCwrite( RTC_ALARM2_MINUTES, &buffer[1U], 4U );
          break;
        default:
          res = RTC_ERROR;
          break;
      }
    }
  }
  return res;
}
/*
 * Read alarm data from the RTC
 * Input:  n     - number of alarm ( 1 or 2 )
 *         alarm - structure of alarm
 * Output: Status of operation
 */
RTC_STATUS eRTCgetAlarm( uint8_t n, RTC_ALARM* alarm )
{
  RTC_STATUS res        = RTC_OK;
  uint8_t    buffer[5U] = { 0x00U, 0x00U, 0x00U, 0x00U, 0x00U };
  switch ( n )
  {
    case 1:
      res = eRTCread( RTC_ALARM1_SECONRTC, buffer, 4U );
      if ( res == RTC_OK )
      {
        res = eRTCread( RTC_CR, &buffer[4U], 1U );
      }
      break;
    case 2:
      res = eRTCread( RTC_ALARM2_MINUTES, &buffer[1U], 4U );
      break;
    default:
      res = RTC_ERROR;
      break;
  }
  if ( res == RTC_OK )
  {
    alarm->rate  = 0U;
    alarm->sec   = bcdToDec( buffer[0U] & RTC_SEC_MSK );
    alarm->rate |= ( buffer[0U] & RTC_AM ) >> 7U;
    alarm->min   = bcdToDec( buffer[1U] & RTC_MIN_MSK );
    alarm->rate |= ( buffer[1U] & RTC_AM ) >> 6U;
    alarm->hour  = bcdToDec( buffer[2U] & RTC_HOUR_MSK );
    alarm->rate |= ( buffer[2U] & RTC_AM ) >> 5U;
    alarm->rate |= ( buffer[3U] & RTC_AM ) >> 4U;
    if ( ( buffer[3U] & RTC_ALARMn_DYDT ) == 0U )
    {
      alarm->type = ALARM_ON_DATE;
      alarm->wday = SUNDAY;
      alarm->day  = bcdToDec( buffer[3U] & RTC_DAY_MSK );
    }
    else
    {
      alarm->type = ALARM_ON_DAY_OF_WEEK;
      alarm->wday = buffer[3U] & RTC_WDAY_MSK;
      alarm->day  = 1U;
    }
    if ( ( buffer[4U] & RTC_CR_A1IE ) > 0U )
    {
      alarm->ie = 1U;
    }
    else
    {
      alarm->ie = 0U;
    }
  }
  return res;
}
/*
 * Read alarm status from the RTC
 * Input:  n - number of alarm:
 *             1 - 1st alarm
 *             2 - 2nd alarm
 *             3 - 1st and 2nd alarms
 * Output: Status of operation
 */
RTC_STATUS vRTCclearAlarm( uint8_t n )
{
  uint8_t    data = 0x00U;
  RTC_STATUS res  = eRTCread( RTC_SR, &data, 1U );
  if ( res == RTC_OK )
  {
    switch ( n )
    {
      case 1:
        data &= ~RTC_SR_A1F;
        break;
      case 2:
        data &= ~RTC_SR_A2F;
        break;
      case 3:
        data &= ~( RTC_SR_A1F | RTC_SR_A2F );
        break;
      default:
        res = RTC_ERROR;
        break;
    }
    res = eRTCwrite( RTC_SR, &data, 1U );
  }
  return res;
}
/*
 * Check alarm status
 * Input:  none
 * Output: 0 - no alarm
 *         1 - 1st alarm
 *         2 - 2nd alarm
 *         3 - 1st and 2nd alarms
 *         4 - error
 */
uint8_t uRTCcheckIfAlarm( void )
{
  uint8_t    res  = 0U;
  uint8_t    data = 0U;
  RTC_STATUS stat = eRTCread( RTC_SR, &data, 1U );
  if ( stat == RTC_OK )
  {
    if ( ( data & RTC_SR_A1F ) > 0U )
    {
      res |= 0x01U;
    }
    if ( ( data & RTC_SR_A2F ) > 0U )
    {
      res |= 0x02U;
    }
  }
  else
  {
    res = 0x04U;
  }
  return res;
}

RTC_STATUS eRTCsetExternSquareWave( uint8_t enb )
{
  uint8_t    data = 0x00U;
  RTC_STATUS res  = eRTCread( RTC_CR, &data, 1U );
  if ( res == RTC_OK )
  {
    if ( enb > 0U )
    {
      data |= RTC_CR_BBSQW;
      data &= ~RTC_CR_INTCN;
    }
    else
    {
      data &= ~RTC_CR_BBSQW;
    }
    res = eRTCwrite( RTC_CR, &data, 1U );
  }
  return res;
}

RTC_STATUS eRTCsetInterrupt( uint8_t ext )
{
  uint8_t    data = 0x00U;
  RTC_STATUS res  = eRTCread( RTC_CR, &data, 1U );
  if ( res == RTC_OK )
  {
    if ( ext > 0U )
    {
      data |= RTC_CR_INTCN | RTC_CR_BBSQW;
    }
    else
    {
      data &= ~( RTC_CR_INTCN | RTC_CR_BBSQW );
    }
    res = eRTCwrite( RTC_CR, &data, 1U );
  }
  return res;
}

RTC_STATUS eRTCreadFreq( RTC_FREQ* freq )
{
  uint8_t    data = 0x00U;
  RTC_STATUS res  = eRTCread( RTC_CR, &data, 1U );
  if (res == RTC_OK )
  {
    data &= RTC_CR_RS1 | RTC_CR_RS2;
    switch ( data )
    {
      case RTC_CR_RS_1Hz:
        *freq = RTC_FREQ_1HZ;
        break;
      case RTC_CR_RS_1024Hz:
        *freq = RTC_FREQ_1024HZ;
        break;
      case RTC_CR_RS_4096Hz:
        *freq = RTC_FREQ_4096HZ;
        break;
      case RTC_CR_RS_8192Hz:
        *freq = RTC_FREQ_8192Hz;
        break;
      default:
    	res = RTC_FORMAT_ERROR;
        break;
    }
  }
  return res;
}
/*
 * Setup oscillator of the RTC
 * Input:  enb  - enable or disable oscillator
 *         freq - frequency of oscillator
 * Output: status of operation
 */
RTC_STATUS eRTCsetOscillator( uint8_t enb, RTC_FREQ freq )
{
  uint8_t    data = 0x00U;
  RTC_STATUS res  = eRTCread( RTC_CR, &data, 1U );
  if ( res == RTC_OK )
  {
    if ( enb == 0U )
    {
      data &= ~RTC_CR_EOSC;
    }
    else
    {
      data |= RTC_CR_EOSC;
      switch ( freq )
      {
        case RTC_FREQ_1HZ:
          data |= RTC_CR_RS_1Hz;
          break;
        case RTC_FREQ_1024HZ:
          data |= RTC_CR_RS_1024Hz;
          break;
        case RTC_FREQ_4096HZ:
          data |= RTC_CR_RS_4096Hz;
          break;
        case RTC_FREQ_8192Hz:
          data |= RTC_CR_RS_8192Hz;
          break;
        default:
          data |= RTC_CR_RS_1Hz;
          break;
      }
    }
    res = eRTCwrite( RTC_CR, &data, 1U );
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
