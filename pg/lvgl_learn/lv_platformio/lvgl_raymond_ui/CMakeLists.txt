idf_component_register(
    SRCS
        "ui.c"
        "screens/ui_screen_boot.c"
        "screens/ui_screen_curing.c"
        "screens/ui_screen_door.c"
        "screens/ui_screen_error.c"
        "screens/ui_screen_finished.c"
        "screens/ui_screen_heat_cool.c"
        "screens/ui_screen_home.c"
        "screens/ui_screen_pre_heat.c"
        "screens/ui_screen_settings.c"
        "screens/ui_screen_wifi.c"
        "screens/ui_screen_drying.c"
        "components/ui_button.c"
        "components/ui_footer.c"
        "components/ui_keyboard.c"
        "components/ui_label.c"
        "components/ui_list_element.c"
        "components/ui_roller.c"
        "components/ui_slider.c"
        "components/ui_spinbox.c"
        "components/ui_switch.c"
        "components/ui_title_bar.c"
        "generated/font_proxima_24.c"
        "generated/font_proxima_28.c"
        "generated/font_proxima_35.c"
        "generated/font_proxima_40.c"
        "generated/font_proxima_44.c"
        "generated/font_proxima_50.c"
        "generated/font_proxima_bold_35.c"
        "generated/font_proxima_bold_40.c"
        "generated/font_proxima_bold_44.c"
        "generated/font_proxima_bold_128.c"
        "generated/font_rubik_28.c"
        "generated/font_rubik_56.c"
        "generated/font_rubik_90.c"
        "generated/icon_chevron_right.c"
        "generated/icon_password_hide.c"
        "generated/icon_password_show.c"
        "generated/icon_wifi_disabled.c"
        "generated/icon_wifi_locked.c"
        "generated/icon_wifi_medium_signal.c"
        "generated/icon_wifi_no_internet.c"
        "generated/icon_wifi_no_signal.c"
        "generated/icon_wifi_strong_signal.c"
        "generated/icon_wifi_unlocked.c"
        "generated/icon_wifi_weak_signal.c"
        "generated/image_button_back.c"
        "generated/image_caret_down.c"
        "generated/image_caret_up.c"
        "generated/image_check.c"
        "generated/image_cooling.c"
        "generated/image_cure_icon.c"
        "generated/image_curing_icon.c"
        "generated/image_custom_preset_icon.c"
        "generated/image_droplet_black_icon.c"
        "generated/image_droplet_clear_icon.c"
        "generated/image_droplet_grey_icon.c"
        "generated/image_droplet_white_icon.c"
        "generated/image_droplet.c"
        "generated/image_drying.c"
        "generated/image_edit_icon.c"
        "generated/image_finished_icon.c"
        "generated/image_formlabs_logo.c"
        "generated/image_forward_arrow.c"
        "generated/image_heat_icon.c"
        "generated/image_heating.c"
        "generated/image_icons.c"
        "generated/image_info_icon.c"
        "generated/image_line_cool.c"
        "generated/image_line_cure.c"
        "generated/image_line_heat.c"
        "generated/image_line_warm.c"
        "generated/image_mask_cool.c"
        "generated/image_mask_cure.c"
        "generated/image_mask_heat.c"
        "generated/image_mask_warm.c"
        "generated/image_next_icon.c"
        "generated/image_open_door.c"
        "generated/image_pause.c"
        "generated/image_plus_icon.c"
        "generated/image_pre_heat_icon.c"
        "generated/image_time_icon.c"

    INCLUDE_DIRS "." "components" "screens" "includes"
    PRIV_REQUIRES lvgl  # Add required components here
)

component_compile_options(-Wno-error=format= -Wno-format)