/*--------------------------------- Includes ---------------------------------*/
#include "EEPROM.h"
#include "cmsis_os2.h"
#include "system.h"
/*-------------------------------- Structures --------------------------------*/
/*--------------------------------- Constant ---------------------------------*/
/*-------------------------------- Variables ---------------------------------*/
/*--------------------------------- Macros -----------------------------------*/
#define EEPROM_SET_CS( eeprom, state )    HAL_GPIO_WritePin( eeprom->cs.port, eeprom->cs.pin, state )
/*-------------------------------- Functions ---------------------------------*/

/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Make 3 byte address from uint32_t number
 * Input:  adr    - input uint32_t number of address
 *         buffer - output 3 byte buffer
 * Output: none
 */
#if defined( OPTIMIZ )
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
void vEEPROMmakeAdr ( uint32_t adr, uint8_t* buffer )
{
  buffer[0U] = ( uint8_t )( adr >> 16U );
  buffer[1U] = ( uint8_t )( adr >> 8U );
  buffer[2U] = ( uint8_t )( adr );
  return;
}
/*----------------------------------------------------------------------------*/
/*
 * Send data via SPI to the EEPROM
 * Input:  cmd  - command of operation
 *         adr  - addition address of memory 3 bytes ( can be null )
 *         data - addition data to write ( can be null )
 *         size - size of addition data ( can be 0U )
 * Outpur: Status of operation
 */
#if defined( OPTIMIZ )
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
EEPROM_STATUS eEEPROMwrite ( const EEPROM_TYPE* eeprom, uint8_t cmd, const uint32_t* adr, uint8_t* data, uint16_t size )
{
  HAL_StatusTypeDef hal        = HAL_OK;
  EEPROM_STATUS     res        = EEPROM_OK;
  uint8_t           buffer[4U] = { cmd, 0x00U, 0x00U, 0x00U };
  uint8_t           bufferLen  = 1U;
  if ( eeprom->spi != NULL )
  {
    if ( adr != NULL )
    {
      vEEPROMmakeAdr( *adr, &buffer[1U] );
      bufferLen = 4U;
    }
    while ( eeprom->spi->State != HAL_SPI_STATE_READY )
    {
      osDelay( eeprom->timeout );
    }
    EEPROM_SET_CS( eeprom, GPIO_PIN_RESET );
    hal = HAL_SPI_Transmit( eeprom->spi, buffer, bufferLen, eeprom->timeout );
    if ( hal == HAL_OK )
    {
      if ( size > 0U )
      {
        while ( eeprom->spi->State != HAL_SPI_STATE_READY )
        {
    	    osDelay( eeprom->timeout );
        }
        hal = HAL_SPI_Transmit( eeprom->spi, ( uint8_t* )data, size, eeprom->timeout );
        EEPROM_SET_CS( eeprom, GPIO_PIN_SET );
        if ( hal != HAL_OK )
        {
          res = EEPROM_ERROR;
        }
      }
    }
    else
    {
      res = EEPROM_ERROR;
    }
  }
  else
  {
    res = EEPROM_INIT_ERROR;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
/*
 * Get data via SPI from EEPROM
 * Input:  cmd  - command of operation
 *         adr  - addition address of memory 3 bytes ( can be null )
 *         data - buffer for output data
 *         size - size of read data
 * Outpur: Status of operationM
 */
#if defined( OPTIMIZ )
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
EEPROM_STATUS eEEPROMread ( const EEPROM_TYPE* eeprom, uint8_t cmd, const uint32_t* adr, uint8_t* data, uint16_t size )
{
  HAL_StatusTypeDef hal        = HAL_OK;
  EEPROM_STATUS     res        = EEPROM_OK;
  uint8_t           buffer[4U] = { cmd, 0x00U, 0x00U, 0x00U };
  uint8_t           bufferLen  = 1U;
  if ( ( eeprom != NULL ) && ( eeprom->spi != NULL ) )
  {
    if ( adr != NULL )
    {
      vEEPROMmakeAdr( *adr, &buffer[1U] );
      bufferLen = 4U;
    }
    while ( eeprom->spi->State != HAL_SPI_STATE_READY )
    {
      osDelay( eeprom->timeout );
    }
    EEPROM_SET_CS( eeprom, GPIO_PIN_RESET );
    hal = HAL_SPI_Transmit( eeprom->spi, buffer, bufferLen, eeprom->timeout );
    if ( hal == HAL_OK )
    {
      if ( size > 0U )
      {
        while ( eeprom->spi->State != HAL_SPI_STATE_READY )
        {
          osDelay( eeprom->timeout );
        }
        hal = HAL_SPI_Receive( eeprom->spi, data, size, eeprom->timeout );
        EEPROM_SET_CS( eeprom, GPIO_PIN_SET );
        if ( hal != HAL_OK )
        {
          res = EEPROM_ERROR;
        }
      }
    }
    else
    {
      res = EEPROM_ERROR;
    }
  }
  else
  {
    res = EEPROM_INIT_ERROR;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
/*
 * Enable write operations of EEPROM
 * Input:  None
 * Output: Status of operation
 */
#if defined( OPTIMIZ )
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
EEPROM_STATUS eEEPROMwriteEnable ( const EEPROM_TYPE* eeprom )
{
  return eEEPROMwrite( eeprom, EEPROM_CMD_WREN, NULL, NULL, 0U );
}
/*----------------------------------------------------------------------------*/
/*
 * Disable write operations of EEPROM
 * Input:  None
 * Output: Status of operation
 */
#if defined( OPTIMIZ )
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
EEPROM_STATUS eEEPROMwriteDisable ( const EEPROM_TYPE* eeprom )
{
  return eEEPROMwrite( eeprom, EEPROM_CMD_RDSR, NULL, NULL, 0U );
}
/*----------------------------------------------------------------------------*/
/*
 * Read status of EEPROM
 * Input:  status - status of EEPROM
 * Output: Status of operation
 */
#if defined( OPTIMIZ )
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
EEPROM_STATUS eEEPROMreadSR ( const EEPROM_TYPE* eeprom, EEPROM_SR_STATE* status )
{
  uint8_t       data = 0x00U;
  EEPROM_STATUS res  = eEEPROMread( eeprom, EEPROM_CMD_RDSR, NULL, &data, 1U );
  if ( ( data & EEPROM_SR_WIP ) > 0U )
  {
    *status = EEPROM_SR_BUSY;
  }
  else if ( ( data & EEPROM_SR_WEL ) > 0U )
  {
    *status = EEPROM_SR_WRITE_READY;
  }
  else if ( ( data & ( EEPROM_SR_BP0 | EEPROM_SR_BP1 ) ) > 0U )
  {
    *status = EEPROM_SR_BLOCK;
  }
  else
  {
    *status = EEPROM_SR_IDLE;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
/*
 * Write status register of EEPROM
 * Input:  data - data for writing
 * Output: Status of operation
 */
#if defined( OPTIMIZ )
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
EEPROM_STATUS eEEPROMwriteSR ( const EEPROM_TYPE* eeprom, uint8_t data )
{
  return eEEPROMwrite( eeprom, EEPROM_CMD_RDSR, NULL, &data, 1U );
}
/*----------------------------------------------------------------------------*/
/*
 * Unblock EEPROM memory for writing
 * Input:  none
 * Output: Status of operation
 */
#if defined( OPTIMIZ )
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
EEPROM_STATUS eEEPROMunblock ( const EEPROM_TYPE* eeprom )
{
  uint8_t       data = 0x00U;
  EEPROM_STATUS res  = eEEPROMread( eeprom, EEPROM_CMD_RDSR, NULL, &data, 1U );
  if ( res == EEPROM_OK )
  {
    data |= EEPROM_SR_BP0 | EEPROM_SR_BP1;
    res   = eEEPROMwrite( eeprom, EEPROM_CMD_WRSR, NULL, &data, 1U );
  }
  return res;
}
/*----------------------------------------------------------------------------*/
/*
 * Block EEPROM set memory protect level
 * Input:  Protect level
 * Output: Status of operation
 */
#if defined( OPTIMIZ )
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
EEPROM_STATUS eEEPROMsetProtect ( const EEPROM_TYPE* eeprom, EEPROM_PROTECT_TYPE level )
{
  uint8_t       data = 0U;
  EEPROM_STATUS res  = eEEPROMread( eeprom, EEPROM_CMD_RDSR, NULL, &data, 1U );
  if ( res == EEPROM_OK )
  {
    data |= ~EEPROM_SR_BP_WPWM;
    if ( level != EEPROM_PROTECT_NONE )
    {
      data |= ( uint8_t )( level < EEPROM_SR_BP_SHIFT );
    }
    res  = eEEPROMwrite( eeprom, EEPROM_CMD_WRSR, NULL, &data, 1U );
  }
  return res;
}
/*----------------------------------------------------------------------------*/
  /*
   * Block EEPROM get memory protect level
   * Input:  Protect level
   * Output: Status of operation
   */
#if defined( OPTIMIZ )
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
EEPROM_STATUS eEEPROMgetProtect ( const EEPROM_TYPE* eeprom, EEPROM_PROTECT_TYPE* level )
{
  uint8_t       data = 0U;
  EEPROM_STATUS res  = eEEPROMread( eeprom, EEPROM_CMD_RDSR, NULL, &data, 1U );
  if ( res == EEPROM_OK )
  {
    *level = ( EEPROM_PROTECT_TYPE )( data > EEPROM_SR_BP_SHIFT );
  }
  return res;
}
/*----------------------------------------------------------------------------*/
/*
 * Wait some status of EEPROM
 * Input:  target - target status of EEPROM
 * Output: Status of operation
 */
#if defined( OPTIMIZ )
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
EEPROM_STATUS eEEPROMpoolUntil ( const EEPROM_TYPE* eeprom, EEPROM_SR_STATE target )
{
  EEPROM_STATUS   res   = EEPROM_OK;
  EEPROM_SR_STATE state = EEPROM_SR_IDLE;
  do
  {
    res = eEEPROMreadSR( eeprom, &state );
    if ( ( state == target ) && ( res == EEPROM_OK ) )
    {
      break;
    }
    osDelay( eeprom->timeout );
  } while ( ( state != target ) || ( res != EEPROM_OK ) );
  return res;
}
/*----------------------------------------------------------------------------*/
/*
 * Write some data to the memory in one page
 * Input:  adr  - address
 *         data - data array
 *         len  - length of data array
 * Output: Status of operation
 */
#if defined( OPTIMIZ )
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
EEPROM_STATUS eEEPROMWriteData ( const EEPROM_TYPE* eeprom, const uint32_t* adr, uint8_t* data, uint8_t len )
{
  EEPROM_STATUS   res   = EEPROM_OK;
  EEPROM_SR_STATE state = EEPROM_SR_IDLE;
  if ( ( *adr + len ) <= eeprom->size )
  {
    if ( eeprom->page - ( *adr - ( ( ( uint8_t )( *adr / eeprom->page ) ) * eeprom->page ) ) >= len )
    {
      res = eEEPROMreadSR( eeprom, &state );
      if ( ( ( state & EEPROM_SR_SRWD ) == 0U ) && ( res == EEPROM_OK ) )
      {
        if ( state != EEPROM_SR_WRITE_READY )
        {
          res = eEEPROMpoolUntil( eeprom, EEPROM_SR_IDLE );
          if ( res == EEPROM_OK )
          {
            res = eEEPROMwriteEnable( eeprom );
            if ( res == EEPROM_OK )
            {
              res = eEEPROMpoolUntil( eeprom, EEPROM_SR_WRITE_READY );
            }
          }
        }
        if ( res == EEPROM_OK )
        {
          res = eEEPROMwrite( eeprom, EEPROM_CMD_WRITE, adr, data, ( uint16_t )len );
          osDelay( eeprom->timeout );
          if ( res == EEPROM_OK )
          {
            res = eEEPROMwriteDisable( eeprom );
            osDelay( eeprom->timeout );
          }
        }
      }
    }
    else
    {
      res = EEPROM_OVER_PAGE;
    }
  }
  else
  {
    res = EEPROM_ADR_ERROR;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Installation of EEPROM
 * Input:  hspi    - pointer to the spi structure
 *         nssPORT - pointer to the nss port structure
 *         nssPIN  - number of nss pin
 * Output: none
 */
#if defined( OPTIMIZ )
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
EEPROM_STATUS eEEPROMInit ( const EEPROM_TYPE* eeprom )
{
  EEPROM_STATUS       res   = EEPROM_OK;
  EEPROM_PROTECT_TYPE level = EEPROM_PROTECT_NONE;
  EEPROM_SET_CS( eeprom, 1U );
  res = eEEPROMgetProtect( eeprom, &level );
  if ( res == EEPROM_OK )
  {
    if ( level != eeprom->protect )
    {
      res = eEEPROMsetProtect( eeprom, EEPROM_PROTECT_NONE );
      if ( res == EEPROM_OK )
      {
        res = eEEPROMpoolUntil( eeprom, EEPROM_SR_IDLE );
      }
    }
  }
  EEPROM_SET_CS( eeprom, 0U );
  return res;
}
/*----------------------------------------------------------------------------*/
/*
 * Read memory of EEPROM
 * Input:  adr  - address in memory
 *         data - output read data
 *         len  - length to read
 * Output: Status of operation
 */
#if defined( OPTIMIZ )
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
EEPROM_STATUS eEEPROMreadMemory ( const EEPROM_TYPE* eeprom, uint32_t adr, uint8_t* data, uint16_t len )
{
  EEPROM_STATUS   res    = EEPROM_OK;
  EEPROM_SR_STATE state  = EEPROM_SR_IDLE;
  uint16_t        i      = 0;
  uint16_t        size   = 0U;             /* Size of data in memory pages */
  uint16_t        remain = 0U;             /* Remain of first page in bytes */
  uint32_t        count  = adr;            /* Counter of address */
  uint32_t        shift  = 0U;             /* Shift in output buffer */
  uint16_t        subLen = len;            /* Length of write iteration */
  res = eEEPROMreadSR( eeprom, &state );
  if ( res == EEPROM_OK )
  {
    if ( ( adr + len ) <= eeprom->size  )
    {
      remain = eeprom->page - ( adr - ( ( ( uint8_t )( adr / eeprom->page ) ) * eeprom->page ) );
      if ( remain < len )
      {
        size   = ( uint16_t )( ( len - remain ) / eeprom->page );
        subLen = remain;
      }
      res    = eEEPROMread( eeprom, EEPROM_CMD_READ, &count, &data[shift], subLen );
      shift += subLen;
      count += subLen;
      if ( res == EEPROM_OK )
      {
        subLen = eeprom->page;
        for ( i=0U; i<size; i++ )
        {
          res    = eEEPROMread( eeprom, EEPROM_CMD_READ, &count, &data[shift], subLen );
          shift += subLen;
          count += eeprom->page;
          if ( res != EEPROM_OK )
          {
            break;
          }
        }
        if ( ( count < ( adr + len ) ) && ( res == EEPROM_OK ) )
        {
          subLen = ( uint16_t )( adr + len - count );
          res    = eEEPROMread( eeprom, EEPROM_CMD_READ, &count, &data[shift], subLen );
        }
      }
    }
    else
    {
      res = EEPROM_ADR_ERROR;
    }
  }
  return res;
}
/*----------------------------------------------------------------------------*/
/*
 * Write data to memory of EEPROM
 * Input:  adr  - address in memory
 *         data - data to write
 *         len  - length to write
 * Output: Status of operation
 */
#if defined( OPTIMIZ )
  __attribute__ ( ( optimize( OPTIMIZ_LEVEL ) ) )
#endif
EEPROM_STATUS eEEPROMwriteMemory ( const EEPROM_TYPE* eeprom, uint32_t adr, uint8_t* data, uint16_t len )
{
  EEPROM_STATUS res    = EEPROM_OK;
  uint16_t      i      = 0U;
  uint16_t      size   = 0U;             /* Size of data in memory pages */
  uint16_t      remain = 0U;             /* Remain of first page in bytes */
  uint32_t      count  = adr;            /* Counter of address */
  uint32_t      shift  = 0U;             /* Shift in output buffer */
  uint16_t      subLen = len;            /* Length of write iteration */
  remain = eeprom->page - ( adr - ( ( ( uint8_t )( adr / eeprom->page ) ) * eeprom->page ) );
  if ( remain < len )
  {
    size   = ( uint16_t )( ( len - remain ) / eeprom->page );
    subLen = remain;
  }
  res    = eEEPROMWriteData( eeprom, &count, &data[shift], subLen );
  count += subLen;
  shift += subLen;
  if ( res == EEPROM_OK )
  {
    subLen = eeprom->page;
    for ( i=0U; i<size; i++ )
    {
      res    = eEEPROMWriteData( eeprom, &count, &data[shift], subLen );
      count += subLen;
      shift += subLen;
      if ( res != EEPROM_OK )
      {
        break;
      }
    }
    if ( ( count < ( adr + len ) ) && ( res == EEPROM_OK ) )
    {
      subLen = ( uint16_t )( adr + len - count );
      res    = eEEPROMWriteData( eeprom, &count, &data[shift], subLen );
    }
  }
  return res;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
