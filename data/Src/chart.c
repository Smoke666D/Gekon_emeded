/*
 * chart.c
 *
 *  Created on: 24 мар. 2020 г.
 *      Author: photo_Mickey
 */
/*----------------------- Includes -------------------------------------*/
#include "chart.h"
/*------------------------ Define --------------------------------------*/
/*----------------------- Constant -------------------------------------*/
const eAxisAttributes axisAtribOil = {
  .min  = 0U,
  .max  = F16( OIL_Y_MAX_DEF ),
  .unit = { 0xd091U, 0xd0b0U, 0xd180U }
};
const eAxisAttributes axisAtribCoolant = {
  .min  = 0U,
  .max  = F16( COOLANT_Y_MAX_DEF ),
  .unit = { 0x0043U, 0x0020U, 0x0020U  }
};
const eAxisAttributes axisAtribFuel = {
  .min  = 0U,
  .max  = F16( FUEL_Y_MAX_DEF ),
  .unit = { 0x0025U, 0x0020U, 0x0020U }
};
const eAxisAttributes axisAtribResistive = {
  .min  = 0U,
  .max  = F16( RESIST_X_MAX_DEF ),
  .unit = { 0xD09EU, 0xD0BCU, 0x0020U },
};
const eAxisAttributes axisAtribCurrent = {
  .min  = 0U,
  .max  = F16( CURRENT_X_MAX_DEF ),
  .unit = { 0xD090U, 0x0020U, 0x0020U },
};
const eAxisAttributes* const  xAxisAtribs[X_AXIS_TYPES_NUMBER] = {
  &axisAtribResistive,
  &axisAtribCurrent,
};
const eAxisAttributes* const  yAxisAtribs[Y_AXIS_TYPES_NUMBER] = {
  &axisAtribOil,
  &axisAtribCoolant,
  &axisAtribFuel,
};
/*---------------------- Structures ------------------------------------*/
SemaphoreHandle_t xCHARTSemaphore;
eChartData oilSensorChart = {
  .xType   = X_AXIS_TYPE_RESISTIVE,
  .yType   = Y_AXIS_TYPE_OIL,
  .size    = 2U,
  .dots[0] =
  {
    .x = 0U,
    .y = 0U,
  },
  .dots[1] =
  {
    .x = 0U,
    .y = 0U,
  },
};

eChartData coolantSensorChart = {
  .xType   = X_AXIS_TYPE_RESISTIVE,
  .yType   = Y_AXIS_TYPE_COOLANT,
  .size    = 2U,
  .dots[0] =
  {
    .x = 0U,
    .y = 0U,
  },
  .dots[1] =
  {
    .x = 0U,
    .y = 0U,
  },
};

eChartData fuelSensorChart = {
  .xType    = X_AXIS_TYPE_RESISTIVE,
  .yType    = Y_AXIS_TYPE_FUEL,
  .size     = 2U,
  .dots[0U] =
  {
    .x = 0U,
    .y = 0U,
  },
  .dots[1U] =
  {
    .x = 0U,
    .y = 0U,
  },
};

eChartData* const charts[CHART_NUMBER] = { &oilSensorChart, &coolantSensorChart, &fuelSensorChart};
/*---------------------------------------------------------------------------------------------------*/
void vCHARTinitCharts ( void )
{
  uint8_t i = 0U;
  xCHARTSemaphore = xSemaphoreCreateMutex();
  for ( i=0U; i<CHART_NUMBER; i++ )
  {
    charts[i]->dots[1U].x = xAxisAtribs[charts[i]->xType]->max;
    charts[i]->dots[1U].y = yAxisAtribs[charts[i]->yType]->max;
  }
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Calculate line functions between dots of chart
 * input:  chart for calculation
 * output: none
 */
void vCHARTcalcFunction ( const eChartData* chart, uint16_t n, eChartFunction* func )
{
  func->k = fix16_div( fix16_sub( chart->dots[n + 1U].y, chart->dots[n].y ), fix16_sub( chart->dots[n + 1U].x, chart->dots[n].x ) );
  func->b = fix16_sub( chart->dots[n].y, fix16_mul( func->k, chart->dots[n].x ) );
  return;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Get value of chart functions
 * input:  x     - input value
 *         chart - chart of the function
 *         y     - output value
 * output: error status
 */
eFunctionError eCHARTfunc ( const eChartData* chart, fix16_t x, fix16_t* y )
{
  eFunctionError res  = FUNC_OK;
  uint16_t       i    = 0U;
  eChartFunction func = { 0U };

  if ( x <= xAxisAtribs[chart->xType]->max )
  {
    if ( x >= xAxisAtribs[chart->xType]->min )
    {
      for ( i=1U; i<chart->size; i++ )
      {
        if ( x < chart->dots[i].x )
        {
          break;
        }
      }
      i--;
      if ( i != chart->size)
      {
        vCHARTcalcFunction( chart, i, &func );
        *y = fix16_add( fix16_mul( func.k, x ), func.b );
      }
      else
      {
        res = FUNC_SIZE_ERROR;
        *y = yAxisAtribs[chart->yType]->max;

      }
    }
    else
    {
      res = FUNC_OVER_MIN_X_ERROR;
      *y = yAxisAtribs[chart->yType]->min;
    }
  }
  else
  {
    res = FUNC_OVER_MAX_X_ERROR;
    *y = yAxisAtribs[chart->yType]->max;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
SemaphoreHandle_t xCHARTgetSemophore ( void )
{
  return xCHARTSemaphore;
}











