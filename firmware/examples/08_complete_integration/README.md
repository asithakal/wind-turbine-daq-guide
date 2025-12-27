# Example 08: Complete System Integration

## Purpose

Full production-ready wind turbine data acquisition system combining all previous examples into a robust, field-deployable solution.

## Features

✅ **Multi-sensor data collection:**
- Wind speed (analog anemometer)
- Rotor RPM (Hall sensor with interrupts)
- Electrical power (INA226 current/voltage)
- Environmental conditions (BME280 temp/humidity/pressure)

✅ **Data logging:**
- SD card CSV logging with timestamps
- Automatic file creation with headers
- Periodic buffer flushing for data safety

✅ **Cloud connectivity (optional):**
- WiFi connection with auto-reconnect
- NTP time synchronization
- MQTT cloud transmission
- Works offline if network unavailable

✅ **Calculated parameters:**
- Air density correction
- Tip speed ratio (λ)
- Power coefficient (Cp)

✅ **Robustness features:**
- Error handling and recovery
- Watchdog protection
- Status LED indicators
- Graceful degradation (continues with available sensors)

## Hardware Required

### **Essential Components:**
- ESP32 development board
- Anemometer (0-5V analog output)
- Hall effect sensor (SS441A or similar)
- INA226 or INA219 power monitor
- BME280 environmental sensor
- MicroSD card module
- MicroSD card (8-32 GB, FAT32)
- Neodymium magnet (for Hall sensor)
- 10kΩ resistors (2× for anemometer voltage divider)
- Breadboard or custom PCB
- Enclosure (IP67 rated for outdoor use)

### **Optional:**
- WiFi router (for cloud features)
- External antenna (for better WiFi range)
- Real-time clock module (if no WiFi/NTP)

## Wiring Diagram

```
                               ESP32
                         ┌─────────────┐
Anemometer ──(divider)──→│ GPIO 34     │
Hall Sensor ────────────→│ GPIO 15     │
                         │             │
INA226 SDA ─────────────→│ GPIO 21     │
INA226 SCL ─────────────→│ GPIO 22     │
BME280 SDA ─────────────→│ GPIO 21     │
BME280 SCL ─────────────→│ GPIO 22     │
                         │             │
SD Card CS ─────────────→│ GPIO 5      │
SD Card SCK ────────────→│ GPIO 18     │
SD Card MOSI ───────────→│ GPIO 23     │
SD Card MISO ───────────→│ GPIO 19     │
                         │             │
Status LED ──────────────│ GPIO 2      │
                         └─────────────┘
```

**Full schematic:** See `hardware/schematics/system_diagram.png`

## Installation

### **Step 1: Hardware Assembly**

1. Follow wiring diagram above
2. Use voltage divider for anemometer (10kΩ + 10kΩ)
3. Mount magnet on rotor shaft (1-3mm from Hall sensor)
4. Install all components in weatherproof enclosure
5. Connect turbine power output to INA226 V+/V- terminals

### **Step 2: Configuration**

Edit configuration section in code (lines 30-60):

```
// WiFi (if using cloud features)
const char* WIFI_SSID = "YourNetwork";
const char* WIFI_PASSWORD = "YourPassword";
const bool ENABLE_WIFI = true;  // Set false for offline

// Turbine dimensions (CRITICAL - change for your turbine!)
const float ROTOR_RADIUS_M = 0.60;     // Your rotor radius in meters
const float ROTOR_HEIGHT_M = 1.50;     // Your rotor height (VAWT)
const float SWEPT_AREA_M2 = 1.80;      // 2*R*H for VAWT, π*R² for HAWT

// Sensor calibration
const float ANEM_OFFSET_V = 0.4;       // Your anemometer offset
const float ANEM_SCALE_V_PER_MS = 0.2; // Your anemometer scale

// Timezone
const char* TIMEZONE = "IST-5:30";     // Your timezone
```

### **Step 3: Upload Firmware**

1. Open `08_complete_integration.ino` in Arduino IDE
2. Install all required libraries (see Dependencies below)
3. Select **ESP32 Dev Module** board
4. Select correct COM port
5. Click **Upload**

### **Step 4: Verification**

1. Open Serial Monitor (115200 baud)
2. Check system status table
3. Verify all sensors show "OK"
4. Spin rotor manually, confirm RPM reading
5. Check SD card for `turbine_data.csv` file

## Dependencies

Install via Library Manager:

- Adafruit INA219 (≥1.2.0)
- Adafruit BME280 (≥2.2.2)
- PubSubClient (≥2.8.0)
- ArduinoJson (≥6.21.0, **not v7!**)

Built-in libraries (no installation):
- Wire
- SPI
- SD
- WiFi

## Configuration Options

### **Operating Modes:**

| Mode | WiFi | MQTT | Use Case |
|------|------|------|----------|
| **Offline** | OFF | OFF | Remote sites, battery power |
| **Local WiFi** | ON | OFF | Lab testing, local network only |
| **Full Cloud** | ON | ON | Real-time monitoring, data sharing |

**Set in code:**
```
const bool ENABLE_WIFI = true;   // false = offline mode
const bool ENABLE_MQTT = true;   // false = SD only
```

### **Sampling Rates:**

```
const int SAMPLE_INTERVAL_MS = 1000;        // 1 Hz (recommended)
const int SD_FLUSH_INTERVAL_MS = 10000;     // Flush every 10 sec
const int MQTT_PUBLISH_INTERVAL_MS = 5000;  // Cloud update every 5 sec
```

**Recommendations:**
- **1 Hz** (1000 ms): Standard for wind turbines
- **0.1 Hz** (10000 ms): Battery-powered systems
- **10 Hz** (100 ms): High-resolution research (large SD card needed)

## Expected Output

### **Serial Monitor (Startup):**

```
================================================================================
        Wind Turbine Data Acquisition System - Complete Integration
================================================================================
Version: 1.0.0
Author: Wind Turbine DAQ Guide
Build Date: Dec 27 2025 15:30:00
================================================================================

Initializing I2C bus... OK
  INA219 power monitor... OK
  BME280 environmental... OK
Initializing anemometer (GPIO 34)... OK
Initializing Hall sensor (GPIO 15)... OK
Initializing SD card... OK (16384 MB total, 16300 MB free)
Connecting to WiFi: YourNetwork... OK (192.168.1.15)
Synchronizing time... OK
Connecting to MQTT broker: broker.hivemq.com
MQTT connected!
Creating data file: /turbine_data.csv

=== System Status ===
INA219 Power Monitor:     OK
BME280 Environmental:     OK
SD Card:                  OK
WiFi:                     Connected
Time Sync:                OK
MQTT:                     Connected

=== Turbine Configuration ===
Rotor Radius:             0.60 m
Rotor Height:             1.50 m
Swept Area:               1.80 m²
Pulses per Revolution:    1
====================

=== Data Acquisition Started ===

Time            Wind    RPM     Power   Temp    Humid   Press   λ       Cp      Status
================================================================================
15:30:01        5.45    245.2   124.8   28.3    75      1012.4  2.15    0.358   SD MQTT
15:30:02        5.63    252.1   132.5   28.3    75      1012.5  2.14    0.362   SD MQTT
15:30:03        5.52    248.7   128.1   28.4    76      1012.4  2.16    0.360   SD MQTT
```

### **CSV File (`/turbine_data.csv`):**

```
timestamp,timestamp_ms,wind_speed_ms,rotor_rpm,voltage_v,current_ma,power_w,temperature_c,humidity_pct,pressure_hpa,air_density_kgm3,lambda,cp
2025-12-27T15:30:01+0530,1000,5.450,245.2,12.45,10.02,124.8,28.3,75.0,1012.4,1.1824,2.150,0.3580
2025-12-27T15:30:02+0530,2000,5.630,252.1,12.52,10.58,132.5,28.3,75.0,1012.5,1.1824,2.140,0.3620
2025-12-27T15:30:03+0530,3000,5.520,248.7,12.48,10.27,128.1,28.4,76.0,1012.4,1.1822,2.160,0.3600
```

## Troubleshooting

### **Problem: System hangs during startup**

**Symptom:** Serial output stops at specific sensor initialization

**Solutions:**
1. **Check wiring** of the sensor that failed to initialize
2. **Disable sensor** if not available:
   ```
   // Comment out sensor in code
   // if (ina219.begin()) { ... }
   ```
3. **Check I2C addresses** with scanner (Example 03)

---

### **Problem: SD card errors after hours/days**

**Symptom:** "SD card write failed repeatedly!"

**Causes:**
- SD card full
- Card removed/loose connection
- Card worn out

**Solutions:**
1. Check free space (will auto-print on startup)
2. Secure SD card connection
3. Replace SD card (use high-endurance type)

---

### **Problem: Time shows incorrect timezone**

**Check timezone string:**
```
const char* TIMEZONE = "IST-5:30";  // Verify this matches your location
```

**Verify with:**
- Current time in Serial Monitor
- Compare with actual time in your location

---

### **Problem: Cp values always > 0.6 (impossible)**

**Cause:** Wrong swept area calculation

**For VAWT:**
```
const float SWEPT_AREA_M2 = 2 * ROTOR_RADIUS_M * ROTOR_HEIGHT_M;
```

**For HAWT:**
```
const float SWEPT_AREA_M2 = PI * pow(ROTOR_RADIUS_M, 2);
```

---

### **Problem: RPM always 0**

**Check:**
1. Magnet close enough to Hall sensor (< 5mm)
2. Hall sensor wired correctly (VCC, GND, OUT)
3. Interrupt pin is GPIO 15 (or change in code)
4. Rotor actually spinning

**Debug:**
```
// Add to loop():
Serial.print("Raw pulse count: ");
Serial.println(pulseCount);
```

---

### **Problem: Wind speed always 0**

**Check:**
1. Anemometer powered (VCC connection)
2. Voltage divider correct (10kΩ + 10kΩ)
3. Calibration constants match your sensor

**Debug:**
```
// Add after analogRead():
Serial.print("Raw ADC: ");
Serial.println(analogRead(PIN_ANEMOMETER));
// Should be 0-4095 range
```

## Field Deployment Checklist

**Before installation:**

- [ ] Test all sensors indoors for 24 hours
- [ ] Verify SD card has sufficient space
- [ ] Weatherproof all connections
- [ ] Apply conformal coating to PCB
- [ ] Install desiccant packs in enclosure
- [ ] Label all cables
- [ ] Document configuration (take photos)

**During installation:**

- [ ] Mount sensors at correct heights
- [ ] Align magnet with Hall sensor
- [ ] Secure all cables (prevent wind damage)
- [ ] Connect power last (avoid shorts)
- [ ] Verify data logging starts
- [ ] Record GPS coordinates and site photos

**Post-installation:**

- [ ] Check data quality after 1 hour
- [ ] Return after 1 week to verify operation
- [ ] Download data monthly (or more frequent)
- [ ] Inspect enclosure for water ingress
- [ ] Clean sensors (dust, bird droppings)

## Data Analysis

**Download data:**
1. Power off ESP32
2. Remove SD card
3. Copy `turbine_data.csv` to PC
4. Reinsert card and power on

**Analyze with Python:**
```
import pandas as pd
import matplotlib.pyplot as plt

# Load data
df = pd.read_csv('turbine_data.csv', parse_dates=['timestamp'])

# Plot Cp vs lambda
plt.scatter(df['lambda'], df['cp'], alpha=0.5)
plt.xlabel('Tip Speed Ratio (λ)')
plt.ylabel('Power Coefficient (Cp)')
plt.title('Turbine Performance Curve')
plt.grid(True)
plt.show()
```

**Or use analysis scripts:**
- See `analysis/python_scripts/03_cp_lambda_analysis.py`

## Performance Monitoring

**System should achieve:**
- **Data loss:** < 0.1% (< 86 samples/day at 1 Hz)
- **Timestamp accuracy:** ± 1 second (with NTP)
- **SD card lifespan:** > 1 year continuous logging
- **Power consumption:** ~200 mA @ 5V (WiFi on), ~80 mA (WiFi off)

**Monitor via Serial or MQTT:**
- Check for "FAILED" messages
- Verify SD flush messages every 10 seconds
- Monitor MQTT publish success rate

## Maintenance

**Monthly:**
- Download and backup data from SD card
- Check SD card free space
- Inspect enclosure for damage
- Clean sensor surfaces

**Quarterly:**
- Recalibrate anemometer (compare to reference)
- Check Hall sensor alignment
- Verify power sensor readings (compare to multimeter)
- Replace desiccant packs

**Annually:**
- Replace SD card
- Check all cable connections
- Update firmware if improvements available

## Power Options

### **Grid Power (Recommended):**
```
5V/2A USB adapter → ESP32 VIN
```

### **Battery (for remote sites):**
```
12V Lead-acid battery → Buck converter (12V→5V) → ESP32
Solar panel (10W+) → Charge controller → Battery
```

**Estimated battery life (12V 7Ah):**
- WiFi on: ~30 hours
- WiFi off: ~80 hours

### **Power from turbine:**
```
Turbine output → Charge controller → 12V battery → Buck converter → ESP32
```

**Note:** Add smoothing capacitors for stable power.

## Security Recommendations

**For public/research data:**
- ✅ Use local SD logging (primary)
- ✅ Use private MQTT broker (not public HiveMQ)
- ✅ Change default MQTT client ID
- ✅ Enable MQTT authentication

**For sensitive data:**
- Enable MQTT TLS/SSL
- Use VPN for remote access
- Encrypt SD card data
- Implement access controls

## Customization

### **Add GPS coordinates:**

```
#include <TinyGPS++.h>

void readGPS() {
  // Read GPS module and add to data structure
  currentData.latitude = gps.location.lat();
  currentData.longitude = gps.location.lng();
}
```

### **Add web dashboard:**

```
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

void setupWebServer() {
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String html = "<h1>Turbine: " + String(currentData.power_w) + " W</h1>";
    request->send(200, "text/html", html);
  });
  server.begin();
}
```

### **Add data validation:**

```
bool isDataValid() {
  // Reject physically impossible values
  if (currentData.wind_speed_ms > 30) return false;
  if (currentData.cp > 0.6) return false;
  if (currentData.power_w < 0) return false;
  return true;
}
```

## Limitations

**Known constraints:**
- **ADC accuracy:** ±2% (ESP32 ADC is non-linear)
- **Time drift:** ~1 sec/hour without NTP resync
- **SD card lifespan:** ~100,000 write cycles per sector
- **WiFi range:** ~50m indoor, ~100m outdoor (depends on antenna)
- **Max data rate:** ~10 Hz (limited by SD write speed)

**Not included (but possible to add):**
- Real-time clock (RTC) for offline timestamping
- LoRa/cellular for remote sites without WiFi
- Multi-turbine management
- Predictive maintenance algorithms
- Remote firmware updates (OTA)

## Next Steps

✅ **Example 08 Complete!**

**You now have a production-ready DAQ system!**

**To deploy:**
1. Customize configuration for your turbine
2. Test indoors for 24+ hours
3. Install on turbine
4. Begin data collection campaign
5. Analyze with `analysis/python_scripts/`

## Further Reading

- **Thesis integration:** `docs/best_practices/thesis_integration.md`
- **Data publication:** `docs/best_practices/fair_data_publication.md`
- **Troubleshooting:** `docs/troubleshooting/`
- **Hardware details:** `hardware/README.md`

## License

MIT License - Free to use, modify, and distribute with attribution.

## Support

- **Issues:** https://github.com/asithakal/wind-turbine-daq-guide/issues
- **Discussions:** https://github.com/asithakal/wind-turbine-daq-guide/discussions
- **Email:** [Contact via GitHub profile]

---

**Happy data collection! 🌬️⚡📊**