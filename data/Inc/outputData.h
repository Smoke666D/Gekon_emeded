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
#define   MAX_BIT_MAP_OUTPUT_LENGTH           16U
#define   OUTPUT_DATA_REGISTER_NUMBER         43U
#define   FILDS_TO_WRITE_OUTPUT_NUMBER        3U
#define   MAX_VALUE_OUTPUT_LENGTH             1U
#define   OUTPUT_DATA_MAX_SIZE                59U     // bytes
#define   OUTPUT_DATA_TOTAL_SIZE              129U   // bytes
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
#define   CHARGER_VOLTAGE_ADR                                    26U
#define   EXTERNAL_DEVICES_ADR                                   27U
#define   DIGITAL_OUTPUT_ADR                                     28U
#define   DIGITAL_INPUT_ADR                                      29U
#define   DEVICE_STATUS_ADR                                      30U
#define   STATUS_ADR                                             31U
#define   LOG_LEN_ADR                                            32U
#define   LOG_ADR_ADR                                            33U
#define   LOG_RECORD_DATA0_ADR                                   34U
#define   LOG_RECORD_DATA1_ADR                                   35U
#define   LOG_RECORD_EVENT_ADR                                   36U
#define   ERROR0_ADR                                             37U
#define   ERROR1_ADR                                             38U
#define   ERROR2_ADR                                             39U
#define   WARNING0_ADR                                           40U
#define   WARNING1_ADR                                           41U
#define   CONTROLL_ADR                                           42U
/*---------------------- Bitmap addresses ------------------------------*/
#define   EXTERNAL_CHARGER_ADR                         0U
#define   EXTERNAL_COOLANT_HEATER_ADR                  1U
#define   EXTERNAL_COOLANT_COOLER_ADR                  2U
#define   EXTERNAL_FUEL_PUMP_ADR                       3U
#define   EXTERNAL_PRE_HEATER_ADR                      4U
#define   DIGITAL_OUTPUT_A_ADR                         0U
#define   DIGITAL_OUTPUT_B_ADR                         1U
#define   DIGITAL_OUTPUT_C_ADR                         2U
#define   DIGITAL_OUTPUT_D_ADR                         3U
#define   DIGITAL_OUTPUT_E_ADR                         4U
#define   DIGITAL_OUTPUT_F_ADR                         5U
#define   DIGITAL_OUTPUT_G_ADR                         6U
#define   DIGITAL_OUTPUT_H_ADR                         7U
#define   DIGITAL_OUTPUT_I_ADR                         8U
#define   DIGITAL_OUTPUT_J_ADR                         9U
#define   DIGITAL_OUTPUT_K_ADR                         10U
#define   DIGITAL_OUTPUT_L_ADR                         11U
#define   DIGITAL_INPUT_A_ADR                          0U
#define   DIGITAL_INPUT_B_ADR                          1U
#define   DIGITAL_INPUT_C_ADR                          2U
#define   DIGITAL_INPUT_D_ADR                          3U
#define   DIGITAL_INPUT_E_ADR                          4U
#define   DIGITAL_INPUT_F_ADR                          5U
#define   DIGITAL_INPUT_G_ADR                          6U
#define   DIGITAL_INPUT_H_ADR                          7U
#define   DIGITAL_INPUT_I_ADR                          8U
#define   DIGITAL_INPUT_J_ADR                          9U
#define   DIGITAL_INPUT_K_ADR                          10U
#define   DIGITAL_INPUT_L_ADR                          11U
#define   STATUS_ALARM_ADR                             0U
#define   STATUS_WARNING_ADR                           1U
#define   STATUS_MAINS_FAIL_ADR                        2U
#define   STATUS_D_P_SREADY_ADR                        3U
#define   STATUS_READY_TO_START_ADR                    4U
#define   STATUS_GENERATOR_REDY_ADR                    5U
#define   ERROR_EXTERN_EMEGENCY_STOP_ADR               0U
#define   ERROR_START_FAIL_ADR                         1U
#define   ERROR_STOP_FAIL_ADR                          2U
#define   ERROR_OIL_LOW_PRESSURE_ADR                   3U
#define   ERROR_OIL_SENSOR_ADR                         4U
#define   ERROR_TEMP_HIGHT_ADR                         5U
#define   ERROR_TEMP_SENSOR_ADR                        6U
#define   ERROR_FUEL_LOW_LEVEL_ADR                     7U
#define   ERROR_FUEL_HIGHT_LEVEL_ADR                   8U
#define   ERROR_FUEL_SENSOR_ADR                        9U
#define   ERROR_SPEED_HIGHT_ADR                        10U
#define   ERROR_SPEED_LOW_ADR                          11U
#define   ERROR_SPEED_SENSOR_ADR                       12U
#define   ERROR_GEN_LOW_VOLTAGE_ADR                    13U
#define   ERROR_GEN_HIGHT_VOLTAGE_ADR                  14U
#define   ERROR_GEN_LOW_FREQ_ADR                       15U
#define   ERROR_GEN_HIGHT_FREQ_ADR                     0U
#define   ERROR_GEN_PHASE_SEQ_ADR                      1U
#define   ERROR_PHASE_IMBALANCE_ADR                    2U
#define   ERROR_OVER_CURRENT_ADR                       3U
#define   ERROR_OVER_POWER_ADR                         4U
#define   ERROR_SHORT_CIRCUIT_ADR                      5U
#define   ERROR_MAINS_PHASE_SEQ_ADR                    6U
#define   ERROR_MAINTENANCE_OIL_ADR                    7U
#define   ERROR_MAINTENANCE_AIR_ADR                    8U
#define   ERROR_MAINTENANCE_FUEL_ADR                   9U
#define   ERROR_SENSOR_COMMON_ADR                      10U
#define   ERROR_ITERRUPTED_START_ADR                   11U
#define   ERROR_ITERRUPTED_STOP_ADR                    12U
#define   ERROR_USER_A_ADR                             13U
#define   ERROR_USER_B_ADR                             14U
#define   ERROR_USER_C_ADR                             15U
#define   ERROR_USER_D_ADR                             0U
#define   WARNING_MAINS_LOW_VOLTAGE_ADR                0U
#define   WARNING_MAINS_HIGHT_VOLTAGE_ADR              1U
#define   WARNING_MAINS_LOW_FREQ_ADR                   2U
#define   WARNING_MAINS_HIGHT_FREQ_ADR                 3U
#define   WARNING_GEN_LOW_VOLTAGE_ADR                  4U
#define   WARNING_GEN_HIGHT_VOLTAGE_ADR                5U
#define   WARNING_GEN_LOW_FREQ_ADR                     6U
#define   WARNING_GEN_HIGHT_FREQ_ADR                   7U
#define   WARNING_BATTERY_LOW_ADR                      8U
#define   WARNING_BATTERY_HIGHT_ADR                    9U
#define   WARNING_OVER_CURRENT_ADR                     10U
#define   WARNING_CHARGER_FAIL_ADR                     11U
#define   WARNING_FUEL_LOW_LEVEL_ADR                   12U
#define   WARNING_FUEL_HIGHT_LEVEL_ADR                 13U
#define   WARNING_TEMP_HIGHT_ADR                       14U
#define   WARNING_OIL_LOW_PRESSURE_ADR                 15U
#define   WARNING_MAINTENANCE_OIL_ADR                  0U
#define   WARNING_MAINTENANCE_AIR_ADR                  1U
#define   WARNING_MAINTENANCE_FUEL_ADR                 2U
#define   CONTROLL_SWITCH_MODE_ADR                     0U
#define   CONTROLL_START_ADR                           1U
#define   CONTROLL_STOP_ADR                            2U
#define   CONTROLL_RESET_ERRORS_ADR                    3U
#define   CONTROLL_SWITCH_LOAD_ADR                     4U
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
extern eConfigReg chargerVoltage;
extern eConfigReg externalDevices;
extern eConfigReg digitalOutput;
extern eConfigReg digitalInput;
extern eConfigReg deviceStatus;
extern eConfigReg status;
extern eConfigReg logLen;
extern eConfigReg logAdr;
extern eConfigReg logRecordData0;
extern eConfigReg logRecordData1;
extern eConfigReg logRecordEvent;
extern eConfigReg error0;
extern eConfigReg error1;
extern eConfigReg error2;
extern eConfigReg warning0;
extern eConfigReg warning1;
extern eConfigReg controll;
extern eConfigReg* const outputDataReg[OUTPUT_DATA_REGISTER_NUMBER];
extern const char*       outputDataDictionary[OUTPUT_DATA_REGISTER_NUMBER];
extern const fix16_t     scaleMulArray[OUTPUT_DATA_SCALE_NUM];
/*----------------------------------------------------------------------*/
#endif /* INC_OUTPUT_DATA_H_ */
