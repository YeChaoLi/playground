/**
 * @file ui.h
 *
 */

#ifndef UI_H
#define UI_H

#ifdef __cplusplus
extern "C"
{
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lvgl/lvgl.h"
#include "lvgl/lvgl_private.h"
#include "screens/ui_screen_home.h"
#include "screens/ui_screen_curing.h"
#include "screens/ui_screen_finished.h"
#include "screens/ui_screen_boot.h"
#include "screens/ui_screen_door.h"
#include "screens/ui_screen_settings.h"
#include "screens/ui_screen_error.h"
#include "screens/ui_screen_pre_heat.h"
#include "screens/ui_screen_drying.h"
#include "screens/ui_screen_wifi.h"
#include "components/ui_footer.h"
/*********************
 *      DEFINES
 *********************/

#define UI_SCREEN_WIDTH 1280
#define UI_SCREEN_HEIGHT 400

#define OBSERVER_TEXT_BUFFER_SIZE 50

/* Colors */
#define COLOR_BACKGROUND lv_color_black()
#define COLOR_SURFACE lv_color_hex(0x1C1C1C)
#define COLOR_NOTIFICATION lv_color_hex(0x222222)
#define COLOR_LINE lv_color_hex(0x333639)

#define COLOR_INTERACTIVE_ACTIVE lv_color_hex(0x1976D2)
#define COLOR_INTERACTIVE_PRESSED lv_color_hex(0x0F477E)
#define COLOR_INTERACTIVE_DISABLED lv_color_hex(0x616161)
#define COLOR_INTERACTIVE_SECONDARY lv_color_hex(0x808080)

#define COLOR_TEXT_PRIMARY lv_color_hex(0xE5E5E5)
#define COLOR_TEXT_SECONDARY lv_color_hex(0xB2B2B2)

#define COLOR_STATUS_POSITIVE lv_color_hex(0x03AA08)
#define COLOR_STATUS_WARNING lv_color_hex(0xFFA81E)
#define COLOR_STATUS_NEGATIVE lv_color_hex(0xD21919)
#define COLOR_STATUS_NEGATIVE_PRESSED lv_color_hex(0x690D0D)

#define SCREEN_TRANSITION_MS 0 /* Screen transition time in milliseconds */

#define AUTO_TEST 0 /* O disable or 1 enable*/
#if AUTO_TEST
#define AUTO_SWITCH_INTERVAL_MS 3000 /* Time in milliseconds before switching to next screen */
#define AUTO_SWITCH_TIME_MS 500 /* Change screen animation time in milliseconds */
#define AUTO_SWITCH_ANIM LV_SCR_LOAD_ANIM_MOVE_LEFT /* Animation to use when switching screen */
#endif

/**********************
 *      TYPEDEFS
 **********************/

typedef enum
{
    SCREEN_BOOT, /* Boot screen */
    SCREEN_HOME, /* Home screen */
    SCREEN_CURE, /* Curing screen */
    SCREEN_DOOR, /* Door open screen */
    SCREEN_ERROR, /* Error screen */
    SCREEN_FINISHED, /* Finished screen */
    SCREEN_SETTINGS, /* Settings screen */
    SCREEN_PRE_HEAT, /* Pre-heat info screen */
    SCREEN_HEAT_COOL, /* Heat cool screen */
    SCREEN_DRYING, /* Heat cool screen */
    SCREEN_WIFI, /* Wi-Fi settings screen */
} screen_t;

typedef struct cure_preset {
    const char * name;
    const void * icon;
    int32_t pre_time;
    int32_t cure_time;
    int32_t cure_temp;
	bool enabled;
    struct cure_preset *subtype;
    size_t subtype_size;
} cure_preset_t;

typedef struct {
    int32_t initial;
    int32_t version_pos;
    cure_preset_t *current;
    cure_preset_t *main;
    cure_preset_t *version;
    cure_preset_t *type;
    int32_t level;
} current_preset_t;

typedef struct {
    const char * ssid;
    int32_t rssi;
    bool secure;
    bool connected;
    const char * ip;
    const char * password;
} wifi_detail_t;

/**********************
 * GLOBAL VARIABLES
 **********************/
LV_IMAGE_DECLARE(image_droplet_black_icon);
LV_IMAGE_DECLARE(image_droplet_clear_icon);
LV_IMAGE_DECLARE(image_droplet_grey_icon);
LV_IMAGE_DECLARE(image_droplet_white_icon);
LV_IMAGE_DECLARE(image_custom_preset_icon);


extern lv_subject_t subject_switch_heat;
extern lv_subject_t subject_switch_cure;
extern lv_subject_t subject_temperature_heat;
extern lv_subject_t subject_temperature_cure;
extern lv_subject_t subject_time_heat;
extern lv_subject_t subject_time_cure;

extern lv_subject_t subject_preset_type;
extern lv_subject_t subject_preset_type_text;
extern lv_subject_t subject_preset_icon;

extern lv_subject_t subject_current_temperature;

extern const lv_image_dsc_t *cure_preset_icons[];


// Device about
extern lv_subject_t subject_device_model; // string
extern lv_subject_t subject_device_serial; // string
extern lv_subject_t subject_device_firmware; // string

// Device usage statistics
extern lv_subject_t subject_stats_cure_cycles; // int
extern lv_subject_t subject_stats_cure_time; // int
extern lv_subject_t subject_stats_cure_aborts; // int
extern lv_subject_t subject_heater_cycle_left; // int
extern lv_subject_t subject_heater_cycle_right; // int
extern lv_subject_t subject_heater_time_left; // int
extern lv_subject_t subject_heater_time_right; // int
extern lv_subject_t subject_led_usage_total; // int

// Drying mode
extern lv_subject_t subject_drying_mode_switch; // int
extern lv_subject_t subject_drying_mode_value; // int
extern lv_subject_t subject_drying_time_counter; // int (seconds)

// Wi-Fi
extern lv_subject_t subject_wifi_switch; // int
extern lv_subject_t subject_wifi_ssid; // string (do not use directy, use wifi_update_status() function)
extern lv_subject_t subject_wifi_ip; // string (do not use directy, use wifi_update_status() function)

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * @brief Show the boot screen
 * Should call ui_init() once cure engine and cure profile mananger are initalized
 */
void ui_show_boot_screen();

 /**
  * Main UI entry function
  */
void ui_init(void);

/**
 * Move an object on the X-axis using animation
 * @param obj Object to be moved
 * @param x Move the object relative to its current position on the x-axis
 * @param delay_ms Time delay before moving the object 
 */
void ui_anim_x(lv_obj_t *obj, int32_t x, int32_t delay_ms);

/**
 * Modify the opacity of an object using animation
 * @param obj Object
 * @param opa Target opacity of the object
 * @param delay_ms Time delay before starting the animation
 */
void ui_anim_opa(lv_obj_t *obj, lv_opa_t opa, int32_t delay_ms);

/**
 * Modify the state of children of an object
 * @param parent The main parent
 * @param state The type of state to be modified
 * @param remove Whether to remove or add the specified state
 * @param levels Depth of recursion: 
 *               0 - Includes only immediate children.
 *               1 - Includes immediate children and their children.
 *               N - Includes children up to N levels deep.
 */
void obj_modify_state_recursive(lv_obj_t * parent, lv_state_t state, bool remove, uint32_t levels);

/**
 * Add a state to children of an object
 * @param parent The main parent
 * @param state The type of state to be added
 * @param levels Depth of recursion: 
 *               0 - Includes only immediate children.
 *               1 - Includes immediate children and their children.
 *               N - Includes children up to N levels deep.
 */
void obj_add_state_recursive(lv_obj_t * parent, lv_state_t state, uint32_t levels);

/**
 * Remove a state from children of an object
 * @param parent The main parent
 * @param state The type of state to be removed
 * @param levels Depth of recursion: 
 *               0 - Includes only immediate children.
 *               1 - Includes immediate children and their children.
 *               N - Includes children up to N levels deep.
 */
void obj_remove_state_recursive(lv_obj_t * parent, lv_state_t state, uint32_t levels);


/**
 * Show an error.
 * @param   title   Error title
 * @param   desc    Error description
 * @param   code    Error code
 * @param   link    Link text for QR code
 */
void show_error(const char * title, const char * desc, int32_t code, const char * link);

/**
 * Add a custom preset eg from saved storage
 * @param   preset  Custom preset
 * @param   index   Index of the preset
 */
void add_custom_preset(cure_preset_t preset, uint32_t index);

/**
 * Load a preset to the UI eg last loaded preset
 * @param   index   Index of the preset
 */
void load_preset(uint32_t index);

/**
 * Show the door open screen
 */
void door_open();

void show_heat_cool_screen();

void show_home_screen();

void show_curing_screen();

void show_finished_screen(bool aborted);

screen_t get_current_screen();

/**
 * Update the progress bar
 * @param pre_heat_progress The current pre-heat progress (0-100).
 * @param cure_progress The current cure time progress (0-100).
 * @param skip_heat Whether the pre-heat was skipped
 * @param paused Whether the stage is paused
 */
void progress_bar_update(int32_t pre_heat_progress, int32_t cure_progress, bool skip_heat, bool paused);

/**
 * Set the Cure label text with countdown timer
 * @param text Text on the label
 */
void set_cure_text(const char * text);

/**
 * Set the countdown timer
 * @param hour Can also be used as minute
 * @param minute Can also be used as second
 */
void set_cure_time(int32_t hour, int32_t minute);

/**
 * Set the remaining time label text
 * @param text Text on the label
 */
void set_remaining_time(const char * text);

/**
 * Shorten the preset type text
 * @param input the input text
 * @param output the output pointer
 * @param max_len maximum length of the output pointer
 */
void ui_shorten_string(const char *input, char *output, size_t max_len);

/**
 * @brief Clear the list of Wi-Fi networks
 */
void wifi_clear_list(void);

/**
 * @brief Add a wifi network to the list. Add each available network to the list individually
 * @param wifi Wi-Fi network (Needed parameters: ssid, rssi, secure)
 */
void wifi_add_list(wifi_detail_t *wifi);

/**
 * @brief Update the Wi-Fi network status
 * @param wifi Wi-Fi network (set the 'connected' parameter to true if connected)
 * @param status Status of the Wi-Fi network
 *               0 - Not connected
 *               1 - Connected (ensure to set the 'IP address' and connected to 'true' in the wifi pointer)
 *               2 - Password error
 *               3 - No internet
 *               4 - WiFi Disabled
 */
void wifi_update_status(wifi_detail_t *wifi, uint8_t status);

/**********************
 *  CALLBACK FUNCTIONS
 **********************/

/**
 * A screen was loaded and shown
 * @param   screen  The screen that was loaded.
 */
void on_screen_loaded_cb(screen_t screen);

/**
 * A button was pressed.
 * @param   screen  The screen on which the button was pressed.
 * @param   button  The button that was pressed.
 */
void on_footer_button_pressed_cb(screen_t screen, footer_button_t button);

/**
 * A preset was selected. Save the preset for next boot.
 * @param   index   Index of the preset (-1 if no preset is selected)
 */
void on_preset_selected_cb(uint32_t index);

/**
 * Custom preset was deleted. 
 * Delete/remove from storage location
 * @param   index   Index of the preset
 */
void on_delete_custom_preset_cb(uint32_t index);

/**
 * Custom preset was edited/saved. 
 * Save to  storage location
 * @param   preset  Custom preset
 * @param   index   Index of the preset
 */
void on_save_custom_preset_cb(cure_preset_t preset, uint32_t index);

/**
 * Heating status changed. Called when started/ended/paused/resumed.
 * @param   state   Enabled/Disabled state
 * @param   temperature Temperature setting of the stage
 */
void on_heat_status_cb(bool state, uint32_t temperature);

/**
 * Curing status changed. Called when started/ended/paused/resumed.
 * @param   state   Enabled/Disabled state
 * @param   temperature Temperature setting of the stage
 */
void on_cure_status_cb(bool state, uint32_t temperature);

/**
 * @brief Get the cure profile info from index cb object
 * @param   index The index of the cure profile to get
 * @param   cure_profile The cure profile object to put the retrieved into to
 */
bool get_cure_profile_info_from_index_cb(uint32_t index, cure_preset_t *cure_profile);

/**
 * @brief Get the number of preset profiles cb object
 * @return The number of preset profiles
 */
size_t get_number_of_preset_profiles_cb();

/**
 * @brief WiFi network was selected, attempt to connect. Use the wifi_update_status() to update the status
 * @param wifi The Wi-Fi network that was selected
 */
void wifi_selected_cb(wifi_detail_t *wifi);

/**
 * @brief WiFi network was forgotten
 * @param wifi The Wi-Fi network that was forgotten
 */
void wifi_forget_cb(wifi_detail_t *wifi);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*UI_H*/
