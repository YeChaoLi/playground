/**
 * @file ui_screen_heat_cool.h
 *
 */

#ifndef UI_SCREEN_HEAT_COOL_H
#define UI_SCREEN_HEAT_COOL_H

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
void ui_screen_heat_cool_init(void);

/**
 * Get the heat cool screen object
 * @return heat cool screen object
 */
lv_obj_t * ui_screen_heat_cool(void);

/**
 * Get the heat cool screen footer object
 * @return heat cool screen footer object
 */
ui_footer_t *ui_footer_heat_cool(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_SCREEN_HEAT_COOL_H*/
