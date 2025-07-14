/**
 * @file ui.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "ui.h"
#include "components/ui_footer.h"
#include "screens/ui_screens.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void temperature_observer_cb(lv_observer_t *observer, lv_subject_t *subject);
static void time_observer_cb(lv_observer_t *observer, lv_subject_t *subject);

static void translate_x_anim(void *var, int32_t v);
static void opa_anim(void *var, int32_t v);

static int32_t roundNearest5(int32_t n);

#if AUTO_TEST
static void auto_switch_cb(lv_timer_t *timer);
#endif

/**********************
 *  GLOBAL VARIABLES
 **********************/

lv_subject_t subject_switch_heat;
lv_subject_t subject_switch_cure;
lv_subject_t subject_temperature_heat;
lv_subject_t subject_temperature_cure;
lv_subject_t subject_time_heat;
lv_subject_t subject_time_cure;

lv_subject_t subject_preset_type;
lv_subject_t subject_preset_type_text;
lv_subject_t subject_preset_icon;

lv_subject_t subject_current_temperature;

const lv_image_dsc_t *cure_preset_icons[] = {
	NULL, /* No preset doesn't have an icon */
	&image_droplet_black_icon,
	&image_droplet_clear_icon,
	&image_droplet_grey_icon,
	&image_droplet_white_icon,
	&image_custom_preset_icon
};

// Device about
lv_subject_t subject_device_model; // string
lv_subject_t subject_device_serial; // string
lv_subject_t subject_device_firmware; // string

// Device usage statistics
lv_subject_t subject_stats_cure_cycles; // int
lv_subject_t subject_stats_cure_time; // int
lv_subject_t subject_stats_cure_aborts; // int
lv_subject_t subject_heater_cycle_total; // int
lv_subject_t subject_heater_cycle_left; // int
lv_subject_t subject_heater_cycle_right; // int
lv_subject_t subject_heater_time_total; // int
lv_subject_t subject_heater_time_left; // int
lv_subject_t subject_heater_time_right; // int
lv_subject_t subject_led_usage_total; // int
lv_subject_t subject_led_usage_left; // int
lv_subject_t subject_led_usage_right; // int

// Drying mode
lv_subject_t subject_drying_mode_switch;  // int
lv_subject_t subject_drying_mode_value;	  // int
lv_subject_t subject_drying_time_counter; // int (seconds)

// Wi-Fi
lv_subject_t subject_wifi_switch; // int
lv_subject_t subject_wifi_ssid; // string
lv_subject_t subject_wifi_ip; // string

/**********************
 *  STATIC VARIABLES
 **********************/

#if AUTO_TEST
static lv_timer_t * auto_timer;
static uint32_t screens;
#endif

static char preset_type_buffer[OBSERVER_TEXT_BUFFER_SIZE];
static char preset_type_text_buffer[OBSERVER_TEXT_BUFFER_SIZE];

static char wifi_ssid_buffer[OBSERVER_TEXT_BUFFER_SIZE];
static char wifi_ip_buffer[OBSERVER_TEXT_BUFFER_SIZE];

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ui_show_boot_screen(void)
{
	ui_screen_boot_init();
	lv_disp_load_scr(ui_screen_boot());
}

void ui_init(void)
{

	lv_subject_init_int(&subject_temperature_heat, 50);
	lv_subject_init_int(&subject_temperature_cure, 40);
	lv_subject_init_int(&subject_time_heat, 5);
	lv_subject_init_int(&subject_time_cure, 3);

	lv_subject_init_int(&subject_switch_heat, 1);
	lv_subject_init_int(&subject_switch_cure, 1);

	lv_subject_init_int(&subject_current_temperature, 20);

	lv_subject_init_string(&subject_preset_type, preset_type_buffer, NULL, OBSERVER_TEXT_BUFFER_SIZE, "No preset selected");
	lv_subject_init_string(&subject_preset_type_text, preset_type_text_buffer, NULL, OBSERVER_TEXT_BUFFER_SIZE, "No preset selected");
	lv_subject_init_int(&subject_preset_icon, 0);

	lv_subject_init_int(&subject_wifi_switch, 1);

	lv_subject_init_string(&subject_wifi_ssid, wifi_ssid_buffer, NULL, OBSERVER_TEXT_BUFFER_SIZE, "No Wi-Fi");
	lv_subject_init_string(&subject_wifi_ip, wifi_ip_buffer, NULL, OBSERVER_TEXT_BUFFER_SIZE, "No IP");

	/* Drying mode settings */
	lv_subject_init_int(&subject_drying_mode_switch, 1);
	lv_subject_init_int(&subject_drying_mode_value, 3);
	lv_subject_init_int(&subject_drying_time_counter, 0);

	lv_subject_add_observer(&subject_time_heat, time_observer_cb, NULL);
	lv_subject_add_observer(&subject_time_cure, time_observer_cb, NULL);
	lv_subject_add_observer(&subject_temperature_heat, temperature_observer_cb, NULL);
	lv_subject_add_observer(&subject_temperature_cure, temperature_observer_cb, NULL);

	ui_screen_home_init();
	ui_screen_settings_init();
	ui_screen_error_init();

	ui_screen_pre_heat_init();
	ui_screen_heat_cool_init();
	ui_screen_curing_init();
	ui_screen_finished_init();
	ui_screen_door_init();
	ui_screen_drying_init();
	ui_screen_wifi_init();

	/* Load home screen after boot */
    lv_screen_load_anim(ui_screen_home(), LV_SCR_LOAD_ANIM_FADE_ON, 500, 0, false);

#if AUTO_TEST
	auto_timer = lv_timer_create(auto_switch_cb, AUTO_SWITCH_INTERVAL_MS, NULL);
	lv_timer_set_repeat_count(auto_timer, -1);

	LV_LOG_WARN("Auto switch screens enabled");

#endif

}

void ui_anim_x(lv_obj_t *obj, int32_t x, int32_t delay_ms)
{
	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_var(&a, obj);
	lv_anim_set_duration(&a, 300);
	lv_anim_set_delay(&a, delay_ms);
	lv_anim_set_values(&a, lv_obj_get_style_translate_x(obj, 0), x);
	lv_anim_set_exec_cb(&a, translate_x_anim);
	lv_anim_start(&a);
}

void ui_anim_opa(lv_obj_t *obj, lv_opa_t opa, int32_t delay_ms)
{
	lv_anim_t a;
	lv_anim_init(&a);
	lv_anim_set_var(&a, obj);
	lv_anim_set_duration(&a, 150);
	lv_anim_set_delay(&a, delay_ms);
	lv_anim_set_values(&a, lv_obj_get_style_opa(obj, 0), opa);
	lv_anim_set_exec_cb(&a, opa_anim);
	lv_anim_start(&a);
}


void obj_add_state_recursive(lv_obj_t * parent, lv_state_t state, uint32_t levels)
{
	obj_modify_state_recursive(parent, state, false, levels);
}

void obj_remove_state_recursive(lv_obj_t * parent, lv_state_t state, uint32_t levels)
{
	obj_modify_state_recursive(parent, state, true, levels);
}

void obj_modify_state_recursive(lv_obj_t * parent, lv_state_t state, bool remove, uint32_t levels)
{
	for (uint32_t i = 0; i < lv_obj_get_child_count(parent); i++) {
		lv_obj_t * child = lv_obj_get_child(parent, i);
		if (remove){
			lv_obj_remove_state(child, state);
		} else {
			lv_obj_add_state(child, state);
		}
		if (levels){
			obj_modify_state_recursive(child, state, remove, levels - 1);
		}
	}
}

void ui_shorten_string(const char *input, char *output, size_t max_len)
{
	printf("ui_shorten_string: %s\n", input);
	if (!input || !output || max_len == 0)
		return;

	const char *space = strchr(input, ' ');
	if (space)
	{
		/* It's a multi-word string: build acronym */
		size_t out_index = 0;
		int word_start = 1;
		for (size_t i = 0; input[i] != '\0' && out_index + 2 < max_len; ++i)
		{
			if (isspace((unsigned char)input[i]))
			{
				word_start = 1;
			}
			else if (word_start)
			{
				if (out_index > 0)
					output[out_index++] = '.';
				output[out_index++] = toupper((unsigned char)input[i]);
				word_start = 0;
			}
		}
		output[out_index] = '\0';
	}
	else
	{
		/* Single word */
		size_t len = strlen(input);
		if (len > 4 && max_len >= 6)
		{
			snprintf(output, max_len, "%.4s.", input);
		}
		else
		{
			strncpy(output, input, max_len - 1);
			output[max_len - 1] = '\0'; /* ensure null-termination */
		}
	}
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void translate_x_anim(void *var, int32_t v)
{
	lv_obj_set_style_translate_x(var, v, 0);
}

static void opa_anim(void *var, int32_t v)
{
	lv_obj_set_style_opa(var, v, 0);
}

static void temperature_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
	LV_UNUSED(observer);

	int32_t v = lv_subject_get_int(subject);
	if (v < 30)
	{
		lv_subject_set_int(subject, 30);
	}
	else if (v > 100)
	{
		lv_subject_set_int(subject, 100);
	} 
	else 
	{
		int32_t new = roundNearest5(v);
		if (new != v)
		{
			lv_subject_set_int(subject, new);
		}
	}
}

static void time_observer_cb(lv_observer_t *observer, lv_subject_t *subject)
{
	LV_UNUSED(observer);

	int32_t v = lv_subject_get_int(subject);
	if (v < 0)
	{
		lv_subject_set_int(subject, 0);
	}
	else if (v > 120)
	{
		lv_subject_set_int(subject, 120);
	} 
	else if (v > 30) 
	{
		int32_t new = roundNearest5(v);
		if (new != v)
		{
			lv_subject_set_int(subject, new);
		}
	}
}

static int32_t roundNearest5(int32_t n)
{
	int32_t r = n % 5;
	if (r == 0){
		return n;
	}
	if (r >= 3){
		return n + (5 - r);
	} else {
		return n - r;
	}
}

#if AUTO_TEST
static void auto_switch_cb(lv_timer_t *timer)
{
	switch (screens)
	{
	case 0:
		lv_screen_load_anim(ui_screen_home(), AUTO_SWITCH_ANIM, AUTO_SWITCH_TIME_MS, 0, false);
		break;
	case 1:
		lv_screen_load_anim(ui_screen_settings(), AUTO_SWITCH_ANIM, AUTO_SWITCH_TIME_MS, 0, false);
		break;
	case 2:
		lv_screen_load_anim(ui_screen_pre_heat(), AUTO_SWITCH_ANIM, AUTO_SWITCH_TIME_MS, 0, false);
		break;
	case 3:
		lv_screen_load_anim(ui_screen_heat_cool(), AUTO_SWITCH_ANIM, AUTO_SWITCH_TIME_MS, 0, false);
		break;
	case 4:
		lv_screen_load_anim(ui_screen_curing(), AUTO_SWITCH_ANIM, AUTO_SWITCH_TIME_MS, 0, false);
		break;
	case 5:
		lv_screen_load_anim(ui_screen_door(), AUTO_SWITCH_ANIM, AUTO_SWITCH_TIME_MS, 0, false);
		break;
	case 6:
		lv_screen_load_anim(ui_screen_finished(), AUTO_SWITCH_ANIM, AUTO_SWITCH_TIME_MS, 0, false);
		break;
	case 7:
		lv_screen_load_anim(ui_screen_error(), AUTO_SWITCH_ANIM, AUTO_SWITCH_TIME_MS, 0, false);
		break;
	case 8:
		lv_screen_load_anim(ui_screen_wifi(), AUTO_SWITCH_ANIM, AUTO_SWITCH_TIME_MS, 0, false);
		break;
	default:
		break;
	}

	screens++;
	if (screens > 8)
	{
		screens = 0;
	}
}
#endif

/**********************
 *  WEAK FUNCTIONS
 **********************/

/* Implement these function elsewhere to override */

__attribute__((weak)) void on_screen_loaded_cb(screen_t screen)
{
	LV_LOG_WARN("Screen loaded %d", screen);
}

__attribute__((weak)) void on_footer_button_pressed_cb(screen_t screen, footer_button_t button)
{
	LV_LOG_WARN("Footer button pressed %d on screen %d", button, screen);
}

__attribute__((weak)) void on_preset_selected_cb(uint32_t index)
{
	LV_LOG_WARN("Preset selected index %d", index);
}

__attribute__((weak)) void on_delete_custom_preset_cb(uint32_t index)
{
	LV_LOG_WARN("Custom Preset deleted index %d", index);
}

__attribute__((weak)) void on_save_custom_preset_cb(cure_preset_t preset, uint32_t index)
{
	LV_LOG_WARN("Custom preset saved index %d", index);
}

__attribute__((weak)) void on_heat_status_cb(bool state, uint32_t temperature)
{
	LV_LOG_WARN("Heat status changed %d -> %d °C", state, temperature);
}

__attribute__((weak)) void on_cure_status_cb(bool state, uint32_t temperature)
{
	LV_LOG_WARN("Cure status changed %d -> %d °C", state, temperature);
}

__attribute__((weak)) bool get_cure_profile_info_from_index_cb(uint32_t index, cure_preset_t *cure_profile)
{
	LV_LOG_WARN("Default callback");
	cure_profile->name = "error";
	cure_profile->icon = NULL;
	cure_profile->pre_time = 0;
	cure_profile->cure_time = 0;
	cure_profile->cure_temp = 0;
	cure_profile->enabled = false;
	return false;
}

__attribute__((weak)) size_t get_number_of_preset_profiles_cb()
{
	LV_LOG_WARN("default get_number_of_preset_profiles_cb, returned 4");
	return 4;
}

__attribute__((weak)) void wifi_selected_cb(wifi_detail_t *wifi)
{
	LV_LOG_WARN("Wi-Fi selected %s%s", wifi->ssid, wifi->secure ? "*" : "");
}

__attribute__((weak)) void wifi_forget_cb(wifi_detail_t *wifi)
{
	LV_LOG_WARN("Wi-Fi forget %s", wifi->ssid);
}
