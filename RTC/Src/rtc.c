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
#include "server.h"
#include "data_type.h"
#include "FreeRTOS.h"
#include "task.h"
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
char 							time[9];
/*----------------------- Functions -----------------------------------------------------------------*/
RTC_STATUS 	eRTCtxtTimeParser( char* input, char *header, char* output );												/* Parsing text of incoming message from time server */
RTC_STATUS 	eRTCdatetimeParser( char* input, RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate );	/* Parsing time server time-date string */
uint8_t 		eRTCgetInt( char* input, uint8_t start, uint8_t length );														/* Get uint8_t from part of string */
RTC_STATUS 	eRTCparsingHttpTime( char* data, RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate );	/* Parsing full message from time server */
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------------------------*/
/* Put the pointer to the current RTC structure
 * Put it after HAL initialization
 * Input: 	RTC HAL structure
 * Output:	none
 */
void vRTCputTimer( RTC_HandleTypeDef *hrtc )
{
	rtc = hrtc;
	return;
}

void vRTCGetTime(DATA_COMMNAD_TYPE cmd, char * Time)
{
	RTC_TimeTypeDef 	t;
	Time[0]= 0;
	switch (cmd)
	{
	case mREAD:

		if ( HAL_RTC_GetTime( rtc, &t, RTC_FORMAT_BCD ) == HAL_OK )
		{
			Time[0]= (t.Hours>>4)+'0';
			Time[1]= (t.Hours & 0x0F)+'0';
			Time[2] = ':';
			Time[3]= (t.Minutes>>4)+'0';
			Time[4]= (t.Minutes & 0x0F)+'0';
			Time[5] = ':';
			Time[6]= (t.Seconds>>4)+'0';
			Time[7]= (t.Seconds & 0x0F)+'0';
			Time[8] = 0;
		}
	default:
		break;
	}
}

static uint8_t CorrectData =0,CorrectPos=0;
static char buffer;
static char MaxData;
void vRTCCorrectTime(DATA_COMMNAD_TYPE cmd,char *Time, uint8_t TimePosition)
{
    uint8_t DataToRead=0;
	RTC_TimeTypeDef 	t;
	Time[0]= 0;
	if ((!CorrectData) ||  (TimePosition != CorrectPos))
		DataToRead =TimePosition;
	switch (cmd)
	{
	case mREAD:
		if ( HAL_RTC_GetTime( rtc, &t, RTC_FORMAT_BIN ) == HAL_OK )
		{
			switch (DataToRead)
			{
				case RTC_MIN:
					Time[0]= (t.Minutes/10)+'0';
					Time[1]= (t.Minutes%10)+'0';
					Time[2] = 0;
					break;
				case RTC_HOUR:
					Time[0]= (t.Hours/10)+'0';
					Time[1]= (t.Hours%10)+'0';
					Time[2] = 0;
					break;
				case RTC_SEC:
					Time[0]= (t.Seconds/10)+'0';
					Time[1]= (t.Seconds%10)+'0';
					Time[2] = 0;
					break;
				case 0:
					Time[0]= (buffer/10)+'0';
					Time[1]= (buffer%10)+'0';
					Time[2] = 0;
					break;
			}
		}
		break;
	case mINC:
	case mDEC:
		if ((CorrectData) && (CorrectPos!=TimePosition))
			CorrectData=0;
		if (!CorrectData)
		{
			CorrectData =1;
			CorrectPos=TimePosition;
			HAL_RTC_GetTime( rtc, &t, RTC_FORMAT_BIN );
			switch (TimePosition)
			{
				case RTC_MIN:
						buffer = t.Minutes;
						MaxData = 59;
						break;
				case RTC_HOUR:
						buffer =  t.Hours;
						MaxData = 23;
						break;
				case RTC_SEC:
					    buffer = t.Seconds;
					    MaxData = 59;
					    break;
			}
		}
		switch (cmd)
		{
		  case mINC:
			  if (buffer<MaxData) buffer++;
			  break;
		  case mDEC:
			  if (buffer) buffer--;
			  break;

		}
		break;
	case mSAVE:
		HAL_RTC_GetTime( rtc, &t, RTC_FORMAT_BIN);
		switch (TimePosition)
		{
				case RTC_MIN:
					t.Minutes =buffer;
				    break;
				case RTC_HOUR:
					t.Hours = buffer;
					break;
				case RTC_SEC:
			        t.Seconds = buffer;
					break;
	    }
		HAL_RTC_SetTime(rtc, &t,RTC_FORMAT_BIN);
	default:
		CorrectData =0;
		buffer=0;
		break;
	}

}



/*---------------------------------------------------------------------------------------------------*/
/* Get the date string for HTTP requests, example:
 * "Thu, 06 Feb 2020 15:11:53 GMT"
 * Input: 	buffer for output
 * Output:	status of operation
 */
RTC_STATUS eRTCgetDateForHttp( char* date )
{
	RTC_STATUS 				res = RTC_OK;
	RTC_TimeTypeDef 	t;
	RTC_DateTypeDef		d;
	uint8_t						i   = 0U;

	if ( HAL_RTC_GetTime( rtc, &t, RTC_FORMAT_BCD ) == HAL_OK )
	{
		if ( HAL_RTC_GetDate( rtc, &d, RTC_FORMAT_BCD ) == HAL_OK )
	  {
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
/*
 * Get simple time string, example:
 * hh.mm.ss
 * input:		buffer for string. Length = 8
 * output:	status of operation
 */
RTC_STATUS eRTCgetTime( char* out )
{
	RTC_STATUS				rtcRes 	= RTC_OK;
	uint8_t i = 0U;

	for( i=0U; i<8; i++ )
	{
		out[i] = time[i];
	}

	return rtcRes;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Get time from time server
 * input:		none
 * output:	status of operation
 */
RTC_STATUS eRTCgetExtrenalTime( void )
{
	RTC_STATUS				rtcRes = RTC_ERROR;
	RTC_TimeTypeDef 	t;
	RTC_DateTypeDef		d;
	char*							data   = pvPortMalloc( 200U );
	HTTP_RESPONSE			response;

	HTTP_REQUEST request = {
			.method = HTTP_METHOD_GET,
			.path   = TIME_PATH,
			.host   = TIME_HOST,
			.cache  = HTTP_CACHE_NO_CACHE,
	};
	if ( eHTTPrequest( &request, &response, data ) == HTTP_STATUS_OK )
	{
		if ( eRTCparsingHttpTime( data, &t, &d ) == RTC_OK )
		{
			if ( HAL_RTC_SetTime( rtc, &t, RTC_FORMAT_BCD ) == HAL_OK )
			{
				if ( HAL_RTC_SetDate( rtc, &d, RTC_FORMAT_BCD ) == HAL_OK )
				{
					rtcRes = RTC_OK;
					sprintf( time, "%02d:%02d:%02d", t.Hours, t.Minutes, t.Seconds );
				}
			}
		}
	}
	vPortFree( data );
	return rtcRes;
}
/*---------------------------------------------------------------------------------------------------*/
/*
 * Parsing text of incoming message from time server.
 * input:		input  - message from time server
 * 					header - header of parsing information
 * 					output - parsed data
 * output:	status of operation
 */
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
/*
 * Get uint8_t from part of string
 * input:		input  - input string
 * 					start  - number of start converting char
 * 					length - length of converting chars
 * output: 	uint8_t data from string
 */
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
/*
 * Parsing time server time-date string.
 * input:		input - input string from time server
 * 					sTime - HAL structure of RTC
 * 					sDate - HAL structure of RTC
 * output:	status of operation
 */
RTC_STATUS eRTCdatetimeParser( char* input, RTC_TimeTypeDef *sTime, RTC_DateTypeDef *sDate  )
{
	RTC_STATUS	rtcRes 	= RTC_ERROR;

	/*	2020-02-12T15:07:06.182854+03:00  */
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
/*
 * Parsing full message from time server
 * input:		data  - input message from time server
 * 					sTime - HAL structure of RTC
 * 					sDate - HAL structure of RTC
 * output:	status of operation
 */
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
