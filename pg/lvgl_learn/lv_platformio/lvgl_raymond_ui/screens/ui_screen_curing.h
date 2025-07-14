/**
 * @file ui_screen_curing.h
 *
 */

#ifndef UI_SCREEN_CURING_H
#define UI_SCREEN_CURING_H

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

 #define PROGRESS_BAR_MAX 1000

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Create the curing screen.
 */
void ui_screen_curing_init(void);

/**
 * Get the curing screen object
 * @return curing screen object
 */
lv_obj_t * ui_screen_curing(void);

/**
 * Get the curing screen footer object
 * @return curing screen footer object
 */
ui_footer_t *ui_footer_curing(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_SCREEN_CURING_H*/
