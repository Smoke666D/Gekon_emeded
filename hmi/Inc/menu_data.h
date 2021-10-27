/*
 * menu_data.h
 *
 *  Created on: 22 окт. 2020 г.
 *      Author: igor.dymov
 */

#ifndef INC_MENU_DATA_H_
#define INC_MENU_DATA_H_

#include "rtc.h"

#include "../../hmi/Inc/data_manager.h"
#include "../../hmi/Inc/menu.h"


#define MENU_LEVEL1_COUNT      7U
#define ENGINE_MENU_COUNT      3U
#define NET_MENU_COUNT         3U
#define GENERATOR_MENU_COUNT   9U
#define SETTINGS_MENU_COUNT    2U
#define YESNO_MENU_COUNT       1U
#define EVENT_MENU_COUNT       1U
#define ABOUT_MENU_COUNT       3U
#define ALARM_MENU_COUNT       1U
#define PASSWORD_MENU_COUNT    1U
#define MESSAGE_MENU_COUNT     1U
#define LO                     1U

 extern xScreenType   xEventScreens[EVENT_MENU_COUNT];
 extern xScreenType  xSettingsScreens[SETTINGS_MENU_COUNT];

 extern xScreenType   xYesNoScreens[SETTINGS_MENU_COUNT];

 extern xScreenType  xScreensLev1[MENU_LEVEL1_COUNT];

 extern xScreenType  xAlarmScreens[ALARM_MENU_COUNT];
 extern xScreenType  xEngineScreens[ENGINE_MENU_COUNT];
 extern xScreenType xGeneratorScreens[GENERATOR_MENU_COUNT];
 extern xScreenType xNetScreens[NET_MENU_COUNT];
 extern xScreenSetObject xSettingsMenu;
 extern xScreenSetObject xMainMenu;
 extern xScreenSetObject xAboutMenu;
 extern xScreenSetObject xAlarmMenu;
 extern xScreenSetObject xEventMenu;
 extern xScreenSetObject xMainMenu;
 extern xScreenSetObject xGeneratorMenu;
 extern xScreenSetObject xEngineMenu;
 extern xScreenSetObject xNetMenu;
 extern xScreenSetObject xSettingsMenu;
 extern xScreenSetObject xPasswordMenu;
 extern xScreenSetObject xMessageMenu;
 extern xScreenSetObject xYesNoMenu;
#endif /* INC_MENU_DATA_H_ */
