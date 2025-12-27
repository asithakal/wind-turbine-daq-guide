/*
 * Example 02: Single Sensor - Wind Speed Anemometer
 * 
 * Purpose: Read analog voltage from wind speed sensor
 * 
 * Learning Objectives:
 * - Read analog pins with analogRead()
 * - Convert ADC values to voltage
 * - Apply sensor calibration (voltage to wind speed)
 * - Understand ADC resolution on ESP32 (12-bit = 0-4095)
 * 
 * Hardware Required:
 * - ESP32 board
 * - Wind speed sensor (Inspeed Vortex or equivalent with 0-5V output)
 * - 10kΩ + 10kΩ voltage divider (to scale 5V to 3.3V for ESP32)
 * 
 * Connections:
 * - Anemometer signal → Voltage divider → GPIO 34 (ADC1_CH6)
 * - Anemometer VCC → 5V
 * - Anemometer GND → GND
 * 
 * Sensor Calibration (Inspeed Vortex):
 * - Output: 0.4V @ 0 m/s, increases 0.2V per m/s
 * - Formula: wind_speed = (voltage - 0.4) / 0.2
 * 
 * Author: Wind Turbine DAQ Guide
 * License: MIT
 */

// Pin definitions
const int ANEMOMETER_PIN = 34;  // GPIO 34 (ADC1_CH6) - analog input only

// Sensor calibration constants (CHANGE THESE for your sensor)
const float ANEM_OFFSET_VOLTS = 0.4;   // Voltage at 0 m/s
const float ANEM_SCALE_V_PER_MS = 0.2; // Voltage increase per m/s

// ADC constants
const int ADC_RESOLUTION = 4095;       // 12-bit ADC: 0-4095
const float ADC_REF_VOLTAGE = 3.3;     // ESP32 ADC reference voltage
const float VOLTAGE_DIVIDER_RATIO = 2.0; // 10k/10k divider scales 5V to 2.5V

// Sampling settings
const int SAMPLE_COUNT = 10;           // Number of samples to average
const int SAMPLE_DELAY_MS = 50;        // Delay between samples

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=================================");
  Serial.println("Wind Speed Sensor - Example 02");
  Serial.println("=================================");
  Serial.println();
  
  // Configure ADC resolution (ESP32 supports 9-12 bits)
  analogReadResolution(12);  // 12-bit resolution
  
  // Set ADC attenuation for 0-3.3V range
  // ADC1_CHANNEL_6 corresponds to GPIO 34
  analogSetAttenuation(ADC_11db);  // Full range: 0-3.3V
  
  Serial.println("Sensor Configuration:");
  Serial.print("  Pin: GPIO ");
  Serial.println(ANEMOMETER_PIN);
  Serial.print("  Offset: ");
  Serial.print(ANEM_OFFSET_VOLTS);
  Serial.println(" V");
  Serial.print("  Scale: ");
  Serial.print(ANEM_SCALE_V_PER_MS);
  Serial.println(" V/(m/s)");
  Serial.println();
  Serial.println("Reading wind speed...");
  Serial.println("Time(s)\tADC\tVoltage(V)\tWind Speed(m/s)");
  Serial.println("-----------------------------------------------------");
}

void loop() {
  // Read analog value with averaging to reduce noise
  float adc_sum = 0;
  for (int i = 0; i < SAMPLE_COUNT; i++) {
    adc_sum += analogRead(ANEMOMETER_PIN);
    delay(SAMPLE_DELAY_MS);
  }
  float adc_avg = adc_sum / SAMPLE_COUNT;
  
  // Convert ADC value to voltage
  // voltage = (ADC_value / ADC_max) * V_ref * divider_ratio
  float voltage = (adc_avg / ADC_RESOLUTION) * ADC_REF_VOLTAGE * VOLTAGE_DIVIDER_RATIO;
  
  // Apply calibration to get wind speed
  float wind_speed = (voltage - ANEM_OFFSET_VOLTS) / ANEM_SCALE_V_PER_MS;
  
  // Constrain to valid range (wind speed can't be negative)
  if (wind_speed < 0) {
    wind_speed = 0.0;
  }
  
  // Print results in tab-separated format (easy to copy to Excel)
  Serial.print(millis() / 1000.0, 1);  // Time in seconds
  Serial.print("\t");
  Serial.print(adc_avg, 0);            // ADC value (no decimals)
  Serial.print("\t");
  Serial.print(voltage, 3);            // Voltage (3 decimals)
  Serial.print("\t\t");
  Serial.println(wind_speed, 2);       // Wind speed (2 decimals)
  
  // Wait before next reading (total sample time = SAMPLE_COUNT * SAMPLE_DELAY_MS + 1000)
  delay(1000);
}