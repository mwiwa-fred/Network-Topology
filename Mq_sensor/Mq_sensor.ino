/*
  ESP-NOW MQ Sensor Node with Alarm
  Sends air quality data to the receiver ESP32 via ESP-NOW.
  Activates red LED and buzzer if gas level exceeds threshold.
*/

#include <WiFi.h>
#include <esp_now.h>

// Define data structure for MQ sensor data
typedef struct struct_message {
    int gasLevel;
} struct_message;

// Create structured data object
struct_message mqData;

// Pin definitions
const int MQ_ANALOG_PIN = A4;     // Analog pin for MQ-2 sensor
const int RED_LED_PIN = 12;       // Pin for red LED (alarm indicator)
const int GREEN_LED_PIN = 20;     // Pin for green LED (normal status)
const int BUZZER_PIN = 39;        // Pin for buzzer

// Threshold value for gas detection
const int SENSOR_THRESHOLD = 600;

// Receiver's MAC address (replace with actual receiver MAC address)
uint8_t receiverMAC[] = {0x70, 0x04, 0x1D, 0x56, 0x1D, 0x22};  // Replace with receiver's MAC

void setup() {
  Serial.begin(115200);

  // Set pin modes
  pinMode(RED_LED_PIN, OUTPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(MQ_ANALOG_PIN, INPUT);

  // Turn on green LED initially
  digitalWrite(GREEN_LED_PIN, HIGH);
  digitalWrite(RED_LED_PIN, LOW);
  noTone(BUZZER_PIN);

  // Start ESP32 in station mode
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register receiver ESP32 as a peer
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
  // Read the gas level from MQ sensor
  mqData.gasLevel = analogRead(MQ_ANALOG_PIN);

  // Display gas level on Serial Monitor
  Serial.print("Gas Level: ");
  Serial.println(mqData.gasLevel);

  // Check if gas level exceeds the threshold
  if (mqData.gasLevel > SENSOR_THRESHOLD) {
    // Trigger alarm: Red LED ON, Green LED OFF, Buzzer ON
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
    tone(BUZZER_PIN, 1000, 200); // 1000 Hz frequency, 200 ms duration
  } else {
    // No alarm: Green LED ON, Red LED OFF, Buzzer OFF
    digitalWrite(RED_LED_PIN, LOW);
    digitalWrite(GREEN_LED_PIN, HIGH);
    noTone(BUZZER_PIN);
  }

  // Send gas level data to receiver ESP32
  esp_err_t result = esp_now_send(receiverMAC, (uint8_t *) &mqData, sizeof(mqData));

  if (result == ESP_OK) {
    Serial.println("Data sent successfully");
  } else {
    Serial.println("Error sending data");
  }

  delay(3000);  // Delay between readings
}
