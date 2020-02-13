/*
 * rtc.c
 *
 *  Created on: Feb 12, 2020
 *      Author: mikhail.mikhailov
 */
/*----------------------- Includes ------------------------------------------------------------------*/
#include "rtc.h"


#include "string.h"
#include "stdio.h"
#include "stdlib.h"

#include "http.h"
#include "server.h"
/*----------------------- Structures ----------------------------------------------------------------*/

/*----------------------- Constant ------------------------------------------------------------------*/
const char *wkdayList[7]  = { "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun" };
const char *monthList[12] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
const char *dayList[31]   = { "01", "02", "03", "04", "05", "06", "07", "08", "09", "10",
															"11", "12", "13", "14", "15", "16", "17", "18", "19", "20",
															"21", "22", "23", "24", "25", "26", "27", "28", "29", "30", "31" };
/*----------------------- Variables -----------------------------------------------------------------*/
RTC_HandleTypeDef *rtc;

uint8_t 					utc_offset;
SIGN 							utc_offset_sign;
uint8_t						day_of_week;
TIME_API_LOCATION	location;
//HAL_StatusTypeDef	halRes;
/*----------------------- Functions -----------------------------------------------------------------*/
RTC_STATUS 	eRTCtxtTimeParser( char* input, char *header, char* output );
RTC_STATUS 	eRTCdatetimeParser( char* input, RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate );
uint8_t 		eRTCgetInt( char* input, uint8_t start, uint8_t length );
RTC_STATUS 	eRTCparsingHttpTime( char* data, RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate );
/*---------------------------------------------------------------------------------------------------*/
void vRTCgetTimer( RTC_HandleTypeDef *hrtc )
{
	rtc = hrtc;
	return;
}
/*---------------------------------------------------------------------------------------------------*/
RTC_STATUS eRTCgetDateForHttp( char* date )
{
	/*  Thu, 06 Feb 2020 15:11:53 GMT  */

	RTC_STATUS 				res = RTC_OK;
	RTC_TimeTypeDef 	t;
	RTC_DateTypeDef		d;
	uint8_t						i   = 0U;

	if ( HAL_RTC_GetTime( rtc, &t, RTC_FORMAT_BCD ) == HAL_OK )
	{
		if ( HAL_RTC_GetDate( rtc, &d, RTC_FORMAT_BCD ) == HAL_OK )
	  {
			// Week day
			for( i=0U; i<3; i++ )
	  	{
				date[i] = wkdayList[day_of_week][i];
	  	}
			date[3U] = ',';
			date[4U] = ' ';
			for( i=0U; i<2U; i++ )
			{
				date[5U + i] = dayList[d.Date][i];
			}
			date[7U] = ' ';
			for( i=0U; i<3U; i++ )
			{
				date[8U + i] = monthList[d.Month][i];
			}
			date[11U] = ' ';
			itoa( d.Year, &date[12U], 10U );
			date[16U] = ' ';
			itoa( t.Hours, &date[17U], 10U );
			date[19U] = ':';
			itoa( t.Minutes, &date[20U], 10U );
			date[22U] = ':';
			itoa( t.Seconds, &date[23U], 10U );
			date[25U] = ' ';
			date[26U] = 'G';
			date[27U] = 'M';
			date[28U] = 'T';
	  }
	}
	return res;
}
/*---------------------------------------------------------------------------------------------------*/
RTC_STATUS eRTCgetHttpTime( void )
{
	HTTP_STATUS 			httpRes	= HTTP_STATUS_BAD_REQUEST;
	RTC_STATUS				rtcRes 	= RTC_OK;
	RTC_TimeTypeDef 	t;
	RTC_DateTypeDef		d;
	char*							buffer = pvPortMalloc(HTTP_BUFER_SIZE);
	char*							data   = pvPortMalloc(200U);
	HTTP_RESPONSE			response;

	HTTP_REQUEST request = {
			.method = HTTP_METHOD_GET,
			.path   = TIME_PATH,
			.host   = TIME_HOST,
			.cache  = HTTP_CACHE_NO_CACHE,
	};

	httpRes = eHTTPmakeRequest( buffer, request );
	eHTTPsendRequest( buffer, request.host );
	httpRes = eHTTPparsingResponse( buffer, data, &response );

	if ( httpRes == HTTP_STATUS_OK )
	{

		rtcRes = eRTCparsingHttpTime( data, &t, &d );
		if ( HAL_RTC_SetTime( rtc, &t, RTC_FORMAT_BCD ) == HAL_OK )
		{
			if ( HAL_RTC_SetDate( rtc, &d, RTC_FORMAT_BCD ) != HAL_OK )
			{
				rtcRes = RTC_ERROR;
			}
		}
		else rtcRes = RTC_ERROR;
	}
	else rtcRes = RTC_ERROR;

	vPortFree( buffer );
	vPortFree( data );
	return rtcRes;
}
/*---------------------------------------------------------------------------------------------------*/
RTC_STATUS eRTCtxtTimeParser( char* input, char *header, char* output)
{
	RTC_STATUS	rtcRes 	= RTC_OK;
	char*				pchSt		= NULL;
	char*				pchEn  	= NULL;

	pchSt = strstr( input, header );
	if( pchSt != NULL )
	{
		pchSt = strchr( pchSt, ':' ) + 2U;
		if ( pchSt != NULL )
		{
			pchEn = strchr( pchSt, LF_HEX );
			if ( pchEn != NULL )
			{
				if ( strncpy( output, ( pchSt ), ( pchEn - pchSt ) ) == NULL )
				{
					rtcRes = RTC_ERROR;
				}
			}
			else rtcRes = RTC_ERROR;
		}
		else rtcRes = RTC_ERROR;
	}
	else rtcRes = RTC_ERROR;

	return rtcRes;
}
/*---------------------------------------------------------------------------------------------------*/
uint8_t eRTCgetInt( char* input, uint8_t start, uint8_t length )
{
	uint8_t		output	 	= 0xFFU;
	char*			pchSt	 	 	= NULL;
	char*			pchEn  	 	= NULL;
	char			buffer[4] = { 0x00, 0x00, 0x00, 0x00 };

	pchSt	 = input + start;
	pchEn  = pchSt + length;
	if( strncpy( buffer, ( pchSt ), ( pchEn - pchSt ) ) != NULL )
	{
		output = atoi( buffer );
	}
	return output;
}

/*---------------------------------------------------------------------------------------------------*/
RTC_STATUS eRTCdatetimeParser( char* input, RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate  )
{
	RTC_STATUS	rtcRes 	= RTC_ERROR;

	/*	2020-02-12T15:07:06.182854+03:00  */
	uint8_t temp 		= strlen( input );
	sDate->Year 		= eRTCgetInt( input, 2U,  2U);
	sDate->Month 		= eRTCgetInt( input, 5U,  2U );
	sDate->Date 		= eRTCgetInt( input, 8U,  2U );
	sTime->Hours 		= eRTCgetInt( input, 11U, 2U );
	sTime->Minutes 	= eRTCgetInt( input, 14U, 2U );
	sTime->Seconds 	= eRTCgetInt( input, 17U, 2U );
	utc_offset = eRTCgetInt( input, 27U, 2U );
	if( input[26U] == '+' )
	{
		utc_offset_sign = plus;
	}
	else
	{
		utc_offset_sign = minus;
	}

	return rtcRes;
}
/*---------------------------------------------------------------------------------------------------*/
RTC_STATUS eRTCparsingHttpTime( char* data, RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate )
{
	RTC_STATUS	rtcRes 	= RTC_OK;
	char				timeBuff[TIME_API_BUFFER_SIZE];
	char				dayBuff[2];


	rtcRes = eRTCtxtTimeParser( data, TIME_API_DATETIME_STR, timeBuff );
	if ( rtcRes != RTC_ERROR )
	{
		rtcRes = eRTCdatetimeParser( timeBuff, sTime, sDate  );
		rtcRes = eRTCtxtTimeParser( data, TIME_API_DAY_OF_WEEK_STR, dayBuff );
		if ( rtcRes != RTC_ERROR )
		{
			sDate->WeekDay = atoi( dayBuff );
		}
	}
	return rtcRes;
}
/*---------------------------------------------------------------------------------------------------*/
