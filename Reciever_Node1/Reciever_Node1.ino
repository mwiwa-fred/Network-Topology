#include <WiFi.h>
#include <esp_now.h>
#include <PubSubClient.h>

// WiFi credentials
const char* ssid = "fredrick";            // Replace with your WiFi SSID
const char* password = "Green2565";    // Replace with your WiFi password

// HiveMQ server credentials
const char* mqttServer = "ce429bb946364c09b36f4d22902d3f1d.s1.eu.hivemq.cloud"; // HiveMQ broker
const int mqttPort = 1883;                    // Default MQTT port
const char* mqttUser = "hivemq.webclient.1730499568865";                    // Optional, if required by your HiveMQ setup
const char* mqttPassword = "Sa691uAoXM#>pn<RdN0%";                // Optional, if required by your HiveMQ setup

// Define data structure for DHT22 sensor data
typedef struct struct_message {
    int id;            // Identifier for the sender
    float temperature; // Temperature value
    float humidity;    // Humidity value
} struct_message;

// Define data structure for MQ sensor data
typedef struct struct_mq_message {
    int id;           // Identifier for the sender
    int gasLevel;     // Air quality (gas level) value
} struct_mq_message;

// Create structured data objects
struct_message dhtData;
struct_mq_message mqData;

// MQTT client
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Callback function to receive ESP-NOW data
void OnDataRecv(const esp_now_recv_info* info, const uint8_t* incomingData, int len) 
{
    Serial.print("Received data length: ");
    Serial.println(len); // Log the length of the incoming data

    if (len == sizeof(dhtData)) {
        memcpy(&dhtData, incomingData, sizeof(dhtData));
        Serial.print("DHT Data - ID: ");
        Serial.println(dhtData.id);
        Serial.print("Temperature: ");
        Serial.println(dhtData.temperature);
        Serial.print("Humidity: ");
        Serial.println(dhtData.humidity);

        // Send DHT data to HiveMQ
        char payload[50];
        snprintf(payload, sizeof(payload), "{\"id\": %d, \"temperature\": %.2f, \"humidity\": %.2f}", dhtData.id, dhtData.temperature, dhtData.humidity);
        mqttClient.publish("esp32/dhtdata", payload);  // Publish to HiveMQ topic
    } 
    else if (len == sizeof(mqData)) {
        memcpy(&mqData, incomingData, sizeof(mqData));
        Serial.print("MQ Data - ID: ");
        Serial.println(mqData.id);
        Serial.print("Gas Level: ");
        Serial.println(mqData.gasLevel);

        // Send MQ data to HiveMQ
        char payload[50];
        snprintf(payload, sizeof(payload), "{\"id\": %d, \"gasLevel\": %d}", mqData.id, mqData.gasLevel);
  
