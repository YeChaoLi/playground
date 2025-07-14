/**
 * @file ui_list_element.h
 *
 */

#ifndef UI_LIST_ELEMENT_H
#define UI_LIST_ELEMENT_H

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
 * Create a list element inside a container
 * @param parent The parent of the list element object
 * @param icon The icon of the list element
 * @param text The text on the list element
 * @return the list element object (container)
 */
lv_obj_t * ui_list_element_create(lv_obj_t * parent, const void *icon, const char * text);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_LIST_ELEMENT_H*/
