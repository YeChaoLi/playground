#include "Display_ST7789.h"
#include "LVGL_Driver.h"
#include "RGB_lamp.h"
#include "ui.h"

#include "NimBLEDevice.h"


const char* targetDeviceMAC1 = "a4:c1:38:fb:64:94"; // MAC address for Sensor 1
static const NimBLEAdvertisedDevice* targetDevice1  = nullptr;

// Separate temperature and humidity variables for each sensor
float temp = 0.0;
float humi = 0.0;
float voltage;
int up=0;

int found=0;
bool setupFinished=1;

static BLEUUID serviceUUID("ebe0ccb0-7a0a-4b0c-8a1a-6ff2997da3a6");
static BLEUUID charUUID("ebe0ccc1-7a0a-4b0c-8a1a-6ff2997da3a6");



// Declare the callback class before setup()

class scanCallbacks : public NimBLEScanCallbacks {
    void onResult(const NimBLEAdvertisedDevice* advertisedDevice) override {
       // Serial.printf("Advertised Device found: %s\n", advertisedDevice->toString().c_str());

      //Serial.println(advertisedDevice->toString().c_str());

        // Compare MAC addresses to identify the devices
        if (advertisedDevice->getAddress().toString() == targetDeviceMAC1) {
          found++;
            Serial.println("Found target device for Sensor 1!");
            Serial.println(advertisedDevice->getRSSI());
            targetDevice1 = advertisedDevice;  // Store the first target device
        }
    }
} scanCB;



// Separate notifyCallback for Sensor 1
static void notifyCallback1(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
  Serial.print("Notify callback for SENSOR 0 ");
  temp = (pData[0] | (pData[1] << 8)) * 0.01; //little endian
  humi = pData[2];
  voltage = (pData[3] | (pData[4] << 8)) * 0.001; //little endian
  Serial.printf("temp = %.1f C ; humidity = %.1f %% ; voltage = %.3f V\n", temp, humi, voltage);
  Serial.println();

   _ui_label_set_property(ui_tmpLBL, _UI_LABEL_PROPERTY_TEXT, String(temp).c_str());
   _ui_label_set_property(ui_humLBL, _UI_LABEL_PROPERTY_TEXT, String(humi).substring(0,4).c_str());
   _ui_label_set_property(ui_batLBL, _UI_LABEL_PROPERTY_TEXT, String(voltage).c_str());
   up++;
   if(up==8) up=0;
    _ui_label_set_property(ui_updLBL, _UI_LABEL_PROPERTY_TEXT, String(up).c_str());
  
}



void connectToSensor(const NimBLEAdvertisedDevice* targetDevice, const char* sensorName, int sensorNumber) {
  if (targetDevice == nullptr) {
    Serial.println("Target device not found for " + String(sensorName));
    return;
  }

  Serial.println("Connecting to " + String(sensorName));

  // Create a client to connect to the sensor
  NimBLEClient* pClient = NimBLEDevice::createClient();

  if (pClient->connect(targetDevice)) {
    Serial.println("Successfully connected to " + String(sensorName));

    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find our service UUID: ");
      Serial.println(serviceUUID.toString().c_str());
      return;
    }
    Serial.println(" - Found our service");

    // Obtain a reference to the characteristic in the service of the remote BLE server
    BLERemoteCharacteristic* pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.print("Failed to find our characteristic UUID: ");
      Serial.println(charUUID.toString().c_str());
      return;
    }
    Serial.println(" - Found our characteristic");

    NimBLERemoteService* pSvc = pClient->getService(serviceUUID);
    // Register for notifications for the sensor
    if (sensorNumber == 1) {
       if (pSvc) {
        NimBLERemoteCharacteristic* pChr = pSvc->getCharacteristic(charUUID);
        if (pChr) {
            if (pChr->canNotify()) {
                if (!pChr->subscribe(true, notifyCallback1)) {
                    pClient->disconnect();
                   
                }
            }
        }
    }
    } 
}
}



void setup()
{
  Serial.begin(115200);     
  LCD_Init();
  Set_Backlight(14);
  Lvgl_Init();
  ui_init();

  
    delay(200);
    NimBLEDevice::init("");

    // Set up the BLE scan for both devices
    NimBLEScan* pBLEScan = NimBLEDevice::getScan();
    //NimBLEDevice::setPower(3); 
    pBLEScan->setActiveScan(true);  // Enable active scan for better results
    pBLEScan->setInterval(45);      // Scan interval
    pBLEScan->setWindow(15);        // Scan window
    pBLEScan->setScanCallbacks(&scanCB, false);
    pBLEScan->start(60000);      // Scan for 60 seconds, using passive scan mode


    while (found <1) {
        delay(100);  // Keep scanning until both devices are found
    }

    NimBLEDevice::getScan()->stop();
    delay(100);
    Serial.println("ajmo se povezati 3");
    // Connect to the first sensor (Sensor 1)
    connectToSensor(targetDevice1, "Sensor 1", 1);
    delay(200);
}

void loop()
{

   
  Timer_Loop();
  RGB_Lamp_Loop(2);
  delay(5);
}
