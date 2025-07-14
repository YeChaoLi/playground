/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "lvgl.h"
#include "app_hal.h"

#include "demos/lv_demos.h"
#include "ui.h"

#ifdef ARDUINO
#include <Arduino.h>

void setup() {
  lv_init();
  hal_setup();
  lv_demo_widgets();
}

void loop() {
  hal_loop();  /* Do not use while loop in this function */
}

#else

int main(void)
{
	lv_init();

	hal_setup();

  // lv_demo_widgets();
  ui_init();

	hal_loop();
}

#endif /*ARDUINO*/