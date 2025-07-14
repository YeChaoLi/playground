#include "Display_ST7789.h"
#include "LVGL_Driver.h"
#include "RGB_lamp.h"
#include "ui.h"

#include <ESP32Time.h> // https://github.com/fbiego/ESP32Time
#include <WiFi.h>

String months[] = {
  "January", "February", "March", "April", "May", "June",
  "July", "August", "September", "October", "November", "December"
};

int pos[7]={69,-69,-46, -23,0,23,46};

ESP32Time rtc(0); 
int zone=1;
const char* ntpServer = "pool.ntp.org";          
String ssid = "xxxxx";
String password = "xxxxxx";

int last_second=0;
String dateString="";
int angle=0;

int n=0; 

void connectWifi()
{ 
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Can't connect to wifi!");
  }
  Serial.println("conected");
}

void setTime()
{
  configTime(3600*zone, 0, ntpServer);
  struct tm timeinfo;
  if (getLocalTime(&timeinfo)){
    rtc.setTimeStruct(timeinfo); 
  }
}


void setup()
{       
  Serial.begin(115200);
  LCD_Init();
  Lvgl_Init();
  Set_Backlight(10);
  Set_Color(200, 200, 2);
  ui_init();

   connectWifi();
   setTime();
  
}

void loop()
{
  lv_task_handler();
  delay(5);

  

     
    if(rtc.getSecond()!=last_second)
      {
        last_second=rtc.getSecond();
        _ui_label_set_property(ui_timeLBL, _UI_LABEL_PROPERTY_TEXT, rtc.getTime().substring(0,5).c_str()); 
        _ui_label_set_property(ui_secLBL, _UI_LABEL_PROPERTY_TEXT, rtc.getTime().substring(6,8).c_str()); 
        _ui_label_set_property(ui_yearLBL, _UI_LABEL_PROPERTY_TEXT, String(rtc.getYear()).c_str()); 
        dateString=months[rtc.getMonth()]+" "+String(rtc.getDay());
        _ui_label_set_property(ui_dateLBL, _UI_LABEL_PROPERTY_TEXT, dateString.c_str()); 
        Serial.println(rtc.getDayofWeek());
         _ui_label_set_property(ui_secHEX, _UI_LABEL_PROPERTY_TEXT, String(n).c_str()); 
        lv_obj_set_pos(ui_dayPNL, 121, pos[rtc.getDayofWeek()]);  

        n++;
        if(n==7200)
        {
        n=0;
        setTime();
        }  
      }

    // Increment the angle
    angle=angle+10;
    if (angle >= 3600) angle = 0; // Reset angle (in LVGL, angle is in tenths of degrees)

    // Set the new angle
    lv_img_set_angle(ui_Image2, angle);
    RGB_Lamp_Loop(3);
}
