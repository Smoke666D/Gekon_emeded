/*
 * chart.c
 *
 *  Created on: 24 мар. 2020 г.
 *      Author: photo_Mickey
 */
/*----------------------- Includes -------------------------------------*/
#include "chart.h"

/*---------------------- Structures ------------------------------------*/
SemaphoreHandle_t xCHARTSemaphore;
eChartData oilSensorChart = {
  .xmin     = 0x00000000U,
  .xmax     = 0x05DC0000U,
  .ymin     = 0x00000000U,
  .ymax     = 0x000F0000U,
  .xunit    = {'О','м',' '},
  .yunit    = {'Б','а','р'},
  .size     = 2U,
  .dots[0]  =
  {
    .x = 0x00000000U,
    .y = 0x00000000U,
  },
  .dots[1]  =
  {
    .x = 0x05DC0000U,
    .y = 0x000F0000U,
  },
};

eChartData coolantSensorChart = {
  .xmin     = 0x00000000U,
  .xmax     = 0x05DC0000U,
  .ymin     = 0x00000000U,
  .ymax     = 0x000F0000U,
  .xunit    = {'О','м',' '},
  .yunit    = {'Б','а','р'},
  .size     = 2U,
  .dots[0]  =
  {
    .x = 0x00000000U,
    .y = 0x00000000U,
  },
  .dots[1]  =
  {
    .x = 0x05DC0000U,
    .y = 0x000F0000U,
  },
};

eChartData fuelSensorChart = {
  .xmin     = 0x00000000U,
  .xmax     = 0x05DC0000U,
  .ymin     = 0x00000000U,
  .ymax     = 0x000F0000U,
  .xunit    = {'О','м',' '},
  .yunit    = {'Б','а','р'},
  .size     = 2U,
  .dots[0U]  =
  {
    .x = 0x00000000U,
    .y = 0x00000000U,
  },
  .dots[1U]  =
  {
    .x = 0x05DC0000U,
    .y = 0x000F0000U,
  },
};

eChartData* const charts[CHART_NUMBER] = { &oilSensorChart, &coolantSensorChart, &fuelSensorChart};

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
eFunctionError eCHARTfunc( const eChartData* chart, fix16_t x, fix16_t* y )
{
  eFunctionError res  = FUNC_OK;
  uint16_t       i    = 0U;
  eChartFunction func = { .b = 0, .k = 0 };

  if ( x <= chart->xmax )
  {
    if ( x >= chart->xmin)
    {
      for ( i=0U; i<chart->size; i++ )
      {
        if ( x > chart->dots[i].x )
        {
          break;
        }
      }
      if ( i != chart->size)
      {
        vCHARTcalcFunction( chart, i, &func );
        *y = fix16_add( fix16_mul( func.k, x ), func.b );
      }
      else res = FUNC_SIZE_ERROR;
    } else res = FUNC_OVER_MIN_X_ERROR;
  } else res = FUNC_OVER_MAX_X_ERROR;
  return res;
}
/*---------------------------------------------------------------------------------------------------*/












