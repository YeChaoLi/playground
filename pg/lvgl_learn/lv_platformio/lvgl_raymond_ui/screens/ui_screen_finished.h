/**
 * @file ui_screen_finished.h
 *
 */

#ifndef UI_SCREEN_FINISHED_H
#define UI_SCREEN_FINISHED_H

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
 * Create the finished screen.
 */
void ui_screen_finished_init(void);

/**
 * Get the finished screen object
 * @return finished screen object
 */
lv_obj_t * ui_screen_finished(void);

/**
 * Get the finished screen footer object
 * @return finished screen footer object
 */
ui_footer_t *ui_footer_finished(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_SCREEN_FINISHED_H*/
