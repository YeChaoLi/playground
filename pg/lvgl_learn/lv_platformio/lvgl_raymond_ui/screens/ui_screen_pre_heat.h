/**
 * @file ui_screen_pre_heat.h
 *
 */

#ifndef UI_SCREEN_PRE_HEAT_H
#define UI_SCREEN_PRE_HEAT_H

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
 * Create the pre heat screen
 */
void ui_screen_pre_heat_init(void);

/**
 * Get the pre heat screen object
 * @return pre heat screen object
 */
lv_obj_t * ui_screen_pre_heat(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_SCREEN_PRE_HEAT_H*/
