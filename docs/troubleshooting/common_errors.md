# Common Firmware Errors and Solutions

Comprehensive troubleshooting guide for Arduino IDE compilation and upload errors.

**Quick navigation:**
- [Compilation Errors](#compilation-errors)
- [Upload Errors](#upload-errors)
- [Runtime Errors](#runtime-errors)
- [Library Errors](#library-errors)
- [Memory Errors](#memory-errors)

---

## Compilation Errors

### **Error 1: "Board not found" or "Platform not installed"**

**Full error message:**
```
Error compiling for board ESP32 Dev Module.
Platform 'espressif:esp32' not installed
```

**Cause:** ESP32 board support not installed

**Solution:**

1. Install ESP32 board support:
   ```
   Tools → Board → Boards Manager → Search "esp32" → Install "esp32 by Espressif"
   ```
2. Restart Arduino IDE
3. Select board: Tools → Board → ESP32 Arduino → ESP32 Dev Module

**Verification:**
- Tools → Board should show "ESP32 Dev Module" (or your board)

---

### **Error 2: "No such file or directory" (Missing Library)**

**Example error:**
```
fatal error: Adafruit_INA219.h: No such file or directory
 #include <Adafruit_INA219.h>
          ^~~~~~~~~~~~~~~~~~~
compilation terminated.
```

**Cause:** Required library not installed

**Solution:**

1. **Identify missing library** from error message (e.g., `Adafruit_INA219.h` → Need "Adafruit INA219" library)
2. Install via Library Manager:
   ```
   Sketch → Include Library → Manage Libraries → Search library name → Install
   ```
3. Common missing libraries:
   - `Adafruit_INA219.h` → Install "Adafruit INA219"
   - `Adafruit_BME280.h` → Install "Adafruit BME280 Library"
   - `PubSubClient.h` → Install "PubSubClient"
   - `ArduinoJson.h` → Install "ArduinoJson" (v6.21.x)

**See also:** [Library Installation Guide](../setup_guides/03_library_installation.md)

---

### **Error 3: "Multiple libraries were found for X.h"**

**Example error:**
```
Multiple libraries were found for "SD.h"
  Used: C:\Arduino\libraries\SD
  Not used: C:\ESP32\libraries\SD
```

**Cause:** Duplicate libraries in different locations

**Solution:**

**Option 1: Remove duplicate (recommended)**
1. Note the "Not used" path from error message
2. Navigate to that folder in File Explorer
3. Delete or rename the duplicate library folder

**Option 2: Use specific library**
```
// In your sketch, use full path:
#include <SD.h>  // Let Arduino choose

// Or explicitly:
#include "C:/Arduino/libraries/SD/src/SD.h"
```

**Prevention:** Only install libraries via Library Manager, not manually

---

### **Error 4: "'X' was not declared in this scope"**

**Example errors:**
```
error: 'Serial' was not declared in this scope
error: 'pinMode' was not declared in this scope
error: 'digitalWrite' was not declared in this scope
```

**Common causes and solutions:**

**A. Typo in function/variable name**
```
// Wrong:
seriall.println("Hello");  // Extra 'l'

// Correct:
Serial.println("Hello");
```

**B. Missing #include**
```
// If using I2C:
#include <Wire.h>  // Must include for Wire functions

// If using SPI:
#include <SPI.h>   // Must include for SPI functions
```

**C. Variable declared inside wrong scope**
```
// Wrong:
void setup() {
  int sensorValue = 0;
}
void loop() {
  Serial.println(sensorValue);  // ERROR: not in scope
}

// Correct:
int sensorValue = 0;  // Declare globally

void setup() {
  // ...
}
void loop() {
  Serial.println(sensorValue);  // OK
}
```

---

### **Error 5: "Expected ';' before X"**

**Example error:**
```
error: expected ';' before 'void'
```

**Cause:** Missing semicolon on previous line

**Solution:**

```
// Wrong:
int sensorPin = 34  // Missing semicolon
void setup() {

// Correct:
int sensorPin = 34;  // Added semicolon
void setup() {
```

**Tip:** Error often points to line *after* the actual problem

---

### **Error 6: "Invalid conversion from 'const char*' to 'char'"**

**Example code causing error:**
```
char* ssid = "MyWiFi";  // ERROR
```

**Cause:** String literal is `const char*`, cannot assign to `char*`

**Solution:**

```
// Option 1: Use const (recommended):
const char* ssid = "MyWiFi";

// Option 2: Use char array:
char ssid[] = "MyWiFi";

// Option 3: Use String object:
String ssid = "MyWiFi";
```

---

## Upload Errors

### **Error 7: "Failed to connect to ESP32: Timed out"**

**Full error:**
```
Connecting........_____....._____.....
Failed to connect to ESP32: Timed out waiting for packet header
```

**Cause:** ESP32 not entering bootloader mode

**Solution (try in order):**

**A. Press BOOT button manually:**
1. Hold **BOOT** button on ESP32
2. Click **Upload** in Arduino IDE
3. When "Connecting..." appears, release BOOT
4. Wait for upload to complete

**B. Check USB cable:**
- Use **data cable**, not charge-only cable
- Try different USB cable
- Try different USB port (avoid USB hubs)

**C. Lower upload speed:**
```
Tools → Upload Speed → 115200 (or 460800)
```

**D. Check drivers:**
- **Windows:** Device Manager → Ports → Should see "COM3 (Silicon Labs)" or similar
- **Linux:** `ls /dev/ttyUSB*` should show device
- Reinstall drivers: [Driver Installation Guide](../setup_guides/02_esp32_driver_installation.md)

**E. Add capacitor (hardware fix):**
- Solder 10µF capacitor between **EN** and **GND** pins
- This auto-resets ESP32 during upload

---

### **Error 8: "Port not available" or "Access denied"**

**Windows error:**
```
Serial port 'COM3' not found
```

**Linux error:**
```
Permission denied: '/dev/ttyUSB0'
```

**Solutions:**

**Windows:**
1. Close any program using the port:
   - Serial Monitor in Arduino IDE
   - PuTTY, Tera Term, other terminal programs
   - Other Arduino IDE instances
2. Replug USB cable
3. Check Device Manager: Right-click port → Disable → Enable

**macOS:**
```
# List available ports:
ls /dev/cu.*

# If empty, check System Information:
# Apple menu → About This Mac → System Report → USB
```

**Linux:**
```
# Add user to dialout group (one-time):
sudo usermod -a -G dialout $USER

# Log out and log back in, or:
su - $USER

# Verify:
groups  # Should show "dialout"

# If still fails, try as root (temporary test):
sudo chmod 666 /dev/ttyUSB0
```

---

### **Error 9: "Brownout detector was triggered"**

**Serial Monitor shows:**
```
Brownout detector was triggered
ets Jun  8 2016 00:22:57
rst:0xc (SW_CPU_RESET),boot:0x13 (SPI_FAST_FLASH_BOOT)
```

**Cause:** ESP32 not getting enough power

**Solutions (try in order):**

1. **Use better power source:**
   - Try different USB port (USB 2.0 better than 3.0 for some boards)
   - Use powered USB hub
   - Use dedicated 5V/1A power supply to VIN pin

2. **Reduce power consumption:**
   ```
   // In setup():
   setCpuFrequencyMhz(80);  // Reduce from 240 MHz to 80 MHz
   WiFi.mode(WIFI_OFF);     // Disable WiFi if not needed
   ```

3. **Check hardware:**
   - Remove all sensors temporarily (test ESP32 alone)
   - Check for short circuits
   - Measure voltage at 3.3V pin (should be 3.2-3.4V)

4. **Disable brownout detector (last resort):**
   ```
   // Add to setup():
   // WARNING: Only for testing! Fix power supply properly!
   #include "soc/soc.h"
   #include "soc/rtc_cntl_reg.h"
   
   void setup() {
     WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);  // Disable brownout
   }
   ```

---

### **Error 10: "Invalid head of packet (0xXX)"**

**Full error:**
```
A fatal error occurred: Invalid head of packet (0x08)
```

**Cause:** Communication error during upload

**Solutions:**

1. **Try lower upload speed:**
   ```
   Tools → Upload Speed → 115200
   ```

2. **Replug USB cable** (both ends)

3. **Hold BOOT during entire upload:**
   - Hold BOOT button
   - Click Upload
   - Keep holding until "Leaving..." appears
   - Release BOOT

4. **Update ESP32 board support:**
   ```
   Tools → Board → Boards Manager → esp32 → Update
   ```

---

## Runtime Errors

### **Error 11: Watchdog Timer Reset**

**Serial Monitor shows:**
```
Task watchdog got triggered. The following tasks did not reset the watchdog in time:
 - IDLE0 (CPU 0)
```

**Cause:** Code taking too long in `loop()` without yielding

**Solutions:**

```
// Bad: Blocking delay in loop
void loop() {
  longFunction();  // Takes 10 seconds
  // Watchdog triggers!
}

// Good: Add yields or reduce time
void loop() {
  longFunction();
  delay(1);        // Yields to watchdog
  // or
  vTaskDelay(1);
}

// Better: Use non-blocking code
void loop() {
  if (millis() - lastTime > 1000) {
    doTask();
    lastTime = millis();
  }
  delay(1);  // Small yield
}
```

**Disable watchdog (debugging only):**
```
#include "esp_task_wdt.h"

void setup() {
  disableCore0WDT();  // Disable for core 0
  disableCore1WDT();  // Disable for core 1
}
```

---

### **Error 12: Guru Meditation Error**

**Serial Monitor shows:**
```
Guru Meditation Error: Core 0 panic'ed (LoadProhibited)
Exception was unhandled.
```

**Common causes:**

**A. NULL pointer access:**
```
// Bad:
File dataFile;
dataFile.println("Hello");  // ERROR: File not opened!

// Good:
File dataFile = SD.open("data.csv", FILE_WRITE);
if (dataFile) {
  dataFile.println("Hello");
  dataFile.close();
}
```

**B. Array out of bounds:**
```
// Bad:
int data;[1]
data = 100;  // ERROR: Index out of range![2]

// Good:
int data;[1]
if (index < 10) {
  data[index] = 100;
}
```

**C. Stack overflow:**
```
// Bad:
void loop() {
  char buffer;  // Too large for stack!
}

// Good:
char* buffer = (char*)malloc(10000);  // Use heap
// ... use buffer ...
free(buffer);
```

---

### **Error 13: "SD Card Mount Failed"**

**See dedicated guide:** [`sd_card_problems.md`](sd_card_problems.md)

Quick check:
```
if (!SD.begin(SD_CS_PIN)) {
  Serial.println("SD Mount Failed");
  Serial.println("Check: Card inserted? Formatted FAT32? Wiring correct?");
}
```

---

## Library Errors

### **Error 14: ArduinoJson - "DeserializationError does not name a type"**

**Cause:** Using ArduinoJson v6 syntax with v7 library (or vice versa)

**Solution: Downgrade to v6.21.x**
```
Tools → Manage Libraries → ArduinoJson → Version 6.21.5 → Install
```

**Version differences:**
```
// ArduinoJson v6 (use this):
StaticJsonDocument<200> doc;
DeserializationError error = deserializeJson(doc, jsonString);

// ArduinoJson v7 (different API):
JsonDocument doc;
auto error = deserializeJson(doc, jsonString);
```

---

### **Error 15: Adafruit Sensor - "Adafruit_Sensor.h not found"**

**Full error:**
```
fatal error: Adafruit_Sensor.h: No such file or directory
```

**Cause:** Missing dependency for Adafruit sensors

**Solution:**
```
Sketch → Include Library → Manage Libraries 
→ Search "Adafruit Unified Sensor" → Install
```

**Also install:**
- Adafruit BusIO (if using I2C/SPI sensors)

---

### **Error 16: PubSubClient - "MQTT message too large"**

**Symptom:** MQTT publish fails silently, or returns `false`

**Cause:** Default max packet size is 256 bytes

**Solution:**

**Option 1: Increase buffer size (in library)**
1. Navigate to: `Documents/Arduino/libraries/PubSubClient/src/PubSubClient.h`
2. Find line: `#define MQTT_MAX_PACKET_SIZE 256`
3. Change to: `#define MQTT_MAX_PACKET_SIZE 512` (or larger)
4. Save and recompile sketch

**Option 2: Reduce payload size**
```
// Instead of sending all data in one message:
client.publish("turbine/data", longJsonString);  // Fails if > 256 bytes

// Split into multiple messages:
client.publish("turbine/wind", windSpeedString);
client.publish("turbine/power", powerString);
client.publish("turbine/rpm", rpmString);
```

---

## Memory Errors

### **Error 17: "Region 'DRAM' overflowed by X bytes"**

**Full error:**
```
region `dram0_0_seg' overflowed by 12345 bytes
collect2.exe: error: ld returned 1 exit status
```

**Cause:** Too much global/static data

**Solutions:**

**A. Move large arrays to PROGMEM (Flash storage):**
```
// Bad: Uses RAM
const char message[] = "Very long string...";

// Good: Uses Flash
const char message[] PROGMEM = "Very long string...";

// Access with:
char buffer;[3]
strcpy_P(buffer, message);
```

**B. Reduce buffer sizes:**
```
// Before:
char buffer;  // Too large!

// After:
char buffer;   // More reasonable
```

**C. Use dynamic allocation:**
```
// Before:
char logBuffer;  // Always allocated

// After:
char* logBuffer = (char*)malloc(1000);  // Only when needed
// ... use buffer ...
free(logBuffer);
```

---

### **Error 18: "Stack smashing detected"**

**Serial Monitor shows:**
```
***ERROR*** A stack overflow in task loopTask has been detected.
```

**Cause:** Too much stack usage (local variables, recursion)

**Solutions:**

1. **Reduce local variable size:**
   ```
   // Bad:
   void loop() {
     char buffer;  // Stack overflow!
   }
   
   // Good:
   void loop() {
     static char buffer;  // Uses heap
   }
   ```

2. **Increase stack size:**
   ```
   // In setup():
   // Default is 8192 bytes
   // Increase to 16384:
   esp_task_wdt_init(30, false);  // 30 second timeout
   ```

3. **Avoid deep recursion:**
   ```
   // Bad:
   void recursive(int n) {
     if (n > 0) recursive(n-1);  // Deep recursion
   }
   
   // Good:
   void iterative(int n) {
     for (int i = 0; i < n; i++) {
       // Do work
     }
   }
   ```

---

## Quick Reference Table

| Error Type | First Thing to Try |
|------------|-------------------|
| **Board not found** | Reinstall ESP32 board support |
| **Library not found** | Install via Library Manager |
| **Upload timeout** | Press BOOT button during upload |
| **Port not available (Linux)** | `sudo usermod -a -G dialout $USER` |
| **Brownout detector** | Use better power supply |
| **Watchdog reset** | Add `delay(1)` in loop |
| **Guru Meditation** | Check for NULL pointers |
| **Stack overflow** | Use `static` or `malloc()` |
| **SD card fails** | Check formatting (FAT32) |
| **MQTT message too large** | Increase `MQTT_MAX_PACKET_SIZE` |

---

## Debugging Tools

### **Serial Monitor Debugging**

```
void setup() {
  Serial.begin(115200);
  delay(1000);  // Wait for Serial to initialize
  
  Serial.println("=== DEBUGGING START ===");
  Serial.print("Free heap: ");
  Serial.println(ESP.getFreeHeap());
  Serial.print("CPU frequency: ");
  Serial.println(ESP.getCpuFreqMHz());
}

void loop() {
  Serial.print("Sensor value: ");
  Serial.println(analogRead(34));
  delay(1000);
}
```

### **I2C Scanner (Find Device Addresses)**

```
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin();
  
  Serial.println("I2C Scanner");
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("Device found at 0x");
      Serial.println(addr, HEX);
    }
  }
}

void loop() {}
```

### **Memory Monitor**

```
void printMemoryStats() {
  Serial.print("Free heap: ");
  Serial.print(ESP.getFreeHeap());
  Serial.println(" bytes");
  
  Serial.print("Heap size: ");
  Serial.print(ESP.getHeapSize());
  Serial.println(" bytes");
  
  Serial.print("Free PSRAM: ");
  Serial.print(ESP.getFreePsram());
  Serial.println(" bytes");
}
```

---

## Getting Further Help

**Before asking for help, provide:**
1. Complete error message (copy-paste from output window)
2. Code causing the error (use code blocks in GitHub issues)
3. Board type (ESP32 Dev Module, ESP32-C3, etc.)
4. Arduino IDE version and ESP32 board support version
5. What you've already tried

**Where to ask:**
- **This repository:** https://github.com/asithakal/wind-turbine-daq-guide/issues
- **ESP32 Forum:** https://esp32.com/
- **Arduino Forum:** https://forum.arduino.cc/c/using-arduino/programming-questions/
- **Stack Overflow:** Tag `[esp32]` and `[arduino]`

---

**Last updated:** December 2024  
**Tested with:** Arduino IDE 2.3.2, ESP32 Board Support 2.0.14