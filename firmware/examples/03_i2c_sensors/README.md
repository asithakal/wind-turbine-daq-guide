# Example 03: I2C Sensors (Power & Environment)

## Purpose

Read multiple I2C sensors simultaneously to measure turbine power output and environmental conditions.

## Learning Objectives

- Understand I2C protocol (two-wire serial communication)
- Initialize multiple I2C devices on the same bus
- Use Adafruit sensor libraries
- Implement error handling for sensor failures
- Scan I2C bus to find device addresses

## Hardware Required

- ESP32 board
- **INA226** or **INA219** power monitor module
- **BME280** environmental sensor module
- Breadboard and jumper wires
- (Optional) 2× 4.7kΩ pull-up resistors (if not on modules)

## Circuit Diagram

```
ESP32                 INA226              BME280
                     (0x40)              (0x76)
GPIO 21 (SDA) ───┬─── SDA ────────────── SDA
                 │
GPIO 22 (SCL) ───┼─── SCL ────────────── SCL
                 │
3.3V ────────────┼─── VCC ────────────── VCC
                 │
GND ─────────────┴─── GND ────────────── GND

Optional: 4.7kΩ pull-ups on SDA and SCL to 3.3V
(Usually already present on breakout modules)
```

## Connections Table

| Component | ESP32 Pin | Notes |
|-----------|-----------|-------|
| INA226 VCC | 3.3V | **Not 5V!** |
| INA226 GND | GND | |
| INA226 SDA | GPIO 21 | |
| INA226 SCL | GPIO 22 | |
| BME280 VCC | 3.3V | Same 3.3V rail |
| BME280 GND | GND | Same GND |
| BME280 SDA | GPIO 21 | Parallel with INA226 |
| BME280 SCL | GPIO 22 | Parallel with INA226 |

**Important:** All sensors share the same SDA/SCL bus - this is normal for I2C!

## I2C Addresses

Each I2C device needs a unique address:

| Sensor | Default Address | Alternative | How to Change |
|--------|-----------------|-------------|---------------|
| INA219 | 0x40 | 0x41, 0x44, 0x45 | Solder jumpers on module |
| INA226 | 0x40 | 0x41, 0x44, 0x45 | Solder jumpers on module |
| BME280 | 0x76 | 0x77 | SDO pin: GND=0x76, VCC=0x77 |

**If INA219 and BME280 both use 0x40:** Change BME280 to 0x77 by connecting SDO to VCC.

## Expected Output

**Serial Monitor (115200 baud):**

```
=================================
I2C Sensors Test - Example 03
=================================

Scanning I2C bus...
  Address       Device
  -------       ------
  0x40          INA219/INA226
  0x76          BME280
  Found 2 device(s)

Initializing INA219 power monitor... SUCCESS
Initializing BME280 environmental sensor... SUCCESS

Starting measurements...

Time(s) Voltage(V)      Current(mA)     Power(W)        Temp(°C)        Humidity(%)     Pressure(hPa)
-------------------------------------------------------------------------------------------
1.0     12.45           1520.3          18.923          28.3            75.2            1012.4
2.0     12.52           1605.7          20.103          28.3            75.3            1012.5
3.0     12.38           1435.2          17.768          28.4            75.3            1012.4
...
```

## Testing Without Turbine

### **INA226 Test:**

Connect a load to test power measurement:

```
12V Power Supply (+) ───→ INA226 V+ ───→ 12V LED Strip (+)
                                    │
                                    └───→ INA226 V-
                                    
12V Power Supply (-) ───→ LED Strip (-) ───→ GND
```

**Expected:** Voltage ~12V, Current = (LED power / 12V), Power = V × I

### **BME280 Test:**

No external connections needed - measures ambient conditions:
- **Temperature:** Should be room temp (20-30°C)
- **Humidity:** Varies by location (40-80% indoors)
- **Pressure:** Typical 950-1050 hPa (depends on altitude)

## Troubleshooting

### **Problem: "No I2C devices found!"**

**Step 1:** Check physical connections
```
- SDA wire securely connected to GPIO 21?
- SCL wire securely connected to GPIO 22?
- VCC to 3.3V (NOT 5V for most modules)?
- GND connected?
```

**Step 2:** Test with I2C scanner only:

Upload simple scanner sketch:
```
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);  // SDA, SCL
  Serial.println("I2C Scanner");
}

void loop() {
  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("Found: 0x");
      Serial.println(addr, HEX);
    }
  }
  Serial.println("Scan complete");
  delay(5000);
}
```

**Step 3:** Verify sensor modules with multimeter:
- VCC pin measures 3.3V?
- Pull-up resistors present? (10kΩ-47kΩ from SDA/SCL to VCC)

---

### **Problem: INA219 found but reads 0.00V**

**Causes:**
1. No voltage connected to V+ and V-
2. V+ and V- swapped
3. Measuring voltage < 0.1V (below resolution)

**Solutions:**
- Connect power source to INA219 screw terminals
- Verify polarity: V+ is positive supply, V- is ground/return path
- Test with known voltage (e.g., 12V battery)

---

### **Problem: BME280 found but reads NaN or crazy values**

**Cause:** Wrong I2C address in code

**Solution:**
Change address in code:
```
#define BME280_ADDRESS 0x77  // Try 0x77 instead of 0x76
```

Run I2C scanner to confirm actual address.

---

### **Problem: One sensor works, other doesn't**

**Likely cause:** Address conflict or sensor not powered

**Debug:**
```
// Add detailed error codes:
uint8_t bme_status = bme280.begin(BME280_ADDRESS);
Serial.print("BME280 status: 0x");
Serial.println(bme_status, HEX);
// 0x60 = success, other = failure code
```

---

### **Problem: Readings fluctuate wildly**

**Causes:**
1. Missing pull-up resistors
2. Long wires (> 20cm) causing signal degradation
3. Electrical noise

**Solutions:**
- Add 4.7kΩ resistors from SDA/SCL to 3.3V
- Use shorter wires or twisted pair
- Add 100nF capacitors across VCC/GND on each sensor

## I2C Protocol Overview

### **How I2C Works:**

1. **Master** (ESP32) controls the bus
2. **Slaves** (sensors) respond when addressed
3. **Two wires:** SDA (data), SCL (clock)
4. **Pull-ups:** Required to idle bus at HIGH

### **Communication sequence:**
```
1. Master sends START condition
2. Master sends device address (7 bits) + R/W bit
3. Slave acknowledges (ACK)
4. Master sends register address
5. Slave acknowledges
6. Master reads/writes data
7. Master sends STOP condition
```

### **Why two sensors can share wires:**

Each device has unique address → Master addresses one at a time → No conflicts!

## Library Documentation

### **Adafruit_INA219:**

Key functions:
```
float getBusVoltage_V()      // Read voltage (V)
float getCurrent_mA()        // Read current (mA)
float getPower_mW()          // Read power (mW)
void setCalibration_32V_2A() // Set range: 0-32V, 0-2A
void setCalibration_16V_400mA() // Alternative: 0-16V, 0-400mA (higher resolution)
```

**Choose calibration based on turbine:**
- Small turbine (< 100W): 16V_400mA for better accuracy
- Medium turbine (100-500W): 32V_2A (default)
- Large turbine (> 500W): 32V_1A (custom calibration needed)

### **Adafruit_BME280:**

Key functions:
```
float readTemperature()      // °C
float readHumidity()         // % RH
float readPressure()         // Pa (divide by 100 for hPa)
float readAltitude(seaLevel) // Meters (requires sea-level pressure input)
```

## Code Explanation

### **I2C Scanner:**

```
void scanI2C() {
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);  // Try to reach device
    error = Wire.endTransmission();    // 0 = success, other = no device
    if (error == 0) {
      // Device found at this address
    }
  }
}
```

**Why 127?** I2C uses 7-bit addresses: 0x01 to 0x7F (1-127 decimal)

### **Error Handling:**

```
bool ina219_available = false;

if (ina219.begin()) {
  ina219_available = true;  // Mark as working
}

// Later in loop:
if (ina219_available) {
  voltage = ina219.getBusVoltage_V();  // Safe to read
} else {
  voltage = -999;  // Error flag
}
```

**Why -999?** Indicates invalid data in CSV files (easy to filter out in analysis).

## Next Steps

✅ **Example 03 Complete!**

Continue to:
- [Example 04: RPM Measurement](../04_rpm_interrupt/) - Hall sensor with interrupts

## Further Reading

- **I2C Specification:** https://www.nxp.com/docs/en/user-guide/UM10204.pdf
- **INA219 Datasheet:** https://www.ti.com/lit/ds/symlink/ina219.pdf
- **BME280 Datasheet:** https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf