/*
 * Configuration file from 'config.csv'
 * Make time: 2020-08-24 08:03:33
 */
/*----------------------------------------------------------------------*/
#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
/*------------------------ Define --------------------------------------*/
#define   MAX_UNITS_LENGTH             4U
#define   SETTING_REGISTER_NUMBER      125U
#define   FILDS_TO_WRITE_NUMBER        3U
#define   BROADCAST_ADR                0xFFFFU
#define   MAX_VALUE_LENGTH             16U
#define   CONFIG_MAX_SIZE              77U     // bytes
#define   CONFIG_TOTAL_SIZE            1614U   // bytes

#define   CONFIG_REG_ADR_STR           "adr"
#define   CONFIG_REG_SCALE_STR         "scale"
#define   CONFIG_REG_VALUE_STR         "value"
#define   CONFIG_REG_MIN_STR           "min"
#define   CONFIG_REG_MAX_STR           "max"
#define   CONFIG_REG_UNITS_STR         "units"
#define   CONFIG_REG_TYPE_STR          "type"
#define   CONFIG_REG_RW_STATUS         "rw"
#define   CONFIG_REG_LEN_STR           "len"
#define   CONFIG_REG_BIT_MAP_SIZE_STR  "bitMapSize"
#define   CONFIG_REG_BIT_MAP_STR       "bit"

#define   BIT_MAP_MASK_STR             "mask"
#define   BIT_MAP_SHIFT_STR            "shift"
#define   BIT_MAP_MIN_STR              "min"
#define   BIT_MAP_MAX_STR              "max"
/*----------------------- Structures -----------------------------------*/
typedef enum
{
  CONFIG_NO    = 0x00U,
  CONFIG_VALUE = 0x01U,
  CONFIG_SCALE = 0x02U,
  CONFIG_UNITS = 0x03U,
} CONFIG_FILD;

typedef struct
{
  uint16_t  mask;
  uint8_t   shift;
} eConfigBitMap;

typedef struct
{
  uint16_t         adr;         // R
  uint16_t         min;         // R
  uint16_t         max;         // R
  uint16_t         type;        // R
  uint8_t          len;         // R
  uint8_t          bitMapSize;  // R
} eConfigAttributes;

typedef struct
{
  eConfigAttributes* atrib;                   // R
  signed char        scale;                   // RW
  uint16_t*          value;                   // RW
  uint16_t           units[MAX_UNITS_LENGTH]; // RW
  eConfigBitMap*     bitMap;                  // RW
} eConfigReg;
/*------------------------- Extern -------------------------------------*/
extern eConfigReg versionController;
extern eConfigReg versionFirmware;
extern eConfigReg serialNumber;
extern eConfigReg displayBrightnesLevel;
extern eConfigReg displayContarstLevel;
extern eConfigReg displaySleepDelay;
extern eConfigReg hysteresisLevel;
extern eConfigReg moduleSetup;
extern eConfigReg oilPressureSetup;
extern eConfigReg oilPressureAlarmLevel;
extern eConfigReg oilPressurePreAlarmLevel;
extern eConfigReg coolantTempSetup;
extern eConfigReg coolantHightTempAlarmLevel;
extern eConfigReg coolantHightTempPreAlarmLevel;
extern eConfigReg coolantTempHeaterOffLevel;
extern eConfigReg coolantTempHeaterOnLevel;
extern eConfigReg coolantTempCoolerOffLevel;
extern eConfigReg coolantTempCoolerOnLevel;
extern eConfigReg fuelLevelSetup;
extern eConfigReg fuelLevelLowAlarmLevel;
extern eConfigReg fuelLevelLowAlarmDelay;
extern eConfigReg fuelLevelLowPreAlarmLevel;
extern eConfigReg fuelLevelLowPreAlarmDelay;
extern eConfigReg fuelLevelHightPreAlarmLevel;
extern eConfigReg fuelLevelHightPreAlarmDelay;
extern eConfigReg fuelLevelHightAlarmLevel;
extern eConfigReg fuelLevelHightAlarmDelay;
extern eConfigReg fuelPumpOnLevel;
extern eConfigReg fuelPumpOffLevel;
extern eConfigReg speedSetup;
extern eConfigReg speedToothNumber;
extern eConfigReg speedLowAlarmLevel;
extern eConfigReg speedHightAlarmLevel;
extern eConfigReg diaSetup;
extern eConfigReg diaDelay;
extern eConfigReg diaMessage;
extern eConfigReg dibSetup;
extern eConfigReg dibDelay;
extern eConfigReg dibMessage;
extern eConfigReg dicSetup;
extern eConfigReg dicDelay;
extern eConfigReg dicMessage;
extern eConfigReg didSetup;
extern eConfigReg didDelay;
extern eConfigReg didMessage;
extern eConfigReg doSetup;
extern eConfigReg doabType;
extern eConfigReg docdType;
extern eConfigReg doefType;
extern eConfigReg timerMainsTransientDelay;
extern eConfigReg timerStartDelay;
extern eConfigReg timerCranking;
extern eConfigReg timerCrankDelay;
extern eConfigReg timerStartupIdleTime;
extern eConfigReg timerNominalRPMDelay;
extern eConfigReg timerSafetyOnDelay;
extern eConfigReg timerWarming;
extern eConfigReg timerTransferDelay;
extern eConfigReg timerGenBreakerTripPulse;
extern eConfigReg timerGenBreakerClosePulse;
extern eConfigReg timerMainsBreakerTripPulse;
extern eConfigReg timerMainsBreakerClosePulse;
extern eConfigReg timerReturnDelay;
extern eConfigReg timerCooling;
extern eConfigReg timerCoolingIdle;
extern eConfigReg timerSolenoidHold;
extern eConfigReg timerFailStopDelay;
extern eConfigReg timerGenTransientDelay;
extern eConfigReg genSetup;
extern eConfigReg genRatedActivePowerLevel;
extern eConfigReg genRatedReactivePowerLevel;
extern eConfigReg genRatedApparentPowerLevel;
extern eConfigReg genRatedFrequencyLevel;
extern eConfigReg genCurrentPrimaryLevel;
extern eConfigReg genCurrentFullLoadRatingLevel;
extern eConfigReg genAlarms;
extern eConfigReg genUnderVoltageAlarmLevel;
extern eConfigReg genUnderVoltagePreAlarmLevel;
extern eConfigReg genOverVoltagePreAlarmLevel;
extern eConfigReg genOverVoltageAlarmLevel;
extern eConfigReg genUnderFrequencyAlarmLevel;
extern eConfigReg genUnderFrequencyPreAlarmLevel;
extern eConfigReg genOverFrequencyPreAlarmLevel;
extern eConfigReg genOverFrequencyAlarmLevel;
extern eConfigReg genOverCurrentThermalProtectionLevel;
extern eConfigReg genOverCurrentCutoffLevel;
extern eConfigReg genOverCurrentAlarmLevel;
extern eConfigReg genOverCurrentAlarmDelay;
extern eConfigReg genCurrentOverloadProtectionLevel;
extern eConfigReg genCurrentOverloadProtectionDelay;
extern eConfigReg genCurrentOverPhaseImbalanceLevel;
extern eConfigReg genCurrentOverPhaseImbalanceDelay;
extern eConfigReg mainsSetup;
extern eConfigReg mainsAlarms;
extern eConfigReg mainsUnderVoltageAlarmLevel;
extern eConfigReg mainsOverVoltageAlarmLevel;
extern eConfigReg mainsUnderFrequencyAlarmLevel;
extern eConfigReg mainsOverFrequencyAlarmLevel;
extern eConfigReg engineSetup;
extern eConfigReg enginePreHeatLevel;
extern eConfigReg enginePreHeatDelay;
extern eConfigReg engineWorkTimeInput;
extern eConfigReg engineStartsNumberInput;
extern eConfigReg starterStopSetup;
extern eConfigReg starterStopGenFreqLevel;
extern eConfigReg starterStopOilPressureLevel;
extern eConfigReg starterStopChargeAlternatorLevel;
extern eConfigReg starterStopSpeedLevel;
extern eConfigReg batteryAlarms;
extern eConfigReg batteryUnderVoltageLevel;
extern eConfigReg batteryUnderVoltageDelay;
extern eConfigReg batteryOverVoltageLevel;
extern eConfigReg batteryOverVoltageDelay;
extern eConfigReg batteryChargeShutdownLevel;
extern eConfigReg batteryChargeShutdownDelay;
extern eConfigReg batteryChargeWarningLevel;
extern eConfigReg batteryChargeWarningDelay;
extern eConfigReg maintenanceAlarms;
extern eConfigReg maintenanceAlarmOilTime;
extern eConfigReg maintenanceAlarmAirTime;
extern eConfigReg maintenanceAlarmFuelTime;
extern const eConfigReg maintenanceAlarmOilTimeLeft;
extern const eConfigReg maintenanceAlarmAirTimeLeft;
extern const eConfigReg maintenanceAlarmFuelTimeLeft;
extern eConfigReg logSetup;
extern eConfigReg* configReg[SETTING_REGISTER_NUMBER];
/*----------------------------------------------------------------------*/
#endif /* INC_CONFIG_H_ */
