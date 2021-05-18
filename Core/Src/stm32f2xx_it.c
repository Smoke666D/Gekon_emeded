/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f2xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f2xx_it.h"
#include "FreeRTOS.h"
#include "task.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include "vrSensor.h"
#include "adc.h"
#include "electro.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
 
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern ETH_HandleTypeDef  heth;
extern PCD_HandleTypeDef  hpcd_USB_OTG_FS;
extern DMA_HandleTypeDef  hdma_adc1;
extern DMA_HandleTypeDef  hdma_adc2;
extern DMA_HandleTypeDef  hdma_adc3;
extern DMA_HandleTypeDef  hdma_spi3_tx;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef  htim2;
extern TIM_HandleTypeDef  htim3;
extern TIM_HandleTypeDef  htim4;
extern TIM_HandleTypeDef  htim5;
extern TIM_HandleTypeDef  htim6;
extern TIM_HandleTypeDef  htim7;
extern TIM_HandleTypeDef  htim8;
extern TIM_HandleTypeDef  htim12;
extern TIM_HandleTypeDef  htim13;

/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M3 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */

  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/******************************************************************************/
/* STM32F2xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f2xx.s).                    */
/******************************************************************************/
void DMA1_Stream5_IRQHandler ( void )
{
  HAL_DMA_IRQHandler( &hdma_spi3_tx );
  return;
}
void EXTI9_5_IRQHandler ( void )
{
  HAL_GPIO_EXTI_IRQHandler( GPIO_PIN_6 );
  vVRextiCallback();
  return;
}
void TIM2_IRQHandler ( void )
{
  __HAL_TIM_CLEAR_IT( &htim2, TIM_IT_UPDATE );
  return;
}
void TIM3_IRQHandler ( void )
{
  __HAL_TIM_CLEAR_IT( &htim3, TIM_IT_UPDATE );
  return;
}
void TIM4_IRQHandler ( void )
{
  HAL_TIM_IRQHandler(&htim4);
  return;
}
void TIM5_IRQHandler ( void )
{
  vLOGICtimerCallback();
  HAL_TIM_IRQHandler( &htim5 );
  return;
}
void TIM8_BRK_TIM12_IRQHandler ( void )
{
  vELECTROtimCallback();
  HAL_TIM_IRQHandler( &htim12 );
  return;
}
void TIM6_DAC_IRQHandler ( void )
{
  vVRtimCallback();
  HAL_TIM_IRQHandler( &htim6 );
  return;
}
void TIM8_UP_TIM13_IRQHandler ( void )
{
  /*--------------------------------------------------------------------------------*/
  __HAL_TIM_CLEAR_IT( &htim8, TIM_IT_UPDATE );
  /*--------------------------------------------------------------------------------*/
  if (__HAL_TIM_GET_FLAG( &htim13, TIM_FLAG_UPDATE ) != RESET)
  {
    if (__HAL_TIM_GET_IT_SOURCE( &htim13, TIM_IT_UPDATE ) != RESET)
    {
      __HAL_TIM_CLEAR_IT( &htim13, TIM_IT_UPDATE );
      vMBtimHandler();
    }
  }
  /*--------------------------------------------------------------------------------*/
  return;
}
void TIM7_IRQHandler ( void )
{
  __HAL_TIM_CLEAR_IT( &htim7, TIM_IT_UPDATE );
  vLCDDelay();
  return;
}
void DMA2_Stream0_IRQHandler ( void )
{
  vADC_Ready( ADC1_READY );
  HAL_DMA_IRQHandler( &hdma_adc1 );
  return;
}
void DMA2_Stream1_IRQHandler ( void )
{
  vADC_Ready( ADC3_READY );
  HAL_DMA_IRQHandler( &hdma_adc3 );
  return;
}
void DMA2_Stream2_IRQHandler ( void )
{
  vADC_Ready( ADC2_READY);
  HAL_DMA_IRQHandler( &hdma_adc2 );
  return;
}
void ETH_IRQHandler ( void )
{
  HAL_ETH_IRQHandler( &heth );
  return;
}
void OTG_FS_IRQHandler ( void )
{
  HAL_PCD_IRQHandler( &hpcd_USB_OTG_FS );
  return;
}
void USART2_IRQHandler ( void )
{
  vMBuartHandler();
  //HAL_UART_IRQHandler( &huart2 );
  return;
}
