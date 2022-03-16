/*
 * MAX31329.h
 *
 *  Created on: Feb 2, 2022
 *      Author: 79110
 */

#ifndef INC_RTC_MAX31329_H_
#define INC_RTC_MAX31329_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
/*------------------------ Define --------------------------------------*/
#define  RTC_DEVICE_ADR           0xD0U
#define  RTC_MEMORY_SIZE          98U
#define  RTC_RAM_SIZE             64U
/*---------- Status Register ------------*/
#define  RTC_SR_A1F               0x01U  /* Alarm 1 Interrupt Flag */
#define  RTC_SR_A2F               0x02U  /* Alarm 2 Interrupt Flag */
#define  RTC_SR_TIF               0x04U  /* Timer Interrupt Flag */
#define  RTC_SR_DIF               0x08U  /* Digital (DIN) Interrupt Flag */
#define  RTC_SR_LOS               0x10U  /* Loss of Signal. Valid only for external clock modes. */
#define  RTC_SR_PFAIL             0x20U  /* Power-Fail Flag */
#define  RTC_SR_OSF               0x40U  /* Oscillator Stop Flag */
#define  RTC_SR_PSDECT            0x80U  /* Main Supply Source Indication */
/*---------- Control Register -----------*/
#define  RTC_CR_A1IE              0x01U  /* Alarm 1 Interrupt Enable */
#define  RTC_CR_A2IE              0x02U  /* Alarm 2 Interrupt Enable */
#define  RTC_CR_TIE               0x04U  /* Timer Interrupt Enable */
#define  RTC_CR_DIE               0x08U  /* Digital (DIN) Interrupt Enable */
#define  RTC_CR_PFAILE            0x20U  /* Power-Fail Interrupt Enable  */
#define  RTC_CR_DOSF              0x40U  /* Disable Oscillator Flag */
/*---------- Reset register -------------*/
#define  RTC_RESET_SWRST          0x01U  /* Active-High Software Reset Bit */
/*-------- Config 1 register ------------*/
#define  RTC_CONFIG1_ENOSC        0x01U  /* Active-High Enable for Oscillator */
#define  RTC_CONFIG1_I2C_TIMEOUT  0x02U  /* I2C Timeout Enable*/
#define  RTC_CONFIG1_DATA_RATE    0x04U  /* Data Retention Mode Enable/Disable */
#define  RTC_CONFIG1_EN_IO        0x08U  /* Disables All Open-Drain I/Os (SDA, SCL, INTAb/CLKIN) when running on VBAT. No effect when running on VCC. */
/*-------- Config 2 register ------------*/
#define  RTC_CONFIG2_CLKIN_HZ0    0x01U  /* Set Input Clock Frequency on INTA/CLKIN */
#define  RTC_CONFIG2_CLKIN_HZ1    0x02U  /* Set Input Clock Frequency on INTA/CLKIN */
#define  RTC_CONFIG2_ENCLKIN      0x04U  /* CLKIN Enable */
#define  RTC_CONFIG2_DIP          0x08U  /* Digital (DIN) Interrupt Polarity */
#define  RTC_CONFIG2_CLKO_HZ0     0x20U  /* Set Output Clock Frequency on INTB/CLKOUT */
#define  RTC_CONFIG2_CLKO_HZ1     0x40U  /* Set Output Clock Frequency on INTB/CLKOUT */
#define  RTC_CONFIG2_ENCLKO       0x80U  /* CLKO Enable */
/*------ Timer config register ----------*/
#define  RTC_TIMER_CONFIG_TFS0    0x01U  /* Timer Frequency Selection */
#define  RTC_TIMER_CONFIG_TFS1    0x02U  /* Timer Frequency Selection */
#define  RTC_TIMER_CONFIG_TRPT    0x04U  /* Timer Repeat Mode */
#define  RTC_TIMER_CONFIG_TPAUSE  0x08U  /* Timer Pause */
#define  RTC_TIMER_CONFIG_TE      0x10U  /* Timer Enable */
/*------------------------- Macros -------------------------------------*/
/*-------------------------- Enum --------------------------------------*/
typedef enum
{
  RTC_SR,
  RTC_CR,
  RTC_RESET,
  RTC_CONFIG1,
  RTC_CONFIG2,
  RTC_TIMER_CONFIG,
  RTC_SECONDS,
  RTC_MINUTES,
  RTC_HOURS,
  RTC_WDAY,
  RTC_DAY,
  RTC_MONTH,
  RTC_YEAR,
  RTC_ALARM1_SECONDS,
  RTC_ALARM1_MINUTES,
  RTC_ALARM1_HOURS,
  RTC_ALARM1_DAY,
  RTC_ALARM1_MONTH,
  RTC_ALARM1_YEAR,
  RTC_ALARM2_MINUTES,
  RTC_ALARM2_HOURS,
  RTC_ALARM2_DAY,
  RTC_TIMER_COUNTER,
  RTC_TIMER_INIT,
  RTC_PWR_MGMT,
  RTC_TRICKLE,
  RTC_RAM0
} RTC_REGISTER;
/*----------------------- Structures -----------------------------------*/
/*------------------------ Functions -----------------------------------*/
/*----------------------------------------------------------------------*/
#endif /* INC_RTC_MAX31329_H_ */
