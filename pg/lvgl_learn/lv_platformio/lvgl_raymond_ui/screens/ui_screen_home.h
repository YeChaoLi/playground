/**
 * @file ui_screen_home.h
 *
 */

#ifndef UI_SCREEN_HOME_H
#define UI_SCREEN_HOME_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"
#include "../components/ui_footer.h"

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
 * Create the home screen.
 */
void ui_screen_home_init(void);

/**
 * Get the home screen object
 * @return home screen object
 */
lv_obj_t * ui_screen_home(void);

/**
 * Get the home screen footer object
 * @return home screen footer object
 */
ui_footer_t *ui_footer_home(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_SCREEN_HOME_H*/
