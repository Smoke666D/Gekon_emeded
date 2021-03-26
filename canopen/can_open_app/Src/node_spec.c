/*
 * node_spec.c
 *
 *  Created on: 26 мар. 2021 г.
 *      Author: 45
 */

#include "node_spec.h"
#include "can_timer.h"
#include "can_drv.h"

/******************************************************************************
* PRIVATE DEFINES
******************************************************************************/

/* Define some default values for our CANopen node: */
#define APP_NODE_ID       1u                  /* CANopen node ID             */
#define APP_BAUDRATE      250000u             /* CAN baudrate                */
#define APP_TMR_N         16u                 /* Number of software timers   */
#define APP_TICKS_PER_SEC 1000u               /* Timer clock frequency in Hz */
#define APP_OBJ_N         128u                /* Object dictionary max size  */

/******************************************************************************
* PRIVATE VARIABLES
******************************************************************************/

/* allocate global variables for runtime value of objects */
static uint8_t  Obj1001_00_08 = 0;

static uint32_t Obj2100_01_20 = 0;
static uint8_t  Obj2100_02_08 = 0;
static uint8_t  Obj2100_03_08 = 0;

/* define the static object dictionary */
static struct CO_OBJ_T ClockOD[APP_OBJ_N] = {
    {CO_KEY(0x1000, 0, CO_UNSIGNED32|CO_OBJ_D__R_), 0, (uintptr_t)0},
    {CO_KEY(0x1001, 0, CO_UNSIGNED8 |CO_OBJ____R_), 0, (uintptr_t)&Obj1001_00_08},
    {CO_KEY(0x1005, 0, CO_UNSIGNED32|CO_OBJ_D__R_), 0, (uintptr_t)0x80},
    {CO_KEY(0x1017, 0, CO_UNSIGNED16|CO_OBJ_D__R_), 0, (uintptr_t)0},

    {CO_KEY(0x1018, 0, CO_UNSIGNED8 |CO_OBJ_D__R_), 0, (uintptr_t)4},
    {CO_KEY(0x1018, 1, CO_UNSIGNED32|CO_OBJ_D__R_), 0, (uintptr_t)0},
    {CO_KEY(0x1018, 2, CO_UNSIGNED32|CO_OBJ_D__R_), 0, (uintptr_t)0},
    {CO_KEY(0x1018, 3, CO_UNSIGNED32|CO_OBJ_D__R_), 0, (uintptr_t)0},
    {CO_KEY(0x1018, 4, CO_UNSIGNED32|CO_OBJ_D__R_), 0, (uintptr_t)0},

    {CO_KEY(0x1200, 0, CO_UNSIGNED8 |CO_OBJ_D__R_), 0, (uintptr_t)2},
    {CO_KEY(0x1200, 1, CO_UNSIGNED32|CO_OBJ_DN_R_), 0, CO_COBID_SDO_REQUEST()},
    {CO_KEY(0x1200, 2, CO_UNSIGNED32|CO_OBJ_DN_R_), 0, CO_COBID_SDO_RESPONSE()},

    {CO_KEY(0x1800, 0, CO_UNSIGNED8 |CO_OBJ_D__R_), 0, (uintptr_t)2},
    {CO_KEY(0x1800, 1, CO_UNSIGNED32|CO_OBJ_DN_R_), 0, CO_COBID_TPDO_DEFAULT(0)},
    {CO_KEY(0x1800, 2, CO_UNSIGNED8 |CO_OBJ_D__R_), 0, (uintptr_t)254},

    {CO_KEY(0x1A00, 0, CO_UNSIGNED8 |CO_OBJ_D__R_), 0, (uintptr_t)3},
    {CO_KEY(0x1A00, 1, CO_UNSIGNED32|CO_OBJ_D__R_), 0, CO_LINK(0x2100, 0x01, 32)},
    {CO_KEY(0x1A00, 2, CO_UNSIGNED32|CO_OBJ_D__R_), 0, CO_LINK(0x2100, 0x02,  8)},
    {CO_KEY(0x1A00, 3, CO_UNSIGNED32|CO_OBJ_D__R_), 0, CO_LINK(0x2100, 0x03,  8)},

    {CO_KEY(0x2100, 0, CO_UNSIGNED8 |CO_OBJ_D__R_), 0, (uintptr_t)3},
    {CO_KEY(0x2100, 1, CO_UNSIGNED32|CO_OBJ___PR_), 0, (uintptr_t)&Obj2100_01_20},
    {CO_KEY(0x2100, 2, CO_UNSIGNED8 |CO_OBJ___PR_), 0, (uintptr_t)&Obj2100_02_08},
    {CO_KEY(0x2100, 3, CO_UNSIGNED8 |CO_OBJ___PR_), CO_TASYNC, (uintptr_t)&Obj2100_03_08},

    CO_OBJ_DIR_ENDMARK  /* mark end of used objects */
};

/* Each software timer needs some memory for managing
 * the lists and states of the timed action events.
 */
static CO_TMR_MEM TmrMem[APP_TMR_N];

/* Each SDO server needs memory for the segmented or
 * block transfer requests.
 */
static uint8_t SdoSrvMem[CO_SDOS_N * CO_SDO_BUF_BYTE];

/* Select the drivers for your application. For possible
 * selections, see the directory /drivers. In this example
 * we select the driver templates. You may fill them with
 * your specific hardware functionality.
 */
static struct CO_IF_DRV_T AppDriver = {
    &STM32F2CanDriver,
    &STM32F2TimerDriver,
    NULL
};

/* Specify the EMCY error codes with the corresponding
 * error register bit. There is a collection of defines
 * for the predefined emergency codes CO_EMCY_CODE...
 * and for the error register bits CO_EMCY_REG... for
 * readability. You can use plain numbers, too.
 */
static CO_EMCY_TBL AppEmcyTbl[APP_ERR_ID_NUM] = {
    { CO_EMCY_REG_GENERAL, CO_EMCY_CODE_GEN_ERR          }, /* APP_ERR_CODE_SOMETHING */
    { CO_EMCY_REG_TEMP   , CO_EMCY_CODE_TEMP_AMBIENT_ERR }  /* APP_ERR_CODE_HAPPENS   */
};

/******************************************************************************
* PUBLIC VARIABLES
******************************************************************************/

/* Collect all node specification settings in a single
 * structure for initializing the node easily.
 */
struct CO_NODE_SPEC_T AppSpec = {
    APP_NODE_ID,             /* default Node-Id                */
    APP_BAUDRATE,            /* default Baudrate               */
    &ClockOD[0],             /* pointer to object dictionary   */
    APP_OBJ_N,               /* object dictionary max length   */
    &AppEmcyTbl[0],          /* EMCY code & register bit table */
    &TmrMem[0],              /* pointer to timer memory blocks */
    APP_TMR_N,               /* number of timer memory blocks  */
    APP_TICKS_PER_SEC,       /* timer clock frequency in Hz    */
    &AppDriver,              /* select drivers for application */
    &SdoSrvMem[0]            /* SDO Transfer Buffer Memory     */
};
