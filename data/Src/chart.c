/*
 * chart.c
 *
 *  Created on: 24 мар. 2020 г.
 *      Author: photo_Mickey
 */
/*----------------------- Includes -------------------------------------*/
#include "chart.h"
/*---------------------- Structures ------------------------------------*/
eChartData oilSensorChart = {
  .xmin     = 0x00000000U,
  .xmax     = 0x00100000U,
  .ymin     = 0x00000000U,
  .ymax     = 0x00100000U,
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
    .x = 0x00100000U,
    .y = 0x00100000U,
  },
};

eChartData coolantSensorChart = {
  .xmin     = 0x00000000U,
  .xmax     = 0x00100000U,
  .ymin     = 0x00000000U,
  .ymax     = 0x00100000U,
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
    .x = 0x00100000U,
    .y = 0x00100000U,
  },
};

eChartData fuelSensorChart = {
  .xmin     = 0x00000000U,
  .xmax     = 0x00100000U,
  .ymin     = 0x00000000U,
  .ymax     = 0x00100000U,
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
    .x = 0x00100000U,
    .y = 0x00100000U,
  },
};

eChartData* charts[CHART_NUMBER] = { &oilSensorChart, &coolantSensorChart, &fuelSensorChart};
