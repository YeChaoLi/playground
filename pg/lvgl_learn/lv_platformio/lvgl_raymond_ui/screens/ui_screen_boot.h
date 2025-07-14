/**
 * @file ui_screen_boot.h
 *
 */

#ifndef UI_SCREEN_BOOT_H
#define UI_SCREEN_BOOT_H

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
 * Create the boot screen.
 */
void ui_screen_boot_init(void);

/**
 * Get the boot screen object
 * @return boot screen object
 */
lv_obj_t * ui_screen_boot(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_SCREEN_BOOT_H*/
