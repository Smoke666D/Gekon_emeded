/*
 * chart.h
 *
 *  Created on: 16 мар. 2020 г.
 *      Author: mikhail.mikhailov
 */
#ifndef INC_CHART_H_
#define INC_CHART_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "fix16.h"
#include "cmsis_os.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
/*------------------------ Define --------------------------------------*/
#define CHART_UNIT_LENGTH       3U
#define CHART_DOTS_SIZE         128U
#define CHART_CHART_SIZE        1027U  /* byte */
#define CHART_NUMBER            3U
#define CHART_DATA_XMIN_STR     "xmin"
#define CHART_DATA_XMAX_STR     "xmax"
#define CHART_DATA_YMIN_STR     "ymin"
#define CHART_DATA_YMAX_STR     "ymax"
#define CHART_DATA_XUNIT_STR    "xunit"
#define CHART_DATA_YUNIT_STR    "yunit"
#define CHART_DATA_SIZE_STR     "size"
#define CHART_DATA_DOTS_STR     "dots"
#define CHART_DOT_X_STR         "x"
#define CHART_DOT_Y_STR         "y"
#define OIL_Y_MAX_DEF           2U
#define COOLANT_Y_MAX_DEF       250U
#define FUEL_Y_MAX_DEF          100U
#define RESIST_X_MAX_DEF        1500U
#define CURRENT_X_MAX_DEF       20U
#define X_AXIS_TYPES_NUMBER     2U
#define Y_AXIS_TYPES_NUMBER     3U
#define OIL_CHART_ADR           0U
#define COOLANT_CHART_ADR       1U
#define FUEL_CHART_ADR          2U
/*------------------------- Enum ---------------------------------------*/
typedef enum
{
  FUNC_OK,
  FUNC_OVER_MAX_X_ERROR,
  FUNC_OVER_MIN_X_ERROR,
  FUNC_SIZE_ERROR,
} eFunctionError;

typedef enum
{
  X_AXIS_TYPE_RESISTIVE,
  X_AXIS_TYPE_CURRENT,
} X_AXIS_TYPE;

typedef enum
{
  Y_AXIS_TYPE_OIL,
  Y_AXIS_TYPE_COOLANT,
  Y_AXIS_TYPE_FUEL
} Y_AXIS_TYPE;
/*----------------------- Structures -----------------------------------*/
typedef struct __packed
{
  fix16_t x;
  fix16_t y;
} eChartDot;

typedef struct __packed
{
  fix16_t k;
  fix16_t b;
} eChartFunction; /* y = k*x + b */

typedef struct __packed
{
  fix16_t  min;                     /* 4 bytes */
  fix16_t  max;                     /* 4 bytes */
  uint16_t unit[CHART_UNIT_LENGTH]; /* 6 bytes */
} eAxisAttributes;

typedef struct __packed
{
  X_AXIS_TYPE xType : 1;
  Y_AXIS_TYPE yType : 2;             /* 1 byte  */
  uint16_t    size;                  /* 2 bytes */
  eChartDot   dots[CHART_DOTS_SIZE]; /* 128 * 8 = 1024 bytes  */
} eChartData;
/*------------------------ Extern --------------------------------------*/
extern       eChartData*      const charts[CHART_NUMBER];
extern const eAxisAttributes* const xAxisAtribs[X_AXIS_TYPES_NUMBER];
extern const eAxisAttributes* const yAxisAtribs[Y_AXIS_TYPES_NUMBER];
/*----------------------- Function -------------------------------------*/
void              vCHARTinitCharts ( void );
void              vCHARTupdateAtrib ( void );
void              vCHARTcalcFunction ( const eChartData* chart, uint16_t n, eChartFunction* func );  /* Calculate line functions between dots of chart */
eFunctionError    eCHARTfunc ( const eChartData* chart, fix16_t x, fix16_t* y );                     /* Get value of chart functions */
SemaphoreHandle_t xCHARTgetSemophore ( void );
/*----------------------------------------------------------------------*/
#endif /* INC_CHART_H_ */
