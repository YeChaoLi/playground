/**
 * @file ui_screen_door.h
 *
 */

#ifndef UI_SCREEN_DOOR_H
#define UI_SCREEN_DOOR_H

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
 * Create the door screen.
 */
void ui_screen_door_init(void);

/**
 * Get the door screen object
 * @return door screen object
 */
lv_obj_t * ui_screen_door(void);

/**
 * Get the door screen footer object
 * @return door screen footer object
 */
ui_footer_t *ui_footer_door(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_SCREEN_DOOR_H*/
