/*
 * Configuration file from 'config.csv'
 * Make time: 2020-03-25 12:07:22
 */
#include   "config.h"

eConfigReg versionController =
{
   .page       = 0U,
   .adr        = 0U,
   .scale      = 0U,
   .value      = 1U,
   .min        = 0U,
   .max        = 0U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg versionFirmware =
{
   .page       = 0U,
   .adr        = 1U,
   .scale      = 0U,
   .value      = 1U,
   .min        = 0U,
   .max        = 0U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg serialNumber =
{
   .page       = 0U,
   .adr        = 2U,
   .scale      = 0U,
   .value      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 6U,
};
eConfigReg displayBrightnesLevel =
{
   .page       = 0U,
   .adr        = 8U,
   .scale      = 0U,
   .value      = 50U,
   .min        = 0U,
   .max        = 100U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg displayContarstLevel =
{
   .page       = 0U,
   .adr        = 9U,
   .scale      = 0U,
   .value      = 30U,
   .min        = 0U,
   .max        = 100U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg displaySleepDelay =
{
   .page       = 0U,
   .adr        = 10U,
   .scale      = 0U,
   .value      = 10U,
   .min        = 2U,
   .max        = 600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
static eConfigBitMap moduleSetupBitMap[2U] = 
{
   { 1U, 0U, 0U, 1U },     // moduleType
   { 2U, 1U, 0U, 1U },     // alarmAllBlock
};
eConfigReg moduleSetup =
{
   .page       = 0U,
   .adr        = 11U,
   .scale      = 0U,
   .value      = 3U,
   .min        = 0U,
   .max        = 3U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 2U,
   .bitMap     = moduleSetupBitMap
};
static eConfigBitMap oilPressureSetupBitMap[4U] = 
{
   { 15U, 0U, 0U, 11U },     // oilPressureSensorType
   { 16U, 4U, 0U, 1U },     // oilPressureOpenCircuitAlarmEnb
   { 32U, 5U, 0U, 1U },     // oilPressureAlarmEnb
   { 64U, 6U, 0U, 1U },     // oilPressurePreAlarmEnb
};
eConfigReg oilPressureSetup =
{
   .page       = 0U,
   .adr        = 12U,
   .scale      = 0U,
   .value      = 50U,
   .min        = 0U,
   .max        = 63U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = oilPressureSetupBitMap
};
eConfigReg oilPressureAlarmLevel =
{
   .page       = 0U,
   .adr        = 13U,
   .scale      = -1,
   .value      = 13U,
   .min        = 0U,
   .max        = 103U,
   .units      = {'Б', 'а', 'р', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg oilPressurePreAlarmLevel =
{
   .page       = 0U,
   .adr        = 14U,
   .scale      = -1,
   .value      = 5U,
   .min        = 0U,
   .max        = 103U,
   .units      = {'Б', 'а', 'р', ' '},
   .type       = 'U',
   .len        = 1U,
};
static eConfigBitMap coolantTempSetupBitMap[6U] = 
{
   { 15U, 0U, 0U, 11U },     // coolantTempSensorType
   { 16U, 4U, 0U, 1U },     // coolantTempOpenCircuitAlarmEnb
   { 32U, 5U, 0U, 1U },     // coolantHightTempAlarmEnb
   { 64U, 6U, 0U, 1U },     // coolantHightTempPreAlarmEnb
   { 128U, 7U, 0U, 1U },     // coolantTempHeaterEnb
   { 256U, 8U, 0U, 1U },     // coolantTempCoolerEnb
};
eConfigReg coolantTempSetup =
{
   .page       = 0U,
   .adr        = 15U,
   .scale      = 0U,
   .value      = 51U,
   .min        = 0U,
   .max        = 63U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 6U,
   .bitMap     = coolantTempSetupBitMap
};
eConfigReg coolantHightTempAlarmLevel =
{
   .page       = 0U,
   .adr        = 16U,
   .scale      = 0U,
   .value      = 96U,
   .min        = 0U,
   .max        = 250U,
   .units      = {'C', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg coolantHightTempPreAlarmLevel =
{
   .page       = 0U,
   .adr        = 17U,
   .scale      = 0U,
   .value      = 90U,
   .min        = 0U,
   .max        = 250U,
   .units      = {'C', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg coolantTempHeaterOffLevel =
{
   .page       = 0U,
   .adr        = 18U,
   .scale      = 0U,
   .value      = 60U,
   .min        = 0U,
   .max        = 250U,
   .units      = {'C', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg coolantTempHeaterOnLevel =
{
   .page       = 0U,
   .adr        = 19U,
   .scale      = 0U,
   .value      = 10U,
   .min        = 0U,
   .max        = 250U,
   .units      = {'C', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg coolantTempCoolerOffLevel =
{
   .page       = 0U,
   .adr        = 20U,
   .scale      = 0U,
   .value      = 120U,
   .min        = 0U,
   .max        = 250U,
   .units      = {'C', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg coolantTempCoolerOnLevel =
{
   .page       = 0U,
   .adr        = 21U,
   .scale      = 0U,
   .value      = 80U,
   .min        = 0U,
   .max        = 250U,
   .units      = {'C', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
static eConfigBitMap fuelLevelSetupBitMap[8U] = 
{
   { 7U, 0U, 0U, 5U },     // fuelLevelSensorType
   { 8U, 3U, 0U, 1U },     // fuelLevelLowAlarmEnb
   { 16U, 4U, 0U, 1U },     // fuelLevelLowAlarmAction
   { 32U, 5U, 0U, 1U },     // fuelLevelLowPreAlarmEnb
   { 64U, 6U, 0U, 1U },     // fuelLevelHightPreAlarmEnb
   { 128U, 7U, 0U, 1U },     // fuelLevelHightAlarmEnb
   { 256U, 8U, 0U, 1U },     // fuelLevelHightAlarmAction
   { 512U, 9U, 0U, 1U },     // fuelPumpEnb
};
eConfigReg fuelLevelSetup =
{
   .page       = 0U,
   .adr        = 22U,
   .scale      = 0U,
   .value      = 665U,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 8U,
   .bitMap     = fuelLevelSetupBitMap
};
eConfigReg fuelLevelLowAlarmLevel =
{
   .page       = 0U,
   .adr        = 23U,
   .scale      = 0U,
   .value      = 10U,
   .min        = 0U,
   .max        = 95U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg fuelLevelLowAlarmDelay =
{
   .page       = 0U,
   .adr        = 24U,
   .scale      = 0U,
   .value      = 100U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg fuelLevelLowPreAlarmLevel =
{
   .page       = 0U,
   .adr        = 25U,
   .scale      = 0U,
   .value      = 25U,
   .min        = 1U,
   .max        = 96U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg fuelLevelLowPreAlarmDelay =
{
   .page       = 0U,
   .adr        = 26U,
   .scale      = 0U,
   .value      = 100U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg fuelLevelHightPreAlarmLevel =
{
   .page       = 0U,
   .adr        = 27U,
   .scale      = 0U,
   .value      = 65U,
   .min        = 3U,
   .max        = 98U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg fuelLevelHightPreAlarmDelay =
{
   .page       = 0U,
   .adr        = 28U,
   .scale      = 0U,
   .value      = 0U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg fuelLevelHightAlarmLevel =
{
   .page       = 0U,
   .adr        = 29U,
   .scale      = 0U,
   .value      = 90U,
   .min        = 5U,
   .max        = 100U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg fuelLevelHightAlarmDelay =
{
   .page       = 0U,
   .adr        = 30U,
   .scale      = 0U,
   .value      = 0U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg fuelPumpOnLevel =
{
   .page       = 0U,
   .adr        = 31U,
   .scale      = 0U,
   .value      = 30U,
   .min        = 0U,
   .max        = 100U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg fuelPumpOffLevel =
{
   .page       = 0U,
   .adr        = 32U,
   .scale      = 0U,
   .value      = 70U,
   .min        = 0U,
   .max        = 100U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
static eConfigBitMap diaSetupBitMap[4U] = 
{
   { 31U, 0U, 0U, 27U },     // diaFunction
   { 32U, 5U, 0U, 1U },     // diaPolarity
   { 192U, 6U, 0U, 3U },     // diaAction
   { 768U, 8U, 0U, 4U },     // diaArming
};
eConfigReg diaSetup =
{
   .page       = 0U,
   .adr        = 33U,
   .scale      = 0U,
   .value      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = diaSetupBitMap
};
eConfigReg diaDelay =
{
   .page       = 0U,
   .adr        = 34U,
   .scale      = 0U,
   .value      = 15U,
   .min        = 0U,
   .max        = 60U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
static eConfigBitMap dibSetupBitMap[4U] = 
{
   { 31U, 0U, 0U, 27U },     // dibFunction
   { 32U, 5U, 0U, 1U },     // dibPolarity
   { 192U, 6U, 0U, 3U },     // dibAction
   { 768U, 8U, 0U, 4U },     // dibArming
};
eConfigReg dibSetup =
{
   .page       = 0U,
   .adr        = 35U,
   .scale      = 0U,
   .value      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = dibSetupBitMap
};
eConfigReg dibDelay =
{
   .page       = 0U,
   .adr        = 36U,
   .scale      = 0U,
   .value      = 15U,
   .min        = 0U,
   .max        = 60U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
static eConfigBitMap dicSetupBitMap[4U] = 
{
   { 31U, 0U, 0U, 27U },     // dicFunction
   { 32U, 5U, 0U, 1U },     // dicPolarity
   { 192U, 6U, 0U, 3U },     // dicAction
   { 768U, 8U, 0U, 4U },     // dicArming
};
eConfigReg dicSetup =
{
   .page       = 0U,
   .adr        = 37U,
   .scale      = 0U,
   .value      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = dicSetupBitMap
};
eConfigReg dicDelay =
{
   .page       = 0U,
   .adr        = 38U,
   .scale      = 0U,
   .value      = 15U,
   .min        = 0U,
   .max        = 60U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
static eConfigBitMap didSetupBitMap[4U] = 
{
   { 31U, 0U, 0U, 27U },     // didFunction
   { 32U, 5U, 0U, 1U },     // didPolarity
   { 192U, 6U, 0U, 3U },     // didAction
   { 768U, 8U, 0U, 4U },     // didArming
};
eConfigReg didSetup =
{
   .page       = 0U,
   .adr        = 39U,
   .scale      = 0U,
   .value      = 64U,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = didSetupBitMap
};
eConfigReg didDelay =
{
   .page       = 0U,
   .adr        = 40U,
   .scale      = 0U,
   .value      = 15U,
   .min        = 0U,
   .max        = 60U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
static eConfigBitMap doSetupBitMap[6U] = 
{
   { 1U, 0U, 0U, 1U },     // doaNOC
   { 2U, 1U, 0U, 1U },     // dobNOC
   { 4U, 2U, 0U, 1U },     // docNOC
   { 8U, 3U, 0U, 1U },     // dodNOC
   { 16U, 4U, 0U, 1U },     // doeNOC
   { 32U, 5U, 0U, 1U },     // dofNOC
};
eConfigReg doSetup =
{
   .page       = 0U,
   .adr        = 41U,
   .scale      = 0U,
   .value      = 21U,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 6U,
   .bitMap     = doSetupBitMap
};
static eConfigBitMap doabTypeBitMap[2U] = 
{
   { 255U, 0U, 0U, 255U },     // doaType
   { 65280U, 8U, 0U, 255U },     // dobType
};
eConfigReg doabType =
{
   .page       = 0U,
   .adr        = 42U,
   .scale      = 0U,
   .value      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 2U,
   .bitMap     = doabTypeBitMap
};
static eConfigBitMap dodTypeBitMap[2U] = 
{
   { 255U, 0U, 0U, 255U },     // docType
   { 65280U, 8U, 0U, 255U },     // doType
};
eConfigReg dodType =
{
   .page       = 0U,
   .adr        = 43U,
   .scale      = 0U,
   .value      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 2U,
   .bitMap     = dodTypeBitMap
};
static eConfigBitMap doefTypeBitMap[2U] = 
{
   { 255U, 0U, 0U, 255U },     // doeType
   { 65280U, 8U, 0U, 255U },     // dofType
};
eConfigReg doefType =
{
   .page       = 0U,
   .adr        = 44U,
   .scale      = 0U,
   .value      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 2U,
   .bitMap     = doefTypeBitMap
};
eConfigReg timerMainsTransientDelay =
{
   .page       = 0U,
   .adr        = 45U,
   .scale      = -1,
   .value      = 1U,
   .min        = 1U,
   .max        = 100U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerStartDelay =
{
   .page       = 0U,
   .adr        = 46U,
   .scale      = 0U,
   .value      = 5U,
   .min        = 1U,
   .max        = 300U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerPreheating =
{
   .page       = 0U,
   .adr        = 47U,
   .scale      = 0U,
   .value      = 5U,
   .min        = 1U,
   .max        = 300U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerCranking =
{
   .page       = 0U,
   .adr        = 48U,
   .scale      = 0U,
   .value      = 10U,
   .min        = 1U,
   .max        = 30U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerCrankDelay =
{
   .page       = 0U,
   .adr        = 49U,
   .scale      = 0U,
   .value      = 10U,
   .min        = 1U,
   .max        = 30U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerStartupIdleTime =
{
   .page       = 0U,
   .adr        = 50U,
   .scale      = 0U,
   .value      = 10U,
   .min        = 1U,
   .max        = 300U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerNominalRPMDelay =
{
   .page       = 0U,
   .adr        = 51U,
   .scale      = 0U,
   .value      = 10U,
   .min        = 1U,
   .max        = 30U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerSafetyOnDelay =
{
   .page       = 0U,
   .adr        = 52U,
   .scale      = 0U,
   .value      = 10U,
   .min        = 1U,
   .max        = 60U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerWarming =
{
   .page       = 0U,
   .adr        = 53U,
   .scale      = 0U,
   .value      = 1U,
   .min        = 1U,
   .max        = 300U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerTransferDelay =
{
   .page       = 0U,
   .adr        = 54U,
   .scale      = -1,
   .value      = 6U,
   .min        = 1U,
   .max        = 300U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerBreakerTripPulse =
{
   .page       = 0U,
   .adr        = 55U,
   .scale      = -1,
   .value      = 5U,
   .min        = 1U,
   .max        = 50U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerBreakerClosePulse =
{
   .page       = 0U,
   .adr        = 56U,
   .scale      = -1,
   .value      = 5U,
   .min        = 1U,
   .max        = 50U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerReturnDelay =
{
   .page       = 0U,
   .adr        = 57U,
   .scale      = 0U,
   .value      = 30U,
   .min        = 1U,
   .max        = 300U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerCooling =
{
   .page       = 0U,
   .adr        = 58U,
   .scale      = 0U,
   .value      = 60U,
   .min        = 1U,
   .max        = 300U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerCoolingIdle =
{
   .page       = 0U,
   .adr        = 59U,
   .scale      = 0U,
   .value      = 20U,
   .min        = 1U,
   .max        = 300U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerSolenoidHold =
{
   .page       = 0U,
   .adr        = 60U,
   .scale      = 0U,
   .value      = 20U,
   .min        = 1U,
   .max        = 60U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerFailStopDelay =
{
   .page       = 0U,
   .adr        = 61U,
   .scale      = 0U,
   .value      = 30U,
   .min        = 1U,
   .max        = 60U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg timerGenTransientDelay =
{
   .page       = 0U,
   .adr        = 62U,
   .scale      = -1,
   .value      = 10U,
   .min        = 1U,
   .max        = 100U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
static eConfigBitMap genSetupBitMap[4U] = 
{
   { 1U, 0U, 0U, 1U },     // genPowerGeneratorControlEnb
   { 30U, 1U, 0U, 16U },     // genPoles
   { 224U, 5U, 0U, 5U },     // genAcSys
   { 256U, 8U, 0U, 1U },     // genLocationCurrentTransformer
};
eConfigReg genSetup =
{
   .page       = 0U,
   .adr        = 63U,
   .scale      = 0U,
   .value      = 104U,
   .min        = 0U,
   .max        = 255U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = genSetupBitMap
};
eConfigReg genRatedActivePowerLevel =
{
   .page       = 0U,
   .adr        = 64U,
   .scale      = 0U,
   .value      = 40000U,
   .min        = 0U,
   .max        = 10000U,
   .units      = {'к', 'В', 'т', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genRatedReactivePowerLevel =
{
   .page       = 0U,
   .adr        = 65U,
   .scale      = 0U,
   .value      = 0U,
   .min        = 0U,
   .max        = 10000U,
   .units      = {'к', 'В', 'А', 'Р'},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genRatedApparentPowerLevel =
{
   .page       = 0U,
   .adr        = 66U,
   .scale      = 0U,
   .value      = 200U,
   .min        = 0U,
   .max        = 10000U,
   .units      = {'к', 'В', 'А', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genRatedFrequencyLevel =
{
   .page       = 0U,
   .adr        = 67U,
   .scale      = -1,
   .value      = 500U,
   .min        = 2U,
   .max        = 747U,
   .units      = {'Г', 'ц', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genCurrentPrimaryLevel =
{
   .page       = 0U,
   .adr        = 68U,
   .scale      = 0U,
   .value      = 600U,
   .min        = 5U,
   .max        = 8000U,
   .units      = {'А', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genCurrentFullLoadRatingLevel =
{
   .page       = 0U,
   .adr        = 69U,
   .scale      = 0U,
   .value      = 500U,
   .min        = 5U,
   .max        = 8000U,
   .units      = {'А', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
static eConfigBitMap genAlarmsBitMap[12U] = 
{
   { 1U, 0U, 0U, 1U },     // genUnderVoltageAlarmEnb
   { 2U, 1U, 0U, 1U },     // genUnderVoltagePreAlarmEnb
   { 4U, 2U, 0U, 1U },     // genOverVoltagePreAlarmEnb
   { 8U, 3U, 0U, 1U },     // genUnderFrequencyAlarmEnb
   { 16U, 4U, 0U, 1U },     // genUnderFrequencyPreAlarmEnb
   { 32U, 5U, 0U, 1U },     // genOverFrequencyPreAlarmEnb
   { 64U, 6U, 0U, 1U },     // genOverFrequencyAlarmEnb
   { 128U, 7U, 0U, 1U },     // genCurrentOverloadProtectionEnb
   { 256U, 8U, 0U, 1U },     // genCurrentOverPhaseImbalanceEnb
   { 1536U, 9U, 0U, 3U },     // genCurrentOverAlarmAction
   { 2048U, 11U, 0U, 1U },     // genCurrentOverloadProtectionAction
   { 4096U, 12U, 0U, 1U },     // genCurrentOverPhaseImbalanceAction
};
eConfigReg genAlarms =
{
   .page       = 0U,
   .adr        = 70U,
   .scale      = 0U,
   .value      = 7167U,
   .min        = 0U,
   .max        = 65535U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 12U,
   .bitMap     = genAlarmsBitMap
};
eConfigReg genUnderVoltageAlarmLevel =
{
   .page       = 0U,
   .adr        = 71U,
   .scale      = 0U,
   .value      = 318U,
   .min        = 86U,
   .max        = 708U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genUnderVoltagePreAlarmLevel =
{
   .page       = 0U,
   .adr        = 72U,
   .scale      = 0U,
   .value      = 339U,
   .min        = 88U,
   .max        = 710U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genOverVoltagePreAlarmLevel =
{
   .page       = 0U,
   .adr        = 73U,
   .scale      = 0U,
   .value      = 439U,
   .min        = 93U,
   .max        = 715U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genOverVoltageAlarmLevel =
{
   .page       = 0U,
   .adr        = 74U,
   .scale      = 0U,
   .value      = 458U,
   .min        = 95U,
   .max        = 717U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genUnderFrequencyAlarmLevel =
{
   .page       = 0U,
   .adr        = 75U,
   .scale      = -1,
   .value      = 400U,
   .min        = 0U,
   .max        = 744U,
   .units      = {'Г', 'ц', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genUnderFrequencyPreAlarmLevel =
{
   .page       = 0U,
   .adr        = 76U,
   .scale      = -1,
   .value      = 420U,
   .min        = 1U,
   .max        = 745U,
   .units      = {'Г', 'ц', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genOverFrequencyPreAlarmLevel =
{
   .page       = 0U,
   .adr        = 77U,
   .scale      = -1,
   .value      = 540U,
   .min        = 4U,
   .max        = 747U,
   .units      = {'Г', 'ц', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genOverFrequencyAlarmLevel =
{
   .page       = 0U,
   .adr        = 78U,
   .scale      = -1,
   .value      = 550U,
   .min        = 5U,
   .max        = 749U,
   .units      = {'Г', 'ц', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genOverCurrentThermalProtectionLevel =
{
   .page       = 0U,
   .adr        = 79U,
   .scale      = 0U,
   .value      = 20U,
   .min        = 0U,
   .max        = 120U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genOverCurrentCutoffLevel =
{
   .page       = 0U,
   .adr        = 80U,
   .scale      = 0U,
   .value      = 40U,
   .min        = 0U,
   .max        = 120U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genOverCurrentAlarmLevel =
{
   .page       = 0U,
   .adr        = 81U,
   .scale      = 0U,
   .value      = 100U,
   .min        = 50U,
   .max        = 120U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genOverCurrentAlarmDelay =
{
   .page       = 0U,
   .adr        = 82U,
   .scale      = 0U,
   .value      = 60U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genCurrentOverloadProtectionLevel =
{
   .page       = 0U,
   .adr        = 83U,
   .scale      = 0U,
   .value      = 100U,
   .min        = 1U,
   .max        = 125U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genCurrentOverloadProtectionDelay =
{
   .page       = 0U,
   .adr        = 84U,
   .scale      = 0U,
   .value      = 5U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genCurrentOverPhaseImbalanceLevel =
{
   .page       = 0U,
   .adr        = 85U,
   .scale      = 0U,
   .value      = 100U,
   .min        = 1U,
   .max        = 125U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg genCurrentOverPhaseImbalanceDelay =
{
   .page       = 0U,
   .adr        = 86U,
   .scale      = 0U,
   .value      = 60U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
static eConfigBitMap mainsSetupBitMap[3U] = 
{
   { 1U, 0U, 0U, 1U },     // mainsControlEnb
   { 2U, 1U, 0U, 1U },     // mainsPowerOffImmediatelyEnb
   { 28U, 2U, 0U, 5U },     // mainAcSys
};
eConfigReg mainsSetup =
{
   .page       = 0U,
   .adr        = 87U,
   .scale      = 0U,
   .value      = 13U,
   .min        = 0U,
   .max        = 31U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 3U,
   .bitMap     = mainsSetupBitMap
};
static eConfigBitMap mainsAlarmsBitMap[4U] = 
{
   { 1U, 0U, 0U, 1U },     // mainsUnderVoltageAlarmEnb
   { 2U, 1U, 0U, 1U },     // mainsOverVoltageAlarmEnb
   { 4U, 2U, 0U, 1U },     // mainsUnderFrequencyAlarmEnb
   { 8U, 3U, 0U, 1U },     // mainsOverFrequencyAlarmEnb
};
eConfigReg mainsAlarms =
{
   .page       = 0U,
   .adr        = 88U,
   .scale      = 0U,
   .value      = 15U,
   .min        = 0U,
   .max        = 15U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = mainsAlarmsBitMap
};
eConfigReg mainsUnderVoltageAlarmLevel =
{
   .page       = 0U,
   .adr        = 89U,
   .scale      = 0U,
   .value      = 318U,
   .min        = 86U,
   .max        = 713U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg mainsOverVoltageAlarmLevel =
{
   .page       = 0U,
   .adr        = 90U,
   .scale      = 0U,
   .value      = 438U,
   .min        = 90U,
   .max        = 717U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg mainsUnderFrequencyAlarmLevel =
{
   .page       = 0U,
   .adr        = 91U,
   .scale      = -1,
   .value      = 450U,
   .min        = 0U,
   .max        = 747U,
   .units      = {'Г', 'ц', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg mainsOverFrequencyAlarmLevel =
{
   .page       = 0U,
   .adr        = 92U,
   .scale      = -1,
   .value      = 520U,
   .min        = 2U,
   .max        = 749U,
   .units      = {'Г', 'ц', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
static eConfigBitMap engineSetupBitMap[3U] = 
{
   { 15U, 0U, 1U, 9U },     // engineStartAttempts
   { 16U, 4U, 0U, 1U },     // enginePreHeatEnb
   { 32U, 5U, 0U, 1U },     // enginePostHeatEnb
};
eConfigReg engineSetup =
{
   .page       = 0U,
   .adr        = 93U,
   .scale      = 0U,
   .value      = 51U,
   .min        = 0U,
   .max        = 63U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 3U,
   .bitMap     = engineSetupBitMap
};
eConfigReg enginePreHeatLevel =
{
   .page       = 0U,
   .adr        = 94U,
   .scale      = 0U,
   .value      = 50U,
   .min        = 0U,
   .max        = 100U,
   .units      = {'C', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg enginePreHeatDelay =
{
   .page       = 0U,
   .adr        = 95U,
   .scale      = 0U,
   .value      = 0U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg enginePostHeatLevel =
{
   .page       = 0U,
   .adr        = 96U,
   .scale      = 0U,
   .value      = 50U,
   .min        = 0U,
   .max        = 100U,
   .units      = {'C', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg enginePostHeatDelay =
{
   .page       = 0U,
   .adr        = 97U,
   .scale      = 0U,
   .value      = 0U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
static eConfigBitMap crankSetupBitMap[4U] = 
{
   { 1U, 0U, 0U, 1U },     // crankDisconnectOilPressureEnb
   { 2U, 1U, 0U, 1U },     // crankOilPressureCheckOnStartEnb
   { 4U, 2U, 0U, 1U },     // crankDisconnectOilPressureEnb
   { 8U, 3U, 0U, 1U },     // crankDisconnectChargeAlternatorEnb
};
eConfigReg crankSetup =
{
   .page       = 0U,
   .adr        = 98U,
   .scale      = 0U,
   .value      = 10U,
   .min        = 0U,
   .max        = 7U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = crankSetupBitMap
};
eConfigReg crankDisconnectgenFreqLevel =
{
   .page       = 0U,
   .adr        = 99U,
   .scale      = -1,
   .value      = 210U,
   .min        = 0U,
   .max        = 400U,
   .units      = {'Г', 'ц', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg crankDisconnectOilPressureLevel =
{
   .page       = 0U,
   .adr        = 100U,
   .scale      = -1,
   .value      = 20U,
   .min        = 5U,
   .max        = 40U,
   .units      = {'Б', 'а', 'р', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg crankDisconnectChargeAlternatorLevel =
{
   .page       = 0U,
   .adr        = 101U,
   .scale      = -1,
   .value      = 60U,
   .min        = 0U,
   .max        = 400U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
static eConfigBitMap batteryAlarmsBitMap[4U] = 
{
   { 1U, 0U, 0U, 1U },     // batteryUnderVoltageEnb
   { 2U, 1U, 0U, 1U },     // batteryOverVoltageEnb
   { 4U, 2U, 0U, 1U },     // batteryChargeShutdownEnb
   { 8U, 3U, 0U, 1U },     // batteryChargeWarningEnb
};
eConfigReg batteryAlarms =
{
   .page       = 0U,
   .adr        = 102U,
   .scale      = 0U,
   .value      = 15U,
   .min        = 0U,
   .max        = 15U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = batteryAlarmsBitMap
};
eConfigReg batteryUnderVoltageLevel =
{
   .page       = 0U,
   .adr        = 103U,
   .scale      = -1,
   .value      = 100U,
   .min        = 0U,
   .max        = 397U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg batteryUnderVoltageDelay =
{
   .page       = 0U,
   .adr        = 104U,
   .scale      = 0U,
   .value      = 60U,
   .min        = 0U,
   .max        = 65535U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg batteryOverVoltageLevel =
{
   .page       = 0U,
   .adr        = 105U,
   .scale      = -1,
   .value      = 300U,
   .min        = 2U,
   .max        = 400U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg batteryOverVoltageDelay =
{
   .page       = 0U,
   .adr        = 106U,
   .scale      = 0U,
   .value      = 60U,
   .min        = 0U,
   .max        = 65535U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg batteryChargeShutdownLevel =
{
   .page       = 0U,
   .adr        = 107U,
   .scale      = -1,
   .value      = 40U,
   .min        = 0U,
   .max        = 388U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg batteryChargeShutdownDelay =
{
   .page       = 0U,
   .adr        = 108U,
   .scale      = 0U,
   .value      = 5U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg batteryChargeWarningLevel =
{
   .page       = 0U,
   .adr        = 109U,
   .scale      = -1,
   .value      = 60U,
   .min        = 1U,
   .max        = 390U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg batteryChargeWarningDelay =
{
   .page       = 0U,
   .adr        = 110U,
   .scale      = 0U,
   .value      = 5U,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .len        = 1U,
};
static eConfigBitMap maintenanceAlarmsBitMap[6U] = 
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
   .adr        = 111U,
   .scale      = 0U,
   .value      = 42U,
   .min        = 0U,
   .max        = 63U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 6U,
   .bitMap     = maintenanceAlarmsBitMap
};
eConfigReg maintenanceAlarmOilTime =
{
   .page       = 0U,
   .adr        = 112U,
   .scale      = 0U,
   .value      = 10U,
   .min        = 10U,
   .max        = 5000U,
   .units      = {'ч', 'а', 'с', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg maintenanceAlarmAirTime =
{
   .page       = 0U,
   .adr        = 113U,
   .scale      = 0U,
   .value      = 10U,
   .min        = 10U,
   .max        = 5000U,
   .units      = {'ч', 'а', 'с', ' '},
   .type       = 'U',
   .len        = 1U,
};
eConfigReg maintenanceAlarmFuelTime =
{
   .page       = 0U,
   .adr        = 114U,
   .scale      = 0U,
   .value      = 10U,
   .min        = 10U,
   .max        = 5000U,
   .units      = {'ч', 'а', 'с', ' '},
   .type       = 'U',
   .len        = 1U,
};

eConfigReg* configReg[SETTING_REGISTER_NUMBER] = { &versionController, &versionFirmware, &serialNumber, &displayBrightnesLevel, &displayContarstLevel, &displaySleepDelay, &moduleSetup, &oilPressureSetup, &oilPressureAlarmLevel, &oilPressurePreAlarmLevel, &coolantTempSetup, &coolantHightTempAlarmLevel, &coolantHightTempPreAlarmLevel, &coolantTempHeaterOffLevel, &coolantTempHeaterOnLevel, &coolantTempCoolerOffLevel, &coolantTempCoolerOnLevel, &fuelLevelSetup, &fuelLevelLowAlarmLevel, &fuelLevelLowAlarmDelay, &fuelLevelLowPreAlarmLevel, &fuelLevelLowPreAlarmDelay, &fuelLevelHightPreAlarmLevel, &fuelLevelHightPreAlarmDelay, &fuelLevelHightAlarmLevel, &fuelLevelHightAlarmDelay, &fuelPumpOnLevel, &fuelPumpOffLevel, &diaSetup, &diaDelay, &dibSetup, &dibDelay, &dicSetup, &dicDelay, &didSetup, &didDelay, &doSetup, &doabType, &dodType, &doefType, &timerMainsTransientDelay, &timerStartDelay, &timerPreheating, &timerCranking, &timerCrankDelay, &timerStartupIdleTime, &timerNominalRPMDelay, &timerSafetyOnDelay, &timerWarming, &timerTransferDelay, &timerBreakerTripPulse, &timerBreakerClosePulse, &timerReturnDelay, &timerCooling, &timerCoolingIdle, &timerSolenoidHold, &timerFailStopDelay, &timerGenTransientDelay, &genSetup, &genRatedActivePowerLevel, &genRatedReactivePowerLevel, &genRatedApparentPowerLevel, &genRatedFrequencyLevel, &genCurrentPrimaryLevel, &genCurrentFullLoadRatingLevel, &genAlarms, &genUnderVoltageAlarmLevel, &genUnderVoltagePreAlarmLevel, &genOverVoltagePreAlarmLevel, &genOverVoltageAlarmLevel, &genUnderFrequencyAlarmLevel, &genUnderFrequencyPreAlarmLevel, &genOverFrequencyPreAlarmLevel, &genOverFrequencyAlarmLevel, &genOverCurrentThermalProtectionLevel, &genOverCurrentCutoffLevel, &genOverCurrentAlarmLevel, &genOverCurrentAlarmDelay, &genCurrentOverloadProtectionLevel, &genCurrentOverloadProtectionDelay, &genCurrentOverPhaseImbalanceLevel, &genCurrentOverPhaseImbalanceDelay, &mainsSetup, &mainsAlarms, &mainsUnderVoltageAlarmLevel, &mainsOverVoltageAlarmLevel, &mainsUnderFrequencyAlarmLevel, &mainsOverFrequencyAlarmLevel, &engineSetup, &enginePreHeatLevel, &enginePreHeatDelay, &enginePostHeatLevel, &enginePostHeatDelay, &crankSetup, &crankDisconnectgenFreqLevel, &crankDisconnectOilPressureLevel, &crankDisconnectChargeAlternatorLevel, &batteryAlarms, &batteryUnderVoltageLevel, &batteryUnderVoltageDelay, &batteryOverVoltageLevel, &batteryOverVoltageDelay, &batteryChargeShutdownLevel, &batteryChargeShutdownDelay, &batteryChargeWarningLevel, &batteryChargeWarningDelay, &maintenanceAlarms, &maintenanceAlarmOilTime, &maintenanceAlarmAirTime, &maintenanceAlarmFuelTime};
