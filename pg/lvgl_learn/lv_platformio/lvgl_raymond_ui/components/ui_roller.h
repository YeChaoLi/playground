/**
 * @file ui_roller.h
 *
 */

#ifndef UI_ROLLER_H
#define UI_ROLLER_H

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
 * Create a roller inside a container
 * @param parent The parent of the roller object
 * @return the roller object
 */
lv_obj_t * ui_roller_create(lv_obj_t * parent, bool temp, lv_subject_t * subject);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_ROLLER_H*/
