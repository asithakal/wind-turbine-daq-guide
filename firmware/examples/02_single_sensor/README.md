# Example 02: Single Sensor - Wind Speed

## Purpose

Read analog voltage from a wind speed sensor (anemometer) and convert it to m/s.

## Learning Objectives

- Read analog pins with `analogRead()`
- Understand ESP32 ADC characteristics (12-bit, 0-3.3V)
- Apply voltage divider calculations
- Implement sensor calibration formulas
- Average multiple samples to reduce noise

## Hardware Required

- ESP32 board
- Wind speed sensor (e.g., Inspeed Vortex, 0-5V output)
- 2× 10kΩ resistors (voltage divider)
- Breadboard and jumper wires

## Circuit Diagram

```
Anemometer (5V output)
    │
    ├─── VCC ──→ ESP32 5V (or external 5V supply)
    ├─── GND ──→ ESP32 GND
    │
    └─── Signal (0-5V)
             │
             ├─── 10kΩ resistor ──→ GPIO 34
             │
             └─── 10kΩ resistor ──→ GND

Result: 5V input becomes 2.5V at GPIO 34 (safe for ESP32)
```

**Why voltage divider?** ESP32 ADC maximum is 3.3V. Anemometer outputs 0-5V, which would damage ESP32. Divider scales 5V → 2.5V.

## Connections

| Component | ESP32 Pin |
|-----------|-----------|
| Anemometer VCC | 5V or VIN |
| Anemometer GND | GND |
| Anemometer Signal (via divider) | GPIO 34 |

**Critical:** GPIO 34 is **ADC1_CH6** - only supports input, no output capability.

## Sensor Calibration

### **Default (Inspeed Vortex):**

- **Offset:** 0.4V at 0 m/s
- **Scale:** 0.2V per m/s increase
- **Formula:** `wind_speed = (voltage - 0.4) / 0.2`

### **Your Sensor Different?**

Modify these lines in code:

```
const float ANEM_OFFSET_VOLTS = 0.4;     // Change to your sensor's 0 m/s voltage
const float ANEM_SCALE_V_PER_MS = 0.2;   // Change to your sensor's V/(m/s) ratio
```

**How to calibrate:**
1. No wind: Measure voltage (this is your offset)
2. Known wind speed: Measure voltage, calculate scale = (V - offset) / wind_speed

## Expected Output

**Serial Monitor (115200 baud):**

```
=================================
Wind Speed Sensor - Example 02
=================================

Sensor Configuration:
  Pin: GPIO 34
  Offset: 0.4 V
  Scale: 0.2 V/(m/s)

Reading wind speed...
Time(s) ADC     Voltage(V)      Wind Speed(m/s)
-----------------------------------------------------
1.0     1241    1.00            3.00
2.0     1448    1.17            3.85
3.0     1034    0.83            2.15
4.0     1655    1.34            4.70
...
```

## Testing Without Anemometer

**Simulate wind speed:**

1. Replace anemometer with potentiometer:
   - Pot terminal 1 → 3.3V
   - Pot terminal 2 (wiper) → GPIO 34
   - Pot terminal 3 → GND

2. Turn knob to change "wind speed"

## Troubleshooting

**Problem: Reading 0.00 m/s constantly**

**Causes:**
- Anemometer not powered (check VCC connection)
- Voltage divider not connected
- Wrong GPIO pin

**Solution:**
```
// Add diagnostic prints:
Serial.print("Raw ADC: ");
Serial.println(analogRead(ANEMOMETER_PIN));
// Should show values between 0-4095
```

---

**Problem: Reading negative wind speed**

**Cause:** Offset voltage too high (sensor reading < 0.4V)

**Solution:** Adjust offset:
```
const float ANEM_OFFSET_VOLTS = 0.3;  // Lower offset
```

---

**Problem: Noisy readings (jumping between values)**

**Causes:**
- Electrical noise on ADC pin
- Unstable power supply

**Solutions:**
1. Increase sample averaging:
   ```
   const int SAMPLE_COUNT = 50;  // More samples = smoother
   ```

2. Add capacitor (100nF ceramic) between GPIO 34 and GND

3. Use shielded cable for sensor connection

---

**Problem: Max reading capped at 2.5V**

**Cause:** Voltage divider working correctly! 5V anemometer output is scaled to 2.5V.

**If you need full 0-3.3V range:** Use 2:1 divider (6.8kΩ + 10kΩ resistors)

## ADC Characteristics (Important!)

ESP32 ADC is **non-linear** and has quirks:

1. **Avoid GPIO 0, 2, 15** - used for boot mode
2. **Use ADC1 pins** (GPIO 32-39) when WiFi is active
3. **ADC2 pins** (GPIO 0, 2, 4, 12-15, 25-27) conflict with WiFi
4. **Effective resolution:** ~10-11 bits (not full 12-bit due to noise)

**For this project:** GPIO 34 (ADC1_CH6) is ideal - no boot conflicts.

## Code Explanation

### **Key Concepts:**

**1. Averaging for noise reduction:**
```
float adc_sum = 0;
for (int i = 0; i < SAMPLE_COUNT; i++) {
  adc_sum += analogRead(ANEMOMETER_PIN);
  delay(SAMPLE_DELAY_MS);
}
float adc_avg = adc_sum / SAMPLE_COUNT;
```
Takes 10 readings, averages them → reduces random noise.

**2. ADC to voltage conversion:**
```
float voltage = (adc_avg / 4095.0) * 3.3 * 2.0;
//               ^^^^^^^^^^^^^^^^   ^^^   ^^^
//               Normalize to 0-1   V_ref Divider ratio
```

**3. Calibration formula:**
```
float wind_speed = (voltage - 0.4) / 0.2;
//                  ^^^^^^^^^^^^^^^
//                  Remove offset, then scale
```

## Next Steps

✅ **Example 02 Complete!**

Continue to:
- [Example 03: I2C Sensors](../03_i2c_sensors/) - Read power and environmental sensors

## Further Reading

- **ESP32 ADC Calibration:** https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/adc_calibration.html
- **Voltage Divider Calculator:** https://www.allaboutcircuits.com/tools/voltage-divider-calculator/