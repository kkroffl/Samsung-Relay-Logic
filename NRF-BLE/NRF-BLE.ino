#include <Arduino.h>
#include <DHT.h>

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

// ---------------- PIN CONFIGURATION ----------------

#define DHTPIN 21
#define DHTTYPE DHT11
#define RELAY_PIN 19

// ---------------- TEMPERATURE THRESHOLDS ----------------

#define TEMP_ON 30.0
#define TEMP_OFF 28.0

// Active LOW relay module
#define RELAY_ON LOW
#define RELAY_OFF HIGH

// ---------------- BLE UUID ----------------

#define SERVICE_UUID \
"6E400001-B5A3-F393-E0A9-E50E24DCCA9E"

#define CHARACTERISTIC_UUID_RX \
"6E400002-B5A3-F393-E0A9-E50E24DCCA9E"

#define CHARACTERISTIC_UUID_TX \
"6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

// ---------------- OBJECTS ----------------

DHT dht(DHTPIN, DHTTYPE);

BLECharacteristic *pTxCharacteristic;

bool deviceConnected = false;
bool relayState = false;
bool autoMode = false;

float temperature = 0;
float humidity = 0;

unsigned long previousMillis = 0;
const unsigned long interval = 2000;

// ---------------- RELAY CONTROL ----------------

void controlRelay(bool state) {

  relayState = state;

  digitalWrite(
    RELAY_PIN,
    relayState ? RELAY_ON : RELAY_OFF
  );

  Serial.print("Relay Status: ");
  Serial.println(relayState ? "ON" : "OFF");
}

// ---------------- SEND BLE DATA ----------------

void sendBLEData(String message) {

  Serial.println(message);

  if (deviceConnected) {

    pTxCharacteristic->setValue(message.c_str());

    pTxCharacteristic->notify();
  }
}

// ---------------- BLE CALLBACK ----------------

class ServerCallbacks : public BLEServerCallbacks {

  void onConnect(BLEServer *server) override {

    deviceConnected = true;

    Serial.println("Mobile Connected!");
  }

  void onDisconnect(BLEServer *server) override {

    deviceConnected = false;

    Serial.println("Mobile Disconnected!");

    BLEDevice::startAdvertising();
  }
};

// ---------------- MOBILE COMMAND CALLBACK ----------------

class CommandCallbacks :
public BLECharacteristicCallbacks {

  void onWrite(
    BLECharacteristic *characteristic
  ) override {

    String command =
      String(characteristic->getValue().c_str());

    command.trim();
    command.toUpperCase();

    Serial.print("Received Command: ");
    Serial.println(command);

    if (command == "ON") {

      if (!autoMode) {

        controlRelay(true);
        sendBLEData("Relay ON");
      }
      else {

        sendBLEData("Disable AUTO mode first");
      }
    }

    else if (command == "OFF") {

      if (!autoMode) {

        controlRelay(false);
        sendBLEData("Relay OFF");
      }
      else {

        sendBLEData("Disable AUTO mode first");
      }
    }

    else if (command == "AUTO") {

      autoMode = true;

      sendBLEData("Automatic Mode Enabled");
    }

    else if (command == "MANUAL") {

      autoMode = false;

      sendBLEData("Manual Mode Enabled");
    }

    else if (command == "STATUS") {

      String data =
        "Temperature: " + String(temperature, 1) +
        " C, Humidity: " + String(humidity, 1) +
        " %, Relay: " +
        String(relayState ? "ON" : "OFF") +
        ", Mode: " +
        String(autoMode ? "AUTO" : "MANUAL");

      sendBLEData(data);
    }

    else {

      sendBLEData("Invalid Command");
    }
  }
};

// ---------------- SETUP ----------------

void setup() {

  Serial.begin(115200);

  // Relay initialization
  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, RELAY_OFF);

  // DHT initialization
  dht.begin();

  Serial.println();
  Serial.println("ESP32 DHT11 BLE RELAY SYSTEM");
  Serial.println("--------------------------------");

  // Initialize BLE
  BLEDevice::init("ESP32_DHT11_RELAY");

  // Display own BLE address
  String macAddress =
    BLEDevice::getAddress().toString().c_str();

  macAddress.toUpperCase();

  Serial.print("ESP32 BLE MAC: ");
  Serial.println(macAddress);

  // Create BLE server
  BLEServer *pServer =
    BLEDevice::createServer();

  pServer->setCallbacks(new ServerCallbacks());

  // Create BLE service
  BLEService *pService =
    pServer->createService(SERVICE_UUID);

  // TX characteristic
  pTxCharacteristic =
    pService->createCharacteristic(
      CHARACTERISTIC_UUID_TX,
      BLECharacteristic::PROPERTY_NOTIFY |
      BLECharacteristic::PROPERTY_READ
    );

  pTxCharacteristic->addDescriptor(
    new BLE2902()
  );

  // RX characteristic
  BLECharacteristic *pRxCharacteristic =
    pService->createCharacteristic(
      CHARACTERISTIC_UUID_RX,
      BLECharacteristic::PROPERTY_WRITE
    );

  pRxCharacteristic->setCallbacks(
    new CommandCallbacks()
  );

  // Start service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising =
    BLEDevice::getAdvertising();

  pAdvertising->addServiceUUID(SERVICE_UUID);

  pAdvertising->setScanResponse(true);

  BLEDevice::startAdvertising();

  Serial.println("Bluetooth Advertising Started");
  Serial.println("Waiting for Mobile Connection...");
  Serial.println("--------------------------------");
}

// ---------------- MAIN LOOP ----------------

void loop() {

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {

    previousMillis = currentMillis;

    // Read DHT11
    float t = dht.readTemperature();
    float h = dht.readHumidity();

    // Check sensor
    if (isnan(t) || isnan(h)) {

      Serial.println("DHT11 Sensor Error!");

      return;
    }

    temperature = t;
    humidity = h;

    // Automatic relay logic
    if (autoMode) {

      if (temperature >= TEMP_ON) {

        controlRelay(true);
      }

      else if (temperature <= TEMP_OFF) {

        controlRelay(false);
      }
    }

    // Display sensor readings
    Serial.println();
    Serial.println("--------- SENSOR DATA --------");

    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" C");

    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    Serial.print("Relay Status: ");
    Serial.println(relayState ? "ON" : "OFF");

    Serial.print("Operating Mode: ");
    Serial.println(autoMode ? "AUTO" : "MANUAL");

    Serial.println("--------------------------------");

    // Transmit readings to mobile
    String data =
      "Temperature: " + String(temperature, 1) +
      " C, Humidity: " + String(humidity, 1) +
      " %, Relay: " +
      String(relayState ? "ON" : "OFF") +
      ", Mode: " +
      String(autoMode ? "AUTO" : "MANUAL");

    sendBLEData(data);
  }
}