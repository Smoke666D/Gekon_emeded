/*
 * tests.h
 *
 *  Created on: 12 апр. 2022 г.
 *      Author: 79110
 */

#ifndef INC_TESTS_H_
#define INC_TESTS_H_

void vTESTsendReport ( void );

void runTest_rtc ( void );
void runTest_cli ( void );
void runTest_usbhid ( void );
void runTest_eeprom ( void );
void runTest_vr ( void );
void runTets_storage ( void );
void runTets_dataAPI ( void );
void runTest_chart ( void );
void runTest_outputProcessing ( void );
void runTest_rest ( void );
void runTest_http ( void );
void runTest_dataSD ( void );

void vTESTInit ( void );

#endif /* INC_TESTS_H_ */
