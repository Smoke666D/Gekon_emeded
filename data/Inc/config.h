/*
 * Configuration file from 'config.csv'
 */
/*----------------------------------------------------------------------*/
#ifndef INC_CONFIG_H_
#define INC_CONFIG_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "fix16.h"
/*------------------------ Define --------------------------------------*/
#define   MAX_UNITS_LENGTH             5U
#define   MAX_BIT_MAP_LENGTH           10U
#define   SETTING_REGISTER_NUMBER      144U
#define   FILDS_TO_WRITE_NUMBER        3U
#define   BROADCAST_ADR                0xFFFFU
#define   MAX_VALUE_LENGTH             4U
#define   CONFIG_MAX_SIZE              49U     // bytes
#define   CONFIG_TOTAL_SIZE            536U   // bytes
#define   MIN_CONFIG_SCALE             ( -2 )
#define   MAX_CONFIG_SCALE             ( 0 )
#define   CONFIG_SCALE_NUM             ( 3 )

#define   CONFIG_REG_ADR_STR           "adr"
#define   CONFIG_REG_SCALE_STR         "scale"
#define   CONFIG_REG_VALUE_STR         "value"
#define   CONFIG_REG_MIN_STR           "min"
#define   CONFIG_REG_MAX_STR           "max"
#define   CONFIG_REG_UNITS_STR         "units"
#define   CONFIG_REG_TYPE_STR          "type"
#define   CONFIG_REG_RW_STATUS         "rw"
#define   CONFIG_REG_LEN_STR           "len"
#define   CONFIG_REG_BIT_MAP_SIZE_STR  "bitMapSize"
#define   CONFIG_REG_BIT_MAP_STR       "bit"

#define   BIT_MAP_MASK_STR             "mask"
#define   BIT_MAP_SHIFT_STR            "shift"
#define   BIT_MAP_MIN_STR              "min"
#define   BIT_MAP_MAX_STR              "max"
/*----------------------- Structures -----------------------------------*/
typedef enum
{
  CONFIG_NO    = 0x00U,
  CONFIG_VALUE = 0x01U,
  CONFIG_SCALE = 0x02U,
  CONFIG_UNITS = 0x03U,
} CONFIG_FILD;

typedef enum
{
  CONFIG_TYPE_UNSIGNED  = 'U',
  CONFIG_TYPE_SIGNED    = 'S',
  CONFIG_TYPE_CHAR      = 'C',
  CONFIG_TYPE_BITMAP    = 'B',
} CONFIG_TYPE;

typedef struct __packed
{
  uint16_t  mask;
  uint8_t   shift;
} eConfigBitMap;

typedef struct __packed
{
  uint16_t              adr;                     // R
  uint16_t              min;                     // R
  uint16_t              max;                     // R
  int8_t                scale;                   // R
  uint16_t              units[MAX_UNITS_LENGTH]; // R
  CONFIG_TYPE           type;                    // R
  uint8_t               len;                     // R
  uint8_t               bitMapSize;              // R
  eConfigBitMap const*  bitMap;                  // R
} eConfigAttributes;

typedef struct __packed
{
  eConfigAttributes const* atrib;  // R
  uint16_t*                value;  // RW
} eConfigReg;
/*---------------------- Register addresses ----------------------------*/
#define   VERSION_CONTROLLER_ADR                                 0U
#define   VERSION_FIRMWARE_ADR                                   1U
#define   VERSION_BOOTLOADER_ADR                                 2U
#define   DEVICE_I_D_ADR                                         3U
#define   SERIAL_NUMBER0_ADR                                     4U
#define   SERIAL_NUMBER1_ADR                                     5U
#define   DISPLAY_BRIGHTNES_LEVEL_ADR                            6U
#define   DISPLAY_CONTARST_LEVEL_ADR                             7U
#define   DISPLAY_SLEEP_DELAY_ADR                                8U
#define   MODBUS_SETUP_ADR                                       9U
#define   HYSTERESIS_LEVEL_ADR                                   10U
#define   MODULE_SETUP_ADR                                       11U
#define   OIL_PRESSURE_SETUP_ADR                                 12U
#define   OIL_PRESSURE_ALARM_LEVEL_ADR                           13U
#define   OIL_PRESSURE_PRE_ALARM_LEVEL_ADR                       14U
#define   COOLANT_TEMP_SETUP_ADR                                 15U
#define   COOLANT_HIGHT_TEMP_ALARM_LEVEL_ADR                     16U
#define   COOLANT_HIGHT_TEMP_ELECTRO_ALARM_LEVEL_ADR             17U
#define   COOLANT_HIGHT_TEMP_PRE_ALARM_LEVEL_ADR                 18U
#define   COOLANT_TEMP_HEATER_OFF_LEVEL_ADR                      19U
#define   COOLANT_TEMP_HEATER_ON_LEVEL_ADR                       20U
#define   COOLANT_TEMP_COOLER_OFF_LEVEL_ADR                      21U
#define   COOLANT_TEMP_COOLER_ON_LEVEL_ADR                       22U
#define   FUEL_LEVEL_SETUP_ADR                                   23U
#define   FUEL_TANK_LEVEL_ADR                                    24U
#define   FUEL_RATE_IDLE_LEVEL_ADR                               25U
#define   FUEL_RATE_LEVEL_ADR                                    26U
#define   FUEL_LEVEL_LOW_ALARM_LEVEL_ADR                         27U
#define   FUEL_LEVEL_LOW_ALARM_DELAY_ADR                         28U
#define   FUEL_LEVEL_LOW_PRE_ALARM_LEVEL_ADR                     29U
#define   FUEL_LEVEL_LOW_PRE_ALARM_DELAY_ADR                     30U
#define   FUEL_LEVEL_HIGHT_PRE_ALARM_LEVEL_ADR                   31U
#define   FUEL_LEVEL_HIGHT_PRE_ALARM_DELAY_ADR                   32U
#define   FUEL_LEVEL_HIGHT_ALARM_LEVEL_ADR                       33U
#define   FUEL_LEVEL_HIGHT_ALARM_DELAY_ADR                       34U
#define   FUEL_PUMP_ON_LEVEL_ADR                                 35U
#define   FUEL_PUMP_OFF_LEVEL_ADR                                36U
#define   SPEED_SETUP_ADR                                        37U
#define   SPEED_TOOTH_NUMBER_ADR                                 38U
#define   SPEED_LOW_ALARM_LEVEL_ADR                              39U
#define   SPEED_HIGHT_ALARM_LEVEL_ADR                            40U
#define   DIA_SETUP_ADR                                          41U
#define   DIA_DELAY_ADR                                          42U
#define   DIA_MESSAGE0_ADR                                       43U
#define   DIA_MESSAGE1_ADR                                       44U
#define   DIA_MESSAGE2_ADR                                       45U
#define   DIA_MESSAGE3_ADR                                       46U
#define   DIB_SETUP_ADR                                          47U
#define   DIB_DELAY_ADR                                          48U
#define   DIB_MESSAGE0_ADR                                       49U
#define   DIB_MESSAGE1_ADR                                       50U
#define   DIB_MESSAGE2_ADR                                       51U
#define   DIB_MESSAGE3_ADR                                       52U
#define   DIC_SETUP_ADR                                          53U
#define   DIC_DELAY_ADR                                          54U
#define   DIC_MESSAGE0_ADR                                       55U
#define   DIC_MESSAGE1_ADR                                       56U
#define   DIC_MESSAGE2_ADR                                       57U
#define   DIC_MESSAGE3_ADR                                       58U
#define   DID_SETUP_ADR                                          59U
#define   DID_DELAY_ADR                                          60U
#define   DID_MESSAGE0_ADR                                       61U
#define   DID_MESSAGE1_ADR                                       62U
#define   DID_MESSAGE2_ADR                                       63U
#define   DID_MESSAGE3_ADR                                       64U
#define   DO_SETUP_ADR                                           65U
#define   DOAB_TYPE_ADR                                          66U
#define   DOCD_TYPE_ADR                                          67U
#define   DOEF_TYPE_ADR                                          68U
#define   DOGH_TYPE_ADR                                          69U
#define   DOIJ_TYPE_ADR                                          70U
#define   TIMER_MAINS_TRANSIENT_DELAY_ADR                        71U
#define   TIMER_START_DELAY_ADR                                  72U
#define   TIMER_CRANKING_ADR                                     73U
#define   TIMER_CRANK_DELAY_ADR                                  74U
#define   TIMER_STARTUP_IDLE_TIME_ADR                            75U
#define   TIMER_NOMINAL_R_P_M_DELAY_ADR                          76U
#define   TIMER_SAFETY_ON_DELAY_ADR                              77U
#define   TIMER_WARMING_ADR                                      78U
#define   TIMER_TRANSFER_DELAY_ADR                               79U
#define   TIMER_GEN_BREAKER_TRIP_PULSE_ADR                       80U
#define   TIMER_GEN_BREAKER_CLOSE_PULSE_ADR                      81U
#define   TIMER_MAINS_BREAKER_TRIP_PULSE_ADR                     82U
#define   TIMER_MAINS_BREAKER_CLOSE_PULSE_ADR                    83U
#define   TIMER_RETURN_DELAY_ADR                                 84U
#define   TIMER_COOLING_ADR                                      85U
#define   TIMER_COOLING_IDLE_ADR                                 86U
#define   TIMER_SOLENOID_HOLD_ADR                                87U
#define   TIMER_FAIL_STOP_DELAY_ADR                              88U
#define   TIMER_GEN_TRANSIENT_DELAY_ADR                          89U
#define   GEN_SETUP_ADR                                          90U
#define   GEN_RATED_ACTIVE_POWER_LEVEL_ADR                       91U
#define   GEN_RATED_REACTIVE_POWER_LEVEL_ADR                     92U
#define   GEN_RATED_APPARENT_POWER_LEVEL_ADR                     93U
#define   GEN_RATED_FREQUENCY_LEVEL_ADR                          94U
#define   GEN_RATED_CURRENT_LEVEL_ADR                            95U
#define   GEN_CURRENT_TRASFORM_RATIO_LEVEL_ADR                   96U
#define   GEN_POLE_QUANTITY_ADR                                  97U
#define   GEN_ALARMS_ADR                                         98U
#define   GEN_UNDER_VOLTAGE_ALARM_LEVEL_ADR                      99U
#define   GEN_UNDER_VOLTAGE_PRE_ALARM_LEVEL_ADR                  100U
#define   GEN_OVER_VOLTAGE_PRE_ALARM_LEVEL_ADR                   101U
#define   GEN_OVER_VOLTAGE_ALARM_LEVEL_ADR                       102U
#define   GEN_UNDER_FREQUENCY_ALARM_LEVEL_ADR                    103U
#define   GEN_UNDER_FREQUENCY_PRE_ALARM_LEVEL_ADR                104U
#define   GEN_OVER_FREQUENCY_PRE_ALARM_LEVEL_ADR                 105U
#define   GEN_OVER_FREQUENCY_ALARM_LEVEL_ADR                     106U
#define   GEN_OVER_CURRENT_THERMAL_PROTECTION_LEVEL_ADR          107U
#define   GEN_OVER_CURRENT_CUTOFF_LEVEL_ADR                      108U
#define   GEN_OVER_CURRENT_WARNING_LEVEL_ADR                     109U
#define   GEN_OVER_CURRENT_WARNING_DELAY_ADR                     110U
#define   GEN_CURRENT_OVERLOAD_PROTECTION_LEVEL_ADR              111U
#define   GEN_CURRENT_OVERLOAD_PROTECTION_DELAY_ADR              112U
#define   GEN_CURRENT_OVER_PHASE_IMBALANCE_LEVEL_ADR             113U
#define   GEN_CURRENT_OVER_PHASE_IMBALANCE_DELAY_ADR             114U
#define   MAINS_SETUP_ADR                                        115U
#define   MAINS_ALARMS_ADR                                       116U
#define   MAINS_UNDER_VOLTAGE_ALARM_LEVEL_ADR                    117U
#define   MAINS_OVER_VOLTAGE_ALARM_LEVEL_ADR                     118U
#define   MAINS_UNDER_FREQUENCY_ALARM_LEVEL_ADR                  119U
#define   MAINS_OVER_FREQUENCY_ALARM_LEVEL_ADR                   120U
#define   ENGINE_SETUP_ADR                                       121U
#define   ENGINE_PRE_HEAT_LEVEL_ADR                              122U
#define   ENGINE_PRE_HEAT_DELAY_ADR                              123U
#define   STARTER_STOP_SETUP_ADR                                 124U
#define   STARTER_STOP_GEN_FREQ_LEVEL_ADR                        125U
#define   STARTER_STOP_OIL_PRESSURE_LEVEL_ADR                    126U
#define   STARTER_STOP_CHARGE_ALTERNATOR_LEVEL_ADR               127U
#define   STARTER_STOP_SPEED_LEVEL_ADR                           128U
#define   BATTERY_ALARMS_ADR                                     129U
#define   BATTERY_UNDER_VOLTAGE_LEVEL_ADR                        130U
#define   BATTERY_UNDER_VOLTAGE_DELAY_ADR                        131U
#define   BATTERY_OVER_VOLTAGE_LEVEL_ADR                         132U
#define   BATTERY_OVER_VOLTAGE_DELAY_ADR                         133U
#define   BATTERY_CHARGE_ALARM_LEVEL_ADR                         134U
#define   BATTERY_CHARGE_ALARM_DELAY_ADR                         135U
#define   MAINTENANCE_ALARMS_ADR                                 136U
#define   MAINTENANCE_ALARM_OIL_TIME_ADR                         137U
#define   MAINTENANCE_ALARM_AIR_TIME_ADR                         138U
#define   MAINTENANCE_ALARM_FUEL_TIME_ADR                        139U
#define   LOG_SETUP_ADR                                          140U
#define   RECORD_SETUP0_ADR                                      141U
#define   RECORD_SETUP1_ADR                                      142U
#define   RECORD_INTERVAL_ADR                                    143U
/*---------------------- Bitmap addresses ------------------------------*/
#define   MODBUS_ADR_ADR                               0U
#define   MODBUS_BAUDRATE_ADR                          1U
#define   PASSWORD_ENB_ADR                             0U
#define   OIL_PRESSURE_SENSOR_TYPE_ADR                 0U
#define   OIL_PRESSURE_OPEN_CIRCUIT_ALARM_ENB_ADR      1U
#define   OIL_PRESSURE_ALARM_ENB_ADR                   2U
#define   OIL_PRESSURE_PRE_ALARM_ENB_ADR               3U
#define   COOLANT_TEMP_SENSOR_TYPE_ADR                 0U
#define   COOLANT_TEMP_OPEN_CIRCUIT_ALARM_ENB_ADR      1U
#define   COOLANT_HIGHT_TEMP_ALARM_ENB_ADR             2U
#define   COOLANT_HIGHT_TEMP_ELECTRO_ALARM_ENB_ADR     3U
#define   COOLANT_HIGHT_TEMP_PRE_ALARM_ENB_ADR         4U
#define   COOLANT_TEMP_HEATER_ENB_ADR                  5U
#define   COOLANT_TEMP_COOLER_ENB_ADR                  6U
#define   FUEL_LEVEL_SENSOR_TYPE_ADR                   0U
#define   FUEL_LEVEL_OPEN_CIRCUIT_ALARM_ENB_ADR        1U
#define   FUEL_LEVEL_LOW_ALARM_ENB_ADR                 2U
#define   FUEL_LEVEL_LOW_PRE_ALARM_ENB_ADR             3U
#define   FUEL_LEVEL_HIGHT_PRE_ALARM_ENB_ADR           4U
#define   FUEL_LEVEL_HIGHT_ALARM_ENB_ADR               5U
#define   FUEL_LEAK_ALARM_ENB_ADR                      6U
#define   FUEL_PUMP_ENB_ADR                            7U
#define   SPEED_ENB_ADR                                0U
#define   SPEED_LOW_ALARM_ENB_ADR                      1U
#define   DIA_FUNCTION_ADR                             0U
#define   DIA_POLARITY_ADR                             1U
#define   DIA_ACTION_ADR                               2U
#define   DIA_ARMING_ADR                               3U
#define   DIB_FUNCTION_ADR                             0U
#define   DIB_POLARITY_ADR                             1U
#define   DIB_ACTION_ADR                               2U
#define   DIB_ARMING_ADR                               3U
#define   DIC_FUNCTION_ADR                             0U
#define   DIC_POLARITY_ADR                             1U
#define   DIC_ACTION_ADR                               2U
#define   DIC_ARMING_ADR                               3U
#define   DID_FUNCTION_ADR                             0U
#define   DID_POLARITY_ADR                             1U
#define   DID_ACTION_ADR                               2U
#define   DID_ARMING_ADR                               3U
#define   DOA_N_O_C_ADR                                0U
#define   DOB_N_O_C_ADR                                1U
#define   DOC_N_O_C_ADR                                2U
#define   DOD_N_O_C_ADR                                3U
#define   DOE_N_O_C_ADR                                4U
#define   DOF_N_O_C_ADR                                5U
#define   DOG_N_O_C_ADR                                6U
#define   DOH_N_O_C_ADR                                7U
#define   DOI_N_O_C_ADR                                8U
#define   DOJ_N_O_C_ADR                                9U
#define   DOA_TYPE_ADR                                 0U
#define   DOB_TYPE_ADR                                 1U
#define   DOC_TYPE_ADR                                 0U
#define   DOD_TYPE_ADR                                 1U
#define   DOE_TYPE_ADR                                 0U
#define   DOF_TYPE_ADR                                 1U
#define   DOG_TYPE_ADR                                 0U
#define   DOH_TYPE_ADR                                 1U
#define   DOI_TYPE_ADR                                 0U
#define   DOJ_TYPE_ADR                                 1U
#define   GEN_POWER_GENERATOR_CONTROL_ENB_ADR          0U
#define   GEN_AC_SYS_ADR                               1U
#define   GEN_PHASE_SEQUENCE_CONTROL_ENB_ADR           2U
#define   GEN_UNDER_VOLTAGE_ALARM_ENB_ADR              0U
#define   GEN_UNDER_VOLTAGE_PRE_ALARM_ENB_ADR          1U
#define   GEN_OVER_VOLTAGE_PRE_ALARM_ENB_ADR           2U
#define   GEN_UNDER_FREQUENCY_ALARM_ENB_ADR            3U
#define   GEN_UNDER_FREQUENCY_PRE_ALARM_ENB_ADR        4U
#define   GEN_OVER_FREQUENCY_PRE_ALARM_ENB_ADR         5U
#define   GEN_OVER_FREQUENCY_ALARM_ENB_ADR             6U
#define   GEN_CURRENT_OVERLOAD_PROTECTION_ENB_ADR      7U
#define   GEN_CURRENT_OVER_PHASE_IMBALANCE_ENB_ADR     8U
#define   GEN_CURRENT_OVER_ACTION_ADR                  9U
#define   MAINS_CONTROL_ENB_ADR                        0U
#define   MAINS_POWER_OFF_IMMEDIATELY_ENB_ADR          1U
#define   MAINS_UNDER_VOLTAGE_ALARM_ENB_ADR            0U
#define   MAINS_OVER_VOLTAGE_ALARM_ENB_ADR             1U
#define   MAINS_UNDER_FREQUENCY_ALARM_ENB_ADR          2U
#define   MAINS_OVER_FREQUENCY_ALARM_ENB_ADR           3U
#define   ENGINE_START_ATTEMPTS_ADR                    0U
#define   STARTER_OIL_PRESSURE_CHECK_ON_START_ENB_ADR  0U
#define   STARTER_STOP_OIL_PRESSURE_ENB_ADR            1U
#define   STARTER_STOP_CHARGE_ALTERNATOR_ENB_ADR       2U
#define   STARTER_STOP_SPEED_ENB_ADR                   3U
#define   BATTERY_UNDER_VOLTAGE_ENB_ADR                0U
#define   BATTERY_OVER_VOLTAGE_ENB_ADR                 1U
#define   BATTERY_CHARGE_ALARM_ENB_ADR                 2U
#define   MAINTENANCE_ALARM_OIL_ENB_ADR                0U
#define   MAINTENANCE_ALARM_OIL_ACTION_ADR             1U
#define   MAINTENANCE_ALARM_AIR_ENB_ADR                2U
#define   MAINTENANCE_ALARM_AIR_ACTION_ADR             3U
#define   MAINTENANCE_ALARM_FUEL_ENB_ADR               4U
#define   MAINTENANCE_ALARM_FUEL_ACTION_ADR            5U
#define   LOG_SAVE_WARNING_EVENTS_ENB_ADR              0U
#define   LOG_POSITIVE_EVENTS_ENB_ADR                  1U
#define   RECORD_ENB_ADR                               0U
#define   RECORD_OIL_PRESSURE_ENB_ADR                  1U
#define   RECORD_COOLANT_TEMP_ENB_ADR                  2U
#define   RECORD_FUEL_LEVEL_ENB_ADR                    3U
#define   RECORD_SPEED_ENB_ADR                         4U
#define   RECORD_INPUTS_ENB_ADR                        5U
#define   RECORD_OUTPUTS_ENB_ADR                       6U
#define   RECORD_GEN_PHASE_VOLTAGE_ENB_ADR             7U
#define   RECORD_GEN_LINE_VOLTAGE_ENB_ADR              8U
#define   RECORD_CURRENT_ENB_ADR                       0U
#define   RECORD_FREQ_GEN_ENB_ADR                      1U
#define   RECORD_COS_FI_ENB_ADR                        2U
#define   RECORD_POWER_ACTIVE_ENB_ADR                  3U
#define   RECORD_POWER_REACTIVE_ENB_ADR                4U
#define   RECORD_POWER_FULL_ENB_ADR                    5U
#define   RECORD_MAINS_PHASE_VOLTAGE_ENB_ADR           6U
#define   RECORD_MAINS_LINE_VOLTAGE_ENB_ADR            7U
#define   RECORD_MAINS_FREQ_ENB_ADR                    8U
#define   RECORD_VOLTAGE_ACC_ENB_ADR                   9U
/*------------------------- Extern -------------------------------------*/
extern eConfigReg versionController;
extern eConfigReg versionFirmware;
extern eConfigReg versionBootloader;
extern eConfigReg deviceID;
extern eConfigReg serialNumber0;
extern eConfigReg serialNumber1;
extern eConfigReg displayBrightnesLevel;
extern eConfigReg displayContarstLevel;
extern eConfigReg displaySleepDelay;
extern eConfigReg modbusSetup;
extern eConfigReg hysteresisLevel;
extern eConfigReg moduleSetup;
extern eConfigReg oilPressureSetup;
extern eConfigReg oilPressureAlarmLevel;
extern eConfigReg oilPressurePreAlarmLevel;
extern eConfigReg coolantTempSetup;
extern eConfigReg coolantHightTempAlarmLevel;
extern eConfigReg coolantHightTempElectroAlarmLevel;
extern eConfigReg coolantHightTempPreAlarmLevel;
extern eConfigReg coolantTempHeaterOffLevel;
extern eConfigReg coolantTempHeaterOnLevel;
extern eConfigReg coolantTempCoolerOffLevel;
extern eConfigReg coolantTempCoolerOnLevel;
extern eConfigReg fuelLevelSetup;
extern eConfigReg fuelTankLevel;
extern eConfigReg fuelRateIdleLevel;
extern eConfigReg fuelRateLevel;
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
extern eConfigReg speedSetup;
extern eConfigReg speedToothNumber;
extern eConfigReg speedLowAlarmLevel;
extern eConfigReg speedHightAlarmLevel;
extern eConfigReg diaSetup;
extern eConfigReg diaDelay;
extern eConfigReg diaMessage0;
extern eConfigReg diaMessage1;
extern eConfigReg diaMessage2;
extern eConfigReg diaMessage3;
extern eConfigReg dibSetup;
extern eConfigReg dibDelay;
extern eConfigReg dibMessage0;
extern eConfigReg dibMessage1;
extern eConfigReg dibMessage2;
extern eConfigReg dibMessage3;
extern eConfigReg dicSetup;
extern eConfigReg dicDelay;
extern eConfigReg dicMessage0;
extern eConfigReg dicMessage1;
extern eConfigReg dicMessage2;
extern eConfigReg dicMessage3;
extern eConfigReg didSetup;
extern eConfigReg didDelay;
extern eConfigReg didMessage0;
extern eConfigReg didMessage1;
extern eConfigReg didMessage2;
extern eConfigReg didMessage3;
extern eConfigReg doSetup;
extern eConfigReg doabType;
extern eConfigReg docdType;
extern eConfigReg doefType;
extern eConfigReg doghType;
extern eConfigReg doijType;
extern eConfigReg timerMainsTransientDelay;
extern eConfigReg timerStartDelay;
extern eConfigReg timerCranking;
extern eConfigReg timerCrankDelay;
extern eConfigReg timerStartupIdleTime;
extern eConfigReg timerNominalRPMDelay;
extern eConfigReg timerSafetyOnDelay;
extern eConfigReg timerWarming;
extern eConfigReg timerTransferDelay;
extern eConfigReg timerGenBreakerTripPulse;
extern eConfigReg timerGenBreakerClosePulse;
extern eConfigReg timerMainsBreakerTripPulse;
extern eConfigReg timerMainsBreakerClosePulse;
extern eConfigReg timerReturnDelay;
extern eConfigReg timerCooling;
extern eConfigReg timerCoolingIdle;
extern eConfigReg timerSolenoidHold;
extern eConfigReg timerFailStopDelay;
extern eConfigReg timerGenTransientDelay;
extern eConfigReg genSetup;
extern eConfigReg genRatedActivePowerLevel;
extern eConfigReg genRatedReactivePowerLevel;
extern eConfigReg genRatedApparentPowerLevel;
extern eConfigReg genRatedFrequencyLevel;
extern eConfigReg genRatedCurrentLevel;
extern eConfigReg genCurrentTrasformRatioLevel;
extern eConfigReg genPoleQuantity;
extern eConfigReg genAlarms;
extern eConfigReg genUnderVoltageAlarmLevel;
extern eConfigReg genUnderVoltagePreAlarmLevel;
extern eConfigReg genOverVoltagePreAlarmLevel;
extern eConfigReg genOverVoltageAlarmLevel;
extern eConfigReg genUnderFrequencyAlarmLevel;
extern eConfigReg genUnderFrequencyPreAlarmLevel;
extern eConfigReg genOverFrequencyPreAlarmLevel;
extern eConfigReg genOverFrequencyAlarmLevel;
extern eConfigReg genOverCurrentThermalProtectionLevel;
extern eConfigReg genOverCurrentCutoffLevel;
extern eConfigReg genOverCurrentWarningLevel;
extern eConfigReg genOverCurrentWarningDelay;
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
extern eConfigReg enginePreHeatLevel;
extern eConfigReg enginePreHeatDelay;
extern eConfigReg starterStopSetup;
extern eConfigReg starterStopGenFreqLevel;
extern eConfigReg starterStopOilPressureLevel;
extern eConfigReg starterStopChargeAlternatorLevel;
extern eConfigReg starterStopSpeedLevel;
extern eConfigReg batteryAlarms;
extern eConfigReg batteryUnderVoltageLevel;
extern eConfigReg batteryUnderVoltageDelay;
extern eConfigReg batteryOverVoltageLevel;
extern eConfigReg batteryOverVoltageDelay;
extern eConfigReg batteryChargeAlarmLevel;
extern eConfigReg batteryChargeAlarmDelay;
extern eConfigReg maintenanceAlarms;
extern eConfigReg maintenanceAlarmOilTime;
extern eConfigReg maintenanceAlarmAirTime;
extern eConfigReg maintenanceAlarmFuelTime;
extern eConfigReg logSetup;
extern eConfigReg recordSetup0;
extern eConfigReg recordSetup1;
extern eConfigReg recordInterval;
extern eConfigReg* const configReg[SETTING_REGISTER_NUMBER];
extern const char*       dictionaryArray[SETTING_REGISTER_NUMBER];
extern const fix16_t     scaleMulArray[CONFIG_SCALE_NUM];
/*----------------------------------------------------------------------*/
#endif /* INC_CONFIG_H_ */
