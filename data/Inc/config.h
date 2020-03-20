/*
 * Configuration file from 'config.csv'
 * Make time: 2020-03-19 17:22:54
 */
/*----------------------------------------------------------------------*/
#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
/*------------------------ Define --------------------------------------*/
#define   MAX_UNITS_LENGTH             4U
#define   SETTING_REGISTER_NUMBER      103U

#define   CONFIG_REG_PAGE_STR          "page"
#define   CONFIG_REG_ADR_STR           "adr"
#define   CONFIG_REG_SCALE_STR         "scale"
#define   CONFIG_REG_VALUE_STR         "value"
#define   CONFIG_REG_MIN_STR           "min"
#define   CONFIG_REG_MAX_STR           "max"
#define   CONFIG_REG_UNITS_STR         "units"
#define   CONFIG_REG_TYPE_STR          "type"
#define   CONFIG_REG_LEN_STR           "len"
#define   CONFIG_REG_BIT_MAP_SIZE_STR  "bitMapSize"
#define   CONFIG_REG_BIT_MAP_STR       "bit"

#define   BIT_MAP_MASK_STR             "mask"
#define   BIT_MAP_SHIFT_STR            "shift"
#define   BIT_MAP_MIN_STR              "min"
#define   BIT_MAP_MAX_STR              "max"
/*----------------------- Structures -----------------------------------*/
typedef struct
{
  uint16_t  mask;
  uint8_t   shift;
  uint8_t   min;
  uint8_t   max;
} eConfigBitMap;

typedef struct
{
  uint16_t         page;
  uint16_t         adr;
  signed char      scale;
  uint16_t         value;
  uint16_t         min;
  uint16_t         max;
  char             units[MAX_UNITS_LENGTH];
  char             type;
  uint8_t          len;
  uint8_t          bitMapSize;
  eConfigBitMap*   bitMap;
} eConfigReg;
/*------------------------- Extern -------------------------------------*/
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
extern eConfigReg diaSetup;
extern eConfigReg diaDelay;
extern eConfigReg dibSetup;
extern eConfigReg dibDelay;
extern eConfigReg dicSetup;
extern eConfigReg dicDelay;
extern eConfigReg didSetup;
extern eConfigReg didDelay;
extern eConfigReg doSetup;
extern eConfigReg doabType;
extern eConfigReg dodType;
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
extern eConfigReg timerBreakerTripPulse;
extern eConfigReg timerBreakerClosePulse;
extern eConfigReg timerReturnDelay;
extern eConfigReg timerCooling;
extern eConfigReg timerCoolingIdle;
extern eConfigReg timerSolenoidHold;
extern eConfigReg timerFailStopDelay;
extern eConfigReg timerGenTransientDelay;
extern eConfigReg genSetup;
extern eConfigReg genRatedActivePower;
extern eConfigReg genRatedReactivePower;
extern eConfigReg genRatedApparentPower;
extern eConfigReg genRatedFrequency;
extern eConfigReg genCurrentPrimary;
extern eConfigReg genCurrentFullLoadRating;
extern eConfigReg genAlarms;
extern eConfigReg genUnderVoltageAlarmLevel;
extern eConfigReg genUnderVoltagePreAlarmLevel;
extern eConfigReg genOverVoltagePreAlarmLevel;
extern eConfigReg genOverVoltageAlarmLevel;
extern eConfigReg genUnderFrequencyAlrmLevel;
extern eConfigReg genUnderFrequencyPreAlrmLevel;
extern eConfigReg genOverFrequencyPreAlrmLevel;
extern eConfigReg genOverFrequencyAlrmLevel;
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
extern eConfigReg enginePreHeatOn;
extern eConfigReg enginePreHeatDuration;
extern eConfigReg enginePostHeatOn;
extern eConfigReg enginePostHeatDuration;
extern eConfigReg crankSetup;
extern eConfigReg crankDisconnectgenFreqLevel;
extern eConfigReg crankDisconnectOilPressureLevel;
extern eConfigReg crankDisconnectChargeAlternatorLevel;
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
extern eConfigReg maintenanceAlarmOilEngineRunTime;
extern eConfigReg maintenanceAlarmAirEngineRunTime;
extern eConfigReg maintenanceAlarmFuelEngineRunTime;
extern eConfigReg* configReg[SETTING_REGISTER_NUMBER];
/*----------------------------------------------------------------------*/
#endif /* INC_CONFIG_H_ */
