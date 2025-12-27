/*
 * Example 06: NTP Time Synchronization
 * 
 * Purpose: Get accurate timestamps from internet time servers
 * 
 * Learning Objectives:
 * - Connect ESP32 to WiFi
 * - Use NTP (Network Time Protocol) to get current time
 * - Configure timezone and DST (Daylight Saving Time)
 * - Format timestamps in ISO 8601 format
 * - Handle time sync failures gracefully
 * 
 * Hardware Required:
 * - ESP32 board
 * - WiFi network with internet access
 * 
 * Connections:
 * - None (uses built-in WiFi)
 * 
 * NTP Servers Used:
 * - pool.ntp.org (global pool)
 * - time.google.com (Google's NTP)
 * - time.cloudflare.com (Cloudflare's NTP)
 * 
 * Timezone:
 * - Set for Sri Lanka (UTC+5:30)
 * - Change for your location
 * 
 * Author: Wind Turbine DAQ Guide
 * License: MIT
 */

#include <WiFi.h>
#include <time.h>

// WiFi credentials - CHANGE THESE!
const char* WIFI_SSID = "YourWiFiNetwork";
const char* WIFI_PASSWORD = "YourPassword";

// NTP configuration
const char* NTP_SERVER1 = "pool.ntp.org";
const char* NTP_SERVER2 = "time.google.com";
const char* NTP_SERVER3 = "time.cloudflare.com";

// Timezone configuration
// Format: "STD offset DST offset, rule"
// Sri Lanka: UTC+5:30, no DST
const char* TIMEZONE = "IST-5:30";

// For other locations, see: https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
// Examples:
//   USA Eastern:    "EST5EDT,M3.2.0,M11.1.0"
//   UK:             "GMT0BST,M3.5.0/1,M10.5.0"
//   Japan:          "JST-9"
//   Australia East: "AEST-10AEDT,M10.1.0,M4.1.0/3"

// Timing
const int SYNC_INTERVAL_MS = 3600000;  // Resync every 1 hour
unsigned long lastSyncTime = 0;

// Status flags
bool wifiConnected = false;
bool timeInitialized = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=================================");
  Serial.println("NTP Time Sync - Example 06");
  Serial.println("=================================");
  Serial.println();
  
  // Connect to WiFi
  connectWiFi();
  
  // Initialize time from NTP servers
  if (wifiConnected) {
    initializeTime();
  }
  
  Serial.println();
  Serial.println("Current Time (updates every second):");
  Serial.println("---------------------------------------");
}

void loop() {
  if (!wifiConnected) {
    Serial.println("ERROR: WiFi not connected. Reconnecting...");
    connectWiFi();
    delay(5000);
    return;
  }
  
  // Check if time is initialized
  if (!timeInitialized) {
    Serial.println("ERROR: Time not initialized. Retrying...");
    initializeTime();
    delay(5000);
    return;
  }
  
  // Resync time periodically
  if (millis() - lastSyncTime > SYNC_INTERVAL_MS) {
    Serial.println("\n--- Periodic time resync ---");
    initializeTime();
  }
  
  // Get current time
  time_t now;
  struct tm timeinfo;
  
  time(&now);
  localtime_r(&now, &timeinfo);
  
  // Format 1: Human-readable
  char timeString[64];
  strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S %Z", &timeinfo);
  Serial.print("Human:     ");
  Serial.println(timeString);
  
  // Format 2: ISO 8601 (for CSV logging)
  strftime(timeString, sizeof(timeString), "%Y-%m-%dT%H:%M:%S%z", &timeinfo);
  Serial.print("ISO 8601:  ");
  Serial.println(timeString);
  
  // Format 3: Unix timestamp (seconds since 1970-01-01)
  Serial.print("Unix:      ");
  Serial.println(now);
  
  // Format 4: Milliseconds since boot (for local timing)
  Serial.print("Millis:    ");
  Serial.println(millis());
  
  Serial.println("---------------------------------------");
  
  delay(1000);  // Update every second
}

void connectWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);
  
  WiFi.mode(WIFI_STA);  // Station mode (connect to existing network)
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();
  
  if (WiFi.status() == WL_CONNECTED) {
    wifiConnected = true;
    Serial.println("WiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal strength: ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    wifiConnected = false;
    Serial.println("WiFi connection FAILED!");
    Serial.println("Check SSID and password in code.");
  }
}

void initializeTime() {
  Serial.print("Synchronizing time with NTP servers... ");
  
  // Configure NTP servers and timezone
  configTime(0, 0, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);
  setenv("TZ", TIMEZONE, 1);
  tzset();
  
  // Wait for time to be set (timeout after 10 seconds)
  int attempts = 0;
  time_t now = 0;
  struct tm timeinfo = {0};
  
  while (timeinfo.tm_year < (2024 - 1900) && attempts < 20) {
    time(&now);
    localtime_r(&now, &timeinfo);
    delay(500);
    Serial.print(".");
    attempts++;
  }
  Serial.println();
  
  if (timeinfo.tm_year >= (2024 - 1900)) {
    timeInitialized = true;
    lastSyncTime = millis();
    
    char timeString[64];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S %Z", &timeinfo);
    Serial.print("Time synchronized: ");
    Serial.println(timeString);
  } else {
    timeInitialized = false;
    Serial.println("Time sync FAILED!");
    Serial.println("Possible causes:");
    Serial.println("  1. No internet connection");
    Serial.println("  2. NTP servers blocked by firewall");
    Serial.println("  3. Router DNS issues");
  }
}

// Helper function to get ISO 8601 timestamp string
String getISO8601Timestamp() {
  time_t now;
  struct tm timeinfo;
  char buffer[30];
  
  time(&now);
  localtime_r(&now, &timeinfo);
  strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%S%z", &timeinfo);
  
  return String(buffer);
}

// Helper function to check if time is valid
bool isTimeValid() {
  time_t now;
  struct tm timeinfo;
  
  time(&now);
  localtime_r(&now, &timeinfo);
  
  // Time is valid if year >= 2024
  return (timeinfo.tm_year >= (2024 - 1900));
}