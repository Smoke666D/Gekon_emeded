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
#include "stm32f2xx_hal_gpio.h"
#include "common.h"
/*------------------------ Define --------------------------------------*/
#define  EEPROM_MAX_ADR_SIZE      4U
//#define  EEPROM_LENGTH_SIZE       3U
/*---------- Commands -----------------*/
#define  EEPROM_CMD_WRSR          0x01U     /* Write Status Register  */
#define  EEPROM_CMD_WRITE         0x02U     /* Write to Memory Array  */
#define  EEPROM_CMD_READ          0x03U     /* Read from Memory Array */
#define  EEPROM_CMD_WRDI          0x04U     /* Write Disable          */
#define  EEPROM_CMD_RDSR          0x05U     /* Read Status Register   */
#define  EEPROM_CMD_WREN          0x06U     /* Write Enable           */
/*---------- Addition commands --------*/
#define  EEPROM_CMD_RDID          0x83U     /* Read Identification Page                                               */
#define  EEPROM_CMD_WRID          0x82U     /* Write Identification Page                                              */
#define  EEPROM_CMD_RDLS          0x83U     /* Reads the Identification Page lock status (only for EEPROMM01-D)       */
#define  EEPROM_CMD_LIF           0x82U     /* Locks the Identification Page in read-only mode (only for EEPROMM01-D) */
/*---------- Status register ----------*/
#define  EEPROM_SR_WIP            0x01U     /* Write In Progress                                */
#define  EEPROM_SR_WEL            0x02U     /* Write Enable Latch                               */
#define  EEPROM_SR_BP0            0x04U     /* Block Protect 0                                  */
#define  EEPROM_SR_BP1            0x08U     /* Block Protect 1                                  */
#define  EEPROM_SR_SRWD           0x80U     /* Status Register Write Disable                    */
#define  EEPROM_SR_BP_SHIFT       2U        /* Shift of BP0 bit                                 */
#define  EEPROM_SR_BP_WPNO        0x00U     /* Write-protected none                             */
#define  EEPROM_SR_BP_WPUQ        0x04U     /* Write-protected Upper quarter: 0x18000 - 0x1FFFF */
#define  EEPROM_SR_BP_WPUH        0x08U     /* Write-protected Upper half: 0x10000 - 0x1FFFF    */
#define  EEPROM_SR_BP_WPWM        0x0CU     /* Write-protected Whole memory: 0x00000 - 0x1FFFF  */
/*------------------------ Macros --------------------------------------*/
/*------------------------- Enum ---------------------------------------*/
typedef enum
{
  EEPROM_OK,              /* Command done and bus is free for new one */
  EEPROM_BUSY,            /* Some process in progress                 */
  EEPROM_WRITE_DISABLE,   /* Write disable by W pin                   */
  EEPROM_OVER_PAGE,    	  /* Try to write over one page               */
  EEPROM_OVER_ROLL,       /* Try to read over end of memory           */
  EEPROM_ADR_ERROR,       /* Try to get access over address           */
  EEPROM_INIT_ERROR,      /* No SPI structure                         */
  EEPROM_SIZE_ERROR,      /* Size control error                       */
  EEPROM_ERROR,           /* Other errors                             */
} EEPROM_STATUS;

typedef enum
{
  EEPROM_PROTECT_NONE,    /* Write-protected none          */
  EEPROM_PROTECT_QUARTER, /* Write-protected Upper quarter */
  EEPROM_PROTECT_HALF,    /* Write-protected Upper half    */
  EEPROM_PROTECT_WHOLE    /* Write-protected Whole memory  */
} EEPROM_PROTECT_TYPE;

typedef enum
{
  EEPROM_SR_IDLE,         /* Idle state                                             */
  EEPROM_SR_BUSY,         /* Write process in progress                              */
  EEPROM_SR_WRITE_READY,  /* Memory is enabled for writing                          */
  EEPROM_SR_UNBLOCK,      /* All memory is unprotected for writing BP0 = 0, BP1 = 0 */
  EEPROM_SR_BLOCK,        /* All memory is protected for writing BP0 = 1, BP1 = 1   */
} EEPROM_SR_STATE;
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  /* EEPROM */
  FunctionalState     SRWD;      /* Is EEPROM have Status Register Write Disable bit */
  FunctionalState     ID;        /* Is EEPROM have identification page               */
  EEPROM_PROTECT_TYPE protect;   /* Define protect level                             */
  uint32_t            size;      /* Size of EEPROM in bytes                          */
  uint32_t            page;      /* Page size                                        */
  uint8_t             frequensy; /* Frequency of SPI                                 */
  uint32_t            timeout;   /* Timeout for SPI transactions                     */
  uint8_t             adrSize;   /* Address size in bytes                            */
  /* Hardware */
  SPI_HandleTypeDef*  spi;
  GPIO_TYPE           cs;
} EEPROM_TYPE;

/*----------------------- Chips ----------------------------------------*/
#define  M95Mxx_SRWD            1U
#define  M95Mxx_ID              0U
#define  M95Mxx_PROTECTION      EEPROM_PROTECT_NONE
#define  M95M01_SIZE            0x20000 /* bytes (128Kb) */
#define  M95M02_SIZE            0x40000 /* bytes (256Kb) */
#define  M95M04_SIZE            0x80000 /* bytes (512Kb) */
#define  M95Mxx_PAGE_SIZE       256U    /* bytes         */
#define  M95Mxx_CLOCK_FREQ      16U     /* MHz           */
#define  M95Mxx_TIMEOUT         10U     /* Ticks         */
#define  M95Mxx_ADR_SIZE        3U      /* bytes         */

#define  AA02E48_SRWD           0U
#define  AA02E48_ID             0U
#define  AA02E48_PROTECTION     EEPROM_PROTECT_QUARTER
#define  AA02E48_SIZE           256U    /* bytes */
#define  AA02E48_PAGE_SIZE      16U     /* bytes */
#define  AA02E48_CLOCK_FREQ     5U      /* MHz   */
#define  AA02E48_TIMEOUT        10U     /* Ticks */
#define  AA02E48_ADR_SIZE       1U      /* bytes */
/*----------------------- Functions ------------------------------------*/
#if defined ( UNIT_TEST )
  void          vEEPROMmakeAdr ( uint32_t adr, uint8_t* buffer, uint8_t length );
  EEPROM_STATUS eEEPROMwrite ( const EEPROM_TYPE* eeprom, uint8_t cmd, const uint32_t* adr, uint8_t* data, uint16_t size );
  EEPROM_STATUS eEEPROMread ( const EEPROM_TYPE* eeprom, uint8_t cmd, const uint32_t* adr, uint8_t* data, uint16_t size );
  EEPROM_STATUS eEEPROMwriteEnable ( const EEPROM_TYPE* eeprom );
  EEPROM_STATUS eEEPROMwriteDisable ( const EEPROM_TYPE* eeprom );
  EEPROM_STATUS eEEPROMreadSR ( const EEPROM_TYPE* eeprom, EEPROM_SR_STATE* status );
  EEPROM_STATUS eEEPROMwriteSR ( const EEPROM_TYPE* eeprom, uint8_t data );
  EEPROM_STATUS eEEPROMunblock ( const EEPROM_TYPE* eeprom );
  EEPROM_STATUS eEEPROMsetProtect ( const EEPROM_TYPE* eeprom, EEPROM_PROTECT_TYPE level );
  EEPROM_STATUS eEEPROMgetProtect ( const EEPROM_TYPE* eeprom, EEPROM_PROTECT_TYPE* level );
  EEPROM_STATUS eEEPROMpoolUntil ( const EEPROM_TYPE* eeprom, EEPROM_SR_STATE target );
  EEPROM_STATUS eEEPROMWriteData ( const EEPROM_TYPE* eeprom, const uint32_t* adr, uint8_t* data, uint8_t len );
#endif
EEPROM_STATUS eEEPROMInit ( const EEPROM_TYPE* eeprom ); /* Installation of EEPROM */
EEPROM_STATUS eEEPROMreadMemory ( const EEPROM_TYPE* eeprom, uint32_t adr, uint8_t* data, uint16_t len );        /* Read memory of EEPROM */
EEPROM_STATUS eEEPROMwriteMemory ( const EEPROM_TYPE* eeprom, uint32_t adr, uint8_t* data, uint16_t len ); /* Write data to memory of EEPROM */
/*----------------------------------------------------------------------*/
#endif /* INC_EEPROM_H_ */
