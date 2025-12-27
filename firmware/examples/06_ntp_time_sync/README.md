# Example 06: NTP Time Synchronization

## Purpose

Get accurate real-time timestamps from internet time servers for data logging.

## Learning Objectives

- Connect ESP32 to WiFi network
- Use NTP (Network Time Protocol) to sync time
- Configure timezone and daylight saving rules
- Format timestamps in multiple formats (ISO 8601, Unix, human-readable)
- Handle network failures and time sync errors
- Implement periodic time resynchronization

## Hardware Required

- ESP32 board
- WiFi router with internet access

## Connections

**None** - uses built-in WiFi radio

## WiFi Configuration

**⚠️ IMPORTANT: Update credentials in code!**

```
const char* WIFI_SSID = "YourWiFiNetwork";      // Change this
const char* WIFI_PASSWORD = "YourPassword";     // Change this
```

## Timezone Configuration

**Default: Sri Lanka (UTC+5:30)**

```
const char* TIMEZONE = "IST-5:30";
```

### **Common Timezones:**

| Location | Timezone String | UTC Offset |
|----------|-----------------|------------|
| **Sri Lanka** | `"IST-5:30"` | +5:30 |
| **India** | `"IST-5:30"` | +5:30 |
| **USA East Coast** | `"EST5EDT,M3.2.0,M11.1.0"` | -5/-4 (DST) |
| **UK** | `"GMT0BST,M3.5.0/1,M10.5.0"` | 0/+1 (DST) |
| **Japan** | `"JST-9"` | +9 |
| **Australia (Sydney)** | `"AEST-10AEDT,M10.1.0,M4.1.0/3"` | +10/+11 (DST) |
| **Germany** | `"CET-1CEST,M3.5.0,M10.5.0/3"` | +1/+2 (DST) |

**Full list:** https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv

### **Timezone Format Explained:**

```
"EST5EDT,M3.2.0,M11.1.0"
 │  │ │  │      │
 │  │ │  │      └─ DST end: November, 1st Sunday, 2am
 │  │ │  └──────── DST start: March, 2nd Sunday, 2am
 │  │ └─────────── Daylight name (EDT)
 │  └───────────── Standard offset from UTC (5 hours behind)
 └──────────────── Standard name (EST)
```

## Expected Output

**Serial Monitor (115200 baud):**

```
=================================
NTP Time Sync - Example 06
=================================

Connecting to WiFi: YourNetwork
..........
WiFi connected!
IP Address: 192.168.1.15
Signal strength: -45 dBm

Synchronizing time with NTP servers... .....
Time synchronized: 2025-12-27 15:30:45 IST

Current Time (updates every second):
---------------------------------------
Human:     2025-12-27 15:30:45 IST
ISO 8601:  2025-12-27T15:30:45+0530
Unix:      1735296045
Millis:    12345
---------------------------------------
Human:     2025-12-27 15:30:46 IST
ISO 8601:  2025-12-27T15:30:46+0530
Unix:      1735296046
Millis:    13345
---------------------------------------
```

## Testing Without Internet

### **Simulate Time Server:**

If internet is unavailable, use local NTP server:

```
const char* NTP_SERVER1 = "192.168.1.100";  // Your local NTP server
```

**Setup local NTP server (Linux):**

```
sudo apt install ntp
sudo systemctl start ntp
```

## Troubleshooting

### **Problem: "WiFi connection FAILED!"**

**Cause 1: Wrong credentials**

```
// Double-check spelling and case:
const char* WIFI_SSID = "MyNetwork";      // Exact name
const char* WIFI_PASSWORD = "Pass123!";   // Case-sensitive
```

**Cause 2: WiFi not in range**

Check signal strength:
- Excellent: -30 to -50 dBm
- Good: -50 to -60 dBm
- Fair: -60 to -70 dBm
- Poor: -70 to -80 dBm (may not connect)

**Solution:** Move ESP32 closer to router

**Cause 3: 5 GHz network**

ESP32 **only supports 2.4 GHz WiFi** (not 5 GHz).

**Solution:** Connect to 2.4 GHz network or enable router's 2.4 GHz band

---

### **Problem: "Time sync FAILED!"**

**Cause 1: No internet connection**

Test connectivity:
```
// Add after WiFi.begin():
if (WiFi.status() == WL_CONNECTED) {
  Serial.print("Gateway: ");
  Serial.println(WiFi.gatewayIP());
}
```

Ping test from PC: `ping pool.ntp.org`

**Cause 2: Firewall blocking NTP (port 123)**

- Check router firewall settings
- Try alternative NTP servers
- Use local NTP server on LAN

**Cause 3: DNS resolution failure**

```
// Try IP address instead of hostname:
const char* NTP_SERVER1 = "216.239.35.0";  // time.google.com IP
```

---

### **Problem: Time is off by several hours**

**Cause: Wrong timezone setting**

Verify your UTC offset:
- Find your timezone: https://www.timeanddate.com/time/map/
- Update `TIMEZONE` constant

**Example for USA Pacific:**
```
const char* TIMEZONE = "PST8PDT,M3.2.0,M11.1.0";  // UTC-8/-7
```

---

### **Problem: Time drifts over days**

**Cause:** ESP32 internal clock drifts ~1 second per hour

**Solution:** Enable periodic resync (already in code):
```
const int SYNC_INTERVAL_MS = 3600000;  // Resync every hour
```

For critical timing: Resync every 15-30 minutes.

---

### **Problem: "WiFi connected" but no time sync after restart**

**Cause:** Router assigns same IP but internet not ready yet

**Solution:** Add delay before time sync:
```
if (WiFi.status() == WL_CONNECTED) {
  delay(2000);  // Wait for internet to stabilize
  initializeTime();
}
```

## Timestamp Formats Comparison

### **1. ISO 8601 (Recommended for CSV)**

```
2025-12-27T15:30:45+0530
```

**Advantages:**
- International standard
- Sortable alphabetically
- Includes timezone offset
- Parseable by Python pandas, Excel

**Use for:** CSV logging, data publication

---

### **2. Human-Readable**

```
2025-12-27 15:30:45 IST
```

**Advantages:**
- Easy to read
- Good for debugging

**Use for:** Serial Monitor, log files

---

### **3. Unix Timestamp**

```
1735296045
```

**Advantages:**
- Compact (integer)
- Timezone-independent (always UTC)
- Easy math (duration = end - start)

**Disadvantages:**
- Not human-readable
- Limited to 2038 (32-bit) or 2106 (ESP32 uses 64-bit)

**Use for:** Database storage, calculations

---

### **4. Milliseconds Since Boot**

```
12345
```

**Advantages:**
- No network needed
- Precise to 1ms
- Good for intervals

**Disadvantages:**
- Resets on reboot
- No absolute time

**Use for:** Local timing, debugging

## NTP Protocol Overview

### **How NTP Works:**

```
1. ESP32 sends request to NTP server (UDP port 123)
2. Server responds with current UTC time
3. ESP32 calculates clock offset accounting for network delay
4. ESP32 adjusts internal clock
```

**Typical accuracy:** ±10-50 ms over internet, ±1 ms on LAN

### **Why Multiple Servers?**

```
configTime(0, 0, NTP_SERVER1, NTP_SERVER2, NTP_SERVER3);
```

**Redundancy:** If one server is down, tries next server.

**pool.ntp.org:** Rotates through hundreds of servers worldwide.

## Code Explanation

### **Time Structures:**

```
time_t now;              // Unix timestamp (seconds since 1970)
struct tm timeinfo;      // Broken-down time (year, month, day, hour, etc.)

time(&now);              // Get current time as Unix timestamp
localtime_r(&now, &timeinfo);  // Convert to local timezone
```

**`struct tm` fields:**
```
timeinfo.tm_year   // Years since 1900 (2025 → 125)
timeinfo.tm_mon    // Month (0-11, January = 0)
timeinfo.tm_mday   // Day of month (1-31)
timeinfo.tm_hour   // Hour (0-23)
timeinfo.tm_min    // Minute (0-59)
timeinfo.tm_sec    // Second (0-59)
timeinfo.tm_wday   // Day of week (0-6, Sunday = 0)
```

### **Formatting Timestamps:**

```
strftime(buffer, size, format, &timeinfo);
```

**Common format codes:**
- `%Y` - Year (2025)
- `%m` - Month (01-12)
- `%d` - Day (01-31)
- `%H` - Hour 24h (00-23)
- `%M` - Minute (00-59)
- `%S` - Second (00-59)
- `%z` - Timezone offset (+0530)
- `%Z` - Timezone name (IST)

**Full list:** https://cplusplus.com/reference/ctime/strftime/

### **Timezone Configuration:**

```
setenv("TZ", TIMEZONE, 1);  // Set TZ environment variable
tzset();                     // Apply timezone
```

Without this, time would be in UTC (not local time).

## Power Consumption Considerations

**WiFi active:** ~160-260 mA  
**WiFi sleep:** ~20 mA

For battery-powered systems:

```
// Sync time once, then disable WiFi
initializeTime();
WiFi.disconnect(true);
WiFi.mode(WIFI_OFF);

// Time continues to tick (drifts slowly)
// Reconnect periodically to resync
```

## Integration with SD Card Logging

**Combine with Example 05:**

```
// Get ISO timestamp
String timestamp = getISO8601Timestamp();

// Write to SD card
dataFile.print(timestamp);
dataFile.print(",");
dataFile.print(windSpeed, 2);
dataFile.println();
```

**CSV output:**
```
timestamp,wind_speed_ms,power_w,rpm
2025-12-27T15:30:45+0530,5.45,124.8,244.5
2025-12-27T15:30:46+0530,5.63,127.2,245.8
```

## Security Considerations

### **WiFi Credentials in Code:**

**⚠️ Risk:** Hardcoded passwords visible in source code

**Better approach:**

1. **Store in separate file** (not uploaded to GitHub):
   ```
   #include "credentials.h"  // Contains WIFI_SSID and WIFI_PASSWORD
   ```

2. **Use WiFiManager library** (prompts for credentials on first boot):
   ```
   WiFiManager wifiManager;
   wifiManager.autoConnect("TurbineDAQ");
   ```

3. **SD card config file:**
   ```
   // Read wifi.txt from SD card
   File configFile = SD.open("/wifi.txt");
   String ssid = configFile.readStringUntil('\n');
   String password = configFile.readStringUntil('\n');
   ```

## Next Steps

✅ **Example 06 Complete!**

Continue to:
- [Example 07: MQTT Transmission](../07_mqtt_publish/) - Send data to cloud

## Further Reading

- **NTP Protocol:** https://en.wikipedia.org/wiki/Network_Time_Protocol
- **POSIX Timezone:** https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
- **ISO 8601:** https://en.wikipedia.org/wiki/ISO_8601