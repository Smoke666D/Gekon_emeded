/*
 * chart.c
 *
 *  Created on: 24 мар. 2020 г.
 *      Author: photo_Mickey
 */
/*----------------------- Includes -------------------------------------*/
#include "chart.h"
/*----------------------- Constant -------------------------------------*/
#define  OIL_Y_MAX_DEF       F16( 2U )
#define  COOLANT_Y_MAX_DEF   F16( 250U )
#define  FUEL_Y_MAX_DEF      F16( 100U )
#define  RESIST_X_MAX_DEF    F16( 1048U )
/*---------------------- Structures ------------------------------------*/
SemaphoreHandle_t xCHARTSemaphore;
eChartData oilSensorChart = {
  .xmin     = 0U,
  .xmax     = RESIST_X_MAX_DEF,
  .ymin     = 0U,
  .ymax     = OIL_Y_MAX_DEF,
  .xunit    = {'О','м',' '},
  .yunit    = {'Б','а','р'},
  .size     = 2U,
  .dots[0]  =
  {
    .x = 0U,
    .y = 0U,
  },
  .dots[1]  =
  {
    .x = RESIST_X_MAX_DEF,
    .y = OIL_Y_MAX_DEF,
  },
};

eChartData coolantSensorChart = {
  .xmin     = 0U,
  .xmax     = RESIST_X_MAX_DEF,
  .ymin     = 0U,
  .ymax     = COOLANT_Y_MAX_DEF,
  .xunit    = {'О','м',' '},
  .yunit    = {'°','C',' '},
  .size     = 2U,
  .dots[0]  =
  {
    .x = 0U,
    .y = 0U,
  },
  .dots[1]  =
  {
    .x = RESIST_X_MAX_DEF,
    .y = COOLANT_Y_MAX_DEF,
  },
};

eChartData fuelSensorChart = {
  .xmin     = 0U,
  .xmax     = RESIST_X_MAX_DEF,
  .ymin     = 0U,
  .ymax     = FUEL_Y_MAX_DEF,
  .xunit    = {'О','м',' '},
  .yunit    = {'%',' ',' '},
  .size     = 2U,
  .dots[0U]  =
  {
    .x = 0U,
    .y = 0U,
  },
  .dots[1U]  =
  {
    .x = RESIST_X_MAX_DEF,
    .y = FUEL_Y_MAX_DEF,
  },
};

eChartData* const charts[CHART_NUMBER] = { &oilSensorChart, &coolantSensorChart, &fuelSensorChart};
/*---------------------------------------------------------------------------------------------------*/
void vCHARTinitCharts ( void )
{
  xCHARTSemaphore = xSemaphoreCreateMutex();
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

  if ( x <= chart->xmax )
  {
    if ( x >= chart->xmin )
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
        *y  = chart->ymax;
      }
    }
    else
    {
      res = FUNC_OVER_MIN_X_ERROR;
      *y  = chart->ymin;
    }
  }
  else
  {
    res = FUNC_OVER_MAX_X_ERROR;
    *y  = chart->ymax;
  }
  return res;
}
/*---------------------------------------------------------------------------------------------------*/
SemaphoreHandle_t xCHARTgetSemophore ( void )
{
  return xCHARTSemaphore;
}











