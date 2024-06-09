#define BLYNK_TEMPLATE_ID "TMPL3Ar5tHpn0"
#define BLYNK_TEMPLATE_NAME "Forest Fire Detection"
#include <SPI.h>
#include <LoRa.h>
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>

// LoRa pin configuration
#define SCK     D5    // GPIO 14 - SCK
#define MISO    D6    // GPIO 12 - MISO
#define MOSI    D7    // GPIO 13 - MOSI
#define SS      D8    // GPIO 15 - NSS
#define RST     D0    // GPIO 16 - RESET
#define DIO0    D4    // GPIO 2 - DIO0

// Replace with your network credentials
const char* ssid = "W";
const char* password = "12345678";
char auth[] = "1z1W6A43BuSTQhwbyutV0ZwCzLiz0fXA";

// Define thresholds for sensor readings
const float TEMP_THRESHOLD = 45.0; // Critical temperature threshold (°C)
const float HUMIDITY_THRESHOLD = 20.0; // Critical humidity threshold (%)
const int LDR_THRESHOLD = 800; // Example threshold for light intensity (LDR)
const float DISTANCE_THRESHOLD = 100.0; // Example threshold for distance (cm)
const int SOIL_MOISTURE_THRESHOLD = 300; // Example threshold for soil moistureCritical: Low soil moisture and high temperature!


// Variables to store the last sensor readings
float lastTemperature = 0.0;
float lastHumidity = 0.0;
int lastLDR = 0;
float lastDistance = 0.0;
int lastSoilMoisture = 0;

void setup() {
  Serial.begin(115200);

  // Initialize LoRa
  LoRa.setPins(SS, RST, DIO0); // Set LoRa pins
  if (!LoRa.begin(433E6)) {    // Set LoRa frequency (433 MHz)
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  // Connect to Wi-Fi using Blynk
  Serial.print("Connecting to ");
  Serial.println(ssid);

  Blynk.begin(auth, ssid, password);
}

void loop() {
  // Try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // Received a packet
    Serial.print("Received packet '");

    // Read packet
    String receivedData = "";
    while (LoRa.available()) {
      receivedData += (char)LoRa.read();
    }
    Serial.print(receivedData);
    Serial.println("' with RSSI " + String(LoRa.packetRssi()));

    // Parse and print the received data
    parseAndPrintData(receivedData);
  }

  // Run Blynk
  Blynk.run();
}

void parseAndPrintData(String data) {
  // Split the data by the delimiter and print each value
  int delimiterIndex;
  String token;

  delimiterIndex = data.indexOf("Humidity: ");
  if (delimiterIndex != -1) {
    token = data.substring(delimiterIndex, data.indexOf("%", delimiterIndex) + 1);
    Serial.println(token);
    float humidity = token.substring(10).toFloat();
    Blynk.virtualWrite(V1, humidity);
    Serial.println(humidity);
    lastHumidity = humidity;
  }

  delimiterIndex = data.indexOf("Temperature: ");
  if (delimiterIndex != -1) {
    token = data.substring(delimiterIndex, data.indexOf("°C", delimiterIndex) + 2);
    Serial.println(token);
    float temperature = token.substring(13).toFloat();
    Blynk.virtualWrite(V0, temperature);
    Serial.println(temperature);
    lastTemperature = temperature;
  }

  delimiterIndex = data.indexOf("LDR: ");
  if (delimiterIndex != -1) {
    token = data.substring(delimiterIndex, data.indexOf(" ", delimiterIndex + 5));
    Serial.println(token);
    int ldrValue = token.substring(5).toInt();
    Blynk.virtualWrite(V3, ldrValue);
    Serial.println(ldrValue);
    lastLDR = ldrValue;
  }

  delimiterIndex = data.indexOf("Distance: ");
  if (delimiterIndex != -1) {
    token = data.substring(delimiterIndex, data.indexOf(" cm", delimiterIndex) + 3);
    Serial.println(token);
    float distance = token.substring(10).toFloat();
    Blynk.virtualWrite(V4, distance);
    Serial.println(distance);
    lastDistance = distance;
  }

  delimiterIndex = data.indexOf("Soil Moisture: ");
  if (delimiterIndex != -1) {
    token = data.substring(delimiterIndex);
    Serial.println(token);
    int soilMoisture = token.substring(15).toInt();
    Blynk.virtualWrite(V2, soilMoisture);
    Serial.println(soilMoisture);
    lastSoilMoisture = soilMoisture;
  }

  // Check critical conditions
  checkCriticalConditions();
}

void checkCriticalConditions() {
  if (lastTemperature > TEMP_THRESHOLD && lastHumidity < HUMIDITY_THRESHOLD) {
    Blynk.logEvent("high_temp_low_humidity", "Critical: High temperature and low humidity!");
  }
  if (lastTemperature > TEMP_THRESHOLD && lastLDR > LDR_THRESHOLD) {
    Blynk.logEvent("high_temp_high_light", "Critical: High temperature and high light intensity!");
  }
  if (lastSoilMoisture < SOIL_MOISTURE_THRESHOLD && lastTemperature > TEMP_THRESHOLD) {
    Blynk.logEvent("low_soil_moisture_high_temp", "Critical: Low soil moisture and high temperature!");
  }
}
