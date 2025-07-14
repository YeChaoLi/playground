/**
 * @file ui_screen_settings.h
 *
 */

#ifndef UI_SCREEN_SETTINGS_H
#define UI_SCREEN_SETTINGS_H

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
 * Create the settings screen.
 */
void ui_screen_settings_init(void);

/**
 * Get the settings screen object
 * @return settings screen object
 */
lv_obj_t * ui_screen_settings(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_SCREEN_SETTINGS_H*/
