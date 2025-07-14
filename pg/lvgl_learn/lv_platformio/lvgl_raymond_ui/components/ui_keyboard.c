/**
 * @file ui_keyboard.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "ui_keyboard.h"
#include "../ui.h"

/*********************
 *      DEFINES
 *********************/
typedef enum {
    LV_BUTTONMATRIX_CTRL_WIDTH_1      = 0x0001, /**< Set the width to 1 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_2      = 0x0002, /**< Set the width to 2 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_3      = 0x0003, /**< Set the width to 3 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_4      = 0x0004, /**< Set the width to 4 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_5      = 0x0005, /**< Set the width to 5 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_6      = 0x0006, /**< Set the width to 6 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_7      = 0x0007, /**< Set the width to 7 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_8      = 0x0008, /**< Set the width to 8 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_9      = 0x0009, /**< Set the width to 9 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_10     = 0x000A, /**< Set the width to 10 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_11     = 0x000B, /**< Set the width to 11 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_12     = 0x000C, /**< Set the width to 12 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_13     = 0x000D, /**< Set the width to 13 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_14     = 0x000E, /**< Set the width to 14 relative to the other buttons in the same row */
    LV_BUTTONMATRIX_CTRL_WIDTH_15     = 0x000F, /**< Set the width to 15 relative to the other buttons in the same row */

} lv_buttonmatrix_ctrl_width_t;

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void check_password_length(lv_obj_t *keyboard);
static void ui_keyboard_event_cb(lv_event_t *e);
static void ui_draw_event_cb(lv_event_t *e);

/**********************
 *  STATIC VARIABLES
 **********************/

static const char *kb_caps_map[] = {
    "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "\n",
    " ", "A", "S", "D", "F", "G", "H", "J", "K", "L", " ", "\n",
    LV_SYMBOL_EJECT, "Z", "X", "C", "V", "B", "N", "M", LV_SYMBOL_BACKSPACE, "\n",
    "?123", " ", LV_SYMBOL_OK, NULL};

static const char *kb_small_map[] = {
    "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "\n",
    " ", "a", "s", "d", "f", "g", "h", "j", "k", "l", " ", "\n",
    LV_SYMBOL_EJECT, "z", "x", "c", "v", "b", "n", "m", LV_SYMBOL_BACKSPACE, "\n",
    "?123", " ", LV_SYMBOL_OK, NULL};

static const lv_buttonmatrix_ctrl_t kb_text_ctrl[] = {
    LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5,
    (lv_buttonmatrix_ctrl_t)(LV_BUTTONMATRIX_CTRL_HIDDEN | LV_BUTTONMATRIX_CTRL_WIDTH_2), LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, (lv_buttonmatrix_ctrl_t)(LV_BUTTONMATRIX_CTRL_HIDDEN | LV_BUTTONMATRIX_CTRL_WIDTH_2),
    (lv_buttonmatrix_ctrl_t)(LV_KEYBOARD_CTRL_BUTTON_FLAGS | LV_BUTTONMATRIX_CTRL_WIDTH_8 | /*LV_BUTTONMATRIX_CTRL_RECOLOR |*/ LV_BUTTONMATRIX_CTRL_CHECKABLE), LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, (lv_buttonmatrix_ctrl_t)(LV_KEYBOARD_CTRL_BUTTON_FLAGS | LV_BUTTONMATRIX_CTRL_WIDTH_8),
    (lv_buttonmatrix_ctrl_t)(LV_KEYBOARD_CTRL_BUTTON_FLAGS | LV_BUTTONMATRIX_CTRL_WIDTH_4 | LV_BUTTONMATRIX_CTRL_CHECKABLE), LV_BUTTONMATRIX_CTRL_WIDTH_13, (lv_buttonmatrix_ctrl_t)(LV_KEYBOARD_CTRL_BUTTON_FLAGS | LV_BUTTONMATRIX_CTRL_DISABLED | LV_BUTTONMATRIX_CTRL_WIDTH_4)};

static const char *kb_num1_map[] = {
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "\n",
    "@", "#", "&", "%", "*", "/", "(", ")", "'", "\"", "\n",
    "1/2", "-", "+", "=", ";", ":", ",", ".", LV_SYMBOL_BACKSPACE, "\n",
    "ABC", " ", LV_SYMBOL_OK, NULL};

static const char *kb_num2_map[] = {
    "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "\n",
    "$", "€", "£", "¥", "^", "\\", "[", "]", "{", "}", "\n",
    "2/2", "_", "~", "|", "<", ">", "!", "?", LV_SYMBOL_BACKSPACE, "\n",
    "ABC", " ", LV_SYMBOL_OK, NULL};
static const lv_buttonmatrix_ctrl_t kb_num_ctrl[] = {
    LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5,
    LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5,
    (lv_buttonmatrix_ctrl_t)(LV_KEYBOARD_CTRL_BUTTON_FLAGS | LV_BUTTONMATRIX_CTRL_WIDTH_8), LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, LV_BUTTONMATRIX_CTRL_WIDTH_5, (lv_buttonmatrix_ctrl_t)(LV_KEYBOARD_CTRL_BUTTON_FLAGS | LV_BUTTONMATRIX_CTRL_WIDTH_8),
    (lv_buttonmatrix_ctrl_t)(LV_KEYBOARD_CTRL_BUTTON_FLAGS | LV_BUTTONMATRIX_CTRL_WIDTH_4), LV_BUTTONMATRIX_CTRL_WIDTH_13, (lv_buttonmatrix_ctrl_t)(LV_KEYBOARD_CTRL_BUTTON_FLAGS | LV_BUTTONMATRIX_CTRL_DISABLED | LV_BUTTONMATRIX_CTRL_WIDTH_4)};

static bool caps_on;
static bool numbers1;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_obj_t *ui_keyboard_create(lv_obj_t *parent)
{

    lv_obj_t *keyboard = lv_keyboard_create(parent);
    lv_obj_set_width(keyboard, 1280);
    lv_obj_set_height(keyboard, 308);
    lv_obj_set_align(keyboard, LV_ALIGN_BOTTOM_MID);
    lv_obj_set_style_bg_color(keyboard, lv_color_hex(0x000000), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(keyboard, 55, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_left(keyboard, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_right(keyboard, 50, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_top(keyboard, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_bottom(keyboard, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_row(keyboard, 15, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_pad_column(keyboard, 15, LV_PART_MAIN | LV_STATE_DEFAULT);

    LV_FONT_DECLARE(font_proxima_35);
    lv_obj_set_style_radius(keyboard, 10, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(keyboard, lv_color_hex(0x333333), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_opa(keyboard, 255, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(keyboard, lv_color_hex(0xFFFFFF), LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(keyboard, 255, LV_PART_ITEMS | LV_STATE_DEFAULT);
    lv_obj_set_style_text_font(keyboard, &font_proxima_35, LV_PART_ITEMS | LV_STATE_DEFAULT);

    lv_obj_set_style_bg_color(keyboard, lv_color_hex(0x1C1C1C), LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_bg_opa(keyboard, 255, LV_PART_ITEMS | LV_STATE_CHECKED);
    lv_obj_set_style_text_color(keyboard, lv_color_hex(0xFFFFFF), LV_PART_ITEMS | LV_STATE_CHECKED);

    lv_obj_set_style_bg_color(keyboard, lv_color_hex(0x616161), LV_PART_ITEMS | LV_STATE_DISABLED);
    lv_obj_set_style_bg_opa(keyboard, 255, LV_PART_ITEMS | LV_STATE_DISABLED);

    lv_keyboard_set_map(keyboard, LV_KEYBOARD_MODE_USER_1, kb_small_map, kb_text_ctrl);
    lv_keyboard_set_map(keyboard, LV_KEYBOARD_MODE_USER_2, kb_caps_map, kb_text_ctrl);
    lv_keyboard_set_map(keyboard, LV_KEYBOARD_MODE_USER_3, kb_num1_map, kb_num_ctrl);
    lv_keyboard_set_map(keyboard, LV_KEYBOARD_MODE_USER_4, kb_num2_map, kb_num_ctrl);

    lv_keyboard_set_mode(keyboard, LV_KEYBOARD_MODE_USER_1);

    uint32_t i;
    uint32_t event_cnt = lv_obj_get_event_count(keyboard);
    for (i = 0; i < event_cnt; i++)
    {
        lv_obj_remove_event(keyboard, i);
    }

    lv_obj_add_event_cb(keyboard, ui_keyboard_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_add_event_cb(keyboard, ui_draw_event_cb, LV_EVENT_DRAW_TASK_ADDED, NULL);
    lv_obj_add_flag(keyboard, LV_OBJ_FLAG_SEND_DRAW_TASK_EVENTS);

    caps_on = false;
    numbers1 = true;

    return keyboard;
}
/**********************
 *   STATIC FUNCTIONS
 **********************/

static void check_password_length(lv_obj_t *keyboard)
{
    lv_obj_t *textarea = lv_keyboard_get_textarea(keyboard);
    if (textarea == NULL)
        return;
    
    lv_keyboard_mode_t mode = lv_keyboard_get_mode(keyboard);
    
    if (lv_strlen(lv_textarea_get_text(textarea)) < 8)
    {
        // disable
        if (mode == LV_KEYBOARD_MODE_USER_1 || mode == LV_KEYBOARD_MODE_USER_2)
        {
            lv_buttonmatrix_set_button_ctrl(keyboard, 32, LV_BUTTONMATRIX_CTRL_DISABLED);
        }
        else if (mode == LV_KEYBOARD_MODE_USER_3 || mode == LV_KEYBOARD_MODE_USER_4)
        {
            lv_buttonmatrix_set_button_ctrl(keyboard, 31, LV_BUTTONMATRIX_CTRL_DISABLED);
        }
    }
    else
    {
        // enable
        if (mode == LV_KEYBOARD_MODE_USER_1 || mode == LV_KEYBOARD_MODE_USER_2)
        {
            lv_buttonmatrix_clear_button_ctrl(keyboard, 32, LV_BUTTONMATRIX_CTRL_DISABLED);
        }
        else if (mode == LV_KEYBOARD_MODE_USER_3 || mode == LV_KEYBOARD_MODE_USER_4)
        {
            lv_buttonmatrix_clear_button_ctrl(keyboard, 31, LV_BUTTONMATRIX_CTRL_DISABLED);
        }
    }
}

static void ui_keyboard_event_cb(lv_event_t *e)
{
    lv_obj_t *obj = lv_event_get_current_target(e);

    lv_keyboard_t *keyboard = (lv_keyboard_t *)obj;
    uint32_t btn_id = lv_buttonmatrix_get_selected_button(obj);

    check_password_length(obj);

    if (btn_id == LV_BUTTONMATRIX_BUTTON_NONE)
        return;

    const char *txt = lv_buttonmatrix_get_button_text(obj, btn_id);
    if (txt == NULL)
        return;

    // LV_LOG_WARN("Button id clicked %d -> %s", btn_id, txt);
    if (lv_strcmp(txt, LV_SYMBOL_EJECT) == 0)
    {
        caps_on = !caps_on;
        if (keyboard->mode == LV_KEYBOARD_MODE_USER_1)
        {
            lv_keyboard_set_mode(obj, LV_KEYBOARD_MODE_USER_2);
        }
        else
        {
            lv_keyboard_set_mode(obj, LV_KEYBOARD_MODE_USER_1);
        }
        check_password_length(obj);
        return;
    }
    else if (lv_strcmp(txt, "?123") == 0)
    {
        if (numbers1)
        {
            lv_keyboard_set_mode(obj, LV_KEYBOARD_MODE_USER_3);
        }
        else
        {
            lv_keyboard_set_mode(obj, LV_KEYBOARD_MODE_USER_4);
        }
        check_password_length(obj);
        return;
    }
    else if (lv_strcmp(txt, "ABC") == 0)
    {
        if (caps_on)
        {
            lv_keyboard_set_mode(obj, LV_KEYBOARD_MODE_USER_2);
        }
        else
        {
            lv_keyboard_set_mode(obj, LV_KEYBOARD_MODE_USER_1);
        }
        check_password_length(obj);
        return;
    }
    else if (lv_strcmp(txt, "1/2") == 0)
    {
        numbers1 = false;
        lv_keyboard_set_mode(obj, LV_KEYBOARD_MODE_USER_4);
        check_password_length(obj);
        return;
    }
    else if (lv_strcmp(txt, "2/2") == 0)
    {
        numbers1 = true;
        lv_keyboard_set_mode(obj, LV_KEYBOARD_MODE_USER_3);
        check_password_length(obj);
        return;
    }
    else if (lv_strcmp(txt, LV_SYMBOL_OK) == 0)
    {
        lv_result_t res = lv_obj_send_event(obj, LV_EVENT_READY, NULL);
        if (res != LV_RESULT_OK)
            return;

        if (keyboard->ta)
        {
            res = lv_obj_send_event(keyboard->ta, LV_EVENT_READY, NULL);
            if (res != LV_RESULT_OK)
                return;
        }
        return;
    }

    /*Add the characters to the text area if set*/
    if (keyboard->ta == NULL)
        return;
    
    lv_obj_remove_state(keyboard->ta, LV_STATE_CHECKED); /* Remove error state */

    if (lv_strcmp(txt, "Enter") == 0 || lv_strcmp(txt, LV_SYMBOL_NEW_LINE) == 0)
    {
        lv_textarea_add_char(keyboard->ta, '\n');
        if (lv_textarea_get_one_line(keyboard->ta))
        {
            lv_result_t res = lv_obj_send_event(keyboard->ta, LV_EVENT_READY, NULL);
            if (res != LV_RESULT_OK)
                return;
        }
    }
    else if (lv_strcmp(txt, LV_SYMBOL_LEFT) == 0)
    {
        lv_textarea_cursor_left(keyboard->ta);
    }
    else if (lv_strcmp(txt, LV_SYMBOL_RIGHT) == 0)
    {
        lv_textarea_cursor_right(keyboard->ta);
    }
    else if (lv_strcmp(txt, LV_SYMBOL_BACKSPACE) == 0)
    {
        lv_textarea_delete_char(keyboard->ta);
    }
    else if (lv_strcmp(txt, "+/-") == 0)
    {
        uint32_t cur = lv_textarea_get_cursor_pos(keyboard->ta);
        const char *ta_txt = lv_textarea_get_text(keyboard->ta);
        if (ta_txt[0] == '-')
        {
            lv_textarea_set_cursor_pos(keyboard->ta, 1);
            lv_textarea_delete_char(keyboard->ta);
            lv_textarea_add_char(keyboard->ta, '+');
            lv_textarea_set_cursor_pos(keyboard->ta, cur);
        }
        else if (ta_txt[0] == '+')
        {
            lv_textarea_set_cursor_pos(keyboard->ta, 1);
            lv_textarea_delete_char(keyboard->ta);
            lv_textarea_add_char(keyboard->ta, '-');
            lv_textarea_set_cursor_pos(keyboard->ta, cur);
        }
        else
        {
            lv_textarea_set_cursor_pos(keyboard->ta, 0);
            lv_textarea_add_char(keyboard->ta, '-');
            lv_textarea_set_cursor_pos(keyboard->ta, cur + 1);
        }
    }
    else
    {
        lv_textarea_add_text(keyboard->ta, txt);
    }
    check_password_length(obj);
}

static void ui_draw_event_cb(lv_event_t *e)
{
    lv_obj_t * obj = lv_event_get_target_obj(e);
    lv_draw_task_t * draw_task = lv_event_get_draw_task(e);
    lv_draw_dsc_base_t * base_dsc = (lv_draw_dsc_base_t *)lv_draw_task_get_draw_dsc(draw_task);
    lv_keyboard_t *keyboard = (lv_keyboard_t *)obj;

    if(base_dsc->part == LV_PART_ITEMS) {
        bool pressed = false;
        if(lv_buttonmatrix_get_selected_button(obj) == base_dsc->id1 && lv_obj_has_state(obj, LV_STATE_PRESSED)) {
            pressed = true;
        }
        int32_t id = 0;
        if (keyboard->mode == LV_KEYBOARD_MODE_USER_1 || keyboard->mode == LV_KEYBOARD_MODE_USER_2)
        {
            id = 32;
        }
        else if (keyboard->mode == LV_KEYBOARD_MODE_USER_3 || keyboard->mode == LV_KEYBOARD_MODE_USER_4)
        {
            id = 31;
        }
        if(base_dsc->id1 == id) {
            lv_draw_fill_dsc_t * fill_draw_dsc = lv_draw_task_get_fill_dsc(draw_task);
            if(fill_draw_dsc) {
                if (lv_buttonmatrix_has_button_ctrl(obj, base_dsc->id1, LV_BUTTONMATRIX_CTRL_DISABLED))
                {
                    fill_draw_dsc->color =  COLOR_INTERACTIVE_DISABLED;
                } else {
                    fill_draw_dsc->color = COLOR_INTERACTIVE_ACTIVE;
                }
                if (pressed){
                    fill_draw_dsc->color = COLOR_INTERACTIVE_PRESSED;
                }
            }
        }
        if(base_dsc->id1 == 21 && (keyboard->mode == LV_KEYBOARD_MODE_USER_1 || keyboard->mode == LV_KEYBOARD_MODE_USER_2)) {
            lv_draw_label_dsc_t * label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
            if(label_draw_dsc) {
                if (caps_on) {
                    label_draw_dsc->color = COLOR_INTERACTIVE_ACTIVE;
                } else {
                    label_draw_dsc->color = lv_color_hex(0xFFFFFF);
                }
            }
        }
        if (lv_buttonmatrix_has_button_ctrl(obj, base_dsc->id1, LV_BUTTONMATRIX_CTRL_CHECKED)) {
            /* Change the offset of checked buttons (Controls) */
            lv_draw_label_dsc_t * label_draw_dsc = lv_draw_task_get_label_dsc(draw_task);
            if(label_draw_dsc) {
                label_draw_dsc->ofs_y = 3;
            }
        }
    }
}

