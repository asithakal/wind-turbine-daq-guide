/*
 * Example 04: RPM Measurement with Interrupts
 * 
 * Purpose: Measure rotor rotational speed using Hall effect sensor
 * 
 * Learning Objectives:
 * - Use hardware interrupts (attachInterrupt)
 * - Count pulses in a time window
 * - Convert pulse count to RPM
 * - Understand volatile variables
 * - Implement debouncing
 * 
 * Hardware Required:
 * - ESP32 board
 * - Hall effect sensor (e.g., Honeywell SS441A)
 * - Magnet mounted on rotor shaft
 * - 10kΩ pull-up resistor (if sensor has open-collector output)
 * 
 * Connections:
 * - Hall sensor VCC → 3.3V or 5V (check datasheet)
 * - Hall sensor GND → GND
 * - Hall sensor OUT → GPIO 15
 * - (Optional) 10kΩ resistor from GPIO 15 to 3.3V
 * 
 * Operation:
 * - Magnet passes sensor once per revolution
 * - Sensor pulls GPIO LOW when magnet near
 * - Interrupt counts each pulse
 * - RPM = (pulses × 60) / time_seconds
 * 
 * Author: Wind Turbine DAQ Guide
 * License: MIT
 */

// Pin definitions
const int HALL_SENSOR_PIN = 15;  // GPIO 15 - supports interrupts

// RPM calculation parameters
const int PULSES_PER_REV = 1;    // 1 magnet = 1 pulse per revolution
const int CALC_PERIOD_MS = 1000; // Calculate RPM every 1 second

// Debounce settings
const unsigned long DEBOUNCE_DELAY_US = 1000; // 1ms debounce (1000 microseconds)

// Volatile variables (modified in interrupt)
volatile unsigned long pulseCount = 0;
volatile unsigned long lastPulseTime_us = 0;

// Non-volatile variables
unsigned long lastCalcTime_ms = 0;
float currentRPM = 0.0;
unsigned long totalRevolutions = 0;

// Function prototypes
void IRAM_ATTR hallSensorISR();

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=================================");
  Serial.println("RPM Measurement - Example 04");
  Serial.println("=================================");
  Serial.println();
  
  // Configure Hall sensor pin as input with pull-up
  pinMode(HALL_SENSOR_PIN, INPUT_PULLUP);
  
  // Attach interrupt: trigger on FALLING edge (sensor pulls LOW)
  attachInterrupt(digitalPinToInterrupt(HALL_SENSOR_PIN), hallSensorISR, FALLING);
  
  Serial.println("Configuration:");
  Serial.print("  Pin: GPIO ");
  Serial.println(HALL_SENSOR_PIN);
  Serial.print("  Pulses per revolution: ");
  Serial.println(PULSES_PER_REV);
  Serial.print("  Calculation period: ");
  Serial.print(CALC_PERIOD_MS);
  Serial.println(" ms");
  Serial.println();
  Serial.println("Spin the rotor to measure RPM...");
  Serial.println();
  Serial.println("Time(s)\tRPM\tRevolutions\tPulse Period(ms)");
  Serial.println("-------------------------------------------------------");
  
  lastCalcTime_ms = millis();
}

void loop() {
  unsigned long currentTime_ms = millis();
  
  // Calculate RPM every CALC_PERIOD_MS
  if (currentTime_ms - lastCalcTime_ms >= CALC_PERIOD_MS) {
    // Disable interrupts during read to prevent data corruption
    noInterrupts();
    unsigned long pulses = pulseCount;
    pulseCount = 0;  // Reset counter
    interrupts();
    
    // Calculate RPM: (pulses / pulses_per_rev) * (60 sec / measurement_period)
    float rpm_period_sec = (currentTime_ms - lastCalcTime_ms) / 1000.0;
    currentRPM = (pulses / (float)PULSES_PER_REV) * (60.0 / rpm_period_sec);
    
    // Update total revolution counter
    totalRevolutions += (pulses / PULSES_PER_REV);
    
    // Calculate average time between pulses (for diagnostic)
    float avgPulsePeriod_ms = 0;
    if (pulses > 0) {
      avgPulsePeriod_ms = CALC_PERIOD_MS / (float)pulses;
    }
    
    // Print results
    Serial.print(currentTime_ms / 1000.0, 1);
    Serial.print("\t");
    Serial.print(currentRPM, 1);
    Serial.print("\t");
    Serial.print(totalRevolutions);
    Serial.print("\t\t");
    Serial.println(avgPulsePeriod_ms, 2);
    
    lastCalcTime_ms = currentTime_ms;
  }
  
  // Main loop can do other tasks here
  delay(10);
}

// Interrupt Service Routine (ISR) - runs when magnet passes sensor
// Must be kept SHORT and FAST!
void IRAM_ATTR hallSensorISR() {
  // Get current time in microseconds
  unsigned long currentTime_us = micros();
  
  // Debouncing: ignore pulses that come too quickly
  // (prevents counting bounces as multiple pulses)
  if (currentTime_us - lastPulseTime_us > DEBOUNCE_DELAY_US) {
    pulseCount++;
    lastPulseTime_us = currentTime_us;
  }
}