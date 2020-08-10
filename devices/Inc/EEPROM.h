/*
 * * * *  EEPROMM01 1-Mbit serial SPI bus EEPROM * * * *
 * Hight speed clock: 16 MHz
 * Byte write within: 5 ms
 * Page write within: 5 ms
 */
#ifndef INC_EEPROM_H_
#define INC_EEPROM_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "stm32f2xx_hal_spi.h"
/*------------------------ Macros --------------------------------------*/

/*------------------------ Define --------------------------------------*/
/*---------- Choose your chip ---------*/
//#define  M95M01
#define  M95M04
/*---------- Software parameters ------*/
#define  EEPROM_TIMEOUT           1000U
/*---------- Chip parameters ----------*/
#define  EEPROM_PAGE_SIZE         0x100U    /* bytes */
#ifdef M95M01
  #define  EEPROM_MAX_ADR         0x1FFFFU
#endif
#ifdef M95M04
  #define  EEPROM_MAX_ADR         0x7FFFFU
#endif
/*---------- Commands -----------------*/
#define  EEPROM_WREN              0x06U     /* Write Enable */
#define  EEPROM_WRDI              0x04U     /* Write Disable */
#define  EEPROM_RDSR              0x05U     /* Read Status Register */
#define  EEPROM_WRSR              0x01U     /* Write Status Register */
#define  EEPROM_READ              0x03U     /* Read from Memory Array */
#define  EEPROM_WRITE             0x02U     /* Write to Memory Array */
#define  EEPROM_RDID              0x83U     /* Read Identification Page */
#define  EEPROM_WRID              0x82U     /* Write Identification Page */
#define  EEPROM_RDLS              0x83U     /* Reads the Identification Page lock status (only for EEPROMM01-D) */
#define  EEPROM_LIF               0x82U     /* Locks the Identification Page in read-only mode (only for EEPROMM01-D) */
/*---------- Status register ----------*/
#define  EEPROM_SR_WIP            0x01U     /* Write In Progress */
#define  EEPROM_SR_WEL            0x02U     /* Write Enable Latch */
#define  EEPROM_SR_BP0            0x04U     /* Block Protect 0 */
#define  EEPROM_SR_BP1            0x08U     /* Block Protect 1 */
#define  EEPROM_SR_SRWD           0x80U     /* Status Register Write Disable */
#define  EEPROM_SR_WPNO           0x00U     /* Write-protected none */
#define  EEPROM_SR_WPUQ           0x04U     /* Write-protected Upper quarter: 0x18000 - 0x1FFFF */
#define  EEPROM_SR_WPUH           0x08U     /* Write-protected Upper half: 0x10000 - 0x1FFFF */
#define  EEPROM_SR_WPWM           0x0CU     /* Write-protected Whole memory: 0x00000 - 0x1FFFF */
/*------------------------- Enum ---------------------------------------*/
typedef enum
{
  EEPROM_OK,            /* Command done and bus is free for new one */
  EEPROM_BUSY,          /* Some process in progress */
  EEPROM_WRITE_DISABLE, /* Write disable by W pin */
  EEPROM_OVER_PAGE,	/* Try to write over one page */
  EEPROM_OVER_ROLL,     /* Try to read over end of memory */
  EEPROM_ADR_ERROR,     /* Try to get access over address */
  EEPROM_INIT_ERROR,    /* No SPI structure*/
  EEPROM_ERROR,         /* Other errors */
} EEPROM_STATUS;

typedef enum
{
  EEPROM_SR_IDLE,        /* Idle state */
  EEPROM_SR_BUSY,        /* Write process in progress */
  EEPROM_SR_WRITE_READY, /* Memory is enabled for writing */
  EEPROM_SR_UNBLOCK,     /* All memory is unprotected for writing BP0 = 0, BP1 = 0 */
  EEPROM_SR_BLOCK,       /* All memory is protected for writing BP0 = 1, BP1 = 1 */
} EEPROM_SR_STATE;
/*----------------------- Functions ------------------------------------*/
EEPROM_STATUS eEEPROMInit ( SPI_HandleTypeDef* hspi, GPIO_TypeDef* nssGPIO, uint32_t nssPIN ); /* Installation of EEPROM */
EEPROM_STATUS eEEPROMReadMemory ( const uint32_t* adr, uint8_t* data, uint8_t len );          /* Read memory of EEPROM */
EEPROM_STATUS eEEPROMWriteMemory ( const uint32_t* adr, uint8_t* data, uint8_t len );         /* Write data to memory of EEPROM */
/*----------------------------------------------------------------------*/
#endif /* INC_EEPROM_H_ */
