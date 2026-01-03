# Comprehensive Fault Diagnosis Table

**Last updated:** 2026-01-03  
**Source:** Field experience, student feedback, laboratory testing

This table consolidates common issues across all system components. For detailed troubleshooting procedures, see individual guides in this folder.

**Quick navigation:**
- [Sensor Issues](#sensor-issues) (6 faults)
- [Data Storage Issues](#data-storage-issues) (3 faults)
- [Communication Issues](#communication-issues) (4 faults)
- [Power Supply Issues](#power-supply-issues) (3 faults)
- [Environmental Issues](#environmental-issues) (4 faults)
- [Firmware Issues](#firmware-issues) (5 faults)

---

## Sensor Issues

| # | Symptom | Probable Cause | Quick Fix | Long-term Solution | Time | Related Guide |
|---|---------|----------------|-----------|-------------------|------|---------------|
| S1 | Anemometer reads constant 25 m/s | Open circuit in cable | Check cable continuity with multimeter | Replace cable or anemometer | 30 min | [hardware_debugging.md](hardware_debugging.md) |
| S2 | Anemometer reads 10-15% low vs. reference | Bearing friction from salt/humidity | Clean and re-lubricate bearings | Replace with ultrasonic anemometer | 1 hour | [../calibration/anemometer_calibration.md](../calibration/anemometer_calibration.md) |
| S3 | Rotor speed reads zero while turbine rotating | Magnet-sensor gap too large or sensor failed | Adjust gap to 3-5 mm; test with multimeter | Replace Hall sensor ($5) | 20 min | [hardware_debugging.md](hardware_debugging.md) |
| S4 | Power sensor negative readings | Current sensor polarity reversed | Swap sensor wire direction | Update firmware with polarity correction | 10 min | [../calibration/power_sensor_calibration.md](../calibration/power_sensor_calibration.md) |
| S5 | BME280 not detected | I2C wiring issue or wrong address | Run I2C scanner; check address (0x76/0x77) | Replace sensor module | 15 min | [hardware_debugging.md](hardware_debugging.md) |
| S6 | Voltage readings fluctuate wildly | ADC noise from poor grounding | Add 0.1 µF capacitor across signal lines | Shielded cables, PCB redesign | 30 min | [hardware_debugging.md](hardware_debugging.md) |

---

## Data Storage Issues

| # | Symptom | Probable Cause | Quick Fix | Long-term Solution | Time | Related Guide |
|---|---------|----------------|-----------|-------------------|------|---------------|
| D1 | SD card not detected | Poor contact or incompatible format | Re-seat card; reformat as FAT32 | Use industrial-grade SD card | 15 min | [sd_card_problems.md](sd_card_problems.md) |
| D2 | Corrupted CSV files | Power loss during write | Implement write buffering (10 records) | Add UPS/battery backup | 2 hours | [sd_card_problems.md](sd_card_problems.md) |
| D3 | SD card fills up quickly | No file rotation enabled | Enable daily file rotation in config.txt | Implement automatic old file deletion | 30 min | [sd_card_problems.md](sd_card_problems.md) |

---

## Communication Issues

| # | Symptom | Probable Cause | Quick Fix | Long-term Solution | Time | Related Guide |
|---|---------|----------------|-----------|-------------------|------|---------------|
| C1 | WiFi won't connect | Wrong credentials or weak signal | Verify SSID/password; check RSSI (-60 dBm min) | Install WiFi repeater or external antenna | 1 hour | [mqtt_connectivity.md](mqtt_connectivity.md) |
| C2 | MQTT data not reaching cloud | Broker credentials wrong or port blocked | Test with mosquitto_pub tool on computer | Switch to HTTPS POST if MQTT blocked | 2 hours | [mqtt_connectivity.md](mqtt_connectivity.md) |
| C3 | WiFi connects but no internet | Router firewall or DNS issue | Test ping to 8.8.8.8; check router settings | Configure static DNS (8.8.8.8) | 1 hour | [mqtt_connectivity.md](mqtt_connectivity.md) |
| C4 | Connection drops every few minutes | Power-saving mode enabled | Disable WiFi sleep mode in config.txt | Upgrade to external antenna | 30 min | [mqtt_connectivity.md](mqtt_connectivity.md) |

---

## Power Supply Issues

| # | Symptom | Probable Cause | Quick Fix | Long-term Solution | Time | Related Guide |
|---|---------|----------------|-----------|-------------------|------|---------------|
| P1 | Random ESP32 reboots | Brownout from insufficient power | Add 1000 µF capacitor across Vin-GND | Upgrade to 5V/2A regulated supply | 30 min | [hardware_debugging.md](hardware_debugging.md) |
| P2 | System works on USB, fails on battery | Voltage drop under load | Check battery voltage (>11V for 12V system) | Replace battery or add voltage regulator | 1 hour | [hardware_debugging.md](hardware_debugging.md) |
| P3 | Turbine power readings are zero | INA226 not powered or misconfigured | Check VCC connection (3.3V or 5V) | Verify I2C address and shunt configuration | 30 min | [../calibration/power_sensor_calibration.md](../calibration/power_sensor_calibration.md) |

---

## Environmental Issues

| # | Symptom | Probable Cause | Quick Fix | Long-term Solution | Time | Related Guide |
|---|---------|----------------|-----------|-------------------|------|---------------|
| E1 | Enclosure overheating (>60°C) | Direct sun, no ventilation | Relocate to shade; add aluminum heat sinks | Install thermostat-controlled fan | 1 hour | [../best_practices/environmental_protection.md](../best_practices/environmental_protection.md) |
| E2 | Moisture inside enclosure | Failed gasket or cable gland seal | Dry components; tighten cable glands; add desiccant | Upgrade to IP67 enclosure with better seals | 2 hours | [../best_practices/environmental_protection.md](../best_practices/environmental_protection.md) |
| E3 | Sensor readings erratic in rain | Water ingress in sensor connectors | Apply dielectric grease to connectors | Use marine-grade sealed connectors | 1 hour | [../best_practices/environmental_protection.md](../best_practices/environmental_protection.md) |
| E4 | UV damage to enclosure (cracking) | Non-UV-resistant material | Apply UV-resistant coating temporarily | Replace with polycarbonate enclosure | 3 hours | [../best_practices/environmental_protection.md](../best_practices/environmental_protection.md) |

---

## Firmware Issues

| # | Symptom | Probable Cause | Quick Fix | Long-term Solution | Time | Related Guide |
|---|---------|----------------|-----------|-------------------|------|---------------|
| F1 | ADC noise (±10% fluctuation) | Poor grounding or EMI | Implement 10-sample averaging filter | Shielded cables, PCB layout improvements | 1 hour | [common_errors.md](common_errors.md) |
| F2 | System crashes after hours | Memory leak or watchdog timeout | Monitor heap with ESP.getFreeHeap() | Review code for dynamic allocation issues | 4 hours | [common_errors.md](common_errors.md) |
| F3 | Compilation errors after library update | API changes in new library version | Revert to previous library version | Update code to match new API | 2 hours | [common_errors.md](common_errors.md) |
| F4 | Upload fails: "Board not found" | Driver issue or wrong COM port | Check Device Manager; reinstall drivers | Use different USB cable (data, not charge-only) | 30 min | [../setup_guides/02_esp32_driver_installation.md](../setup_guides/02_esp32_driver_installation.md) |
| F5 | Sensor values show -999 or NaN | Sensor initialization failed | Check wiring and I2C scanner output | Add error handling and sensor reset logic | 2 hours | [common_errors.md](common_errors.md) |

---

## Calibration Issues

| # | Symptom | Probable Cause | Quick Fix | Long-term Solution | Time | Related Guide |
|---|---------|----------------|-----------|-------------------|------|---------------|
| L1 | Sensor drift (>5% over 3 months) | Environmental stress, component aging | Recalibrate against reference instrument | Quarterly calibration schedule | 3 hours | [../calibration/](../calibration/) + Book Appendix C |
| L2 | High scatter in calibration data (R² < 0.95) | Turbulent wind conditions | Repeat calibration in better conditions | Use wind tunnel calibration | 4 hours | [../calibration/anemometer_calibration.md](../calibration/anemometer_calibration.md) |
| L3 | Power readings consistently 20% off | Shunt resistor mismatch in firmware | Measure actual shunt with multimeter; update code | Order correct shunt resistor | 1 hour | [../calibration/power_sensor_calibration.md](../calibration/power_sensor_calibration.md) |

---

## Contributing

Found a new issue or better solution? 

**Add to this table:**
1. Fork repository
2. Add row to appropriate category
3. Submit Pull Request with description

**Include:**
- Clear symptom description
- Root cause analysis
- Tested solution
- Estimated time to fix
