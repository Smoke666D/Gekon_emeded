/*
 */
/*----------------------------------------------------------------------*/
#ifndef INC_OUTPUT_DATA_H_
#define INC_OUTPUT_DATA_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "config.h"
#include "fix16.h"
/*------------------------ Define --------------------------------------*/
#define   MAX_UNITS_OUTPUT_LENGTH             4U
#define   MAX_BIT_MAP_OUTPUT_LENGTH           6U
#define   OUTPUT_DATA_REGISTER_NUMBER         29U
#define   FILDS_TO_WRITE_OUTPUT_NUMBER        3U
#define   MAX_VALUE_OUTPUT_LENGTH             1U
#define   OUTPUT_DATA_MAX_SIZE                29U     // bytes
#define   OUTPUT_DATA_TOTAL_SIZE              102U   // bytes
#define   MIN_OUTPUT_DATA_SCALE               ( -2 )
#define   MAX_OUTPUT_DATA_SCALE               ( 0 )
#define   OUTPUT_DATA_SCALE_NUM               ( 3 )

/*----------------------- Structures -----------------------------------*/
/*---------------------- Register addresses ----------------------------*/
#define   OIL_PRESSURE_ADR                                       0U
#define   COOLANT_TEMP_ADR                                       1U
#define   FUEL_LEVEL_ADR                                         2U
#define   SPEED_LEVEL_ADR                                        3U
#define   MAINS_PHASE_VOLTAGE_L1_ADR                             4U
#define   MAINS_PHASE_VOLTAGE_L2_ADR                             5U
#define   MAINS_PHASE_VOLTAGE_L3_ADR                             6U
#define   MAINS_LINE_VOLTAGE_L1_ADR                              7U
#define   MAINS_LINE_VOLTAGE_L2_ADR                              8U
#define   MAINS_LINE_VOLTAGE_L3_ADR                              9U
#define   MAINS_FREQ_ADR                                         10U
#define   GENERATOR_PHASE_VOLTAGE_L1_ADR                         11U
#define   GENERATOR_PHASE_VOLTAGE_L2_ADR                         12U
#define   GENERATOR_PHASE_VOLTAGE_L3_ADR                         13U
#define   GENERATOR_LINE_VOLTAGE_L1_ADR                          14U
#define   GENERATOR_LINE_VOLTAGE_L2_ADR                          15U
#define   GENERATOR_LINE_VOLTAGE_L3_ADR                          16U
#define   GENERATOR_CURRENT_L1_ADR                               17U
#define   GENERATOR_CURRENT_L2_ADR                               18U
#define   GENERATOR_CURRENT_L3_ADR                               19U
#define   GENERATOR_FREQ_ADR                                     20U
#define   GENERATOR_COS_FI_ADR                                   21U
#define   GENERATOR_POWER_ACTIVE_ADR                             22U
#define   GENERATOR_POWER_REACTIVE_ADR                           23U
#define   GENERATOR_POWER_FULL_ADR                               24U
#define   BATTERY_VOLTAGE_ADR                                    25U
#define   CONTROLL_ADR                                           26U
#define   STATUS_ADR                                             27U
#define   LOG_ADR_ADR                                            28U
#define   LOG_RECORD_DATA0_ADR                                   29U
#define   LOG_RECORD_DATA1_ADR                                   30U
#define   LOG_RECORD_EVENT_ADR                                   31U
#define   ERROR_ADR_ADR                                          32U
#define   ERROR_RECORD_ADR                                       33U
/*---------------------- Bitmap addresses ------------------------------*/
#define   CONTROLL_SWITCH_MODE_ADR                     0U
#define   CONTROLL_REMOTE_START_ADR                    1U
#define   CONTROLL_BAN_AUTO_START_ADR                  2U
#define   CONTROLL_BAN_GENERATOR_LOAD_ADR              3U
#define   CONTROLL_BAN_AUTO_SHUTDOWN_ADR               4U
#define   CONTROLL_GO_TO_IDLING_ADR                    5U
#define   STATUS_ALARM_ADR                             0U
#define   STATUS_WARNING_ADR                           1U
#define   STATUS_MAINS_FAIL_ADR                        2U
#define   STATUS_D_P_SREADY_ADR                        3U
#define   STATUS_READY_TO_START_ADR                    4U
#define   STATUS_GENERATOR_REDY_ADR                    5U
/*------------------------- Extern -------------------------------------*/
extern eConfigReg oilPressure;
extern eConfigReg coolantTemp;
extern eConfigReg fuelLevel;
extern eConfigReg speedLevel;
extern eConfigReg mainsPhaseVoltageL1;
extern eConfigReg mainsPhaseVoltageL2;
extern eConfigReg mainsPhaseVoltageL3;
extern eConfigReg mainsLineVoltageL1;
extern eConfigReg mainsLineVoltageL2;
extern eConfigReg mainsLineVoltageL3;
extern eConfigReg mainsFreq;
extern eConfigReg generatorPhaseVoltageL1;
extern eConfigReg generatorPhaseVoltageL2;
extern eConfigReg generatorPhaseVoltageL3;
extern eConfigReg generatorLineVoltageL1;
extern eConfigReg generatorLineVoltageL2;
extern eConfigReg generatorLineVoltageL3;
extern eConfigReg generatorCurrentL1;
extern eConfigReg generatorCurrentL2;
extern eConfigReg generatorCurrentL3;
extern eConfigReg generatorFreq;
extern eConfigReg generatorCosFi;
extern eConfigReg generatorPowerActive;
extern eConfigReg generatorPowerReactive;
extern eConfigReg generatorPowerFull;
extern eConfigReg batteryVoltage;
extern eConfigReg controll;
extern eConfigReg status;
extern eConfigReg logAdr;
extern eConfigReg logRecordData0;
extern eConfigReg logRecordData1;
extern eConfigReg logRecordEvent;
extern eConfigReg errorAdr;
extern eConfigReg errorRecord;
extern eConfigReg* const outputDataReg[OUTPUT_DATA_REGISTER_NUMBER];
extern const char*       outputDataDictionary[OUTPUT_DATA_REGISTER_NUMBER];
extern const fix16_t     scaleMulArray[OUTPUT_DATA_SCALE_NUM];
/*----------------------------------------------------------------------*/
#endif /* INC_OUTPUT_DATA_H_ */
