# Firmware Design Patterns for Embedded DAQ Systems

Best practices for writing robust, maintainable firmware for ESP32-based data acquisition systems.

**Target audience:** Students with basic Arduino experience  
**Covers:** Code organization, state machines, error handling, memory management, debugging strategies

---

## Why Design Patterns Matter

### **Common Student Firmware Issues**

| Anti-Pattern | Consequence | Fix |
|-------------|-------------|-----|
| "Everything in `loop()`" | Hard to debug, impossible to extend | State machine architecture |
| Global variables everywhere | Namespace pollution, race conditions | Encapsulation in classes/structs |
| No error handling | Silent failures, corrupt data | Defensive programming |
| Magic numbers | Unmaintainable code | Named constants |
| Copy-paste coding | Bug multiplication | Functions/libraries |
| Blocking delays | System freezes, missed data | Non-blocking timing |

**Result of poor design:** System works in lab, fails in field deployment

**Result of good design:** System runs unattended for 6 months

---

## Pattern 1: State Machine Architecture

### **Problem: Spaghetti Code**

**Typical beginner approach:**

```
void loop() {
  // Read sensors
  float wind = analogRead(A0) * 0.2 + 0.4;
  float rpm = pulseCount * 60 / 1000;
  float power = ina219.getPower();
  
  // Check if time to log
  if (millis() - lastLog > 1000) {
    // Log to SD
    File f = SD.open("data.csv", FILE_WRITE);
    f.println(String(wind) + "," + String(rpm));
    f.close();
    
    // Send MQTT
    if (WiFi.status() == WL_CONNECTED) {
      mqtt.publish("data", "some json");
    }
    
    lastLog = millis();
  }
  
  // More code...
}
```

**Issues:**
- What if SD card is full?
- What if WiFi disconnects during MQTT publish?
- What if sensor read fails?
- Hard to add new features

---

### **Solution: Explicit State Machine**

**Define system states:**

```
enum SystemState {
  STATE_INIT,          // Initialize hardware
  STATE_IDLE,          // Waiting for timer
  STATE_ACQUIRE,       // Reading sensors
  STATE_PROCESS,       // Calculate derived quantities
  STATE_LOG,           // Write to SD card
  STATE_TRANSMIT,      // Send MQTT (optional)
  STATE_ERROR          // Handle faults
};

SystemState currentState = STATE_INIT;
```

**Implement state transitions:**

```
void loop() {
  switch (currentState) {
    case STATE_INIT:
      handleInit();
      break;
    
    case STATE_IDLE:
      handleIdle();
      break;
    
    case STATE_ACQUIRE:
      handleAcquire();
      break;
    
    case STATE_PROCESS:
      handleProcess();
      break;
    
    case STATE_LOG:
      handleLog();
      break;
    
    case STATE_TRANSMIT:
      handleTransmit();
      break;
    
    case STATE_ERROR:
      handleError();
      break;
  }
}
```

**State handler example:**

```
void handleAcquire() {
  // Read all sensors
  bool success = true;
  
  // Wind speed
  sensorData.windSpeed = readAnemometer();
  if (sensorData.windSpeed < 0) {
    errorFlags |= ERROR_ANEMOMETER;
    success = false;
  }
  
  // RPM
  noInterrupts();
  sensorData.rpm = calculateRPM();
  interrupts();
  
  // Power
  if (!ina219.getValues(&sensorData.voltage, &sensorData.current)) {
    errorFlags |= ERROR_POWER_SENSOR;
    success = false;
  }
  
  // Environment
  if (!bme280.read(&sensorData.temp, &sensorData.pressure, &sensorData.humidity)) {
    errorFlags |= ERROR_ENV_SENSOR;
    success = false;
  }
  
  // Transition
  if (success) {
    currentState = STATE_PROCESS;
  } else {
    currentState = STATE_ERROR;
  }
}
```

**Benefits:**
- ✅ Clear flow: INIT → IDLE → ACQUIRE → PROCESS → LOG → (TRANSMIT) → IDLE
- ✅ Easy to add new states
- ✅ Explicit error handling
- ✅ Testable (mock states)

---

### **State Transition Diagram**

```
    [INIT]
      ↓
    [IDLE] ←──────────────┐
      ↓                    │
   [ACQUIRE]               │
      ↓                    │
   [PROCESS]               │
      ↓                    │
    [LOG] ──→ Success ─────┤
      ↓                    │
   [TRANSMIT] (optional)   │
      ↓                    │
   Success ────────────────┘
      
   Any state → [ERROR] → Recovery → [IDLE]
```

---

## Pattern 2: Non-Blocking Timing

### **Problem: `delay()` Blocks Everything**

**Bad practice:**

```
void loop() {
  readSensors();
  logData();
  delay(1000);  // ❌ System frozen for 1 second
}
```

**Consequences:**
- Can't respond to button presses
- Hall sensor interrupts may overflow buffer
- MQTT keepalive fails
- Watchdog timer resets system

---

### **Solution 1: Millis-Based Timing**

```
const unsigned long LOG_INTERVAL = 1000;  // 1 second
unsigned long lastLogTime = 0;

void loop() {
  unsigned long currentTime = millis();
  
  // Check if it's time to log
  if (currentTime - lastLogTime >= LOG_INTERVAL) {
    readSensors();
    logData();
    lastLogTime = currentTime;
  }
  
  // Other tasks can run here
  checkButtons();
  handleMQTT();
}
```

**Handles rollover automatically:** `millis()` rolls over every 49.7 days, but subtraction still works

---

### **Solution 2: Hardware Timers (Precise Timing)**

**For 1 Hz sampling requirement:**

```
hw_timer_t *timer = NULL;
volatile bool sampleReady = false;

// Timer interrupt (executes every 1 second)
void IRAM_ATTR onTimer() {
  sampleReady = true;  // Set flag only, don't do heavy work in ISR
}

void setup() {
  // Configure timer: 80 MHz / 80 = 1 MHz tick rate
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  
  // Set alarm to 1,000,000 microseconds = 1 second
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);
}

void loop() {
  if (sampleReady) {
    sampleReady = false;  // Clear flag
    currentState = STATE_ACQUIRE;  // Trigger acquisition
  }
  
  // State machine continues...
}
```

**Benefits:**
- ✅ Precise 1.000 Hz (not 0.997 Hz due to `loop()` overhead)
- ✅ Independent of loop execution time
- ✅ Critical for research-grade data

---

## Pattern 3: Defensive Sensor Reading

### **Problem: Unchecked Sensor Failures**

**Naive approach:**

```
float temp = bme280.readTemperature();
// What if sensor is disconnected? temp = NaN or garbage
```

---

### **Solution: Validation Wrapper**

```
// Return struct with validity flag
struct SensorReading {
  float value;
  bool valid;
  uint32_t timestamp;
};

SensorReading readTemperature() {
  SensorReading result;
  result.timestamp = millis();
  
  // Attempt read
  float rawValue = bme280.readTemperature();
  
  // Validate
  if (isnan(rawValue)) {
    result.valid = false;
    result.value = -999.0;  // Error sentinel
    errorFlags |= ERROR_TEMP_NAN;
  } else if (rawValue < -40 || rawValue > 85) {
    result.valid = false;
    result.value = rawValue;
    errorFlags |= ERROR_TEMP_RANGE;
  } else {
    result.valid = true;
    result.value = rawValue;
  }
  
  return result;
}
```

**Usage:**

```
SensorReading temp = readTemperature();
if (temp.valid) {
  // Use temp.value
} else {
  // Log error, use previous value, or skip this sample
}
```

---

### **Pattern: Timeout-Protected I2C**

**Problem:** I2C bus can hang if sensor fails mid-transaction

**Solution:**

```
bool readI2CSensorWithTimeout(uint8_t address, uint8_t reg, uint8_t* data, uint32_t timeout_ms) {
  unsigned long startTime = millis();
  
  Wire.beginTransmission(address);
  Wire.write(reg);
  if (Wire.endTransmission() != 0) {
    return false;  // NACK or bus error
  }
  
  Wire.requestFrom(address, (uint8_t)1);
  
  // Wait for data with timeout
  while (Wire.available() == 0) {
    if (millis() - startTime > timeout_ms) {
      Wire.end();    // Reset I2C bus
      Wire.begin();  // Reinitialize
      return false;
    }
  }
  
  *data = Wire.read();
  return true;
}
```

---

## Pattern 4: Error Handling and Recovery

### **Problem: Silent Failures**

**What not to do:**

```
if (!SD.begin(CS_PIN)) {
  // Do nothing, continue anyway
}
// Data lost forever!
```

---

### **Solution: Error Flags and Graceful Degradation**

**Define error codes:**

```
// 8-bit error flags (expandable to 32-bit)
#define ERROR_NONE          0x00
#define ERROR_SD_INIT       0x01
#define ERROR_SD_WRITE      0x02
#define ERROR_ANEMOMETER    0x04
#define ERROR_POWER_SENSOR  0x08
#define ERROR_ENV_SENSOR    0x10
#define ERROR_WIFI          0x20
#define ERROR_MQTT          0x40
#define ERROR_LOW_MEMORY    0x80

volatile uint8_t errorFlags = ERROR_NONE;
```

**Set errors:**

```
if (!SD.begin(CS_PIN)) {
  errorFlags |= ERROR_SD_INIT;
  Serial.println("ERROR: SD card init failed");
}
```

**Check and handle:**

```
void handleError() {
  Serial.print("Error flags: 0x");
  Serial.println(errorFlags, HEX);
  
  // SD card errors
  if (errorFlags & ERROR_SD_INIT) {
    // Attempt re-init
    if (SD.begin(CS_PIN)) {
      errorFlags &= ~ERROR_SD_INIT;  // Clear flag
      Serial.println("SD card recovered");
    } else {
      // Fall back to RAM buffer
      useRAMBuffer = true;
    }
  }
  
  // Sensor errors
  if (errorFlags & ERROR_POWER_SENSOR) {
    // Try re-initializing I2C sensor
    if (ina219.begin()) {
      errorFlags &= ~ERROR_POWER_SENSOR;
    }
  }
  
  // Critical errors: reboot
  if (errorFlags & ERROR_LOW_MEMORY) {
    Serial.println("CRITICAL: Low memory, rebooting...");
    ESP.restart();
  }
  
  // Return to normal operation if errors cleared
  if (errorFlags == ERROR_NONE) {
    currentState = STATE_IDLE;
  } else {
    // Stay in error state, retry in 5 seconds
    delay(5000);
  }
}
```

---

### **Pattern: Watchdog Timer**

**Purpose:** Auto-reboot if firmware hangs

```
#include <esp_task_wdt.h>

#define WDT_TIMEOUT 30  // 30 seconds

void setup() {
  // Enable watchdog
  esp_task_wdt_init(WDT_TIMEOUT, true);
  esp_task_wdt_add(NULL);  // Add current task
}

void loop() {
  // Reset watchdog (must be called every <30 seconds)
  esp_task_wdt_reset();
  
  // State machine...
}
```

**Critical:** Call `esp_task_wdt_reset()` in every loop iteration

**If loop() hangs:** System auto-reboots after 30 seconds

---

## Pattern 5: Memory Management

### **Problem: Heap Fragmentation**

**Bad practice:**

```
void loop() {
  String data = String(windSpeed) + "," + String(rpm);  // ❌ Allocates heap memory
  // Memory leak over time
}
```

**ESP32 has ~300KB free RAM, but fragmentation causes crashes**

---

### **Solution 1: Pre-Allocate Buffers**

```
// Global fixed-size buffers
char csvBuffer;       // For CSV formatting
char jsonBuffer;      // For MQTT payloads
uint8_t sdBuffer;     // For SD card writes

void formatCSV(SensorData* data) {
  snprintf(csvBuffer, sizeof(csvBuffer),
    "%lu,%.2f,%.1f,%.2f,%.1f,%.1f,%.1f,%.3f,%.3f\n",
    data->timestamp,
    data->windSpeed,
    data->rpm,
    data->power,
    data->temp,
    data->pressure,
    data->humidity,
    data->lambda,
    data->cp
  );
}
```

**Benefits:**
- ✅ No heap allocation
- ✅ Predictable memory usage
- ✅ Fast (no malloc overhead)

---

### **Solution 2: Stack Allocation**

```
void processData() {
  // Allocated on stack (freed automatically when function returns)
  char tempBuffer;[1]
  
  snprintf(tempBuffer, sizeof(tempBuffer), "Wind: %.2f m/s", windSpeed);
  Serial.println(tempBuffer);
  
  // tempBuffer freed here
}
```

**Rule:** Stack limited to ~8KB on ESP32, use for small temporary buffers only

---

### **Solution 3: Avoid String Class**

**Problem with `String`:**
```
String msg = "Wind: " + String(windSpeed) + " m/s";
// 3 heap allocations, prone to fragmentation
```

**Better: C strings**
```
char msg;[2]
snprintf(msg, sizeof(msg), "Wind: %.2f m/s", windSpeed);
// 1 stack allocation
```

---

### **Memory Monitoring**

```
void printMemoryUsage() {
  Serial.print("Free heap: ");
  Serial.print(ESP.getFreeHeap());
  Serial.print(" bytes, Min free: ");
  Serial.print(ESP.getMinFreeHeap());
  Serial.println(" bytes");
}
```

**Call periodically to detect memory leaks:**
- If `getFreeHeap()` steadily decreases → memory leak
- If `getMinFreeHeap()` < 10KB → risk of crash

---

## Pattern 6: Interrupt Safety

### **Problem: Race Conditions**

**Vulnerable code:**

```
volatile unsigned long pulseCount = 0;

void IRAM_ATTR hallISR() {
  pulseCount++;  // Interrupt modifies
}

void loop() {
  float rpm = (pulseCount / 1.0) * 60;  // Main loop reads
  pulseCount = 0;  // ❌ RACE CONDITION
}
```

**Issue:** If interrupt fires between read and reset, count lost

---

### **Solution: Atomic Operations**

```
volatile unsigned long pulseCount = 0;

void IRAM_ATTR hallISR() {
  pulseCount++;
}

unsigned long getPulseCount() {
  noInterrupts();  // Disable interrupts
  unsigned long count = pulseCount;
  pulseCount = 0;
  interrupts();    // Re-enable
  return count;
}

void loop() {
  unsigned long count = getPulseCount();  // Atomic read-and-reset
  float rpm = (count / 1.0) * 60;
}
```

---

### **ISR Best Practices**

**Rules for interrupt service routines:**

1. **Keep it short:** <10 microseconds ideal, <100 microseconds maximum
2. **No blocking:** No `delay()`, `Serial.print()`, `WiFi`, `SD`, etc.
3. **Volatile variables:** Any variable shared with ISR must be `volatile`
4. **IRAM attribute:** Store ISR in IRAM for speed

**Good ISR (minimal work):**

```
volatile bool dataReady = false;

void IRAM_ATTR sampleISR() {
  dataReady = true;  // Just set a flag
}
```

**Bad ISR (too much work):**

```
void IRAM_ATTR badISR() {
  float temp = bme280.readTemperature();  // ❌ I2C call (slow)
  File f = SD.open("data.csv");           // ❌ SD card (very slow)
  f.println(temp);                        // ❌ Blocking I/O
  f.close();
}
```

---

## Pattern 7: Modular Code Organization

### **Problem: One Giant .ino File**

**Typical student project:**
```
project.ino  (1500 lines)
```

**Issues:**
- Hard to navigate
- Can't reuse code
- Merge conflicts in group work

---

### **Solution: Multi-File Structure**

```
complete_system/
├── complete_system.ino    # Main file (setup, loop, state machine)
├── config.h               # Configuration constants
├── sensors.h              # Sensor function declarations
├── sensors.cpp            # Sensor implementations
├── data_logger.h
├── data_logger.cpp
├── mqtt_handler.h
├── mqtt_handler.cpp
└── calculations.h
└── calculations.cpp
```

**config.h (constants):**

```
#ifndef CONFIG_H
#define CONFIG_H

// Hardware pins
#define PIN_ANEMOMETER    34
#define PIN_HALL_SENSOR   15
#define PIN_SD_CS         5

// Turbine geometry
#define ROTOR_RADIUS_M    0.60
#define ROTOR_HEIGHT_M    1.50
#define SWEPT_AREA_M2     1.80

// Sampling
#define SAMPLE_RATE_HZ    1.0
#define LOG_INTERVAL_MS   1000

// WiFi
#define WIFI_SSID         "your_network"
#define WIFI_PASSWORD     "your_password"

#endif
```

**sensors.h (interface):**

```
#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>

struct SensorData {
  uint32_t timestamp;
  float windSpeed;
  float rpm;
  float voltage;
  float current;
  float power;
  float temp;
  float pressure;
  float humidity;
  float airDensity;
  float lambda;
  float cp;
};

bool initSensors();
bool readAllSensors(SensorData* data);
float readAnemometer();
float calculateRPM();
bool readPowerSensor(float* voltage, float* current);
bool readEnvironment(float* temp, float* pressure, float* humidity);

#endif
```

**sensors.cpp (implementation):**

```
#include "sensors.h"
#include "config.h"
#include <Adafruit_INA219.h>
#include <Adafruit_BME280.h>

Adafruit_INA219 ina219;
Adafruit_BME280 bme280;

bool initSensors() {
  if (!ina219.begin()) {
    Serial.println("ERROR: INA219 init failed");
    return false;
  }
  
  if (!bme280.begin(0x76)) {
    Serial.println("ERROR: BME280 init failed");
    return false;
  }
  
  return true;
}

bool readAllSensors(SensorData* data) {
  data->timestamp = millis();
  data->windSpeed = readAnemometer();
  data->rpm = calculateRPM();
  
  if (!readPowerSensor(&data->voltage, &data->current)) {
    return false;
  }
  data->power = data->voltage * data->current;
  
  if (!readEnvironment(&data->temp, &data->pressure, &data->humidity)) {
    return false;
  }
  
  return true;
}

// ... other functions
```

**Benefits:**
- ✅ Each file < 300 lines
- ✅ Easy to find functions
- ✅ Can reuse `sensors.cpp` in other projects
- ✅ Team members work on different files

---

## Pattern 8: Configuration Management

### **Problem: Hardcoded Values**

**Bad:**

```
float windSpeed = analogRead(34) * 0.2 + 0.4;  // What do these numbers mean?
```

---

### **Solution: Named Constants**

```
// config.h
#define ANEM_OFFSET_VOLTS   0.4
#define ANEM_SCALE_V_PER_MS 0.2
#define ADC_MAX_VALUE       4095
#define ADC_REF_VOLTAGE     3.3

// sensors.cpp
float readAnemometer() {
  int rawADC = analogRead(PIN_ANEMOMETER);
  float voltage = (rawADC / (float)ADC_MAX_VALUE) * ADC_REF_VOLTAGE;
  float windSpeed = (voltage - ANEM_OFFSET_VOLTS) / ANEM_SCALE_V_PER_MS;
  return windSpeed;
}
```

**Benefits:**
- ✅ Self-documenting
- ✅ Easy to recalibrate (change one value)
- ✅ No magic numbers

---

### **Advanced: Runtime Configuration**

**Store calibration values in EEPROM/Preferences:**

```
#include <Preferences.h>

Preferences prefs;

void saveCalibration() {
  prefs.begin("config", false);  // Read-write mode
  prefs.putFloat("anem_offset", 0.42);
  prefs.putFloat("anem_scale", 0.19);
  prefs.end();
}

void loadCalibration() {
  prefs.begin("config", true);  // Read-only mode
  anemOffset = prefs.getFloat("anem_offset", 0.4);  // Default 0.4 if not set
  anemScale = prefs.getFloat("anem_scale", 0.2);
  prefs.end();
}
```

**Use case:** Adjust calibration without recompiling firmware

---

## Pattern 9: Logging and Debugging

### **Problem: `Serial.print()` Everywhere**

**Issues:**
- Slows down code (serial output is slow)
- Can't disable in production
- No log levels (info vs error)

---

### **Solution: Debug Macros**

```
// config.h
#define DEBUG_LEVEL 2  // 0=None, 1=Error, 2=Warning, 3=Info, 4=Debug

#if DEBUG_LEVEL >= 1
  #define DEBUG_ERROR(x) Serial.println("[ERROR] " x)
#else
  #define DEBUG_ERROR(x)
#endif

#if DEBUG_LEVEL >= 2
  #define DEBUG_WARN(x) Serial.println("[WARN] " x)
#else
  #define DEBUG_WARN(x)
#endif

#if DEBUG_LEVEL >= 3
  #define DEBUG_INFO(x) Serial.println("[INFO] " x)
#else
  #define DEBUG_INFO(x)
#endif

#if DEBUG_LEVEL >= 4
  #define DEBUG_DEBUG(x) Serial.println("[DEBUG] " x)
#else
  #define DEBUG_DEBUG(x)
#endif
```

**Usage:**

```
DEBUG_INFO("System initialized");
DEBUG_WARN("SD card slow, check class rating");
DEBUG_ERROR("Sensor timeout!");
DEBUG_DEBUG("ADC raw value: 2048");
```

**Set `DEBUG_LEVEL = 0` for production → all debug code compiled out (zero overhead)**

---

### **Advanced: Persistent Logging**

**Log errors to SD card:**

```
void logError(const char* message) {
  File errorLog = SD.open("/errors.log", FILE_APPEND);
  if (errorLog) {
    char timestamp;[3]
    snprintf(timestamp, sizeof(timestamp), "[%lu] ", millis());
    errorLog.print(timestamp);
    errorLog.println(message);
    errorLog.close();
  }
}
```

**Review logs after field deployment to diagnose issues**

---

## Pattern 10: Testing and Validation

### **Problem: "It works on my desk"**

**Code that works in lab but fails in field:**
- No error handling
- Untested edge cases
- Timing assumptions

---

### **Solution: Built-In Self-Test (BIST)**

```
bool runSelfTest() {
  Serial.println("\n=== SELF TEST ===");
  
  // Test 1: Memory
  Serial.print("Free heap: ");
  Serial.print(ESP.getFreeHeap());
  if (ESP.getFreeHeap() < 50000) {
    Serial.println(" [FAIL]");
    return false;
  }
  Serial.println(" [PASS]");
  
  // Test 2: SD Card
  Serial.print("SD card: ");
  if (!SD.begin(PIN_SD_CS)) {
    Serial.println("[FAIL]");
    return false;
  }
  Serial.println("[PASS]");
  
  // Test 3: I2C Sensors
  Serial.print("INA219: ");
  if (!ina219.begin()) {
    Serial.println("[FAIL]");
    return false;
  }
  Serial.println("[PASS]");
  
  Serial.print("BME280: ");
  if (!bme280.begin(0x76)) {
    Serial.println("[FAIL]");
    return false;
  }
  Serial.println("[PASS]");
  
  // Test 4: WiFi
  Serial.print("WiFi: ");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    attempts++;
  }
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("[PASS]");
  } else {
    Serial.println("[FAIL - continuing without WiFi]");
  }
  
  Serial.println("=== TEST COMPLETE ===\n");
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  if (!runSelfTest()) {
    Serial.println("CRITICAL: Self-test failed, halting");
    while(1) { delay(1000); }  // Halt system
  }
  
  // Continue with normal initialization...
}
```

---

### **Unit Testing (Advanced)**

**For complex algorithms, test on PC first:**

```
// calculations.cpp (same code runs on PC and ESP32)
float calculateCp(float power, float windSpeed, float airDensity, float sweptArea) {
  if (windSpeed < 0.5) return 0.0;  // Avoid division by zero
  
  float windPower = 0.5 * airDensity * sweptArea * pow(windSpeed, 3);
  float cp = power / windPower;
  
  // Validate against Betz limit
  if (cp > 0.6) return 0.0;  // Invalid (exceeds Betz limit)
  if (cp < 0.0) return 0.0;  // Invalid (negative)
  
  return cp;
}
```

**Test on PC with assertions:**

```
// test_calculations.cpp (compile on PC, not ESP32)
#include <assert.h>
#include "calculations.h"

void testCalculateCp() {
  // Test 1: Typical case
  float cp = calculateCp(250, 8.0, 1.2, 1.8);
  assert(cp > 0.25 && cp < 0.4);  // Reasonable range
  
  // Test 2: Zero wind
  cp = calculateCp(100, 0.0, 1.2, 1.8);
  assert(cp == 0.0);  // Should return 0, not divide by zero
  
  // Test 3: Exceeds Betz limit (invalid)
  cp = calculateCp(1000, 3.0, 1.2, 1.8);
  assert(cp == 0.0);  // Should reject impossible values
  
  printf("All tests passed!\n");
}
```

---

## Complete Example: Well-Structured Firmware

### **File: complete_system.ino**

```
#include "config.h"
#include "sensors.h"
#include "data_logger.h"
#include "mqtt_handler.h"
#include "calculations.h"

// State machine
enum SystemState {
  STATE_INIT,
  STATE_IDLE,
  STATE_ACQUIRE,
  STATE_PROCESS,
  STATE_LOG,
  STATE_TRANSMIT,
  STATE_ERROR
};

SystemState currentState = STATE_INIT;
SensorData sensorData;
volatile bool sampleReady = false;
uint8_t errorFlags = 0;

// Hardware timer ISR
void IRAM_ATTR onSampleTimer() {
  sampleReady = true;
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  DEBUG_INFO("System starting...");
  
  // Self-test
  if (!runSelfTest()) {
    currentState = STATE_ERROR;
    errorFlags = 0xFF;
    return;
  }
  
  // Initialize subsystems
  if (!initSensors()) {
    errorFlags |= ERROR_SENSOR_INIT;
  }
  
  if (!initDataLogger()) {
    errorFlags |= ERROR_SD_INIT;
  }
  
  if (!initMQTT()) {
    errorFlags |= ERROR_MQTT_INIT;
  }
  
  // Setup sample timer (1 Hz)
  setupSampleTimer(1000000, onSampleTimer);  // 1,000,000 μs = 1 second
  
  if (errorFlags == 0) {
    DEBUG_INFO("Initialization complete");
    currentState = STATE_IDLE;
  } else {
    DEBUG_ERROR("Initialization errors detected");
    currentState = STATE_ERROR;
  }
}

void loop() {
  esp_task_wdt_reset();  // Feed watchdog
  
  switch (currentState) {
    case STATE_INIT:
      // Already handled in setup()
      currentState = STATE_IDLE;
      break;
    
    case STATE_IDLE:
      if (sampleReady) {
        sampleReady = false;
        currentState = STATE_ACQUIRE;
      }
      break;
    
    case STATE_ACQUIRE:
      if (readAllSensors(&sensorData)) {
        currentState = STATE_PROCESS;
      } else {
        errorFlags |= ERROR_SENSOR_READ;
        currentState = STATE_ERROR;
      }
      break;
    
    case STATE_PROCESS:
      calculateDerivedQuantities(&sensorData);
      currentState = STATE_LOG;
      break;
    
    case STATE_LOG:
      if (logData(&sensorData)) {
        if (shouldTransmit()) {
          currentState = STATE_TRANSMIT;
        } else {
          currentState = STATE_IDLE;
        }
      } else {
        errorFlags |= ERROR_SD_WRITE;
        currentState = STATE_ERROR;
      }
      break;
    
    case STATE_TRANSMIT:
      transmitMQTT(&sensorData);
      currentState = STATE_IDLE;
      break;
    
    case STATE_ERROR:
      handleSystemError();
      break;
  }
  
  // Background tasks
  handleMQTTLoop();
  checkSerialCommands();
}
```

**Lines of code:** ~100 (vs. 1500 in spaghetti code)  
**Readability:** High  
**Maintainability:** Excellent  
**Testability:** Each module can be tested independently

---

## Summary Checklist

**Before deploying firmware, verify:**

- [ ] State machine architecture implemented
- [ ] All timing is non-blocking (`millis()` or hardware timers)
- [ ] Sensor reads have timeout and validation
- [ ] Error flags defined for all subsystems
- [ ] Watchdog timer enabled
- [ ] No `String` objects in performance-critical code
- [ ] Interrupts use `volatile` and atomic access
- [ ] Code split into logical modules (<300 lines/file)
- [ ] Debug macros for logging (disable in production)
- [ ] Self-test runs on startup
- [ ] Memory usage monitored (free heap logged)
- [ ] All magic numbers replaced with named constants

**Deployment confidence:** High if all checked ✓

---

## Additional Resources

### **Books**
- *Embedded Systems Architecture* by Tammy Noergaard
- *Making Embedded Systems* by Elecia White

### **Online**
- ESP32 Technical Reference: https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf
- FreeRTOS Documentation: https://www.freertos.org/Documentation/RTOS_book.html

### **Code Examples**
- Official ESP32 Arduino examples: `File → Examples → ESP32`
- Repository examples: [`../../firmware/examples/`](../../firmware/examples/)

---

**Questions about firmware design?** Open an issue: https://github.com/asithakal/wind-turbine-daq-guide/issues