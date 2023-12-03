#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#include <iostream>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define DEVICE_NAME         "TriboSamplerV1"
#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

BLECharacteristic *pCharacteristic = NULL;

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string value = pCharacteristic->getValue();

      if (value.length() > 0) {
        Serial.println("*********");
        Serial.print("New value: ");
        for (int i = 0; i < value.length(); i++)
          Serial.print(value[i]);

        Serial.println();
        Serial.println("*********");
      }
    }
};

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      pServer->startAdvertising(); // restart advertising
    };

    void onDisconnect(BLEServer* pServer) {
      pServer->startAdvertising(); // restart advertising
    }
};

unsigned long sampTimer = 0;
unsigned long sampleInterval = 25; //100 ms = 10Hz rate

void setup() {
  Serial.begin(115200);

  BLEDevice::init(DEVICE_NAME);
  BLEServer *pServer = BLEDevice::createServer();

  pServer->setCallbacks(new MyServerCallbacks());

  BLEService *pService = pServer->createService(SERVICE_UUID);

  pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  pCharacteristic->setValue("Hello World");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

int i = 0;

void loop() {
  unsigned long currMillis = millis();
  if(currMillis - sampTimer >= sampleInterval){
    sampTimer = currMillis;
    std::string stringValue = std::to_string(analogRead(A0));
    std::string time = std::to_string(currMillis);
    stringValue = stringValue + "|" + time;
    pCharacteristic->setValue(stringValue);
  }
}