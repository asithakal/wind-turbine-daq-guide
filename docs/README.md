# Documentation

Comprehensive guides for setup, calibration, troubleshooting, and best practices.

## 📂 Folder Contents

### **`setup_guides/`**
Step-by-step installation instructions for beginners.

| Guide | Covers | Estimated Time |
|-------|--------|----------------|
| `01_arduino_ide_setup.md` | Install Arduino IDE 2.x | 15 minutes |
| `02_esp32_driver_installation.md` | Install ESP32 board support + USB drivers | 20 minutes |
| `03_library_installation.md` | Install required Arduino libraries | 30 minutes |
| `04_first_upload.md` | Upload first sketch to ESP32 | 15 minutes |

**Total:** ~1.5 hours to go from zero to working ESP32 setup.

### **`calibration/`**
Sensor calibration procedures (critical for research-grade data).

| Procedure | Sensor | Method | Equipment Needed |
|-----------|--------|--------|------------------|
| `anemometer_calibration.md` | Wind speed | Reference comparison | Calibrated reference anemometer or wind tunnel |
| `power_sensor_calibration.md` | INA226 | Two-point calibration | Precision resistors, multimeter |
| `temperature_sensor_calibration.md` | BME280 | Ice-point/boiling-point | Thermometer, ice bath, boiling water |
| `calibration_datasheet_template.xlsx` | (All) | Record template | Excel |

**Recommended frequency:** Every 3 months for long-term campaigns.

### **`troubleshooting/`**
Problem-solving guides for common issues.

| Guide | Addresses | Symptoms |
|-------|-----------|----------|
| `common_errors.md` | Firmware compilation/upload errors | "Board not found", library errors |
| `hardware_debugging.md` | Sensor wiring, power issues | Sensors return -999, system resets |
| `sd_card_problems.md` | SD card detection, file corruption | "SD card not found", incomplete data |
| `mqtt_connectivity.md` | Wi-Fi and cloud connection issues | "Connection timeout", data not appearing on dashboard |

### **`best_practices/`**
Advanced guidance for professional implementation.

- `firmware_design_patterns.md`: Code organization, state machines, error handling
- `environmental_protection.md`: Weatherproofing for tropical/coastal climates
- `data_quality_assurance.md`: QA/QC procedures, outlier detection
- `fair_data_publication.md`: FAIR principles implementation checklist

---

## 🚀 Getting Started (Complete Setup Path)

### **New to ESP32? Start Here:**

Follow setup guides in order:

**Week 1: Environment Setup**
1. **Day 1:** [`setup_guides/01_arduino_ide_setup.md`](setup_guides/01_arduino_ide_setup.md)
2. **Day 2:** [`setup_guides/02_esp32_driver_installation.md`](setup_guides/02_esp32_driver_installation.md)
3. **Day 3:** [`setup_guides/03_library_installation.md`](setup_guides/03_library_installation.md)
4. **Day 4:** [`setup_guides/04_first_upload.md`](setup_guides/04_first_upload.md)

**Week 2-3: Firmware Examples**  
Work through [`../firmware/examples/`](../firmware/examples/) (01-08)

**Week 4: Hardware Assembly**  
Build circuit per [`../hardware/schematics/`](../hardware/schematics/)

**Week 5: Calibration**  
Calibrate sensors per [`calibration/`](calibration/) procedures

**Week 6: Deployment**  
Install on turbine, start data collection

---

## 📚 Documentation by Use Case

### **Use Case 1: "I'm building the system from scratch"**

**Read in this order:**
1. [`setup_guides/01_arduino_ide_setup.md`](setup_guides/01_arduino_ide_setup.md)
2. [`../hardware/README.md`](../hardware/README.md)
3. [`../firmware/examples/README.md`](../firmware/examples/README.md)
4. [`calibration/anemometer_calibration.md`](calibration/anemometer_calibration.md)

---

### **Use Case 2: "My system is built but sensors aren't working"**

**Troubleshooting flowchart:**

```
Sensor returns -999 or NaN?
  ├─→ Check wiring: `troubleshooting/hardware_debugging.md`
  ├─→ Check I2C address: `troubleshooting/common_errors.md` (Section 3.2)
  └─→ Test with example code: `../firmware/examples/03_i2c_sensors/`

System resets randomly?
  ├─→ Power supply issue: `troubleshooting/hardware_debugging.md` (Section 2)
  └─→ Watchdog timeout: `troubleshooting/common_errors.md` (Section 5.1)

SD card not detected?
  ├─→ Format issue: `troubleshooting/sd_card_problems.md` (Section 1)
  ├─→ Wiring issue: Check CS pin in `../hardware/schematics/pin_assignment.csv`
  └─→ Card compatibility: `troubleshooting/sd_card_problems.md` (Section 2)

Wi-Fi won't connect?
  ├─→ Credentials: Check SSID/password in `config.h`
  ├─→ Signal strength: `troubleshooting/mqtt_connectivity.md` (Section 3)
  └─→ Router firewall: `troubleshooting/mqtt_connectivity.md` (Section 4)
```

---

### **Use Case 3: "I need to calibrate my sensors"**

**Calibration priority order:**

| Priority | Sensor | Impact on Cp Accuracy | Guide |
|----------|--------|------------------------|-------|
| **HIGH** | Wind speed (anemometer) | Direct (cubed relationship) | [`anemometer_calibration.md`](calibration/anemometer_calibration.md) |
| **HIGH** | Power (INA226) | Direct (linear) | [`power_sensor_calibration.md`](calibration/power_sensor_calibration.md) |
| **MEDIUM** | Rotor RPM (Hall sensor) | Indirect (via λ) | Built-in (pulse counting) |
| **LOW** | Temperature/Pressure | Indirect (via air density) | [`temperature_sensor_calibration.md`](calibration/temperature_sensor_calibration.md) |

**Quick calibration (1 day):**
- Anemometer: 2-point check (0 wind, known wind speed)
- Power sensor: Compare to multimeter readings

**Full calibration (3-5 days):**
- Multi-point anemometer curve (5-10 wind speeds)
- Environmental sensors against reference standards
- Document in `calibration_datasheet_template.xlsx`

---

### **Use Case 4: "I'm deploying in harsh conditions"**

**Read:**
1. [`best_practices/environmental_protection.md`](best_practices/environmental_protection.md)
2. [`../hardware/enclosure/`](../hardware/enclosure/) assembly photos

**Key takeaways:**
- Use IP67 enclosure minimum (IP68 for submersible)
- Apply conformal coating to PCB (protects against humidity)
- Add desiccant packs (replace monthly)
- Stainless steel fasteners only (Grade 316 for coastal)
- Downward-facing cable entries (prevent water pooling)
- UV-resistant enclosure material (polycarbonate, not ABS)

**Specific climates:**

| Climate | Additional Measures |
|---------|---------------------|
| **Tropical/Coastal** (Sri Lanka, Southeast Asia) | Salt spray protection, fungus-resistant sealant |
| **Arctic/Alpine** | Heater for electronics, low-temp batteries |
| **Desert** | Dust filters, heat sinks, shade for electronics |
| **Urban** | EMI shielding, vibration dampeners |

---

## 🔬 Quality Assurance Procedures

### **Data Quality Checklist (Daily)**

From [`best_practices/data_quality_assurance.md`](best_practices/data_quality_assurance.md):

**Automated checks (run weekly):**
```
# Using analysis script
python ../analysis/python_scripts/01_data_quality_check.py \
  --input latest_week.csv
```

**Manual checks:**
- [ ] Visual inspection of time-series plots
- [ ] No extended flat lines (sensor stuck)
- [ ] No impossible values (Cp > 0.6, negative power)
- [ ] Timestamp continuity (no large gaps)
- [ ] SD card storage remaining > 20%

**Monthly checks:**
- [ ] Sensor drift check (compare to baseline)
- [ ] Enclosure inspection (water ingress, corrosion)
- [ ] Cable condition (chafing, UV damage)
- [ ] Data backup to secondary location

---

## 🎓 Academic Best Practices

### **For Thesis/Dissertation Quality**

Follow [`best_practices/fair_data_publication.md`](best_practices/fair_data_publication.md):

**Essential documentation:**
1. **Traceability:** Requirements → Design → Tests (use RTM template)
2. **Reproducibility:** Full sensor specs, calibration certs
3. **Uncertainty:** Quantified for all measurements
4. **Provenance:** Who, what, when, where, how

**Red flags reviewers look for:**
- ❌ Missing calibration certificates
- ❌ Undefined "raw data" vs "processed data"
- ❌ No uncertainty analysis
- ❌ Unexplained data gaps

**Gold standard:**
- ✅ Pre-registered test plan
- ✅ Continuous quality logs
- ✅ Peer-reviewed methodology
- ✅ Open data + open code

---

## 🛠️ Advanced Topics

### **Firmware Design Patterns**

From [`best_practices/firmware_design_patterns.md`](best_practices/firmware_design_patterns.md):

**Key concepts:**
- **State machines:** INIT → ACQUIRE → PROCESS → LOG → TRANSMIT
- **Error handling:** Fail-safe defaults, watchdog timers
- **Modular code:** Separate files for sensors, logging, networking
- **Memory management:** Avoid dynamic allocation, use ring buffers

**Example:** Implementing graceful SD card failure handling

```
// Bad practice: Crash on SD failure
void logData() {
  File dataFile = SD.open("data.csv", FILE_WRITE);
  dataFile.println(csvRow);  // CRASH if SD failed!
  dataFile.close();
}

// Good practice: Continue with RAM buffer, retry later
void logData() {
  if (SD_card_available) {
    File dataFile = SD.open("data.csv", FILE_WRITE);
    if (dataFile) {
      dataFile.println(csvRow);
      dataFile.close();
    } else {
      bufferData(csvRow);  // Store in RAM
      retrySD();
    }
  } else {
    bufferData(csvRow);
  }
}
```

---

## 🔗 External Resources

### **Official Documentation**
- **ESP32:** https://docs.espressif.com/projects/arduino-esp32/
- **Arduino Reference:** https://www.arduino.cc/reference/
- **PlatformIO:** https://docs.platformio.org/

### **Standards**
- **IEC 61400-12-2:** Power performance of small wind turbines
- **IEC 61400-1:** Design requirements for wind turbines
- **ISO GUM:** Guide to expression of uncertainty in measurement

### **Communities**
- **Arduino Forum:** https://forum.arduino.cc/
- **ESP32 Reddit:** https://www.reddit.com/r/esp32/
- **Stack Overflow:** Tag `[esp32]` for specific questions

---

## 🆘 Getting Help

### **When Stuck:**

**1. Search existing issues:**  
https://github.com/asithakal/wind-turbine-daq-guide/issues

**2. Check troubleshooting docs:**  
[`troubleshooting/common_errors.md`](troubleshooting/common_errors.md) (covers 90% of issues)

**3. Open a new issue:**  
https://github.com/asithakal/wind-turbine-daq-guide/issues/new

**Provide:**
- Hardware setup (ESP32 board type, sensor models)
- Firmware version or example number
- Complete error message (copy-paste from Serial Monitor)
- What you've already tried

**Response time:** Typically 1-3 days

---

## 📖 Related Documentation

- **Main Repository README:** [`../README.md`](../README.md)
- **Firmware Examples:** [`../firmware/README.md`](../firmware/README.md)
- **Hardware Assembly:** [`../hardware/README.md`](../hardware/README.md)
- **Data Analysis:** [`../analysis/README.md`](../analysis/README.md)

---

## 🤝 Contributing

Found an error? Have a better troubleshooting tip?

Submit documentation improvements via [Pull Request](https://github.com/asithakal/wind-turbine-daq-guide/pulls).

**Especially welcome:**
- Troubleshooting guides for new issues
- Regional adaptations (e.g., "Setup guide for Raspberry Pi alternative")
- Translation to other languages

---

## 📜 License

Documentation: CC BY 4.0 (Creative Commons Attribution)

See [LICENSE](../LICENSE) for details.