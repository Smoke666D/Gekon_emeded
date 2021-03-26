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
