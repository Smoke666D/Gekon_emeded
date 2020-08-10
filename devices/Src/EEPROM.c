/*--------------------------------- Includes ---------------------------------*/
#include "EEPROM.h"
#include "cmsis_os2.h"
/*-------------------------------- Structures --------------------------------*/
static SPI_HandleTypeDef* EEPROM_SPI = NULL;
/*--------------------------------- Constant ---------------------------------*/
/*-------------------------------- Variables ---------------------------------*/
/*-------------------------------- Functions ---------------------------------*/
EEPROM_STATUS eEEPROMwrite( uint8_t cmd, const uint8_t* adr, uint8_t* data, uint8_t size );  /* Send data via SPI to the EEPROM */
EEPROM_STATUS eEEPROMread( uint8_t cmd, const uint8_t* adr, uint8_t* data, uint8_t size );   /* Get data via SPI from EEPROM */
EEPROM_STATUS eEEPROMwriteEnable( void );                                              /* Enable writing */
EEPROM_STATUS eEEPROMwriteDisable( void );                                             /* Disable writing */
EEPROM_STATUS eEEPROMreadSR( EEPROM_SR_STATE* status );                                /* Read status regester */
EEPROM_STATUS eEEPROMwriteSR( uint8_t data );                                          /* Write status register */
EEPROM_STATUS vEEPROMUnblock( void );                                                  /* Unblock memory for writing */
EEPROM_STATUS vEEPROMpoolUntil( EEPROM_SR_STATE target );                              /* Waiting for state */
/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Send data via SPI to the EEPROM
 * Input:  cmd  - command of operation
 *         adr  - addition address of memory 3 bytes ( can be null )
 *         data - addition data to write ( can be null )
 *         size - size of addition data ( can be 0U )
 * Outpur: Status of operation
 */
EEPROM_STATUS eEEPROMwrite( uint8_t cmd, const uint8_t* adr, uint8_t* data, uint8_t size )
{
  HAL_StatusTypeDef hal        = HAL_OK;
  EEPROM_STATUS     res        = EEPROM_OK;
  uint8_t           buffer[4U] = { cmd, 0x00U, 0x00U, 0x00U };
  uint8_t           bufferLen  = 1U;
  if ( EEPROM_SPI != NULL )
  {
    if ( adr != NULL )
    {
      buffer[1U] = adr[0U];
      buffer[2U] = adr[1U];
      buffer[3U] = adr[2U];
      bufferLen  = 4U;
    }
    while ( EEPROM_SPI->State != HAL_SPI_STATE_READY )
    {
      osDelay( 1U );
    }
    hal = HAL_SPI_Transmit( EEPROM_SPI, buffer, bufferLen, EEPROM_TIMEOUT );
    if ( hal == HAL_OK )
    {
      if ( size > 0U )
      {
        while ( EEPROM_SPI->State != HAL_SPI_STATE_READY )
        {
    	  osDelay( 1U );
        }
        hal = HAL_SPI_Transmit( EEPROM_SPI, data, size, EEPROM_TIMEOUT );
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
/*
 * Get data via SPI from EEPRO
 * Input:  cmd  - command of operation
 *         adr  - addition address of memory 3 bytes ( can be null )
 *         data - addition data to write ( can be null )
 *         size - size of addition data ( can be 0U )
 * Outpur: Status of operationM
 */
EEPROM_STATUS eEEPROMread( uint8_t cmd, const uint8_t* adr, uint8_t* data, uint8_t size )
{
  HAL_StatusTypeDef hal        = HAL_OK;
  EEPROM_STATUS     res        = EEPROM_OK;
  uint8_t           buffer[4U] = { cmd, 0x00U, 0x00U, 0x00U };
  uint8_t           bufferLen  = 1U;
  if ( EEPROM_SPI != NULL )
  {
    if ( adr != NULL )
    {
      buffer[1U] = adr[0U];
      buffer[2U] = adr[1U];
      buffer[3U] = adr[2U];
      bufferLen  = 4U;
    }
    while ( EEPROM_SPI->State != HAL_SPI_STATE_READY )
    {
      osDelay( 1U );
    }
    hal = HAL_SPI_Transmit( EEPROM_SPI, buffer, bufferLen, EEPROM_TIMEOUT );
    if ( hal == HAL_OK )
    {
      if ( size > 0U )
      {
        while ( EEPROM_SPI->State != HAL_SPI_STATE_READY )
        {
          osDelay( 1U );
        }
        hal = HAL_SPI_Receive( EEPROM_SPI, data, size, EEPROM_TIMEOUT );
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
/*
 * Enable write operations of EEPROM
 * Input:  None
 * Output: Status of operation
 */
EEPROM_STATUS eEEPROMwriteEnable( void )
{
  return eEEPROMwrite( EEPROM_WREN, NULL, NULL, 0U );
}
/*
 * Disable write operations of EEPROM
 * Input:  None
 * Output: Status of operation
 */
EEPROM_STATUS eEEPROMwriteDisable( void )
{
  return eEEPROMwrite( EEPROM_RDSR, NULL, NULL, 0U );
}
/*
 * Read status of EEPROM
 * Input:  status - status of EEPROM
 * Output: Status of operation
 */
EEPROM_STATUS eEEPROMreadSR( EEPROM_SR_STATE* status )
{
  uint8_t       data = 0x00U;
  EEPROM_STATUS res  = eEEPROMread( EEPROM_RDSR, NULL, &data, 1U );
  if ( data & EEPROM_SR_WIP )
  {
    *status = EEPROM_SR_BUSY;
  }
  else if ( data & EEPROM_SR_WEL )
  {
    *status = EEPROM_SR_WRITE_READY;
  }
  else if ( ( data & ( EEPROM_SR_BP0 | EEPROM_SR_BP1 ) ) == 0U )
  {
    *status = EEPROM_SR_UNBLOCK;
  }
  else
  {
    *status = EEPROM_SR_IDLE;
  }
  return res;
}
/*
 * Write status register of EEPROM
 * Input:  data - data for writing
 * Output: Status of operation
 */
EEPROM_STATUS eEEPROMwriteSR( uint8_t data )
{
  return eEEPROMwrite( EEPROM_RDSR, NULL, &data, 1U );
}
/*
 * Unblock EEPROM memory for writing
 * Input:  none
 * Output: Status of operation
 */
EEPROM_STATUS vEEPROMunblock ( void )
{
  uint8_t       data = 0x00;
  EEPROM_STATUS res  = eEEPROMread( EEPROM_RDSR, NULL, &data, 1U );
  if ( res == EEPROM_OK )
  {
    data |= EEPROM_SR_BP0 | EEPROM_SR_BP1;
    res = eEEPROMwrite( EEPROM_WRSR, NULL, &data, 1U );
  }
  return res;
}
/*
 * Wait some status of EEPROM
 * Input:  target - target status of EEPROM
 * Output: Status of operation
 */
EEPROM_STATUS vEEPROMpoolUntil ( EEPROM_SR_STATE target )
{
  EEPROM_STATUS   res   = EEPROM_OK;
  EEPROM_SR_STATE state = EEPROM_SR_IDLE;
  do
  {
    res = eEEPROMreadSR( &state );
    if ( ( state == target) && ( res == EEPROM_OK ) )
    {
      break;
    }
    osDelay( 5U );
  } while ( ( state != target ) && ( res != EEPROM_OK ) );
  return res;
}
/*----------------------------------------------------------------------------*/
/*----------------------- PABLICK --------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*
 * Installation of EEPROM
 * Input:  hspi - pointer to the spi structure
 * Output: none
 */
void vEEPROMInit( SPI_HandleTypeDef* hspi )
{
  EEPROM_SPI = hspi;
  return;
}
/*
 * Transferm uint32_t address to the 24bit array ( uint8_t x3 )
 * Input:  adr    - input address
 *         buffer - output aaray
 * Output: none
 */
void vEEPROMformAdr ( uint32_t adr, uint8_t* buffer )
{
  buffer[0U] = ( uint8_t )adr;
  buffer[1U] = ( uint8_t )( adr >> 8U );
  buffer[2U] = ( uint8_t )( adr >> 16U );
  return;
}
/*
 * Read memory of EEPROM
 * Input:  adr  - address in memory
 *         data - output read data
 *         len  - length to read
 * Output: Status of operation
 */
EEPROM_STATUS eEEPROMReadMemory ( const uint8_t* adr, uint8_t* data, uint32_t len )
{
  EEPROM_STATUS res        = EEPROM_OK;
  uint32_t      adr32      = ADR_TO_UINT( adr );
  uint8_t       buffer[4U] = { EEPROM_READ, REVERSE_BYTE( adr[2U] ), REVERSE_BYTE( adr[1U] ), REVERSE_BYTE( adr[0U] ) };
  if ( adr32 <= EEPROM_MAX_ADR )
  {
    res = vEEPROMpoolUntil( EEPROM_SR_UNBLOCK );
    if (res == EEPROM_OK )
    {
      res = eEEPROMread( EEPROM_READ, buffer, data, len );
    }
  }
  else
  {
    res = EEPROM_ADR_ERROR;
  }
  return res;
}
/*
 * Write data to memory of EEPROM
 * Input:  adr  - address in memory
 *         data - data to write
 *         len  - length to write
 * Output: Status of operation
 */
EEPROM_STATUS eEEPROMWriteMemory ( uint8_t* adr, uint8_t* data, uint32_t len )
{
  EEPROM_STATUS res        = EEPROM_OK;
  uint32_t      adr32      = ADR_TO_UINT( adr );
  uint8_t       buffer[4U] = { EEPROM_WRITE, REVERSE_BYTE( adr[2U] ), REVERSE_BYTE( adr[1U] ), REVERSE_BYTE( adr[0U] ) };
  if ( adr32 <= EEPROM_MAX_ADR )
  {
    res = vEEPROMpoolUntil( EEPROM_SR_IDLE );
    if ( res == EEPROM_OK )
    {
      res = eEEPROMwriteEnable();
      if ( res == EEPROM_OK )
      {
        res = vEEPROMpoolUntil( EEPROM_SR_WRITE_READY );
        if ( res == EEPROM_OK )
        {
          res = vEEPROMunblock();
          if ( res == EEPROM_OK )
          {
            res = vEEPROMpoolUntil( EEPROM_SR_UNBLOCK );
            if ( res == EEPROM_OK )
            {
              res = eEEPROMwrite( EEPROM_WRITE, buffer, data, len );
            }
          }
        }
      }
    }
  }
  else
  {
    res = EEPROM_ADR_ERROR;
  }
  return res;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
