/*
 * Example 07: MQTT Cloud Transmission
 * 
 * Purpose: Send sensor data to cloud MQTT broker
 * 
 * Learning Objectives:
 * - Use MQTT protocol for IoT communication
 * - Connect to public/private MQTT brokers
 * - Publish sensor data in JSON format
 * - Implement reconnection logic
 * - Balance local SD logging with cloud transmission
 * 
 * Hardware Required:
 * - ESP32 board
 * - WiFi network with internet access
 * 
 * Connections:
 * - None (uses built-in WiFi)
 * 
 * MQTT Broker Options:
 * - HiveMQ (public, no auth): broker.hivemq.com:1883
 * - Mosquitto (local): localhost:1883
 * - InfluxDB/Grafana: your-server:1883
 * 
 * Author: Wind Turbine DAQ Guide
 * License: MIT
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* WIFI_SSID = "YourWiFiNetwork";
const char* WIFI_PASSWORD = "YourPassword";

// MQTT broker configuration
const char* MQTT_BROKER = "broker.hivemq.com";  // Public test broker
const int MQTT_PORT = 1883;
const char* MQTT_CLIENT_ID = "ESP32_Turbine_001";  // Must be unique!

// MQTT topics (hierarchical structure)
const char* TOPIC_DATA = "turbine/turbine001/data";
const char* TOPIC_STATUS = "turbine/turbine001/status";
const char* TOPIC_ERRORS = "turbine/turbine001/errors";

// MQTT authentication (if required by broker)
// const char* MQTT_USERNAME = "your_username";
// const char* MQTT_PASSWORD = "your_password";

// Publishing configuration
const int PUBLISH_INTERVAL_MS = 5000;  // Send data every 5 seconds
unsigned long lastPublishTime = 0;

// Simulated sensor data
float windSpeed = 5.5;
float power = 125.0;
float rpm = 245.0;
float temperature = 28.5;

// Objects
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// Status flags
bool mqttConnected = false;
int reconnectAttempts = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=================================");
  Serial.println("MQTT Cloud Transmission - Example 07");
  Serial.println("=================================");
  Serial.println();
  
  // Connect to WiFi
  connectWiFi();
  
  // Configure MQTT
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  mqttClient.setCallback(mqttCallback);  // For receiving messages (optional)
  
  // Connect to MQTT broker
  connectMQTT();
  
  Serial.println();
  Serial.println("Publishing sensor data to cloud...");
  Serial.println("Time(s)\tWind(m/s)\tPower(W)\tRPM\tStatus");
  Serial.println("------------------------------------------------------------");
}

void loop() {
  // Maintain MQTT connection
  if (!mqttClient.connected()) {
    Serial.println("MQTT disconnected. Reconnecting...");
    connectMQTT();
  }
  mqttClient.loop();  // Process incoming messages and keep connection alive
  
  unsigned long currentTime = millis();
  
  // Publish data periodically
  if (currentTime - lastPublishTime >= PUBLISH_INTERVAL_MS) {
    // Simulate sensor readings (replace with real sensors)
    windSpeed = 5.5 + (random(-100, 100) / 100.0);
    power = 125.0 + (random(-500, 500) / 10.0);
    rpm = 245.0 + (random(-50, 50) / 10.0);
    temperature = 28.5 + (random(-20, 20) / 10.0);
    
    // Publish to MQTT
    bool success = publishSensorData();
    
    // Print to Serial Monitor
    Serial.print(currentTime / 1000.0, 1);
    Serial.print("\t");
    Serial.print(windSpeed, 2);
    Serial.print("\t\t");
    Serial.print(power, 1);
    Serial.print("\t\t");
    Serial.print(rpm, 1);
    Serial.print("\t");
    Serial.println(success ? "Published" : "FAILED");
    
    lastPublishTime = currentTime;
  }
  
  delay(100);  // Small delay to prevent tight loop
}

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("WiFi connection FAILED!");
  }
}

void connectMQTT() {
  while (!mqttClient.connected() && reconnectAttempts < 5) {
    Serial.print("Connecting to MQTT broker: ");
    Serial.println(MQTT_BROKER);
    
    // Attempt connection
    // For brokers requiring authentication, use:
    // if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
    if (mqttClient.connect(MQTT_CLIENT_ID)) {
      Serial.println("MQTT connected!");
      mqttConnected = true;
      reconnectAttempts = 0;
      
      // Publish status message
      mqttClient.publish(TOPIC_STATUS, "online", true);  // Retained message
      
      // Subscribe to command topic (optional)
      // mqttClient.subscribe("turbine/turbine001/commands");
      
    } else {
      Serial.print("MQTT connection FAILED, rc=");
      Serial.println(mqttClient.state());
      reconnectAttempts++;
      delay(2000);
    }
  }
  
  if (!mqttClient.connected()) {
    Serial.println("MQTT connection failed after 5 attempts. Continuing without cloud...");
    mqttConnected = false;
  }
}

bool publishSensorData() {
  if (!mqttClient.connected()) {
    return false;
  }
  
  // Create JSON document
  StaticJsonDocument<256> doc;
  
  doc["timestamp"] = millis();
  doc["wind_speed_ms"] = windSpeed;
  doc["power_w"] = power;
  doc["rpm"] = rpm;
  doc["temperature_c"] = temperature;
  
  // Calculate derived values
  float tipSpeedRatio = (rpm * 2 * PI * 0.6) / (60.0 * windSpeed);  // R=0.6m
  doc["lambda"] = tipSpeedRatio;
  
  // Serialize JSON to string
  char jsonBuffer[256];
  serializeJson(doc, jsonBuffer);
  
  // Publish to MQTT
  bool success = mqttClient.publish(TOPIC_DATA, jsonBuffer);
  
  if (!success) {
    Serial.println("MQTT publish failed - message too large or connection lost");
  }
  
  return success;
}

// Callback for incoming MQTT messages (optional)
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  
  Serial.print("Payload: ");
  for (unsigned int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  
  // Parse commands here if needed
  // Example: If payload is "reset", restart ESP32
}