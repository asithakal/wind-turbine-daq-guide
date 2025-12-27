/*
 * Example 08: Complete Wind Turbine Data Acquisition System
 * 
 * Purpose: Full integration of all sensors and logging systems
 * 
 * Features:
 * - Wind speed measurement (analog anemometer)
 * - Rotor RPM measurement (Hall sensor with interrupts)
 * - Power monitoring (INA226 via I2C)
 * - Environmental sensors (BME280 via I2C)
 * - SD card logging with timestamps
 * - NTP time synchronization
 * - MQTT cloud transmission
 * - Error handling and watchdog protection
 * - Status LED indicators
 * 
 * Hardware Required:
 * - ESP32 board
 * - Anemometer (analog output)
 * - Hall effect sensor + magnet
 * - INA226 power monitor
 * - BME280 environmental sensor
 * - MicroSD card module + card
 * - WiFi network (optional for cloud features)
 * 
 * Author: Wind Turbine DAQ Guide
 * License: MIT
 * Version: 1.0.0
 */

// ============================================================================
// INCLUDES
// ============================================================================
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <WiFi.h>
#include <time.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_INA219.h>
#include <Adafruit_BME280.h>

// ============================================================================
// CONFIGURATION - CHANGE THESE FOR YOUR SETUP
// ============================================================================

// WiFi Configuration (optional - system works without WiFi)
const char* WIFI_SSID = "YourWiFiNetwork";
const char* WIFI_PASSWORD = "YourPassword";
const bool ENABLE_WIFI = true;  // Set false for offline operation

// MQTT Configuration (requires WiFi)
const char* MQTT_BROKER = "broker.hivemq.com";
const int MQTT_PORT = 1883;
const char* MQTT_CLIENT_ID = "ESP32_Turbine_001";
const bool ENABLE_MQTT = true;

// Turbine Physical Parameters (CHANGE FOR YOUR TURBINE!)
const float ROTOR_RADIUS_M = 0.60;          // 0.6m radius (1.2m diameter)
const float ROTOR_HEIGHT_M = 1.50;          // 1.5m height
const float SWEPT_AREA_M2 = 1.80;           // 2*R*H for VAWT, π*R² for HAWT
const int PULSES_PER_REV = 1;               // Number of magnets on rotor

// Sensor Calibration (CHANGE FOR YOUR SENSORS!)
const float ANEM_OFFSET_V = 0.4;            // Anemometer: voltage at 0 m/s
const float ANEM_SCALE_V_PER_MS = 0.2;      // V per m/s
const float VOLTAGE_DIVIDER_RATIO = 2.0;    // For 5V anemometer on 3.3V ESP32

// Timezone (CHANGE FOR YOUR LOCATION!)
const char* TIMEZONE = "IST-5:30";          // Sri Lanka/India UTC+5:30

// ============================================================================
// PIN DEFINITIONS
// ============================================================================
const int PIN_ANEMOMETER = 34;      // ADC1_CH6 - analog wind speed
const int PIN_HALL_SENSOR = 15;     // Digital interrupt - rotor RPM
const int PIN_SD_CS = 5;            // SD card chip select
const int PIN_STATUS_LED = 2;       // Onboard LED for status

const int PIN_I2C_SDA = 21;         // I2C data (INA226, BME280)
const int PIN_I2C_SCL = 22;         // I2C clock

const int PIN_SPI_MISO = 19;        // SD card
const int PIN_SPI_MOSI = 23;
const int PIN_SPI_SCK = 18;

// ============================================================================
// TIMING CONFIGURATION
// ============================================================================
const int SAMPLE_INTERVAL_MS = 1000;        // Main sampling rate: 1 Hz
const int SD_FLUSH_INTERVAL_MS = 10000;     // Flush SD every 10 seconds
const int MQTT_PUBLISH_INTERVAL_MS = 5000;  // Publish to cloud every 5 sec
const int TIME_SYNC_INTERVAL_MS = 3600000;  // Resync time every 1 hour
const int WIFI_RECONNECT_INTERVAL_MS = 30000; // Check WiFi every 30 sec

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================
Adafruit_INA219 ina219;
Adafruit_BME280 bme280;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

// ============================================================================
// GLOBAL VARIABLES
// ============================================================================

// Sensor data structure
struct SensorData {
  unsigned long timestamp_ms;
  String timestamp_iso;
  float wind_speed_ms;
  float rotor_rpm;
  float voltage_v;
  float current_ma;
  float power_w;
  float temperature_c;
  float humidity_pct;
  float pressure_hpa;
  float air_density_kgm3;
  float lambda;
  float cp;
};

SensorData currentData;

// RPM measurement (interrupt variables)
volatile unsigned long pulseCount = 0;
volatile unsigned long lastPulseTime_us = 0;
const unsigned long DEBOUNCE_DELAY_US = 1000;

// Timing variables
unsigned long lastSampleTime = 0;
unsigned long lastSDFlush = 0;
unsigned long lastMQTTPublish = 0;
unsigned long lastTimeSync = 0;
unsigned long lastWiFiCheck = 0;

// Status flags
bool sdCardAvailable = false;
bool ina219Available = false;
bool bme280Available = false;
bool wifiConnected = false;
bool mqttConnected = false;
bool timeInitialized = false;

// Error counters
int sdErrorCount = 0;
int mqttErrorCount = 0;

// File handling
const char* DATA_FILENAME = "/turbine_data.csv";
File dataFile;

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  Serial.begin(115200);
  delay(2000);
  
  printHeader();
  
  // Initialize status LED
  pinMode(PIN_STATUS_LED, OUTPUT);
  digitalWrite(PIN_STATUS_LED, LOW);
  
  // Initialize sensors and peripherals
  initializeI2C();
  initializeAnemometer();
  initializeHallSensor();
  initializeSDCard();
  
  if (ENABLE_WIFI) {
    initializeWiFi();
    if (wifiConnected) {
      initializeTime();
      if (ENABLE_MQTT) {
        initializeMQTT();
      }
    }
  }
  
  // Create/open data file
  if (sdCardAvailable) {
    setupDataFile();
  }
  
  printSystemStatus();
  
  Serial.println("\n=== Data Acquisition Started ===\n");
  Serial.println("Time\t\tWind\tRPM\tPower\tTemp\tHumid\tPress\tλ\tCp\tStatus");
  Serial.println("================================================================================");
  
  lastSampleTime = millis();
  lastSDFlush = millis();
  lastMQTTPublish = millis();
}

// ============================================================================
// MAIN LOOP
// ============================================================================
void loop() {
  unsigned long currentTime = millis();
  
  // Sample sensors at defined interval
  if (currentTime - lastSampleTime >= SAMPLE_INTERVAL_MS) {
    blinkLED();
    
    readAllSensors();
    calculateDerivedValues();
    
    // Log to SD card
    if (sdCardAvailable) {
      logToSDCard();
    }
    
    // Flush SD buffer periodically
    if (currentTime - lastSDFlush >= SD_FLUSH_INTERVAL_MS) {
      if (sdCardAvailable && dataFile) {
        dataFile.flush();
        lastSDFlush = currentTime;
      }
    }
    
    // Publish to MQTT
    if (ENABLE_MQTT && mqttConnected && 
        currentTime - lastMQTTPublish >= MQTT_PUBLISH_INTERVAL_MS) {
      publishToMQTT();
      lastMQTTPublish = currentTime;
    }
    
    // Print to serial
    printToSerial();
    
    lastSampleTime = currentTime;
  }
  
  // Maintain MQTT connection
  if (ENABLE_MQTT && wifiConnected) {
    if (!mqttClient.connected()) {
      reconnectMQTT();
    }
    mqttClient.loop();
  }
  
  // Periodic WiFi check
  if (ENABLE_WIFI && currentTime - lastWiFiCheck >= WIFI_RECONNECT_INTERVAL_MS) {
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi disconnected. Reconnecting...");
      initializeWiFi();
    }
    lastWiFiCheck = currentTime;
  }
  
  // Periodic time resync
  if (timeInitialized && currentTime - lastTimeSync >= TIME_SYNC_INTERVAL_MS) {
    syncTime();
    lastTimeSync = currentTime;
  }
  
  delay(10);  // Small delay to prevent watchdog timeout
}

// ============================================================================
// INITIALIZATION FUNCTIONS
// ============================================================================

void initializeI2C() {
  Serial.print("Initializing I2C bus... ");
  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  Serial.println("OK");
  
  // Initialize INA219
  Serial.print("  INA219 power monitor... ");
  if (ina219.begin()) {
    ina219.setCalibration_32V_2A();
    ina219Available = true;
    Serial.println("OK");
  } else {
    Serial.println("FAILED");
  }
  
  // Initialize BME280
  Serial.print("  BME280 environmental... ");
  if (bme280.begin(0x76) || bme280.begin(0x77)) {
    bme280.setSampling(Adafruit_BME280::MODE_NORMAL,
                      Adafruit_BME280::SAMPLING_X2,
                      Adafruit_BME280::SAMPLING_X16,
                      Adafruit_BME280::SAMPLING_X1,
                      Adafruit_BME280::FILTER_X16,
                      Adafruit_BME280::STANDBY_MS_500);
    bme280Available = true;
    Serial.println("OK");
  } else {
    Serial.println("FAILED");
  }
}

void initializeAnemometer() {
  Serial.print("Initializing anemometer (GPIO ");
  Serial.print(PIN_ANEMOMETER);
  Serial.print(")... ");
  pinMode(PIN_ANEMOMETER, INPUT);
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);
  Serial.println("OK");
}

void initializeHallSensor() {
  Serial.print("Initializing Hall sensor (GPIO ");
  Serial.print(PIN_HALL_SENSOR);
  Serial.print(")... ");
  pinMode(PIN_HALL_SENSOR, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PIN_HALL_SENSOR), hallSensorISR, FALLING);
  Serial.println("OK");
}

void initializeSDCard() {
  Serial.print("Initializing SD card... ");
  SPI.begin(PIN_SPI_SCK, PIN_SPI_MISO, PIN_SPI_MOSI, PIN_SD_CS);
  
  if (SD.begin(PIN_SD_CS)) {
    sdCardAvailable = true;
    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    uint64_t freeSpace = (SD.totalBytes() - SD.usedBytes()) / (1024 * 1024);
    Serial.print("OK (");
    Serial.print(cardSize);
    Serial.print(" MB total, ");
    Serial.print(freeSpace);
    Serial.println(" MB free)");
  } else {
    Serial.println("FAILED");
    sdErrorCount++;
  }
}

void initializeWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.print(WIFI_SSID);
  Serial.print("... ");
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.print("OK (");
    Serial.print(WiFi.localIP());
    Serial.println(")");
  } else {
    Serial.println("FAILED");
  }
}

void initializeTime() {
  if (!wifiConnected) return;
  
  Serial.print("Synchronizing time... ");
  configTime(0, 0, "pool.ntp.org", "time.google.com");
  setenv("TZ", TIMEZONE, 1);
  tzset();
  
  int attempts = 0;
  time_t now = 0;
  struct tm timeinfo = {0};
  
  while (timeinfo.tm_year < (2024 - 1900) && attempts < 20) {
    time(&now);
    localtime_r(&now, &timeinfo);
    delay(500);
    attempts++;
  }
  
  if (timeinfo.tm_year >= (2024 - 1900)) {
    timeInitialized = true;
    Serial.println("OK");
  } else {
    Serial.println("FAILED");
  }
}

void initializeMQTT() {
  mqttClient.setServer(MQTT_BROKER, MQTT_PORT);
  reconnectMQTT();
}

// ============================================================================
// DATA ACQUISITION FUNCTIONS
// ============================================================================

void readAllSensors() {
  currentData.timestamp_ms = millis();
  
  // Get ISO timestamp if time is available
  if (timeInitialized) {
    currentData.timestamp_iso = getISO8601Timestamp();
  } else {
    currentData.timestamp_iso = String(currentData.timestamp_ms);
  }
  
  // Read wind speed (analog)
  float adcSum = 0;
  for (int i = 0; i < 10; i++) {
    adcSum += analogRead(PIN_ANEMOMETER);
    delay(5);
  }
  float adcAvg = adcSum / 10.0;
  float voltage = (adcAvg / 4095.0) * 3.3 * VOLTAGE_DIVIDER_RATIO;
  currentData.wind_speed_ms = max(0.0f, (voltage - ANEM_OFFSET_V) / ANEM_SCALE_V_PER_MS);
  
  // Read RPM (from interrupt counter)
  noInterrupts();
  unsigned long pulses = pulseCount;
  pulseCount = 0;
  interrupts();
  float rpmPeriod = (millis() - lastSampleTime) / 1000.0;
  currentData.rotor_rpm = (pulses / (float)PULSES_PER_REV) * (60.0 / rpmPeriod);
  
  // Read power (INA219)
  if (ina219Available) {
    currentData.voltage_v = ina219.getBusVoltage_V();
    currentData.current_ma = ina219.getCurrent_mA();
    currentData.power_w = ina219.getPower_mW() / 1000.0;
  } else {
    currentData.voltage_v = -999;
    currentData.current_ma = -999;
    currentData.power_w = -999;
  }
  
  // Read environmental (BME280)
  if (bme280Available) {
    currentData.temperature_c = bme280.readTemperature();
    currentData.humidity_pct = bme280.readHumidity();
    currentData.pressure_hpa = bme280.readPressure() / 100.0;
  } else {
    currentData.temperature_c = -999;
    currentData.humidity_pct = -999;
    currentData.pressure_hpa = -999;
  }
}

void calculateDerivedValues() {
  // Calculate air density (kg/m³)
  // ρ = P / (R_specific * T)
  if (bme280Available) {
    float T_kelvin = currentData.temperature_c + 273.15;
    float P_pascal = currentData.pressure_hpa * 100.0;
    const float R_specific = 287.05;  // J/(kg·K) for dry air
    currentData.air_density_kgm3 = P_pascal / (R_specific * T_kelvin);
  } else {
    currentData.air_density_kgm3 = 1.225;  // Standard air density at sea level
  }
  
  // Calculate tip speed ratio (λ)
  // λ = (ω * R) / V
  // where ω = 2πN/60 (angular velocity in rad/s)
  if (currentData.wind_speed_ms > 0.1) {
    float omega = (currentData.rotor_rpm * 2.0 * PI) / 60.0;
    float tipSpeed = omega * ROTOR_RADIUS_M;
    currentData.lambda = tipSpeed / currentData.wind_speed_ms;
  } else {
    currentData.lambda = 0;
  }
  
  // Calculate power coefficient (Cp)
  // Cp = P_actual / P_available
  // P_available = 0.5 * ρ * A * V³
  if (currentData.wind_speed_ms > 0.1 && currentData.power_w > 0) {
    float windPower = 0.5 * currentData.air_density_kgm3 * SWEPT_AREA_M2 * 
                      pow(currentData.wind_speed_ms, 3);
    currentData.cp = currentData.power_w / windPower;
    
    // Sanity check (Betz limit = 0.593)
    if (currentData.cp > 0.6) {
      currentData.cp = -999;  // Invalid (exceeds Betz limit)
    }
  } else {
    currentData.cp = 0;
  }
}

// ============================================================================
// DATA LOGGING FUNCTIONS
// ============================================================================

void setupDataFile() {
  if (!SD.exists(DATA_FILENAME)) {
    Serial.print("Creating data file: ");
    Serial.println(DATA_FILENAME);
    
    dataFile = SD.open(DATA_FILENAME, FILE_WRITE);
    if (dataFile) {
      // Write CSV header
      dataFile.println("timestamp,timestamp_ms,wind_speed_ms,rotor_rpm,voltage_v,current_ma,power_w,temperature_c,humidity_pct,pressure_hpa,air_density_kgm3,lambda,cp");
      dataFile.close();
    }
  }
}

void logToSDCard() {
  dataFile = SD.open(DATA_FILENAME, FILE_APPEND);
  
  if (dataFile) {
    dataFile.print(currentData.timestamp_iso);
    dataFile.print(",");
    dataFile.print(currentData.timestamp_ms);
    dataFile.print(",");
    dataFile.print(currentData.wind_speed_ms, 3);
    dataFile.print(",");
    dataFile.print(currentData.rotor_rpm, 2);
    dataFile.print(",");
    dataFile.print(currentData.voltage_v, 3);
    dataFile.print(",");
    dataFile.print(currentData.current_ma, 2);
    dataFile.print(",");
    dataFile.print(currentData.power_w, 3);
    dataFile.print(",");
    dataFile.print(currentData.temperature_c, 2);
    dataFile.print(",");
    dataFile.print(currentData.humidity_pct, 1);
    dataFile.print(",");
    dataFile.print(currentData.pressure_hpa, 2);
    dataFile.print(",");
    dataFile.print(currentData.air_density_kgm3, 4);
    dataFile.print(",");
    dataFile.print(currentData.lambda, 3);
    dataFile.print(",");
    dataFile.println(currentData.cp, 4);
    
    dataFile.close();
    sdErrorCount = 0;  // Reset error counter on success
  } else {
    sdErrorCount++;
    if (sdErrorCount > 10) {
      sdCardAvailable = false;
      Serial.println("ERROR: SD card write failed repeatedly!");
    }
  }
}

void publishToMQTT() {
  if (!mqttClient.connected()) return;
  
  StaticJsonDocument<384> doc;
  
  doc["timestamp"] = currentData.timestamp_iso;
  doc["wind_speed_ms"] = round(currentData.wind_speed_ms * 100) / 100.0;
  doc["rotor_rpm"] = round(currentData.rotor_rpm * 10) / 10.0;
  doc["power_w"] = round(currentData.power_w * 100) / 100.0;
  doc["temperature_c"] = round(currentData.temperature_c * 10) / 10.0;
  doc["humidity_pct"] = round(currentData.humidity_pct);
  doc["lambda"] = round(currentData.lambda * 100) / 100.0;
  doc["cp"] = round(currentData.cp * 1000) / 1000.0;
  
  char jsonBuffer[384];
  serializeJson(doc, jsonBuffer);
  
  String topic = String("turbine/") + MQTT_CLIENT_ID + "/data";
  if (mqttClient.publish(topic.c_str(), jsonBuffer)) {
    mqttErrorCount = 0;
  } else {
    mqttErrorCount++;
  }
}

void printToSerial() {
  Serial.print(currentData.timestamp_iso.substring(11, 19));  // HH:MM:SS only
  Serial.print("\t");
  Serial.print(currentData.wind_speed_ms, 2);
  Serial.print("\t");
  Serial.print(currentData.rotor_rpm, 1);
  Serial.print("\t");
  Serial.print(currentData.power_w, 2);
  Serial.print("\t");
  Serial.print(currentData.temperature_c, 1);
  Serial.print("\t");
  Serial.print(currentData.humidity_pct, 0);
  Serial.print("\t");
  Serial.print(currentData.pressure_hpa, 1);
  Serial.print("\t");
  Serial.print(currentData.lambda, 2);
  Serial.print("\t");
  Serial.print(currentData.cp, 3);
  Serial.print("\t");
  
  // Status indicators
  if (sdCardAvailable) Serial.print("SD ");
  if (mqttConnected) Serial.print("MQTT ");
  Serial.println();
}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

void IRAM_ATTR hallSensorISR() {
  unsigned long currentTime_us = micros();
  if (currentTime_us - lastPulseTime_us > DEBOUNCE_DELAY_US) {
    pulseCount++;
    lastPulseTime_us = currentTime_us;
  }
}

void reconnectMQTT() {
  if (!wifiConnected) return;
  
  int attempts = 0;
  while (!mqttClient.connected() && attempts < 3) {
    if (mqttClient.connect(MQTT_CLIENT_ID)) {
      mqttConnected = true;
      String topic = String("turbine/") + MQTT_CLIENT_ID + "/status";
      mqttClient.publish(topic.c_str(), "online", true);
      return;
    }
    attempts++;
    delay(1000);
  }
  mqttConnected = false;
}

void syncTime() {
  if (wifiConnected) {
    configTime(0, 0, "pool.ntp.org");
  }
}

String getISO8601Timestamp() {
  time_t now;
  struct tm timeinfo;
  char buffer[30];
  
  time(&now);
  localtime_r(&now, &timeinfo);
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S%z", &timeinfo);
  
  return String(buffer);
}

void blinkLED() {
  digitalWrite(PIN_STATUS_LED, HIGH);
  delay(50);
  digitalWrite(PIN_STATUS_LED, LOW);
}

void printHeader() {
  Serial.println("\n");
  Serial.println("================================================================================");
  Serial.println("        Wind Turbine Data Acquisition System - Complete Integration");
  Serial.println("================================================================================");
  Serial.println("Version: 1.0.0");
  Serial.println("Author: Wind Turbine DAQ Guide");
  Serial.println("Build Date: " + String(__DATE__) + " " + String(__TIME__));
  Serial.println("================================================================================\n");
}

void printSystemStatus() {
  Serial.println("\n=== System Status ===");
  Serial.print("INA219 Power Monitor:     "); Serial.println(ina219Available ? "OK" : "FAILED");
  Serial.print("BME280 Environmental:     "); Serial.println(bme280Available ? "OK" : "FAILED");
  Serial.print("SD Card:                  "); Serial.println(sdCardAvailable ? "OK" : "FAILED");
  Serial.print("WiFi:                     "); Serial.println(wifiConnected ? "Connected" : "Disabled/Failed");
  Serial.print("Time Sync:                "); Serial.println(timeInitialized ? "OK" : "Failed/Offline");
  Serial.print("MQTT:                     "); Serial.println(mqttConnected ? "Connected" : "Disabled/Failed");
  Serial.println("\n=== Turbine Configuration ===");
  Serial.print("Rotor Radius:             "); Serial.print(ROTOR_RADIUS_M); Serial.println(" m");
  Serial.print("Rotor Height:             "); Serial.print(ROTOR_HEIGHT_M); Serial.println(" m");
  Serial.print("Swept Area:               "); Serial.print(SWEPT_AREA_M2); Serial.println(" m²");
  Serial.print("Pulses per Revolution:    "); Serial.println(PULSES_PER_REV);
  Serial.println("====================");
}