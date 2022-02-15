/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f2xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
typedef enum
{
  SYSTEM_ERROR_CLOCK,
  SYSTEM_ERROR_EEPROM,
  SYSTEM_ERROR_EERPOM_SIZE,
  SYSTEM_ERROR_RTC,
  SYSTEM_ERROR_RTC_BATTERY,
  SYSTEM_ERROR_ADC_OVER,
  SYSTEM_ERROR_LAN,
} SYSTEM_ERROR;
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define FPI_A_Pin GPIO_PIN_2
#define FPI_A_GPIO_Port GPIOE
#define FPI_B_Pin GPIO_PIN_3
#define FPI_B_GPIO_Port GPIOE
#define FPI_C_Pin GPIO_PIN_4
#define FPI_C_GPIO_Port GPIOE
#define FPI_D_Pin GPIO_PIN_5
#define FPI_D_GPIO_Port GPIOE
#define POUT_AB_CS_Pin GPIO_PIN_6
#define POUT_AB_CS_GPIO_Port GPIOE
#define BTN_Pin GPIO_PIN_13
#define BTN_GPIO_Port GPIOC
#define ANALOG_SWITCH_Pin GPIO_PIN_0
#define ANALOG_SWITCH_GPIO_Port GPIOF
#define AIN_INPOW_Pin GPIO_PIN_4
#define AIN_INPOW_GPIO_Port GPIOF
#define AIN_DIN_SM_Pin GPIO_PIN_5
#define AIN_DIN_SM_GPIO_Port GPIOF
#define AIN_NET_N_Pin GPIO_PIN_6
#define AIN_NET_N_GPIO_Port GPIOF
#define AIN_NET_PH3_Pin GPIO_PIN_7
#define AIN_NET_PH3_GPIO_Port GPIOF
#define AIN_NET_PH2_Pin GPIO_PIN_8
#define AIN_NET_PH2_GPIO_Port GPIOF
#define AIN_NET_PH1_Pin GPIO_PIN_9
#define AIN_NET_PH1_GPIO_Port GPIOF
#define AIN_CHARG_ALTER_Pin GPIO_PIN_0
#define AIN_CHARG_ALTER_GPIO_Port GPIOC
#define RMII_MDC_Pin GPIO_PIN_1
#define RMII_MDC_GPIO_Port GPIOC
#define AIN_OILPRES_COOLTEMP_Pin GPIO_PIN_2
#define AIN_OILPRES_COOLTEMP_GPIO_Port GPIOC
#define AIN_COM_Pin GPIO_PIN_3
#define AIN_COM_GPIO_Port GPIOC
#define AIN_POW_PH3_Pin GPIO_PIN_0
#define AIN_POW_PH3_GPIO_Port GPIOA
#define RMII_REF_CLK_Pin GPIO_PIN_1
#define RMII_REF_CLK_GPIO_Port GPIOA
#define RMII_MDIO_Pin GPIO_PIN_2
#define RMII_MDIO_GPIO_Port GPIOA
#define AIN_POW_PH2_Pin GPIO_PIN_3
#define AIN_POW_PH2_GPIO_Port GPIOA
#define AIN_POW_PH1_Pin GPIO_PIN_4
#define AIN_POW_PH1_GPIO_Port GPIOA
#define AIN_GEN_N_Pin GPIO_PIN_5
#define AIN_GEN_N_GPIO_Port GPIOA
#define AIN_GEN_PH3_Pin GPIO_PIN_6
#define AIN_GEN_PH3_GPIO_Port GPIOA
#define RMII_CRS_DV_Pin GPIO_PIN_7
#define RMII_CRS_DV_GPIO_Port GPIOA
#define RMII_RXD0_Pin GPIO_PIN_4
#define RMII_RXD0_GPIO_Port GPIOC
#define RMII_RXD1_Pin GPIO_PIN_5
#define RMII_RXD1_GPIO_Port GPIOC
#define AIN_GEN_PH2_Pin GPIO_PIN_0
#define AIN_GEN_PH2_GPIO_Port GPIOB
#define AIN_GEN_PH1_Pin GPIO_PIN_1
#define AIN_GEN_PH1_GPIO_Port GPIOB
#define DIN_OFFSET_Pin GPIO_PIN_12
#define DIN_OFFSET_GPIO_Port GPIOF
#define CHARG_ON_Pin GPIO_PIN_0
#define CHARG_ON_GPIO_Port GPIOG
#define POUT_A_Pin GPIO_PIN_7
#define POUT_A_GPIO_Port GPIOE
#define POUT_B_Pin GPIO_PIN_8
#define POUT_B_GPIO_Port GPIOE
#define POUT_D_Pin GPIO_PIN_9
#define POUT_D_GPIO_Port GPIOE
#define POUT_CD_CS_Pin GPIO_PIN_10
#define POUT_CD_CS_GPIO_Port GPIOE
#define POUT_C_Pin GPIO_PIN_11
#define POUT_C_GPIO_Port GPIOE
#define POUT_F_Pin GPIO_PIN_12
#define POUT_F_GPIO_Port GPIOE
#define POUT_EF_CS_Pin GPIO_PIN_13
#define POUT_EF_CS_GPIO_Port GPIOE
#define POUT_E_Pin GPIO_PIN_14
#define POUT_E_GPIO_Port GPIOE
#define LCD_RST_Pin GPIO_PIN_15
#define LCD_RST_GPIO_Port GPIOE
#define LCD_SCK_Pin GPIO_PIN_10
#define LCD_SCK_GPIO_Port GPIOB
#define LCD_LED_Pin GPIO_PIN_11
#define LCD_LED_GPIO_Port GPIOB
#define LCD_CS_Pin GPIO_PIN_12
#define LCD_CS_GPIO_Port GPIOB
#define RMII_TXD1_Pin GPIO_PIN_13
#define RMII_TXD1_GPIO_Port GPIOB
#define LCD_MISO_Pin GPIO_PIN_14
#define LCD_MISO_GPIO_Port GPIOB
#define LCD_MOSI_Pin GPIO_PIN_15
#define LCD_MOSI_GPIO_Port GPIOB
#define KL_UP_Pin GPIO_PIN_11
#define KL_UP_GPIO_Port GPIOD
#define KL_DOWN_Pin GPIO_PIN_12
#define KL_DOWN_GPIO_Port GPIOD
#define KL_START_Pin GPIO_PIN_13
#define KL_START_GPIO_Port GPIOD
#define KL_AUTO_Pin GPIO_PIN_14
#define KL_AUTO_GPIO_Port GPIOD
#define KL_STOP_Pin GPIO_PIN_15
#define KL_STOP_GPIO_Port GPIOD
#define LED1_Pin GPIO_PIN_2
#define LED1_GPIO_Port GPIOG
#define LED2_Pin GPIO_PIN_3
#define LED2_GPIO_Port GPIOG
#define LED3_Pin GPIO_PIN_4
#define LED3_GPIO_Port GPIOG
#define VR_COUNT_Pin GPIO_PIN_5
#define VR_COUNT_GPIO_Port GPIOG
#define VR_COUNT_EXTI_IRQn EXTI9_5_IRQn
#define USB_ENB_Pin GPIO_PIN_6
#define USB_ENB_GPIO_Port GPIOG
#define USB_DETECTED_Pin GPIO_PIN_7
#define USB_DETECTED_GPIO_Port GPIOG
#define SDIO_CD_Pin GPIO_PIN_8
#define SDIO_CD_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define EEPROM_NSS_Pin GPIO_PIN_15
#define EEPROM_NSS_GPIO_Port GPIOA
#define RS485_DE_Pin GPIO_PIN_4
#define RS485_DE_GPIO_Port GPIOD
#define RX485_TX_Pin GPIO_PIN_5
#define RX485_TX_GPIO_Port GPIOD
#define RS485_RX_Pin GPIO_PIN_6
#define RS485_RX_GPIO_Port GPIOD
#define LED_SYS_Pin GPIO_PIN_7
#define LED_SYS_GPIO_Port GPIOD
#define POUT_GH_CS_Pin GPIO_PIN_9
#define POUT_GH_CS_GPIO_Port GPIOG
#define POUT_G_Pin GPIO_PIN_10
#define POUT_G_GPIO_Port GPIOG
#define RMII_TX_EN_Pin GPIO_PIN_11
#define RMII_TX_EN_GPIO_Port GPIOG
#define POUT_H_Pin GPIO_PIN_12
#define POUT_H_GPIO_Port GPIOG
#define RMII_TXD0_Pin GPIO_PIN_13
#define RMII_TXD0_GPIO_Port GPIOG
#define POUT_J_Pin GPIO_PIN_15
#define POUT_J_GPIO_Port GPIOG
#define EEPROM_SCK_Pin GPIO_PIN_3
#define EEPROM_SCK_GPIO_Port GPIOB
#define EEPROM_MISO_Pin GPIO_PIN_4
#define EEPROM_MISO_GPIO_Port GPIOB
#define EEPROM_MOSI_Pin GPIO_PIN_5
#define EEPROM_MOSI_GPIO_Port GPIOB
#define POUT_IJ_CS_Pin GPIO_PIN_6
#define POUT_IJ_CS_GPIO_Port GPIOB
#define POUT_I_Pin GPIO_PIN_7
#define POUT_I_GPIO_Port GPIOB
#define RTC_SCL_Pin GPIO_PIN_8
#define RTC_SCL_GPIO_Port GPIOB
#define RTC_SDA_Pin GPIO_PIN_9
#define RTC_SDA_GPIO_Port GPIOB
#define RTC_INT_Pin GPIO_PIN_0
#define RTC_INT_GPIO_Port GPIOE
#define RTC_RST_Pin GPIO_PIN_1
#define RTC_RST_GPIO_Port GPIOE
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
