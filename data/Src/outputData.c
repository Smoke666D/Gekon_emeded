/*
 */
#include   "outputData.h"

uint16_t oilPressureValue[1U] = { 0U };
const eConfigAttributes oilPressureAtrib =
{
   .adr        = 0U,
   .scale      = -2,
   .min        = 0U,
   .max        = 1000U,
   .units      = { 0xd091U, 0xd0b0U, 0xd180U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg oilPressure =
{
   .atrib = &oilPressureAtrib,
   .value = oilPressureValue,
};
/*----------------------------------------------------------------*/
uint16_t coolantTempValue[1U] = { 0U };
const eConfigAttributes coolantTempAtrib =
{
   .adr        = 1U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 300U,
   .units      = { 0xd0a1U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg coolantTemp =
{
   .atrib = &coolantTempAtrib,
   .value = coolantTempValue,
};
/*----------------------------------------------------------------*/
uint16_t fuelLevelValue[1U] = { 0U };
const eConfigAttributes fuelLevelAtrib =
{
   .adr        = 2U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 100U,
   .units      = { 0x0025U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg fuelLevel =
{
   .atrib = &fuelLevelAtrib,
   .value = fuelLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t speedLevelValue[1U] = { 0U };
const eConfigAttributes speedLevelAtrib =
{
   .adr        = 3U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 6000U,
   .units      = { 0x0052U, 0x0050U, 0x004dU, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg speedLevel =
{
   .atrib = &speedLevelAtrib,
   .value = speedLevelValue,
};
/*----------------------------------------------------------------*/
uint16_t mainsPhaseVoltageL1Value[1U] = { 0U };
const eConfigAttributes mainsPhaseVoltageL1Atrib =
{
   .adr        = 4U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 800U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg mainsPhaseVoltageL1 =
{
   .atrib = &mainsPhaseVoltageL1Atrib,
   .value = mainsPhaseVoltageL1Value,
};
/*----------------------------------------------------------------*/
uint16_t mainsPhaseVoltageL2Value[1U] = { 0U };
const eConfigAttributes mainsPhaseVoltageL2Atrib =
{
   .adr        = 5U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 800U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg mainsPhaseVoltageL2 =
{
   .atrib = &mainsPhaseVoltageL2Atrib,
   .value = mainsPhaseVoltageL2Value,
};
/*----------------------------------------------------------------*/
uint16_t mainsPhaseVoltageL3Value[1U] = { 0U };
const eConfigAttributes mainsPhaseVoltageL3Atrib =
{
   .adr        = 6U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 800U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg mainsPhaseVoltageL3 =
{
   .atrib = &mainsPhaseVoltageL3Atrib,
   .value = mainsPhaseVoltageL3Value,
};
/*----------------------------------------------------------------*/
uint16_t mainsLineVoltageL1Value[1U] = { 0U };
const eConfigAttributes mainsLineVoltageL1Atrib =
{
   .adr        = 7U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 800U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg mainsLineVoltageL1 =
{
   .atrib = &mainsLineVoltageL1Atrib,
   .value = mainsLineVoltageL1Value,
};
/*----------------------------------------------------------------*/
uint16_t mainsLineVoltageL2Value[1U] = { 0U };
const eConfigAttributes mainsLineVoltageL2Atrib =
{
   .adr        = 8U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 800U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg mainsLineVoltageL2 =
{
   .atrib = &mainsLineVoltageL2Atrib,
   .value = mainsLineVoltageL2Value,
};
/*----------------------------------------------------------------*/
uint16_t mainsLineVoltageL3Value[1U] = { 0U };
const eConfigAttributes mainsLineVoltageL3Atrib =
{
   .adr        = 9U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 800U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg mainsLineVoltageL3 =
{
   .atrib = &mainsLineVoltageL3Atrib,
   .value = mainsLineVoltageL3Value,
};
/*----------------------------------------------------------------*/
uint16_t mainsFreqValue[1U] = { 0U };
const eConfigAttributes mainsFreqAtrib =
{
   .adr        = 10U,
   .scale      = -1,
   .min        = 0U,
   .max        = 750U,
   .units      = { 0xd093U, 0xd186U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg mainsFreq =
{
   .atrib = &mainsFreqAtrib,
   .value = mainsFreqValue,
};
/*----------------------------------------------------------------*/
uint16_t generatorPhaseVoltageL1Value[1U] = { 0U };
const eConfigAttributes generatorPhaseVoltageL1Atrib =
{
   .adr        = 11U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 800U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg generatorPhaseVoltageL1 =
{
   .atrib = &generatorPhaseVoltageL1Atrib,
   .value = generatorPhaseVoltageL1Value,
};
/*----------------------------------------------------------------*/
uint16_t generatorPhaseVoltageL2Value[1U] = { 0U };
const eConfigAttributes generatorPhaseVoltageL2Atrib =
{
   .adr        = 12U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 800U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg generatorPhaseVoltageL2 =
{
   .atrib = &generatorPhaseVoltageL2Atrib,
   .value = generatorPhaseVoltageL2Value,
};
/*----------------------------------------------------------------*/
uint16_t generatorPhaseVoltageL3Value[1U] = { 0U };
const eConfigAttributes generatorPhaseVoltageL3Atrib =
{
   .adr        = 13U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 800U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg generatorPhaseVoltageL3 =
{
   .atrib = &generatorPhaseVoltageL3Atrib,
   .value = generatorPhaseVoltageL3Value,
};
/*----------------------------------------------------------------*/
uint16_t generatorLineVoltageL1Value[1U] = { 0U };
const eConfigAttributes generatorLineVoltageL1Atrib =
{
   .adr        = 14U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 800U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg generatorLineVoltageL1 =
{
   .atrib = &generatorLineVoltageL1Atrib,
   .value = generatorLineVoltageL1Value,
};
/*----------------------------------------------------------------*/
uint16_t generatorLineVoltageL2Value[1U] = { 0U };
const eConfigAttributes generatorLineVoltageL2Atrib =
{
   .adr        = 15U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 800U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg generatorLineVoltageL2 =
{
   .atrib = &generatorLineVoltageL2Atrib,
   .value = generatorLineVoltageL2Value,
};
/*----------------------------------------------------------------*/
uint16_t generatorLineVoltageL3Value[1U] = { 0U };
const eConfigAttributes generatorLineVoltageL3Atrib =
{
   .adr        = 16U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 800U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg generatorLineVoltageL3 =
{
   .atrib = &generatorLineVoltageL3Atrib,
   .value = generatorLineVoltageL3Value,
};
/*----------------------------------------------------------------*/
uint16_t generatorCurrentL1Value[1U] = { 0U };
const eConfigAttributes generatorCurrentL1Atrib =
{
   .adr        = 17U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 1500U,
   .units      = { 0xd090U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg generatorCurrentL1 =
{
   .atrib = &generatorCurrentL1Atrib,
   .value = generatorCurrentL1Value,
};
/*----------------------------------------------------------------*/
uint16_t generatorCurrentL2Value[1U] = { 0U };
const eConfigAttributes generatorCurrentL2Atrib =
{
   .adr        = 18U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 1500U,
   .units      = { 0xd090U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg generatorCurrentL2 =
{
   .atrib = &generatorCurrentL2Atrib,
   .value = generatorCurrentL2Value,
};
/*----------------------------------------------------------------*/
uint16_t generatorCurrentL3Value[1U] = { 0U };
const eConfigAttributes generatorCurrentL3Atrib =
{
   .adr        = 19U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 1500U,
   .units      = { 0xd090U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg generatorCurrentL3 =
{
   .atrib = &generatorCurrentL3Atrib,
   .value = generatorCurrentL3Value,
};
/*----------------------------------------------------------------*/
uint16_t generatorFreqValue[1U] = { 0U };
const eConfigAttributes generatorFreqAtrib =
{
   .adr        = 20U,
   .scale      = -1,
   .min        = 0U,
   .max        = 750U,
   .units      = { 0xd093U, 0xd186U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg generatorFreq =
{
   .atrib = &generatorFreqAtrib,
   .value = generatorFreqValue,
};
/*----------------------------------------------------------------*/
uint16_t generatorCosFiValue[1U] = { 0U };
const eConfigAttributes generatorCosFiAtrib =
{
   .adr        = 21U,
   .scale      = -2,
   .min        = 0U,
   .max        = 100U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg generatorCosFi =
{
   .atrib = &generatorCosFiAtrib,
   .value = generatorCosFiValue,
};
/*----------------------------------------------------------------*/
uint16_t generatorPowerActiveValue[1U] = { 0U };
const eConfigAttributes generatorPowerActiveAtrib =
{
   .adr        = 22U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 1000U,
   .units      = { 0xd0baU, 0xd092U, 0xd182U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg generatorPowerActive =
{
   .atrib = &generatorPowerActiveAtrib,
   .value = generatorPowerActiveValue,
};
/*----------------------------------------------------------------*/
uint16_t generatorPowerReactiveValue[1U] = { 0U };
const eConfigAttributes generatorPowerReactiveAtrib =
{
   .adr        = 23U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 1000U,
   .units      = { 0xd0baU, 0xd092U, 0xd090U, 0xd0a0U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg generatorPowerReactive =
{
   .atrib = &generatorPowerReactiveAtrib,
   .value = generatorPowerReactiveValue,
};
/*----------------------------------------------------------------*/
uint16_t generatorPowerFullValue[1U] = { 0U };
const eConfigAttributes generatorPowerFullAtrib =
{
   .adr        = 24U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 1000U,
   .units      = { 0xd0baU, 0xd092U, 0xd090U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg generatorPowerFull =
{
   .atrib = &generatorPowerFullAtrib,
   .value = generatorPowerFullValue,
};
/*----------------------------------------------------------------*/
uint16_t batteryVoltageValue[1U] = { 0U };
const eConfigAttributes batteryVoltageAtrib =
{
   .adr        = 25U,
   .scale      = -1,
   .min        = 0U,
   .max        = 400U,
   .units      = { 0xd092U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg batteryVoltage =
{
   .atrib = &batteryVoltageAtrib,
   .value = batteryVoltageValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap digitalOutputBitMap[12U] = 
{
   { 1U, 0U },     // digitalOutputA
   { 2U, 1U },     // digitalOutputB
   { 4U, 2U },     // digitalOutputC
   { 8U, 3U },     // digitalOutputD
   { 16U, 4U },     // digitalOutputE
   { 32U, 5U },     // digitalOutputF
   { 64U, 6U },     // digitalOutputG
   { 128U, 7U },     // digitalOutputH
   { 256U, 8U },     // digitalOutputI
   { 512U, 9U },     // digitalOutputJ
   { 1024U, 10U },     // digitalOutputK
   { 2048U, 11U },     // digitalOutputL
};
uint16_t digitalOutputValue[1U] = { 0U };
const eConfigAttributes digitalOutputAtrib =
{
   .adr        = 26U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 12U,
   .bitMap     = digitalOutputBitMap
};
eConfigReg digitalOutput =
{
   .atrib = &digitalOutputAtrib,
   .value = digitalOutputValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap digitalInputBitMap[12U] = 
{
   { 1U, 0U },     // digitalInputA
   { 2U, 1U },     // digitalInputB
   { 4U, 2U },     // digitalInputC
   { 8U, 3U },     // digitalInputD
   { 16U, 4U },     // digitalInputE
   { 32U, 5U },     // digitalInputF
   { 64U, 6U },     // digitalInputG
   { 128U, 7U },     // digitalInputH
   { 256U, 8U },     // digitalInputI
   { 512U, 9U },     // digitalInputJ
   { 1024U, 10U },     // digitalInputK
   { 2048U, 11U },     // digitalInputL
};
uint16_t digitalInputValue[1U] = { 0U };
const eConfigAttributes digitalInputAtrib =
{
   .adr        = 27U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 12U,
   .bitMap     = digitalInputBitMap
};
eConfigReg digitalInput =
{
   .atrib = &digitalInputAtrib,
   .value = digitalInputValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap statusBitMap[6U] = 
{
   { 1U, 0U },     // statusAlarm
   { 2U, 1U },     // statusWarning
   { 4U, 2U },     // statusMainsFail
   { 8U, 3U },     // statusDPSready
   { 16U, 4U },     // statusReadyToStart
   { 32U, 5U },     // statusGeneratorRedy
};
uint16_t statusValue[1U] = { 0U };
const eConfigAttributes statusAtrib =
{
   .adr        = 28U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 6U,
   .bitMap     = statusBitMap
};
eConfigReg status =
{
   .atrib = &statusAtrib,
   .value = statusValue,
};
/*----------------------------------------------------------------*/
uint16_t logLenValue[1U] = { 0U };
const eConfigAttributes logLenAtrib =
{
   .adr        = 29U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg logLen =
{
   .atrib = &logLenAtrib,
   .value = logLenValue,
};
/*----------------------------------------------------------------*/
uint16_t errorLenValue[1U] = { 0U };
const eConfigAttributes errorLenAtrib =
{
   .adr        = 30U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg errorLen =
{
   .atrib = &errorLenAtrib,
   .value = errorLenValue,
};
/*----------------------------------------------------------------*/
uint16_t logRecordData0Value[1U] = { 0U };
const eConfigAttributes logRecordData0Atrib =
{
   .adr        = 31U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg logRecordData0 =
{
   .atrib = &logRecordData0Atrib,
   .value = logRecordData0Value,
};
/*----------------------------------------------------------------*/
uint16_t logRecordData1Value[1U] = { 0U };
const eConfigAttributes logRecordData1Atrib =
{
   .adr        = 32U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg logRecordData1 =
{
   .atrib = &logRecordData1Atrib,
   .value = logRecordData1Value,
};
/*----------------------------------------------------------------*/
uint16_t logRecordEventValue[1U] = { 0U };
const eConfigAttributes logRecordEventAtrib =
{
   .adr        = 33U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg logRecordEvent =
{
   .atrib = &logRecordEventAtrib,
   .value = logRecordEventValue,
};
/*----------------------------------------------------------------*/
uint16_t errorRecordData0Value[1U] = { 0U };
const eConfigAttributes errorRecordData0Atrib =
{
   .adr        = 34U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg errorRecordData0 =
{
   .atrib = &errorRecordData0Atrib,
   .value = errorRecordData0Value,
};
/*----------------------------------------------------------------*/
uint16_t errorRecordData1Value[1U] = { 0U };
const eConfigAttributes errorRecordData1Atrib =
{
   .adr        = 35U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg errorRecordData1 =
{
   .atrib = &errorRecordData1Atrib,
   .value = errorRecordData1Value,
};
/*----------------------------------------------------------------*/
uint16_t errorRecordEventValue[1U] = { 0U };
const eConfigAttributes errorRecordEventAtrib =
{
   .adr        = 36U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg errorRecordEvent =
{
   .atrib = &errorRecordEventAtrib,
   .value = errorRecordEventValue,
};
/*----------------------------------------------------------------*/
const eConfigBitMap controllBitMap[5U] = 
{
   { 1U, 0U },     // controllSwitchMode
   { 2U, 1U },     // controllStart
   { 4U, 2U },     // controllStop
   { 8U, 3U },     // controllResetErrors
   { 16U, 4U },     // controllSwitchLoad
};
uint16_t controllValue[1U] = { 0U };
const eConfigAttributes controllAtrib =
{
   .adr        = 37U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_BITMAP,
   .len        = 1U,
   .bitMapSize = 5U,
   .bitMap     = controllBitMap
};
eConfigReg controll =
{
   .atrib = &controllAtrib,
   .value = controllValue,
};
/*----------------------------------------------------------------*/
uint16_t logAdrValue[1U] = { 0U };
const eConfigAttributes logAdrAtrib =
{
   .adr        = 38U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg logAdr =
{
   .atrib = &logAdrAtrib,
   .value = logAdrValue,
};
/*----------------------------------------------------------------*/
uint16_t errorAdrValue[1U] = { 0U };
const eConfigAttributes errorAdrAtrib =
{
   .adr        = 39U,
   .scale      = 0U,
   .min        = 0U,
   .max        = 0U,
   .units      = { 0x0020U, 0x0020U, 0x0020U, 0x0020U },
   .type       = CONFIG_TYPE_UNSIGNED,
   .len        = 1U,
   .bitMapSize = 0U,
   .bitMap     = NULL,
};
eConfigReg errorAdr =
{
   .atrib = &errorAdrAtrib,
   .value = errorAdrValue,
};
/*----------------------------------------------------------------*/

eConfigReg* const outputDataReg[OUTPUT_DATA_REGISTER_NUMBER]  = { &oilPressure, &coolantTemp, &fuelLevel, &speedLevel, &mainsPhaseVoltageL1, &mainsPhaseVoltageL2, &mainsPhaseVoltageL3, &mainsLineVoltageL1, &mainsLineVoltageL2, &mainsLineVoltageL3, &mainsFreq, &generatorPhaseVoltageL1, &generatorPhaseVoltageL2, &generatorPhaseVoltageL3, &generatorLineVoltageL1, &generatorLineVoltageL2, &generatorLineVoltageL3, &generatorCurrentL1, &generatorCurrentL2, &generatorCurrentL3, &generatorFreq, &generatorCosFi, &generatorPowerActive, &generatorPowerReactive, &generatorPowerFull, &batteryVoltage, &digitalOutput, &digitalInput, &status, &logLen, &errorLen, &logRecordData0, &logRecordData1, &logRecordEvent, &errorRecordData0, &errorRecordData1, &errorRecordEvent, &controll, &logAdr, &errorAdr};
const char*       outputDataDictionary[OUTPUT_DATA_REGISTER_NUMBER] = { "Давление масла", "Температура ОЖ", "Уровень топлива", "Скорость двигателя", "Фазное напряжение сети L1", "Фазное напряжение сети L2", "Фазное напряжение сети L3", "Линейное напряжение сети L1", "Линейное напряжение сети L2", "Линейное напряжение сети L3", "Частота сети", "Фазное напряжение генератора L1", "Фазное напряжение генератора L2", "Фазное напряжение генератора L3", "Линейное напряжение генератора L1", "Линейное напряжение генератора L2", "Линейное напряжение генератора L3", "Ток генератора L1", "Ток генератора L2", "Ток генератора L3", "Частота генератора", "Cos F генератора", "Активная мощность генератора", "Реактивная мощность генератора", "Полная мощность генератора", "Напряжение АКБ", "Цифровые выходы", "Цифровые входы", "Регистр статуса", "Кол-во записей в журнале", "Кол-во ошибок", "Дата записи журнала 0", "Дата записи журнала 1", "Событие записи журнала", "Дата ошибки 0", "Дата ошибки 1", "Событие ошибки", "Регистр управления", "Адрес записи в журнале", "Адрес ошибки"};
