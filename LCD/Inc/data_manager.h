/*
 * data_manager.h
 *
 *  Created on: 4 мар. 2020 г.
 *      Author: igor.dymov
 */

#ifndef INC_DATA_MANAGER_H_
#define INC_DATA_MANAGER_H_


#define READ_COMMNAD  0x00

#define ID_TEST_DATA   0
#define ID_RTC_DATA    1


void vdmGetData(uint8_t command, uint16_t DataID,uint8_t * pchTextString);

#endif /* INC_DATA_MANAGER_H_ */
