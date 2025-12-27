# ESP32 Driver Installation Guide

Install ESP32 board support and USB drivers for Arduino IDE.

**Estimated time:** 20 minutes  
**Skill level:** Beginner  
**Prerequisites:** Arduino IDE 2.x installed ([Step 1](01_arduino_ide_setup.md))

---

## Overview

To program ESP32 boards, you need:
1. **Board definitions** (tells Arduino IDE about ESP32)
2. **USB-to-UART drivers** (allows computer to talk to ESP32)
3. **Board selection** (choose your specific ESP32 model)

---

## Part 1: Install ESP32 Board Support

### **Step 1: Add ESP32 Board Manager URL**

1. Open **Arduino IDE**
2. **File** → **Preferences**
3. Find field: **"Additional boards manager URLs:"**
4. Paste this URL:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
5. **Multiple URLs?** Separate with commas or click icon next to field to open multi-line editor
6. Click **"OK"**

**Screenshot reference:** Field is near bottom of Preferences window

---

### **Step 2: Install ESP32 Boards**

1. **Tools** → **Board** → **Boards Manager**
2. Type **"esp32"** in search box
3. Find **"esp32 by Espressif Systems"** (official package)
4. Click **"INSTALL"** button
5. Wait for download (~250 MB, takes 3-5 minutes)
6. Progress bar will show: "Installing tools..."

**Success message:** "INSTALLED" badge appears next to version number

**Which version?** Use latest stable (e.g., 2.0.14 as of Dec 2024). Avoid pre-release versions (labeled "rc" or "beta")

---

### **Step 3: Verify Installation**

1. **Tools** → **Board** → **ESP32 Arduino**
2. You should now see long list of ESP32 boards:
   - ESP32 Dev Module ✓ (most common)
   - ESP32-WROOM-DA Module
   - ESP32-C3 Dev Module
   - ESP32-S3 Dev Module
   - etc.

**Success!** ESP32 support installed.

---

## Part 2: Install USB Drivers

### **Identify Your ESP32 Board's USB Chip**

Most ESP32 boards use one of these USB-to-UART chips:
- **CP210x** (Silicon Labs) - Most common on NodeMCU-32S, DOIT boards
- **CH340** (WCH) - Common on cheap clones
- **FTDI** - Rare, mostly on older boards

**How to identify:**
- Check board label or product page
- Or try plugging in: Windows will show "Unknown device" with chip name in Device Manager

---

### **Windows Driver Installation**

#### **Method 1: Automatic Detection (Recommended)**

1. Connect ESP32 to computer via USB cable
2. Windows will attempt automatic driver installation
3. Wait 1-2 minutes
4. Check **Device Manager** (Win+X → Device Manager):
   - If under **"Ports (COM & LPT)"** you see "Silicon Labs CP210x" or "USB-SERIAL CH340" → **Drivers already installed!**
   - If under **"Other devices"** you see "Unknown device" → Continue to manual installation

#### **Method 2: Manual Installation**

**For CP210x (Most Common):**

1. Download driver from: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
2. Click **"Downloads"** tab
3. Download **"CP210x Windows Drivers"** (ZIP file, ~2 MB)
4. Extract ZIP file
5. Run **"CP210xVCPInstaller_x64.exe"** (or x86 for 32-bit Windows)
6. Click **"Next"** → **"Install"**
7. **Windows Security warning?** Click "Install this driver software anyway"
8. Reboot computer

**For CH340:**

1. Download from: http://www.wch.cn/downloads/CH341SER_EXE.html
2. Extract and run **"CH341SER.EXE"**
3. Click **"INSTALL"**
4. Reboot computer

**Verification:**
- Unplug and replug ESP32
- Device Manager → Ports → You should see "COM3" or similar

---

### **macOS Driver Installation**

**Good news:** macOS 10.14+ includes CP210x drivers by default!

**For CH340 (if needed):**

1. Download from: https://github.com/adrianmihalko/ch340g-ch34g-ch34x-mac-os-x-driver
2. Extract ZIP
3. Run installer package
4. **Security warning:** System Preferences → Security & Privacy → Click "Allow"
5. Reboot Mac

**Verification:**

```
# Before connecting ESP32:
ls /dev/tty.*

# After connecting:
ls /dev/tty.*
# Should see new entry like /dev/tty.SLAB_USBtoUART or /dev/tty.wchusbserial*
```

---

### **Linux Driver Installation**

**Good news:** Drivers already included in kernel!

**Only need to set permissions:**

```
# Add yourself to dialout group
sudo usermod -a -G dialout $USER

# Alternative (Arch Linux):
sudo usermod -a -G uucp $USER

# Log out and log back in (or reboot)

# Verify group membership:
groups
# Should include "dialout" or "uucp"
```

**Verification:**

```
# Before connecting:
ls /dev/ttyUSB*

# After connecting:
ls /dev/ttyUSB*
# Should see /dev/ttyUSB0 or similar

# Check device info:
dmesg | tail
# Should see "cp210x converter now attached to ttyUSB0" or similar
```

---

## Part 3: Configure Board Settings

### **Select Your ESP32 Board**

1. **Tools** → **Board** → **ESP32 Arduino** → **ESP32 Dev Module**

**Don't see your exact board model?** Use **"ESP32 Dev Module"** - it works for most boards.

**Common board selections:**
- NodeMCU-32S → **ESP32 Dev Module**
- DOIT ESP32 DEVKIT V1 → **ESP32 Dev Module**
- ESP32-WROOM-32 → **ESP32 Dev Module**
- ESP32-C3 boards → **ESP32C3 Dev Module**
- ESP32-S3 boards → **ESP32S3 Dev Module**

---

### **Select COM Port**

1. **Tools** → **Port**
2. Look for:
   - **Windows:** `COM3 (Silicon Labs CP210x)` or `COM5 (USB-SERIAL CH340)`
   - **macOS:** `/dev/cu.SLAB_USBtoUART` or `/dev/cu.wchusbserial*`
   - **Linux:** `/dev/ttyUSB0` or `/dev/ttyACM0`

**Multiple ports?** Unplug ESP32, note which port disappears, replug, select that port.

**No port shown?** Driver issue - revisit Part 2 above.

---

### **Configure Upload Speed (Optional)**

1. **Tools** → **Upload Speed** → **115200** (safe default)

**Faster uploads:** Try 921600 if your cable is good quality  
**Upload errors:** Reduce to 115200 or 460800

---

## Part 4: Test Upload

### **Upload Blink Test**

1. **File** → **Examples** → **01.Basics** → **Blink**
2. Modify LED pin for ESP32:

```
// Change line 9 from:
// int led = 13;

// To:
int led = 2;  // Most ESP32 boards have onboard LED on GPIO 2
```

3. Click **→ (Upload)** button
4. Watch output window:
   ```
   Connecting........_____.....
   Chip is ESP32-D0WDQ6 (revision 1)
   Writing at 0x00001000... (7%)
   ...
   Leaving...
   Hard resetting via RTS pin...
   ```

5. **Success!** Onboard LED should blink every second.

---

## Troubleshooting

### **Problem: "Failed to connect to ESP32"**

**Solution 1: Press BOOT button**
- Hold **BOOT** button on ESP32 board
- Click **Upload** in Arduino IDE
- Release BOOT when you see "Connecting..."

**Solution 2: Check USB cable**
- Use data cable, not charge-only cable
- Try different cable

**Solution 3: Lower upload speed**
- **Tools** → **Upload Speed** → **115200**

---

### **Problem: "Port not available" or greyed out**

**Windows:**
```
# Check Device Manager
devmgmt.msc

# Under "Ports (COM & LPT)", look for:
# - Yellow exclamation mark = driver issue
# - Red X = device disabled

# Right-click → Update driver
```

**macOS:**
```
# Check if port appears at all:
ls /dev/cu.*

# If empty, check System Information:
# Apple menu → About This Mac → System Report → USB
# Look for "CP2102" or "CH340" device
```

**Linux:**
```
# Check port permissions:
ls -l /dev/ttyUSB0

# Should show: crw-rw---- 1 root dialout
# If you're not in dialout group:
sudo usermod -a -G dialout $USER
# Then logout/login
```

---

### **Problem: "Brownout detector was triggered"**

**Cause:** Insufficient power from USB port

**Solutions:**
- Use powered USB hub
- Try different USB port (avoid USB 3.0 on some laptops)
- Add external 5V power supply to ESP32 VIN pin

---

### **Problem: Upload works but nothing happens**

**Check:**
1. LED pin: Most ESP32 boards use GPIO 2 (some use GPIO 5 or GPIO 13)
2. Serial Monitor: **Tools** → **Serial Monitor** → Set to **115200 baud**
3. EN button: Press **EN** (reset) button on board after upload

---

## Verification Checklist

Before continuing to next guide, verify:

- [ ] ESP32 board appears in Tools → Board menu
- [ ] COM port appears in Tools → Port menu
- [ ] Blink example uploads successfully
- [ ] Onboard LED blinks
- [ ] Serial Monitor shows output at 115200 baud

**All checked?** You're ready for [Library Installation](03_library_installation.md)!

---

## Board-Specific Notes

### **ESP32-C3 / ESP32-S3**

These newer chips have **built-in USB** (no separate USB-to-UART chip):

1. **Tools** → **USB CDC On Boot** → **"Enabled"**
2. **Tools** → **Upload Mode** → **"UART0 / Hardware CDC"**

### **ESP32-CAM**

No USB port! Requires external USB-to-UART programmer:
- Connect: GND-GND, 5V-5V, U0T-TX, U0R-RX
- Connect GPIO 0 to GND before uploading
- See detailed guide: [`../troubleshooting/hardware_debugging.md`](../troubleshooting/hardware_debugging.md#esp32-cam-programming)

---

## Next Steps

✅ **ESP32 drivers installed successfully!**

**Continue to:**
- [`03_library_installation.md`](03_library_installation.md) - Install sensor libraries
- [`../firmware/examples/01_blink_test/`](../../firmware/examples/01_blink_test/) - First ESP32 project

---

**Need help?** Open an issue: https://github.com/asithakal/wind-turbine-daq-guide/issues
```

***

### **3. `docs/setup_guides/03_library_installation.md`**

```markdown
# Arduino Library Installation Guide

Install all required libraries for the wind turbine DAQ system.

**Estimated time:** 30 minutes  
**Skill level:** Beginner  
**Prerequisites:** 
- Arduino IDE 2.x with ESP32 support ([Steps 1-2](01_arduino_ide_setup.md))
- ESP32 board connected and working

---

## Required Libraries Overview

| Library | Purpose | Version | Size |
|---------|---------|---------|------|
| **Adafruit INA219** | Power monitoring (current/voltage) | ≥1.2.0 | ~50 KB |
| **Adafruit BME280** | Environmental sensor (temp/humidity/pressure) | ≥2.2.2 | ~80 KB |
| **PubSubClient** | MQTT communication | ≥2.8.0 | ~30 KB |
| **ArduinoJson** | JSON data formatting | ≥6.21.0 | ~200 KB |
| **SD** | SD card logging | Built-in | N/A |
| **SPI** | SPI communication protocol | Built-in | N/A |
| **Wire** | I2C communication protocol | Built-in | N/A |

**Total download:** ~360 KB

---

## Method 1: Library Manager (Recommended)

### **Step 1: Open Library Manager**

1. **Tools** → **Manage Libraries...** (or Ctrl+Shift+I / Cmd+Shift+I)
2. Library Manager window opens
3. Wait for index update (~10 seconds on first open)

---

### **Step 2: Install Adafruit INA219**

1. Type **"INA219"** in search box
2. Find **"Adafruit INA219"** by Adafruit
   - ⚠️ Avoid similar names: "INA219_WE" or "INA219 library" (different authors)
3. Click **"INSTALL"** button
4. **Dependencies detected:** Click "INSTALL ALL" to include:
   - Adafruit BusIO
   - Adafruit Unified Sensor
5. Wait for "INSTALLED" badge

**Version note:** Use latest stable (1.2.3 or newer). Avoid pre-release versions.

---

### **Step 3: Install Adafruit BME280**

1. Clear search box, type **"BME280"**
2. Find **"Adafruit BME280 Library"** by Adafruit
   - ⚠️ **Not "BME280" by Tyler Glenn** (different API)
3. Click **"INSTALL"**
4. Dependencies (Adafruit Unified Sensor, Adafruit BusIO) may already be installed
5. Click "CLOSE" if prompted

---

### **Step 4: Install PubSubClient**

1. Search **"PubSubClient"**
2. Find **"PubSubClient"** by Nick O'Leary
3. Click **"INSTALL"**
4. No dependencies

**Use case:** MQTT communication for cloud data transmission (optional - can skip if only using local SD logging)

---

### **Step 5: Install ArduinoJson**

1. Search **"ArduinoJson"**
2. Find **"ArduinoJson"** by Benoit Blanchon
3. Click **"INSTALL"**
4. **Choose version:** Select **6.21.x** series (not 7.x - API changed significantly)

**Why version 6?** Firmware examples use v6 API. Version 7 requires code modifications.

---

### **Step 6: Verify Built-in Libraries**

These ship with ESP32 board support (no installation needed):

**Check they exist:**

1. **File** → **Examples** → **Scroll to "Examples for ESP32 Dev Module"** section
2. You should see:
   - **SD** (SD card)
   - **SPI** (SPI protocol)
   - **Wire** (I2C protocol)
   - **WiFi** (networking)

**Missing?** Reinstall ESP32 board support ([Step 2](02_esp32_driver_installation.md))

---

## Method 2: Manual Installation (Alternative)

Use if Library Manager fails or you need specific versions.

### **Step 1: Download Libraries**

**Adafruit INA219:**
1. Go to https://github.com/adafruit/Adafruit_INA219
2. Click green **"Code"** button → **"Download ZIP"**
3. Save `Adafruit_INA219-master.zip`

**Adafruit BME280:**
1. Go to https://github.com/adafruit/Adafruit_BME280_Library
2. Download ZIP

**PubSubClient:**
1. Go to https://github.com/knolleary/pubsubclient
2. Download ZIP

**ArduinoJson:**
1. Go to https://github.com/bblanchon/ArduinoJson
2. Click **"Releases"** → Download **v6.21.x** ZIP (not latest v7)

---

### **Step 2: Install from ZIP**

1. Arduino IDE → **Sketch** → **Include Library** → **Add .ZIP Library...**
2. Select downloaded ZIP file
3. Click **"Open"**
4. Wait for "Library added to your libraries" message
5. Repeat for each library

---

### **Step 3: Install Dependencies**

For Adafruit libraries, also install:

**Adafruit BusIO:**
- https://github.com/adafruit/Adafruit_BusIO

**Adafruit Unified Sensor:**
- https://github.com/adafruit/Adafruit_Sensor

---

## Verification

### **Test 1: Check Library List**

1. **Sketch** → **Include Library** → Scroll to **"Contributed libraries"**
2. You should see:
   - Adafruit BME280 Library
   - Adafruit INA219
   - ArduinoJson
   - PubSubClient

**Duplicates?** Remove from:
- **Windows:** `Documents\Arduino\libraries\`
- **macOS:** `~/Documents/Arduino/libraries/`
- **Linux:** `~/Arduino/libraries/`

---

### **Test 2: Compile Test Sketch**

Paste this code into Arduino IDE:

```
// Library compatibility test
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <Adafruit_INA219.h>
#include <Adafruit_BME280.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

void setup() {
  Serial.begin(115200);
  Serial.println("All libraries loaded successfully!");
}

void loop() {
  // Empty
}
```

**Click Verify (✓ button)**

**Expected output:**
```
Compiling sketch...
Sketch uses 234567 bytes (17%) of program storage space
Global variables use 12345 bytes (3%) of dynamic memory
```

**Success!** All libraries installed correctly.

**Error "No such file or directory"?** That library is missing - reinstall it.

---

## Library Locations

Libraries are stored in:

| OS | Path |
|----|------|
| **Windows** | `C:\Users\YourName\Documents\Arduino\libraries\` |
| **macOS** | `/Users/YourName/Documents/Arduino/libraries/` |
| **Linux** | `/home/yourname/Arduino/libraries/` |

**Troubleshooting:** Delete library folder and reinstall if corrupted.

---

## Library Version Management

### **Check Installed Versions**

1. **Tools** → **Manage Libraries**
2. Type library name
3. Installed version shown below library name

### **Update Libraries**

1. Library Manager → Find library
2. If "UPDATE" button visible → Click to update
3. **Caution:** Major version updates (e.g., ArduinoJson 6→7) may break code

### **Downgrade Library**

1. Library Manager → Select library
2. Click version dropdown (next to INSTALL button)
3. Select older version
4. Click "INSTALL"

---

## Common Library Issues

### **Problem: "Multiple libraries were found"**

**Example error:**
```
Multiple libraries were found for "SD.h"
  Used: C:\Users\...\Arduino\libraries\SD
  Not used: C:\Users\...\hardware\esp32\libraries\SD
```

**Cause:** Duplicate libraries in different locations

**Solution:**
1. Note the "Not used" path
2. Navigate to that folder
3. Delete or rename the duplicate library folder

---

### **Problem: Compilation errors after updating ArduinoJson**

**Error:**
```
'DeserializationError' does not name a type
```

**Cause:** ArduinoJson v7 has breaking API changes

**Solution: Downgrade to v6.21.x**
1. Library Manager → Search "ArduinoJson"
2. Version dropdown → Select **6.21.5**
3. Click "INSTALL"

---

### **Problem: "Adafruit_Sensor.h: No such file"**

**Cause:** Missing dependency

**Solution:**
1. Library Manager → Search "Adafruit Unified Sensor"
2. Install "Adafruit Unified Sensor" by Adafruit
3. Recompile sketch

---

## Library-Specific Notes

### **Adafruit INA219**

**Configuration:** Measures voltage and current  
**I2C Address:** 0x40 (default)  
**Example:** File → Examples → Adafruit INA219 → getcurrent

**Common issue:** If reading 0.00V:
- Check I2C wiring (SDA=GPIO 21, SCL=GPIO 22)
- Run I2C scanner: File → Examples → Wire → i2c_scanner

---

### **Adafruit BME280**

**Configuration:** Environmental sensor (temp/humidity/pressure)  
**I2C Address:** 0x76 or 0x77 (check with scanner)  
**Example:** File → Examples → Adafruit BME280 → bme280test

**Tip:** If readings are NaN:
```
// Change address in code:
bme.begin(0x76);  // Try 0x77 if not working
```

---

### **PubSubClient (MQTT)**

**Configuration:** Requires WiFi setup first  
**Max message size:** 256 bytes (default), increase if needed:

```
// In PubSubClient.h (line 26):
#define MQTT_MAX_PACKET_SIZE 512  // Increase from 256
```

**Example:** File → Examples → PubSubClient → mqtt_esp8266

---

### **ArduinoJson**

**Memory allocation:** Use `StaticJsonDocument` for fixed size:

```
StaticJsonDocument<200> doc;  // 200 bytes on stack
doc["wind_speed"] = 7.5;
doc["power"] = 320;
serializeJson(doc, Serial);
```

**Documentation:** https://arduinojson.org/v6/doc/

---

## Optional Libraries

### **For Advanced Features:**

| Library | Purpose | Install Command |
|---------|---------|-----------------|
| **WiFiManager** | Easy WiFi configuration | Search "WiFiManager" by tzapu |
| **ESP32Time** | Real-time clock | Search "ESP32Time" |
| **Preferences** | Non-volatile storage | Built-in with ESP32 |
| **ESPAsyncWebServer** | Web dashboard | Manual install from GitHub |

---

## Next Steps

✅ **All libraries installed successfully!**

**Continue to:**
- [`04_first_upload.md`](04_first_upload.md) - Upload your first complete sketch
- [`../../firmware/examples/03_i2c_sensors/`](../../firmware/examples/03_i2c_sensors/) - Test sensors

---

## Library Versions Tested

This guide verified with:
- Adafruit INA219: v1.2.3
- Adafruit BME280: v2.2.4
- PubSubClient: v2.8.0
- ArduinoJson: v6.21.5
- ESP32 Board Support: v2.0.14

**Date:** December 2024

---

**Need help?** Open an issue: https://github.com/asithakal/wind-turbine-daq-guide/issues