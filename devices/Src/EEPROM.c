/*--------------------------------- Includes ---------------------------------*/
#include "EEPROM.h"
#include "cmsis_os2.h"
/*-------------------------------- Structures --------------------------------*/
static SPI_HandleTypeDef* EEPROMspi     = NULL;
static GPIO_TypeDef*      EEPROMnssPort = NULL;
/*--------------------------------- Constant ---------------------------------*/
/*-------------------------------- Variables ---------------------------------*/
static uint32_t           EEPROMnssPin  = 0U;
/*---------------------------------- Macros ----------------------------------*/
#define EEPROM_NSS_RESET  HAL_GPIO_WritePin( EEPROMnssPort, EEPROMnssPin, GPIO_PIN_RESET )
#define EEPROM_NSS_SET    HAL_GPIO_WritePin( EEPROMnssPort, EEPROMnssPin, GPIO_PIN_SET )
/*-------------------------------- Functions ---------------------------------*/
EEPROM_STATUS eEEPROMwrite ( uint8_t cmd, const uint32_t* adr, uint8_t* data, uint16_t size ); /* Send data via SPI to the EEPROM */
EEPROM_STATUS eEEPROMread ( uint8_t cmd, const uint32_t* adr, uint8_t* data, uint16_t size );  /* Get data via SPI from EEPROM */
EEPROM_STATUS eEEPROMwriteEnable ( void );                                                     /* Enable writing */
EEPROM_STATUS eEEPROMwriteDisable ( void );                                                    /* Disable writing */
EEPROM_STATUS eEEPROMreadSR ( EEPROM_SR_STATE* status );                                       /* Read status register */
EEPROM_STATUS eEEPROMwriteSR ( uint8_t data );                                                 /* Write status register */
EEPROM_STATUS eEEPROMunblock ( void );                                                         /* Unblock memory for writing */
EEPROM_STATUS eEEPROMblock ( void );                                                           /* Block memory for writing */
EEPROM_STATUS eEEPROMpoolUntil ( EEPROM_SR_STATE target );                                     /* Waiting for state */
/*----------------------------------------------------------------------------*/
/*----------------------- PRIVATE --------------------------------------------*/
/*----------------------------------------------------------------------------*/
void vEEPROMmakeAdr ( uint32_t adr, uint8_t* buffer )
{
  buffer[2U] = ( uint8_t )( adr );
  buffer[1U] = ( uint8_t )( adr >> 8U );
  buffer[0U] = ( uint8_t )( adr >> 16U );
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
EEPROM_STATUS eEEPROMwrite( uint8_t cmd, const uint32_t* adr, uint8_t* data, uint16_t size )
{
  HAL_StatusTypeDef hal        = HAL_OK;
  EEPROM_STATUS     res        = EEPROM_OK;
  uint8_t           buffer[4U] = { cmd, 0x00U, 0x00U, 0x00U };
  uint8_t           bufferLen  = 1U;
  if ( EEPROMspi != NULL )
  {
    if ( adr != NULL )
    {
      vEEPROMmakeAdr( *adr, &buffer[1U] );
      bufferLen  = 4U;
    }
    while ( EEPROMspi->State != HAL_SPI_STATE_READY )
    {
      osDelay( EEPROM_TIMEOUT );
    }
    EEPROM_NSS_RESET;
    hal = HAL_SPI_Transmit( EEPROMspi, buffer, bufferLen, EEPROM_TIMEOUT );
    if ( hal == HAL_OK )
    {
      if ( size > 0U )
      {
        while ( EEPROMspi->State != HAL_SPI_STATE_READY )
        {
    	    osDelay( EEPROM_TIMEOUT );
        }
        hal = HAL_SPI_Transmit( EEPROMspi, data, size, EEPROM_TIMEOUT );
        EEPROM_NSS_SET;
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
 * Get data via SPI from EEPRO
 * Input:  cmd  - command of operation
 *         adr  - addition address of memory 3 bytes ( can be null )
 *         data - addition data to write ( can be null )
 *         size - size of addition data ( can be 0U )
 * Outpur: Status of operationM
 */
EEPROM_STATUS eEEPROMread( uint8_t cmd, const uint32_t* adr, uint8_t* data, uint16_t size )
{
  HAL_StatusTypeDef hal        = HAL_OK;
  EEPROM_STATUS     res        = EEPROM_OK;
  uint8_t           buffer[4U] = { cmd, 0x00U, 0x00U, 0x00U };
  uint8_t           bufferLen  = 1U;
  if ( EEPROMspi != NULL )
  {
    if ( adr != NULL )
    {
      vEEPROMmakeAdr( *adr, &buffer[1U] );
      bufferLen  = 4U;
    }
    while ( EEPROMspi->State != HAL_SPI_STATE_READY )
    {
      osDelay( EEPROM_TIMEOUT );
    }
    EEPROM_NSS_RESET;
    hal = HAL_SPI_Transmit( EEPROMspi, buffer, bufferLen, EEPROM_TIMEOUT );
    if ( hal == HAL_OK )
    {
      if ( size > 0U )
      {
        while ( EEPROMspi->State != HAL_SPI_STATE_READY )
        {
          osDelay( EEPROM_TIMEOUT );
        }
        hal = HAL_SPI_Receive( EEPROMspi, data, size, EEPROM_TIMEOUT );
        EEPROM_NSS_SET;
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
EEPROM_STATUS eEEPROMwriteEnable( void )
{
  return eEEPROMwrite( EEPROM_WREN, NULL, NULL, 0U );
}
/*----------------------------------------------------------------------------*/
/*
 * Disable write operations of EEPROM
 * Input:  None
 * Output: Status of operation
 */
EEPROM_STATUS eEEPROMwriteDisable( void )
{
  return eEEPROMwrite( EEPROM_RDSR, NULL, NULL, 0U );
}
/*----------------------------------------------------------------------------*/
/*
 * Read status of EEPROM
 * Input:  status - status of EEPROM
 * Output: Status of operation
 */
EEPROM_STATUS eEEPROMreadSR( EEPROM_SR_STATE* status )
{
  uint8_t       data = 0x00U;
  EEPROM_STATUS res  = eEEPROMread( EEPROM_RDSR, NULL, &data, 1U );
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
EEPROM_STATUS eEEPROMwriteSR( uint8_t data )
{
  return eEEPROMwrite( EEPROM_RDSR, NULL, &data, 1U );
}
/*----------------------------------------------------------------------------*/
/*
 * Unblock EEPROM memory for writing
 * Input:  none
 * Output: Status of operation
 */
EEPROM_STATUS eEEPROMunblock ( void )
{
  uint8_t       data = 0x00U;
  EEPROM_STATUS res  = eEEPROMread( EEPROM_RDSR, NULL, &data, 1U );
  if ( res == EEPROM_OK )
  {
    data |= EEPROM_SR_BP0 | EEPROM_SR_BP1;
    res   = eEEPROMwrite( EEPROM_WRSR, NULL, &data, 1U );
  }
  return res;
}
/*----------------------------------------------------------------------------*/
/*
 * Block EEPROM memory for writing
 * Input:  none
 * Output: Status of operation
 */
EEPROM_STATUS eEEPROMblock ( void )
{
  uint8_t       data = 0x00;
  EEPROM_STATUS res  = eEEPROMread( EEPROM_RDSR, NULL, &data, 1U );
  if ( res == EEPROM_OK )
  {
    data &= ~( EEPROM_SR_BP0 | EEPROM_SR_BP1 );
    res  = eEEPROMwrite( EEPROM_WRSR, NULL, &data, 1U );
  }
  return res;
}
/*----------------------------------------------------------------------------*/
/*
 * Wait some status of EEPROM
 * Input:  target - target status of EEPROM
 * Output: Status of operation
 */
EEPROM_STATUS eEEPROMpoolUntil ( EEPROM_SR_STATE target )
{
  EEPROM_STATUS   res   = EEPROM_OK;
  EEPROM_SR_STATE state = EEPROM_SR_IDLE;
  do
  {
    res = eEEPROMreadSR( &state );
    if ( ( state == target ) && ( res == EEPROM_OK ) )
    {
      break;
    }
    osDelay( EEPROM_TIMEOUT );
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
EEPROM_STATUS eEEPROMWriteData ( const uint32_t* adr, uint8_t* data, uint8_t len )
{
  EEPROM_STATUS   res   = EEPROM_OK;
  EEPROM_SR_STATE state = EEPROM_SR_IDLE;
  if ( ( *adr + len ) <= EEPROM_MAX_ADR )
  {
    if ( EEPROM_PAGE_SIZE - ( *adr - ( ( ( uint8_t )( *adr / EEPROM_PAGE_SIZE ) ) * EEPROM_PAGE_SIZE ) ) >= len )
    {
      res = eEEPROMreadSR( &state );
      if ( ( state & EEPROM_SR_SRWD ) == 0U )
      {
        res = eEEPROMpoolUntil( EEPROM_SR_IDLE );
        if ( res == EEPROM_OK )
        {
          res = eEEPROMwriteEnable();
          if ( res == EEPROM_OK )
          {
            res = eEEPROMpoolUntil( EEPROM_SR_WRITE_READY );
            if ( res == EEPROM_OK )
            {
              res = eEEPROMwrite( EEPROM_WRITE, adr, data, ( uint16_t )len );
              osDelay( EEPROM_TIMEOUT );
              if ( res == EEPROM_OK )
              {
                res = eEEPROMwriteDisable();
                osDelay( EEPROM_TIMEOUT );
              }
            }
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
EEPROM_STATUS eEEPROMInit( SPI_HandleTypeDef* hspi, GPIO_TypeDef* nssPORT, uint32_t nssPIN )
{
  EEPROM_STATUS res = EEPROM_OK;
  EEPROMspi     = hspi;
  EEPROMnssPin  = nssPIN;
  EEPROMnssPort = nssPORT;
  EEPROM_NSS_SET;
  res = eEEPROMblock();
  if ( res == EEPROM_OK )
  {
    res = eEEPROMpoolUntil( EEPROM_SR_IDLE );
  }
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
EEPROM_STATUS eEEPROMreadMemory ( uint32_t adr, uint8_t* data, uint16_t len )
{
  EEPROM_STATUS   res    = EEPROM_OK;
  EEPROM_SR_STATE state  = EEPROM_SR_IDLE;
  uint16_t        i      = 0;
  uint16_t        size   = 0U;             /* Size of data in memory pages */
  uint16_t        remain = 0U;             /* Remain of first page in bytes */
  uint32_t        count  = adr;            /* Counter of address */
  uint32_t        shift  = 0U;             /* Shift in output buffer */
  uint16_t        subLen = len;            /* Length of write iteration */
  res = eEEPROMreadSR( &state );
  if ( res == EEPROM_OK )
  {
    if ( ( adr + len ) <= EEPROM_MAX_ADR  )
    {
      remain = EEPROM_PAGE_SIZE - ( adr - ( ( ( uint8_t )( adr / EEPROM_PAGE_SIZE ) ) * EEPROM_PAGE_SIZE ) );
      if ( remain < len )
      {
        size   = ( uint8_t )( ( len - remain ) / EEPROM_PAGE_SIZE );
        subLen = remain;
      }
      res    = eEEPROMread( EEPROM_READ, &count, &data[shift], subLen );
      shift += subLen;
      count += subLen;
      if ( res == EEPROM_OK )
      {
	      subLen = EEPROM_PAGE_SIZE;
	      for ( i=0U; i<size; i++ )
	      {
	        res    = eEEPROMread( EEPROM_READ, &count, &data[shift], subLen );
	        shift += subLen;
	        count += EEPROM_PAGE_SIZE;
	        if ( res != EEPROM_OK )
	        {
	          break;
	        }
	      }
	      if ( ( count < ( adr + len ) ) && ( res == EEPROM_OK ) )
	      {
	        subLen = ( uint16_t )( adr + len - count );
	        res    = eEEPROMread( EEPROM_READ, &count, &data[shift], subLen );
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
EEPROM_STATUS eEEPROMwriteMemory ( uint32_t adr, uint8_t* data, uint16_t len )
{
  EEPROM_STATUS res    = EEPROM_OK;
  uint16_t      i      = 0U;
  uint16_t      size   = 0U;             /* Size of data in memory pages */
  uint16_t      remain = 0U;             /* Remain of first page in bytes */
  uint32_t      count  = adr;            /* Counter of address */
  uint32_t      shift  = 0U;             /* Shift in output buffer */
  uint16_t      subLen = len;            /* Length of write iteration */
  remain = EEPROM_PAGE_SIZE - ( adr - ( ( ( uint8_t )( adr / EEPROM_PAGE_SIZE ) ) * EEPROM_PAGE_SIZE ) );
  if ( remain < len )
  {
    size   = (uint8_t)( ( len - remain ) / EEPROM_PAGE_SIZE );
    subLen = remain;
  }
  res    = eEEPROMWriteData( &count, &data[shift], subLen );
  count += subLen;
  shift += subLen;
  if ( res == EEPROM_OK )
  {
    subLen = EEPROM_PAGE_SIZE;
    for ( i=0U; i<size; i++ )
    {
      res    = eEEPROMWriteData( &count, &data[shift], subLen );
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
      res    = eEEPROMWriteData( &count, &data[shift], subLen );
    }
  }
  return res;
}
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
/*----------------------------------------------------------------------------*/
