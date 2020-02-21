/*
 * Configuration file from 'config.csv'
 * Make time: 2020-02-21 11:54:22
 */
#include   "config.h"

eConfigBitMap moduleSetupBitMap[2U] = 
{
   { 1U, 0U, 0U, 1U },     // moduleType
   { 2U, 1U, 0U, 1U },     // alarmAllBlock
};
eConfigReg moduleSetup =
{
   .page       = 0U,
   .adr        = 0U,
   .scale      = 1U,
   .value      = 3U,
   .min        = 0U,
   .max        = 3U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 2U,
   .bitMap     = moduleSetupBitMap
};
eConfigBitMap oilPressureSetupBitMap[3U] = 
{
   { 1U, 0U, 0U, 1U },     // oilPressureInputType
   { 30U, 1U, 0U, 11U },     // oilPressureSensorType
   { 32U, 5U, 0U, 1U },     // oilPressureEnbOpenCircuitAlarm
};
eConfigReg oilPressureSetup =
{
   .page       = 0U,
   .adr        = 1U,
   .scale      = 1U,
   .value      = 36U,
   .min        = 0U,
   .max        = 63U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 3U,
   .bitMap     = oilPressureSetupBitMap
};
eConfigBitMap coolantTempSetupBitMap[3U] = 
{
   { 1U, 0U, 0U, 1U },     // coolantTempInputType
   { 30U, 1U, 0U, 11U },     // coolantTempSensorType
   { 32U, 5U, 0U, 1U },     // coolantTempEnbOpenCircuitAlarm
};
eConfigReg coolantTempSetup =
{
   .page       = 0U,
   .adr        = 2U,
   .scale      = 1U,
   .value      = 39U,
   .min        = 0U,
   .max        = 63U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 3U,
   .bitMap     = coolantTempSetupBitMap
};
eConfigBitMap fuelLevelAlarmsBitMap[8U] = 
{
   { 1U, 0U, 0U, 1U },     // fuelLevelSensorType
   { 14U, 1U, 0U, 5U },     // fuelLevelInputType
   { 16U, 4U, 0U, 1U },     // fuelLevelLowAlarmEnb
   { 32U, 5U, 0U, 1U },     // fuelLevelLowAlarmAction
   { 64U, 6U, 0U, 1U },     // fuelLevelLowPreAlarmEnb
   { 128U, 7U, 0U, 1U },     // fuelLevelHightPreAlarmLevelEnb
   { 256U, 8U, 0U, 1U },     // fuelLevelHightAlarmEnb
   { 512U, 9U, 0U, 1U },     // fuelLevelHightAlarmAction
};
eConfigReg fuelLevelAlarms =
{
   .page       = 0U,
   .adr        = 3U,
   .scale      = 1U,
   .value      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 8U,
   .bitMap     = fuelLevelAlarmsBitMap
};
eConfigReg fuelLevelLowAlarmLevel =
{
   .page       = 0U,
   .adr        = 4U,
   .scale      = 1U,
   .value      = 10U,
   .min        = 0U,
   .max        = 95U,
   .units      = {'%', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg fuelLevelLowAlarmDelay =
{
   .page       = 0U,
   .adr        = 5U,
   .scale      = 1U,
   .value      = 0U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg fuelLevelLowPreAlarmLevelTrip =
{
   .page       = 0U,
   .adr        = 6U,
   .scale      = 1U,
   .value      = 25U,
   .min        = 1U,
   .max        = 96U,
   .units      = {'%', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg fuelLevelLowPreAlarmLevelReturn =
{
   .page       = 0U,
   .adr        = 7U,
   .scale      = 1U,
   .value      = 30U,
   .min        = 2U,
   .max        = 97U,
   .units      = {'%', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg fuelLevelLowPreAlarmLevelDelay =
{
   .page       = 0U,
   .adr        = 8U,
   .scale      = 1U,
   .value      = 0U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg fuelLevelHightPreAlarmLevelReturn =
{
   .page       = 0U,
   .adr        = 9U,
   .scale      = 1U,
   .value      = 65U,
   .min        = 3U,
   .max        = 98U,
   .units      = {'%', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg fuelFueLevelHightPreAlarmLevelTrip =
{
   .page       = 0U,
   .adr        = 10U,
   .scale      = 1U,
   .value      = 70U,
   .min        = 4U,
   .max        = 99U,
   .units      = {'%', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg fuelLevelHightPreAlarmLevelDelay =
{
   .page       = 0U,
   .adr        = 11U,
   .scale      = 1U,
   .value      = 0U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg fuelLevelHightAlarmLevel =
{
   .page       = 0U,
   .adr        = 12U,
   .scale      = 1U,
   .value      = 90U,
   .min        = 5U,
   .max        = 100U,
   .units      = {'%', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg fuelLevelHightAlarmDelay =
{
   .page       = 0U,
   .adr        = 13U,
   .scale      = 1U,
   .value      = 0U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigBitMap diaSetupBitMap[4U] = 
{
   { 31U, 0U, 0U, 27U },     // diaFunction
   { 32U, 5U, 0U, 1U },     // diaPolarity
   { 192U, 6U, 0U, 3U },     // diaAction
   { 768U, 8U, 0U, 4U },     // diaArming
};
eConfigReg diaSetup =
{
   .page       = 0U,
   .adr        = 14U,
   .scale      = 1U,
   .value      = 19U,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = diaSetupBitMap
};
eConfigReg diaDelay =
{
   .page       = 0U,
   .adr        = 15U,
   .scale      = 1U,
   .value      = 0U,
   .min        = 0U,
   .max        = 60U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigBitMap dibSetupBitMap[4U] = 
{
   { 31U, 0U, 0U, 27U },     // dibFunction
   { 32U, 5U, 0U, 1U },     // dibPolarity
   { 192U, 6U, 0U, 3U },     // dibAction
   { 768U, 8U, 0U, 4U },     // dibArming
};
eConfigReg dibSetup =
{
   .page       = 0U,
   .adr        = 16U,
   .scale      = 1U,
   .value      = 8U,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = dibSetupBitMap
};
eConfigReg dibDelay =
{
   .page       = 0U,
   .adr        = 17U,
   .scale      = 1U,
   .value      = 0U,
   .min        = 0U,
   .max        = 60U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigBitMap dicSetupBitMap[4U] = 
{
   { 31U, 0U, 0U, 27U },     // dicFunction
   { 32U, 5U, 0U, 1U },     // dicPolarity
   { 192U, 6U, 0U, 3U },     // dicAction
   { 768U, 8U, 0U, 4U },     // dicArming
};
eConfigReg dicSetup =
{
   .page       = 0U,
   .adr        = 18U,
   .scale      = 1U,
   .value      = 2U,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = dicSetupBitMap
};
eConfigReg dicDelay =
{
   .page       = 0U,
   .adr        = 19U,
   .scale      = 1U,
   .value      = 0U,
   .min        = 0U,
   .max        = 60U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigBitMap didSetupBitMap[4U] = 
{
   { 31U, 0U, 0U, 27U },     // didFunction
   { 32U, 5U, 0U, 1U },     // didPolarity
   { 192U, 6U, 0U, 3U },     // didAction
   { 768U, 8U, 0U, 4U },     // didArming
};
eConfigReg didSetup =
{
   .page       = 0U,
   .adr        = 20U,
   .scale      = 1U,
   .value      = 192U,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = didSetupBitMap
};
eConfigReg didDelay =
{
   .page       = 0U,
   .adr        = 21U,
   .scale      = 1U,
   .value      = 0U,
   .min        = 0U,
   .max        = 60U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigBitMap diabTypeBitMap[2U] = 
{
   { 255U, 0U, 0U, 255U },     // diaType
   { 65280U, 8U, 0U, 255U },     // diaType
};
eConfigReg diabType =
{
   .page       = 0U,
   .adr        = 22U,
   .scale      = 1U,
   .value      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 2U,
   .bitMap     = diabTypeBitMap
};
eConfigBitMap dicdTypeBitMap[2U] = 
{
   { 255U, 0U, 0U, 255U },     // dicType
   { 65280U, 8U, 0U, 255U },     // didType
};
eConfigReg dicdType =
{
   .page       = 0U,
   .adr        = 23U,
   .scale      = 1U,
   .value      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 2U,
   .bitMap     = dicdTypeBitMap
};
eConfigBitMap dirfTypeBitMap[2U] = 
{
   { 255U, 0U, 0U, 255U },     // dieType
   { 65280U, 8U, 0U, 255U },     // difType
};
eConfigReg dirfType =
{
   .page       = 0U,
   .adr        = 24U,
   .scale      = 1U,
   .value      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 2U,
   .bitMap     = dirfTypeBitMap
};
eConfigReg timerMainsTransientDelay =
{
   .page       = 0U,
   .adr        = 25U,
   .scale      = 1U,
   .value      = 2U,
   .min        = 0U,
   .max        = 30U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerStartDelay =
{
   .page       = 0U,
   .adr        = 26U,
   .scale      = 1U,
   .value      = 5U,
   .min        = 0U,
   .max        = 36000U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerDelayCrank =
{
   .page       = 0U,
   .adr        = 27U,
   .scale      = 1U,
   .value      = 10U,
   .min        = 3U,
   .max        = 60U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerCranling =
{
   .page       = 0U,
   .adr        = 28U,
   .scale      = 1U,
   .value      = 10U,
   .min        = 3U,
   .max        = 60U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerSmokeLimit =
{
   .page       = 0U,
   .adr        = 29U,
   .scale      = 1U,
   .value      = 0U,
   .min        = 0U,
   .max        = 900U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerSmokeLimitOff =
{
   .page       = 0U,
   .adr        = 30U,
   .scale      = 1U,
   .value      = 0U,
   .min        = 0U,
   .max        = 60U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerSafetyOnDelay =
{
   .page       = 0U,
   .adr        = 31U,
   .scale      = 1U,
   .value      = 10U,
   .min        = 0U,
   .max        = 60U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerWarming =
{
   .page       = 0U,
   .adr        = 32U,
   .scale      = 1U,
   .value      = 1U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerTransferDelay =
{
   .page       = 0U,
   .adr        = 33U,
   .scale      = 0.1F,
   .value      = 6U,
   .min        = 0U,
   .max        = 6000U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerBreakerTripPulse =
{
   .page       = 0U,
   .adr        = 34U,
   .scale      = 0.1F,
   .value      = 5U,
   .min        = 0U,
   .max        = 50U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerBreakerClosePulse =
{
   .page       = 0U,
   .adr        = 35U,
   .scale      = 0.1F,
   .value      = 5U,
   .min        = 0U,
   .max        = 50U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerReturnDelay =
{
   .page       = 0U,
   .adr        = 36U,
   .scale      = 1U,
   .value      = 30U,
   .min        = 0U,
   .max        = 18000U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerCooling =
{
   .page       = 0U,
   .adr        = 37U,
   .scale      = 1U,
   .value      = 60U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerCoolingIdle =
{
   .page       = 0U,
   .adr        = 38U,
   .scale      = 1U,
   .value      = 0U,
   .min        = 0U,
   .max        = 900U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerSolenoidHold =
{
   .page       = 0U,
   .adr        = 39U,
   .scale      = 1U,
   .value      = 0U,
   .min        = 0U,
   .max        = 120U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerFailStopDelay =
{
   .page       = 0U,
   .adr        = 40U,
   .scale      = 1U,
   .value      = 30U,
   .min        = 10U,
   .max        = 120U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timergenTransientDelay =
{
   .page       = 0U,
   .adr        = 41U,
   .scale      = 0.1F,
   .value      = 0U,
   .min        = 0U,
   .max        = 300U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigBitMap genSetupBitMap[3U] = 
{
   { 1U, 0U, 0U, 1U },     // genAltematorFitted
   { 30U, 1U, 0U, 16U },     // genPoles
   { 224U, 5U, 0U, 5U },     // genAcSys
};
eConfigReg genSetup =
{
   .page       = 0U,
   .adr        = 42U,
   .scale      = 1U,
   .value      = 105U,
   .min        = 0U,
   .max        = 255U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 3U,
   .bitMap     = genSetupBitMap
};
eConfigBitMap genAlarmsBitMap[15U] = 
{
   { 1U, 0U, 0U, 1U },     // genUnderVoltageAlarmEnb
   { 2U, 1U, 0U, 1U },     // genUnderVoltagepreAlarmEnb
   { 4U, 2U, 0U, 1U },     // genOverVoltagePreAlarmEnb
   { 8U, 3U, 0U, 1U },     // genUnderFrequencyAlrmEnb
   { 16U, 4U, 0U, 1U },     // genUnderFrequencyPreAlrmEnb
   { 32U, 5U, 0U, 1U },     // genOverFrequencyPreAlrm
   { 64U, 6U, 0U, 1U },     // genOverFrequencyShutdownEnb
   { 128U, 7U, 0U, 1U },     // genCurrentPrimaryEnb
   { 256U, 8U, 0U, 1U },     // genCurrentSecondary
   { 512U, 9U, 0U, 1U },     // genCurrentPosition
   { 1024U, 10U, 0U, 1U },     // genOverCurrentImmWarning
   { 2048U, 11U, 0U, 1U },     // genOverCurrentDelayAlarmEnb
   { 12288U, 12U, 0U, 3U },     // genOverCurrentDelayAlarmAction
   { 16384U, 14U, 0U, 1U },     // genCurrentOverloadProtectionEnb
   { 32768U, 15U, 0U, 1U },     // genCurrentOverloadProtectionAction
};
eConfigReg genAlarms =
{
   .page       = 0U,
   .adr        = 43U,
   .scale      = 1U,
   .value      = 36351U,
   .min        = 0U,
   .max        = 65535U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 15U,
   .bitMap     = genAlarmsBitMap
};
eConfigReg genUnderVoltageAlarmTrip =
{
   .page       = 0U,
   .adr        = 44U,
   .scale      = 1U,
   .value      = 318U,
   .min        = 86U,
   .max        = 708U,
   .units      = {'V', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genUnderVoltagepreAlarmTrip =
{
   .page       = 0U,
   .adr        = 45U,
   .scale      = 1U,
   .value      = 339U,
   .min        = 88U,
   .max        = 710U,
   .units      = {'V', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genVoltageLoad =
{
   .page       = 0U,
   .adr        = 46U,
   .scale      = 1U,
   .value      = 358U,
   .min        = 90U,
   .max        = 711U,
   .units      = {'V', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genVoltageNominal =
{
   .page       = 0U,
   .adr        = 47U,
   .scale      = 1U,
   .value      = 398U,
   .min        = 91U,
   .max        = 713U,
   .units      = {'V', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genOverVoltagePreAlarmReturn =
{
   .page       = 0U,
   .adr        = 48U,
   .scale      = 1U,
   .value      = 439U,
   .min        = 93U,
   .max        = 715U,
   .units      = {'V', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genOverVoltagePreAlarmTrip =
{
   .page       = 0U,
   .adr        = 49U,
   .scale      = 1U,
   .value      = 458U,
   .min        = 95U,
   .max        = 717U,
   .units      = {'V', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genOverVoltageShutdownTrip =
{
   .page       = 0U,
   .adr        = 50U,
   .scale      = 1U,
   .value      = 479U,
   .min        = 96U,
   .max        = 718U,
   .units      = {'V', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genUnderFrequencyAlrmTrip =
{
   .page       = 0U,
   .adr        = 51U,
   .scale      = 0.1F,
   .value      = 400U,
   .min        = 0U,
   .max        = 744U,
   .units      = {'H', 'z', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genUnderFrequencyPreAlrmTrip =
{
   .page       = 0U,
   .adr        = 52U,
   .scale      = 0.1F,
   .value      = 420U,
   .min        = 1U,
   .max        = 745U,
   .units      = {'H', 'z', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genFrequencyLoad =
{
   .page       = 0U,
   .adr        = 53U,
   .scale      = 0.1F,
   .value      = 450U,
   .min        = 2U,
   .max        = 745U,
   .units      = {'H', 'z', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genFrequencyNominal =
{
   .page       = 0U,
   .adr        = 54U,
   .scale      = 0.1F,
   .value      = 500U,
   .min        = 2U,
   .max        = 747U,
   .units      = {'H', 'z', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genOverFrequencyPreAlrmReturn =
{
   .page       = 0U,
   .adr        = 55U,
   .scale      = 0.1F,
   .value      = 540U,
   .min        = 4U,
   .max        = 747U,
   .units      = {'H', 'z', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genOverFrequencyPreAlrmTrip =
{
   .page       = 0U,
   .adr        = 56U,
   .scale      = 0.1F,
   .value      = 550U,
   .min        = 5U,
   .max        = 749U,
   .units      = {'H', 'z', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genOverFrequencyShutdownTrip =
{
   .page       = 0U,
   .adr        = 57U,
   .scale      = 0.1F,
   .value      = 570U,
   .min        = 5U,
   .max        = 750U,
   .units      = {'H', 'z', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genCurrentPrimary =
{
   .page       = 0U,
   .adr        = 58U,
   .scale      = 1U,
   .value      = 600U,
   .min        = 5U,
   .max        = 8000U,
   .units      = {'A', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genCurrentFullLoadRating =
{
   .page       = 0U,
   .adr        = 59U,
   .scale      = 1U,
   .value      = 500U,
   .min        = 5U,
   .max        = 8000U,
   .units      = {'A', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genOverCurrentAlarmDelay =
{
   .page       = 0U,
   .adr        = 60U,
   .scale      = 1U,
   .value      = 60U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genOverCurrentAlarmTrip =
{
   .page       = 0U,
   .adr        = 61U,
   .scale      = 1U,
   .value      = 100U,
   .min        = 50U,
   .max        = 120U,
   .units      = {'%', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genCurrentRating =
{
   .page       = 0U,
   .adr        = 62U,
   .scale      = 1U,
   .value      = 200U,
   .min        = 1U,
   .max        = 20000U,
   .units      = {'A', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genCurrentOverloadProtectionTrip =
{
   .page       = 0U,
   .adr        = 63U,
   .scale      = 1U,
   .value      = 100U,
   .min        = 1U,
   .max        = 125U,
   .units      = {'%', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genCurrentOverloadProtectionDelay =
{
   .page       = 0U,
   .adr        = 64U,
   .scale      = 1U,
   .value      = 5U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigBitMap mainsSetupBitMap[3U] = 
{
   { 1U, 0U, 0U, 1U },     // mainsFailDetection
   { 2U, 1U, 0U, 1U },     // mainImmDropout
   { 28U, 2U, 0U, 5U },     // mainAcSys
};
eConfigReg mainsSetup =
{
   .page       = 0U,
   .adr        = 65U,
   .scale      = 1U,
   .value      = 13U,
   .min        = 0U,
   .max        = 31U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 3U,
   .bitMap     = mainsSetupBitMap
};
eConfigBitMap mainsAlarmsBitMap[4U] = 
{
   { 1U, 0U, 0U, 1U },     // mainsUnderVoltageAlarm
   { 2U, 1U, 0U, 1U },     // mainsOverVoltageAlarm
   { 4U, 2U, 0U, 1U },     // mainsUnderFrequencyAlarm
   { 8U, 3U, 0U, 1U },     // mainsOverFrequencyAlarm
};
eConfigReg mainsAlarms =
{
   .page       = 0U,
   .adr        = 66U,
   .scale      = 1U,
   .value      = 15U,
   .min        = 0U,
   .max        = 15U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = mainsAlarmsBitMap
};
eConfigReg mainsUnderVoltageAlarmTrip =
{
   .page       = 0U,
   .adr        = 67U,
   .scale      = 1U,
   .value      = 318U,
   .min        = 86U,
   .max        = 713U,
   .units      = {'V', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg mainsUnderVoltageAlarmReturn =
{
   .page       = 0U,
   .adr        = 68U,
   .scale      = 1U,
   .value      = 358U,
   .min        = 88U,
   .max        = 715U,
   .units      = {'V', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg mainsOverVoltageAlarmReturn =
{
   .page       = 0U,
   .adr        = 69U,
   .scale      = 1U,
   .value      = 438U,
   .min        = 90U,
   .max        = 717U,
   .units      = {'V', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg mainsOverVoltageAlarmTrip =
{
   .page       = 0U,
   .adr        = 70U,
   .scale      = 1U,
   .value      = 478U,
   .min        = 91U,
   .max        = 718U,
   .units      = {'V', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg mainsUnderFrequencyAlarmTrip =
{
   .page       = 0U,
   .adr        = 71U,
   .scale      = 0.1F,
   .value      = 450U,
   .min        = 0U,
   .max        = 747U,
   .units      = {'H', 'z', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg mainsUnderFrequencyAlarmReturn =
{
   .page       = 0U,
   .adr        = 72U,
   .scale      = 0.1F,
   .value      = 480U,
   .min        = 1U,
   .max        = 747U,
   .units      = {'H', 'z', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg mainsOverFrequencyAlarmReturn =
{
   .page       = 0U,
   .adr        = 73U,
   .scale      = 0.1F,
   .value      = 520U,
   .min        = 2U,
   .max        = 749U,
   .units      = {'H', 'z', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg mainsOverFrequencyAlarmTrip =
{
   .page       = 0U,
   .adr        = 74U,
   .scale      = 0.1F,
   .value      = 550U,
   .min        = 2U,
   .max        = 750U,
   .units      = {'H', 'z', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigBitMap engineSetupBitMap[3U] = 
{
   { 15U, 0U, 1U, 9U },     // engineStartAttempts
   { 16U, 4U, 0U, 1U },     // enginePreHeatEnb
   { 32U, 5U, 0U, 1U },     // enginePostHeatEnb
};
eConfigReg engineSetup =
{
   .page       = 0U,
   .adr        = 75U,
   .scale      = 1U,
   .value      = 51U,
   .min        = 0U,
   .max        = 63U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 3U,
   .bitMap     = engineSetupBitMap
};
eConfigReg enginePreHeatOn =
{
   .page       = 0U,
   .adr        = 76U,
   .scale      = 1U,
   .value      = 50U,
   .min        = 0U,
   .max        = 100U,
   .units      = {'C', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg enginePreHeatDuration =
{
   .page       = 0U,
   .adr        = 77U,
   .scale      = 1U,
   .value      = 0U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg enginePostHeatOn =
{
   .page       = 0U,
   .adr        = 78U,
   .scale      = 1U,
   .value      = 50U,
   .min        = 0U,
   .max        = 100U,
   .units      = {'C', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg enginePostHeatDuration =
{
   .page       = 0U,
   .adr        = 79U,
   .scale      = 1U,
   .value      = 0U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigBitMap crankSetupBitMap[3U] = 
{
   { 1U, 0U, 0U, 1U },     // crankDisconnectOilPressure
   { 2U, 1U, 0U, 1U },     // oilPressureCheckOnStart
   { 4U, 2U, 0U, 1U },     // crankDisconnectCharge
};
eConfigReg crankSetup =
{
   .page       = 0U,
   .adr        = 80U,
   .scale      = 1U,
   .value      = 2U,
   .min        = 0U,
   .max        = 7U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 3U,
   .bitMap     = crankSetupBitMap
};
eConfigReg crankDisconnectgenFreq =
{
   .page       = 0U,
   .adr        = 81U,
   .scale      = 0.1F,
   .value      = 210U,
   .min        = 0U,
   .max        = 400U,
   .units      = {'H', 'z', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg crankDisconnectEngineSpeed =
{
   .page       = 0U,
   .adr        = 82U,
   .scale      = 1U,
   .value      = 600U,
   .min        = 0U,
   .max        = 3000U,
   .units      = {'R', 'P', 'M'},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg crankDisconnectOilPressure =
{
   .page       = 0U,
   .adr        = 83U,
   .scale      = 0.1F,
   .value      = 20U,
   .min        = 5U,
   .max        = 40U,
   .units      = {'B', 'a', 'r'},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg crankDisconnectOilPressureDelay =
{
   .page       = 0U,
   .adr        = 84U,
   .scale      = 0.1F,
   .value      = 0U,
   .min        = 0U,
   .max        = 600U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg crankDisconnectChargeAlternator =
{
   .page       = 0U,
   .adr        = 85U,
   .scale      = 0.1F,
   .value      = 60U,
   .min        = 0U,
   .max        = 400U,
   .units      = {'V', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigBitMap batteryAlarmsBitMap[4U] = 
{
   { 1U, 0U, 0U, 1U },     // batteryUnderVoltageEnb
   { 2U, 1U, 0U, 1U },     // batteryOverVoltageEnb
   { 4U, 2U, 0U, 1U },     // batteryChargeShutdownEnb
   { 8U, 3U, 0U, 1U },     // batteryChargeWarningEnb
};
eConfigReg batteryAlarms =
{
   .page       = 0U,
   .adr        = 86U,
   .scale      = 1U,
   .value      = 15U,
   .min        = 0U,
   .max        = 15U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = batteryAlarmsBitMap
};
eConfigReg batteryUnderVoltageWarning =
{
   .page       = 0U,
   .adr        = 87U,
   .scale      = 0.1F,
   .value      = 100U,
   .min        = 0U,
   .max        = 397U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg batteryUnderVoltageReturn =
{
   .page       = 0U,
   .adr        = 88U,
   .scale      = 0.1F,
   .value      = 100U,
   .min        = 1U,
   .max        = 397U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg batteryUnderVoltageDelay =
{
   .page       = 0U,
   .adr        = 89U,
   .scale      = 1U,
   .value      = 60U,
   .min        = 0U,
   .max        = 86400U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg batteryOverVoltageReturn =
{
   .page       = 0U,
   .adr        = 90U,
   .scale      = 0.1F,
   .value      = 100U,
   .min        = 2U,
   .max        = 398U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg batteryOverVoltageWarning =
{
   .page       = 0U,
   .adr        = 91U,
   .scale      = 0.1F,
   .value      = 300U,
   .min        = 2U,
   .max        = 400U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg batteryOverVoltageDelay =
{
   .page       = 0U,
   .adr        = 92U,
   .scale      = 1U,
   .value      = 60U,
   .min        = 0U,
   .max        = 86400U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg batteryChargeShutdownTrip =
{
   .page       = 0U,
   .adr        = 93U,
   .scale      = 0.1F,
   .value      = 40U,
   .min        = 0U,
   .max        = 388U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg batteryChargeShutdownDelay =
{
   .page       = 0U,
   .adr        = 94U,
   .scale      = 1U,
   .value      = 5U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg batteryChargeWarningTrip =
{
   .page       = 0U,
   .adr        = 95U,
   .scale      = 0.1F,
   .value      = 60U,
   .min        = 1U,
   .max        = 390U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg batteryChargeWarningDelay =
{
   .page       = 0U,
   .adr        = 96U,
   .scale      = 1U,
   .value      = 5U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'s', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigBitMap maintenanceAlarmsBitMap[6U] = 
{
   { 1U, 0U, 0U, 1U },     // maintenanceAlarmOilEnb
   { 2U, 1U, 0U, 1U },     // maintenanceAlarmOilAction
   { 4U, 2U, 0U, 1U },     // maintenanceAlarmAirEnb
   { 8U, 3U, 0U, 1U },     // maintenanceAlarmAirAction
   { 16U, 4U, 0U, 1U },     // maintenanceAlarmFuelEnb
   { 32U, 5U, 0U, 1U },     // maintenanceAlarmFuelAction
};
eConfigReg maintenanceAlarms =
{
   .page       = 0U,
   .adr        = 97U,
   .scale      = 1U,
   .value      = 42U,
   .min        = 0U,
   .max        = 63U,
   .units      = {' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 6U,
   .bitMap     = maintenanceAlarmsBitMap
};
eConfigReg maintenanceAlarmOilEngineRunTime =
{
   .page       = 0U,
   .adr        = 98U,
   .scale      = 1U,
   .value      = 10U,
   .min        = 10U,
   .max        = 5000U,
   .units      = {'h', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg maintenanceAlarmAirEngineRunTime =
{
   .page       = 0U,
   .adr        = 99U,
   .scale      = 1U,
   .value      = 10U,
   .min        = 10U,
   .max        = 5000U,
   .units      = {'h', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg maintenanceAlarmFuelEngineRunTime =
{
   .page       = 0U,
   .adr        = 100U,
   .scale      = 1U,
   .value      = 10U,
   .min        = 10U,
   .max        = 5000U,
   .units      = {'h', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};

eConfigReg* configReg[SETTING_REGISTER_NUMBER] = { &moduleSetup, &oilPressureSetup, &coolantTempSetup, &fuelLevelAlarms, &fuelLevelLowAlarmLevel, &fuelLevelLowAlarmDelay, &fuelLevelLowPreAlarmLevelTrip, &fuelLevelLowPreAlarmLevelReturn, &fuelLevelLowPreAlarmLevelDelay, &fuelLevelHightPreAlarmLevelReturn, &fuelFueLevelHightPreAlarmLevelTrip, &fuelLevelHightPreAlarmLevelDelay, &fuelLevelHightAlarmLevel, &fuelLevelHightAlarmDelay, &diaSetup, &diaDelay, &dibSetup, &dibDelay, &dicSetup, &dicDelay, &didSetup, &didDelay, &diabType, &dicdType, &dirfType, &timerMainsTransientDelay, &timerStartDelay, &timerDelayCrank, &timerCranling, &timerSmokeLimit, &timerSmokeLimitOff, &timerSafetyOnDelay, &timerWarming, &timerTransferDelay, &timerBreakerTripPulse, &timerBreakerClosePulse, &timerReturnDelay, &timerCooling, &timerCoolingIdle, &timerSolenoidHold, &timerFailStopDelay, &timergenTransientDelay, &genSetup, &genAlarms, &genUnderVoltageAlarmTrip, &genUnderVoltagepreAlarmTrip, &genVoltageLoad, &genVoltageNominal, &genOverVoltagePreAlarmReturn, &genOverVoltagePreAlarmTrip, &genOverVoltageShutdownTrip, &genUnderFrequencyAlrmTrip, &genUnderFrequencyPreAlrmTrip, &genFrequencyLoad, &genFrequencyNominal, &genOverFrequencyPreAlrmReturn, &genOverFrequencyPreAlrmTrip, &genOverFrequencyShutdownTrip, &genCurrentPrimary, &genCurrentFullLoadRating, &genOverCurrentAlarmDelay, &genOverCurrentAlarmTrip, &genCurrentRating, &genCurrentOverloadProtectionTrip, &genCurrentOverloadProtectionDelay, &mainsSetup, &mainsAlarms, &mainsUnderVoltageAlarmTrip, &mainsUnderVoltageAlarmReturn, &mainsOverVoltageAlarmReturn, &mainsOverVoltageAlarmTrip, &mainsUnderFrequencyAlarmTrip, &mainsUnderFrequencyAlarmReturn, &mainsOverFrequencyAlarmReturn, &mainsOverFrequencyAlarmTrip, &engineSetup, &enginePreHeatOn, &enginePreHeatDuration, &enginePostHeatOn, &enginePostHeatDuration, &crankSetup, &crankDisconnectgenFreq, &crankDisconnectEngineSpeed, &crankDisconnectOilPressure, &crankDisconnectOilPressureDelay, &crankDisconnectChargeAlternator, &batteryAlarms, &batteryUnderVoltageWarning, &batteryUnderVoltageReturn, &batteryUnderVoltageDelay, &batteryOverVoltageReturn, &batteryOverVoltageWarning, &batteryOverVoltageDelay, &batteryChargeShutdownTrip, &batteryChargeShutdownDelay, &batteryChargeWarningTrip, &batteryChargeWarningDelay, &maintenanceAlarms, &maintenanceAlarmOilEngineRunTime, &maintenanceAlarmAirEngineRunTime, &maintenanceAlarmFuelEngineRunTime};
