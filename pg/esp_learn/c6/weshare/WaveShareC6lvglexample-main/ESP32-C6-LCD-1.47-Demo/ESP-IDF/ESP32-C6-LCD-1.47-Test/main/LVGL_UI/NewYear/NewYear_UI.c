#include "NewYear_UI.h"
#include "ui.h"

void NewYear_UI_Init(void)
{
    ui_init();
}

void NewYear_UI_Close(void)
{
    lv_obj_clean(lv_scr_act());
} 