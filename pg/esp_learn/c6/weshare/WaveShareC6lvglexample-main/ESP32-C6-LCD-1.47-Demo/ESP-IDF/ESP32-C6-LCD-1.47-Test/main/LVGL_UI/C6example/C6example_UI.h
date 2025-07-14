#ifndef C6EXAMPLE_UI_H
#define C6EXAMPLE_UI_H

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "esp_http_client.h"
#include "esp_sntp.h"
#include "time.h"

#ifdef __cplusplus
extern "C" {
#endif

void C6example_UI_Init(void);
void C6example_UI_Close(void);
void C6example_UI_Loop(void);

#ifdef __cplusplus
}
#endif

#endif /* C6EXAMPLE_UI_H */ 