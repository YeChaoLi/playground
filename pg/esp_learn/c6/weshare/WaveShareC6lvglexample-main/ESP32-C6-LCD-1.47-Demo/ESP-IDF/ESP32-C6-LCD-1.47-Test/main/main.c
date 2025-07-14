/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

#include "ST7789.h"
#include "SD_SPI.h"
#include "RGB.h"
#include "Wireless.h"
#include "C6example_UI.h"

void app_main(void)
{
    Wireless_Init();
    Flash_Searching();
    RGB_Init();
    RGB_Example();
    SD_Init();                              // SD must be initialized behind the LCD
    LCD_Init();
    BK_Light(10);                           // Set backlight to 10%
    LVGL_Init();                            // returns the screen object

/********************* C6example UI *********************/
    C6example_UI_Init();

    while (1) {
        // raise the task priority of LVGL and/or reduce the handler period can improve the performance
        vTaskDelay(pdMS_TO_TICKS(5));
        // The task running lv_timer_handler should have lower priority than that running `lv_tick_inc`
        lv_timer_handler();
        C6example_UI_Loop();
    }
}
