# Hardware Debugging Guide

Systematic approach to diagnosing hardware issues in your wind turbine DAQ system.

**Contents:**
- [Power Issues](#power-issues)
- [Sensor Problems](#sensor-problems)
- [I2C Communication](#i2c-communication)
- [SD Card Hardware](#sd-card-hardware)
- [Wiring Verification](#wiring-verification)
- [Tools and Multimeter Usage](#tools-and-multimeter-usage)

---

## Power Issues

### **Symptom 1: ESP32 won't power on (no LED, no response)**

**Diagnostic flowchart:**

```
ESP32 completely dead?
├─ Check USB cable:
│  ├─ Try different cable (must be DATA cable, not charge-only)
│  ├─ Try different USB port
│  └─ Try different computer
│
├─ Check power LED on ESP32 board:
│  ├─ LED on → Power OK, move to next section
│  └─ LED off → Continue diagnostics
│
├─ Measure voltage at 3.3V pin:
│  ├─ 3.2-3.4V → Power OK
│  ├─ 0V → Power supply failure
│  └─ >3.5V → Voltage regulator damaged
│
└─ Check for short circuits:
   ├─ Measure resistance between 3.3V and GND (should be > 100Ω)
   └─ If < 10Ω → Short circuit (check wiring)
```

**Multimeter test:**

1. Set multimeter to DC voltage (20V range)
2. Black probe to GND pin
3. Red probe to 3.3V pin
4. Reading should be **3.2-3.4V**

**If 0V:**
- USB cable faulty (try different one)
- USB port insufficient power (try powered hub)
- ESP32 board damaged (replace)

**If >3.5V:**
- Voltage regulator failed (dangerous! Disconnect immediately)

---

### **Symptom 2: ESP32 resets randomly or "brownout" errors**

**Causes:**
1. Insufficient power supply current
2. Power supply noise from sensors
3. Loose connections

**Solution A: Measure current draw**

1. Set multimeter to DC current (mA or A range)
2. Break power connection (USB or VIN)
3. Insert multimeter in series
4. Typical draw:
   - ESP32 idle: 80-100 mA
   - ESP32 with WiFi: 150-240 mA
   - With sensors: Add 50-100 mA

**If total > 500mA:** USB port may be insufficient

**Solution B: Add capacitors (stabilize power)**

```
Component placement:
┌─────────────────┐
│   ESP32 Board   │
│                 │
│  GND ─────┬────┤
│           │     │
│        ┌──┴──┐  │
│        │ 100µF│  │  ← Add 100µF electrolytic capacitor
│        └──┬──┘  │     (+ to 3.3V, - to GND)
│           │     │
│  3.3V ────┴────┤
└─────────────────┘

Also add near each sensor:
  0.1µF ceramic capacitor between VCC and GND
```

**Solution C: Use external power supply**

- 5V/2A power supply connected to ESP32 VIN pin
- Or 3.7V LiPo battery with voltage regulator

---

### **Symptom 3: Sensors cause system reset when connected**

**Diagnostic steps:**

1. **Isolate problem sensor:**
   ```
   a) Disconnect ALL sensors
   b) Upload test sketch (blink LED)
   c) If stable → Power issue from sensors
   d) Connect sensors one by one until system crashes
   e) Culprit identified!
   ```

2. **Check sensor power consumption:**
   - INA226: ~1 mA (OK)
   - BME280: ~3.6 µA (OK)
   - Hall sensor: ~5 mA (OK)
   - SD card: 50-100 mA (can cause issues!)
   
   **If using multiple high-power sensors:** Use external 5V supply, not ESP32's 3.3V regulator

3. **Check for short circuits:**
   ```
   Power off → Multimeter in resistance mode
   Measure between VCC and GND of each sensor
   Should show > 100Ω (typically 1kΩ - 10kΩ)
   If < 10Ω → Sensor damaged or wiring shorted
   ```

---

## Sensor Problems

### **Symptom 4: Sensor always returns -999, NaN, or 0**

**Cause:** Sensor not responding (wiring, power, or I2C address issue)

**Diagnostic protocol:**

**Step 1: Visual inspection**
- Check all solder connections
- Look for bent pins
- Check for loose jumper wires
- Verify correct sensor module (BME280 vs BMP280, etc.)

**Step 2: Power test**
```
Multimeter DC voltage mode:
  Sensor VCC pin → Should read 3.3V (±0.2V)
  Sensor GND pin → Should read 0V
  
If wrong voltage:
  - Check wiring to ESP32 3.3V/GND pins
  - Check if power LED on sensor board is lit
```

**Step 3: I2C communication test**

Upload I2C scanner sketch:

```
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);  // SDA=21, SCL=22 (ESP32 defaults)
  Serial.println("\nI2C Scanner");
}

void loop() {
  byte error, address;
  int devices = 0;
  
  Serial.println("Scanning...");
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    
    if (error == 0) {
      Serial.print("Device found at 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      devices++;
    }
  }
  
  if (devices == 0) {
    Serial.println("No I2C devices found!");
    Serial.println("Check wiring:");
    Serial.println("  SDA → GPIO 21");
    Serial.println("  SCL → GPIO 22");
    Serial.println("  VCC → 3.3V");
    Serial.println("  GND → GND");
  }
  
  delay(5000);
}
```

**Expected output:**
```
Scanning...
Device found at 0x40  ← INA226
Device found at 0x76  ← BME280 (or 0x77)
```

**If no devices found:**
- Wiring error (check SDA/SCL connections)
- Pull-up resistors missing (most breakout boards have built-in, but check)
- Sensor damaged

**If device found but still returns -999:**
- Wrong I2C address in code:
  ```
  // Try both addresses:
  bme.begin(0x76);  // First try
  // If fails:
  bme.begin(0x77);  // Then try
  ```

---

### **Symptom 5: Hall sensor (RPM) not counting**

**Problem:** RPM always reads 0 or incorrect value

**Diagnostic steps:**

**1. Check magnet detection:**
```
// Simple magnet test sketch:
void setup() {
  pinMode(15, INPUT);  // GPIO 15 = Hall sensor pin
  Serial.begin(115200);
}

void loop() {
  int state = digitalRead(15);
  Serial.print("Hall sensor: ");
  Serial.println(state);  // Should toggle 0/1 when magnet passes
  delay(100);
}
```

**Expected behavior:**
- No magnet: Reads HIGH (1)
- Magnet passes: Reads LOW (0)
- **If always HIGH or always LOW:** Sensor not working

**2. Check distance:**
- Hall sensor effective range: 5-10 mm
- Too far: No detection
- Too close: Constant LOW
- **Optimal:** 5-7 mm gap

**3. Check magnet polarity:**
- Sensor detects **South pole** (usually)
- Try flipping magnet 180°

**4. Check wiring:**
```
Hall Sensor (3 pins):
  VCC → ESP32 3.3V
  GND → ESP32 GND
  OUT → ESP32 GPIO 15

Measure voltage on OUT pin:
  No magnet: ~3.3V
  Magnet near: ~0V
```

---

### **Symptom 6: Anemometer reads incorrect values**

**Problem:** Wind speed always 0, or unrealistic values (e.g., 200 m/s)

**Diagnostic steps:**

**1. Check analog voltage:**

```
void setup() {
  Serial.begin(115200);
}

void loop() {
  int rawADC = analogRead(34);  // GPIO 34
  float voltage = rawADC * (3.3 / 4095.0);
  
  Serial.print("ADC raw: ");
  Serial.print(rawADC);
  Serial.print("\tVoltage: ");
  Serial.println(voltage);
  
  delay(500);
}
```

**Expected output (Inspeed Vortex anemometer):**
- No wind (cups not spinning): 0.4V
- Light breeze (2-3 m/s): ~0.8V
- Moderate wind (7-8 m/s): ~1.5V
- Strong wind (15 m/s): ~2.8V

**If always 0V:**
- Wiring error (check signal wire)
- Anemometer not powered (check VCC/GND)
- Anemometer damaged

**If always 3.3V or random values:**
- Pin not connected (floating input)
- Check GPIO 34 is ADC-capable (ESP32 GPIO 34-39 are ADC1)

**2. Calibration check:**

```
// Expected formula for Inspeed Vortex:
// V_out = 0.4V + (wind_speed_m/s * 0.2V)

float voltage = analogRead(34) * (3.3 / 4095.0);
float windSpeed = (voltage - 0.4) / 0.2;

// At 0 wind, should read ~0 m/s
// At known wind (e.g., fan), compare to reference anemometer
```

**See:** [Anemometer Calibration Guide](../calibration/anemometer_calibration.md)

---

## I2C Communication

### **Symptom 7: I2C bus crashes or freezes**

**Problem:** System hangs when trying to read sensors

**Causes:**
1. Bus conflict (two devices same address)
2. Missing pull-up resistors
3. Wiring too long (noise/capacitance)

**Solution A: Check for address conflicts**

Run I2C scanner (see above). Each device must have unique address.

**Common addresses:**
- 0x40: INA219/INA226 (default)
- 0x76/0x77: BME280/BMP280
- 0x48-0x4B: ADS1115 (if using external ADC)

**If conflict:** Change address via solder jumpers on breakout board (check module datasheet)

**Solution B: Add pull-up resistors (if missing)**

```
SDA line:  ──────┬──────── (to ESP32 GPIO 21)
                 │
              ┌──┴──┐
              │ 4.7kΩ│  ← Pull-up resistor to 3.3V
              └──┬──┘
                 │
            3.3V─┴

Repeat for SCL line (GPIO 22)
```

**Note:** Most breakout boards have built-in pull-ups. Only add if:
- Using bare sensor chips
- I2C bus > 30 cm long
- Multiple sensors (>3) on same bus

**Solution C: Reduce wire length**
- Keep I2C wires < 30 cm if possible
- Use twisted pair or shielded cable for longer runs
- Add 100pF capacitors between SDA/SCL and GND (noise filtering)

---

### **Symptom 8: I2C works intermittently**

**Problem:** Sensors work sometimes, fail randomly

**Causes:** Noise, loose connections, voltage drops

**Solutions:**

**1. Add decoupling capacitors near each sensor:**
```
At EACH sensor breakout board:
  VCC ─┬─── (to ESP32 3.3V)
       │
    ┌──┴──┐
    │0.1µF│ ← Ceramic capacitor
    └──┬──┘
       │
  GND ─┴─── (to ESP32 GND)
```

**2. Check solder joints:**
- Reflow all I2C connections
- Check for cold solder joints (dull, grainy appearance)

**3. Use logic analyzer (advanced):**
- Connect logic analyzer to SDA/SCL lines
- Look for glitches, bus stretching, NACK errors
- PulseView software (free): https://sigrok.org/

---

## SD Card Hardware

### **Symptom 9: SD card not detected**

**Full diagnostic guide:** [`sd_card_problems.md`](sd_card_problems.md)

**Quick hardware checks:**

**1. Wiring verification:**
```
SD Card Module (SPI):
  CS   → GPIO 5  (Chip Select)
  SCK  → GPIO 18 (Clock)
  MOSI → GPIO 23 (Master Out Slave In)
  MISO → GPIO 19 (Master In Slave Out)
  VCC  → 5V or 3.3V (check module specs!)
  GND  → GND

Measure continuity with multimeter (power off!):
  ESP32 GPIO 5  ↔ SD Module CS pin  (should beep)
  Repeat for all connections
```

**2. Power test:**
```
Multimeter DC voltage (card inserted, system powered):
  SD Module VCC: Should read 3.3V or 5V
  If 0V → Power wiring error
```

**3. Card compatibility:**
- Use 32 GB or smaller (SDHC)
- Avoid SDXC cards (> 32 GB) - not supported by all libraries
- Format as FAT32, not exFAT
- Try different card if one doesn't work

---

## Wiring Verification

### **Master Checklist: Complete System Wiring**

Print this and check off each connection:

```
═══════════════════════════════════════════════════════════
POWER CONNECTIONS
═══════════════════════════════════════════════════════════
[ ] ESP32 USB connected to computer/power supply
[ ] All sensor VCC pins connected to ESP32 3.3V pin
[ ] All sensor GND pins connected to ESP32 GND pin
[ ] SD module VCC connected (check if 3.3V or 5V required!)

═══════════════════════════════════════════════════════════
I2C SENSORS (INA226 + BME280)
═══════════════════════════════════════════════════════════
[ ] INA226 SDA → ESP32 GPIO 21
[ ] INA226 SCL → ESP32 GPIO 22
[ ] INA226 VCC → ESP32 3.3V
[ ] INA226 GND → ESP32 GND

[ ] BME280 SDA → ESP32 GPIO 21 (shared with INA226)
[ ] BME280 SCL → ESP32 GPIO 22 (shared with INA226)
[ ] BME280 VCC → ESP32 3.3V
[ ] BME280 GND → ESP32 GND

═══════════════════════════════════════════════════════════
ANALOG SENSOR (Anemometer)
═══════════════════════════════════════════════════════════
[ ] Anemometer Signal → ESP32 GPIO 34 (ADC1_CH6)
[ ] Anemometer VCC → ESP32 3.3V (or 5V if required)
[ ] Anemometer GND → ESP32 GND

═══════════════════════════════════════════════════════════
DIGITAL SENSOR (Hall Effect RPM)
═══════════════════════════════════════════════════════════
[ ] Hall Sensor OUT → ESP32 GPIO 15
[ ] Hall Sensor VCC → ESP32 3.3V
[ ] Hall Sensor GND → ESP32 GND

═══════════════════════════════════════════════════════════
SD CARD MODULE (SPI)
═══════════════════════════════════════════════════════════
[ ] SD Module CS   → ESP32 GPIO 5
[ ] SD Module SCK  → ESP32 GPIO 18
[ ] SD Module MOSI → ESP32 GPIO 23
[ ] SD Module MISO → ESP32 GPIO 19
[ ] SD Module VCC  → ESP32 5V or 3.3V (check module datasheet!)
[ ] SD Module GND  → ESP32 GND
[ ] SD card inserted and formatted FAT32

═══════════════════════════════════════════════════════════
COMMON GROUND CHECK
═══════════════════════════════════════════════════════════
[ ] All GND connections tied together (star ground preferred)
[ ] Continuity test: All GND points should show 0Ω resistance

═══════════════════════════════════════════════════════════
```

---

## Tools and Multimeter Usage

### **Essential Tools**

1. **Digital Multimeter (DMM)**
   - Functions needed: DC voltage, DC current, resistance, continuity
   - Budget: $15-30 (sufficient for this project)
   - Recommended: Uni-T UT61E or similar

2. **Logic Analyzer (optional but helpful)**
   - For I2C/SPI debugging
   - Budget: $10-20 (Saleae clones on AliExpress)
   - Software: PulseView (free)

3. **USB Power Meter**
   - Measures current draw in real-time
   - Helps identify power issues
   - Budget: $10-15

---

### **Multimeter Quick Reference**

**Measure DC Voltage:**
```
1. Turn dial to "V⎓" or "DCV" (20V range)
2. Black probe → GND
3. Red probe → Point to measure
4. Expected: 0V (GND), 3.3V (VCC), 5V (USB)
```

**Measure Current:**
```
1. Turn dial to "A⎓" or "DCA" (10A or mA range)
2. Break circuit (disconnect wire)
3. Insert meter in series (black to -, red to +)
4. Expected: 80-250 mA for ESP32 system
```

**Check Continuity:**
```
1. Turn dial to "continuity" symbol ())) or Ω
2. Touch probes to two points
3. Meter beeps = connection OK (resistance < 50Ω)
4. No beep = open circuit or bad connection
```

**Measure Resistance:**
```
1. Turn dial to "Ω" (200Ω or 2kΩ range)
2. Power OFF circuit!
3. Touch probes to component
4. Expected: Pull-up resistors ~4.7kΩ, shorts <1Ω
```

---

### **Common Measurement Points**

**System health check (all in DC voltage mode):**

| Point | Expected | If Wrong |
|-------|----------|----------|
| ESP32 3.3V pin | 3.2-3.4V | Power supply issue |
| ESP32 GND pin | 0V | N/A (reference) |
| USB VBUS (5V) | 4.8-5.2V | Bad USB cable/port |
| INA226 VCC | 3.2-3.4V | Wiring error |
| BME280 VCC | 3.2-3.4V | Wiring error |
| Anemometer output (no wind) | 0.4V ± 0.1V | Sensor issue |
| Hall sensor output (no magnet) | ~3.3V | Sensor issue |
| SD Module VCC | 3.3V or 5V | Check module specs |

---

## When Hardware Is Confirmed OK

If all hardware checks pass but system still doesn't work:

**Check firmware issues:**
- [`common_errors.md`](common_errors.md) - Compilation/upload errors
- [`sd_card_problems.md`](sd_card_problems.md) - SD card software issues
- [`mqtt_connectivity.md`](mqtt_connectivity.md) - WiFi/network issues

**Get help:**
- Post issue with:
  - Photos of wiring
  - Multimeter voltage readings
  - Serial Monitor output
  - Sketch code
- Repository: https://github.com/asithakal/wind-turbine-daq-guide/issues

---

**Last updated:** December 2024  
**Hardware tested:** ESP32-WROOM-32, INA226, BME280, Inspeed Vortex anemometer