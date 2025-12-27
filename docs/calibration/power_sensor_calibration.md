# Power Sensor Calibration Procedure

Complete guide for calibrating INA226/INA219 current and voltage sensors.

**Estimated time:** 1-2 hours  
**Skill level:** Beginner-Intermediate  
**Required equipment:** Multimeter, precision resistors, DC power supply  
**Target accuracy:** ±1-3%

---

## Why Calibrate?

Power measurement accuracy depends on both voltage and current measurements:

\[ P = V \times I \]

**Impact of sensor errors:**
- 2% voltage error + 2% current error ≈ 4% power error
- Directly affects Cp calculation (linear relationship)

**Calibration frequency:**
- Before deployment: Mandatory
- Every 6 months: Recommended
- After board repair/replacement: Mandatory
- If power readings seem incorrect: Immediate

---

## Equipment Needed

| Item | Specification | Source |
|------|---------------|--------|
| **Digital multimeter (DMM)** | 0.5% accuracy or better, auto-ranging | ~$50 USD (Fluke 115, UNI-T UT61E) |
| **Precision resistors** | 1% tolerance, 10Ω and 1Ω, 5W rated | Electronics supplier |
| **DC power supply** | 0-30V, 0-3A adjustable | Lab equipment or bench PSU |
| **Shunt resistor** | 0.1Ω, 1%, 2W (if using INA226) | Included in INA226 breakout |
| **Breadboard + wires** | For test connections | Standard electronics kit |
| **Load resistor** | 10-50Ω, 10W (simulates turbine load) | High-power resistor |

---

## INA226 vs INA219: Key Differences

| Feature | INA226 | INA219 |
|---------|--------|--------|
| **Voltage range** | 0-36V | 0-26V |
| **Current range** | ±20A (with 0.005Ω shunt) | ±3.2A (with 0.1Ω shunt) |
| **Resolution** | 16-bit | 12-bit |
| **Accuracy** | ±0.1% (voltage), ±0.5% (current) | ±0.5% (voltage), ±1% (current) |
| **I2C address** | 0x40 (default) | 0x40 (default) |

**This guide covers both.** Procedures are identical except where noted.

---

## Method 1: Two-Point Calibration ⭐ **(Recommended for Field Use)**

**Accuracy:** ±2-3%  
**Time:** 30-60 minutes  
**Sufficient for:** Research-grade measurements

### **Setup Diagram**

```
    [DC Power Supply]
           |
           +--[+]---> INA226 V+ ----> [Load Resistor] ---+
           |                                              |
       [DMM Voltage]                                      |
           |                                              |
           +--[-]---> INA226 V- <-----[Shunt Resistor]---+
                           |                    |
                      [DMM Current]        [INA226 GND]
```

---

### **Step 1: Voltage Calibration**

**Test Point 1: Low Voltage (5V)**

1. **Set power supply:** 5.0V, current limit 1A
2. **Connect DMM in parallel:** Measure voltage across INA226 V+ to V-
3. **Record readings:**
   - DMM (reference): 5.02V
   - INA226 (Serial Monitor): 5.15V
   - Error: +2.6%

**Test Point 2: High Voltage (20V)**

1. **Set power supply:** 20.0V, current limit 1A
2. **Record readings:**
   - DMM (reference): 20.04V
   - INA226 (Serial Monitor): 20.28V
   - Error: +1.2%

**Calculate correction factor:**

\[ \text{Voltage correction} = \frac{\text{DMM average}}{\text{INA226 average}} \]

Example:

\[ \text{Correction} = \frac{(5.02 + 20.04) / 2}{(5.15 + 20.28) / 2} = \frac{12.53}{12.715} = 0.9854 \]

---

### **Step 2: Current Calibration**

**Test Point 1: Low Current (0.5A)**

1. **Set load resistor:** 10Ω, 5W
2. **Set power supply:** 5V → Current ≈ 0.5A
3. **Connect DMM in series:** Measures current through circuit
4. **Record readings:**
   - DMM (reference): 0.498A
   - INA226 (Serial Monitor): 0.512A
   - Error: +2.8%

**Test Point 2: High Current (2A)**

1. **Set power supply:** 20V with 10Ω load → Current ≈ 2A
2. **⚠️ Warning:** Check resistor temperature (should be <100°C)
3. **Record readings:**
   - DMM (reference): 1.996A
   - INA226 (Serial Monitor): 2.048A
   - Error: +2.6%

**Calculate correction factor:**

\[ \text{Current correction} = \frac{\text{DMM average}}{\text{INA226 average}} \]

Example:

\[ \text{Correction} = \frac{(0.498 + 1.996) / 2}{(0.512 + 2.048) / 2} = \frac{1.247}{1.280} = 0.9742 \]

---

### **Step 3: Apply Corrections in Firmware**

**Update `config.h`:**

```
// Calibration coefficients from your measurements
#define INA226_VOLTAGE_CORRECTION 0.9854  // From Step 1
#define INA226_CURRENT_CORRECTION 0.9742  // From Step 2

// In main code (sensors.cpp):
float getVoltage() {
  float raw_voltage = ina226.getBusVoltage_V();
  return raw_voltage * INA226_VOLTAGE_CORRECTION;
}

float getCurrent() {
  float raw_current = ina226.getCurrent_mA() / 1000.0;  // Convert mA to A
  return raw_current * INA226_CURRENT_CORRECTION;
}

float getPower() {
  float voltage = getVoltage();
  float current = getCurrent();
  return voltage * current;  // Watts
}
```

---

### **Step 4: Verification**

**Re-test at mid-range (12V, 1.2A):**

1. Set power supply: 12V with appropriate load
2. Measure with DMM:
   - Voltage: 12.03V
   - Current: 1.203A
   - Power: 14.48W (calculated)

3. Read from INA226 (with corrections applied):
   - Voltage: 12.01V (error: -0.17%)
   - Current: 1.198A (error: -0.42%)
   - Power: 14.38W (error: -0.69%)

**Acceptance criteria:**
- Voltage error < 2%
- Current error < 2%
- Power error < 3%

**Pass!** Calibration successful.

---

## Method 2: Multi-Point Calibration **(Higher Accuracy)**

**Accuracy:** ±1%  
**Time:** 2-3 hours  
**Use for:** Publication-quality data, doctoral research

### **Voltage Calibration Matrix**

| Set Voltage | DMM Reading | INA226 Raw | Correction |
|-------------|-------------|------------|------------|
| 0V | 0.00V | 0.02V | - |
| 5V | 5.01V | 5.12V | 0.978 |
| 10V | 10.03V | 10.18V | 0.985 |
| 15V | 15.02V | 15.20V | 0.988 |
| 20V | 20.04V | 20.26V | 0.989 |
| 25V | 25.01V | 25.28V | 0.989 |

**Generate calibration curve:**

Using linear regression (Excel or Python):

```
import numpy as np
from scipy import stats

dmm_readings = [5.01, 10.03, 15.02, 20.04, 25.01]
ina_readings = [5.12, 10.18, 15.20, 20.26, 25.28]

slope, intercept, r_value, _, _ = stats.linregress(ina_readings, dmm_readings)

print(f"Voltage correction: V_true = {slope:.5f} × V_measured + {intercept:.5f}")
print(f"R² = {r_value**2:.6f}")
```

**Example output:**
```
Voltage correction: V_true = 0.98542 × V_measured + 0.01234
R² = 0.999987
```

**Apply in code:**

```
float getCalibratedVoltage(float raw_voltage) {
  return 0.98542 * raw_voltage + 0.01234;
}
```

---

### **Current Calibration Matrix**

| Set Current | DMM Reading | INA226 Raw | Correction |
|-------------|-------------|------------|------------|
| 0A | 0.000A | 0.003A | - |
| 0.5A | 0.498A | 0.512A | 0.973 |
| 1.0A | 0.997A | 1.024A | 0.974 |
| 1.5A | 1.495A | 1.536A | 0.973 |
| 2.0A | 1.996A | 2.048A | 0.975 |
| 2.5A | 2.498A | 2.560A | 0.976 |

**Similar regression analysis:**

```
Current correction: I_true = 0.97425 × I_measured - 0.00156
R² = 0.999992
```

---

## Special Considerations

### **Shunt Resistor Selection (INA226)**

**Standard shunt values:**

| Shunt Resistance | Max Current | Resolution | Power Rating Needed |
|------------------|-------------|------------|---------------------|
| 0.1Ω | 3.2A | 0.8mA | 1W @ 3A |
| 0.05Ω | 6.4A | 1.6mA | 2W @ 6A |
| 0.01Ω | 20A | 8mA | 4W @ 20A |
| 0.005Ω | 40A | 16mA | 8W @ 40A |

**For 500W turbine @ 24V:**
- Expected current: 500W / 24V ≈ 21A peak
- **Recommended shunt:** 0.005Ω, 10W rated

**⚠️ Critical:** Shunt resistance must match INA226 configuration in code!

```
// In setup():
ina226.begin();
ina226.configure(INA226_AVERAGES_16, INA226_BUS_CONV_TIME_1100US,
                 INA226_SHUNT_CONV_TIME_1100US, INA226_MODE_SHUNT_BUS_CONT);

// Calibrate for 0.005Ω shunt, 20A max:
ina226.calibrate(0.005, 20);  // (shunt_resistance, max_current)
```

---

### **Self-Heating Error**

**Problem:** Shunt resistor heats up during high current, changing resistance.

**Example:**
- 0.1Ω shunt at 3A → 0.9W dissipation
- Temperature rise: ~40°C (if no heatsink)
- Resistance change: +0.4% (assuming 100 ppm/°C TCR)

**Solutions:**
1. Use low-TCR shunt (< 50 ppm/°C, e.g., manganin alloy)
2. Add heatsink to shunt resistor
3. Calibrate at expected operating temperature
4. Apply temperature compensation in code (if sensor available)

---

### **Load Effect on Calibration**

**Important:** Calibrate with realistic load, not just resistors!

**Reason:** Turbine electrical characteristics differ from resistive load:
- Inductive effects from generator
- PWM switching from charge controller
- Transient current spikes

**Best practice:**
1. Initial calibration with precision resistors (this guide)
2. Field verification with actual turbine connected
3. Re-check if systematic errors appear in dataset

---

## Calibration Certificate Template

Record in `calibration_datasheet_template.xlsx`:

### **Device Information**
- Sensor model: INA226 (or INA219)
- Breakout board: Adafruit 904 (or specify)
- Shunt resistance: 0.1Ω
- Shunt tolerance: 1%
- Date calibrated: 2025-06-15

### **Calibration Equipment**
- Multimeter model: Fluke 115
- Last calibration: 2024-12-10 (certificate #FL115-202412)
- Power supply: Rigol DP832
- Ambient temperature: 25°C

### **Results**
- Voltage correction: `V_true = 0.9854 × V_meas + 0.0123`
- Current correction: `I_true = 0.9742 × I_meas - 0.0016`
- Valid range: 0-25V, 0-3A
- Accuracy achieved: ±1.5% voltage, ±2.0% current

### **Verification Test**
- Test point: 12V, 1.2A
- Error: Voltage -0.17%, Current -0.42%, Power -0.69%
- Status: ✅ PASS

**Attachments:**
- Calibration data CSV
- Photo of test setup
- Multimeter calibration certificate

---

## Troubleshooting

### **Problem: INA226 reads 0.00V and 0.00A**

**Possible causes:**
1. **I2C wiring issue:**
   - Check SDA = GPIO 21, SCL = GPIO 22
   - Run I2C scanner: File → Examples → Wire → i2c_scanner
   - Should show address 0x40

2. **Incorrect I2C address:**
   ```
   // Try different addresses:
   ina226.begin(0x40);  // Default
   ina226.begin(0x41);  // If A0 jumper set
   ```

3. **Power supply:**
   - INA226 needs 3.3-5V on VCC
   - Check with multimeter

---

### **Problem: Readings fluctuate wildly**

**Causes:**
1. **Noisy power supply:**
   - Add 0.1µF capacitor between VCC and GND
   - Add 10µF capacitor on bus voltage

2. **Poor connections:**
   - Solder wires instead of breadboard
   - Ensure shunt connections are solid (use screw terminals)

3. **PWM interference:**
   - Increase averaging in INA226 config:
     ```
     ina226.configure(INA226_AVERAGES_128, ...);  // Max averaging
     ```

---

### **Problem: Current reads correctly, but power is wrong**

**Likely cause:** Mismatch between shunt value in code and actual shunt.

**Fix:**
```
// Measure actual shunt resistance with DMM (4-wire method if possible)
// Update calibration:
float actual_shunt = 0.098;  // Measured value (not nominal 0.1Ω)
ina226.calibrate(actual_shunt, 20);
```

---

## Measurement Uncertainty Budget

### **Error Sources:**

| Source | Typical Uncertainty | Effect on Power |
|--------|---------------------|-----------------|
| DMM accuracy | ±0.5% | ±0.5% |
| Shunt tolerance | ±1% | ±1% |
| INA226 ADC | ±0.1% | ±0.1% |
| Connection resistance | ±0.2% | ±0.2% |
| Temperature drift | ±0.5% | ±0.5% |
| **Combined (RSS)** | **±1.3%** | **±2.6% (V×I)** |

**Formula:**
\[ u_P = P \sqrt{\left(\frac{u_V}{V}\right)^2 + \left(\frac{u_I}{I}\right)^2} \]

---

## Advanced: Temperature Compensation

**For harsh environments (0-60°C range):**

```
// Measure temperature with BME280
float temp_c = bme.readTemperature();

// Apply temperature coefficient (typical: 50 ppm/°C for manganin shunt)
float temp_correction = 1.0 + 0.00005 * (temp_c - 25.0);

float getCalibratedCurrent(float raw_current) {
  float corrected = raw_current * INA226_CURRENT_CORRECTION;
  return corrected * temp_correction;
}
```

---

## Calibration Schedule

| Condition | Interval |
|-----------|----------|
| **New sensor** | Before first use |
| **Standard use** | Every 6 months |
| **Precision research** | Every 3 months |
| **After power surge** | Immediately |
| **Shunt replacement** | Immediately |

---

## References

- INA226 Datasheet: Texas Instruments SBOS547 (Rev. D)
- INA219 Datasheet: Texas Instruments SBOS448 (Rev. G)
- ISO 5725: Accuracy and precision of measurement methods

---

## Related Documents

- [`anemometer_calibration.md`](anemometer_calibration.md)
- [`temperature_sensor_calibration.md`](temperature_sensor_calibration.md)
- [`../troubleshooting/hardware_debugging.md`](../troubleshooting/hardware_debugging.md)

---

**Questions?** Open an issue: https://github.com/asithakal/wind-turbine-daq-guide/issues