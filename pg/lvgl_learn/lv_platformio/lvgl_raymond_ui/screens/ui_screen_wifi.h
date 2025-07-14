/**
 * @file ui_screen_wifi.h
 *
 */

#ifndef UI_SCREEN_WIFI_H
#define UI_SCREEN_WIFI_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create the wifi screen.
 */
void ui_screen_wifi_init(void);

/**
 * Get the wifi screen object
 * @return wifi screen object
 */
lv_obj_t * ui_screen_wifi(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_SCREEN_WIFI_H*/
