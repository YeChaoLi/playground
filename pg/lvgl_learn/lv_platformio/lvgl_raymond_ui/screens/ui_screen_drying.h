/**
 * @file ui_screen_drying.h
 *
 */

#ifndef UI_SCREEN_DRYING_H
#define UI_SCREEN_DRYING_H

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
 * Create the heat cool screen.
 */
void ui_screen_drying_init(void);

/**
 * Get the heat cool screen object
 * @return heat cool screen object
 */
lv_obj_t * ui_screen_drying(void);

/**
 * Get the heat cool screen footer object
 * @return heat cool screen footer object
 */
ui_footer_t *ui_footer_drying(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_SCREEN_DRYING_H*/
