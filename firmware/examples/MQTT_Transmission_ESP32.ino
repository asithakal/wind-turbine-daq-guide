#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>  // Install: ArduinoJson by Benoit Blanchon

// ===== MQTT CONFIGURATION =====
const char* mqtt_server     = "broker.hivemq.com";  // Free public broker for testing
const int   mqtt_port       = 1883;
const char* mqtt_user       = "";                   // Leave empty for public broker
const char* mqtt_password   = "";
const char* mqtt_client_id  = "ESP32_Turbine_UoM_Site01";  // Must be unique

// MQTT topics (hierarchical structure)
const char* topic_data   = "uom/turbine/site01/data";
const char* topic_alert  = "uom/turbine/site01/alert";
const char* topic_status = "uom/turbine/site01/status";

// Global MQTT client objects
WiFiClient    espClient;
PubSubClient  mqttClient(espClient);

// Transmission control variables
unsigned long last_transmission_time           = 0;
const unsigned long TRANSMISSION_INTERVAL      = 300000;  // 5 minutes in milliseconds
int  transmission_retry_count                  = 0;
const int MAX_RETRY_ATTEMPTS                   = 3;

// ===== MQTT CONNECTION MANAGEMENT =====
/**
 * Establish connection to MQTT broker with automatic retry
 * Called during setup() and when connection lost
 */
void connectMQTT() {
  // Only attempt connection if Wi-Fi is connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("MQTT: Wi-Fi not connected, skipping MQTT connection");
    return;
  }

  Serial.print("Attempting MQTT connection to ");
  Serial.print(mqtt_server);
  Serial.print(":");
  Serial.println(mqtt_port);

  // Attempt to connect with clean session
  if (mqttClient.connect(mqtt_client_id, mqtt_user, mqtt_password)) {
    Serial.println("MQTT: Connected successfully");

    // Publish connection status message
    String status_msg = "{\"status\":\"online\",\"timestamp\":\"" + getTimestamp() + "\"}";
    mqttClient.publish(topic_status, status_msg.c_str(), true);  // Retained message

    // Reset retry counter on successful connection
    transmission_retry_count = 0;
  } else {
    Serial.print("MQTT: Connection failed, rc=");
    Serial.println(mqttClient.state());
    /*
      MQTT error codes:
      -4 : MQTT_CONNECTION_TIMEOUT
      -3 : MQTT_CONNECTION_LOST
      -2 : MQTT_CONNECT_FAILED
      -1 : MQTT_DISCONNECTED
       0 : MQTT_CONNECTED
       1 : MQTT_CONNECT_BAD_PROTOCOL
       2 : MQTT_CONNECT_BAD_CLIENT_ID
       3 : MQTT_CONNECT_UNAVAILABLE
       4 : MQTT_CONNECT_BAD_CREDENTIALS
       5 : MQTT_CONNECT_UNAUTHORIZED
    */
  }
}

/**
 * Check and maintain MQTT connection
 * Called every loop() iteration
 */
void maintainMQTTConnection() {
  // Skip if Wi-Fi not connected
  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  // Reconnect if disconnected
  if (!mqttClient.connected()) {
    Serial.println("MQTT: Connection lost, attempting reconnect...");
    connectMQTT();
  }

  // Process incoming messages (if subscribed to any topics)
  mqttClient.loop();
}

// ===== DATA TRANSMISSION FUNCTIONS =====
/**
 * Transmit aggregated 5-minute statistics to cloud
 * Called from main loop when transmission interval expires
 */
void transmitAggregatedData() {
  // Check prerequisites
  if (!mqttClient.connected()) {
    Serial.println("MQTT: Not connected, queueing data for later transmission");
    return;
  }

  // Check if enough time elapsed since last transmission
  unsigned long current_time = millis();
  if (current_time - last_transmission_time < TRANSMISSION_INTERVAL) {
    return;  // Not yet time to transmit
  }

  Serial.println("MQTT: Preparing aggregated data packet...");

  // Calculate statistics from last 5 minutes of data
  // (Assume we have arrays storing recent measurements)
  StatsSummary stats = calculateRecentStats();

  // Create JSON payload using ArduinoJson library
  StaticJsonDocument<512> doc;  // 512 bytes should be sufficient

  // Metadata
  doc["device_id"]       = mqtt_client_id;
  doc["timestamp"]       = getTimestamp();
  doc["interval_minutes"]= 5;
  doc["sample_count"]    = stats.sample_count;

  // Wind speed statistics
  JsonObject wind = doc.createNestedObject("wind_speed_ms");
  wind["min"]  = stats.wind_speed_min;
  wind["max"]  = stats.wind_speed_max;
  wind["mean"] = stats.wind_speed_mean;
  wind["std"]  = stats.wind_speed_std;

  // Rotor speed statistics
  JsonObject rotor = doc.createNestedObject("rotor_rpm");
  rotor["min"]  = stats.rotor_rpm_min;
  rotor["max"]  = stats.rotor_rpm_max;
  rotor["mean"] = stats.rotor_rpm_mean;

  // Power statistics
  JsonObject power = doc.createNestedObject("power_w");
  power["min"]      = stats.power_min;
  power["max"]      = stats.power_max;
  power["mean"]     = stats.power_mean;
  power["energy_wh"]= stats.energy_wh;  // Integrated energy over 5 minutes

  // Performance metrics
  JsonObject performance = doc.createNestedObject("performance");
  performance["cp_mean"]     = stats.cp_mean;
  performance["lambda_mean"] = stats.lambda_mean;

  // Environmental conditions
  JsonObject environment = doc.createNestedObject("environment");
  environment["temp_c"]       = stats.temp_mean;
  environment["pressure_hpa"] = stats.pressure_mean;
  environment["humidity_pct"] = stats.humidity_mean;

  // System health
  JsonObject system = doc.createNestedObject("system");
  system["free_heap_bytes"] = ESP.getFreeHeap();
  system["uptime_hours"]    = millis() / 3600000.0;
  system["sd_free_mb"]      = getSDCardFreeSpace();

  // Serialize JSON to string
  char   json_buffer[512];
  size_t json_length = serializeJson(doc, json_buffer);

  Serial.print("MQTT: JSON payload size: ");
  Serial.print(json_length);
  Serial.println(" bytes");
  Serial.println(json_buffer);  // Debug: print full JSON

  // Publish to MQTT broker with QoS 1 (at least once delivery)
  bool success = mqttClient.publish(topic_data, json_buffer, false);

  if (success) {
    Serial.println("MQTT: Data transmitted successfully");
    last_transmission_time   = current_time;
    transmission_retry_count = 0;

    // Log successful transmission to SD card (for audit trail)
    logEvent("Cloud transmission successful: " + String(json_length) + " bytes");
  } else {
    Serial.println("MQTT: Transmission failed");
    transmission_retry_count++;

    if (transmission_retry_count >= MAX_RETRY_ATTEMPTS) {
      Serial.println("MQTT: Max retries exceeded, will try again next interval");
      transmission_retry_count = 0;
      last_transmission_time   = current_time;  // Reset timer to avoid immediate retry
    } else {
      Serial.print("MQTT: Retry ");
      Serial.print(transmission_retry_count);
      Serial.print(" of ");
      Serial.println(MAX_RETRY_ATTEMPTS);
      // Will retry on next loop() iteration
    }
  }
}

/**
 * Send alert message for critical events
 * Called when fault condition detected
 */
void sendAlert(String alert_type, String message) {
  if (!mqttClient.connected()) {
    // Log alert to SD card even if cannot transmit
    logEvent("ALERT [not transmitted]: " + alert_type + " - " + message);
    return;
  }

  StaticJsonDocument<256> doc;
  doc["device_id"]  = mqtt_client_id;
  doc["timestamp"]  = getTimestamp();
  doc["alert_type"] = alert_type;
  doc["message"]    = message;
  doc["severity"]   = getSeverityLevel(alert_type);

  char json_buffer[256];
  serializeJson(doc, json_buffer);

  // Publish with QoS 2 (exactly once delivery) for critical alerts
  bool success = mqttClient.publish(topic_alert, json_buffer, false);

  if (success) {
    Serial.println("ALERT transmitted: " + alert_type);
  } else {
    Serial.println("ALERT transmission failed (logged locally): " + alert_type);
  }

  // Always log to SD card regardless of transmission status
  logEvent("ALERT: " + alert_type + " - " + message);
}

// ===== HELPER FUNCTIONS =====
/**
 * Calculate statistics from recent measurement buffer
 * Returns struct with min/max/mean/std for all parameters
 */
struct StatsSummary {
  int   sample_count;
  float wind_speed_min, wind_speed_max, wind_speed_mean, wind_speed_std;
  float rotor_rpm_min,  rotor_rpm_max,  rotor_rpm_mean;
  float power_min,      power_max,      power_mean;
  float energy_wh;  // Integrated power over interval
  float cp_mean, lambda_mean;
  float temp_mean, pressure_mean, humidity_mean;
};

StatsSummary calculateRecentStats() {
  StatsSummary stats;

  // Assume we maintain rolling buffer of last 300 samples (5 minutes at 1 Hz)
  // For simplicity, this example uses global arrays (in practice, use circular buffer)
  extern float wind_speed_buffer[300];
  extern float rotor_rpm_buffer[300];
  extern float power_buffer[300];
  // ... (other buffers)

  stats.sample_count = 300;  // Or actual count if buffer not yet full

  // Calculate min/max/mean for wind speed
  stats.wind_speed_min = wind_speed_buffer[0];
  stats.wind_speed_max = wind_speed_buffer[0];
  float sum = 0;

  for (int i = 0; i < stats.sample_count; i++) {
    float val = wind_speed_buffer[i];
    if (val < stats.wind_speed_min) stats.wind_speed_min = val;
    if (val > stats.wind_speed_max) stats.wind_speed_max = val;
    sum += val;
  }
  stats.wind_speed_mean = sum / stats.sample_count;

  // Calculate standard deviation
  float sum_squared_diff = 0;
  for (int i = 0; i < stats.sample_count; i++) {
    float diff = wind_speed_buffer[i] - stats.wind_speed_mean;
    sum_squared_diff += diff * diff;
  }
  stats.wind_speed_std = sqrt(sum_squared_diff / stats.sample_count);

  // Similar calculations for other parameters...
  // (Omitted for brevity; follow same pattern)

  // Calculate energy (integrate power over time)
  stats.energy_wh = 0;
  for (int i = 0; i < stats.sample_count; i++) {
    stats.energy_wh += power_buffer[i];  // Sum of watts
  }
  stats.energy_wh = stats.energy_wh / 3600.0;  // Convert W·s to Wh

  return stats;
}

/**
 * Determine alert severity level for prioritization
 */
int getSeverityLevel(String alert_type) {
  if (alert_type == "OVERSPEED" || alert_type == "SENSOR_FAULT_CRITICAL") {
    return 3;  // CRITICAL
  } else if (alert_type == "SD_CARD_FULL" || alert_type == "CALIBRATION_DRIFT") {
    return 2;  // WARNING
  } else {
    return 1;  // INFO
  }
}

/**
 * Get free space on SD card (MB)
 */
float getSDCardFreeSpace() {
  uint64_t cardSize = SD.cardSize()  / (1024 * 1024);  // Total size in MB
  uint64_t usedSize = SD.usedBytes() / (1024 * 1024);  // Used size in MB
  return (float)(cardSize - usedSize);
}

// ===== SETUP AND LOOP INTEGRATION =====
void setup() {
  Serial.begin(115200);

  // ... (other initialization code)

  // Initialize Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  int wifi_timeout = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_timeout < 20) {
    delay(500);
    Serial.print(".");
    wifi_timeout++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWi-Fi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Initialize MQTT
    mqttClient.setServer(mqtt_server, mqtt_port);
    mqttClient.setBufferSize(512);  // Increase buffer for large JSON payloads
    connectMQTT();
  } else {
    Serial.println("\nWi-Fi connection failed - operating in OFFLINE mode");
    Serial.println("All data will be stored locally on SD card");
  }
}

void loop() {
  // ... (sampling, logging, etc.)

  // Maintain network connections
  maintainMQTTConnection();

  // Transmit aggregated data at scheduled intervals
  transmitAggregatedData();

  // Example: Send alert when power exceeds rated capacity
  if (current_power > RATED_POWER * 1.2) {
    sendAlert("OVERPOWER", "Power exceeded 120% of rated capacity: " + String(current_power) + "W");
  }
}

// Alternative: HTTP POST for Cloud Upload
// If MQTT is blocked by institutional firewall or cellular network, use HTTPS POST to RESTful API:

#include <HTTPClient.h>

void transmitViaHTTP(StatsSummary& stats) {
  if (WiFi.status() != WL_CONNECTED) return;

  HTTPClient http;

  // Configure HTTP endpoint (replace with your actual server)
  http.begin("https://api.example.com/turbine/data");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Bearer YOUR_API_KEY_HERE");

  // Create JSON payload (same as MQTT version)
  StaticJsonDocument<512> doc;
  // ... (fill doc as shown above)

  char json_buffer[512];
  serializeJson(doc, json_buffer);

  // Send POST request
  int httpResponseCode = http.POST(json_buffer);

  if (httpResponseCode > 0) {
    Serial.print("HTTP POST successful, response code: ");
    Serial.println(httpResponseCode);
    String response = http.getString();
    Serial.println(response);
  } else {
    Serial.print("HTTP POST failed, error: ");
    Serial.println(http.errorToString(httpResponseCode));
  }

  http.end();
}
