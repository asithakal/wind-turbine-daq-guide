# Hardware Documentation

This folder contains all hardware design files, schematics, bills of materials, and assembly guides for the wind turbine data acquisition system.

## 📂 Folder Contents

### **`schematics/`**
Complete wiring diagrams and circuit schematics for the ESP32-based DAQ system.

| File | Description | Format |
|------|-------------|--------|
| `system_diagram.fzz` | Fritzing project file (editable) | Fritzing |
| `system_diagram.png` | Full system wiring diagram | PNG |
| `pin_assignment.csv` | ESP32 GPIO pin mapping table | CSV |
| `wiring_guide.md` | Step-by-step wiring instructions | Markdown |

### **`bom/`**
Bills of materials with component specifications and supplier information.

| File | Purpose |
|------|---------|
| `bom_complete.xlsx` | Master BOM with all components, datasheets, and pricing |
| `bom_sri_lanka.csv` | Local suppliers (Unity Plaza, Colombo, etc.) |
| `bom_international.csv` | International sources (AliExpress, Mouser, DigiKey) |

**Total estimated cost:** ~$450 USD (excluding turbine)

### **`enclosure/`**
Weatherproof enclosure design for tropical/coastal environments.

- `enclosure_design.pdf`: Dimensions, ventilation, cable glands
- `mounting_template.pdf`: Drill template for panel mounting
- `assembly_photos/`: Installation reference photos

**Recommended:** IP67-rated polycarbonate box (200×150×100 mm minimum)

### **`datasheets/`**
Links to component datasheets and comparison tables.

- `datasheet_links.md`: Curated links to manufacturer datasheets
- `component_comparison.xlsx`: Alternative sensor options with trade-offs

---

## 🔧 Quick Start

### **1. Purchase Components**

Download [`bom_sri_lanka.csv`](bom/bom_sri_lanka.csv) or [`bom_international.csv`](bom/bom_international.csv) depending on your location.

**Core components:**
- ESP32-WROOM-32 development board
- INA226 current/voltage sensor (I2C)
- BME280 environmental sensor (I2C)
- Inspeed Vortex wind sensor (or equivalent cup anemometer)
- Honeywell SS441A Hall effect sensor
- MicroSD card module
- IP67 enclosure

### **2. Assemble Circuit**

1. Open [`system_diagram.png`](schematics/system_diagram.png)
2. Follow [`wiring_guide.md`](schematics/wiring_guide.md)
3. Reference [`pin_assignment.csv`](schematics/pin_assignment.csv) for GPIO connections

**Tip:** Test each sensor individually before full system integration (see `firmware/examples/`)

### **3. Enclosure Assembly**

1. Print [`mounting_template.pdf`](enclosure/mounting_template.pdf)
2. Drill holes for cable glands and ventilation
3. Mount components using [`assembly_photos/`](enclosure/assembly_photos/) as reference

---

## 🌧️ Tropical Climate Considerations

**Key challenges addressed:**
- ✅ High humidity (75-95% RH): Conformal coating on PCB, desiccant packs
- ✅ Salt spray corrosion: Stainless steel fasteners, marine-grade cable glands
- ✅ UV degradation: UV-resistant enclosure material
- ✅ Heavy rain: IP67 rating with downward-facing cable entries
- ✅ Temperature cycling: Ventilation holes with mesh (prevent insect entry)

See [`../docs/best_practices/environmental_protection.md`](../docs/best_practices/environmental_protection.md) for detailed guidance.

---

## 🔌 Pin Assignments (Quick Reference)

| Sensor | ESP32 GPIO | Connection Type |
|--------|------------|-----------------|
| **Wind Speed (Anemometer)** | GPIO 34 | Analog (ADC1) |
| **Rotor RPM (Hall Sensor)** | GPIO 15 | Digital (Interrupt) |
| **Power Monitor (INA226)** | GPIO 21 (SDA), 22 (SCL) | I2C |
| **Environmental (BME280)** | GPIO 21 (SDA), 22 (SCL) | I2C |
| **SD Card** | GPIO 5 (CS), 18 (SCK), 19 (MISO), 23 (MOSI) | SPI |

**Full pinout:** See [`schematics/pin_assignment.csv`](schematics/pin_assignment.csv)

---

## 📊 Power Budget

| Component | Current Draw | Notes |
|-----------|--------------|-------|
| ESP32 (WiFi active) | ~180 mA | Peaks at 240 mA during transmission |
| INA226 | 1 mA | I2C sensor |
| BME280 | 3.6 µA | I2C sensor (sleep mode) |
| SD Card | 50 mA | During write operations |
| Hall Sensor | 5 mA | Continuous |
| **Total (peak)** | ~240 mA | Requires 5V @ 1A minimum supply |

**Recommended:** 5V/2A power supply or solar panel (10W) + 12V battery + buck converter

---

## 🛠️ Alternative Components

If specific sensors are unavailable, see [`datasheets/component_comparison.xlsx`](datasheets/component_comparison.xlsx) for alternatives:

- **Anemometer:** Davis 6410 / Adafruit 1733 / DIY cup anemometer
- **Current sensor:** INA219 (lower accuracy, cheaper) / ACS712 (hall-effect)
- **Environmental:** DHT22 (lower accuracy) / SHT31 (higher accuracy)

---

## 🚨 Safety Warnings

⚠️ **Electrical Safety:**
- Turbine output can exceed 100V DC at high wind speeds
- Use proper voltage dividers and isolation for power measurement
- Ground all metal enclosures

⚠️ **Installation Safety:**
- Work on turbine only in zero-wind conditions
- Use fall protection when installing sensors above ground
- Secure all cables to prevent wind damage

---

## 📖 Related Documentation

- **Firmware Setup:** [`../firmware/README.md`](../firmware/README.md)
- **Calibration Procedures:** [`../docs/calibration/`](../docs/calibration/)
- **Troubleshooting:** [`../docs/troubleshooting/hardware_debugging.md`](../docs/troubleshooting/hardware_debugging.md)

---

## 🤝 Contributing

Found a better component source? Have a custom PCB design? 

Submit improvements via [Pull Request](https://github.com/asithakal/wind-turbine-daq-guide/pulls) or [Issue](https://github.com/asithakal/wind-turbine-daq-guide/issues).

---

## 📜 License

Hardware designs: CC BY 4.0  
Documentation: CC BY 4.0

See [LICENSE](../LICENSE) for details.