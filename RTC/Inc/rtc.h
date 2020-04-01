/*
 * rtc.h
 *
 *  Created on: Feb 12, 2020
 *      Author: mikhail.mikhailov
 */

#ifndef INC_RTC_H_
#define INC_RTC_H_
/*----------------------- Includes -------------------------------------*/
#include "stm32f2xx_hal.h"
#include "data_type.h"
/*------------------------ Define --------------------------------------*/
#define		TIME_HOST									"worldtimeapi.org"
#define		TIME_PATH									"/api/ip.txt"

#define		TIME_API_SIZE							10U
#define		TIME_API_BUFFER_SIZE			32U
#define		HTTP_DATE_LENGTH					29U
#define		TIME_API_LOCATION_STR 		"abbreviation"
#define		TIME_API_DATETIME_STR 		"datetime"
#define		TIME_API_DAY_OF_WEEK_STR 	"day_of_week"
#define     RTC_MIN 1
#define 	RTC_HOUR 2
#define     RTC_SEC 3

/*------------------------- Enum ---------------------------------------*/
typedef enum
{
	MSK,
}TIME_API_LOCATION;

typedef enum
{
	DST_NO,
	DST_YES,
}TIME_API_DST;

typedef enum
{
	RTC_OK,
	RTC_ERROR,
}RTC_STATUS;

typedef enum
{
	minus,
	plus,
}SIGN;
/*----------------------- Structures -----------------------------------*/
/*

#define		TIME_API_DAY_OF_YEAR_STR 	"day_of_year"
#define		TIME_API_DST_STR 					"dst"
#define		TIME_API_DST_FROM_STR 		"dst_from"
#define		TIME_API_DST_OFFSET_STR 	"dst_offset"
#define		TIME_API_DST_UNTIL_STR 		"dst_until"
#define		TIME_API_RAW_OFFSET_STR 	"raw_offset"
#define		TIME_API_WEEK_NUMBER_STR 	"week_number"
*/
/*----------------------- Functions ------------------------------------*/
void        vRTCgetTimer( RTC_HandleTypeDef *hrtc );
void        vRTCputTimer( RTC_HandleTypeDef *hrtc );
RTC_STATUS	eRTCgetHttpTime( void );
RTC_STATUS 	eRTCgetDateForHttp( char* date );
void vRTCGetTime(DATA_COMMNAD_TYPE cmd, char * Time);
void vRTCCorrectTime(DATA_COMMNAD_TYPE cmd,char *Time, uint8_t TimePosition);
RTC_STATUS 	eRTCgetTime( char* time );
/*----------------------------------------------------------------------*/
#endif /* INC_RTC_H_ */
