/**
 * @file ui_switch.h
 *
 */

#ifndef UI_SWITCH_H
#define UI_SWITCH_H

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
 * Create a switch
 * @param parent The parent of the switch object
 * @return the switch object
 */
lv_obj_t * ui_switch_create(lv_obj_t * parent);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_SWITCH_H*/
