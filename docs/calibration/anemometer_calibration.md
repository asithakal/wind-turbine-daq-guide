# Anemometer Calibration Procedure

Complete guide for calibrating wind speed sensors to ensure research-grade accuracy.

**Estimated time:** 2-4 hours (simple), 1-2 days (full calibration)  
**Skill level:** Intermediate  
**Required equipment:** Reference anemometer OR wind tunnel OR vehicle method  
**Target accuracy:** ±5% over 3-20 m/s range

---

## Why Calibrate?

Anemometer accuracy directly affects Cp calculation (wind speed is cubed in power equation):

\[ P_{\text{available}} = \frac{1}{2} \rho A v^3 \]

**Impact of wind speed error on Cp:**
- 5% wind speed error → 15% error in Cp
- 10% wind speed error → 30% error in Cp

**Calibration frequency:**
- Before deployment: Mandatory
- Every 3 months: Recommended for tropical/coastal environments
- After bearing replacement: Mandatory
- If readings seem suspect: Immediate

---

## Method 1: Reference Anemometer Comparison ⭐ **(Recommended)**

**Accuracy:** ±2-5%  
**Cost:** Medium (if reference unit available)  
**Time:** 2-4 hours

### **Equipment Needed**

| Item | Specification | Source |
|------|---------------|--------|
| **Reference anemometer** | Calibrated, ±2% accuracy | Borrow from meteorology dept |
| **Mounting pole** | 2m height minimum | Hardware store |
| **Data logger** | ESP32 with your firmware | Your DAQ system |
| **Laptop** | For recording data | Personal |
| **Open field** | Minimal obstacles, 50m radius | University grounds |

---

### **Step 1: Site Selection**

**Ideal location criteria:**
- ✅ Open field, away from buildings (>50m)
- ✅ Flat terrain
- ✅ Consistent wind direction for 2-3 hours
- ✅ Wind speed range 3-15 m/s

**Bad locations:**
- ❌ Between buildings (turbulent flow)
- ❌ Near trees (wind shadow)
- ❌ Rooftops (accelerated flow)

---

### **Step 2: Mounting Setup**

**Side-by-side configuration:**

```
                Wind Direction →
                
    [Reference    [Test
     Anemometer]  Anemometer]
         |            |
         |            |
    |----+----+-------+-----| Mounting bar (1m spacing)
              |
         [Support pole]
              |
         [Base/tripod]
```

**Critical requirements:**
1. **Same height:** Within ±5 cm
2. **Horizontal separation:** 1-2 meters (perpendicular to wind)
3. **No mutual interference:** Anemometers should not shadow each other
4. **Orientation:** Both facing into wind

**Tip:** Use spirit level to ensure both sensors are vertical.

---

### **Step 3: Data Collection**

1. **Wait for steady wind:** 
   - Avoid gusty conditions
   - Best time: Mid-morning to early afternoon
   - Wind speed > 3 m/s

2. **Start logging:**
   - Reference anemometer → Record manually or log to separate system
   - Test anemometer → ESP32 SD card logging at 1 Hz
   - Duration: Minimum 1 hour, preferably 3 hours

3. **Record conditions:**
   - Timestamp (start/end)
   - Temperature
   - Weather (clear/cloudy)
   - Wind direction (compass bearing)

**Sample recording sheet:**

| Time | Reference (m/s) | Test (m/s) | Notes |
|------|-----------------|------------|-------|
| 10:00 | 5.2 | 5.5 | Steady wind |
| 10:01 | 5.3 | 5.6 | |
| 10:02 | 5.1 | 5.4 | |
| ... | ... | ... | |

---

### **Step 4: Data Analysis**

**Using Excel or Python:**

#### **Option A: Excel Method**

1. Import both datasets to columns A (reference) and B (test)
2. Create scatter plot: Reference (X-axis) vs Test (Y-axis)
3. Add trendline:
   - Right-click data → Add Trendline
   - Select "Linear"
   - Check "Display equation" and "Display R²"

**Expected equation:**
```
Test = m × Reference + b
```

Where:
- `m` = slope (ideally 1.0)
- `b` = offset (ideally 0.0)
- `R²` = correlation (should be >0.95)

**Example result:**
```
Test = 1.05 × Reference + 0.1
R² = 0.98
```

**Interpretation:**
- Your anemometer reads 5% high (slope = 1.05)
- Has a +0.1 m/s offset

#### **Option B: Python Method**

```
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy import stats

# Load data
df = pd.read_csv('calibration_data.csv')
reference = df['reference_ms']
test = df['test_ms']

# Linear regression
slope, intercept, r_value, p_value, std_err = stats.linregress(reference, test)

print(f"Calibration equation: Test = {slope:.3f} × Reference + {intercept:.3f}")
print(f"R² = {r_value**2:.4f}")
print(f"Standard error = {std_err:.3f} m/s")

# Plot
plt.figure(figsize=(8, 6))
plt.scatter(reference, test, alpha=0.5, label='Data points')
plt.plot(reference, slope*reference + intercept, 'r-', label=f'Fit: y={slope:.3f}x+{intercept:.3f}')
plt.plot(,, 'k--', label='Ideal (1:1)')[1]
plt.xlabel('Reference Anemometer (m/s)')
plt.ylabel('Test Anemometer (m/s)')
plt.legend()
plt.grid(True, alpha=0.3)
plt.title('Anemometer Calibration')
plt.savefig('calibration_curve.png', dpi=300)
plt.show()
```

---

### **Step 5: Apply Correction**

**In firmware code:**

```
// Before calibration:
float wind_speed_raw = analogRead(ANEM_PIN) * ADC_TO_VOLTAGE;

// After calibration (solve for reference):
// Test = 1.05 × Reference + 0.1
// Reference = (Test - 0.1) / 1.05

float wind_speed_corrected = (wind_speed_raw - 0.1) / 1.05;
```

**Update in `config.h`:**

```
// Calibration coefficients (update from your analysis)
#define ANEM_SLOPE 1.05      // From calibration
#define ANEM_OFFSET 0.10     // From calibration (m/s)

// In main code:
float correctWindSpeed(float raw_reading) {
  return (raw_reading - ANEM_OFFSET) / ANEM_SLOPE;
}
```

---

### **Step 6: Verification**

**Re-test after applying correction:**

1. Same setup as Step 2
2. Collect 30 minutes of new data
3. Compare corrected test values to reference

**Acceptance criteria:**
- Mean error: < 5%
- Standard deviation: < 0.3 m/s
- No systematic bias

**If fail:** Repeat calibration or check sensor condition (bearings, alignment).

---

## Method 2: Wind Tunnel Calibration **(Most Accurate)**

**Accuracy:** ±1-2%  
**Cost:** High (tunnel rental) or Free (university facility)  
**Time:** 4-8 hours

### **Equipment Needed**

- Wind tunnel with controllable fan speed
- Reference velocity measurement (Pitot tube or hot-wire anemometer)
- Data logger
- Calibration certificate for reference instrument

---

### **Procedure**

1. **Mount anemometer in tunnel:**
   - Center of test section
   - Avoid boundary layer effects (>30 cm from walls)
   - Align with flow direction

2. **Run calibration matrix:**

| Set Point (m/s) | Reference (m/s) | Test Reading (V) | Test (m/s) | Error (%) |
|-----------------|-----------------|------------------|------------|-----------|
| 0.0 | 0.0 | 0.42 | 0.5 | - |
| 3.0 | 2.98 | 1.15 | 3.2 | +7.4% |
| 5.0 | 5.02 | 1.85 | 5.5 | +9.6% |
| 7.0 | 6.95 | 2.50 | 7.2 | +3.6% |
| 10.0 | 9.98 | 3.45 | 10.3 | +3.2% |
| 15.0 | 14.96 | 5.10 | 15.2 | +1.6% |
| 20.0 | 19.89 | 6.75 | 20.1 | +1.1% |

**Repeat each point 3 times, average results.**

3. **Generate calibration curve:**
   - Multi-point polynomial fit (2nd or 3rd order if non-linear)
   - Document equation in calibration certificate

---

## Method 3: Vehicle Method **(Low-Cost Alternative)**

**Accuracy:** ±10% (sufficient for preliminary tests only)  
**Cost:** Free  
**Time:** 1-2 hours

⚠️ **Warning:** Less accurate than other methods. Use only if no alternatives available.

---

### **Procedure**

1. **Mount anemometer on vehicle roof:**
   - Center of roof
   - Extend above roof by 50 cm (minimize car's flow disturbance)
   - Secure firmly

2. **Select test route:**
   - Long straight road (>2 km)
   - Minimal traffic
   - No wind day (< 2 m/s ambient)
   - Flat terrain

3. **Collect data:**

| Vehicle Speed (km/h) | GPS Speed (m/s) | Anemometer Reading (m/s) | Error |
|----------------------|-----------------|--------------------------|-------|
| 20 | 5.56 | 6.2 | +11.5% |
| 40 | 11.11 | 12.1 | +8.9% |
| 60 | 16.67 | 17.8 | +6.8% |

**Run each speed 5 times (forward and backward to cancel wind).**

4. **Calculate average:**
   - Average forward and backward runs at each speed
   - Plot calibration curve

**Limitations:**
- Turbulence from vehicle body
- Bearing friction at low speeds
- Difficult to achieve steady speeds
- GPS drift at low speeds

**Use only for rough calibration check!**

---

## Calibration Certificate Template

Document all calibrations in `calibration_datasheet_template.xlsx` with these sections:

### **Section 1: Device Information**
- Sensor model: (e.g., Inspeed Vortex Wind Sensor)
- Serial number: (if available)
- Date of manufacture:
- Purchase date:
- Installation date:

### **Section 2: Calibration Details**
- Calibration date:
- Performed by:
- Method used: (Reference / Wind tunnel / Vehicle)
- Reference instrument: (Model, serial, last cal date)
- Location: (GPS coordinates)

### **Section 3: Environmental Conditions**
- Temperature: 28.5°C
- Humidity: 75% RH
- Pressure: 1013 hPa
- Wind conditions: Steady, 5-12 m/s

### **Section 4: Results**
- Calibration equation: `v_true = (v_measured - 0.1) / 1.05`
- R² value: 0.982
- Standard error: 0.15 m/s
- Valid range: 3-20 m/s

### **Section 5: Acceptance Criteria**
- [ ] R² > 0.95
- [ ] Standard error < 0.3 m/s
- [ ] Mean absolute error < 5%

**Sign-off:**
- Technician signature: _____________
- Supervisor signature: _____________

---

## Troubleshooting

### **Problem: Inconsistent readings (high scatter)**

**Possible causes:**
1. **Turbulent wind conditions:**
   - Solution: Wait for steadier wind, try different time of day
   
2. **Bearing friction:**
   - Test: Spin cups by hand, should rotate >10 seconds
   - Solution: Clean bearings or replace anemometer

3. **Electrical noise:**
   - Check: Use oscilloscope to view anemometer signal
   - Solution: Add capacitor (0.1 µF) across signal wires

---

### **Problem: Readings always too high or too low**

**Systematic offset:**
- Check ADC reference voltage (should be 3.3V)
- Verify voltage divider circuit (if used)
- Check anemometer specifications (some output 0-5V, others 0-40mV/mph)

---

### **Problem: Non-linear response**

**Expected:** Most cup anemometers have slight non-linearity

**Solution:**
- Use 2nd-order polynomial fit instead of linear:
  ```
  v_true = a × v_measured² + b × v_measured + c
  ```
- Or use lookup table for correction

---

## Measurement Uncertainty

### **Sources of Uncertainty:**

| Source | Typical Value | Effect on Cp |
|--------|---------------|--------------|
| Reference anemometer | ±2% | ±6% on Cp |
| Mounting alignment | ±1° | ±0.5% |
| Data sampling | ±0.1 m/s | ±3% at v=3m/s |
| Environmental (temp, density) | ±1% | ±1% |
| **Combined (quadrature sum)** | **±5%** | **±15% on Cp** |

**Formula:**
\[ u_{\text{combined}} = \sqrt{u_1^2 + u_2^2 + u_3^2 + \ldots} \]

---

## Calibration Schedule

### **Recommended Frequency:**

| Condition | Calibration Interval |
|-----------|---------------------|
| **New anemometer** | Before first use |
| **Normal use (inland)** | Every 6 months |
| **Coastal/tropical use** | Every 3 months |
| **After bearing service** | Immediately |
| **After damage/drop** | Immediately |
| **Readings seem suspicious** | As soon as possible |

---

## Best Practices

1. **Always calibrate in-situ if possible** (same mounting, same location)
2. **Document everything:** Photos, weather conditions, equipment details
3. **Keep calibration history:** Track drift over time
4. **Compare to manufacturer specs:** Deviations >10% may indicate damage
5. **Use same ADC settings:** Match sampling rate, averaging to actual deployment

---

## Example Calculation

**Given:**
- Test anemometer reading: 8.2 m/s
- Calibration equation: v_true = (v_test - 0.15) / 1.08

**Calculate corrected wind speed:**

\[ v_{\text{true}} = \frac{8.2 - 0.15}{1.08} = \frac{8.05}{1.08} = 7.45 \text{ m/s} \]

**Impact on power calculation:**

\[ P_{\text{available}} = \frac{1}{2} \times 1.2 \times 2.4 \times 7.45^3 = 590 \text{ W} \]

Without calibration (using 8.2 m/s):

\[ P_{\text{available}} = \frac{1}{2} \times 1.2 \times 2.4 \times 8.2^3 = 792 \text{ W} \]

**Error:** 34% overestimate in available power!

---

## References

- IEC 61400-12-2:2013 - Power performance of small wind turbines
- ISO 17713-1:2007 - Meteorological instruments — Anemometers
- ASTM D5096-17 - Standard test method for determining calibration of anemometers

---

## Related Documents

- [`power_sensor_calibration.md`](power_sensor_calibration.md)
- [`temperature_sensor_calibration.md`](temperature_sensor_calibration.md)
- [`../best_practices/data_quality_assurance.md`](../best_practices/data_quality_assurance.md)

---

**Questions?** Open an issue: https://github.com/asithakal/wind-turbine-daq-guide/issues