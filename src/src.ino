#include "BLEDevice.h"
#include "BLEUtils.h"
#include "BLEBeacon.h"
#include <SPIFFS.h>

#define FLOAT_1 15
#define FLOAT_2 2
#define FLOAT_3 4

#define LED 2

typedef struct {
    double x;
    double y;
} DataPoint;

DataPoint refs_1[] = {
    {1665, 100},
    {1587, 88},
    {1499, 80},
    {1408, 73},
    {1310, 65},
    {1200, 58},
    {1107, 50},
    {1004, 42},
    {865, 34},
    {715, 27},
    {549, 19},
    {240, 12},
    {0, 0},
};

DataPoint refs_2[] = {
    {1665, 100},
    {1587, 88},
    {1499, 80},
    {1408, 73},
    {1310, 65},
    {1200, 58},
    {1107, 50},
    {1004, 42},
    {865, 34},
    {715, 27},
    {549, 19},
    {240, 12},
    {0, 0},
};

DataPoint refs_3[] = {
    {1665, 100},
    {1587, 88},
    {1499, 80},
    {1408, 73},
    {1310, 65},
    {1200, 58},
    {1107, 50},
    {1004, 42},
    {865, 34},
    {715, 27},
    {549, 19},
    {240, 12},
    {0, 0},
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
double float3_permille = 0;

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
    mfdata += (char)float3_permille;
    advdata.setManufacturerData(mfdata);
    advertising->setAdvertisementData(advdata);
    advertising->setScanResponseData(scanresponse);
}

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);

  pinMode(FLOAT_1, INPUT);
  pinMode(FLOAT_2, INPUT);
  pinMode(FLOAT_3, INPUT);
  analogReadResolution(12);
  Serial.begin(115200);
  BLEDevice::init("ESP32+watersensor");
  advertising = BLEDevice::getAdvertising();
}

void loop() {
  int float1_buffer = 0;
  int float2_buffer = 0;
  int float3_buffer = 0;
  
  for(int i = 0; i < 50; i++){
    float1_buffer += analogRead(FLOAT_1);
    float2_buffer += analogRead(FLOAT_2);
    float3_buffer += analogRead(FLOAT_3);
    delay(100);
  }
  
  float1_permille = findClosest(float1_buffer / 50, refs_1, 13);
  float2_permille = findClosest(float2_buffer / 50, refs_2, 13);
  float3_permille = findClosest(float3_buffer / 50, refs_3, 13);

  Serial.print(float1_buffer / 50);
  Serial.print(" ");
  Serial.print(float2_buffer / 50);
  Serial.print(" ");
  Serial.print(float3_buffer / 50);
  Serial.println();

  set_beacon();
  digitalWrite(LED, HIGH);
  advertising->start();
  delay(100);
  advertising->stop();
  digitalWrite(LED, LOW);
}
