/*
 * Example 05: SD Card Logging
 * 
 * Purpose: Write sensor data to SD card in CSV format
 * 
 * Learning Objectives:
 * - Initialize SD card with SPI protocol
 * - Create and write files
 * - Format data as CSV (Comma-Separated Values)
 * - Handle SD card errors gracefully
 * - Implement periodic file flushing
 * 
 * Hardware Required:
 * - ESP32 board
 * - MicroSD card module (SPI interface)
 * - MicroSD card (formatted FAT32, ≤ 32 GB)
 * 
 * Connections:
 * - SD Card CS  → GPIO 5
 * - SD Card SCK → GPIO 18
 * - SD Card MOSI → GPIO 23
 * - SD Card MISO → GPIO 19
 * - SD Card VCC → 3.3V (NOT 5V!)
 * - SD Card GND → GND
 * 
 * File Format:
 * - CSV with headers: timestamp, sensor1, sensor2, ...
 * - One measurement per line
 * - Compatible with Excel, Python pandas, MATLAB
 * 
 * Author: Wind Turbine DAQ Guide
 * License: MIT
 */

#include <SPI.h>
#include <SD.h>

// SD Card pin definitions (ESP32 default SPI pins)
const int SD_CS_PIN = 5;    // Chip Select
const int SD_SCK_PIN = 18;  // Clock
const int SD_MOSI_PIN = 23; // Master Out Slave In
const int SD_MISO_PIN = 19; // Master In Slave Out

// File configuration
const char* DATA_FILENAME = "/turbine_data.csv";
const int FLUSH_INTERVAL_MS = 10000;  // Flush to SD every 10 seconds

// Simulated sensor data (replace with real sensors in complete system)
float simulatedWindSpeed = 5.5;
float simulatedPower = 125.3;
float simulatedRPM = 245.0;

// Timing variables
unsigned long lastFlushTime = 0;
unsigned long recordCount = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=================================");
  Serial.println("SD Card Logging - Example 05");
  Serial.println("=================================");
  Serial.println();
  
  // Initialize SPI with custom pins
  SPI.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
  
  // Initialize SD card
  Serial.print("Initializing SD card... ");
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("FAILED!");
    Serial.println();
    Serial.println("Possible causes:");
    Serial.println("  1. SD card not inserted");
    Serial.println("  2. Wiring incorrect (check CS, SCK, MOSI, MISO)");
    Serial.println("  3. SD card not formatted (use FAT32)");
    Serial.println("  4. SD card > 32 GB (not supported)");
    Serial.println();
    Serial.println("System halted. Fix issue and restart.");
    while(1) {
      delay(1000);  // Halt
    }
  }
  Serial.println("SUCCESS");
  
  // Print SD card info
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.print("SD Card Size: ");
  Serial.print(cardSize);
  Serial.println(" MB");
  
  uint64_t usedSpace = SD.usedBytes() / (1024 * 1024);
  Serial.print("Used Space: ");
  Serial.print(usedSpace);
  Serial.println(" MB");
  
  uint64_t freeSpace = (SD.totalBytes() - SD.usedBytes()) / (1024 * 1024);
  Serial.print("Free Space: ");
  Serial.print(freeSpace);
  Serial.println(" MB");
  Serial.println();
  
  // Check if data file exists
  if (SD.exists(DATA_FILENAME)) {
    Serial.print("Data file exists: ");
    Serial.println(DATA_FILENAME);
    Serial.println("Appending to existing file...");
  } else {
    Serial.print("Creating new file: ");
    Serial.println(DATA_FILENAME);
    
    // Create file and write CSV header
    File dataFile = SD.open(DATA_FILENAME, FILE_WRITE);
    if (dataFile) {
      dataFile.println("timestamp_ms,wind_speed_ms,power_w,rpm");
      dataFile.close();
      Serial.println("CSV header written");
    } else {
      Serial.println("ERROR: Could not create file!");
      while(1) { delay(1000); }  // Halt
    }
  }
  
  Serial.println();
  Serial.println("Logging started. Data is written to SD card.");
  Serial.println("Remove SD card ONLY after seeing 'Flushed' message.");
  Serial.println();
  Serial.println("Time(s)\tWind(m/s)\tPower(W)\tRPM\tStatus");
  Serial.println("------------------------------------------------------------");
  
  lastFlushTime = millis();
}

void loop() {
  unsigned long currentTime_ms = millis();
  
  // Simulate sensor readings (replace with actual sensor code)
  // Add small random variations to make data realistic
  simulatedWindSpeed = 5.5 + (random(-100, 100) / 100.0);
  simulatedPower = 125.0 + (random(-500, 500) / 10.0);
  simulatedRPM = 245.0 + (random(-50, 50) / 10.0);
  
  // Open file in append mode
  File dataFile = SD.open(DATA_FILENAME, FILE_APPEND);
  
  if (dataFile) {
    // Write data in CSV format: timestamp,wind_speed,power,rpm
    dataFile.print(currentTime_ms);
    dataFile.print(",");
    dataFile.print(simulatedWindSpeed, 2);
    dataFile.print(",");
    dataFile.print(simulatedPower, 1);
    dataFile.print(",");
    dataFile.println(simulatedRPM, 1);
    
    // File is NOT immediately written to SD card (buffered)
    // We'll flush periodically for data safety
    
    recordCount++;
    
    // Check if it's time to flush buffer to SD card
    bool flushed = false;
    if (currentTime_ms - lastFlushTime >= FLUSH_INTERVAL_MS) {
      dataFile.flush();  // Force write to SD card
      lastFlushTime = currentTime_ms;
      flushed = true;
    }
    
    dataFile.close();
    
    // Print to Serial Monitor
    Serial.print(currentTime_ms / 1000.0, 1);
    Serial.print("\t");
    Serial.print(simulatedWindSpeed, 2);
    Serial.print("\t\t");
    Serial.print(simulatedPower, 1);
    Serial.print("\t\t");
    Serial.print(simulatedRPM, 1);
    Serial.print("\t");
    if (flushed) {
      Serial.print("Flushed (");
      Serial.print(recordCount);
      Serial.println(" records)");
      recordCount = 0;
    } else {
      Serial.println("Buffered");
    }
    
  } else {
    Serial.println("ERROR: Failed to open file for writing!");
    Serial.println("SD card may be removed or corrupted.");
  }
  
  delay(1000);  // Log every 1 second
}