#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

// ================== CONFIGURATION ==================

struct MqttConfig {
  const char* server      = "broker.hivemq.com";
  uint16_t    port        = 1883;
  const char* user        = "";
  const char* password    = "";
  const char* clientId    = "ESP32_Turbine_UoM_Site01";

  const char* topicData   = "uom/turbine/site01/data";
  const char* topicAlert  = "uom/turbine/site01/alert";
  const char* topicStatus = "uom/turbine/site01/status";

  // Transmission policy
  const unsigned long intervalMs     = 5UL * 60UL * 1000UL;  // 5 min
  const uint8_t       maxRetry       = 3;
};

MqttConfig cfg;

WiFiClient    wifiClient;
PubSubClient  mqttClient(wifiClient);

unsigned long lastTxMillis      = 0;
uint8_t       txRetryCount      = 0;

// Forward declarations for external helpers
String getTimestamp();
void   logEvent(const String& msg);

// ================== STATS STRUCT ==================

struct StatsSummary {
  int   sample_count;
  float wind_speed_min, wind_speed_max, wind_speed_mean, wind_speed_std;
  float rotor_rpm_min, rotor_rpm_max, rotor_rpm_mean;
  float power_min, power_max, power_mean;
  float energy_wh;
  float cp_mean, lambda_mean;
  float temp_mean, pressure_mean, humidity_mean;
};

StatsSummary calculateRecentStats();

// ================== MQTT CORE ==================

bool wifiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

bool mqttConnected() {
  return mqttClient.connected();
}

void publishStatusOnline() {
  StaticJsonDocument<128> doc;
  doc["status"]    = "online";
  doc["timestamp"] = getTimestamp();

  char buf[128];
  size_t len = serializeJson(doc, buf);
  mqttClient.publish(cfg.topicStatus, buf, len, true);  // retained
}

void connectMQTT() {
  if (!wifiConnected()) {
    Serial.println(F("MQTT: Wi-Fi not connected, skipping MQTT connection"));
    return;
  }

  Serial.printf("MQTT: Connecting to %s:%u\n", cfg.server, cfg.port);

  if (mqttClient.connect(cfg.clientId, cfg.user, cfg.password)) {
    Serial.println(F("MQTT: Connected"));
    publishStatusOnline();
    txRetryCount = 0;
  } else {
    Serial.printf("MQTT: Connection failed, rc=%d\n", mqttClient.state());
  }
}

void maintainMQTTConnection() {
  if (!wifiConnected()) return;

  if (!mqttConnected()) {
    Serial.println(F("MQTT: Connection lost, reconnecting..."));
    connectMQTT();
  }

  mqttClient.loop();
}

// ================== JSON HELPERS ==================

void fillCommonMetadata(JsonDocument& doc, const StatsSummary& stats) {
  doc["device_id"]      = cfg.clientId;
  doc["timestamp"]      = getTimestamp();
  doc["interval_minutes"] = 5;
  doc["sample_count"]   = stats.sample_count;
}

void fillWindStats(JsonDocument& doc, const StatsSummary& s) {
  JsonObject wind = doc.createNestedObject("wind_speed_ms");
  wind["min"]  = s.wind_speed_min;
  wind["max"]  = s.wind_speed_max;
  wind["mean"] = s.wind_speed_mean;
  wind["std"]  = s.wind_speed_std;
}

void fillRotorStats(JsonDocument& doc, const StatsSummary& s) {
  JsonObject rotor = doc.createNestedObject("rotor_rpm");
  rotor["min"]  = s.rotor_rpm_min;
  rotor["max"]  = s.rotor_rpm_max;
  rotor["mean"] = s.rotor_rpm_mean;
}

void fillPowerStats(JsonDocument& doc, const StatsSummary& s) {
  JsonObject power = doc.createNestedObject("power_w");
  power["min"]      = s.power_min;
  power["max"]      = s.power_max;
  power["mean"]     = s.power_mean;
  power["energy_wh"]= s.energy_wh;
}

void fillPerformance(JsonDocument& doc, const StatsSummary& s) {
  JsonObject perf = doc.createNestedObject("performance");
  perf["cp_mean"]     = s.cp_mean;
  perf["lambda_mean"] = s.lambda_mean;
}

void fillEnvironment(JsonDocument& doc, const StatsSummary& s) {
  JsonObject env = doc.createNestedObject("environment");
  env["temp_c"]       = s.temp_mean;
  env["pressure_hpa"] = s.pressure_mean;
  env["humidity_pct"] = s.humidity_mean;
}

void fillSystem(JsonDocument& doc) {
  JsonObject sys = doc.createNestedObject("system");
  sys["free_heap_bytes"] = ESP.getFreeHeap();
  sys["uptime_hours"]    = millis() / 3600000.0;
  sys["sd_free_mb"]      = getSDCardFreeSpace();
}

// ================== DATA TRANSMISSION ==================

void transmitAggregatedData() {
  if (!mqttConnected()) {
    Serial.println(F("MQTT: Not connected, queueing data for later"));
    return;
  }

  unsigned long now = millis();
  if (now - lastTxMillis < cfg.intervalMs) return;

  Serial.println(F("MQTT: Preparing aggregated data packet..."));

  StatsSummary stats = calculateRecentStats();
  StaticJsonDocument<512> doc;

  fillCommonMetadata(doc, stats);
  fillWindStats(doc, stats);
  fillRotorStats(doc, stats);
  fillPowerStats(doc, stats);
  fillPerformance(doc, stats);
  fillEnvironment(doc, stats);
  fillSystem(doc);

  char buf[512];
  size_t len = serializeJson(doc, buf);

  Serial.printf("MQTT: JSON size=%u bytes\n", (unsigned)len);
  Serial.println(buf);

  bool ok = mqttClient.publish(cfg.topicData, buf, len, false);

  if (ok) {
    Serial.println(F("MQTT: Data transmitted"));
    lastTxMillis   = now;
    txRetryCount   = 0;
    logEvent("Cloud transmission OK: " + String(len) + " bytes");
  } else {
    Serial.println(F("MQTT: Transmission FAILED"));
    txRetryCount++;

    if (txRetryCount >= cfg.maxRetry) {
      Serial.println(F("MQTT: Max retries exceeded, postponing"));
      txRetryCount = 0;
      lastTxMillis = now;
    } else {
      Serial.printf("MQTT: Retry %u/%u\n", txRetryCount, cfg.maxRetry);
    }
  }
}

// ================== ALERTING ==================

int getSeverityLevel(const String& type) {
  if (type == "OVERSPEED" || type == "SENSOR_FAULT_CRITICAL") return 3;
  if (type == "SD_CARD_FULL" || type == "CALIBRATION_DRIFT")  return 2;
  return 1;
}

void sendAlert(const String& alertType, const String& message) {
  if (!mqttConnected()) {
    logEvent("ALERT [offline]: " + alertType + " - " + message);
    return;
  }

  StaticJsonDocument<256> doc;
  doc["device_id"]  = cfg.clientId;
  doc["timestamp"]  = getTimestamp();
  doc["alert_type"] = alertType;
  doc["message"]    = message;
  doc["severity"]   = getSeverityLevel(alertType);

  char buf[256];
  size_t len = serializeJson(doc, buf);
  bool ok = mqttClient.publish(cfg.topicAlert, buf, len, false);

  if (ok)
    Serial.println("ALERT sent: " + alertType);
  else
    Serial.println("ALERT FAILED (logged locally): " + alertType);

  logEvent("ALERT: " + alertType + " - " + message);
}

// ================== SD CARD SPACE ==================

float getSDCardFreeSpace() {
  uint64_t totalMb = SD.cardSize()  / (1024ULL * 1024ULL);
  uint64_t usedMb  = SD.usedBytes() / (1024ULL * 1024ULL);
  return (float)(totalMb - usedMb);
}

// ================== STATS IMPLEMENTATION (EXAMPLE) ==================

extern float wind_speed_buffer[300];
extern float rotor_rpm_buffer[300];
extern float power_buffer[300];

StatsSummary calculateRecentStats() {
  StatsSummary s{};
  s.sample_count = 300;  // or actual count

  // Wind speed stats
  s.wind_speed_min = s.wind_speed_max = wind_speed_buffer[0];
  float sum = 0;

  for (int i = 0; i < s.sample_count; ++i) {
    float v = wind_speed_buffer[i];
    if (v < s.wind_speed_min) s.wind_speed_min = v;
    if (v > s.wind_speed_max) s.wind_speed_max = v;
    sum += v;
  }
  s.wind_speed_mean = sum / s.sample_count;

  float sumSq = 0;
  for (int i = 0; i < s.sample_count; ++i) {
    float d = wind_speed_buffer[i] - s.wind_speed_mean;
    sumSq  += d * d;
  }
  s.wind_speed_std = sqrt(sumSq / s.sample_count);

  // Energy (Wh) from power buffer
  s.energy_wh = 0;
  for (int i = 0; i < s.sample_count; ++i) {
    s.energy_wh += power_buffer[i];
  }
  s.energy_wh /= 3600.0f;

  // TODO: fill rotor / power / env / Cp stats as needed

  return s;
}

// ================== SETUP & LOOP ==================

void setup() {
  Serial.begin(115200);

  // Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print(F("Connecting to Wi-Fi"));

  uint8_t attempts = 0;
  while (!wifiConnected() && attempts < 20) {
    delay(500);
    Serial.print('.');
    attempts++;
  }

  if (wifiConnected()) {
    Serial.println(F("\nWi-Fi connected"));
    Serial.print(F("IP: "));
    Serial.println(WiFi.localIP());

    mqttClient.setServer(cfg.server, cfg.port);
    mqttClient.setBufferSize(512);
    connectMQTT();
  } else {
    Serial.println(F("\nWi-Fi FAILED – offline mode, logging to SD only"));
  }
}

void loop() {
  // ... sampling, buffering, SD logging, etc.

  maintainMQTTConnection();
  transmitAggregatedData();

  if (current_power > RATED_POWER * 1.2f) {
    sendAlert("OVERPOWER",
              "Power exceeded 120% of rated: " + String(current_power) + " W");
  }
}
