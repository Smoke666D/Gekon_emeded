/*
 * Configuration file from 'config.csv'
 * Make time: 2020-02-21 08:00:54
 */
/*----------------------------------------------------------------------*/
#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
/*------------------------ Define --------------------------------------*/
#define   MAX_UNITS_LENGTH             3U
#define   SETTING_REGISTER_NUMBER      79U
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
  float            scale;
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
extern eConfigReg coolantTempSetup;
extern eConfigReg fuelLevelAlarms;
extern eConfigReg fuelLevelLowAlarmLevel;
extern eConfigReg fuelLevelLowAlarmDelay;
extern eConfigReg fuelLevelLowPreAlarmLevelTrip;
extern eConfigReg fuelLevelLowPreAlarmLevelReturn;
extern eConfigReg fuelLevelLowPreAlarmLevelDelay;
extern eConfigReg fuelLevelHightPreAlarmLevelReturn;
extern eConfigReg fuelFueLevelHightPreAlarmLevelTrip;
extern eConfigReg fuelLevelHightPreAlarmLevelDelay;
extern eConfigReg fuelLevelHightAlarmLevel;
extern eConfigReg fuelLevelHightAlarmDelay;
extern eConfigReg diaSetup;
extern eConfigReg diaDelay;
extern eConfigReg dibSetup;
extern eConfigReg dibDelay;
extern eConfigReg dicSetup;
extern eConfigReg dicDelay;
extern eConfigReg didSetup;
extern eConfigReg didDelay;
extern eConfigReg diabType;
extern eConfigReg dicdType;
extern eConfigReg dirfType;
extern eConfigReg timerMainsTransientDelay;
extern eConfigReg timerStartDelay;
extern eConfigReg timerDelayCrank;
extern eConfigReg timerCranling;
extern eConfigReg timerSmokeLimit;
extern eConfigReg timerSmokeLimitOff;
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
extern eConfigReg timergenTransientDelay;
extern eConfigReg genSetup;
extern eConfigReg genAlarms;
extern eConfigReg genUnderVoltageAlarmTrip;
extern eConfigReg genUnderVoltagepreAlarmTrip;
extern eConfigReg genVoltageLoad;
extern eConfigReg genVoltageNominal;
extern eConfigReg genOverVoltagePreAlarmReturn;
extern eConfigReg genOverVoltagePreAlarmTrip;
extern eConfigReg genOverVoltageShutdownTrip;
extern eConfigReg genUnderFrequencyAlrmTrip;
extern eConfigReg genUnderFrequencyPreAlrmTrip;
extern eConfigReg genFrequencyLoad;
extern eConfigReg genFrequencyNominal;
extern eConfigReg genOverFrequencyPreAlrmReturn;
extern eConfigReg genOverFrequencyPreAlrmTrip;
extern eConfigReg genOverFrequencyShutdownTrip;
extern eConfigReg genCurrentPrimary;
extern eConfigReg genCurrentFullLoadRating;
extern eConfigReg genOverCurrentAlarmDelay;
extern eConfigReg genOverCurrentAlarmTrip;
extern eConfigReg genCurrentRating;
extern eConfigReg genCurrentOverloadProtectionTrip;
extern eConfigReg genCurrentOverloadProtectionDelay;
extern eConfigReg mainsSetup;
extern eConfigReg mainsAlarms;
extern eConfigReg mainsUnderVoltageAlarmTrip;
extern eConfigReg mainsUnderVoltageAlarmReturn;
extern eConfigReg mainsOverVoltageAlarmReturn;
extern eConfigReg mainsOverVoltageAlarmTrip;
extern eConfigReg mainsUnderFrequencyAlarmTrip;
extern eConfigReg mainsUnderFrequencyAlarmReturn;
extern eConfigReg mainsOverFrequencyAlarmReturn;
extern eConfigReg mainsOverFrequencyAlarmTrip;
extern eConfigReg engineSetup;
extern eConfigReg enginePreHeatOn;
extern eConfigReg enginePreHeatDuration;
extern eConfigReg enginePostHeatOn;
extern eConfigReg enginePostHeatDuration;
extern eConfigReg crankSetup;
extern eConfigReg crankDisconnectgenFreq;
extern eConfigReg crankDisconnectEngineSpeed;
extern eConfigReg crankDisconnectOilPressure;
extern eConfigReg crankDisconnectOilPressureDelay;
extern eConfigReg crankDisconnectChargeAlternator;
extern eConfigReg batteryAlarms;
extern eConfigReg batteryUnderVoltageWarning;
extern eConfigReg batteryUnderVoltageReturn;
extern eConfigReg batteryUnderVoltageDelay;
extern eConfigReg batteryOverVoltageReturn;
extern eConfigReg batteryOverVoltageWarning;
extern eConfigReg batteryOverVoltageDelay;
extern eConfigReg batteryChargeShutdownTrip;
extern eConfigReg batteryChargeShutdownDelay;
extern eConfigReg batteryChargeWarningTrip;
extern eConfigReg batteryChargeWarningDelay;
extern eConfigReg maintenanceAlarms;
extern eConfigReg maintenanceAlarmOilEngineRunTime;
extern eConfigReg maintenanceAlarmAirEngineRunTime;
extern eConfigReg maintenanceAlarmFuelEngineRunTime;
/*----------------------------------------------------------------------*/
#endif /* INC_CONFIG_H_ */
