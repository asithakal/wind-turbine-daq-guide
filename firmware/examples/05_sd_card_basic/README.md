# Example 05: SD Card Logging

## Purpose

Store sensor data on SD card in CSV format for long-term data collection campaigns.

## Learning Objectives

- Initialize SD card using SPI protocol
- Create and manipulate files
- Write CSV-formatted data
- Implement periodic buffer flushing for data safety
- Handle SD card errors (removed, full, corrupted)
- Check available storage space

## Hardware Required

- ESP32 board
- **MicroSD card module** (SPI interface, not SDIO)
- **MicroSD card:** 
  - Size: 1-32 GB (32 GB max for FAT32)
  - Speed: Class 10 recommended
  - Format: **FAT32** (NOT exFAT or NTFS)
- Breadboard and jumper wires

## SD Card Module Types

### **⚠️ Important: Use SPI Module, Not SDIO!**

| Type | Voltage | Pins | Compatible? |
|------|---------|------|-------------|
| **SPI Module** | 3.3V or 5V | 6 pins: VCC, GND, MISO, MOSI, SCK, CS | ✅ YES |
| **SDIO Module** | 3.3V | 9+ pins: includes CMD, D0-D3 | ❌ NO (different protocol) |

**This example uses SPI protocol** - most common for Arduino/ESP32.

## Circuit Diagram

```
MicroSD Card Module (SPI)         ESP32

VCC ──────────────────────────→ 3.3V (NOT 5V unless module has regulator)
GND ──────────────────────────→ GND
MISO ─────────────────────────→ GPIO 19
MOSI ─────────────────────────→ GPIO 23
SCK ──────────────────────────→ GPIO 18
CS ───────────────────────────→ GPIO 5
```

## Connections

| SD Module Pin | ESP32 Pin | SPI Function |
|---------------|-----------|--------------|
| VCC | 3.3V | Power (check module - some accept 5V) |
| GND | GND | Ground |
| MISO | GPIO 19 | Master In Slave Out (data from SD) |
| MOSI | GPIO 23 | Master Out Slave In (data to SD) |
| SCK | GPIO 18 | Serial Clock |
| CS | GPIO 5 | Chip Select (enable SD card) |

### **⚠️ Voltage Warning:**

Most SD cards are **3.3V devices**. Check your module:
- **3.3V module:** Connect to ESP32 3.3V pin
- **5V module with level shifters:** Can use 5V/VIN pin
- **Unsure?** Use 3.3V to be safe

## SD Card Preparation

### **Step 1: Format SD Card**

**Windows:**
1. Insert SD card into PC
2. Right-click drive → **Format**
3. **File System:** FAT32
4. **Allocation Unit Size:** Default
5. Click **Start**

**macOS:**
1. Open **Disk Utility**
2. Select SD card
3. Click **Erase**
4. **Format:** MS-DOS (FAT)
5. Click **Erase**

**Linux:**
```
# Find SD card device (e.g., /dev/sdb1)
lsblk

# Format as FAT32
sudo mkfs.vfat -F 32 /dev/sdb1

# Unmount
sudo umount /dev/sdb1
```

### **Step 2: Verify Format**

SD card should show:
- **File System:** FAT32 (or FAT for small cards < 2 GB)
- **Empty:** No files
- **Working:** PC can read/write to it

**⚠️ exFAT or NTFS won't work!** ESP32 SD library only supports FAT16/FAT32.

## Expected Output

**Serial Monitor (115200 baud):**

```
=================================
SD Card Logging - Example 05
=================================

Initializing SD card... SUCCESS
SD Card Size: 16384 MB
Used Space: 12 MB
Free Space: 16372 MB

Creating new file: /turbine_data.csv
CSV header written

Logging started. Data is written to SD card.
Remove SD card ONLY after seeing 'Flushed' message.

Time(s) Wind(m/s)       Power(W)        RPM     Status
------------------------------------------------------------
1.0     5.45            124.8           244.5   Buffered
2.0     5.63            127.2           245.8   Buffered
3.0     5.52            123.9           244.2   Buffered
...
10.0    5.58            126.1           245.3   Flushed (10 records)
11.0    5.49            125.5           244.9   Buffered
...
```

## Generated CSV File

**File:** `/turbine_data.csv` on SD card

**Contents:**
```
timestamp_ms,wind_speed_ms,power_w,rpm
1000,5.45,124.8,244.5
2000,5.63,127.2,245.8
3000,5.52,123.9,244.2
4000,5.57,126.3,245.1
...
```

**Open in Excel:**
- Column A: Timestamp (milliseconds)
- Column B: Wind Speed (m/s)
- Column C: Power (W)
- Column D: RPM

## Troubleshooting

### **Problem: "Initializing SD card... FAILED!"**

**Cause 1: Wiring incorrect**

Double-check connections (especially CS pin):
```
// Add diagnostic:
Serial.print("CS Pin: ");
Serial.println(SD_CS_PIN);
pinMode(SD_CS_PIN, OUTPUT);
digitalWrite(SD_CS_PIN, HIGH);  // Should be HIGH when idle
```

**Cause 2: SD card not inserted**

- Remove and reinsert card
- Check card clicks into place

**Cause 3: SD card not formatted**

- Format as FAT32 (see preparation steps above)

**Cause 4: SD card > 32 GB**

- FAT32 officially supports up to 32 GB
- Use smaller card or format as FAT32 anyway (may work)

**Cause 5: Faulty SD card**

- Try different SD card
- Test card in PC first

---

### **Problem: "ERROR: Could not create file!"**

**Cause 1: SD card full**

Check available space:
```
Serial.print("Free Space: ");
Serial.println(SD.totalBytes() - SD.usedBytes());
```

**Solution:** Delete old files or use larger card

**Cause 2: SD card write-protected**

- Check physical write-protect tab on SD card adapter
- Slide tab to "unlocked" position

**Cause 3: File system corrupted**

- Reformat SD card
- Run `chkdsk` (Windows) or `fsck` (Linux) to repair

---

### **Problem: Data stops logging after some time**

**Cause: SD card removed or disconnected**

**Solution:** Check wiring, ensure stable connections

**Diagnostic:**
```
// Add to loop():
if (!SD.begin(SD_CS_PIN)) {
  Serial.println("SD card disconnected!");
}
```

---

### **Problem: Only partial data saved (missing last entries)**

**Cause: Power lost before flush**

**Why?** Data is buffered in RAM, only written to SD periodically.

**Solution:** Wait for "Flushed" message before removing power.

**Or:** Reduce flush interval:
```
const int FLUSH_INTERVAL_MS = 5000;  // Flush every 5 seconds
```

**Trade-off:** More frequent flushes = more SD wear, slower performance.

---

### **Problem: SD card wears out quickly**

**Cause:** Writing to same sectors repeatedly

**SD card lifespan:**
- Typical: 10,000-100,000 write cycles per sector
- At 1 Hz logging: ~115 days to 3 years (varies by card quality)

**Solutions:**
1. **Use high-endurance SD cards** (rated for video recording)
2. **Batch writes** (buffer in RAM, write every 10 seconds)
3. **Rotate files daily:**
   ```
   String filename = "/data_" + String(day()) + ".csv";
   ```

## Understanding Buffering

### **Why Flush?**

```
RAM Buffer:  [data1, data2, data3, ... data100]
                              ↓ flush()
SD Card:     [data1, data2, data3, ... data100] ← Physically written
```

**Without flush:**
- Data stays in RAM buffer
- If power lost → data lost

**With periodic flush:**
- Data written to SD every 10 seconds
- Max 10 seconds of data at risk

### **Flush Strategies:**

| Strategy | Flush Frequency | Data Risk | SD Wear | Performance |
|----------|-----------------|-----------|---------|-------------|
| **Every write** | 1 Hz | Minimal | High | Slow |
| **Every 10 sec** | 0.1 Hz | 10 sec | Medium | Good |
| **Every 1 min** | 0.017 Hz | 60 sec | Low | Fast |

**Recommendation for turbines:** 10-30 seconds (balance safety and SD life).

## File Operations Reference

### **Opening files:**

```
File file = SD.open("/filename.txt", FILE_READ);   // Read only
File file = SD.open("/filename.txt", FILE_WRITE);  // Write (creates if not exists)
File file = SD.open("/filename.txt", FILE_APPEND); // Append (adds to end)
```

### **Writing data:**

```
file.print("text");         // No newline
file.println("text");       // With newline
file.write(byteArray, len); // Binary data
```

### **Reading data:**

```
String line = file.readStringUntil('\n'); // Read line
int available = file.available();         // Bytes remaining
byte b = file.read();                     // Read single byte
```

### **File management:**

```
SD.exists("/file.txt");      // Check if file exists
SD.remove("/file.txt");      // Delete file
SD.mkdir("/folder");         // Create directory
SD.rmdir("/folder");         // Delete directory (must be empty)
```

### **Always close files:**

```
file.close();  // CRITICAL - releases SD card for other operations
```

## CSV Format Best Practices

### **Good CSV:**

```
timestamp_ms,wind_speed_ms,power_w,rpm
1000,5.45,124.8,244.5
2000,5.63,127.2,245.8
```

✅ Header row with column names  
✅ Comma-separated (no spaces)  
✅ Consistent number of columns  
✅ No quotes unless necessary

### **Bad CSV:**

```
Time, Wind Speed (m/s), Power
1000, "5.45", 124.8, 244.5, extra_column
2000, , 127.2, 245.8
```

❌ Spaces after commas (OK but inconsistent)  
❌ Inconsistent columns  
❌ Missing values (should be `NaN` or `-999`)

## Storage Capacity Calculation

### **Example: 6-month campaign**

**Parameters:**
- Sampling rate: 1 Hz
- Bytes per record: ~50 bytes (CSV line)
- Duration: 180 days

**Calculation:**
```
Records per day = 1 Hz × 86,400 sec = 86,400 records
Total records = 86,400 × 180 days = 15,552,000 records
Storage needed = 15,552,000 × 50 bytes = 777.6 MB
```

**Conclusion:** 2 GB SD card is sufficient (with safety margin).

### **Rule of Thumb:**

| Campaign Duration | Sampling Rate | Recommended SD Card |
|-------------------|---------------|---------------------|
| 1 week | 1 Hz | 512 MB |
| 1 month | 1 Hz | 2 GB |
| 6 months | 1 Hz | 4 GB |
| 1 year | 1 Hz | 8 GB |

## Code Explanation

### **SPI Initialization:**

```
SPI.begin(SD_SCK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
```

Configures ESP32 SPI pins (default is VSPI bus).

### **SD Card Initialization:**

```
if (!SD.begin(SD_CS_PIN)) {
  // Initialization failed
}
```

Mounts SD card file system. Returns `false` if card not found or unreadable.

### **Appending data:**

```
File dataFile = SD.open(DATA_FILENAME, FILE_APPEND);
dataFile.println(data);  // Adds to end of file
dataFile.close();
```

**FILE_APPEND** opens file without erasing existing content.

### **Flushing buffer:**

```
dataFile.flush();  // Force buffered data to SD card
```

Ensures data is physically written (not just in RAM).

## Next Steps

✅ **Example 05 Complete!**

Continue to:
- [Example 06: NTP Time Sync](../06_ntp_time_sync/) - Add real-time timestamps

## Further Reading

- **SD Library:** https://www.arduino.cc/reference/en/libraries/sd/
- **SPI Protocol:** https://en.wikipedia.org/wiki/Serial_Peripheral_Interface
- **FAT File System:** https://en.wikipedia.org/wiki/File_Allocation_Table