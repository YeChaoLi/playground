/**
 * @file ui_screen_error.h
 *
 */

#ifndef UI_SCREEN_ERROR_H
#define UI_SCREEN_ERROR_H

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
 * Create the error screen.
 */
void ui_screen_error_init(void);

/**
 * Get the error screen object
 * @return error screen object
 */
lv_obj_t * ui_screen_error(void);

/**
 * Get the error screen footer object
 * @return error screen footer object
 */
ui_footer_t *ui_footer_error(void);

/**
 * Show an error.
 * @param   title   Error title
 * @param   desc    Error description
 * @param   code    Error code
 * @param   link    Link text for QR code
 */
void show_error(const char * title, const char * desc, int32_t code, const char * link);


/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_SCREEN_ERROR_H*/
