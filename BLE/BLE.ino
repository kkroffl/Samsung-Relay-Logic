#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLEAdvertising.h>

BLEAdvertising *pAdvertising;

String esp32Address;

void setup() {

  Serial.begin(115200);
  delay(1000);

  Serial.println("\nESP32 OWN BLE ADDRESS TRANSMITTER");

  // Initialize BLE
  BLEDevice::init("ESP32_ADDRESS");

  // Get ESP32's own BLE MAC address
  esp32Address =
    BLEDevice::getAddress().toString().c_str();

  esp32Address.toUpperCase();

  Serial.println("--------------------------------");

  Serial.print("ESP32 BLE MAC ADDRESS: ");
  Serial.println(esp32Address);

  // Create BLE server
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService =
    pServer->createService(
      "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
    );

  // Create characteristic to expose own address
  BLECharacteristic *pCharacteristic =
    pService->createCharacteristic(
      "6E400003-B5A3-F393-E0A9-E50E24DCCA9E",
      BLECharacteristic::PROPERTY_READ |
      BLECharacteristic::PROPERTY_NOTIFY
    );

  // Store own MAC address as characteristic value
  pCharacteristic->setValue(esp32Address.c_str());

  pService->start();

  // Configure advertising
  pAdvertising = BLEDevice::getAdvertising();

  pAdvertising->addServiceUUID(
    "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
  );

  pAdvertising->setScanResponse(true);

  pAdvertising->start();

  Serial.println("--------------------------------");
  Serial.println("ESP32 IS BROADCASTING ITS OWN ADDRESS");
  Serial.println("Bluetooth advertising started");
}

void loop() {

  delay(5000);

  Serial.print("MY ESP32 BLE ADDRESS: ");
  Serial.println(esp32Address);
}