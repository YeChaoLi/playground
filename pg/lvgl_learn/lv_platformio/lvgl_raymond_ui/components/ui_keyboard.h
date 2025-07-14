/**
 * @file ui_keyboard.h
 *
 */

#ifndef UI_KEYBOARD_H
#define UI_KEYBOARD_H

#ifdef __cplusplus
extern "C"
{
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
 * Creates a keyboard object.
 * @param parent The parent object of the keyboard.
 * @return Pointer to the created keyboard object.
 */
lv_obj_t *ui_keyboard_create(lv_obj_t *parent);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_KEYBOARD_H*/
