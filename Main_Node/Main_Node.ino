/*
  ESP-NOW DHT22 Sensor Node
  Sends temperature and humidity data to the main receiver ESP32 via ESP-NOW
*/

// Include required libraries
#include <WiFi.h>
#include <esp_now.h>
#include "DHT.h"

// Define data structure for DHT22 sensor data
typedef struct struct_message {
    int id;          // Identifier for the sender
    float temperature; // Temperature value
    float humidity;    // Humidity value
} struct_message;

// Create structured data object
struct_message dhtData;

// DHT22 sensor settings
#define DHTPIN 5         // Pin where DHT22 is connected
#define DHTTYPE DHT22    // DHT 22 (AM2302)

// Create DHT object
DHT dht(DHTPIN, DHTTYPE);

// Broadcast MAC address of the receiver ESP32
uint8_t receiverMAC[] = {0x70, 0x04, 0x1D, 0x56, 0x1D, 0x22};  // Replace with receiver's MAC

void setup() {
  // Set up Serial Monitor
  Serial.begin(115200);

  // Start DHT sensor
  dht.begin();

  // Start ESP32 in Station mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the receiver ESP32 as a peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, receiverMAC, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  // Read temperature and humidity from DHT22
  dhtData.temperature = dht.readTemperature();
  dhtData.humidity = dht.readHumidity();
  dhtData.id = 1; // Unique ID for this sender

  // Check if any reads failed and exit early
  if (isnan(dhtData.temperature) || isnan(dhtData.humidity)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Send DHT sensor data to receiver ESP32
  esp_err_t result = esp_now_send(receiverMAC, (uint8_t *) &dhtData, sizeof(dhtData));

  if (result == ESP_OK) {
    Serial.println("DHT Data sent successfully");
  } else {
    Serial.println("Error sending DHT Data");
  }

  delay(2000);  // Send data every 2 seconds
}
