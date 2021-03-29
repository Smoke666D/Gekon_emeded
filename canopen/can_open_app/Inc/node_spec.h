/*
 * node_spec.h
 *
 *  Created on: 26 мар. 2021 г.
 *      Author: 45
 */

#ifndef INC_NODE_SPEC_H_
#define INC_NODE_SPEC_H_

#include "co_core.h"

/******************************************************************************
* PUBLIC DEFINES
******************************************************************************/

#define   DEV_POF_NUMBER    0x194  /* Стандартный профиль 404 Messuring devicr */
#define   DI_BLOCK          0x1000
#define   AI_BLOCK          0x2000
#define   DO_BLOCK          0x4000
#define   AO_BLOCK          0x8000
#define   CNTR_BLOCK        0x10000
#define   ALARM_BLOCK       0X20000

/* Specify the EMCY-IDs for the application */
enum EMCY_CODES {
    APP_ERR_ID_SOMETHING = 0,
    APP_ERR_ID_HOT,

    APP_ERR_ID_NUM            /* number of EMCY error codes in application */
};

/******************************************************************************
* PUBLIC SYMBOLS
******************************************************************************/

extern struct CO_NODE_SPEC_T AppSpec;

#endif /* INC_NODE_SPEC_H_ */
