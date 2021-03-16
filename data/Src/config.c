/*
 */
#include   "config.h"

const uint16_t versionControllerValue[1U] = { 1U };
const eConfigAttributes versionControllerAtrib =
{
   .adr        = 0U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
const eConfigReg versionController =
{
   .atrib = &versionControllerAtrib,
   .value = versionControllerValue,
};
/*----------------------------------------------------------------*/
const uint16_t versionFirmwareValue[1U] = { 1U };
const eConfigAttributes versionFirmwareAtrib =
{
   .adr        = 1U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
const eConfigReg versionFirmware =
{
   .atrib = &versionFirmwareAtrib,
   .value = versionFirmwareValue,
};
/*----------------------------------------------------------------*/
uint16_t serialNumberValue[6U] = { 0U, 0U, 0U, 0U, 0U, 0U };
const eConfigAttributes serialNumberAtrib =
{
   .adr        = 2U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 6U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg serialNumber =
{
   .atrib = &serialNumberAtrib,
   .value = serialNumberValue,
};
/*----------------------------------------------------------------*/
uint16_t displayBrightnesLevelValue[1U] = { 50U };
const eConfigAttributes displayBrightnesLevelAtrib =
{
   .adr        = 3U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 100U,
   .units      = { 0x0025U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg displayBrightnesLevel =
{
   .atrib = &displayBrightnesLevelAtrib,
   .value = displayBrightnesLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t displayContarstLevelValue[1U] = { 30U };
const eConfigAttributes displayContarstLevelAtrib =
{
   .adr        = 4U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 100U,
   .units      = { 0x0025U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg displayContarstLevel =
{
   .atrib = &displayContarstLevelAtrib,
   .value = displayContarstLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t displaySleepDelayValue[1U] = { 10U };
const eConfigAttributes displaySleepDelayAtrib =
{
   .adr        = 5U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 600U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg displaySleepDelay =
{
   .atrib = &displaySleepDelayAtrib,
   .value = displaySleepDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t hysteresisLevelValue[1U] = { 20U };
const eConfigAttributes hysteresisLevelAtrib =
{
   .adr        = 6U,
   .scale      = -1,
   .min        = 0U,
   .max        = 500U,
   .units      = { 0x0025U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg hysteresisLevel =
{
   .atrib = &hysteresisLevelAtrib,
   .value = hysteresisLevelValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap moduleSetupBitMap[1U] = 
{
   { 1U, 0U },     // passwordEnb
};
uint16_t moduleSetupValue[1U] = { 0U };
const eConfigAttributes moduleSetupAtrib =
{
   .adr        = 7U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 3U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 1U,
   .bitMap     = moduleSetupBitMap
};
eConfigReg moduleSetup =
{
   .atrib = &moduleSetupAtrib,
   .value = moduleSetupValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap oilPressureSetupBitMap[4U] = 
{
   { 15U, 0U },     // oilPressureSensorType
   { 16U, 4U },     // oilPressureOpenCircuitAlarmEnb
   { 32U, 5U },     // oilPressureAlarmEnb
   { 64U, 6U },     // oilPressurePreAlarmEnb
};
uint16_t oilPressureSetupValue[1U] = { 115U };
const eConfigAttributes oilPressureSetupAtrib =
{
   .adr        = 8U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 63U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = oilPressureSetupBitMap
};
eConfigReg oilPressureSetup =
{
   .atrib = &oilPressureSetupAtrib,
   .value = oilPressureSetupValue,
};
/*----------------------------------------------------------------*/
uint16_t oilPressureAlarmLevelValue[1U] = { 103U };
const eConfigAttributes oilPressureAlarmLevelAtrib =
{
   .adr        = 9U,
   .scale      = -2,
   .min        = 0U,
   .max        = 1000U,
   .units      = { 0xd091U, 0xd0b0U, 0xd180U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg oilPressureAlarmLevel =
{
   .atrib = &oilPressureAlarmLevelAtrib,
   .value = oilPressureAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t oilPressurePreAlarmLevelValue[1U] = { 124U };
const eConfigAttributes oilPressurePreAlarmLevelAtrib =
{
   .adr        = 10U,
   .scale      = -2,
   .min        = 0U,
   .max        = 1000U,
   .units      = { 0xd091U, 0xd0b0U, 0xd180U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg oilPressurePreAlarmLevel =
{
   .atrib = &oilPressurePreAlarmLevelAtrib,
   .value = oilPressurePreAlarmLevelValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap coolantTempSetupBitMap[7U] = 
{
   { 15U, 0U },     // coolantTempSensorType
   { 16U, 4U },     // coolantTempOpenCircuitAlarmEnb
   { 32U, 5U },     // coolantHightTempAlarmEnb
   { 64U, 6U },     // coolantHightTempElectroAlarmEnb
   { 128U, 7U },     // coolantHightTempPreAlarmEnb
   { 256U, 8U },     // coolantTempHeaterEnb
   { 512U, 9U },     // coolantTempCoolerEnb
};
uint16_t coolantTempSetupValue[1U] = { 179U };
const eConfigAttributes coolantTempSetupAtrib =
{
   .adr        = 11U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 63U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 7U,
   .bitMap     = coolantTempSetupBitMap
};
eConfigReg coolantTempSetup =
{
   .atrib = &coolantTempSetupAtrib,
   .value = coolantTempSetupValue,
};
/*----------------------------------------------------------------*/
uint16_t coolantHightTempAlarmLevelValue[1U] = { 96U };
const eConfigAttributes coolantHightTempAlarmLevelAtrib =
{
   .adr        = 12U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 100U,
   .units      = { 0x0043U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg coolantHightTempAlarmLevel =
{
   .atrib = &coolantHightTempAlarmLevelAtrib,
   .value = coolantHightTempAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t coolantHightTempElectroAlarmLevelValue[1U] = { 92U };
const eConfigAttributes coolantHightTempElectroAlarmLevelAtrib =
{
   .adr        = 13U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 100U,
   .units      = { 0x0043U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg coolantHightTempElectroAlarmLevel =
{
   .atrib = &coolantHightTempElectroAlarmLevelAtrib,
   .value = coolantHightTempElectroAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t coolantHightTempPreAlarmLevelValue[1U] = { 90U };
const eConfigAttributes coolantHightTempPreAlarmLevelAtrib =
{
   .adr        = 14U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 100U,
   .units      = { 0x0043U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg coolantHightTempPreAlarmLevel =
{
   .atrib = &coolantHightTempPreAlarmLevelAtrib,
   .value = coolantHightTempPreAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t coolantTempHeaterOffLevelValue[1U] = { 50U };
const eConfigAttributes coolantTempHeaterOffLevelAtrib =
{
   .adr        = 15U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 100U,
   .units      = { 0x0043U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg coolantTempHeaterOffLevel =
{
   .atrib = &coolantTempHeaterOffLevelAtrib,
   .value = coolantTempHeaterOffLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t coolantTempHeaterOnLevelValue[1U] = { 35U };
const eConfigAttributes coolantTempHeaterOnLevelAtrib =
{
   .adr        = 16U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 100U,
   .units      = { 0x0043U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg coolantTempHeaterOnLevel =
{
   .atrib = &coolantTempHeaterOnLevelAtrib,
   .value = coolantTempHeaterOnLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t coolantTempCoolerOffLevelValue[1U] = { 70U };
const eConfigAttributes coolantTempCoolerOffLevelAtrib =
{
   .adr        = 17U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 100U,
   .units      = { 0x0043U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg coolantTempCoolerOffLevel =
{
   .atrib = &coolantTempCoolerOffLevelAtrib,
   .value = coolantTempCoolerOffLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t coolantTempCoolerOnLevelValue[1U] = { 75U };
const eConfigAttributes coolantTempCoolerOnLevelAtrib =
{
   .adr        = 18U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 100U,
   .units      = { 0x0043U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg coolantTempCoolerOnLevel =
{
   .atrib = &coolantTempCoolerOnLevelAtrib,
   .value = coolantTempCoolerOnLevelValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap fuelLevelSetupBitMap[7U] = 
{
   { 7U, 0U },     // fuelLevelSensorType
   { 8U, 3U },     // fuelLevelOpenCircuitAlarmEnb
   { 16U, 4U },     // fuelLevelLowAlarmEnb
   { 32U, 5U },     // fuelLevelLowPreAlarmEnb
   { 64U, 6U },     // fuelLevelHightPreAlarmEnb
   { 128U, 7U },     // fuelLevelHightAlarmEnb
   { 256U, 8U },     // fuelPumpEnb
};
uint16_t fuelLevelSetupValue[1U] = { 155U };
const eConfigAttributes fuelLevelSetupAtrib =
{
   .adr        = 19U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 7U,
   .bitMap     = fuelLevelSetupBitMap
};
eConfigReg fuelLevelSetup =
{
   .atrib = &fuelLevelSetupAtrib,
   .value = fuelLevelSetupValue,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelLowAlarmLevelValue[1U] = { 10U };
const eConfigAttributes fuelLevelLowAlarmLevelAtrib =
{
   .adr        = 20U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 100U,
   .units      = { 0x0025U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg fuelLevelLowAlarmLevel =
{
   .atrib = &fuelLevelLowAlarmLevelAtrib,
   .value = fuelLevelLowAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelLowAlarmDelayValue[1U] = { 10U };
const eConfigAttributes fuelLevelLowAlarmDelayAtrib =
{
   .adr        = 21U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 360U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg fuelLevelLowAlarmDelay =
{
   .atrib = &fuelLevelLowAlarmDelayAtrib,
   .value = fuelLevelLowAlarmDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelLowPreAlarmLevelValue[1U] = { 25U };
const eConfigAttributes fuelLevelLowPreAlarmLevelAtrib =
{
   .adr        = 22U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 100U,
   .units      = { 0x0025U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg fuelLevelLowPreAlarmLevel =
{
   .atrib = &fuelLevelLowPreAlarmLevelAtrib,
   .value = fuelLevelLowPreAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelLowPreAlarmDelayValue[1U] = { 10U };
const eConfigAttributes fuelLevelLowPreAlarmDelayAtrib =
{
   .adr        = 23U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 360U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg fuelLevelLowPreAlarmDelay =
{
   .atrib = &fuelLevelLowPreAlarmDelayAtrib,
   .value = fuelLevelLowPreAlarmDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelHightPreAlarmLevelValue[1U] = { 90U };
const eConfigAttributes fuelLevelHightPreAlarmLevelAtrib =
{
   .adr        = 24U,
   .scale      = 0U,
   .min        = 3U,
   .max        = 100U,
   .units      = { 0x0025U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg fuelLevelHightPreAlarmLevel =
{
   .atrib = &fuelLevelHightPreAlarmLevelAtrib,
   .value = fuelLevelHightPreAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelHightPreAlarmDelayValue[1U] = { 0U };
const eConfigAttributes fuelLevelHightPreAlarmDelayAtrib =
{
   .adr        = 25U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 360U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg fuelLevelHightPreAlarmDelay =
{
   .atrib = &fuelLevelHightPreAlarmDelayAtrib,
   .value = fuelLevelHightPreAlarmDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelHightAlarmLevelValue[1U] = { 95U };
const eConfigAttributes fuelLevelHightAlarmLevelAtrib =
{
   .adr        = 26U,
   .scale      = 0U,
   .min        = 5U,
   .max        = 100U,
   .units      = { 0x0025U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg fuelLevelHightAlarmLevel =
{
   .atrib = &fuelLevelHightAlarmLevelAtrib,
   .value = fuelLevelHightAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelHightAlarmDelayValue[1U] = { 0U };
const eConfigAttributes fuelLevelHightAlarmDelayAtrib =
{
   .adr        = 27U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 360U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg fuelLevelHightAlarmDelay =
{
   .atrib = &fuelLevelHightAlarmDelayAtrib,
   .value = fuelLevelHightAlarmDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t fuelPumpOnLevelValue[1U] = { 30U };
const eConfigAttributes fuelPumpOnLevelAtrib =
{
   .adr        = 28U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 100U,
   .units      = { 0x0025U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg fuelPumpOnLevel =
{
   .atrib = &fuelPumpOnLevelAtrib,
   .value = fuelPumpOnLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t fuelPumpOffLevelValue[1U] = { 85U };
const eConfigAttributes fuelPumpOffLevelAtrib =
{
   .adr        = 29U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 100U,
   .units      = { 0x0025U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg fuelPumpOffLevel =
{
   .atrib = &fuelPumpOffLevelAtrib,
   .value = fuelPumpOffLevelValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap speedSetupBitMap[2U] = 
{
   { 1U, 0U },     // speedEnb
   { 2U, 1U },     // speedLowAlarmEnb
};
uint16_t speedSetupValue[1U] = { 0U };
const eConfigAttributes speedSetupAtrib =
{
   .adr        = 30U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 2U,
   .bitMap     = speedSetupBitMap
};
eConfigReg speedSetup =
{
   .atrib = &speedSetupAtrib,
   .value = speedSetupValue,
};
/*----------------------------------------------------------------*/
uint16_t speedToothNumberValue[1U] = { 190U };
const eConfigAttributes speedToothNumberAtrib =
{
   .adr        = 31U,
   .scale      = 0U,
   .min        = 10U,
   .max        = 500U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg speedToothNumber =
{
   .atrib = &speedToothNumberAtrib,
   .value = speedToothNumberValue,
};
/*----------------------------------------------------------------*/
uint16_t speedLowAlarmLevelValue[1U] = { 1200U };
const eConfigAttributes speedLowAlarmLevelAtrib =
{
   .adr        = 32U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 6000U,
   .units      = { 0x0052U, 0x0050U, 0x004dU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg speedLowAlarmLevel =
{
   .atrib = &speedLowAlarmLevelAtrib,
   .value = speedLowAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t speedHightAlarmLevelValue[1U] = { 1710U };
const eConfigAttributes speedHightAlarmLevelAtrib =
{
   .adr        = 33U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 6000U,
   .units      = { 0x0052U, 0x0050U, 0x004dU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg speedHightAlarmLevel =
{
   .atrib = &speedHightAlarmLevelAtrib,
   .value = speedHightAlarmLevelValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap diaSetupBitMap[4U] = 
{
   { 31U, 0U },     // diaFunction
   { 32U, 5U },     // diaPolarity
   { 192U, 6U },     // diaAction
   { 768U, 8U },     // diaArming
};
uint16_t diaSetupValue[1U] = { 0U };
const eConfigAttributes diaSetupAtrib =
{
   .adr        = 34U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = diaSetupBitMap
};
eConfigReg diaSetup =
{
   .atrib = &diaSetupAtrib,
   .value = diaSetupValue,
};
/*----------------------------------------------------------------*/
uint16_t diaDelayValue[1U] = { 0U };
const eConfigAttributes diaDelayAtrib =
{
   .adr        = 35U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 60U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg diaDelay =
{
   .atrib = &diaDelayAtrib,
   .value = diaDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t diaMessageValue[16U] = { 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U };
const eConfigAttributes diaMessageAtrib =
{
   .adr        = 36U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_CHAR,
   .len        = 16U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg diaMessage =
{
   .atrib = &diaMessageAtrib,
   .value = diaMessageValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap dibSetupBitMap[4U] = 
{
   { 31U, 0U },     // dibFunction
   { 32U, 5U },     // dibPolarity
   { 192U, 6U },     // dibAction
   { 768U, 8U },     // dibArming
};
uint16_t dibSetupValue[1U] = { 0U };
const eConfigAttributes dibSetupAtrib =
{
   .adr        = 37U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = dibSetupBitMap
};
eConfigReg dibSetup =
{
   .atrib = &dibSetupAtrib,
   .value = dibSetupValue,
};
/*----------------------------------------------------------------*/
uint16_t dibDelayValue[1U] = { 0U };
const eConfigAttributes dibDelayAtrib =
{
   .adr        = 38U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 60U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg dibDelay =
{
   .atrib = &dibDelayAtrib,
   .value = dibDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t dibMessageValue[16U] = { 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U };
const eConfigAttributes dibMessageAtrib =
{
   .adr        = 39U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_CHAR,
   .len        = 16U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg dibMessage =
{
   .atrib = &dibMessageAtrib,
   .value = dibMessageValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap dicSetupBitMap[4U] = 
{
   { 31U, 0U },     // dicFunction
   { 32U, 5U },     // dicPolarity
   { 192U, 6U },     // dicAction
   { 768U, 8U },     // dicArming
};
uint16_t dicSetupValue[1U] = { 0U };
const eConfigAttributes dicSetupAtrib =
{
   .adr        = 40U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = dicSetupBitMap
};
eConfigReg dicSetup =
{
   .atrib = &dicSetupAtrib,
   .value = dicSetupValue,
};
/*----------------------------------------------------------------*/
uint16_t dicDelayValue[1U] = { 0U };
const eConfigAttributes dicDelayAtrib =
{
   .adr        = 41U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 60U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg dicDelay =
{
   .atrib = &dicDelayAtrib,
   .value = dicDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t dicMessageValue[16U] = { 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U };
const eConfigAttributes dicMessageAtrib =
{
   .adr        = 42U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_CHAR,
   .len        = 16U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg dicMessage =
{
   .atrib = &dicMessageAtrib,
   .value = dicMessageValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap didSetupBitMap[4U] = 
{
   { 31U, 0U },     // didFunction
   { 32U, 5U },     // didPolarity
   { 192U, 6U },     // didAction
   { 768U, 8U },     // didArming
};
uint16_t didSetupValue[1U] = { 64U };
const eConfigAttributes didSetupAtrib =
{
   .adr        = 43U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = didSetupBitMap
};
eConfigReg didSetup =
{
   .atrib = &didSetupAtrib,
   .value = didSetupValue,
};
/*----------------------------------------------------------------*/
uint16_t didDelayValue[1U] = { 0U };
const eConfigAttributes didDelayAtrib =
{
   .adr        = 44U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 60U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg didDelay =
{
   .atrib = &didDelayAtrib,
   .value = didDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t didMessageValue[16U] = { 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U, 0x0020U };
const eConfigAttributes didMessageAtrib =
{
   .adr        = 45U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_CHAR,
   .len        = 16U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg didMessage =
{
   .atrib = &didMessageAtrib,
   .value = didMessageValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap doSetupBitMap[6U] = 
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
   .adr        = 46U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 6U,
   .bitMap     = doSetupBitMap
};
eConfigReg doSetup =
{
   .atrib = &doSetupAtrib,
   .value = doSetupValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap doabTypeBitMap[2U] = 
{
   { 255U, 0U },     // doaType
   { 65280U, 8U },     // dobType
};
uint16_t doabTypeValue[1U] = { 0U };
const eConfigAttributes doabTypeAtrib =
{
   .adr        = 47U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 2U,
   .bitMap     = doabTypeBitMap
};
eConfigReg doabType =
{
   .atrib = &doabTypeAtrib,
   .value = doabTypeValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap docdTypeBitMap[2U] = 
{
   { 255U, 0U },     // docType
   { 65280U, 8U },     // dodType
};
uint16_t docdTypeValue[1U] = { 0U };
const eConfigAttributes docdTypeAtrib =
{
   .adr        = 48U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 2U,
   .bitMap     = docdTypeBitMap
};
eConfigReg docdType =
{
   .atrib = &docdTypeAtrib,
   .value = docdTypeValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap doefTypeBitMap[2U] = 
{
   { 255U, 0U },     // doeType
   { 65280U, 8U },     // dofType
};
uint16_t doefTypeValue[1U] = { 0U };
const eConfigAttributes doefTypeAtrib =
{
   .adr        = 49U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 1023U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 2U,
   .bitMap     = doefTypeBitMap
};
eConfigReg doefType =
{
   .atrib = &doefTypeAtrib,
   .value = doefTypeValue,
};
/*----------------------------------------------------------------*/
uint16_t timerMainsTransientDelayValue[1U] = { 1U };
const eConfigAttributes timerMainsTransientDelayAtrib =
{
   .adr        = 50U,
   .scale      = -1,
   .min        = 1U,
   .max        = 100U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg timerMainsTransientDelay =
{
   .atrib = &timerMainsTransientDelayAtrib,
   .value = timerMainsTransientDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t timerStartDelayValue[1U] = { 5U };
const eConfigAttributes timerStartDelayAtrib =
{
   .adr        = 51U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 300U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg timerStartDelay =
{
   .atrib = &timerStartDelayAtrib,
   .value = timerStartDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t timerCrankingValue[1U] = { 10U };
const eConfigAttributes timerCrankingAtrib =
{
   .adr        = 52U,
   .scale      = 0U,
   .min        = 1U,
   .max        = 30U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg timerCranking =
{
   .atrib = &timerCrankingAtrib,
   .value = timerCrankingValue,
};
/*----------------------------------------------------------------*/
uint16_t timerCrankDelayValue[1U] = { 10U };
const eConfigAttributes timerCrankDelayAtrib =
{
   .adr        = 53U,
   .scale      = 0U,
   .min        = 1U,
   .max        = 30U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg timerCrankDelay =
{
   .atrib = &timerCrankDelayAtrib,
   .value = timerCrankDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t timerStartupIdleTimeValue[1U] = { 0U };
const eConfigAttributes timerStartupIdleTimeAtrib =
{
   .adr        = 54U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 300U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg timerStartupIdleTime =
{
   .atrib = &timerStartupIdleTimeAtrib,
   .value = timerStartupIdleTimeValue,
};
/*----------------------------------------------------------------*/
uint16_t timerNominalRPMDelayValue[1U] = { 0U };
const eConfigAttributes timerNominalRPMDelayAtrib =
{
   .adr        = 55U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 30U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg timerNominalRPMDelay =
{
   .atrib = &timerNominalRPMDelayAtrib,
   .value = timerNominalRPMDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t timerSafetyOnDelayValue[1U] = { 10U };
const eConfigAttributes timerSafetyOnDelayAtrib =
{
   .adr        = 56U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 60U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg timerSafetyOnDelay =
{
   .atrib = &timerSafetyOnDelayAtrib,
   .value = timerSafetyOnDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t timerWarmingValue[1U] = { 0U };
const eConfigAttributes timerWarmingAtrib =
{
   .adr        = 57U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 300U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg timerWarming =
{
   .atrib = &timerWarmingAtrib,
   .value = timerWarmingValue,
};
/*----------------------------------------------------------------*/
uint16_t timerTransferDelayValue[1U] = { 6U };
const eConfigAttributes timerTransferDelayAtrib =
{
   .adr        = 58U,
   .scale      = -1,
   .min        = 1U,
   .max        = 300U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg timerTransferDelay =
{
   .atrib = &timerTransferDelayAtrib,
   .value = timerTransferDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t timerGenBreakerTripPulseValue[1U] = { 5U };
const eConfigAttributes timerGenBreakerTripPulseAtrib =
{
   .adr        = 59U,
   .scale      = -1,
   .min        = 1U,
   .max        = 50U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg timerGenBreakerTripPulse =
{
   .atrib = &timerGenBreakerTripPulseAtrib,
   .value = timerGenBreakerTripPulseValue,
};
/*----------------------------------------------------------------*/
uint16_t timerGenBreakerClosePulseValue[1U] = { 5U };
const eConfigAttributes timerGenBreakerClosePulseAtrib =
{
   .adr        = 60U,
   .scale      = -1,
   .min        = 1U,
   .max        = 50U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg timerGenBreakerClosePulse =
{
   .atrib = &timerGenBreakerClosePulseAtrib,
   .value = timerGenBreakerClosePulseValue,
};
/*----------------------------------------------------------------*/
uint16_t timerMainsBreakerTripPulseValue[1U] = { 5U };
const eConfigAttributes timerMainsBreakerTripPulseAtrib =
{
   .adr        = 61U,
   .scale      = -1,
   .min        = 1U,
   .max        = 50U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg timerMainsBreakerTripPulse =
{
   .atrib = &timerMainsBreakerTripPulseAtrib,
   .value = timerMainsBreakerTripPulseValue,
};
/*----------------------------------------------------------------*/
uint16_t timerMainsBreakerClosePulseValue[1U] = { 5U };
const eConfigAttributes timerMainsBreakerClosePulseAtrib =
{
   .adr        = 62U,
   .scale      = -1,
   .min        = 1U,
   .max        = 50U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg timerMainsBreakerClosePulse =
{
   .atrib = &timerMainsBreakerClosePulseAtrib,
   .value = timerMainsBreakerClosePulseValue,
};
/*----------------------------------------------------------------*/
uint16_t timerReturnDelayValue[1U] = { 30U };
const eConfigAttributes timerReturnDelayAtrib =
{
   .adr        = 63U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 300U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg timerReturnDelay =
{
   .atrib = &timerReturnDelayAtrib,
   .value = timerReturnDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t timerCoolingValue[1U] = { 60U };
const eConfigAttributes timerCoolingAtrib =
{
   .adr        = 64U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 300U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg timerCooling =
{
   .atrib = &timerCoolingAtrib,
   .value = timerCoolingValue,
};
/*----------------------------------------------------------------*/
uint16_t timerCoolingIdleValue[1U] = { 0U };
const eConfigAttributes timerCoolingIdleAtrib =
{
   .adr        = 65U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 300U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg timerCoolingIdle =
{
   .atrib = &timerCoolingIdleAtrib,
   .value = timerCoolingIdleValue,
};
/*----------------------------------------------------------------*/
uint16_t timerSolenoidHoldValue[1U] = { 0U };
const eConfigAttributes timerSolenoidHoldAtrib =
{
   .adr        = 66U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 60U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg timerSolenoidHold =
{
   .atrib = &timerSolenoidHoldAtrib,
   .value = timerSolenoidHoldValue,
};
/*----------------------------------------------------------------*/
uint16_t timerFailStopDelayValue[1U] = { 20U };
const eConfigAttributes timerFailStopDelayAtrib =
{
   .adr        = 67U,
   .scale      = 0U,
   .min        = 1U,
   .max        = 60U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg timerFailStopDelay =
{
   .atrib = &timerFailStopDelayAtrib,
   .value = timerFailStopDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t timerGenTransientDelayValue[1U] = { 1U };
const eConfigAttributes timerGenTransientDelayAtrib =
{
   .adr        = 68U,
   .scale      = -1,
   .min        = 1U,
   .max        = 100U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg timerGenTransientDelay =
{
   .atrib = &timerGenTransientDelayAtrib,
   .value = timerGenTransientDelayValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap genSetupBitMap[3U] = 
{
   { 1U, 0U },     // genPowerGeneratorControlEnb
   { 6U, 1U },     // genAcSys
   { 8U, 3U },     // genPhaseSequenceControlEnb
};
uint16_t genSetupValue[1U] = { 9U };
const eConfigAttributes genSetupAtrib =
{
   .adr        = 69U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 255U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 3U,
   .bitMap     = genSetupBitMap
};
eConfigReg genSetup =
{
   .atrib = &genSetupAtrib,
   .value = genSetupValue,
};
/*----------------------------------------------------------------*/
uint16_t genRatedActivePowerLevelValue[1U] = { 400U };
const eConfigAttributes genRatedActivePowerLevelAtrib =
{
   .adr        = 70U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 1000U,
   .units      = { 0xd0baU, 0xd092U, 0xd182U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genRatedActivePowerLevel =
{
   .atrib = &genRatedActivePowerLevelAtrib,
   .value = genRatedActivePowerLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t genRatedReactivePowerLevelValue[1U] = { 300U };
const eConfigAttributes genRatedReactivePowerLevelAtrib =
{
   .adr        = 71U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 1000U,
   .units      = { 0xd0baU, 0xd092U, 0xd090U, 0xd0a0U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genRatedReactivePowerLevel =
{
   .atrib = &genRatedReactivePowerLevelAtrib,
   .value = genRatedReactivePowerLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t genRatedApparentPowerLevelValue[1U] = { 500U };
const eConfigAttributes genRatedApparentPowerLevelAtrib =
{
   .adr        = 72U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 1000U,
   .units      = { 0xd0baU, 0xd092U, 0xd090U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genRatedApparentPowerLevel =
{
   .atrib = &genRatedApparentPowerLevelAtrib,
   .value = genRatedApparentPowerLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t genRatedFrequencyLevelValue[1U] = { 500U };
const eConfigAttributes genRatedFrequencyLevelAtrib =
{
   .adr        = 73U,
   .scale      = -1,
   .min        = 2U,
   .max        = 747U,
   .units      = { 0xd093U, 0xd186U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genRatedFrequencyLevel =
{
   .atrib = &genRatedFrequencyLevelAtrib,
   .value = genRatedFrequencyLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t genRatedCurrentLevelValue[1U] = { 750U };
const eConfigAttributes genRatedCurrentLevelAtrib =
{
   .adr        = 74U,
   .scale      = 0U,
   .min        = 1U,
   .max        = 1500U,
   .units      = { 0xd090U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genRatedCurrentLevel =
{
   .atrib = &genRatedCurrentLevelAtrib,
   .value = genRatedCurrentLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t genCurrentTrasformRatioLevelValue[1U] = { 160U };
const eConfigAttributes genCurrentTrasformRatioLevelAtrib =
{
   .adr        = 75U,
   .scale      = 0U,
   .min        = 1U,
   .max        = 1000U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genCurrentTrasformRatioLevel =
{
   .atrib = &genCurrentTrasformRatioLevelAtrib,
   .value = genCurrentTrasformRatioLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t genPoleQuantityValue[1U] = { 2U };
const eConfigAttributes genPoleQuantityAtrib =
{
   .adr        = 76U,
   .scale      = 0U,
   .min        = 1U,
   .max        = 12U,
   .units      = { 0xd188U, 0xd182U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genPoleQuantity =
{
   .atrib = &genPoleQuantityAtrib,
   .value = genPoleQuantityValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap genAlarmsBitMap[10U] = 
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
   { 512U, 9U },     // genCurrentOverAction
};
uint16_t genAlarmsValue[1U] = { 511U };
const eConfigAttributes genAlarmsAtrib =
{
   .adr        = 77U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 65535U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 10U,
   .bitMap     = genAlarmsBitMap
};
eConfigReg genAlarms =
{
   .atrib = &genAlarmsAtrib,
   .value = genAlarmsValue,
};
/*----------------------------------------------------------------*/
uint16_t genUnderVoltageAlarmLevelValue[1U] = { 320U };
const eConfigAttributes genUnderVoltageAlarmLevelAtrib =
{
   .adr        = 78U,
   .scale      = 0U,
   .min        = 20U,
   .max        = 800U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genUnderVoltageAlarmLevel =
{
   .atrib = &genUnderVoltageAlarmLevelAtrib,
   .value = genUnderVoltageAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t genUnderVoltagePreAlarmLevelValue[1U] = { 360U };
const eConfigAttributes genUnderVoltagePreAlarmLevelAtrib =
{
   .adr        = 79U,
   .scale      = 0U,
   .min        = 20U,
   .max        = 800U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genUnderVoltagePreAlarmLevel =
{
   .atrib = &genUnderVoltagePreAlarmLevelAtrib,
   .value = genUnderVoltagePreAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t genOverVoltagePreAlarmLevelValue[1U] = { 440U };
const eConfigAttributes genOverVoltagePreAlarmLevelAtrib =
{
   .adr        = 80U,
   .scale      = 0U,
   .min        = 20U,
   .max        = 800U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genOverVoltagePreAlarmLevel =
{
   .atrib = &genOverVoltagePreAlarmLevelAtrib,
   .value = genOverVoltagePreAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t genOverVoltageAlarmLevelValue[1U] = { 480U };
const eConfigAttributes genOverVoltageAlarmLevelAtrib =
{
   .adr        = 81U,
   .scale      = 0U,
   .min        = 20U,
   .max        = 800U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genOverVoltageAlarmLevel =
{
   .atrib = &genOverVoltageAlarmLevelAtrib,
   .value = genOverVoltageAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t genUnderFrequencyAlarmLevelValue[1U] = { 450U };
const eConfigAttributes genUnderFrequencyAlarmLevelAtrib =
{
   .adr        = 82U,
   .scale      = -1,
   .min        = 200U,
   .max        = 800U,
   .units      = { 0xd093U, 0xd186U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genUnderFrequencyAlarmLevel =
{
   .atrib = &genUnderFrequencyAlarmLevelAtrib,
   .value = genUnderFrequencyAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t genUnderFrequencyPreAlarmLevelValue[1U] = { 470U };
const eConfigAttributes genUnderFrequencyPreAlarmLevelAtrib =
{
   .adr        = 83U,
   .scale      = -1,
   .min        = 200U,
   .max        = 800U,
   .units      = { 0xd093U, 0xd186U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genUnderFrequencyPreAlarmLevel =
{
   .atrib = &genUnderFrequencyPreAlarmLevelAtrib,
   .value = genUnderFrequencyPreAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t genOverFrequencyPreAlarmLevelValue[1U] = { 530U };
const eConfigAttributes genOverFrequencyPreAlarmLevelAtrib =
{
   .adr        = 84U,
   .scale      = -1,
   .min        = 200U,
   .max        = 800U,
   .units      = { 0xd093U, 0xd186U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genOverFrequencyPreAlarmLevel =
{
   .atrib = &genOverFrequencyPreAlarmLevelAtrib,
   .value = genOverFrequencyPreAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t genOverFrequencyAlarmLevelValue[1U] = { 550U };
const eConfigAttributes genOverFrequencyAlarmLevelAtrib =
{
   .adr        = 85U,
   .scale      = -1,
   .min        = 200U,
   .max        = 800U,
   .units      = { 0xd093U, 0xd186U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genOverFrequencyAlarmLevel =
{
   .atrib = &genOverFrequencyAlarmLevelAtrib,
   .value = genOverFrequencyAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t genOverCurrentThermalProtectionLevelValue[1U] = { 100U };
const eConfigAttributes genOverCurrentThermalProtectionLevelAtrib =
{
   .adr        = 86U,
   .scale      = 0U,
   .min        = 50U,
   .max        = 200U,
   .units      = { 0x0025U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genOverCurrentThermalProtectionLevel =
{
   .atrib = &genOverCurrentThermalProtectionLevelAtrib,
   .value = genOverCurrentThermalProtectionLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t genOverCurrentCutoffLevelValue[1U] = { 200U };
const eConfigAttributes genOverCurrentCutoffLevelAtrib =
{
   .adr        = 87U,
   .scale      = 0U,
   .min        = 100U,
   .max        = 300U,
   .units      = { 0x0025U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genOverCurrentCutoffLevel =
{
   .atrib = &genOverCurrentCutoffLevelAtrib,
   .value = genOverCurrentCutoffLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t genOverCurrentWarningLevelValue[1U] = { 100U };
const eConfigAttributes genOverCurrentWarningLevelAtrib =
{
   .adr        = 88U,
   .scale      = 0U,
   .min        = 50U,
   .max        = 120U,
   .units      = { 0x0025U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genOverCurrentWarningLevel =
{
   .atrib = &genOverCurrentWarningLevelAtrib,
   .value = genOverCurrentWarningLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t genOverCurrentWarningDelayValue[1U] = { 10U };
const eConfigAttributes genOverCurrentWarningDelayAtrib =
{
   .adr        = 89U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 360U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genOverCurrentWarningDelay =
{
   .atrib = &genOverCurrentWarningDelayAtrib,
   .value = genOverCurrentWarningDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t genCurrentOverloadProtectionLevelValue[1U] = { 100U };
const eConfigAttributes genCurrentOverloadProtectionLevelAtrib =
{
   .adr        = 90U,
   .scale      = 0U,
   .min        = 1U,
   .max        = 120U,
   .units      = { 0x0025U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genCurrentOverloadProtectionLevel =
{
   .atrib = &genCurrentOverloadProtectionLevelAtrib,
   .value = genCurrentOverloadProtectionLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t genCurrentOverloadProtectionDelayValue[1U] = { 5U };
const eConfigAttributes genCurrentOverloadProtectionDelayAtrib =
{
   .adr        = 91U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 360U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genCurrentOverloadProtectionDelay =
{
   .atrib = &genCurrentOverloadProtectionDelayAtrib,
   .value = genCurrentOverloadProtectionDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t genCurrentOverPhaseImbalanceLevelValue[1U] = { 20U };
const eConfigAttributes genCurrentOverPhaseImbalanceLevelAtrib =
{
   .adr        = 92U,
   .scale      = 0U,
   .min        = 1U,
   .max        = 100U,
   .units      = { 0x0025U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genCurrentOverPhaseImbalanceLevel =
{
   .atrib = &genCurrentOverPhaseImbalanceLevelAtrib,
   .value = genCurrentOverPhaseImbalanceLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t genCurrentOverPhaseImbalanceDelayValue[1U] = { 0U };
const eConfigAttributes genCurrentOverPhaseImbalanceDelayAtrib =
{
   .adr        = 93U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 360U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg genCurrentOverPhaseImbalanceDelay =
{
   .atrib = &genCurrentOverPhaseImbalanceDelayAtrib,
   .value = genCurrentOverPhaseImbalanceDelayValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap mainsSetupBitMap[2U] = 
{
   { 1U, 0U },     // mainsControlEnb
   { 2U, 1U },     // mainsPowerOffImmediatelyEnb
};
uint16_t mainsSetupValue[1U] = { 3U };
const eConfigAttributes mainsSetupAtrib =
{
   .adr        = 94U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 31U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 2U,
   .bitMap     = mainsSetupBitMap
};
eConfigReg mainsSetup =
{
   .atrib = &mainsSetupAtrib,
   .value = mainsSetupValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap mainsAlarmsBitMap[4U] = 
{
   { 1U, 0U },     // mainsUnderVoltageAlarmEnb
   { 2U, 1U },     // mainsOverVoltageAlarmEnb
   { 4U, 2U },     // mainsUnderFrequencyAlarmEnb
   { 8U, 3U },     // mainsOverFrequencyAlarmEnb
};
uint16_t mainsAlarmsValue[1U] = { 15U };
const eConfigAttributes mainsAlarmsAtrib =
{
   .adr        = 95U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 15U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = mainsAlarmsBitMap
};
eConfigReg mainsAlarms =
{
   .atrib = &mainsAlarmsAtrib,
   .value = mainsAlarmsValue,
};
/*----------------------------------------------------------------*/
uint16_t mainsUnderVoltageAlarmLevelValue[1U] = { 360U };
const eConfigAttributes mainsUnderVoltageAlarmLevelAtrib =
{
   .adr        = 96U,
   .scale      = 0U,
   .min        = 20U,
   .max        = 800U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg mainsUnderVoltageAlarmLevel =
{
   .atrib = &mainsUnderVoltageAlarmLevelAtrib,
   .value = mainsUnderVoltageAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t mainsOverVoltageAlarmLevelValue[1U] = { 440U };
const eConfigAttributes mainsOverVoltageAlarmLevelAtrib =
{
   .adr        = 97U,
   .scale      = 0U,
   .min        = 20U,
   .max        = 800U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg mainsOverVoltageAlarmLevel =
{
   .atrib = &mainsOverVoltageAlarmLevelAtrib,
   .value = mainsOverVoltageAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t mainsUnderFrequencyAlarmLevelValue[1U] = { 475U };
const eConfigAttributes mainsUnderFrequencyAlarmLevelAtrib =
{
   .adr        = 98U,
   .scale      = -1,
   .min        = 200U,
   .max        = 800U,
   .units      = { 0xd093U, 0xd186U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg mainsUnderFrequencyAlarmLevel =
{
   .atrib = &mainsUnderFrequencyAlarmLevelAtrib,
   .value = mainsUnderFrequencyAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t mainsOverFrequencyAlarmLevelValue[1U] = { 525U };
const eConfigAttributes mainsOverFrequencyAlarmLevelAtrib =
{
   .adr        = 99U,
   .scale      = -1,
   .min        = 200U,
   .max        = 800U,
   .units      = { 0xd093U, 0xd186U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg mainsOverFrequencyAlarmLevel =
{
   .atrib = &mainsOverFrequencyAlarmLevelAtrib,
   .value = mainsOverFrequencyAlarmLevelValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap engineSetupBitMap[1U] = 
{
   { 15U, 0U },     // engineStartAttempts
};
uint16_t engineSetupValue[1U] = { 3U };
const eConfigAttributes engineSetupAtrib =
{
   .adr        = 100U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 63U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 1U,
   .bitMap     = engineSetupBitMap
};
eConfigReg engineSetup =
{
   .atrib = &engineSetupAtrib,
   .value = engineSetupValue,
};
/*----------------------------------------------------------------*/
uint16_t enginePreHeatLevelValue[1U] = { 38U };
const eConfigAttributes enginePreHeatLevelAtrib =
{
   .adr        = 101U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 100U,
   .units      = { 0x0043U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg enginePreHeatLevel =
{
   .atrib = &enginePreHeatLevelAtrib,
   .value = enginePreHeatLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t enginePreHeatDelayValue[1U] = { 5U };
const eConfigAttributes enginePreHeatDelayAtrib =
{
   .adr        = 102U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 60U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg enginePreHeatDelay =
{
   .atrib = &enginePreHeatDelayAtrib,
   .value = enginePreHeatDelayValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap starterStopSetupBitMap[4U] = 
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
   .scale      = 0U,
   .min        = 0U,
   .max        = 7U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 4U,
   .bitMap     = starterStopSetupBitMap
};
eConfigReg starterStopSetup =
{
   .atrib = &starterStopSetupAtrib,
   .value = starterStopSetupValue,
};
/*----------------------------------------------------------------*/
uint16_t starterStopGenFreqLevelValue[1U] = { 210U };
const eConfigAttributes starterStopGenFreqLevelAtrib =
{
   .adr        = 104U,
   .scale      = -1,
   .min        = 150U,
   .max        = 400U,
   .units      = { 0xd093U, 0xd186U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg starterStopGenFreqLevel =
{
   .atrib = &starterStopGenFreqLevelAtrib,
   .value = starterStopGenFreqLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t starterStopOilPressureLevelValue[1U] = { 20U };
const eConfigAttributes starterStopOilPressureLevelAtrib =
{
   .adr        = 105U,
   .scale      = -1,
   .min        = 5U,
   .max        = 40U,
   .units      = { 0xd091U, 0xd0b0U, 0xd180U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg starterStopOilPressureLevel =
{
   .atrib = &starterStopOilPressureLevelAtrib,
   .value = starterStopOilPressureLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t starterStopChargeAlternatorLevelValue[1U] = { 60U };
const eConfigAttributes starterStopChargeAlternatorLevelAtrib =
{
   .adr        = 106U,
   .scale      = -1,
   .min        = 0U,
   .max        = 400U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg starterStopChargeAlternatorLevel =
{
   .atrib = &starterStopChargeAlternatorLevelAtrib,
   .value = starterStopChargeAlternatorLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t starterStopSpeedLevelValue[1U] = { 4000U };
const eConfigAttributes starterStopSpeedLevelAtrib =
{
   .adr        = 107U,
   .scale      = -1,
   .min        = 0U,
   .max        = 30000U,
   .units      = { 0x0052U, 0x0050U, 0x004dU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg starterStopSpeedLevel =
{
   .atrib = &starterStopSpeedLevelAtrib,
   .value = starterStopSpeedLevelValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap batteryAlarmsBitMap[3U] = 
{
   { 1U, 0U },     // batteryUnderVoltageEnb
   { 2U, 1U },     // batteryOverVoltageEnb
   { 4U, 2U },     // batteryChargeAlarmEnb
};
uint16_t batteryAlarmsValue[1U] = { 7U };
const eConfigAttributes batteryAlarmsAtrib =
{
   .adr        = 108U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 15U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 3U,
   .bitMap     = batteryAlarmsBitMap
};
eConfigReg batteryAlarms =
{
   .atrib = &batteryAlarmsAtrib,
   .value = batteryAlarmsValue,
};
/*----------------------------------------------------------------*/
uint16_t batteryUnderVoltageLevelValue[1U] = { 100U };
const eConfigAttributes batteryUnderVoltageLevelAtrib =
{
   .adr        = 109U,
   .scale      = -1,
   .min        = 0U,
   .max        = 397U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg batteryUnderVoltageLevel =
{
   .atrib = &batteryUnderVoltageLevelAtrib,
   .value = batteryUnderVoltageLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t batteryUnderVoltageDelayValue[1U] = { 60U };
const eConfigAttributes batteryUnderVoltageDelayAtrib =
{
   .adr        = 110U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 65535U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg batteryUnderVoltageDelay =
{
   .atrib = &batteryUnderVoltageDelayAtrib,
   .value = batteryUnderVoltageDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t batteryOverVoltageLevelValue[1U] = { 300U };
const eConfigAttributes batteryOverVoltageLevelAtrib =
{
   .adr        = 111U,
   .scale      = -1,
   .min        = 2U,
   .max        = 400U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg batteryOverVoltageLevel =
{
   .atrib = &batteryOverVoltageLevelAtrib,
   .value = batteryOverVoltageLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t batteryOverVoltageDelayValue[1U] = { 60U };
const eConfigAttributes batteryOverVoltageDelayAtrib =
{
   .adr        = 112U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 65535U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg batteryOverVoltageDelay =
{
   .atrib = &batteryOverVoltageDelayAtrib,
   .value = batteryOverVoltageDelayValue,
};
/*----------------------------------------------------------------*/
uint16_t batteryChargeAlarmLevelValue[1U] = { 40U };
const eConfigAttributes batteryChargeAlarmLevelAtrib =
{
   .adr        = 113U,
   .scale      = -1,
   .min        = 0U,
   .max        = 388U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg batteryChargeAlarmLevel =
{
   .atrib = &batteryChargeAlarmLevelAtrib,
   .value = batteryChargeAlarmLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t batteryChargeAlarmDelayValue[1U] = { 5U };
const eConfigAttributes batteryChargeAlarmDelayAtrib =
{
   .adr        = 114U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 3600U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg batteryChargeAlarmDelay =
{
   .atrib = &batteryChargeAlarmDelayAtrib,
   .value = batteryChargeAlarmDelayValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap maintenanceAlarmsBitMap[6U] = 
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
   .adr        = 115U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 63U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 6U,
   .bitMap     = maintenanceAlarmsBitMap
};
eConfigReg maintenanceAlarms =
{
   .atrib = &maintenanceAlarmsAtrib,
   .value = maintenanceAlarmsValue,
};
/*----------------------------------------------------------------*/
uint16_t maintenanceAlarmOilTimeValue[1U] = { 10U };
const eConfigAttributes maintenanceAlarmOilTimeAtrib =
{
   .adr        = 116U,
   .scale      = 0U,
   .min        = 10U,
   .max        = 5000U,
   .units      = { 0xd187U, 0xd0b0U, 0xd181U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg maintenanceAlarmOilTime =
{
   .atrib = &maintenanceAlarmOilTimeAtrib,
   .value = maintenanceAlarmOilTimeValue,
};
/*----------------------------------------------------------------*/
uint16_t maintenanceAlarmAirTimeValue[1U] = { 10U };
const eConfigAttributes maintenanceAlarmAirTimeAtrib =
{
   .adr        = 117U,
   .scale      = 0U,
   .min        = 10U,
   .max        = 5000U,
   .units      = { 0xd187U, 0xd0b0U, 0xd181U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg maintenanceAlarmAirTime =
{
   .atrib = &maintenanceAlarmAirTimeAtrib,
   .value = maintenanceAlarmAirTimeValue,
};
/*----------------------------------------------------------------*/
uint16_t maintenanceAlarmFuelTimeValue[1U] = { 10U };
const eConfigAttributes maintenanceAlarmFuelTimeAtrib =
{
   .adr        = 118U,
   .scale      = 0U,
   .min        = 10U,
   .max        = 5000U,
   .units      = { 0xd187U, 0xd0b0U, 0xd181U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg maintenanceAlarmFuelTime =
{
   .atrib = &maintenanceAlarmFuelTimeAtrib,
   .value = maintenanceAlarmFuelTimeValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap logSetupBitMap[2U] = 
{
   { 1U, 0U },     // logSaveWarningEventsEnb
   { 2U, 1U },     // logPositiveEventsEnb
};
uint16_t logSetupValue[1U] = { 3U };
const eConfigAttributes logSetupAtrib =
{
   .adr        = 119U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 65535U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 2U,
   .bitMap     = logSetupBitMap
};
eConfigReg logSetup =
{
   .atrib = &logSetupAtrib,
   .value = logSetupValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap recordSetupBitMap[14U] = 
{
   { 1U, 0U },     // recordEnb
   { 2U, 1U },     // recordOilPressureEnb
   { 4U, 2U },     // recordCoolantTempEnb
   { 8U, 3U },     // recordFuelLevelEnb
   { 16U, 4U },     // recordSpeedEnb
   { 32U, 5U },     // recordInputAEnb
   { 64U, 6U },     // recordInputBEnb
   { 128U, 7U },     // recordInputCEnb
   { 256U, 8U },     // recordInputDEnb
   { 512U, 9U },     // recordVoltageGenEnb
   { 1024U, 10U },     // recordFreqGenEnb
   { 2048U, 11U },     // recordVoltageNetEnb
   { 4096U, 12U },     // recordFreqNetEnb
   { 8192U, 13U },     // recordVoltageAccEnb
};
uint16_t recordSetupValue[1U] = { 1U };
const eConfigAttributes recordSetupAtrib =
{
   .adr        = 120U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 65535U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 14U,
   .bitMap     = recordSetupBitMap
};
eConfigReg recordSetup =
{
   .atrib = &recordSetupAtrib,
   .value = recordSetupValue,
};
/*----------------------------------------------------------------*/
uint16_t recordIntervalValue[1U] = { 1000U };
const eConfigAttributes recordIntervalAtrib =
{
   .adr        = 121U,
   .scale      = -1,
   .min        = 1U,
   .max        = 36000U,
   .units      = { 0xd181U, 0xd0b5U, 0xd0baU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg recordInterval =
{
   .atrib = &recordIntervalAtrib,
   .value = recordIntervalValue,
};
/*----------------------------------------------------------------*/

eConfigReg* const configReg[SETTING_REGISTER_NUMBER]  = { &versionController, &versionFirmware, &serialNumber, &displayBrightnesLevel, &displayContarstLevel, &displaySleepDelay, &hysteresisLevel, &moduleSetup, &oilPressureSetup, &oilPressureAlarmLevel, &oilPressurePreAlarmLevel, &coolantTempSetup, &coolantHightTempAlarmLevel, &coolantHightTempElectroAlarmLevel, &coolantHightTempPreAlarmLevel, &coolantTempHeaterOffLevel, &coolantTempHeaterOnLevel, &coolantTempCoolerOffLevel, &coolantTempCoolerOnLevel, &fuelLevelSetup, &fuelLevelLowAlarmLevel, &fuelLevelLowAlarmDelay, &fuelLevelLowPreAlarmLevel, &fuelLevelLowPreAlarmDelay, &fuelLevelHightPreAlarmLevel, &fuelLevelHightPreAlarmDelay, &fuelLevelHightAlarmLevel, &fuelLevelHightAlarmDelay, &fuelPumpOnLevel, &fuelPumpOffLevel, &speedSetup, &speedToothNumber, &speedLowAlarmLevel, &speedHightAlarmLevel, &diaSetup, &diaDelay, &diaMessage, &dibSetup, &dibDelay, &dibMessage, &dicSetup, &dicDelay, &dicMessage, &didSetup, &didDelay, &didMessage, &doSetup, &doabType, &docdType, &doefType, &timerMainsTransientDelay, &timerStartDelay, &timerCranking, &timerCrankDelay, &timerStartupIdleTime, &timerNominalRPMDelay, &timerSafetyOnDelay, &timerWarming, &timerTransferDelay, &timerGenBreakerTripPulse, &timerGenBreakerClosePulse, &timerMainsBreakerTripPulse, &timerMainsBreakerClosePulse, &timerReturnDelay, &timerCooling, &timerCoolingIdle, &timerSolenoidHold, &timerFailStopDelay, &timerGenTransientDelay, &genSetup, &genRatedActivePowerLevel, &genRatedReactivePowerLevel, &genRatedApparentPowerLevel, &genRatedFrequencyLevel, &genRatedCurrentLevel, &genCurrentTrasformRatioLevel, &genPoleQuantity, &genAlarms, &genUnderVoltageAlarmLevel, &genUnderVoltagePreAlarmLevel, &genOverVoltagePreAlarmLevel, &genOverVoltageAlarmLevel, &genUnderFrequencyAlarmLevel, &genUnderFrequencyPreAlarmLevel, &genOverFrequencyPreAlarmLevel, &genOverFrequencyAlarmLevel, &genOverCurrentThermalProtectionLevel, &genOverCurrentCutoffLevel, &genOverCurrentWarningLevel, &genOverCurrentWarningDelay, &genCurrentOverloadProtectionLevel, &genCurrentOverloadProtectionDelay, &genCurrentOverPhaseImbalanceLevel, &genCurrentOverPhaseImbalanceDelay, &mainsSetup, &mainsAlarms, &mainsUnderVoltageAlarmLevel, &mainsOverVoltageAlarmLevel, &mainsUnderFrequencyAlarmLevel, &mainsOverFrequencyAlarmLevel, &engineSetup, &enginePreHeatLevel, &enginePreHeatDelay, &starterStopSetup, &starterStopGenFreqLevel, &starterStopOilPressureLevel, &starterStopChargeAlternatorLevel, &starterStopSpeedLevel, &batteryAlarms, &batteryUnderVoltageLevel, &batteryUnderVoltageDelay, &batteryOverVoltageLevel, &batteryOverVoltageDelay, &batteryChargeAlarmLevel, &batteryChargeAlarmDelay, &maintenanceAlarms, &maintenanceAlarmOilTime, &maintenanceAlarmAirTime, &maintenanceAlarmFuelTime, &logSetup, &recordSetup, &recordInterval};
const char*       configDictionary[SETTING_REGISTER_NUMBER] = { "Версия контроллера", "Версия прошивки", "Серийный номер", "Яркость дисплея", "Контрастность дисплея", "Время переходя в спящий режим", "Величина гизтерезиса", "", "Настройки давления масла", "Аварийный уровень двления масла", "Предупредительный уровень давления масла", "Настройки температуры ОЖ", "Аварийный уровень температуры ОЖ", "Плановой остановки уровень температуры ОЖ", "Предупредительный уровень температуры ОЖ", "Уровень отключения нагревателя ОЖ", "Уровень включения нагревателя ОЖ", "Уровень отключения охладителя ОЖ", "Уровень включения нагревателя ОЖ", "Настройки уровня топлива", "Нижний аварийный уровень топлива", "Задержка нижнего аварийного сигнала уровня топлива", "Нижний предупредительный уровень топлива", "Задержка нижнего предупредительного сигнала уровня топлива", "Верхний предупредительный уровень топлива", "Задержка верхнего предупредительного сигнала уровня топлива", "Верхний аварийный уровень топлива", "Задержка аерхнего аварийного сигнала уровня топлива", "Уровень включения подкачки топлива", "Уровень выключения подкачки топлива", "Настройки оборотов двигателя", "Количество зубьев маховика", "Нижний аварийный уровень оборотов двигателя", "Верхний аварийный уровень оборотов двигателя", "Настройки цифрового входа А", "Задержка", "Сообщение", "Настройки цифрового входа B", "Задержка", "Сообщение", "Настройки цифрового входа C", "Задержка", "Сообщение", "Настройки цифрового входа D", "Задержка", "Сообщение", "Настройки цифровых выходов", "Функция цифровых выходов A и B", "Функция цифровых выходов C и D", "Функция цифровых выходов E и F", "Коммутационные скачки сети", "Задержка запуска", "Время пркрутки стартера", "Пауза между прокрутками стартера", "Время работы на холостых оборотах", "Задержка перехода на номинальные обороты", "Время блокировки контроля параметров при старте", "Время прогрева", "Пауза между переключениями", "Длина импульса отключения генераторного автомата", "Длина импульса включения генераторного автомата", "Длина импульса отключения сетевого автомата", "Длина импульса включения сетевого автомата", "Задержка останова", "Время охлаждения", "Время охлаждения на холостых оборотах", "Время работы стопового соленоида", "Задержка сигнала «Сбой останова»", "Коммутационные скачки параметров генератора", "Настройки генератора", "Номинальная активная мощность генератора", "Номинальная реактивная мощность генеартора", "Номинальная полная мощность генератора", "Номинальная частота генератора", "Номинальный ток генератора", "Коэффициент трансформатора тока", "Количество полюсов генератора", "Настройки событий генератора", "Нижний аварийный уровень напряжения генератора", "Нижний предупредтельный уровень напряжения генератора", "Верхний предупредительный уровень напряжения генератора", "Верхний аварийный уровень напряжения генератора", "Нижний аварийный уровень частоты генератора", "Нижний предупредительный уровень частоты генератора", "Верхний предупредительный уровень частоты генератора", "Верхний аварийный уровень частоты генератора", "Тепловая защита генератора", "Токовая отсечка генератора", "Верхний аварийный уровень тока генератора", "Задержка верхнего аваарийного сигнала тревоги тока генератора", "Уровень защиты от перегрузки генератора", "Задержка срабатывания защиты от перегрузки генератора", "Уровень допустимого перекоса фаз генератора", "Задержка срабатывания защиты перекоса фаз генератора", "Настройки сети", "Настройки событий сети", "Нижний аварийный уровень напряжения сети", "Верхний аварийный уровень напряжения сети", "Нижний аварийный уровень частоты сети", "Верхний аварийный уровень частоты сети", "Настройки двигателя", "Уровень включения свечей накаливания", "Длительность работы свечей накаливания", "Настройки стартера", "Отключить стартер по частоте генератора", "Давление масла отключения стартера", "Напряжение зарядного генератора отключения стартаера", "Обороты двигателя отключения стартера", "Настройки событий АКБ", "Нижний аварийный уровень напряжения АКБ", "Задержка нижнего аварийного сигнала напряжения АКБ", "Верхний аварийный уровень напряжения АКБ", "Задержка верхнего аварийного сигнала напряжения АКБ", "Уровень аварийного сигнала зарядного генератора", "Задержка аварийного сигнала зарядного генератора", "Настройки событий ТО", "Периодв срабатывания ТО масло", "Периодв срабатывания ТО воздух", "Периодв срабатывания ТО топливо", "Настройки журнала", "Настройки измерений", "Интервал времени между измерениями"};
const fix16_t     scaleMulArray[CONFIG_SCALE_NUM] = { 655, 6554, 65536 };
