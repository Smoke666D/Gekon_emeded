/*
 * data_manager.h
 *
 *  Created on: 4 мар. 2020 г.
 *      Author: igor.dymov
 */

#ifndef INC_DATA_MANAGER_H_
#define INC_DATA_MANAGER_H_


#define READ_COMMNAD  0x00
#define INC_COMMAND   0x01
#define DEC_COMMNAD   0x02
#define ENTER_COMMAND 0x03
#define ESC_COMMNAD	  0x04

#define ID_TEST_DATA   0
#define ID_RTC_DATA    1


void vdmGetData(uint8_t command, uint16_t DataID,uint8_t * pchTextString);

#endif /* INC_DATA_MANAGER_H_ */
