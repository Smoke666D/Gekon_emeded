/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : stm32f2xx_hal_msp.c
  * Description        : This file provides code for the MSP Initialization 
  *                      and de-Initialization codes.
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

extern DMA_HandleTypeDef hdma_adc1;
extern DMA_HandleTypeDef hdma_adc2;
extern DMA_HandleTypeDef hdma_adc3;
extern DMA_HandleTypeDef hdma_spi3_tx;


void HAL_MspInit(void)
{
  __HAL_RCC_SYSCFG_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_NVIC_SetPriority(PendSV_IRQn, 15, 0);
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (hadc->Instance==ADC1)
  {
    __HAL_RCC_ADC1_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitStruct.Pin = AIN_POW_PH1_Pin|AIN_POW_PH2_Pin|AIN_POW_PH3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    hdma_adc1.Instance = DMA2_Stream0;
    hdma_adc1.Init.Channel = DMA_CHANNEL_0;
    hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc1.Init.Mode = DMA_NORMAL;
    hdma_adc1.Init.Priority = DMA_PRIORITY_LOW;
    hdma_adc1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
    {
      Error_Handler();
    }
    __HAL_LINKDMA(hadc,DMA_Handle,hdma_adc1);
  }
  else if (hadc->Instance==ADC2)
  {
    __HAL_RCC_ADC2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = AIN_GEN_N_Pin|AIN_GEN_PH3_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = AIN_GEN_PH2_Pin|AIN_GEN_PH1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    hdma_adc2.Instance = DMA2_Stream2;
    hdma_adc2.Init.Channel = DMA_CHANNEL_1;
    hdma_adc2.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc2.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc2.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc2.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc2.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc2.Init.Mode = DMA_NORMAL;
    hdma_adc2.Init.Priority = DMA_PRIORITY_LOW;
    hdma_adc2.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_adc2) != HAL_OK)
    {
      Error_Handler();
    }
    __HAL_LINKDMA(hadc,DMA_Handle,hdma_adc2);
  }
  else if (hadc->Instance==ADC3)
  {
    __HAL_RCC_ADC3_CLK_ENABLE();
    __HAL_RCC_GPIOF_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStruct.Pin = AIN_INPOW_Pin|AIN_DIN_SM_Pin|AIN_NET_N_Pin|AIN_NET_PH3_Pin
                          |AIN_NET_PH2_Pin|AIN_NET_PH1_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
    GPIO_InitStruct.Pin = AIN_CHARG_ALTER_Pin|AIN_OILPRES_COOLTEMP_Pin|AIN_FUEL_COM_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    hdma_adc3.Instance = DMA2_Stream1;
    hdma_adc3.Init.Channel = DMA_CHANNEL_2;
    hdma_adc3.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_adc3.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_adc3.Init.MemInc = DMA_MINC_ENABLE;
    hdma_adc3.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
    hdma_adc3.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
    hdma_adc3.Init.Mode = DMA_NORMAL;
    hdma_adc3.Init.Priority = DMA_PRIORITY_HIGH;
    hdma_adc3.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_adc3) != HAL_OK)
    {
      Error_Handler();
    }
    __HAL_LINKDMA(hadc,DMA_Handle,hdma_adc3);
  }
}

void HAL_ADC_MspDeInit(ADC_HandleTypeDef* hadc)
{
  if (hadc->Instance==ADC1)
  {
    __HAL_RCC_ADC1_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOA, AIN_POW_PH1_Pin|AIN_POW_PH2_Pin|AIN_POW_PH3_Pin);
    HAL_DMA_DeInit(hadc->DMA_Handle);
  }
  else if (hadc->Instance==ADC2)
  {
    __HAL_RCC_ADC2_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOA, AIN_GEN_N_Pin|AIN_GEN_PH3_Pin);
    HAL_GPIO_DeInit(GPIOB, AIN_GEN_PH2_Pin|AIN_GEN_PH1_Pin);
    HAL_DMA_DeInit(hadc->DMA_Handle);
  }
  else if (hadc->Instance==ADC3)
  {
    __HAL_RCC_ADC3_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOF, AIN_INPOW_Pin|AIN_DIN_SM_Pin|AIN_NET_N_Pin|AIN_NET_PH3_Pin
                          |AIN_NET_PH2_Pin|AIN_NET_PH1_Pin);
    HAL_GPIO_DeInit(GPIOC, AIN_CHARG_ALTER_Pin|AIN_OILPRES_COOLTEMP_Pin|AIN_FUEL_COM_Pin);
    HAL_DMA_DeInit(hadc->DMA_Handle);
  }
}
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (hcan->Instance==CAN1)
  {
    __HAL_RCC_CAN1_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* hcan)
{
  if (hcan->Instance==CAN1)
  {
    __HAL_RCC_CAN1_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_0|GPIO_PIN_1);
  }
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (hi2c->Instance==I2C1)
  {
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = RTC_SCL_Pin|RTC_SDA_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    __HAL_RCC_I2C1_CLK_ENABLE();
  }
}

void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
{
  if (hi2c->Instance==I2C1)
  {
    __HAL_RCC_I2C1_CLK_DISABLE();
    HAL_GPIO_DeInit(RTC_SCL_GPIO_Port, RTC_SCL_Pin);
    HAL_GPIO_DeInit(RTC_SDA_GPIO_Port, RTC_SDA_Pin);
  }
}
void HAL_SPI_MspInit(SPI_HandleTypeDef* hspi)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if (hspi->Instance==SPI1)
  {
    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = EEPROM_SCK_Pin|EEPROM_MISO_Pin|EEPROM_MOSI_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  }
  else if (hspi->Instance==SPI3)
  {
    __HAL_RCC_SPI3_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStruct.Pin = LCD_SCK_Pin|LCD_MISO_Pin|LCD_MOSI_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    hdma_spi3_tx.Instance = DMA1_Stream5;
    hdma_spi3_tx.Init.Channel = DMA_CHANNEL_0;
    hdma_spi3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_spi3_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_spi3_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_spi3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_spi3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_spi3_tx.Init.Mode = DMA_NORMAL;
    hdma_spi3_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_spi3_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
    if (HAL_DMA_Init(&hdma_spi3_tx) != HAL_OK)
    {
      Error_Handler();
    }
    __HAL_LINKDMA(hspi,hdmatx,hdma_spi3_tx);
  }
}
void HAL_SPI_MspDeInit(SPI_HandleTypeDef* hspi)
{
  if (hspi->Instance==SPI1)
  {
    __HAL_RCC_SPI1_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOB, EEPROM_SCK_Pin|EEPROM_MISO_Pin|EEPROM_MOSI_Pin);
  }
  else if (hspi->Instance==SPI3)
  {
    __HAL_RCC_SPI3_CLK_DISABLE();
    HAL_GPIO_DeInit(GPIOC, LCD_SCK_Pin|LCD_MISO_Pin|LCD_MOSI_Pin);
    HAL_DMA_DeInit(hspi->hdmatx);
  }
}
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* htim_base)
{
  if ( htim_base->Instance == TIM2 )
  {
    __HAL_RCC_TIM2_CLK_ENABLE();
    HAL_NVIC_SetPriority( TIM2_IRQn, 5, 0 );
    HAL_NVIC_EnableIRQ( TIM2_IRQn );
  }
  else if ( htim_base->Instance == TIM3 )
  {
    __HAL_RCC_TIM3_CLK_ENABLE();
    HAL_NVIC_SetPriority( TIM3_IRQn, 0, 0 );
    HAL_NVIC_EnableIRQ( TIM3_IRQn );
  }
  else if ( htim_base->Instance == TIM5 )
  {
    __HAL_RCC_TIM5_CLK_ENABLE();
    HAL_NVIC_SetPriority( TIM5_IRQn, 5, 0 );
    HAL_NVIC_EnableIRQ( TIM5_IRQn );
  }
  else if ( htim_base->Instance == TIM6 )
  {
    __HAL_RCC_TIM6_CLK_ENABLE();
    HAL_NVIC_SetPriority( TIM6_DAC_IRQn, 5, 0 );
    HAL_NVIC_EnableIRQ( TIM6_DAC_IRQn );
  }
  else if ( htim_base->Instance == TIM7 )
  {
    __HAL_RCC_TIM7_CLK_ENABLE();
    HAL_NVIC_SetPriority( TIM7_IRQn, 5, 0 );
    HAL_NVIC_EnableIRQ( TIM7_IRQn );
  }
  else if ( htim_base->Instance == TIM8 )
  {
    __HAL_RCC_TIM8_CLK_ENABLE();
    HAL_NVIC_SetPriority( TIM8_UP_TIM13_IRQn, 0, 0 );
    HAL_NVIC_EnableIRQ( TIM8_UP_TIM13_IRQn );
  }
  else if ( htim_base->Instance == TIM12 )
  {
    __HAL_RCC_TIM12_CLK_ENABLE();
    HAL_NVIC_SetPriority(TIM8_BRK_TIM12_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(TIM8_BRK_TIM12_IRQn);
  }
  else if ( htim_base->Instance == TIM13 )
  {
    __HAL_RCC_TIM13_CLK_ENABLE();
    HAL_NVIC_SetPriority( TIM8_UP_TIM13_IRQn, 0, 0 );
    HAL_NVIC_EnableIRQ( TIM8_UP_TIM13_IRQn );
  }
}
void HAL_TIM_Base_MspDeInit ( TIM_HandleTypeDef* htim_base )
{
  if ( htim_base->Instance == TIM2 )
  {
    __HAL_RCC_TIM2_CLK_DISABLE();
    HAL_NVIC_DisableIRQ( TIM2_IRQn );
  }
  else if ( htim_base->Instance == TIM3 )
  {
    __HAL_RCC_TIM3_CLK_DISABLE();
    HAL_NVIC_DisableIRQ( TIM3_IRQn );
  }
  else if ( htim_base->Instance == TIM6 )
  {
    __HAL_RCC_TIM6_CLK_DISABLE();
    HAL_NVIC_DisableIRQ( TIM6_DAC_IRQn );
  }
  else if ( htim_base->Instance == TIM7 )
  {
    __HAL_RCC_TIM7_CLK_DISABLE();
    HAL_NVIC_DisableIRQ( TIM7_IRQn );
  }
  else if ( htim_base->Instance == TIM8 )
  {
    __HAL_RCC_TIM8_CLK_DISABLE();
    HAL_NVIC_DisableIRQ( TIM8_UP_TIM13_IRQn );
  }
}

void HAL_UART_MspInit ( UART_HandleTypeDef* huart )
{
  GPIO_InitTypeDef GPIO_InitStruct = { 0U };
  if ( huart->Instance == USART2 )
  {
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitStruct.Pin       = RS485_TX_Pin|RS485_RX_Pin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init( GPIOD, &GPIO_InitStruct );
    HAL_NVIC_SetPriority( USART2_IRQn, 0U, 0U );
    HAL_NVIC_EnableIRQ( USART2_IRQn );
  }
  else if ( huart->Instance == USART3 )
  {
    __HAL_RCC_USART3_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitStruct.Pin       = STLK_RX_Pin|STLK_TX_Pin;
    GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull      = GPIO_NOPULL;
    GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART3;
    HAL_GPIO_Init( GPIOD, &GPIO_InitStruct );
  }
}

void HAL_UART_MspDeInit ( UART_HandleTypeDef* huart )
{
  if ( huart->Instance == USART2 )
  {
    __HAL_RCC_USART2_CLK_DISABLE();
    HAL_GPIO_DeInit( GPIOD, RS485_TX_Pin | RS485_RX_Pin );
    HAL_NVIC_DisableIRQ( USART2_IRQn );
  }
  else if ( huart->Instance == USART3 )
  {
    __HAL_RCC_USART3_CLK_DISABLE();
    HAL_GPIO_DeInit( GPIOD, STLK_RX_Pin | STLK_TX_Pin );
  }
}
