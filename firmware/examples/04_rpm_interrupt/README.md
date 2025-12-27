# Example 04: RPM Measurement with Interrupts

## Purpose

Measure rotor rotational speed (RPM) using a Hall effect sensor and hardware interrupts.

## Learning Objectives

- Use hardware interrupts (`attachInterrupt()`)
- Implement Interrupt Service Routines (ISRs)
- Understand `volatile` keyword
- Count pulses over time windows
- Apply debouncing to prevent false triggers
- Convert pulse frequency to RPM

## Hardware Required

- ESP32 board
- Hall effect sensor (digital output type):
  - **Honeywell SS441A** (unipolar)
  - **A3144** (unipolar)
  - **OH137** (latching type)
- Neodymium magnet (10mm diameter recommended)
- 10kΩ resistor (pull-up, if sensor is open-collector)
- Breadboard and jumper wires

## Sensor Types

### **Digital Hall Sensors:**

| Type | Behavior | Best For |
|------|----------|----------|
| **Unipolar** (SS441A) | LOW when S-pole near, HIGH otherwise | Simple, 1 magnet per revolution |
| **Bipolar** (SS461A) | LOW with S-pole, HIGH with N-pole | Reversible rotation detection |
| **Latching** (OH137) | Toggles on each S-pole pass | Not recommended (harder to use) |

**Recommendation:** Use unipolar sensor (SS441A) for simplicity.

## Circuit Diagram

```
Hall Sensor (SS441A)                    ESP32
    
    VCC (Pin 1) ──────────────────→ 3.3V or 5V
    
    OUT (Pin 2) ───┬──────────────→ GPIO 15
                   │
                 10kΩ (pull-up)
                   │
                   └──────────────→ 3.3V
    
    GND (Pin 3) ──────────────────→ GND

Magnet: Mount on rotor shaft, 1-5mm from sensor face
```

**Critical:** Magnet must pass within 5mm of sensor for reliable detection.

## Connections

| Component | ESP32 Pin | Notes |
|-----------|-----------|-------|
| Hall Sensor VCC | 3.3V or 5V | Check sensor datasheet |
| Hall Sensor OUT | GPIO 15 | Interrupt-capable pin |
| Hall Sensor GND | GND | |
| Pull-up resistor | GPIO 15 to 3.3V | Only if sensor is open-collector |

### **Why GPIO 15?**

- Supports hardware interrupts
- No boot mode conflicts (unlike GPIO 0, 2)
- Not used by WiFi (unlike GPIO 4, 12-15 on some boards)

**Alternative pins:** GPIO 13, 14, 16, 17, 18, 19, 23, 25-27, 32-39

**Avoid:** GPIO 34-39 (input-only, but work for interrupts)

## Expected Output

**Serial Monitor (115200 baud):**

```
=================================
RPM Measurement - Example 04
=================================

Configuration:
  Pin: GPIO 15
  Pulses per revolution: 1
  Calculation period: 1000 ms

Spin the rotor to measure RPM...

Time(s) RPM     Revolutions     Pulse Period(ms)
-------------------------------------------------------
1.0     0.0     0               0.00
2.0     45.2    0               1326.99
3.0     52.8    1               1137.56
4.0     58.3    2               1028.93
5.0     62.1    3               966.18
...
```

**Interpretation:**
- **RPM:** Revolutions per minute (0 = stopped)
- **Revolutions:** Total count since startup
- **Pulse Period:** Average time between pulses (ms)

## Testing Without Turbine

### **Method 1: Manual Testing**

1. Upload code
2. Open Serial Monitor
3. Wave magnet past sensor repeatedly
4. Should see RPM calculations based on wave speed

**Expected:** ~60 RPM if you wave once per second

### **Method 2: Drill Test**

1. Tape magnet to drill bit
2. Mount sensor near spinning bit (careful!)
3. Low drill speed = low RPM reading

**Safety:** Keep fingers clear, secure sensor well!

### **Method 3: Frequency Generator**

Connect function generator to GPIO 15:
- Set to square wave, 1 Hz = 60 RPM
- Amplitude: 0-3.3V
- Verify RPM calculation accuracy

## Troubleshooting

### **Problem: RPM always reads 0.00**

**Causes:**
1. Magnet too far from sensor (> 5mm)
2. Magnet not moving
3. Sensor not powered
4. Wrong pin in code

**Debug:**

```
// Add to loop():
Serial.print("Pin state: ");
Serial.println(digitalRead(HALL_SENSOR_PIN));
// Should toggle HIGH/LOW when magnet passes
```

**Solution:**
- Move magnet closer (< 3mm ideal)
- Check VCC is 3.3V or 5V
- Verify sensor type (must be digital output, not analog)

---

### **Problem: RPM fluctuates wildly (e.g., 45 → 120 → 30 RPM)**

**Cause:** Mechanical vibration causing false triggers

**Solutions:**

1. **Increase debounce time:**
   ```
   const unsigned long DEBOUNCE_DELAY_US = 5000; // 5ms instead of 1ms
   ```

2. **Add hysteresis capacitor:**
   - 100nF ceramic capacitor between GPIO 15 and GND
   - Filters high-frequency noise

3. **Check magnet alignment:**
   - Magnet should pass sensor perpendicular
   - Not wobbling or vibrating

---

### **Problem: RPM reading is half/double actual speed**

**Cause:** Wrong pulses per revolution setting

**If RPM is double:**
```
const int PULSES_PER_REV = 2; // You have 2 magnets, not 1!
```

**If RPM is half:**
- Check if sensor is bipolar (triggers on N and S poles)
- May be counting both N and S as separate pulses

---

### **Problem: "Guru Meditation Error: Core 1 panic'ed"**

**Cause:** ISR accessing non-IRAM functions

**Solution:** Ensure ISR has `IRAM_ATTR`:
```
void IRAM_ATTR hallSensorISR() {
  // ISR code
}
```

This stores function in RAM for faster access during interrupt.

## Understanding Interrupts

### **What is an Interrupt?**

Normal code execution is **interrupted** when event occurs → ISR runs → code resumes.

**Without interrupts:**
```
void loop() {
  if (digitalRead(PIN) == LOW) {  // Polling - might miss fast pulses
    count++;
  }
}
```

**With interrupts:**
```
void IRAM_ATTR ISR() {
  count++;  // Runs IMMEDIATELY when pin goes LOW
}
```

### **Interrupt Triggers:**

```
RISING   // LOW → HIGH transition
FALLING  // HIGH → LOW transition  (used in this example)
CHANGE   // Any transition
LOW      // While pin is LOW (avoid - can trigger many times)
HIGH     // While pin is HIGH (avoid - can trigger many times)
```

### **Why FALLING?**

Hall sensor (SS441A) outputs:
- **HIGH (3.3V)** when no magnet
- **LOW (0V)** when magnet near

**FALLING** triggers once per magnet pass = 1 revolution.

## Volatile Variables

```
volatile unsigned long pulseCount = 0;
```

**Why `volatile`?**

Tells compiler: "This variable can change outside normal code flow (via interrupt)."

Without `volatile`, compiler might optimize code incorrectly, thinking variable never changes.

**Rule:** Any variable modified in ISR **must** be `volatile`.

## RPM Calculation Explained

### **Formula:**

```
RPM = (pulses / pulses_per_rev) × (60 seconds / measurement_period)
```

### **Example:**

- **10 pulses** counted in **1 second**
- **1 pulse per revolution** (1 magnet)

```
RPM = (10 / 1) × (60 / 1) = 600 RPM
```

### **Why disable interrupts during read?**

```
noInterrupts();
unsigned long pulses = pulseCount;
pulseCount = 0;
interrupts();
```

Prevents ISR from modifying `pulseCount` while we're reading it (data corruption).

**Without protection:**
```
Main code reads pulseCount = 5
→ Interrupt fires during read
→ ISR increments to 6
→ Main code gets corrupted value (maybe 5, maybe 6, maybe garbage)
```

## Debouncing

### **What is Bouncing?**

Mechanical switches and sensors don't transition cleanly:

```
Signal:  ─────┐     ┌─┐ ┌───
              └─────┘ └─┘     (multiple transitions from bouncing)
              
Desired: ─────┐
              └────────        (single clean transition)
```

### **Solution: Ignore pulses closer than 1ms**

```
if (currentTime_us - lastPulseTime_us > DEBOUNCE_DELAY_US) {
  pulseCount++;  // Valid pulse
}
```

**For very high RPM (> 10,000), reduce debounce:**
```
const unsigned long DEBOUNCE_DELAY_US = 100; // 0.1ms
```

## Code Optimization

### **ISR Best Practices:**

✅ **DO:**
- Keep ISR short (< 10 lines)
- Use `IRAM_ATTR`
- Only modify volatile variables
- Use `micros()` for timing

❌ **DON'T:**
- Call `Serial.println()` in ISR (too slow)
- Use `delay()` in ISR (blocks system)
- Perform calculations in ISR
- Access WiFi/network in ISR

### **Why `IRAM_ATTR`?**

ESP32 normally stores code in flash memory (slow). `IRAM_ATTR` stores ISR in RAM (fast).

**Without it:** ISR might be too slow, causing missed pulses at high RPM.

## Advanced: Multiple Magnets

If you mount **3 magnets** on rotor (120° apart):

```
const int PULSES_PER_REV = 3;  // 3 pulses per revolution
```

**Advantages:**
- Better resolution at low RPM
- More frequent updates

**Disadvantages:**
- Must align magnets precisely
- More complex mechanical setup

## Next Steps

✅ **Example 04 Complete!**

Continue to:
- [Example 05: SD Card Logging](../05_sd_card_basic/) - Store data locally

## Further Reading

- **ESP32 Interrupts:** https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/intr_alloc.html
- **Hall Effect Sensors:** https://sensing.honeywell.com/hallbook.pdf
- **Debouncing Guide:** https://www.allaboutcircuits.com/technical-articles/switch-bounce-how-to-deal-with-it/