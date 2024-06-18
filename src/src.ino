#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEBeacon.h"
#include <SPIFFS.h>

#define FLOAT_1 2
#define FLOAT_2 15

#define LED 2

typedef struct {
    double x;
    double y;
} DataPoint;

DataPoint refs1[] = {
    {1300, 100},
    {1370, 90},
    {1460, 74},
    {1585, 56},
    {1720, 0},
    {1830, 0}
};

DataPoint refs2[] = {
    {1310, 100},
    {1385, 83},
    {1465, 69},
    {1520, 57},
    {1655, 44},
    {1735, 31},
    {1830, 18},
    {1935, 0}
};

double findClosest(int x, DataPoint* refs, int dataCount) {
    int bestId = -1;

    for (int i = 0; i < dataCount; i++) {
        if(bestId == -1 || abs(refs[i].x-x) < abs(refs[bestId].x-x)){
          bestId = i;
        }
    }
    return refs[bestId].y;
}

BLEAdvertising *advertising;

double float1_permille = 0;
double float2_permille = 0;

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
  
  float1_permille = findClosest(float1_buffer / 50, refs1, 6);
  float2_permille = findClosest(float2_buffer / 50, refs2, 8);

  set_beacon();
  digitalWrite(LED, HIGH);
  advertising->start();
  delay(100);
  advertising->stop();
  digitalWrite(LED, LOW);
}
