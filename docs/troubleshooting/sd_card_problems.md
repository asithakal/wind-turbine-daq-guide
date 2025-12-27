# SD Card Troubleshooting Guide

Complete diagnostic guide for SD card issues in ESP32 data logging systems.

**Quick navigation:**
- [Card Not Detected](#card-not-detected)
- [Formatting Issues](#formatting-issues)
- [Write Failures](#write-failures)
- [Data Corruption](#data-corruption)
- [Performance Problems](#performance-problems)
- [Hardware Issues](#hardware-issues)

---

## Card Not Detected

### **Error: "SD Card Mount Failed" or "Card initialization failed"**

**Serial Monitor shows:**
```
SD Card Mount Failed
or
Card initialization failed!
```

**Diagnostic flowchart:**

```
SD card not detected?
│
├─ Is card physically inserted?
│  └─ YES → Continue
│
├─ Is card formatted correctly?
│  ├─ Format: Must be FAT32 (not exFAT or NTFS)
│  ├─ Allocation size: 4096 bytes (default)
│  └─ See "Formatting Issues" section below
│
├─ Check wiring (power off first!):
│  ├─ CS   → GPIO 5
│  ├─ SCK  → GPIO 18
│  ├─ MOSI → GPIO 23
│  ├─ MISO → GPIO 19
│  ├─ VCC  → 3.3V or 5V (check module!)
│  └─ GND  → GND
│
├─ Check power supply:
│  ├─ Measure VCC on SD module: Should be 3.3V or 5V
│  └─ If 0V → Power wiring error
│
├─ Try different SD card:
│  ├─ Use 32GB or smaller (SDHC)
│  ├─ Avoid cheap/counterfeit cards
│  └─ Recommended: SanDisk, Samsung, Kingston
│
└─ Check code:
   ├─ Correct CS pin defined?
   ├─ SPI.begin() called?
   └─ See "Code Examples" below
```

---

### **Solution 1: Verify Hardware Connections**

**Step 1: Visual inspection**
- Remove and reinsert SD card
- Check for dust/debris in slot
- Ensure card clicks into place
- Check for bent pins on module

**Step 2: Wiring verification**

Use multimeter in continuity mode (power off!):

| Connection | ESP32 Pin | SD Module Pin | Continuity Test |
|------------|-----------|---------------|-----------------|
| Chip Select | GPIO 5 | CS | Should beep |
| Clock | GPIO 18 | SCK/CLK | Should beep |
| Data Out | GPIO 23 | MOSI/DI | Should beep |
| Data In | GPIO 19 | MISO/DO | Should beep |
| Power | 3.3V or 5V | VCC | Should beep |
| Ground | GND | GND | Should beep |

**Step 3: Power verification**

With system powered on and multimeter in DC voltage mode:

```
Black probe → ESP32 GND
Red probe → SD module VCC pin
Reading should be:
  - 3.3V ± 0.2V (if module is 3.3V)
  - 5.0V ± 0.2V (if module is 5V)

If 0V: Power wiring problem
If wrong voltage: Check module specs (some require 5V!)
```

---

### **Solution 2: Check Card Compatibility**

**Compatible cards:**
- ✅ SD (up to 2GB)
- ✅ SDHC (4GB to 32GB) ← **Recommended**
- ❌ SDXC (64GB+) - Often not supported by ESP32 SD library

**Card speed class:** (Usually not critical for 1 Hz logging)
- ✅ Class 4, 6, 10 - All work fine
- ✅ UHS-I, UHS-II - Backward compatible

**Brands tested:**
- ✅ SanDisk Ultra/Extreme (reliable)
- ✅ Samsung EVO/PRO
- ✅ Kingston Canvas
- ⚠️ Generic/no-name cards - Hit or miss
- ❌ Counterfeit cards - Common cause of failures

**Test with known-good card:**
Borrow a working SD card from camera/phone to isolate card vs. hardware issue.

---

### **Solution 3: Verify Initialization Code**

**Minimal working example:**

```
#include <SPI.h>
#include <SD.h>

#define SD_CS_PIN 5  // Chip Select pin

void setup() {
  Serial.begin(115200);
  delay(1000);  // Wait for Serial Monitor
  
  Serial.println("Initializing SD card...");
  
  // Initialize SPI bus first
  SPI.begin();
  
  // Initialize SD card
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD Card Mount Failed!");
    Serial.println("Check:");
    Serial.println("  1. Card inserted?");
    Serial.println("  2. Card formatted FAT32?");
    Serial.println("  3. Wiring correct?");
    Serial.println("  4. CS pin = GPIO 5?");
    return;
  }
  
  // Card type detection
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  
  Serial.print("SD Card Type: ");
  switch (cardType) {
    case CARD_MMC:  Serial.println("MMC"); break;
    case CARD_SD:   Serial.println("SDSC"); break;
    case CARD_SDHC: Serial.println("SDHC"); break;
    default:        Serial.println("UNKNOWN"); break;
  }
  
  // Card size
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  
  Serial.println("SD card initialized successfully!");
}

void loop() {
  // Empty
}
```

**Expected output (successful):**
```
Initializing SD card...
SD Card Type: SDHC
SD Card Size: 16384MB
SD card initialized successfully!
```

**Common code mistakes:**

❌ **Wrong:**
```
SD.begin();  // Missing CS pin!
```

✅ **Correct:**
```
SD.begin(5);  // Always specify CS pin (GPIO 5)
```

❌ **Wrong:**
```
SD.begin(5);
SPI.begin();  // SPI.begin() called AFTER SD.begin()
```

✅ **Correct:**
```
SPI.begin();  // SPI.begin() called FIRST
SD.begin(5);
```

---

## Formatting Issues

### **Problem: "Card formatted but still not recognized"**

**Correct formatting procedure:**

**Windows:**

1. Insert SD card via card reader
2. Open **File Explorer** → Right-click SD card drive → **Format...**
3. Settings:
   - **File system:** FAT32 (not exFAT or NTFS!)
   - **Allocation unit size:** 4096 bytes (default)
   - **Volume label:** (optional, e.g., "TURBINE_LOG")
   - **Quick Format:** Unchecked (full format preferred for first time)
4. Click **Start**
5. Wait for completion (3-10 minutes for full format)

**If "FAT32" option not available (card > 32GB):**
- Windows won't format >32GB as FAT32 via GUI
- Use tool: **FAT32 Format** (http://ridgecrop.co.uk/index.htm?guiformat.htm)
- Or use Command Prompt:
  ```
  format F: /FS:FAT32 /A:4096
  # Replace F: with your card's drive letter
  ```

**macOS:**

1. Open **Disk Utility** (Applications → Utilities)
2. Select SD card from left sidebar
3. Click **Erase** button (top toolbar)
4. Settings:
   - **Format:** MS-DOS (FAT) ← This is FAT32
   - **Scheme:** Master Boot Record
5. Click **Erase**

**Linux:**

```
# List devices:
lsblk

# Identify SD card (e.g., /dev/sdb1)
# WARNING: Double-check device name! Wrong device = data loss!

# Unmount if mounted:
sudo umount /dev/sdb1

# Format as FAT32:
sudo mkfs.vfat -F 32 -n TURBINE_LOG /dev/sdb1

# Verify:
sudo fsck.vfat /dev/sdb1
```

---

### **Problem: exFAT or NTFS formatted card**

**Symptoms:**
- Card detected by computer but not by ESP32
- Error: "Invalid file system" or mount fails

**Why:** ESP32 SD library only supports FAT16/FAT32, not exFAT or NTFS

**Solution:** Reformat as FAT32 (see above)

**Check current format (Windows):**
```
File Explorer → Right-click SD card → Properties
Look at "File system:" field
```

---

## Write Failures

### **Problem: "Failed to open file for writing"**

**Serial Monitor shows:**
```
Failed to open file for writing
```

**Causes:**

**1. Card write-protected**
- Check physical write-protect switch on SD card
- Switch should be in "unlock" position (away from contacts)

**2. Card full**
```
// Check free space:
uint64_t totalBytes = SD.totalBytes();
uint64_t usedBytes = SD.usedBytes();
uint64_t freeBytes = totalBytes - usedBytes;

Serial.printf("Free space: %llu MB\n", freeBytes / (1024 * 1024));

if (freeBytes < 10 * 1024 * 1024) {  // Less than 10MB
  Serial.println("WARNING: Low disk space!");
}
```

**3. Invalid filename**
```
// FAT32 filename restrictions:
// ❌ BAD:
File file = SD.open("/data/measurements.csv", FILE_WRITE);  // Subdirectory may not exist
File file = SD.open("long_filename_over_8_chars.csv", FILE_WRITE);  // Too long in 8.3 format

// ✅ GOOD:
File file = SD.open("/data.csv", FILE_WRITE);  // Root directory, short name
File file = SD.open("/log_2025.csv", FILE_WRITE);  // Underscore OK
```

**FAT32 filename rules:**
- 8.3 format (8 chars name + 3 chars extension)
- Or long filenames (255 chars) if library supports (ESP32 SD lib does)
- Avoid: `/`, `\`, `:`, `*`, `?`, `"`, `<`, `>`, `|`

**4. File already open**
```
// ❌ BAD: File not closed
void loop() {
  File file = SD.open("/data.csv", FILE_WRITE);
  file.println("data");
  // FILE NOT CLOSED! Eventually runs out of file handles
}

// ✅ GOOD: Always close
void loop() {
  File file = SD.open("/data.csv", FILE_WRITE);
  if (file) {
    file.println("data");
    file.close();  // ALWAYS CLOSE!
  }
}
```

---

### **Problem: Data writes but file is empty**

**Cause:** File not closed or flushed before power loss

**Solution: Proper file handling**

```
// Method 1: Open, write, close (safest but slower)
void logData(String data) {
  File file = SD.open("/data.csv", FILE_APPEND);
  if (file) {
    file.println(data);
    file.close();  // Flushes buffer and closes
  }
}

// Method 2: Keep open, flush periodically (faster)
File dataFile;  // Global

void setup() {
  dataFile = SD.open("/data.csv", FILE_APPEND);
  if (!dataFile) {
    Serial.println("Failed to open file!");
  }
}

void loop() {
  if (dataFile) {
    dataFile.println(millis());
    
    // Flush every 10 writes to ensure data on card
    static int writeCount = 0;
    if (++writeCount >= 10) {
      dataFile.flush();  // Forces write to physical card
      writeCount = 0;
    }
  }
  delay(1000);
}
```

**Trade-off:**
- **Open/close each write:** Safest (data always on card), but slower (~50-100ms per write)
- **Keep open, flush periodically:** Faster, but risk losing unflushed data if power loss

**Recommendation for field deployment:** Open/close each write (1 Hz logging = 1 second between writes, plenty of time)

---

## Data Corruption

### **Problem: CSV file has garbled/corrupted data**

**Symptoms:**
- File opens but contains random characters
- Missing lines
- Truncated mid-line

**Causes:**

**1. Card removed while writing**
- **Always** safely eject/unmount card before removal
- Add LED indicator to show write activity:
  ```
  digitalWrite(LED_PIN, HIGH);  // LED on = writing
  file.println(data);
  file.close();
  digitalWrite(LED_PIN, LOW);   // LED off = safe to remove
  ```

**2. Power loss during write**
- Use battery backup or UPS
- Implement graceful shutdown:
  ```
  void handlePowerLoss() {
    if (dataFile) {
      dataFile.flush();
      dataFile.close();
    }
    SD.end();
  }
  
  // Call before deep sleep or detected power failure
  ```

**3. Fake/low-quality SD card**
- Controller chip faking capacity (reports 32GB but actually 4GB)
- **Test:** Fill card completely and read back data
- Tool: **H2testw** (Windows) or **F3** (Linux/Mac)

**4. SD card wearing out**
- SD cards have limited write cycles (10,000-100,000 typically)
- **Monitor:** Check if write speed decreasing over time
- **Solution:** Rotate to new card periodically (every 6-12 months for continuous logging)

---

### **Recovery: Extract data from corrupted card**

**Step 1: Stop writing immediately**
- Remove card from ESP32
- Write-protect it (flip switch to lock)

**Step 2: Try reading on computer**
```
Windows: Open in Notepad/Excel
macOS: Open in TextEdit/Numbers
Linux: cat filename.csv
```

**If readable:** Copy data immediately to backup

**If unreadable:** Use recovery tools:
- **Windows:** Recuva (free), PhotoRec (free, open-source)
- **macOS:** Disk Drill (free version)
- **Linux:** TestDisk/PhotoRec (free)

---

## Performance Problems

### **Problem: Slow write speed causing data loss**

**Symptom:** 1 Hz logging works, but faster sampling (e.g., 10 Hz) misses samples

**Diagnostic: Measure write time**

```
void testWriteSpeed() {
  File file = SD.open("/test.txt", FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open test file");
    return;
  }
  
  unsigned long startTime = millis();
  
  for (int i = 0; i < 100; i++) {
    file.println("Test data line for speed measurement");
  }
  
  file.close();
  
  unsigned long elapsed = millis() - startTime;
  float avgTime = elapsed / 100.0;
  
  Serial.printf("Average write time: %.2f ms per line\n", avgTime);
  Serial.printf("Max sampling rate: %.1f Hz\n", 1000.0 / avgTime);
}
```

**Typical results:**
- **Good card (Class 10):** 10-30 ms per write → Can handle 10-50 Hz
- **Slow card (Class 4):** 50-100 ms per write → Limit to 5-10 Hz
- **Fake card:** >200 ms per write → Use different card!

**Solutions:**

**1. Buffer writes in RAM:**
```
#define BUFFER_SIZE 10
String buffer[BUFFER_SIZE];
int bufferIndex = 0;

void loop() {
  // Collect data
  String data = String(millis()) + "," + String(sensorRead());
  
  // Add to buffer
  buffer[bufferIndex++] = data;
  
  // Write buffer when full
  if (bufferIndex >= BUFFER_SIZE) {
    File file = SD.open("/data.csv", FILE_APPEND);
    if (file) {
      for (int i = 0; i < BUFFER_SIZE; i++) {
        file.println(buffer[i]);
      }
      file.close();
    }
    bufferIndex = 0;  // Reset buffer
  }
}
```

**2. Use faster SD card:**
- Upgrade to Class 10 or UHS-I
- Look for "U1" or "U3" marking

**3. Reduce CSV line length:**
```
// Instead of:
file.println("2025-12-27T10:30:45+0530,7.35,142.5,287.3,28.4,1012.3,82.1");  // 68 chars

// Use:
file.println("1735283445,7.35,142.5,287.3,28.4,1012.3,82.1");  // 49 chars (Unix timestamp)
```

**4. Pre-allocate file (advanced):**
```
// Create large empty file once:
File file = SD.open("/data.csv", FILE_WRITE);
file.seek(10 * 1024 * 1024);  // 10MB
file.write(0);
file.close();

// Later appends will be faster (no file extension needed)
```

---

## Hardware Issues

### **Problem: SD module not compatible with ESP32**

**Symptoms:**
- Card works on computer but not ESP32
- Works intermittently
- Requires many retry attempts

**Cause:** Some SD modules use 5V logic, ESP32 uses 3.3V logic

**Check your module:**

| Module Type | Logic Level | Works with ESP32? |
|-------------|-------------|-------------------|
| **Standard SD adapter** | 5V | ❌ Needs level shifter |
| **Micro SD module (with 3.3V regulator)** | 3.3V | ✅ Yes |
| **Adafruit MicroSD breakout** | 3.3V/5V auto | ✅ Yes |
| **Catalex TF/Micro SD module** | 3.3V | ✅ Yes |

**Solution if 5V module:**

**Option 1: Buy 3.3V module** ($1-2 on AliExpress)

**Option 2: Add voltage divider** (for 5V→3.3V conversion)

```
ESP32 MISO (GPIO 19) ←─────────── SD Module MISO (direct, no resistor)

                       R1 (1kΩ)
ESP32 MOSI (GPIO 23) ───┬──────── SD Module MOSI
                        │
                      R2 (2kΩ)
                        │
                       GND

Repeat for CS and SCK lines
```

**Formula:** R2 = R1 × (Vin/Vout - 1) = 1kΩ × (5V/3.3V - 1) ≈ 2kΩ

---

### **Problem: Poor connections in card slot**

**Symptoms:**
- Card detection intermittent
- Works when pressed but fails when released
- "Card removed" errors during operation

**Solutions:**

**1. Clean contacts:**
```
Power off → Remove card
Isopropyl alcohol (90%+) on cotton swab
Gently clean card contacts and slot pins
Let dry 5 minutes → Reinsert card
```

**2. Check spring contacts:**
- Some modules have spring-loaded pins
- Bent/damaged springs = poor contact
- Gently straighten with tweezers (careful!)

**3. Add card retention:**
```
If card pops out slightly:
  - Small piece of tape over card edge (not covering contacts!)
  - Or 3D print retention clip
  - Or use module with locking mechanism
```

**4. Solder directly (permanent solution):**
```
For field deployment where card won't be removed:
  - Use microSD card in adapter
  - Solder adapter pins directly to module
  - Hot glue for strain relief
  - No more contact issues!
```

---

## Advanced Diagnostics

### **Low-level SD card info**

```
#include <SD.h>
#include <SPI.h>

void printSDInfo() {
  Serial.println("\n=== SD Card Information ===");
  
  // Card type
  uint8_t cardType = SD.cardType();
  Serial.print("Card Type: ");
  switch(cardType) {
    case CARD_NONE: Serial.println("No card"); break;
    case CARD_MMC:  Serial.println("MMC"); break;
    case CARD_SD:   Serial.println("SD"); break;
    case CARD_SDHC: Serial.println("SDHC"); break;
    default:        Serial.println("UNKNOWN"); break;
  }
  
  // Capacity
  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("Total size: %llu MB\n", cardSize);
  
  // Usage
  uint64_t totalBytes = SD.totalBytes();
  uint64_t usedBytes = SD.usedBytes();
  Serial.printf("Total space: %llu MB\n", totalBytes / (1024 * 1024));
  Serial.printf("Used space: %llu MB\n", usedBytes / (1024 * 1024));
  Serial.printf("Free space: %llu MB\n", (totalBytes - usedBytes) / (1024 * 1024));
  
  // FAT type (not directly available, infer from size)
  if (totalBytes < 4ULL * 1024 * 1024 * 1024) {
    Serial.println("Likely FAT32 (< 4GB)");
  } else {
    Serial.println("Likely exFAT or NTFS (> 4GB) - May not be compatible!");
  }
  
  Serial.println("===========================\n");
}
```

---

## Preventive Maintenance

### **Best Practices for Long-term Reliability**

**1. Use quality cards:**
- Name brands: SanDisk, Samsung, Kingston
- Avoid eBay/AliExpress no-name cards
- Check reviews for "industrial" or "endurance" SD cards

**2. Format periodically:**
- Every 3-6 months during long campaigns
- Backup data first!
- Reduces fragmentation

**3. Monitor health:**
```
// Log write errors:
File file = SD.open("/data.csv", FILE_APPEND);
if (!file) {
  errorCount++;
  if (errorCount > 10) {
    Serial.println("SD CARD FAILING - REPLACE SOON!");
  }
}
```

**4. Backup strategy:**
- Weekly: Download data via WiFi or remove card
- Keep previous card as backup
- Offsite backup (cloud storage)

**5. Environmental protection:**
- Conformal coating on SD module PCB (humidity protection)
- Desiccant pack in enclosure
- Avoid temperature extremes (SD cards rated 0-70°C typically)

---

## Quick Reference: Error Messages

| Error Message | Most Likely Cause | First Solution to Try |
|---------------|-------------------|------------------------|
| "Card Mount Failed" | Not formatted FAT32 | Format as FAT32 on computer |
| "No SD card attached" | Card not inserted / bad contact | Reinsert card firmly |
| "Failed to open file" | Filename too long or invalid | Use 8.3 filename format |
| "Card removed" | Loose connection | Check wiring, clean contacts |
| Random data corruption | Fake/bad card | Replace with quality card |
| Slow write speed | Class 4 or slower card | Upgrade to Class 10 |

---

## Still Having Issues?

**Before asking for help, gather:**

1. **Serial Monitor output** (complete, from startup)
2. **SD card info:**
   - Brand, capacity, speed class
   - How formatted (tool used, settings)
3. **Hardware photos:**
   - Wiring connections (clear, well-lit)
   - SD module type
4. **Code snippet** (minimal example that reproduces issue)
5. **Multimeter readings:**
   - VCC on SD module
   - Continuity tests on all connections

**Post to:**
- Repository: https://github.com/asithakal/wind-turbine-daq-guide/issues
- Arduino Forum: https://forum.arduino.cc/c/using-arduino/storage/
- ESP32 Forum: https://esp32.com/

---

**Last updated:** December 2024  
**Tested with:** ESP32-WROOM-32, various SD modules, 1GB-32GB cards