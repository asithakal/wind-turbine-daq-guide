/*
 * Example 03: I2C Sensors - Power Monitor & Environmental Sensor
 * 
 * Purpose: Read multiple I2C sensors (INA226 and BME280)
 * 
 * Learning Objectives:
 * - Use I2C communication protocol (Wire library)
 * - Work with multiple I2C devices on same bus
 * - Use Adafruit sensor libraries
 * - Handle sensor initialization errors
 * 
 * Hardware Required:
 * - ESP32 board
 * - INA226 power monitor module (I2C address 0x40)
 * - BME280 environmental sensor (I2C address 0x76 or 0x77)
 * - 2× 4.7kΩ pull-up resistors (usually on modules already)
 * 
 * Connections:
 * - SDA (all devices) → GPIO 21
 * - SCL (all devices) → GPIO 22
 * - VCC (all devices) → 3.3V
 * - GND (all devices) → GND
 * 
 * I2C Addresses:
 * - INA226: 0x40 (default)
 * - BME280: 0x76 or 0x77 (check with I2C scanner)
 * 
 * Author: Wind Turbine DAQ Guide
 * License: MIT
 */

#include <Wire.h>
#include <Adafruit_INA219.h>
#include <Adafruit_BME280.h>

// Pin definitions (ESP32 default I2C pins)
const int I2C_SDA = 21;
const int I2C_SCL = 22;

// I2C addresses
#define INA226_ADDRESS 0x40
#define BME280_ADDRESS 0x76  // Try 0x77 if this doesn't work

// Sensor objects
Adafruit_INA219 ina219;
Adafruit_BME280 bme280;

// Measurement variables
float busVoltage_V = 0;
float current_mA = 0;
float power_mW = 0;
float temperature_C = 0;
float humidity_pct = 0;
float pressure_hPa = 0;

// Status flags
bool ina219_available = false;
bool bme280_available = false;

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=================================");
  Serial.println("I2C Sensors Test - Example 03");
  Serial.println("=================================");
  Serial.println();
  
  // Initialize I2C with custom pins
  Wire.begin(I2C_SDA, I2C_SCL);
  
  // Scan for I2C devices
  Serial.println("Scanning I2C bus...");
  scanI2C();
  Serial.println();
  
  // Initialize INA219 (power monitor)
  Serial.print("Initializing INA219 power monitor... ");
  if (ina219.begin()) {
    Serial.println("SUCCESS");
    ina219_available = true;
    
    // Configure for 32V, 2A range (adjust for your turbine)
    ina219.setCalibration_32V_2A();
  } else {
    Serial.println("FAILED");
    Serial.println("  → Check wiring and I2C address (default 0x40)");
  }
  
  // Initialize BME280 (environmental sensor)
  Serial.print("Initializing BME280 environmental sensor... ");
  if (bme280.begin(BME280_ADDRESS)) {
    Serial.println("SUCCESS");
    bme280_available = true;
    
    // Configure sensor settings
    bme280.setSampling(Adafruit_BME280::MODE_NORMAL,     // Normal continuous mode
                      Adafruit_BME280::SAMPLING_X2,      // Temp oversampling ×2
                      Adafruit_BME280::SAMPLING_X16,     // Pressure oversampling ×16
                      Adafruit_BME280::SAMPLING_X1,      // Humidity oversampling ×1
                      Adafruit_BME280::FILTER_X16,       // IIR filter coefficient 16
                      Adafruit_BME280::STANDBY_MS_500);  // Standby 500ms
  } else {
    Serial.println("FAILED");
    Serial.println("  → Check wiring and I2C address (try 0x76 or 0x77)");
  }
  
  Serial.println();
  
  // Check if at least one sensor works
  if (!ina219_available && !bme280_available) {
    Serial.println("ERROR: No sensors detected!");
    Serial.println("Please check connections and restart.");
    while(1) {
      delay(1000);  // Halt execution
    }
  }
  
  Serial.println("Starting measurements...");
  Serial.println();
  Serial.println("Time(s)\tVoltage(V)\tCurrent(mA)\tPower(W)\tTemp(°C)\tHumidity(%)\tPressure(hPa)");
  Serial.println("-------------------------------------------------------------------------------------------");
}

void loop() {
  // Read INA219 (power measurements)
  if (ina219_available) {
    busVoltage_V = ina219.getBusVoltage_V();
    current_mA = ina219.getCurrent_mA();
    power_mW = ina219.getPower_mW();
  } else {
    busVoltage_V = -999;
    current_mA = -999;
    power_mW = -999;
  }
  
  // Read BME280 (environmental measurements)
  if (bme280_available) {
    temperature_C = bme280.readTemperature();
    humidity_pct = bme280.readHumidity();
    pressure_hPa = bme280.readPressure() / 100.0;  // Convert Pa to hPa
  } else {
    temperature_C = -999;
    humidity_pct = -999;
    pressure_hPa = -999;
  }
  
  // Print results in tab-separated format
  Serial.print(millis() / 1000.0, 1);
  Serial.print("\t");
  Serial.print(busVoltage_V, 2);
  Serial.print("\t\t");
  Serial.print(current_mA, 1);
  Serial.print("\t\t");
  Serial.print(power_mW / 1000.0, 3);  // Convert mW to W
  Serial.print("\t\t");
  Serial.print(temperature_C, 1);
  Serial.print("\t\t");
  Serial.print(humidity_pct, 1);
  Serial.print("\t\t");
  Serial.println(pressure_hPa, 1);
  
  delay(1000);  // Read every 1 second
}

// Function to scan I2C bus and print connected devices
void scanI2C() {
  byte error, address;
  int deviceCount = 0;
  
  Serial.println("  Address\tDevice");
  Serial.println("  -------\t------");
  
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("  0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.print("\t");
      
      // Identify common sensors
      if (address == 0x40) Serial.println("INA219/INA226");
      else if (address == 0x76 || address == 0x77) Serial.println("BME280");
      else Serial.println("Unknown");
      
      deviceCount++;
    }
  }
  
  if (deviceCount == 0) {
    Serial.println("  No I2C devices found!");
    Serial.println("  → Check SDA/SCL connections");
    Serial.println("  → Check power to sensors");
  } else {
    Serial.print("  Found ");
    Serial.print(deviceCount);
    Serial.println(" device(s)");
  }
}