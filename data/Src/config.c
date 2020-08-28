/*
 * Configuration file from 'config.csv'
 * Make time: 2020-08-28 13:09:26
 */
#include   "config.h"

uint16_t versionControllerValue[1U] = { 1U };
const eConfigAttributes versionControllerAtrib =
{
   .adr        = 0U,
   .min        = 0U,
   .max        = 0U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg versionController =
{
   .atrib      = &versionControllerAtrib,
   .scale      = 0U,
   .value      = versionControllerValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t versionFirmwareValue[1U] = { 1U };
const eConfigAttributes versionFirmwareAtrib =
{
   .adr        = 1U,
   .min        = 0U,
   .max        = 0U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg versionFirmware =
{
   .atrib      = &versionFirmwareAtrib,
   .scale      = 0U,
   .value      = versionFirmwareValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t serialNumberValue[6U] = { 0U, 0U, 0U, 0U, 0U, 0U };
const eConfigAttributes serialNumberAtrib =
{
   .adr        = 2U,
   .min        = 0U,
   .max        = 0U,
   .type       = 'U',
   .len        = 6U,
   .bitMapSize = 0U,
};
eConfigReg serialNumber =
{
   .atrib      = &serialNumberAtrib,
   .scale      = 0U,
   .value      = serialNumberValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t displayBrightnesLevelValue[1U] = { 50U };
const eConfigAttributes displayBrightnesLevelAtrib =
{
   .adr        = 3U,
   .min        = 0U,
   .max        = 100U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg displayBrightnesLevel =
{
   .atrib      = &displayBrightnesLevelAtrib,
   .scale      = 0U,
   .value      = displayBrightnesLevelValue,
   .units      = {'%', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t displayContarstLevelValue[1U] = { 30U };
const eConfigAttributes displayContarstLevelAtrib =
{
   .adr        = 4U,
   .min        = 0U,
   .max        = 100U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg displayContarstLevel =
{
   .atrib      = &displayContarstLevelAtrib,
   .scale      = 0U,
   .value      = displayContarstLevelValue,
   .units      = {'%', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t displaySleepDelayValue[1U] = { 10U };
const eConfigAttributes displaySleepDelayAtrib =
{
   .adr        = 5U,
   .min        = 2U,
   .max        = 600U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg displaySleepDelay =
{
   .atrib      = &displaySleepDelayAtrib,
   .scale      = 0U,
   .value      = displaySleepDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t hysteresisLevelValue[1U] = { 20U };
const eConfigAttributes hysteresisLevelAtrib =
{
   .adr        = 6U,
   .min        = 0U,
   .max        = 1000U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg hysteresisLevel =
{
   .atrib      = &hysteresisLevelAtrib,
   .scale      = -1,
   .value      = hysteresisLevelValue,
   .units      = {'%', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
static eConfigBitMap moduleSetupBitMap[2U] = 
{
   { 1U, 0U },     // moduleType
   { 2U, 1U },     // alarmAllBlock
};
uint16_t moduleSetupValue[1U] = { 3U };
const eConfigAttributes moduleSetupAtrib =
{
   .adr        = 7U,
   .min        = 0U,
   .max        = 3U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 2U,
};
eConfigReg moduleSetup =
{
   .atrib      = &moduleSetupAtrib,
   .scale      = 0U,
   .value      = moduleSetupValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = moduleSetupBitMap
};
/*----------------------------------------------------------------*/
static eConfigBitMap oilPressureSetupBitMap[4U] = 
{
   { 15U, 0U },     // oilPressureSensorType
   { 16U, 4U },     // oilPressureOpenCircuitAlarmEnb
   { 32U, 5U },     // oilPressureAlarmEnb
   { 64U, 6U },     // oilPressurePreAlarmEnb
};
uint16_t oilPressureSetupValue[1U] = { 51U };
const eConfigAttributes oilPressureSetupAtrib =
{
   .adr        = 8U,
   .min        = 0U,
   .max        = 63U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
};
eConfigReg oilPressureSetup =
{
   .atrib      = &oilPressureSetupAtrib,
   .scale      = 0U,
   .value      = oilPressureSetupValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = oilPressureSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t oilPressureAlarmLevelValue[1U] = { 103U };
const eConfigAttributes oilPressureAlarmLevelAtrib =
{
   .adr        = 9U,
   .min        = 0U,
   .max        = 1030U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg oilPressureAlarmLevel =
{
   .atrib      = &oilPressureAlarmLevelAtrib,
   .scale      = -2,
   .value      = oilPressureAlarmLevelValue,
   .units      = {'Б', 'а', 'р', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t oilPressurePreAlarmLevelValue[1U] = { 124U };
const eConfigAttributes oilPressurePreAlarmLevelAtrib =
{
   .adr        = 10U,
   .min        = 0U,
   .max        = 1030U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg oilPressurePreAlarmLevel =
{
   .atrib      = &oilPressurePreAlarmLevelAtrib,
   .scale      = -2,
   .value      = oilPressurePreAlarmLevelValue,
   .units      = {'Б', 'а', 'р', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
static eConfigBitMap coolantTempSetupBitMap[6U] = 
{
   { 15U, 0U },     // coolantTempSensorType
   { 16U, 4U },     // coolantTempOpenCircuitAlarmEnb
   { 32U, 5U },     // coolantHightTempAlarmEnb
   { 64U, 6U },     // coolantHightTempPreAlarmEnb
   { 128U, 7U },     // coolantTempHeaterEnb
   { 256U, 8U },     // coolantTempCoolerEnb
};
uint16_t coolantTempSetupValue[1U] = { 51U };
const eConfigAttributes coolantTempSetupAtrib =
{
   .adr        = 11U,
   .min        = 0U,
   .max        = 63U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 6U,
};
eConfigReg coolantTempSetup =
{
   .atrib      = &coolantTempSetupAtrib,
   .scale      = 0U,
   .value      = coolantTempSetupValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = coolantTempSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t coolantHightTempAlarmLevelValue[1U] = { 96U };
const eConfigAttributes coolantHightTempAlarmLevelAtrib =
{
   .adr        = 12U,
   .min        = 0U,
   .max        = 250U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg coolantHightTempAlarmLevel =
{
   .atrib      = &coolantHightTempAlarmLevelAtrib,
   .scale      = 0U,
   .value      = coolantHightTempAlarmLevelValue,
   .units      = {'C', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t coolantHightTempPreAlarmLevelValue[1U] = { 90U };
const eConfigAttributes coolantHightTempPreAlarmLevelAtrib =
{
   .adr        = 13U,
   .min        = 0U,
   .max        = 250U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg coolantHightTempPreAlarmLevel =
{
   .atrib      = &coolantHightTempPreAlarmLevelAtrib,
   .scale      = 0U,
   .value      = coolantHightTempPreAlarmLevelValue,
   .units      = {'C', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t coolantTempHeaterOffLevelValue[1U] = { 60U };
const eConfigAttributes coolantTempHeaterOffLevelAtrib =
{
   .adr        = 14U,
   .min        = 0U,
   .max        = 250U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg coolantTempHeaterOffLevel =
{
   .atrib      = &coolantTempHeaterOffLevelAtrib,
   .scale      = 0U,
   .value      = coolantTempHeaterOffLevelValue,
   .units      = {'C', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t coolantTempHeaterOnLevelValue[1U] = { 10U };
const eConfigAttributes coolantTempHeaterOnLevelAtrib =
{
   .adr        = 15U,
   .min        = 0U,
   .max        = 250U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg coolantTempHeaterOnLevel =
{
   .atrib      = &coolantTempHeaterOnLevelAtrib,
   .scale      = 0U,
   .value      = coolantTempHeaterOnLevelValue,
   .units      = {'C', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t coolantTempCoolerOffLevelValue[1U] = { 120U };
const eConfigAttributes coolantTempCoolerOffLevelAtrib =
{
   .adr        = 16U,
   .min        = 0U,
   .max        = 250U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg coolantTempCoolerOffLevel =
{
   .atrib      = &coolantTempCoolerOffLevelAtrib,
   .scale      = 0U,
   .value      = coolantTempCoolerOffLevelValue,
   .units      = {'C', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t coolantTempCoolerOnLevelValue[1U] = { 80U };
const eConfigAttributes coolantTempCoolerOnLevelAtrib =
{
   .adr        = 17U,
   .min        = 0U,
   .max        = 250U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg coolantTempCoolerOnLevel =
{
   .atrib      = &coolantTempCoolerOnLevelAtrib,
   .scale      = 0U,
   .value      = coolantTempCoolerOnLevelValue,
   .units      = {'C', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
static eConfigBitMap fuelLevelSetupBitMap[9U] = 
{
   { 7U, 0U },     // fuelLevelSensorType
   { 8U, 3U },     // fuelLevelOpenCircuitAlarmEnb
   { 16U, 4U },     // fuelLevelLowAlarmEnb
   { 32U, 5U },     // fuelLevelLowAlarmAction
   { 64U, 6U },     // fuelLevelLowPreAlarmEnb
   { 128U, 7U },     // fuelLevelHightPreAlarmEnb
   { 256U, 8U },     // fuelLevelHightAlarmEnb
   { 512U, 9U },     // fuelLevelHightAlarmAction
   { 0U, 10U },     // fuelPumpEnb
};
uint16_t fuelLevelSetupValue[1U] = { 1339U };
const eConfigAttributes fuelLevelSetupAtrib =
{
   .adr        = 18U,
   .min        = 0U,
   .max        = 1023U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 9U,
};
eConfigReg fuelLevelSetup =
{
   .atrib      = &fuelLevelSetupAtrib,
   .scale      = 0U,
   .value      = fuelLevelSetupValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = fuelLevelSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelLowAlarmLevelValue[1U] = { 10U };
const eConfigAttributes fuelLevelLowAlarmLevelAtrib =
{
   .adr        = 19U,
   .min        = 0U,
   .max        = 95U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg fuelLevelLowAlarmLevel =
{
   .atrib      = &fuelLevelLowAlarmLevelAtrib,
   .scale      = 0U,
   .value      = fuelLevelLowAlarmLevelValue,
   .units      = {'%', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelLowAlarmDelayValue[1U] = { 100U };
const eConfigAttributes fuelLevelLowAlarmDelayAtrib =
{
   .adr        = 20U,
   .min        = 0U,
   .max        = 3600U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg fuelLevelLowAlarmDelay =
{
   .atrib      = &fuelLevelLowAlarmDelayAtrib,
   .scale      = 0U,
   .value      = fuelLevelLowAlarmDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelLowPreAlarmLevelValue[1U] = { 25U };
const eConfigAttributes fuelLevelLowPreAlarmLevelAtrib =
{
   .adr        = 21U,
   .min        = 1U,
   .max        = 96U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg fuelLevelLowPreAlarmLevel =
{
   .atrib      = &fuelLevelLowPreAlarmLevelAtrib,
   .scale      = 0U,
   .value      = fuelLevelLowPreAlarmLevelValue,
   .units      = {'%', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelLowPreAlarmDelayValue[1U] = { 100U };
const eConfigAttributes fuelLevelLowPreAlarmDelayAtrib =
{
   .adr        = 22U,
   .min        = 0U,
   .max        = 3600U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg fuelLevelLowPreAlarmDelay =
{
   .atrib      = &fuelLevelLowPreAlarmDelayAtrib,
   .scale      = 0U,
   .value      = fuelLevelLowPreAlarmDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelHightPreAlarmLevelValue[1U] = { 65U };
const eConfigAttributes fuelLevelHightPreAlarmLevelAtrib =
{
   .adr        = 23U,
   .min        = 3U,
   .max        = 98U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg fuelLevelHightPreAlarmLevel =
{
   .atrib      = &fuelLevelHightPreAlarmLevelAtrib,
   .scale      = 0U,
   .value      = fuelLevelHightPreAlarmLevelValue,
   .units      = {'%', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelHightPreAlarmDelayValue[1U] = { 0U };
const eConfigAttributes fuelLevelHightPreAlarmDelayAtrib =
{
   .adr        = 24U,
   .min        = 0U,
   .max        = 3600U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg fuelLevelHightPreAlarmDelay =
{
   .atrib      = &fuelLevelHightPreAlarmDelayAtrib,
   .scale      = 0U,
   .value      = fuelLevelHightPreAlarmDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelHightAlarmLevelValue[1U] = { 90U };
const eConfigAttributes fuelLevelHightAlarmLevelAtrib =
{
   .adr        = 25U,
   .min        = 5U,
   .max        = 100U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg fuelLevelHightAlarmLevel =
{
   .atrib      = &fuelLevelHightAlarmLevelAtrib,
   .scale      = 0U,
   .value      = fuelLevelHightAlarmLevelValue,
   .units      = {'%', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelHightAlarmDelayValue[1U] = { 0U };
const eConfigAttributes fuelLevelHightAlarmDelayAtrib =
{
   .adr        = 26U,
   .min        = 0U,
   .max        = 3600U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg fuelLevelHightAlarmDelay =
{
   .atrib      = &fuelLevelHightAlarmDelayAtrib,
   .scale      = 0U,
   .value      = fuelLevelHightAlarmDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t fuelPumpOnLevelValue[1U] = { 30U };
const eConfigAttributes fuelPumpOnLevelAtrib =
{
   .adr        = 27U,
   .min        = 0U,
   .max        = 100U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg fuelPumpOnLevel =
{
   .atrib      = &fuelPumpOnLevelAtrib,
   .scale      = 0U,
   .value      = fuelPumpOnLevelValue,
   .units      = {'%', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t fuelPumpOffLevelValue[1U] = { 70U };
const eConfigAttributes fuelPumpOffLevelAtrib =
{
   .adr        = 28U,
   .min        = 0U,
   .max        = 100U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg fuelPumpOffLevel =
{
   .atrib      = &fuelPumpOffLevelAtrib,
   .scale      = 0U,
   .value      = fuelPumpOffLevelValue,
   .units      = {'%', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
static eConfigBitMap speedSetupBitMap[2U] = 
{
   { 1U, 0U },     // speedEnb
   { 2U, 1U },     // speedLowAlarmEnb
};
uint16_t speedSetupValue[1U] = { 0U };
const eConfigAttributes speedSetupAtrib =
{
   .adr        = 29U,
   .min        = 0U,
   .max        = 1023U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 2U,
};
eConfigReg speedSetup =
{
   .atrib      = &speedSetupAtrib,
   .scale      = 0U,
   .value      = speedSetupValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = speedSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t speedToothNumberValue[1U] = { 190U };
const eConfigAttributes speedToothNumberAtrib =
{
   .adr        = 30U,
   .min        = 10U,
   .max        = 500U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg speedToothNumber =
{
   .atrib      = &speedToothNumberAtrib,
   .scale      = 0U,
   .value      = speedToothNumberValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t speedLowAlarmLevelValue[1U] = { 1200U };
const eConfigAttributes speedLowAlarmLevelAtrib =
{
   .adr        = 31U,
   .min        = 0U,
   .max        = 6000U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg speedLowAlarmLevel =
{
   .atrib      = &speedLowAlarmLevelAtrib,
   .scale      = 0U,
   .value      = speedLowAlarmLevelValue,
   .units      = {'R', 'P', 'M', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t speedHightAlarmLevelValue[1U] = { 1710U };
const eConfigAttributes speedHightAlarmLevelAtrib =
{
   .adr        = 32U,
   .min        = 0U,
   .max        = 6000U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg speedHightAlarmLevel =
{
   .atrib      = &speedHightAlarmLevelAtrib,
   .scale      = 0U,
   .value      = speedHightAlarmLevelValue,
   .units      = {'R', 'P', 'M', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
static eConfigBitMap diaSetupBitMap[4U] = 
{
   { 31U, 0U },     // diaFunction
   { 32U, 5U },     // diaPolarity
   { 192U, 6U },     // diaAction
   { 768U, 8U },     // diaArming
};
uint16_t diaSetupValue[1U] = { 0U };
const eConfigAttributes diaSetupAtrib =
{
   .adr        = 33U,
   .min        = 0U,
   .max        = 1023U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
};
eConfigReg diaSetup =
{
   .atrib      = &diaSetupAtrib,
   .scale      = 0U,
   .value      = diaSetupValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = diaSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t diaDelayValue[1U] = { 15U };
const eConfigAttributes diaDelayAtrib =
{
   .adr        = 34U,
   .min        = 0U,
   .max        = 60U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg diaDelay =
{
   .atrib      = &diaDelayAtrib,
   .scale      = 0U,
   .value      = diaDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t diaMessageValue[16U] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
const eConfigAttributes diaMessageAtrib =
{
   .adr        = 35U,
   .min        = 0U,
   .max        = 0U,
   .type       = 'S',
   .len        = 16U,
   .bitMapSize = 0U,
};
eConfigReg diaMessage =
{
   .atrib      = &diaMessageAtrib,
   .scale      = 0U,
   .value      = diaMessageValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
static eConfigBitMap dibSetupBitMap[4U] = 
{
   { 31U, 0U },     // dibFunction
   { 32U, 5U },     // dibPolarity
   { 192U, 6U },     // dibAction
   { 768U, 8U },     // dibArming
};
uint16_t dibSetupValue[1U] = { 0U };
const eConfigAttributes dibSetupAtrib =
{
   .adr        = 36U,
   .min        = 0U,
   .max        = 1023U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
};
eConfigReg dibSetup =
{
   .atrib      = &dibSetupAtrib,
   .scale      = 0U,
   .value      = dibSetupValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = dibSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t dibDelayValue[1U] = { 15U };
const eConfigAttributes dibDelayAtrib =
{
   .adr        = 37U,
   .min        = 0U,
   .max        = 60U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg dibDelay =
{
   .atrib      = &dibDelayAtrib,
   .scale      = 0U,
   .value      = dibDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t dibMessageValue[16U] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
const eConfigAttributes dibMessageAtrib =
{
   .adr        = 38U,
   .min        = 0U,
   .max        = 0U,
   .type       = 'S',
   .len        = 16U,
   .bitMapSize = 0U,
};
eConfigReg dibMessage =
{
   .atrib      = &dibMessageAtrib,
   .scale      = 0U,
   .value      = dibMessageValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
static eConfigBitMap dicSetupBitMap[4U] = 
{
   { 31U, 0U },     // dicFunction
   { 32U, 5U },     // dicPolarity
   { 192U, 6U },     // dicAction
   { 768U, 8U },     // dicArming
};
uint16_t dicSetupValue[1U] = { 0U };
const eConfigAttributes dicSetupAtrib =
{
   .adr        = 39U,
   .min        = 0U,
   .max        = 1023U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
};
eConfigReg dicSetup =
{
   .atrib      = &dicSetupAtrib,
   .scale      = 0U,
   .value      = dicSetupValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = dicSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t dicDelayValue[1U] = { 15U };
const eConfigAttributes dicDelayAtrib =
{
   .adr        = 40U,
   .min        = 0U,
   .max        = 60U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg dicDelay =
{
   .atrib      = &dicDelayAtrib,
   .scale      = 0U,
   .value      = dicDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t dicMessageValue[16U] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
const eConfigAttributes dicMessageAtrib =
{
   .adr        = 41U,
   .min        = 0U,
   .max        = 0U,
   .type       = 'S',
   .len        = 16U,
   .bitMapSize = 0U,
};
eConfigReg dicMessage =
{
   .atrib      = &dicMessageAtrib,
   .scale      = 0U,
   .value      = dicMessageValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
static eConfigBitMap didSetupBitMap[4U] = 
{
   { 31U, 0U },     // didFunction
   { 32U, 5U },     // didPolarity
   { 192U, 6U },     // didAction
   { 768U, 8U },     // didArming
};
uint16_t didSetupValue[1U] = { 64U };
const eConfigAttributes didSetupAtrib =
{
   .adr        = 42U,
   .min        = 0U,
   .max        = 1023U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
};
eConfigReg didSetup =
{
   .atrib      = &didSetupAtrib,
   .scale      = 0U,
   .value      = didSetupValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = didSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t didDelayValue[1U] = { 15U };
const eConfigAttributes didDelayAtrib =
{
   .adr        = 43U,
   .min        = 0U,
   .max        = 60U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg didDelay =
{
   .atrib      = &didDelayAtrib,
   .scale      = 0U,
   .value      = didDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t didMessageValue[16U] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
const eConfigAttributes didMessageAtrib =
{
   .adr        = 44U,
   .min        = 0U,
   .max        = 0U,
   .type       = 'S',
   .len        = 16U,
   .bitMapSize = 0U,
};
eConfigReg didMessage =
{
   .atrib      = &didMessageAtrib,
   .scale      = 0U,
   .value      = didMessageValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
static eConfigBitMap doSetupBitMap[6U] = 
{
   { 1U, 0U },     // doaNOC
   { 2U, 1U },     // dobNOC
   { 4U, 2U },     // docNOC
   { 8U, 3U },     // dodNOC
   { 16U, 4U },     // doeNOC
   { 32U, 5U },     // dofNOC
};
uint16_t doSetupValue[1U] = { 0U };
const eConfigAttributes doSetupAtrib =
{
   .adr        = 45U,
   .min        = 0U,
   .max        = 1023U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 6U,
};
eConfigReg doSetup =
{
   .atrib      = &doSetupAtrib,
   .scale      = 0U,
   .value      = doSetupValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = doSetupBitMap
};
/*----------------------------------------------------------------*/
static eConfigBitMap doabTypeBitMap[2U] = 
{
   { 255U, 0U },     // doaType
   { 65280U, 8U },     // dobType
};
uint16_t doabTypeValue[1U] = { 0U };
const eConfigAttributes doabTypeAtrib =
{
   .adr        = 46U,
   .min        = 0U,
   .max        = 1023U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 2U,
};
eConfigReg doabType =
{
   .atrib      = &doabTypeAtrib,
   .scale      = 0U,
   .value      = doabTypeValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = doabTypeBitMap
};
/*----------------------------------------------------------------*/
static eConfigBitMap docdTypeBitMap[2U] = 
{
   { 255U, 0U },     // docType
   { 65280U, 8U },     // dodType
};
uint16_t docdTypeValue[1U] = { 0U };
const eConfigAttributes docdTypeAtrib =
{
   .adr        = 47U,
   .min        = 0U,
   .max        = 1023U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 2U,
};
eConfigReg docdType =
{
   .atrib      = &docdTypeAtrib,
   .scale      = 0U,
   .value      = docdTypeValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = docdTypeBitMap
};
/*----------------------------------------------------------------*/
static eConfigBitMap doefTypeBitMap[2U] = 
{
   { 255U, 0U },     // doeType
   { 65280U, 8U },     // dofType
};
uint16_t doefTypeValue[1U] = { 0U };
const eConfigAttributes doefTypeAtrib =
{
   .adr        = 48U,
   .min        = 0U,
   .max        = 1023U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 2U,
};
eConfigReg doefType =
{
   .atrib      = &doefTypeAtrib,
   .scale      = 0U,
   .value      = doefTypeValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = doefTypeBitMap
};
/*----------------------------------------------------------------*/
uint16_t timerMainsTransientDelayValue[1U] = { 1U };
const eConfigAttributes timerMainsTransientDelayAtrib =
{
   .adr        = 49U,
   .min        = 1U,
   .max        = 100U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg timerMainsTransientDelay =
{
   .atrib      = &timerMainsTransientDelayAtrib,
   .scale      = -1,
   .value      = timerMainsTransientDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t timerStartDelayValue[1U] = { 5U };
const eConfigAttributes timerStartDelayAtrib =
{
   .adr        = 50U,
   .min        = 0U,
   .max        = 300U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg timerStartDelay =
{
   .atrib      = &timerStartDelayAtrib,
   .scale      = 0U,
   .value      = timerStartDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t timerCrankingValue[1U] = { 10U };
const eConfigAttributes timerCrankingAtrib =
{
   .adr        = 51U,
   .min        = 1U,
   .max        = 30U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg timerCranking =
{
   .atrib      = &timerCrankingAtrib,
   .scale      = 0U,
   .value      = timerCrankingValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t timerCrankDelayValue[1U] = { 10U };
const eConfigAttributes timerCrankDelayAtrib =
{
   .adr        = 52U,
   .min        = 1U,
   .max        = 30U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg timerCrankDelay =
{
   .atrib      = &timerCrankDelayAtrib,
   .scale      = 0U,
   .value      = timerCrankDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t timerStartupIdleTimeValue[1U] = { 10U };
const eConfigAttributes timerStartupIdleTimeAtrib =
{
   .adr        = 53U,
   .min        = 0U,
   .max        = 300U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg timerStartupIdleTime =
{
   .atrib      = &timerStartupIdleTimeAtrib,
   .scale      = 0U,
   .value      = timerStartupIdleTimeValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t timerNominalRPMDelayValue[1U] = { 10U };
const eConfigAttributes timerNominalRPMDelayAtrib =
{
   .adr        = 54U,
   .min        = 1U,
   .max        = 30U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg timerNominalRPMDelay =
{
   .atrib      = &timerNominalRPMDelayAtrib,
   .scale      = 0U,
   .value      = timerNominalRPMDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t timerSafetyOnDelayValue[1U] = { 10U };
const eConfigAttributes timerSafetyOnDelayAtrib =
{
   .adr        = 55U,
   .min        = 1U,
   .max        = 60U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg timerSafetyOnDelay =
{
   .atrib      = &timerSafetyOnDelayAtrib,
   .scale      = 0U,
   .value      = timerSafetyOnDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t timerWarmingValue[1U] = { 1U };
const eConfigAttributes timerWarmingAtrib =
{
   .adr        = 56U,
   .min        = 0U,
   .max        = 300U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg timerWarming =
{
   .atrib      = &timerWarmingAtrib,
   .scale      = 0U,
   .value      = timerWarmingValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t timerTransferDelayValue[1U] = { 6U };
const eConfigAttributes timerTransferDelayAtrib =
{
   .adr        = 57U,
   .min        = 1U,
   .max        = 300U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg timerTransferDelay =
{
   .atrib      = &timerTransferDelayAtrib,
   .scale      = -1,
   .value      = timerTransferDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t timerGenBreakerTripPulseValue[1U] = { 5U };
const eConfigAttributes timerGenBreakerTripPulseAtrib =
{
   .adr        = 58U,
   .min        = 1U,
   .max        = 50U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg timerGenBreakerTripPulse =
{
   .atrib      = &timerGenBreakerTripPulseAtrib,
   .scale      = -1,
   .value      = timerGenBreakerTripPulseValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t timerGenBreakerClosePulseValue[1U] = { 5U };
const eConfigAttributes timerGenBreakerClosePulseAtrib =
{
   .adr        = 59U,
   .min        = 1U,
   .max        = 50U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg timerGenBreakerClosePulse =
{
   .atrib      = &timerGenBreakerClosePulseAtrib,
   .scale      = -1,
   .value      = timerGenBreakerClosePulseValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t timerMainsBreakerTripPulseValue[1U] = { 5U };
const eConfigAttributes timerMainsBreakerTripPulseAtrib =
{
   .adr        = 60U,
   .min        = 1U,
   .max        = 50U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg timerMainsBreakerTripPulse =
{
   .atrib      = &timerMainsBreakerTripPulseAtrib,
   .scale      = -1,
   .value      = timerMainsBreakerTripPulseValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t timerMainsBreakerClosePulseValue[1U] = { 5U };
const eConfigAttributes timerMainsBreakerClosePulseAtrib =
{
   .adr        = 61U,
   .min        = 1U,
   .max        = 50U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg timerMainsBreakerClosePulse =
{
   .atrib      = &timerMainsBreakerClosePulseAtrib,
   .scale      = -1,
   .value      = timerMainsBreakerClosePulseValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t timerReturnDelayValue[1U] = { 30U };
const eConfigAttributes timerReturnDelayAtrib =
{
   .adr        = 62U,
   .min        = 0U,
   .max        = 300U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg timerReturnDelay =
{
   .atrib      = &timerReturnDelayAtrib,
   .scale      = 0U,
   .value      = timerReturnDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t timerCoolingValue[1U] = { 60U };
const eConfigAttributes timerCoolingAtrib =
{
   .adr        = 63U,
   .min        = 0U,
   .max        = 300U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg timerCooling =
{
   .atrib      = &timerCoolingAtrib,
   .scale      = 0U,
   .value      = timerCoolingValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t timerCoolingIdleValue[1U] = { 20U };
const eConfigAttributes timerCoolingIdleAtrib =
{
   .adr        = 64U,
   .min        = 0U,
   .max        = 300U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg timerCoolingIdle =
{
   .atrib      = &timerCoolingIdleAtrib,
   .scale      = 0U,
   .value      = timerCoolingIdleValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t timerSolenoidHoldValue[1U] = { 20U };
const eConfigAttributes timerSolenoidHoldAtrib =
{
   .adr        = 65U,
   .min        = 0U,
   .max        = 60U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg timerSolenoidHold =
{
   .atrib      = &timerSolenoidHoldAtrib,
   .scale      = 0U,
   .value      = timerSolenoidHoldValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t timerFailStopDelayValue[1U] = { 30U };
const eConfigAttributes timerFailStopDelayAtrib =
{
   .adr        = 66U,
   .min        = 1U,
   .max        = 60U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg timerFailStopDelay =
{
   .atrib      = &timerFailStopDelayAtrib,
   .scale      = 0U,
   .value      = timerFailStopDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t timerGenTransientDelayValue[1U] = { 10U };
const eConfigAttributes timerGenTransientDelayAtrib =
{
   .adr        = 67U,
   .min        = 1U,
   .max        = 100U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg timerGenTransientDelay =
{
   .atrib      = &timerGenTransientDelayAtrib,
   .scale      = -1,
   .value      = timerGenTransientDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
static eConfigBitMap genSetupBitMap[2U] = 
{
   { 1U, 0U },     // genPowerGeneratorControlEnb
   { 6U, 1U },     // genAcSys
};
uint16_t genSetupValue[1U] = { 1U };
const eConfigAttributes genSetupAtrib =
{
   .adr        = 68U,
   .min        = 0U,
   .max        = 255U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 2U,
};
eConfigReg genSetup =
{
   .atrib      = &genSetupAtrib,
   .scale      = 0U,
   .value      = genSetupValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = genSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t genRatedActivePowerLevelValue[1U] = { 40000U };
const eConfigAttributes genRatedActivePowerLevelAtrib =
{
   .adr        = 69U,
   .min        = 0U,
   .max        = 10000U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genRatedActivePowerLevel =
{
   .atrib      = &genRatedActivePowerLevelAtrib,
   .scale      = 0U,
   .value      = genRatedActivePowerLevelValue,
   .units      = {'к', 'В', 'т', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genRatedReactivePowerLevelValue[1U] = { 0U };
const eConfigAttributes genRatedReactivePowerLevelAtrib =
{
   .adr        = 70U,
   .min        = 0U,
   .max        = 10000U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genRatedReactivePowerLevel =
{
   .atrib      = &genRatedReactivePowerLevelAtrib,
   .scale      = 0U,
   .value      = genRatedReactivePowerLevelValue,
   .units      = {'к', 'В', 'А', 'Р'},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genRatedApparentPowerLevelValue[1U] = { 200U };
const eConfigAttributes genRatedApparentPowerLevelAtrib =
{
   .adr        = 71U,
   .min        = 0U,
   .max        = 10000U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genRatedApparentPowerLevel =
{
   .atrib      = &genRatedApparentPowerLevelAtrib,
   .scale      = 0U,
   .value      = genRatedApparentPowerLevelValue,
   .units      = {'к', 'В', 'А', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genRatedFrequencyLevelValue[1U] = { 500U };
const eConfigAttributes genRatedFrequencyLevelAtrib =
{
   .adr        = 72U,
   .min        = 2U,
   .max        = 747U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genRatedFrequencyLevel =
{
   .atrib      = &genRatedFrequencyLevelAtrib,
   .scale      = -1,
   .value      = genRatedFrequencyLevelValue,
   .units      = {'Г', 'ц', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genCurrentPrimaryLevelValue[1U] = { 600U };
const eConfigAttributes genCurrentPrimaryLevelAtrib =
{
   .adr        = 73U,
   .min        = 5U,
   .max        = 8000U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genCurrentPrimaryLevel =
{
   .atrib      = &genCurrentPrimaryLevelAtrib,
   .scale      = 0U,
   .value      = genCurrentPrimaryLevelValue,
   .units      = {'А', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genCurrentFullLoadRatingLevelValue[1U] = { 500U };
const eConfigAttributes genCurrentFullLoadRatingLevelAtrib =
{
   .adr        = 74U,
   .min        = 5U,
   .max        = 8000U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genCurrentFullLoadRatingLevel =
{
   .atrib      = &genCurrentFullLoadRatingLevelAtrib,
   .scale      = 0U,
   .value      = genCurrentFullLoadRatingLevelValue,
   .units      = {'А', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
static eConfigBitMap genAlarmsBitMap[12U] = 
{
   { 1U, 0U },     // genUnderVoltageAlarmEnb
   { 2U, 1U },     // genUnderVoltagePreAlarmEnb
   { 4U, 2U },     // genOverVoltagePreAlarmEnb
   { 8U, 3U },     // genUnderFrequencyAlarmEnb
   { 16U, 4U },     // genUnderFrequencyPreAlarmEnb
   { 32U, 5U },     // genOverFrequencyPreAlarmEnb
   { 64U, 6U },     // genOverFrequencyAlarmEnb
   { 128U, 7U },     // genCurrentOverloadProtectionEnb
   { 256U, 8U },     // genCurrentOverPhaseImbalanceEnb
   { 1536U, 9U },     // genCurrentOverAlarmAction
   { 2048U, 11U },     // genCurrentOverloadProtectionAction
   { 4096U, 12U },     // genCurrentOverPhaseImbalanceAction
};
uint16_t genAlarmsValue[1U] = { 7167U };
const eConfigAttributes genAlarmsAtrib =
{
   .adr        = 75U,
   .min        = 0U,
   .max        = 65535U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 12U,
};
eConfigReg genAlarms =
{
   .atrib      = &genAlarmsAtrib,
   .scale      = 0U,
   .value      = genAlarmsValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = genAlarmsBitMap
};
/*----------------------------------------------------------------*/
uint16_t genUnderVoltageAlarmLevelValue[1U] = { 318U };
const eConfigAttributes genUnderVoltageAlarmLevelAtrib =
{
   .adr        = 76U,
   .min        = 86U,
   .max        = 708U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genUnderVoltageAlarmLevel =
{
   .atrib      = &genUnderVoltageAlarmLevelAtrib,
   .scale      = 0U,
   .value      = genUnderVoltageAlarmLevelValue,
   .units      = {'В', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genUnderVoltagePreAlarmLevelValue[1U] = { 339U };
const eConfigAttributes genUnderVoltagePreAlarmLevelAtrib =
{
   .adr        = 77U,
   .min        = 88U,
   .max        = 710U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genUnderVoltagePreAlarmLevel =
{
   .atrib      = &genUnderVoltagePreAlarmLevelAtrib,
   .scale      = 0U,
   .value      = genUnderVoltagePreAlarmLevelValue,
   .units      = {'В', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genOverVoltagePreAlarmLevelValue[1U] = { 439U };
const eConfigAttributes genOverVoltagePreAlarmLevelAtrib =
{
   .adr        = 78U,
   .min        = 93U,
   .max        = 715U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genOverVoltagePreAlarmLevel =
{
   .atrib      = &genOverVoltagePreAlarmLevelAtrib,
   .scale      = 0U,
   .value      = genOverVoltagePreAlarmLevelValue,
   .units      = {'В', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genOverVoltageAlarmLevelValue[1U] = { 458U };
const eConfigAttributes genOverVoltageAlarmLevelAtrib =
{
   .adr        = 79U,
   .min        = 95U,
   .max        = 717U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genOverVoltageAlarmLevel =
{
   .atrib      = &genOverVoltageAlarmLevelAtrib,
   .scale      = 0U,
   .value      = genOverVoltageAlarmLevelValue,
   .units      = {'В', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genUnderFrequencyAlarmLevelValue[1U] = { 400U };
const eConfigAttributes genUnderFrequencyAlarmLevelAtrib =
{
   .adr        = 80U,
   .min        = 0U,
   .max        = 744U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genUnderFrequencyAlarmLevel =
{
   .atrib      = &genUnderFrequencyAlarmLevelAtrib,
   .scale      = -1,
   .value      = genUnderFrequencyAlarmLevelValue,
   .units      = {'Г', 'ц', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genUnderFrequencyPreAlarmLevelValue[1U] = { 420U };
const eConfigAttributes genUnderFrequencyPreAlarmLevelAtrib =
{
   .adr        = 81U,
   .min        = 1U,
   .max        = 745U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genUnderFrequencyPreAlarmLevel =
{
   .atrib      = &genUnderFrequencyPreAlarmLevelAtrib,
   .scale      = -1,
   .value      = genUnderFrequencyPreAlarmLevelValue,
   .units      = {'Г', 'ц', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genOverFrequencyPreAlarmLevelValue[1U] = { 540U };
const eConfigAttributes genOverFrequencyPreAlarmLevelAtrib =
{
   .adr        = 82U,
   .min        = 4U,
   .max        = 747U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genOverFrequencyPreAlarmLevel =
{
   .atrib      = &genOverFrequencyPreAlarmLevelAtrib,
   .scale      = -1,
   .value      = genOverFrequencyPreAlarmLevelValue,
   .units      = {'Г', 'ц', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genOverFrequencyAlarmLevelValue[1U] = { 550U };
const eConfigAttributes genOverFrequencyAlarmLevelAtrib =
{
   .adr        = 83U,
   .min        = 5U,
   .max        = 749U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genOverFrequencyAlarmLevel =
{
   .atrib      = &genOverFrequencyAlarmLevelAtrib,
   .scale      = -1,
   .value      = genOverFrequencyAlarmLevelValue,
   .units      = {'Г', 'ц', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genOverCurrentThermalProtectionLevelValue[1U] = { 20U };
const eConfigAttributes genOverCurrentThermalProtectionLevelAtrib =
{
   .adr        = 84U,
   .min        = 90U,
   .max        = 200U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genOverCurrentThermalProtectionLevel =
{
   .atrib      = &genOverCurrentThermalProtectionLevelAtrib,
   .scale      = 0U,
   .value      = genOverCurrentThermalProtectionLevelValue,
   .units      = {'%', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genOverCurrentCutoffLevelValue[1U] = { 40U };
const eConfigAttributes genOverCurrentCutoffLevelAtrib =
{
   .adr        = 85U,
   .min        = 200U,
   .max        = 500U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genOverCurrentCutoffLevel =
{
   .atrib      = &genOverCurrentCutoffLevelAtrib,
   .scale      = 0U,
   .value      = genOverCurrentCutoffLevelValue,
   .units      = {'%', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genOverCurrentAlarmLevelValue[1U] = { 100U };
const eConfigAttributes genOverCurrentAlarmLevelAtrib =
{
   .adr        = 86U,
   .min        = 50U,
   .max        = 120U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genOverCurrentAlarmLevel =
{
   .atrib      = &genOverCurrentAlarmLevelAtrib,
   .scale      = 0U,
   .value      = genOverCurrentAlarmLevelValue,
   .units      = {'%', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genOverCurrentAlarmDelayValue[1U] = { 60U };
const eConfigAttributes genOverCurrentAlarmDelayAtrib =
{
   .adr        = 87U,
   .min        = 0U,
   .max        = 3600U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genOverCurrentAlarmDelay =
{
   .atrib      = &genOverCurrentAlarmDelayAtrib,
   .scale      = 0U,
   .value      = genOverCurrentAlarmDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genCurrentOverloadProtectionLevelValue[1U] = { 100U };
const eConfigAttributes genCurrentOverloadProtectionLevelAtrib =
{
   .adr        = 88U,
   .min        = 1U,
   .max        = 125U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genCurrentOverloadProtectionLevel =
{
   .atrib      = &genCurrentOverloadProtectionLevelAtrib,
   .scale      = 0U,
   .value      = genCurrentOverloadProtectionLevelValue,
   .units      = {'%', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genCurrentOverloadProtectionDelayValue[1U] = { 5U };
const eConfigAttributes genCurrentOverloadProtectionDelayAtrib =
{
   .adr        = 89U,
   .min        = 0U,
   .max        = 3600U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genCurrentOverloadProtectionDelay =
{
   .atrib      = &genCurrentOverloadProtectionDelayAtrib,
   .scale      = 0U,
   .value      = genCurrentOverloadProtectionDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genCurrentOverPhaseImbalanceLevelValue[1U] = { 100U };
const eConfigAttributes genCurrentOverPhaseImbalanceLevelAtrib =
{
   .adr        = 90U,
   .min        = 1U,
   .max        = 125U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genCurrentOverPhaseImbalanceLevel =
{
   .atrib      = &genCurrentOverPhaseImbalanceLevelAtrib,
   .scale      = 0U,
   .value      = genCurrentOverPhaseImbalanceLevelValue,
   .units      = {'%', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t genCurrentOverPhaseImbalanceDelayValue[1U] = { 60U };
const eConfigAttributes genCurrentOverPhaseImbalanceDelayAtrib =
{
   .adr        = 91U,
   .min        = 0U,
   .max        = 3600U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg genCurrentOverPhaseImbalanceDelay =
{
   .atrib      = &genCurrentOverPhaseImbalanceDelayAtrib,
   .scale      = 0U,
   .value      = genCurrentOverPhaseImbalanceDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
static eConfigBitMap mainsSetupBitMap[2U] = 
{
   { 1U, 0U },     // mainsControlEnb
   { 2U, 1U },     // mainsPowerOffImmediatelyEnb
};
uint16_t mainsSetupValue[1U] = { 1U };
const eConfigAttributes mainsSetupAtrib =
{
   .adr        = 92U,
   .min        = 0U,
   .max        = 31U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 2U,
};
eConfigReg mainsSetup =
{
   .atrib      = &mainsSetupAtrib,
   .scale      = 0U,
   .value      = mainsSetupValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = mainsSetupBitMap
};
/*----------------------------------------------------------------*/
static eConfigBitMap mainsAlarmsBitMap[4U] = 
{
   { 1U, 0U },     // mainsUnderVoltageAlarmEnb
   { 2U, 1U },     // mainsOverVoltageAlarmEnb
   { 4U, 2U },     // mainsUnderFrequencyAlarmEnb
   { 8U, 3U },     // mainsOverFrequencyAlarmEnb
};
uint16_t mainsAlarmsValue[1U] = { 15U };
const eConfigAttributes mainsAlarmsAtrib =
{
   .adr        = 93U,
   .min        = 0U,
   .max        = 15U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
};
eConfigReg mainsAlarms =
{
   .atrib      = &mainsAlarmsAtrib,
   .scale      = 0U,
   .value      = mainsAlarmsValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = mainsAlarmsBitMap
};
/*----------------------------------------------------------------*/
uint16_t mainsUnderVoltageAlarmLevelValue[1U] = { 318U };
const eConfigAttributes mainsUnderVoltageAlarmLevelAtrib =
{
   .adr        = 94U,
   .min        = 86U,
   .max        = 713U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg mainsUnderVoltageAlarmLevel =
{
   .atrib      = &mainsUnderVoltageAlarmLevelAtrib,
   .scale      = 0U,
   .value      = mainsUnderVoltageAlarmLevelValue,
   .units      = {'В', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t mainsOverVoltageAlarmLevelValue[1U] = { 438U };
const eConfigAttributes mainsOverVoltageAlarmLevelAtrib =
{
   .adr        = 95U,
   .min        = 90U,
   .max        = 717U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg mainsOverVoltageAlarmLevel =
{
   .atrib      = &mainsOverVoltageAlarmLevelAtrib,
   .scale      = 0U,
   .value      = mainsOverVoltageAlarmLevelValue,
   .units      = {'В', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t mainsUnderFrequencyAlarmLevelValue[1U] = { 450U };
const eConfigAttributes mainsUnderFrequencyAlarmLevelAtrib =
{
   .adr        = 96U,
   .min        = 0U,
   .max        = 747U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg mainsUnderFrequencyAlarmLevel =
{
   .atrib      = &mainsUnderFrequencyAlarmLevelAtrib,
   .scale      = -1,
   .value      = mainsUnderFrequencyAlarmLevelValue,
   .units      = {'Г', 'ц', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t mainsOverFrequencyAlarmLevelValue[1U] = { 520U };
const eConfigAttributes mainsOverFrequencyAlarmLevelAtrib =
{
   .adr        = 97U,
   .min        = 2U,
   .max        = 749U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg mainsOverFrequencyAlarmLevel =
{
   .atrib      = &mainsOverFrequencyAlarmLevelAtrib,
   .scale      = -1,
   .value      = mainsOverFrequencyAlarmLevelValue,
   .units      = {'Г', 'ц', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
static eConfigBitMap engineSetupBitMap[2U] = 
{
   { 15U, 0U },     // engineStartAttempts
   { 16U, 4U },     // enginePreHeatEnb
};
uint16_t engineSetupValue[1U] = { 3U };
const eConfigAttributes engineSetupAtrib =
{
   .adr        = 98U,
   .min        = 0U,
   .max        = 63U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 2U,
};
eConfigReg engineSetup =
{
   .atrib      = &engineSetupAtrib,
   .scale      = 0U,
   .value      = engineSetupValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = engineSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t enginePreHeatLevelValue[1U] = { 50U };
const eConfigAttributes enginePreHeatLevelAtrib =
{
   .adr        = 99U,
   .min        = 0U,
   .max        = 100U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg enginePreHeatLevel =
{
   .atrib      = &enginePreHeatLevelAtrib,
   .scale      = 0U,
   .value      = enginePreHeatLevelValue,
   .units      = {'C', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t enginePreHeatDelayValue[1U] = { 0U };
const eConfigAttributes enginePreHeatDelayAtrib =
{
   .adr        = 100U,
   .min        = 0U,
   .max        = 3600U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg enginePreHeatDelay =
{
   .atrib      = &enginePreHeatDelayAtrib,
   .scale      = 0U,
   .value      = enginePreHeatDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t engineWorkTimeInputValue[1U] = { 0U };
const eConfigAttributes engineWorkTimeInputAtrib =
{
   .adr        = 101U,
   .min        = 0U,
   .max        = 65535U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg engineWorkTimeInput =
{
   .atrib      = &engineWorkTimeInputAtrib,
   .scale      = 0U,
   .value      = engineWorkTimeInputValue,
   .units      = {'ч', 'а', 'с', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t engineStartsNumberInputValue[1U] = { 0U };
const eConfigAttributes engineStartsNumberInputAtrib =
{
   .adr        = 102U,
   .min        = 0U,
   .max        = 65535U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg engineStartsNumberInput =
{
   .atrib      = &engineStartsNumberInputAtrib,
   .scale      = 0U,
   .value      = engineStartsNumberInputValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
static eConfigBitMap starterStopSetupBitMap[4U] = 
{
   { 1U, 0U },     // starterOilPressureCheckOnStartEnb
   { 2U, 1U },     // starterStopOilPressureEnb
   { 4U, 2U },     // starterStopChargeAlternatorEnb
   { 8U, 3U },     // starterStopSpeedEnb
};
uint16_t starterStopSetupValue[1U] = { 5U };
const eConfigAttributes starterStopSetupAtrib =
{
   .adr        = 103U,
   .min        = 0U,
   .max        = 7U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
};
eConfigReg starterStopSetup =
{
   .atrib      = &starterStopSetupAtrib,
   .scale      = 0U,
   .value      = starterStopSetupValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = starterStopSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t starterStopGenFreqLevelValue[1U] = { 210U };
const eConfigAttributes starterStopGenFreqLevelAtrib =
{
   .adr        = 104U,
   .min        = 0U,
   .max        = 400U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg starterStopGenFreqLevel =
{
   .atrib      = &starterStopGenFreqLevelAtrib,
   .scale      = -1,
   .value      = starterStopGenFreqLevelValue,
   .units      = {'Г', 'ц', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t starterStopOilPressureLevelValue[1U] = { 20U };
const eConfigAttributes starterStopOilPressureLevelAtrib =
{
   .adr        = 105U,
   .min        = 5U,
   .max        = 40U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg starterStopOilPressureLevel =
{
   .atrib      = &starterStopOilPressureLevelAtrib,
   .scale      = -1,
   .value      = starterStopOilPressureLevelValue,
   .units      = {'Б', 'а', 'р', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t starterStopChargeAlternatorLevelValue[1U] = { 60U };
const eConfigAttributes starterStopChargeAlternatorLevelAtrib =
{
   .adr        = 106U,
   .min        = 0U,
   .max        = 400U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg starterStopChargeAlternatorLevel =
{
   .atrib      = &starterStopChargeAlternatorLevelAtrib,
   .scale      = -1,
   .value      = starterStopChargeAlternatorLevelValue,
   .units      = {'В', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t starterStopSpeedLevelValue[1U] = { 15000U };
const eConfigAttributes starterStopSpeedLevelAtrib =
{
   .adr        = 107U,
   .min        = 0U,
   .max        = 60000U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg starterStopSpeedLevel =
{
   .atrib      = &starterStopSpeedLevelAtrib,
   .scale      = -1,
   .value      = starterStopSpeedLevelValue,
   .units      = {'R', 'P', 'M', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
static eConfigBitMap batteryAlarmsBitMap[4U] = 
{
   { 1U, 0U },     // batteryUnderVoltageEnb
   { 2U, 1U },     // batteryOverVoltageEnb
   { 4U, 2U },     // batteryChargeShutdownEnb
   { 8U, 3U },     // batteryChargeWarningEnb
};
uint16_t batteryAlarmsValue[1U] = { 15U };
const eConfigAttributes batteryAlarmsAtrib =
{
   .adr        = 108U,
   .min        = 0U,
   .max        = 15U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 4U,
};
eConfigReg batteryAlarms =
{
   .atrib      = &batteryAlarmsAtrib,
   .scale      = 0U,
   .value      = batteryAlarmsValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = batteryAlarmsBitMap
};
/*----------------------------------------------------------------*/
uint16_t batteryUnderVoltageLevelValue[1U] = { 100U };
const eConfigAttributes batteryUnderVoltageLevelAtrib =
{
   .adr        = 109U,
   .min        = 0U,
   .max        = 397U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg batteryUnderVoltageLevel =
{
   .atrib      = &batteryUnderVoltageLevelAtrib,
   .scale      = -1,
   .value      = batteryUnderVoltageLevelValue,
   .units      = {'В', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t batteryUnderVoltageDelayValue[1U] = { 60U };
const eConfigAttributes batteryUnderVoltageDelayAtrib =
{
   .adr        = 110U,
   .min        = 0U,
   .max        = 65535U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg batteryUnderVoltageDelay =
{
   .atrib      = &batteryUnderVoltageDelayAtrib,
   .scale      = 0U,
   .value      = batteryUnderVoltageDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t batteryOverVoltageLevelValue[1U] = { 300U };
const eConfigAttributes batteryOverVoltageLevelAtrib =
{
   .adr        = 111U,
   .min        = 2U,
   .max        = 400U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg batteryOverVoltageLevel =
{
   .atrib      = &batteryOverVoltageLevelAtrib,
   .scale      = -1,
   .value      = batteryOverVoltageLevelValue,
   .units      = {'В', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t batteryOverVoltageDelayValue[1U] = { 60U };
const eConfigAttributes batteryOverVoltageDelayAtrib =
{
   .adr        = 112U,
   .min        = 0U,
   .max        = 65535U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg batteryOverVoltageDelay =
{
   .atrib      = &batteryOverVoltageDelayAtrib,
   .scale      = 0U,
   .value      = batteryOverVoltageDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t batteryChargeShutdownLevelValue[1U] = { 40U };
const eConfigAttributes batteryChargeShutdownLevelAtrib =
{
   .adr        = 113U,
   .min        = 0U,
   .max        = 388U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg batteryChargeShutdownLevel =
{
   .atrib      = &batteryChargeShutdownLevelAtrib,
   .scale      = -1,
   .value      = batteryChargeShutdownLevelValue,
   .units      = {'В', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t batteryChargeShutdownDelayValue[1U] = { 5U };
const eConfigAttributes batteryChargeShutdownDelayAtrib =
{
   .adr        = 114U,
   .min        = 0U,
   .max        = 3600U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg batteryChargeShutdownDelay =
{
   .atrib      = &batteryChargeShutdownDelayAtrib,
   .scale      = 0U,
   .value      = batteryChargeShutdownDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t batteryChargeWarningLevelValue[1U] = { 60U };
const eConfigAttributes batteryChargeWarningLevelAtrib =
{
   .adr        = 115U,
   .min        = 1U,
   .max        = 390U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg batteryChargeWarningLevel =
{
   .atrib      = &batteryChargeWarningLevelAtrib,
   .scale      = -1,
   .value      = batteryChargeWarningLevelValue,
   .units      = {'В', ' ', ' ', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t batteryChargeWarningDelayValue[1U] = { 5U };
const eConfigAttributes batteryChargeWarningDelayAtrib =
{
   .adr        = 116U,
   .min        = 0U,
   .max        = 3600U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg batteryChargeWarningDelay =
{
   .atrib      = &batteryChargeWarningDelayAtrib,
   .scale      = 0U,
   .value      = batteryChargeWarningDelayValue,
   .units      = {'с', 'е', 'к', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
static eConfigBitMap maintenanceAlarmsBitMap[6U] = 
{
   { 1U, 0U },     // maintenanceAlarmOilEnb
   { 2U, 1U },     // maintenanceAlarmOilAction
   { 4U, 2U },     // maintenanceAlarmAirEnb
   { 8U, 3U },     // maintenanceAlarmAirAction
   { 16U, 4U },     // maintenanceAlarmFuelEnb
   { 32U, 5U },     // maintenanceAlarmFuelAction
};
uint16_t maintenanceAlarmsValue[1U] = { 42U };
const eConfigAttributes maintenanceAlarmsAtrib =
{
   .adr        = 117U,
   .min        = 0U,
   .max        = 63U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 6U,
};
eConfigReg maintenanceAlarms =
{
   .atrib      = &maintenanceAlarmsAtrib,
   .scale      = 0U,
   .value      = maintenanceAlarmsValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = maintenanceAlarmsBitMap
};
/*----------------------------------------------------------------*/
uint16_t maintenanceAlarmOilTimeValue[1U] = { 10U };
const eConfigAttributes maintenanceAlarmOilTimeAtrib =
{
   .adr        = 118U,
   .min        = 10U,
   .max        = 5000U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg maintenanceAlarmOilTime =
{
   .atrib      = &maintenanceAlarmOilTimeAtrib,
   .scale      = 0U,
   .value      = maintenanceAlarmOilTimeValue,
   .units      = {'ч', 'а', 'с', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t maintenanceAlarmAirTimeValue[1U] = { 10U };
const eConfigAttributes maintenanceAlarmAirTimeAtrib =
{
   .adr        = 119U,
   .min        = 10U,
   .max        = 5000U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg maintenanceAlarmAirTime =
{
   .atrib      = &maintenanceAlarmAirTimeAtrib,
   .scale      = 0U,
   .value      = maintenanceAlarmAirTimeValue,
   .units      = {'ч', 'а', 'с', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
uint16_t maintenanceAlarmFuelTimeValue[1U] = { 10U };
const eConfigAttributes maintenanceAlarmFuelTimeAtrib =
{
   .adr        = 120U,
   .min        = 10U,
   .max        = 5000U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 0U,
};
eConfigReg maintenanceAlarmFuelTime =
{
   .atrib      = &maintenanceAlarmFuelTimeAtrib,
   .scale      = 0U,
   .value      = maintenanceAlarmFuelTimeValue,
   .units      = {'ч', 'а', 'с', ' '},
   .bitMap     = NULL,
};
/*----------------------------------------------------------------*/
static eConfigBitMap logSetupBitMap[1U] = 
{
   { 1U, 0U },     // logSaveWarningEventsEnb
};
uint16_t logSetupValue[1U] = { 1U };
const eConfigAttributes logSetupAtrib =
{
   .adr        = 121U,
   .min        = 0U,
   .max        = 65535U,
   .type       = 'U',
   .len        = 1U,
   .bitMapSize = 1U,
};
eConfigReg logSetup =
{
   .atrib      = &logSetupAtrib,
   .scale      = 0U,
   .value      = logSetupValue,
   .units      = {' ', ' ', ' ', ' '},
   .bitMap     = logSetupBitMap
};
/*----------------------------------------------------------------*/

eConfigReg* const configReg[SETTING_REGISTER_NUMBER] = { &versionController, &versionFirmware, &serialNumber, &displayBrightnesLevel, &displayContarstLevel, &displaySleepDelay, &hysteresisLevel, &moduleSetup, &oilPressureSetup, &oilPressureAlarmLevel, &oilPressurePreAlarmLevel, &coolantTempSetup, &coolantHightTempAlarmLevel, &coolantHightTempPreAlarmLevel, &coolantTempHeaterOffLevel, &coolantTempHeaterOnLevel, &coolantTempCoolerOffLevel, &coolantTempCoolerOnLevel, &fuelLevelSetup, &fuelLevelLowAlarmLevel, &fuelLevelLowAlarmDelay, &fuelLevelLowPreAlarmLevel, &fuelLevelLowPreAlarmDelay, &fuelLevelHightPreAlarmLevel, &fuelLevelHightPreAlarmDelay, &fuelLevelHightAlarmLevel, &fuelLevelHightAlarmDelay, &fuelPumpOnLevel, &fuelPumpOffLevel, &speedSetup, &speedToothNumber, &speedLowAlarmLevel, &speedHightAlarmLevel, &diaSetup, &diaDelay, &diaMessage, &dibSetup, &dibDelay, &dibMessage, &dicSetup, &dicDelay, &dicMessage, &didSetup, &didDelay, &didMessage, &doSetup, &doabType, &docdType, &doefType, &timerMainsTransientDelay, &timerStartDelay, &timerCranking, &timerCrankDelay, &timerStartupIdleTime, &timerNominalRPMDelay, &timerSafetyOnDelay, &timerWarming, &timerTransferDelay, &timerGenBreakerTripPulse, &timerGenBreakerClosePulse, &timerMainsBreakerTripPulse, &timerMainsBreakerClosePulse, &timerReturnDelay, &timerCooling, &timerCoolingIdle, &timerSolenoidHold, &timerFailStopDelay, &timerGenTransientDelay, &genSetup, &genRatedActivePowerLevel, &genRatedReactivePowerLevel, &genRatedApparentPowerLevel, &genRatedFrequencyLevel, &genCurrentPrimaryLevel, &genCurrentFullLoadRatingLevel, &genAlarms, &genUnderVoltageAlarmLevel, &genUnderVoltagePreAlarmLevel, &genOverVoltagePreAlarmLevel, &genOverVoltageAlarmLevel, &genUnderFrequencyAlarmLevel, &genUnderFrequencyPreAlarmLevel, &genOverFrequencyPreAlarmLevel, &genOverFrequencyAlarmLevel, &genOverCurrentThermalProtectionLevel, &genOverCurrentCutoffLevel, &genOverCurrentAlarmLevel, &genOverCurrentAlarmDelay, &genCurrentOverloadProtectionLevel, &genCurrentOverloadProtectionDelay, &genCurrentOverPhaseImbalanceLevel, &genCurrentOverPhaseImbalanceDelay, &mainsSetup, &mainsAlarms, &mainsUnderVoltageAlarmLevel, &mainsOverVoltageAlarmLevel, &mainsUnderFrequencyAlarmLevel, &mainsOverFrequencyAlarmLevel, &engineSetup, &enginePreHeatLevel, &enginePreHeatDelay, &engineWorkTimeInput, &engineStartsNumberInput, &starterStopSetup, &starterStopGenFreqLevel, &starterStopOilPressureLevel, &starterStopChargeAlternatorLevel, &starterStopSpeedLevel, &batteryAlarms, &batteryUnderVoltageLevel, &batteryUnderVoltageDelay, &batteryOverVoltageLevel, &batteryOverVoltageDelay, &batteryChargeShutdownLevel, &batteryChargeShutdownDelay, &batteryChargeWarningLevel, &batteryChargeWarningDelay, &maintenanceAlarms, &maintenanceAlarmOilTime, &maintenanceAlarmAirTime, &maintenanceAlarmFuelTime, &logSetup};
