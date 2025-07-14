#include "C6example_UI.h"
#include "ui.h"
#include "esp_wifi.h"
#include "esp_sntp.h"
#include "esp_log.h"

static const char *TAG = "C6example_UI";

static const char* months[] = {
    "January", "February", "March", "April", "May", "June",
    "July", "August", "September", "October", "November", "December"
};

static int pos[7] = {69, -69, -46, -23, 0, 23, 46};
static int last_second = 0;
static char dateString[32];
static int angle = 0;
static int n = 0;
static char sec_hex_str[8];  // Buffer for sec_hex string

static void time_sync_notification_cb(struct timeval *tv)
{
    ESP_LOGI(TAG, "Time synchronized from NTP server");
}

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                             int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        
        // Initialize SNTP
        sntp_setoperatingmode(SNTP_OPMODE_POLL);
        sntp_setservername(0, "pool.ntp.org");
        sntp_set_time_sync_notification_cb(time_sync_notification_cb);
        sntp_init();
    }
}

static void wifi_init_sta(void)
{
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = "xxxxx",
            .password = "xxxxxx",
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

void C6example_UI_Init(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize WiFi
    wifi_init_sta();

    // Initialize UI
    ui_init();
}

void C6example_UI_Close(void)
{
    lv_obj_clean(lv_scr_act());
}

void C6example_UI_Loop(void)
{
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    if (timeinfo.tm_sec != last_second) {
        last_second = timeinfo.tm_sec;
        
        char time_str[6];
        char sec_str[3];
        char year_str[5];
        
        snprintf(time_str, sizeof(time_str), "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
        snprintf(sec_str, sizeof(sec_str), "%02d", timeinfo.tm_sec);
        // snprintf(year_str, sizeof(year_str), "%d", timeinfo.tm_year + 1900);
        snprintf(dateString, sizeof(dateString), "%s %d", months[timeinfo.tm_mon], timeinfo.tm_mday);
        snprintf(sec_hex_str, sizeof(sec_hex_str), "%d", n);  // Convert n to string
        
        _ui_label_set_property(ui_timeLBL, _UI_LABEL_PROPERTY_TEXT, time_str);
        _ui_label_set_property(ui_secLBL, _UI_LABEL_PROPERTY_TEXT, sec_str);
        _ui_label_set_property(ui_yearLBL, _UI_LABEL_PROPERTY_TEXT, year_str);
        _ui_label_set_property(ui_dateLBL, _UI_LABEL_PROPERTY_TEXT, dateString);
        _ui_label_set_property(ui_secHEX, _UI_LABEL_PROPERTY_TEXT, sec_hex_str);
        
        lv_obj_set_pos(ui_dayPNL, 121, pos[timeinfo.tm_wday]);
        
        n++;
        if (n == 7200) {
            n = 0;
        }
    }

    // Increment the angle
    angle = angle + 10;
    if (angle >= 3600) angle = 0; // Reset angle (in LVGL, angle is in tenths of degrees)

    // Set the new angle
    lv_img_set_angle(ui_Image2, angle);
} 