/*
 * Configuration file from 'config.csv'
 * Make time: 2020-05-27 10:17:27
 */
#include   "config.h"

uint16_t versionControllerValue[1U] = { 1U };
eConfigReg versionController =
{
   .page       = 0U,
   .adr        = 0U,
   .scale      = 0U,
   .value      = versionControllerValue,
   .min        = 0U,
   .max        = 0U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_ONLY,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t versionFirmwareValue[1U] = { 2U };
eConfigReg versionFirmware =
{
   .page       = 0U,
   .adr        = 1U,
   .scale      = 0U,
   .value      = versionFirmwareValue,
   .min        = 0U,
   .max        = 0U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_ONLY,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t serialNumberValue[6U] = { 0U, 0U, 0U, 0U, 0U, 0U };
eConfigReg serialNumber =
{
   .page       = 0U,
   .adr        = 2U,
   .scale      = 0U,
   .value      = serialNumberValue,
   .min        = 0U,
   .max        = 0U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_ONLY,
   .len        = 6U,
};
/*----------------------------------------------------------------*/
uint16_t displayBrightnesLevelValue[1U] = { 50U };
eConfigReg displayBrightnesLevel =
{
   .page       = 0U,
   .adr        = 3U,
   .scale      = 0U,
   .value      = displayBrightnesLevelValue,
   .min        = 0U,
   .max        = 100U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t displayContarstLevelValue[1U] = { 30U };
eConfigReg displayContarstLevel =
{
   .page       = 0U,
   .adr        = 4U,
   .scale      = 0U,
   .value      = displayContarstLevelValue,
   .min        = 0U,
   .max        = 100U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t displaySleepDelayValue[1U] = { 10U };
eConfigReg displaySleepDelay =
{
   .page       = 0U,
   .adr        = 5U,
   .scale      = 0U,
   .value      = displaySleepDelayValue,
   .min        = 2U,
   .max        = 600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
static eConfigBitMap moduleSetupBitMap[2U] = 
{
   { 1U, 0U, 0U, 1U },     // moduleType
   { 2U, 1U, 0U, 1U },     // alarmAllBlock
};
uint16_t moduleSetupValue[1U] = { 3U };
eConfigReg moduleSetup =
{
   .page       = 0U,
   .adr        = 6U,
   .scale      = 0U,
   .value      = moduleSetupValue,
   .min        = 0U,
   .max        = 3U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 2U,
   .bitMap     = moduleSetupBitMap
};
/*----------------------------------------------------------------*/
static eConfigBitMap oilPressureSetupBitMap[4U] = 
{
   { 15U, 0U, 0U, 11U },     // oilPressureSensorType
   { 16U, 4U, 0U, 1U },     // oilPressureOpenCircuitAlarmEnb
   { 32U, 5U, 0U, 1U },     // oilPressureAlarmEnb
   { 64U, 6U, 0U, 1U },     // oilPressurePreAlarmEnb
};
uint16_t oilPressureSetupValue[1U] = { 50U };
eConfigReg oilPressureSetup =
{
   .page       = 0U,
   .adr        = 7U,
   .scale      = 0U,
   .value      = oilPressureSetupValue,
   .min        = 0U,
   .max        = 63U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = oilPressureSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t oilPressureAlarmLevelValue[1U] = { 13U };
eConfigReg oilPressureAlarmLevel =
{
   .page       = 0U,
   .adr        = 8U,
   .scale      = -1,
   .value      = oilPressureAlarmLevelValue,
   .min        = 0U,
   .max        = 103U,
   .units      = {'Б', 'а', 'р', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t oilPressurePreAlarmLevelValue[1U] = { 5U };
eConfigReg oilPressurePreAlarmLevel =
{
   .page       = 0U,
   .adr        = 9U,
   .scale      = -1,
   .value      = oilPressurePreAlarmLevelValue,
   .min        = 0U,
   .max        = 103U,
   .units      = {'Б', 'а', 'р', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
static eConfigBitMap coolantTempSetupBitMap[6U] = 
{
   { 15U, 0U, 0U, 11U },     // coolantTempSensorType
   { 16U, 4U, 0U, 1U },     // coolantTempOpenCircuitAlarmEnb
   { 32U, 5U, 0U, 1U },     // coolantHightTempAlarmEnb
   { 64U, 6U, 0U, 1U },     // coolantHightTempPreAlarmEnb
   { 128U, 7U, 0U, 1U },     // coolantTempHeaterEnb
   { 256U, 8U, 0U, 1U },     // coolantTempCoolerEnb
};
uint16_t coolantTempSetupValue[1U] = { 51U };
eConfigReg coolantTempSetup =
{
   .page       = 0U,
   .adr        = 10U,
   .scale      = 0U,
   .value      = coolantTempSetupValue,
   .min        = 0U,
   .max        = 63U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 6U,
   .bitMap     = coolantTempSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t coolantHightTempAlarmLevelValue[1U] = { 96U };
eConfigReg coolantHightTempAlarmLevel =
{
   .page       = 0U,
   .adr        = 11U,
   .scale      = 0U,
   .value      = coolantHightTempAlarmLevelValue,
   .min        = 0U,
   .max        = 250U,
   .units      = {'C', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t coolantHightTempPreAlarmLevelValue[1U] = { 90U };
eConfigReg coolantHightTempPreAlarmLevel =
{
   .page       = 0U,
   .adr        = 12U,
   .scale      = 0U,
   .value      = coolantHightTempPreAlarmLevelValue,
   .min        = 0U,
   .max        = 250U,
   .units      = {'C', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t coolantTempHeaterOffLevelValue[1U] = { 60U };
eConfigReg coolantTempHeaterOffLevel =
{
   .page       = 0U,
   .adr        = 13U,
   .scale      = 0U,
   .value      = coolantTempHeaterOffLevelValue,
   .min        = 0U,
   .max        = 250U,
   .units      = {'C', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t coolantTempHeaterOnLevelValue[1U] = { 10U };
eConfigReg coolantTempHeaterOnLevel =
{
   .page       = 0U,
   .adr        = 14U,
   .scale      = 0U,
   .value      = coolantTempHeaterOnLevelValue,
   .min        = 0U,
   .max        = 250U,
   .units      = {'C', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t coolantTempCoolerOffLevelValue[1U] = { 120U };
eConfigReg coolantTempCoolerOffLevel =
{
   .page       = 0U,
   .adr        = 15U,
   .scale      = 0U,
   .value      = coolantTempCoolerOffLevelValue,
   .min        = 0U,
   .max        = 250U,
   .units      = {'C', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t coolantTempCoolerOnLevelValue[1U] = { 80U };
eConfigReg coolantTempCoolerOnLevel =
{
   .page       = 0U,
   .adr        = 16U,
   .scale      = 0U,
   .value      = coolantTempCoolerOnLevelValue,
   .min        = 0U,
   .max        = 250U,
   .units      = {'C', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
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
uint16_t fuelLevelSetupValue[1U] = { 665U };
eConfigReg fuelLevelSetup =
{
   .page       = 0U,
   .adr        = 17U,
   .scale      = 0U,
   .value      = fuelLevelSetupValue,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 8U,
   .bitMap     = fuelLevelSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelLowAlarmLevelValue[1U] = { 10U };
eConfigReg fuelLevelLowAlarmLevel =
{
   .page       = 0U,
   .adr        = 18U,
   .scale      = 0U,
   .value      = fuelLevelLowAlarmLevelValue,
   .min        = 0U,
   .max        = 95U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelLowAlarmDelayValue[1U] = { 100U };
eConfigReg fuelLevelLowAlarmDelay =
{
   .page       = 0U,
   .adr        = 19U,
   .scale      = 0U,
   .value      = fuelLevelLowAlarmDelayValue,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelLowPreAlarmLevelValue[1U] = { 25U };
eConfigReg fuelLevelLowPreAlarmLevel =
{
   .page       = 0U,
   .adr        = 20U,
   .scale      = 0U,
   .value      = fuelLevelLowPreAlarmLevelValue,
   .min        = 1U,
   .max        = 96U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelLowPreAlarmDelayValue[1U] = { 100U };
eConfigReg fuelLevelLowPreAlarmDelay =
{
   .page       = 0U,
   .adr        = 21U,
   .scale      = 0U,
   .value      = fuelLevelLowPreAlarmDelayValue,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelHightPreAlarmLevelValue[1U] = { 65U };
eConfigReg fuelLevelHightPreAlarmLevel =
{
   .page       = 0U,
   .adr        = 22U,
   .scale      = 0U,
   .value      = fuelLevelHightPreAlarmLevelValue,
   .min        = 3U,
   .max        = 98U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelHightPreAlarmDelayValue[1U] = { 0U };
eConfigReg fuelLevelHightPreAlarmDelay =
{
   .page       = 0U,
   .adr        = 23U,
   .scale      = 0U,
   .value      = fuelLevelHightPreAlarmDelayValue,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelHightAlarmLevelValue[1U] = { 90U };
eConfigReg fuelLevelHightAlarmLevel =
{
   .page       = 0U,
   .adr        = 24U,
   .scale      = 0U,
   .value      = fuelLevelHightAlarmLevelValue,
   .min        = 5U,
   .max        = 100U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelHightAlarmDelayValue[1U] = { 0U };
eConfigReg fuelLevelHightAlarmDelay =
{
   .page       = 0U,
   .adr        = 25U,
   .scale      = 0U,
   .value      = fuelLevelHightAlarmDelayValue,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t fuelPumpOnLevelValue[1U] = { 30U };
eConfigReg fuelPumpOnLevel =
{
   .page       = 0U,
   .adr        = 26U,
   .scale      = 0U,
   .value      = fuelPumpOnLevelValue,
   .min        = 0U,
   .max        = 100U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t fuelPumpOffLevelValue[1U] = { 70U };
eConfigReg fuelPumpOffLevel =
{
   .page       = 0U,
   .adr        = 27U,
   .scale      = 0U,
   .value      = fuelPumpOffLevelValue,
   .min        = 0U,
   .max        = 100U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
static eConfigBitMap diaSetupBitMap[4U] = 
{
   { 31U, 0U, 0U, 27U },     // diaFunction
   { 32U, 5U, 0U, 1U },     // diaPolarity
   { 192U, 6U, 0U, 3U },     // diaAction
   { 768U, 8U, 0U, 4U },     // diaArming
};
uint16_t diaSetupValue[1U] = { 14U };
eConfigReg diaSetup =
{
   .page       = 0U,
   .adr        = 28U,
   .scale      = 0U,
   .value      = diaSetupValue,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = diaSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t diaDelayValue[1U] = { 15U };
eConfigReg diaDelay =
{
   .page       = 0U,
   .adr        = 29U,
   .scale      = 0U,
   .value      = diaDelayValue,
   .min        = 0U,
   .max        = 60U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t diaMessageValue[16U] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
eConfigReg diaMessage =
{
   .page       = 0U,
   .adr        = 30U,
   .scale      = 0U,
   .value      = diaMessageValue,
   .min        = 0U,
   .max        = 0U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'S',
   .rw         = CONFIG_READ_WRITE,
   .len        = 16U,
};
/*----------------------------------------------------------------*/
static eConfigBitMap dibSetupBitMap[4U] = 
{
   { 31U, 0U, 0U, 27U },     // dibFunction
   { 32U, 5U, 0U, 1U },     // dibPolarity
   { 192U, 6U, 0U, 3U },     // dibAction
   { 768U, 8U, 0U, 4U },     // dibArming
};
uint16_t dibSetupValue[1U] = { 0U };
eConfigReg dibSetup =
{
   .page       = 0U,
   .adr        = 31U,
   .scale      = 0U,
   .value      = dibSetupValue,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = dibSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t dibDelayValue[1U] = { 15U };
eConfigReg dibDelay =
{
   .page       = 0U,
   .adr        = 32U,
   .scale      = 0U,
   .value      = dibDelayValue,
   .min        = 0U,
   .max        = 60U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t dibMessageValue[16U] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
eConfigReg dibMessage =
{
   .page       = 0U,
   .adr        = 33U,
   .scale      = 0U,
   .value      = dibMessageValue,
   .min        = 0U,
   .max        = 0U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'S',
   .rw         = CONFIG_READ_WRITE,
   .len        = 16U,
};
/*----------------------------------------------------------------*/
static eConfigBitMap dicSetupBitMap[4U] = 
{
   { 31U, 0U, 0U, 27U },     // dicFunction
   { 32U, 5U, 0U, 1U },     // dicPolarity
   { 192U, 6U, 0U, 3U },     // dicAction
   { 768U, 8U, 0U, 4U },     // dicArming
};
uint16_t dicSetupValue[1U] = { 0U };
eConfigReg dicSetup =
{
   .page       = 0U,
   .adr        = 34U,
   .scale      = 0U,
   .value      = dicSetupValue,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = dicSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t dicDelayValue[1U] = { 15U };
eConfigReg dicDelay =
{
   .page       = 0U,
   .adr        = 35U,
   .scale      = 0U,
   .value      = dicDelayValue,
   .min        = 0U,
   .max        = 60U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t dicMessageValue[16U] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
eConfigReg dicMessage =
{
   .page       = 0U,
   .adr        = 36U,
   .scale      = 0U,
   .value      = dicMessageValue,
   .min        = 0U,
   .max        = 0U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'S',
   .rw         = CONFIG_READ_WRITE,
   .len        = 16U,
};
/*----------------------------------------------------------------*/
static eConfigBitMap didSetupBitMap[4U] = 
{
   { 31U, 0U, 0U, 27U },     // didFunction
   { 32U, 5U, 0U, 1U },     // didPolarity
   { 192U, 6U, 0U, 3U },     // didAction
   { 768U, 8U, 0U, 4U },     // didArming
};
uint16_t didSetupValue[1U] = { 64U };
eConfigReg didSetup =
{
   .page       = 0U,
   .adr        = 37U,
   .scale      = 0U,
   .value      = didSetupValue,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = didSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t didDelayValue[1U] = { 15U };
eConfigReg didDelay =
{
   .page       = 0U,
   .adr        = 38U,
   .scale      = 0U,
   .value      = didDelayValue,
   .min        = 0U,
   .max        = 60U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t didMessageValue[16U] = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
eConfigReg didMessage =
{
   .page       = 0U,
   .adr        = 39U,
   .scale      = 0U,
   .value      = didMessageValue,
   .min        = 0U,
   .max        = 0U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'S',
   .rw         = CONFIG_READ_WRITE,
   .len        = 16U,
};
/*----------------------------------------------------------------*/
static eConfigBitMap doSetupBitMap[6U] = 
{
   { 1U, 0U, 0U, 1U },     // doaNOC
   { 2U, 1U, 0U, 1U },     // dobNOC
   { 4U, 2U, 0U, 1U },     // docNOC
   { 8U, 3U, 0U, 1U },     // dodNOC
   { 16U, 4U, 0U, 1U },     // doeNOC
   { 32U, 5U, 0U, 1U },     // dofNOC
};
uint16_t doSetupValue[1U] = { 0U };
eConfigReg doSetup =
{
   .page       = 0U,
   .adr        = 40U,
   .scale      = 0U,
   .value      = doSetupValue,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 6U,
   .bitMap     = doSetupBitMap
};
/*----------------------------------------------------------------*/
static eConfigBitMap doabTypeBitMap[2U] = 
{
   { 255U, 0U, 0U, 255U },     // doaType
   { 65280U, 8U, 0U, 255U },     // dobType
};
uint16_t doabTypeValue[1U] = { 0U };
eConfigReg doabType =
{
   .page       = 0U,
   .adr        = 41U,
   .scale      = 0U,
   .value      = doabTypeValue,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 2U,
   .bitMap     = doabTypeBitMap
};
/*----------------------------------------------------------------*/
static eConfigBitMap dodTypeBitMap[2U] = 
{
   { 255U, 0U, 0U, 255U },     // docType
   { 65280U, 8U, 0U, 255U },     // doType
};
uint16_t dodTypeValue[1U] = { 0U };
eConfigReg dodType =
{
   .page       = 0U,
   .adr        = 42U,
   .scale      = 0U,
   .value      = dodTypeValue,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 2U,
   .bitMap     = dodTypeBitMap
};
/*----------------------------------------------------------------*/
static eConfigBitMap doefTypeBitMap[2U] = 
{
   { 255U, 0U, 0U, 255U },     // doeType
   { 65280U, 8U, 0U, 255U },     // dofType
};
uint16_t doefTypeValue[1U] = { 0U };
eConfigReg doefType =
{
   .page       = 0U,
   .adr        = 43U,
   .scale      = 0U,
   .value      = doefTypeValue,
   .min        = 0U,
   .max        = 1023U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 2U,
   .bitMap     = doefTypeBitMap
};
/*----------------------------------------------------------------*/
uint16_t timerMainsTransientDelayValue[1U] = { 1U };
eConfigReg timerMainsTransientDelay =
{
   .page       = 0U,
   .adr        = 44U,
   .scale      = -1,
   .value      = timerMainsTransientDelayValue,
   .min        = 1U,
   .max        = 100U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t timerStartDelayValue[1U] = { 5U };
eConfigReg timerStartDelay =
{
   .page       = 0U,
   .adr        = 45U,
   .scale      = 0U,
   .value      = timerStartDelayValue,
   .min        = 0U,
   .max        = 300U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t timerPreheatingValue[1U] = { 5U };
eConfigReg timerPreheating =
{
   .page       = 0U,
   .adr        = 46U,
   .scale      = 0U,
   .value      = timerPreheatingValue,
   .min        = 0U,
   .max        = 300U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t timerCrankingValue[1U] = { 10U };
eConfigReg timerCranking =
{
   .page       = 0U,
   .adr        = 47U,
   .scale      = 0U,
   .value      = timerCrankingValue,
   .min        = 1U,
   .max        = 30U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t timerCrankDelayValue[1U] = { 10U };
eConfigReg timerCrankDelay =
{
   .page       = 0U,
   .adr        = 48U,
   .scale      = 0U,
   .value      = timerCrankDelayValue,
   .min        = 1U,
   .max        = 30U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t timerStartupIdleTimeValue[1U] = { 10U };
eConfigReg timerStartupIdleTime =
{
   .page       = 0U,
   .adr        = 49U,
   .scale      = 0U,
   .value      = timerStartupIdleTimeValue,
   .min        = 0U,
   .max        = 300U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t timerNominalRPMDelayValue[1U] = { 10U };
eConfigReg timerNominalRPMDelay =
{
   .page       = 0U,
   .adr        = 50U,
   .scale      = 0U,
   .value      = timerNominalRPMDelayValue,
   .min        = 1U,
   .max        = 30U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t timerSafetyOnDelayValue[1U] = { 10U };
eConfigReg timerSafetyOnDelay =
{
   .page       = 0U,
   .adr        = 51U,
   .scale      = 0U,
   .value      = timerSafetyOnDelayValue,
   .min        = 1U,
   .max        = 60U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t timerWarmingValue[1U] = { 1U };
eConfigReg timerWarming =
{
   .page       = 0U,
   .adr        = 52U,
   .scale      = 0U,
   .value      = timerWarmingValue,
   .min        = 0U,
   .max        = 300U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t timerTransferDelayValue[1U] = { 6U };
eConfigReg timerTransferDelay =
{
   .page       = 0U,
   .adr        = 53U,
   .scale      = -1,
   .value      = timerTransferDelayValue,
   .min        = 1U,
   .max        = 300U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t timerBreakerTripPulseValue[1U] = { 5U };
eConfigReg timerBreakerTripPulse =
{
   .page       = 0U,
   .adr        = 54U,
   .scale      = -1,
   .value      = timerBreakerTripPulseValue,
   .min        = 1U,
   .max        = 50U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t timerBreakerClosePulseValue[1U] = { 5U };
eConfigReg timerBreakerClosePulse =
{
   .page       = 0U,
   .adr        = 55U,
   .scale      = -1,
   .value      = timerBreakerClosePulseValue,
   .min        = 1U,
   .max        = 50U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t timerReturnDelayValue[1U] = { 30U };
eConfigReg timerReturnDelay =
{
   .page       = 0U,
   .adr        = 56U,
   .scale      = 0U,
   .value      = timerReturnDelayValue,
   .min        = 0U,
   .max        = 300U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t timerCoolingValue[1U] = { 60U };
eConfigReg timerCooling =
{
   .page       = 0U,
   .adr        = 57U,
   .scale      = 0U,
   .value      = timerCoolingValue,
   .min        = 0U,
   .max        = 300U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t timerCoolingIdleValue[1U] = { 20U };
eConfigReg timerCoolingIdle =
{
   .page       = 0U,
   .adr        = 58U,
   .scale      = 0U,
   .value      = timerCoolingIdleValue,
   .min        = 0U,
   .max        = 300U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t timerSolenoidHoldValue[1U] = { 20U };
eConfigReg timerSolenoidHold =
{
   .page       = 0U,
   .adr        = 59U,
   .scale      = 0U,
   .value      = timerSolenoidHoldValue,
   .min        = 0U,
   .max        = 60U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t timerFailStopDelayValue[1U] = { 30U };
eConfigReg timerFailStopDelay =
{
   .page       = 0U,
   .adr        = 60U,
   .scale      = 0U,
   .value      = timerFailStopDelayValue,
   .min        = 1U,
   .max        = 60U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t timerGenTransientDelayValue[1U] = { 10U };
eConfigReg timerGenTransientDelay =
{
   .page       = 0U,
   .adr        = 61U,
   .scale      = -1,
   .value      = timerGenTransientDelayValue,
   .min        = 1U,
   .max        = 100U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
static eConfigBitMap genSetupBitMap[4U] = 
{
   { 1U, 0U, 0U, 1U },     // genPowerGeneratorControlEnb
   { 30U, 1U, 0U, 16U },     // genPoles
   { 224U, 5U, 0U, 5U },     // genAcSys
   { 256U, 8U, 0U, 1U },     // genLocationCurrentTransformer
};
uint16_t genSetupValue[1U] = { 104U };
eConfigReg genSetup =
{
   .page       = 0U,
   .adr        = 62U,
   .scale      = 0U,
   .value      = genSetupValue,
   .min        = 0U,
   .max        = 255U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = genSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t genRatedActivePowerLevelValue[1U] = { 40000U };
eConfigReg genRatedActivePowerLevel =
{
   .page       = 0U,
   .adr        = 63U,
   .scale      = 0U,
   .value      = genRatedActivePowerLevelValue,
   .min        = 0U,
   .max        = 10000U,
   .units      = {'к', 'В', 'т', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genRatedReactivePowerLevelValue[1U] = { 0U };
eConfigReg genRatedReactivePowerLevel =
{
   .page       = 0U,
   .adr        = 64U,
   .scale      = 0U,
   .value      = genRatedReactivePowerLevelValue,
   .min        = 0U,
   .max        = 10000U,
   .units      = {'к', 'В', 'А', 'Р'},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genRatedApparentPowerLevelValue[1U] = { 200U };
eConfigReg genRatedApparentPowerLevel =
{
   .page       = 0U,
   .adr        = 65U,
   .scale      = 0U,
   .value      = genRatedApparentPowerLevelValue,
   .min        = 0U,
   .max        = 10000U,
   .units      = {'к', 'В', 'А', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genRatedFrequencyLevelValue[1U] = { 500U };
eConfigReg genRatedFrequencyLevel =
{
   .page       = 0U,
   .adr        = 66U,
   .scale      = -1,
   .value      = genRatedFrequencyLevelValue,
   .min        = 2U,
   .max        = 747U,
   .units      = {'Г', 'ц', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genCurrentPrimaryLevelValue[1U] = { 600U };
eConfigReg genCurrentPrimaryLevel =
{
   .page       = 0U,
   .adr        = 67U,
   .scale      = 0U,
   .value      = genCurrentPrimaryLevelValue,
   .min        = 5U,
   .max        = 8000U,
   .units      = {'А', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genCurrentFullLoadRatingLevelValue[1U] = { 500U };
eConfigReg genCurrentFullLoadRatingLevel =
{
   .page       = 0U,
   .adr        = 68U,
   .scale      = 0U,
   .value      = genCurrentFullLoadRatingLevelValue,
   .min        = 5U,
   .max        = 8000U,
   .units      = {'А', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
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
uint16_t genAlarmsValue[1U] = { 7167U };
eConfigReg genAlarms =
{
   .page       = 0U,
   .adr        = 69U,
   .scale      = 0U,
   .value      = genAlarmsValue,
   .min        = 0U,
   .max        = 65535U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 12U,
   .bitMap     = genAlarmsBitMap
};
/*----------------------------------------------------------------*/
uint16_t genUnderVoltageAlarmLevelValue[1U] = { 318U };
eConfigReg genUnderVoltageAlarmLevel =
{
   .page       = 0U,
   .adr        = 70U,
   .scale      = 0U,
   .value      = genUnderVoltageAlarmLevelValue,
   .min        = 86U,
   .max        = 708U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genUnderVoltagePreAlarmLevelValue[1U] = { 339U };
eConfigReg genUnderVoltagePreAlarmLevel =
{
   .page       = 0U,
   .adr        = 71U,
   .scale      = 0U,
   .value      = genUnderVoltagePreAlarmLevelValue,
   .min        = 88U,
   .max        = 710U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genOverVoltagePreAlarmLevelValue[1U] = { 439U };
eConfigReg genOverVoltagePreAlarmLevel =
{
   .page       = 0U,
   .adr        = 72U,
   .scale      = 0U,
   .value      = genOverVoltagePreAlarmLevelValue,
   .min        = 93U,
   .max        = 715U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genOverVoltageAlarmLevelValue[1U] = { 458U };
eConfigReg genOverVoltageAlarmLevel =
{
   .page       = 0U,
   .adr        = 73U,
   .scale      = 0U,
   .value      = genOverVoltageAlarmLevelValue,
   .min        = 95U,
   .max        = 717U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genUnderFrequencyAlarmLevelValue[1U] = { 400U };
eConfigReg genUnderFrequencyAlarmLevel =
{
   .page       = 0U,
   .adr        = 74U,
   .scale      = -1,
   .value      = genUnderFrequencyAlarmLevelValue,
   .min        = 0U,
   .max        = 744U,
   .units      = {'Г', 'ц', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genUnderFrequencyPreAlarmLevelValue[1U] = { 420U };
eConfigReg genUnderFrequencyPreAlarmLevel =
{
   .page       = 0U,
   .adr        = 75U,
   .scale      = -1,
   .value      = genUnderFrequencyPreAlarmLevelValue,
   .min        = 1U,
   .max        = 745U,
   .units      = {'Г', 'ц', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genOverFrequencyPreAlarmLevelValue[1U] = { 540U };
eConfigReg genOverFrequencyPreAlarmLevel =
{
   .page       = 0U,
   .adr        = 76U,
   .scale      = -1,
   .value      = genOverFrequencyPreAlarmLevelValue,
   .min        = 4U,
   .max        = 747U,
   .units      = {'Г', 'ц', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genOverFrequencyAlarmLevelValue[1U] = { 550U };
eConfigReg genOverFrequencyAlarmLevel =
{
   .page       = 0U,
   .adr        = 77U,
   .scale      = -1,
   .value      = genOverFrequencyAlarmLevelValue,
   .min        = 5U,
   .max        = 749U,
   .units      = {'Г', 'ц', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genOverCurrentThermalProtectionLevelValue[1U] = { 20U };
eConfigReg genOverCurrentThermalProtectionLevel =
{
   .page       = 0U,
   .adr        = 78U,
   .scale      = 0U,
   .value      = genOverCurrentThermalProtectionLevelValue,
   .min        = 0U,
   .max        = 120U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genOverCurrentCutoffLevelValue[1U] = { 40U };
eConfigReg genOverCurrentCutoffLevel =
{
   .page       = 0U,
   .adr        = 79U,
   .scale      = 0U,
   .value      = genOverCurrentCutoffLevelValue,
   .min        = 0U,
   .max        = 120U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genOverCurrentAlarmLevelValue[1U] = { 100U };
eConfigReg genOverCurrentAlarmLevel =
{
   .page       = 0U,
   .adr        = 80U,
   .scale      = 0U,
   .value      = genOverCurrentAlarmLevelValue,
   .min        = 50U,
   .max        = 120U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genOverCurrentAlarmDelayValue[1U] = { 60U };
eConfigReg genOverCurrentAlarmDelay =
{
   .page       = 0U,
   .adr        = 81U,
   .scale      = 0U,
   .value      = genOverCurrentAlarmDelayValue,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genCurrentOverloadProtectionLevelValue[1U] = { 100U };
eConfigReg genCurrentOverloadProtectionLevel =
{
   .page       = 0U,
   .adr        = 82U,
   .scale      = 0U,
   .value      = genCurrentOverloadProtectionLevelValue,
   .min        = 1U,
   .max        = 125U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genCurrentOverloadProtectionDelayValue[1U] = { 5U };
eConfigReg genCurrentOverloadProtectionDelay =
{
   .page       = 0U,
   .adr        = 83U,
   .scale      = 0U,
   .value      = genCurrentOverloadProtectionDelayValue,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genCurrentOverPhaseImbalanceLevelValue[1U] = { 100U };
eConfigReg genCurrentOverPhaseImbalanceLevel =
{
   .page       = 0U,
   .adr        = 84U,
   .scale      = 0U,
   .value      = genCurrentOverPhaseImbalanceLevelValue,
   .min        = 1U,
   .max        = 125U,
   .units      = {'%', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t genCurrentOverPhaseImbalanceDelayValue[1U] = { 60U };
eConfigReg genCurrentOverPhaseImbalanceDelay =
{
   .page       = 0U,
   .adr        = 85U,
   .scale      = 0U,
   .value      = genCurrentOverPhaseImbalanceDelayValue,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
static eConfigBitMap mainsSetupBitMap[3U] = 
{
   { 1U, 0U, 0U, 1U },     // mainsControlEnb
   { 2U, 1U, 0U, 1U },     // mainsPowerOffImmediatelyEnb
   { 28U, 2U, 0U, 5U },     // mainAcSys
};
uint16_t mainsSetupValue[1U] = { 13U };
eConfigReg mainsSetup =
{
   .page       = 0U,
   .adr        = 86U,
   .scale      = 0U,
   .value      = mainsSetupValue,
   .min        = 0U,
   .max        = 31U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 3U,
   .bitMap     = mainsSetupBitMap
};
/*----------------------------------------------------------------*/
static eConfigBitMap mainsAlarmsBitMap[4U] = 
{
   { 1U, 0U, 0U, 1U },     // mainsUnderVoltageAlarmEnb
   { 2U, 1U, 0U, 1U },     // mainsOverVoltageAlarmEnb
   { 4U, 2U, 0U, 1U },     // mainsUnderFrequencyAlarmEnb
   { 8U, 3U, 0U, 1U },     // mainsOverFrequencyAlarmEnb
};
uint16_t mainsAlarmsValue[1U] = { 15U };
eConfigReg mainsAlarms =
{
   .page       = 0U,
   .adr        = 87U,
   .scale      = 0U,
   .value      = mainsAlarmsValue,
   .min        = 0U,
   .max        = 15U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = mainsAlarmsBitMap
};
/*----------------------------------------------------------------*/
uint16_t mainsUnderVoltageAlarmLevelValue[1U] = { 318U };
eConfigReg mainsUnderVoltageAlarmLevel =
{
   .page       = 0U,
   .adr        = 88U,
   .scale      = 0U,
   .value      = mainsUnderVoltageAlarmLevelValue,
   .min        = 86U,
   .max        = 713U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t mainsOverVoltageAlarmLevelValue[1U] = { 438U };
eConfigReg mainsOverVoltageAlarmLevel =
{
   .page       = 0U,
   .adr        = 89U,
   .scale      = 0U,
   .value      = mainsOverVoltageAlarmLevelValue,
   .min        = 90U,
   .max        = 717U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t mainsUnderFrequencyAlarmLevelValue[1U] = { 450U };
eConfigReg mainsUnderFrequencyAlarmLevel =
{
   .page       = 0U,
   .adr        = 90U,
   .scale      = -1,
   .value      = mainsUnderFrequencyAlarmLevelValue,
   .min        = 0U,
   .max        = 747U,
   .units      = {'Г', 'ц', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t mainsOverFrequencyAlarmLevelValue[1U] = { 520U };
eConfigReg mainsOverFrequencyAlarmLevel =
{
   .page       = 0U,
   .adr        = 91U,
   .scale      = -1,
   .value      = mainsOverFrequencyAlarmLevelValue,
   .min        = 2U,
   .max        = 749U,
   .units      = {'Г', 'ц', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
static eConfigBitMap engineSetupBitMap[3U] = 
{
   { 15U, 0U, 1U, 9U },     // engineStartAttempts
   { 16U, 4U, 0U, 1U },     // enginePreHeatEnb
   { 32U, 5U, 0U, 1U },     // enginePostHeatEnb
};
uint16_t engineSetupValue[1U] = { 51U };
eConfigReg engineSetup =
{
   .page       = 0U,
   .adr        = 92U,
   .scale      = 0U,
   .value      = engineSetupValue,
   .min        = 0U,
   .max        = 63U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 3U,
   .bitMap     = engineSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t enginePreHeatLevelValue[1U] = { 50U };
eConfigReg enginePreHeatLevel =
{
   .page       = 0U,
   .adr        = 93U,
   .scale      = 0U,
   .value      = enginePreHeatLevelValue,
   .min        = 0U,
   .max        = 100U,
   .units      = {'C', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t enginePreHeatDelayValue[1U] = { 0U };
eConfigReg enginePreHeatDelay =
{
   .page       = 0U,
   .adr        = 94U,
   .scale      = 0U,
   .value      = enginePreHeatDelayValue,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t enginePostHeatLevelValue[1U] = { 50U };
eConfigReg enginePostHeatLevel =
{
   .page       = 0U,
   .adr        = 95U,
   .scale      = 0U,
   .value      = enginePostHeatLevelValue,
   .min        = 0U,
   .max        = 100U,
   .units      = {'C', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t enginePostHeatDelayValue[1U] = { 0U };
eConfigReg enginePostHeatDelay =
{
   .page       = 0U,
   .adr        = 96U,
   .scale      = 0U,
   .value      = enginePostHeatDelayValue,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
static eConfigBitMap crankSetupBitMap[4U] = 
{
   { 1U, 0U, 0U, 1U },     // crankDisconnectOilPressureEnb
   { 2U, 1U, 0U, 1U },     // crankOilPressureCheckOnStartEnb
   { 4U, 2U, 0U, 1U },     // crankDisconnectOilPressureEnb
   { 8U, 3U, 0U, 1U },     // crankDisconnectChargeAlternatorEnb
};
uint16_t crankSetupValue[1U] = { 10U };
eConfigReg crankSetup =
{
   .page       = 0U,
   .adr        = 97U,
   .scale      = 0U,
   .value      = crankSetupValue,
   .min        = 0U,
   .max        = 7U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = crankSetupBitMap
};
/*----------------------------------------------------------------*/
uint16_t crankDisconnectgenFreqLevelValue[1U] = { 210U };
eConfigReg crankDisconnectgenFreqLevel =
{
   .page       = 0U,
   .adr        = 98U,
   .scale      = -1,
   .value      = crankDisconnectgenFreqLevelValue,
   .min        = 0U,
   .max        = 400U,
   .units      = {'Г', 'ц', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t crankDisconnectOilPressureLevelValue[1U] = { 20U };
eConfigReg crankDisconnectOilPressureLevel =
{
   .page       = 0U,
   .adr        = 99U,
   .scale      = -1,
   .value      = crankDisconnectOilPressureLevelValue,
   .min        = 5U,
   .max        = 40U,
   .units      = {'Б', 'а', 'р', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t crankDisconnectChargeAlternatorLevelValue[1U] = { 60U };
eConfigReg crankDisconnectChargeAlternatorLevel =
{
   .page       = 0U,
   .adr        = 100U,
   .scale      = -1,
   .value      = crankDisconnectChargeAlternatorLevelValue,
   .min        = 0U,
   .max        = 400U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
static eConfigBitMap batteryAlarmsBitMap[4U] = 
{
   { 1U, 0U, 0U, 1U },     // batteryUnderVoltageEnb
   { 2U, 1U, 0U, 1U },     // batteryOverVoltageEnb
   { 4U, 2U, 0U, 1U },     // batteryChargeShutdownEnb
   { 8U, 3U, 0U, 1U },     // batteryChargeWarningEnb
};
uint16_t batteryAlarmsValue[1U] = { 15U };
eConfigReg batteryAlarms =
{
   .page       = 0U,
   .adr        = 101U,
   .scale      = 0U,
   .value      = batteryAlarmsValue,
   .min        = 0U,
   .max        = 15U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = batteryAlarmsBitMap
};
/*----------------------------------------------------------------*/
uint16_t batteryUnderVoltageLevelValue[1U] = { 100U };
eConfigReg batteryUnderVoltageLevel =
{
   .page       = 0U,
   .adr        = 102U,
   .scale      = -1,
   .value      = batteryUnderVoltageLevelValue,
   .min        = 0U,
   .max        = 397U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t batteryUnderVoltageDelayValue[1U] = { 60U };
eConfigReg batteryUnderVoltageDelay =
{
   .page       = 0U,
   .adr        = 103U,
   .scale      = 0U,
   .value      = batteryUnderVoltageDelayValue,
   .min        = 0U,
   .max        = 65535U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t batteryOverVoltageLevelValue[1U] = { 300U };
eConfigReg batteryOverVoltageLevel =
{
   .page       = 0U,
   .adr        = 104U,
   .scale      = -1,
   .value      = batteryOverVoltageLevelValue,
   .min        = 2U,
   .max        = 400U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t batteryOverVoltageDelayValue[1U] = { 60U };
eConfigReg batteryOverVoltageDelay =
{
   .page       = 0U,
   .adr        = 105U,
   .scale      = 0U,
   .value      = batteryOverVoltageDelayValue,
   .min        = 0U,
   .max        = 65535U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t batteryChargeShutdownLevelValue[1U] = { 40U };
eConfigReg batteryChargeShutdownLevel =
{
   .page       = 0U,
   .adr        = 106U,
   .scale      = -1,
   .value      = batteryChargeShutdownLevelValue,
   .min        = 0U,
   .max        = 388U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t batteryChargeShutdownDelayValue[1U] = { 5U };
eConfigReg batteryChargeShutdownDelay =
{
   .page       = 0U,
   .adr        = 107U,
   .scale      = 0U,
   .value      = batteryChargeShutdownDelayValue,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t batteryChargeWarningLevelValue[1U] = { 60U };
eConfigReg batteryChargeWarningLevel =
{
   .page       = 0U,
   .adr        = 108U,
   .scale      = -1,
   .value      = batteryChargeWarningLevelValue,
   .min        = 1U,
   .max        = 390U,
   .units      = {'В', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t batteryChargeWarningDelayValue[1U] = { 5U };
eConfigReg batteryChargeWarningDelay =
{
   .page       = 0U,
   .adr        = 109U,
   .scale      = 0U,
   .value      = batteryChargeWarningDelayValue,
   .min        = 0U,
   .max        = 3600U,
   .units      = {'с', 'е', 'к', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
static eConfigBitMap maintenanceAlarmsBitMap[6U] = 
{
   { 1U, 0U, 0U, 1U },     // maintenanceAlarmOilEnb
   { 2U, 1U, 0U, 1U },     // maintenanceAlarmOilAction
   { 4U, 2U, 0U, 1U },     // maintenanceAlarmAirEnb
   { 8U, 3U, 0U, 1U },     // maintenanceAlarmAirAction
   { 16U, 4U, 0U, 1U },     // maintenanceAlarmFuelEnb
   { 32U, 5U, 0U, 1U },     // maintenanceAlarmFuelAction
};
uint16_t maintenanceAlarmsValue[1U] = { 42U };
eConfigReg maintenanceAlarms =
{
   .page       = 0U,
   .adr        = 110U,
   .scale      = 0U,
   .value      = maintenanceAlarmsValue,
   .min        = 0U,
   .max        = 63U,
   .units      = {' ', ' ', ' ', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
   .bitMapSize = 6U,
   .bitMap     = maintenanceAlarmsBitMap
};
/*----------------------------------------------------------------*/
uint16_t maintenanceAlarmOilTimeValue[1U] = { 10U };
eConfigReg maintenanceAlarmOilTime =
{
   .page       = 0U,
   .adr        = 111U,
   .scale      = 0U,
   .value      = maintenanceAlarmOilTimeValue,
   .min        = 10U,
   .max        = 5000U,
   .units      = {'ч', 'а', 'с', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t maintenanceAlarmAirTimeValue[1U] = { 10U };
eConfigReg maintenanceAlarmAirTime =
{
   .page       = 0U,
   .adr        = 112U,
   .scale      = 0U,
   .value      = maintenanceAlarmAirTimeValue,
   .min        = 10U,
   .max        = 5000U,
   .units      = {'ч', 'а', 'с', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/
uint16_t maintenanceAlarmFuelTimeValue[1U] = { 10U };
eConfigReg maintenanceAlarmFuelTime =
{
   .page       = 0U,
   .adr        = 113U,
   .scale      = 0U,
   .value      = maintenanceAlarmFuelTimeValue,
   .min        = 10U,
   .max        = 5000U,
   .units      = {'ч', 'а', 'с', ' '},
   .type       = 'U',
   .rw         = CONFIG_READ_WRITE,
   .len        = 1U,
};
/*----------------------------------------------------------------*/

eConfigReg* configReg[SETTING_REGISTER_NUMBER] = { &versionController, &versionFirmware, &serialNumber, &displayBrightnesLevel, &displayContarstLevel, &displaySleepDelay, &moduleSetup, &oilPressureSetup, &oilPressureAlarmLevel, &oilPressurePreAlarmLevel, &coolantTempSetup, &coolantHightTempAlarmLevel, &coolantHightTempPreAlarmLevel, &coolantTempHeaterOffLevel, &coolantTempHeaterOnLevel, &coolantTempCoolerOffLevel, &coolantTempCoolerOnLevel, &fuelLevelSetup, &fuelLevelLowAlarmLevel, &fuelLevelLowAlarmDelay, &fuelLevelLowPreAlarmLevel, &fuelLevelLowPreAlarmDelay, &fuelLevelHightPreAlarmLevel, &fuelLevelHightPreAlarmDelay, &fuelLevelHightAlarmLevel, &fuelLevelHightAlarmDelay, &fuelPumpOnLevel, &fuelPumpOffLevel, &diaSetup, &diaDelay, &diaMessage, &dibSetup, &dibDelay, &dibMessage, &dicSetup, &dicDelay, &dicMessage, &didSetup, &didDelay, &didMessage, &doSetup, &doabType, &dodType, &doefType, &timerMainsTransientDelay, &timerStartDelay, &timerPreheating, &timerCranking, &timerCrankDelay, &timerStartupIdleTime, &timerNominalRPMDelay, &timerSafetyOnDelay, &timerWarming, &timerTransferDelay, &timerBreakerTripPulse, &timerBreakerClosePulse, &timerReturnDelay, &timerCooling, &timerCoolingIdle, &timerSolenoidHold, &timerFailStopDelay, &timerGenTransientDelay, &genSetup, &genRatedActivePowerLevel, &genRatedReactivePowerLevel, &genRatedApparentPowerLevel, &genRatedFrequencyLevel, &genCurrentPrimaryLevel, &genCurrentFullLoadRatingLevel, &genAlarms, &genUnderVoltageAlarmLevel, &genUnderVoltagePreAlarmLevel, &genOverVoltagePreAlarmLevel, &genOverVoltageAlarmLevel, &genUnderFrequencyAlarmLevel, &genUnderFrequencyPreAlarmLevel, &genOverFrequencyPreAlarmLevel, &genOverFrequencyAlarmLevel, &genOverCurrentThermalProtectionLevel, &genOverCurrentCutoffLevel, &genOverCurrentAlarmLevel, &genOverCurrentAlarmDelay, &genCurrentOverloadProtectionLevel, &genCurrentOverloadProtectionDelay, &genCurrentOverPhaseImbalanceLevel, &genCurrentOverPhaseImbalanceDelay, &mainsSetup, &mainsAlarms, &mainsUnderVoltageAlarmLevel, &mainsOverVoltageAlarmLevel, &mainsUnderFrequencyAlarmLevel, &mainsOverFrequencyAlarmLevel, &engineSetup, &enginePreHeatLevel, &enginePreHeatDelay, &enginePostHeatLevel, &enginePostHeatDelay, &crankSetup, &crankDisconnectgenFreqLevel, &crankDisconnectOilPressureLevel, &crankDisconnectChargeAlternatorLevel, &batteryAlarms, &batteryUnderVoltageLevel, &batteryUnderVoltageDelay, &batteryOverVoltageLevel, &batteryOverVoltageDelay, &batteryChargeShutdownLevel, &batteryChargeShutdownDelay, &batteryChargeWarningLevel, &batteryChargeWarningDelay, &maintenanceAlarms, &maintenanceAlarmOilTime, &maintenanceAlarmAirTime, &maintenanceAlarmFuelTime};
