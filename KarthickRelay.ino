#include <WiFi.h>
#include <ThingSpeak.h>
#include <DHT.h>

// -------- Wi-Fi credentials --------
const char* ssid = "Srivar";
const char* password = "sriram@06";

// -------- ThingSpeak settings --------
unsigned long channelID = 3548817;
const char* writeAPIKey = "WX7FQYPVRK7YSEUU";

// -------- DHT11 configuration --------
#define DHTPIN 21
#define DHTTYPE DHT11
#define RELAY_PIN 19 //

// #define TEMP_ON 30.0 
// #define TEMP_OFF 25.0 

#define RELAY_ON LOW //
#define RELAY_OFF HIGH //

DHT dht(DHTPIN, DHTTYPE);

bool relayState = false; //

WiFiClient client;

void connectWiFi() {
  Serial.print("Connecting to Wi-Fi");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWi-Fi connection failed.");
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(RELAY_PIN, OUTPUT); //

  digitalWrite(RELAY_PIN, RELAY_OFF); //

  Serial.println("\nESP32 DHT11 ThingSpeak Logger");

  dht.begin();
  connectWiFi();

  ThingSpeak.begin(client);
}

void loop() {
  // Reconnect if Wi-Fi disconnects
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi disconnected.");
    connectWiFi();
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Upload skipped. Wi-Fi unavailable.");
    delay(10000);
    return;
  }

  // Read sensor data
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    Serial.println("DHT11 reading failed. Check sensor wiring.");
    delay(20000);
    return;
  }

  // Display sensor readings
  Serial.println("\n----- Sensor Readings -----");

  Serial.print("Temperature: ");
  Serial.print(temperature, 1);
  Serial.println(" °C");

  Serial.print("Humidity: ");
  Serial.print(humidity, 1);
  Serial.println(" %");

  if (temperature >= 30.0) {
    if (relayState == false) {
      relayState = true;

      digitalWrite(RELAY_PIN, RELAY_ON);
      
      Serial.println("Temp Threshold Reached!");
      Serial.println("RELAY TURNED ON!");
    }
  }

  else if (temperature <= 25.0) {
    if (relayState = true) {
      relayState = false;

      digitalWrite(RELAY_PIN, RELAY_OFF);
      
      Serial.println("Temp Below Threshold!");
      Serial.println("RELAY TURNED OFF!");
    }
  }

  // Prepare ThingSpeak fields
  ThingSpeak.setField(1, temperature);
  ThingSpeak.setField(2, humidity);

  // Upload both values in one channel update
  int responseCode =
      ThingSpeak.writeFields(channelID, writeAPIKey);

  if (responseCode == 200) {
    Serial.println("ThingSpeak upload successful!");
  } else {
    Serial.print("ThingSpeak upload failed. HTTP code: ");
    Serial.println(responseCode);
  }

  Serial.println("---------------------------");

  // Wait 20 seconds between uploads
  delay(1000);
}