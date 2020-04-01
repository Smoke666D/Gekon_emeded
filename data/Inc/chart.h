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
/*------------------------ Define --------------------------------------*/
#define	CHART_UNIT_LENGTH       3U
#define	CHART_CHART_MAX_SIZE    128U
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
/*----------------------- Structures -----------------------------------*/
typedef enum
{
  FUNC_OK,
  FUNC_OVER_MAX_X_ERROR,
  FUNC_OVER_MIN_X_ERROR,
  FUNC_SIZE_ERROR,
} eFunctionError;

typedef struct
{
  fix16_t x;
  fix16_t y;
} eChartDot;

typedef struct
{
  fix16_t k;
  fix16_t b;
} eChartFunction; /* y = k*x + b */

typedef struct
{
  fix16_t 	      xmin;
  fix16_t 	      xmax;
  fix16_t  	      ymin;
  fix16_t 	      ymax;
  uint16_t        xunit[CHART_UNIT_LENGTH];
  uint16_t        yunit[CHART_UNIT_LENGTH];
  uint16_t	      size;
  eChartDot	      dots[CHART_CHART_MAX_SIZE];
  eChartFunction  func[CHART_CHART_MAX_SIZE - 1U];
} eChartData;
/*------------------------ Extern --------------------------------------*/
extern eChartData  oilSensorChart;
extern eChartData  coolantSensorChart;
extern eChartData  fuelSensorChart;
extern eChartData* charts[CHART_NUMBER];
/*----------------------- Function -------------------------------------*/
void           vCHARTcalcFunction( eChartData* chart );                 /* Calculate line functions between dots of chart */
eFunctionError eCHARTfunc( fix16_t x, eChartData* chart, fix16_t* y );  /* Get value of chart functions */
/*----------------------------------------------------------------------*/
#endif /* INC_CHART_H_ */
