#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEBeacon.h"
#include <SPIFFS.h>

#define FLOAT_1 34
#define FLOAT_2 35

#define LED 2

typedef struct {
    double x;
    double y;
} DataPoint;

DataPoint data[] = {
    {1359, 100},
    {1395, 87.5},
    {1474, 75},
    {1517, 62.5},
    {1603, 50},
    {1644, 37.5},
    {1747, 25},
    {1806, 12.5},
    {2017, 0}
};
int dataCount = sizeof(data) / sizeof(data[0]);

int interpolate(double x) {
    if (x <= data[0].x) return data[0].y;
    if (x >= data[dataCount - 1].x) return data[dataCount - 1].y;

    // Find the right interval for interpolation
    for (int i = 0; i < dataCount - 1; i++) {
        if (x >= data[i].x && x <= data[i+1].x) {
            // Perform linear interpolation
            double slope = (data[i+1].y - data[i].y) / (data[i+1].x - data[i].x);
            return data[i].y + slope * (x - data[i].x);
        }
    }
    return 0;
}

BLEAdvertising *advertising;

int float1_permille = 0;
int float2_permille = 0;

void set_beacon() {
    BLEBeacon beacon = BLEBeacon();
    BLEAdvertisementData advdata = BLEAdvertisementData();
    BLEAdvertisementData scanresponse = BLEAdvertisementData();
    
    advdata.setFlags(0x06); // BR_EDR_NOT_SUPPORTED 0x04 & LE General discoverable 0x02

    std::string mfdata = "";
    mfdata += (char)0xE5; mfdata += (char)0x02;  // Espressif Incorporated Vendor ID = 0x02E5
    mfdata += (char)0x48; mfdata += (char)0xE9;  // Identifier for this sketch is 0xE948 (Oxygen)
    mfdata += (char)float1_permille;
    mfdata += (char)float2_permille;
    Serial.print(float1_permille);
    Serial.print(" ");
    Serial.print(float2_permille);
    Serial.println();
    advdata.setManufacturerData(mfdata);
    advertising->setAdvertisementData(advdata);
    advertising->setScanResponseData(scanresponse);
}

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  pinMode(FLOAT_1, INPUT);
  pinMode(FLOAT_2, INPUT);
  analogReadResolution(12);
  Serial.begin(115200);
  BLEDevice::init("ESP32+watersensor");
  advertising = BLEDevice::getAdvertising();
}

void loop() {
  int float1_buffer = 0;
  int float2_buffer = 0;

  for(int i = 0; i < 50; i++){
    float1_buffer += analogRead(FLOAT_1);
    float2_buffer += analogRead(FLOAT_2);
    delay(100);
  }
  
  float1_permille = interpolate(float1_buffer / 50);
  float2_permille = interpolate(float2_buffer / 50);

  set_beacon();
  digitalWrite(LED, HIGH);
  advertising->start();
  delay(100);
  advertising->stop();
  digitalWrite(LED, LOW);
}
