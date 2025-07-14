/**
 * @file ui_screens.h
 *
 */

#ifndef UI_SCREENS_H
#define UI_SCREENS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
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

/* ui_screen_boot.c */
void ui_screen_boot_init(void);
lv_obj_t * ui_screen_boot(void);

/* ui_screen_door.c */
void ui_screen_door_init(void);
lv_obj_t * ui_screen_door(void);
ui_footer_t *ui_footer_door(void);

/* ui_screen_finished.c */
void ui_screen_finished_init(void);
lv_obj_t * ui_screen_finished(void);
ui_footer_t *ui_footer_finished(void);

/* ui_screen_curing.c */
void ui_screen_curing_init(void);
lv_obj_t * ui_screen_curing(void);
ui_footer_t *ui_footer_curing(void);

/* ui_screen_heat_cool.c */
void ui_screen_heat_cool_init(void);
lv_obj_t * ui_screen_heat_cool(void);
ui_footer_t *ui_footer_heat_cool(void);

/* ui_screen_pre_heat.c */
void ui_screen_pre_heat_init(void);
lv_obj_t * ui_screen_pre_heat(void);

/* ui_screen_error.c */
void ui_screen_error_init(void);
lv_obj_t * ui_screen_error(void);
ui_footer_t *ui_footer_error(void);

/* ui_screen_home.c */
void ui_screen_home_init(void);
lv_obj_t *  ui_screen_home(void);
ui_footer_t *ui_footer_home(void);

/* ui_screen_settings.c */
void ui_screen_settings_init(void);
lv_obj_t * ui_screen_settings(void);

/* ui_screen_drying.c */
void ui_screen_drying_init(void);
lv_obj_t * ui_screen_drying(void);

/* ui_screen_wifi.c */
void ui_screen_wifi_init(void);
lv_obj_t * ui_screen_wifi(void);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_SCREENS_H*/
