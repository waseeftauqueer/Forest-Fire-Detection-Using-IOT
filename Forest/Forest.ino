#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>

// Define the pins used by the DHT11 sensor
#define DHTPIN 15          // GPIO 15
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Define the pins used by the LDR and Soil Moisture sensor
#define LDRPIN 4         // GPIO 4 (ADC1_6)
#define SOILPIN 35        // GPIO 35 (ADC1_7)

// Define the pins used by the Ultrasonic sensor
#define TRIGPIN 5        // GPIO 5
#define ECHOPIN 18        // GPIO 18

// Define the LoRa pins
#define SCK     21    // GPIO 21 - SCK
#define MISO    19   // GPIO 19 - MISO
#define MOSI    23   // GPIO 23 - MOSI
#define SS      2   // GPIO 2 - NSS
#define RST     25   // GPIO 25 - RESET
#define DIO0    26   // GPIO 26 - DIO0

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize DHT sensor
  dht.begin();

  // Initialize Ultrasonic sensor pins
  pinMode(TRIGPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);

  // Initialize LoRa module
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);

  if (!LoRa.begin(433E6)) {  // Set frequency to 433MHz
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  Serial.println("LoRa Transmitter Initialized");
}

void loop() {
  // Read DHT11 sensor
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  // Read LDR value
  int ldrValue = analogRead(LDRPIN);

  // Read Ultrasonic sensor
  digitalWrite(TRIGPIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGPIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGPIN, LOW);
  long duration = pulseIn(ECHOPIN, HIGH);
  float distance = duration * 0.034 / 2;

  // Read Soil moisture sensor
  int soilMoistureValue = analogRead(SOILPIN);

  // Print values to Serial Monitor
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print("%  Temperature: ");
  Serial.print(t);
  Serial.print("°C  LDR: ");
  Serial.print(ldrValue);
  Serial.print("  Distance: ");
  Serial.print(distance);
  Serial.print(" cm  Soil Moisture: ");
  Serial.println(soilMoistureValue);

  // Transmit data via LoRa
  LoRa.beginPacket();
  LoRa.print("Humidity: ");
  LoRa.print(h);
  LoRa.print("%  Temperature: ");
  LoRa.print(t);
  LoRa.print("°C  LDR: ");
  LoRa.print(ldrValue);
  LoRa.print("  Distance: ");
  LoRa.print(distance);
  LoRa.print(" cm  Soil Moisture: ");
  LoRa.print(soilMoistureValue);
  LoRa.endPacket();

  delay(2000); // Delay for 2 seconds before the next reading
}
