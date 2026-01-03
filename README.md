# Wind Turbine Data Acquisition System Guide
## A Practical Resource for Student Researchers

[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.18093662.svg)](https://doi.org/10.5281/zenodo.18093662)

This repository accompanies the technical guide **"Wind Turbine Data Acquisition Systems: From Requirements Engineering to FAIR Data Publication"** by Dr. Asitha Kulasekera (University of Moratuwa).

### 📚 About This Resource

This repository provides **ready-to-use code, templates, and tools** for undergraduate and postgraduate students building data acquisition systems for small-scale wind turbines (100W-5kW). It emphasizes:

- ✅ Low-cost implementation (<$500 USD)
- ✅ Tropical/coastal climate adaptation (Sri Lankan context)
- ✅ FAIR data principles
- ✅ Systems engineering approach (V-Model)
- ✅ Research-grade data quality

---

### 🚀 Quick Start

#### **For Students Starting a New Project:**
1. **Read the Book First**: Access the full guide [PDF link or publisher]
2. **Hardware Setup**: See [`hardware/README.md`](hardware/README.md) for schematics and BOM
3. **Firmware Upload**: Start with [`firmware/examples/01_blink_test/`](firmware/examples/01_blink_test/)
4. **Data Analysis**: Use scripts in [`analysis/python_scripts/`](analysis/python_scripts/)
5. **Dataset Publication**: Follow templates in [`templates/documentation/`](templates/documentation/)

#### **For Quick Testing:**
```
# Clone repository
git clone https://github.com/yourusername/wind-turbine-daq-guide.git
cd wind-turbine-daq-guide

# Install Python dependencies
pip install -r analysis/requirements.txt

# Test analysis script with sample data
python analysis/python_scripts/01_data_quality_check.py --input data/example_turbine_data/sample_1week.csv
```

---

### 📂 Repository Structure

| Folder | Contents | When to Use |
|--------|----------|-------------|
| **[`firmware/`](firmware/)** | ESP32 Arduino code, examples | Developing your DAQ system |
| **[`hardware/`](hardware/)** | Schematics, BOM, wiring guides | Purchasing parts, assembling hardware |
| **[`analysis/`](analysis/)** | Python scripts for Cp-λ analysis | Processing collected data |
| **[`templates/`](templates/)** | Document templates (README, metadata, lab notebook) | Writing thesis, publishing data |
| **[`docs/`](docs/)** | Setup guides, calibration procedures | Installation, troubleshooting |
| **[`data/`](data/)** | Example datasets, wind resource data | Testing analysis scripts |

---

### 🎯 Key Features

#### **Hardware Design**
- 🔌 ESP32-based microcontroller platform
- 📊 Sensor suite: wind speed, RPM, power, environmental
- 💧 IP67 enclosure design for tropical climates
- 🔋 Battery backup for grid outages

#### **Firmware Capabilities**
- ⏱️ 1 Hz sampling with hardware timers
- 💾 Local-first storage (SD card primary, cloud secondary)
- 🌐 MQTT cloud transmission (InfluxDB/Grafana compatible)
- ⚡ Real-time Cp and λ calculation
- 🚨 Fault detection and watchdog recovery

#### **Data Pipeline**
- ✅ Quality filtering and gap analysis
- 📈 Cp-λ curve generation with uncertainty bands
- 📊 Steady-state identification
- 📄 FAIR-compliant dataset formatting

---

### 📖 Documentation

**Main Guide (Book):**
- [PDF Download](#) - Full technical guide
- [Publisher Website](#) - Purchase print edition

**Repository Docs:**
- [Firmware Setup Guide](docs/setup_guides/)
- [Calibration Procedures](docs/calibration/)
- [Troubleshooting](docs/troubleshooting/)
- [FAIR Data Publication](docs/best_practices/fair_data_publication.md)

---

### 🏆 Example Projects Using This Guide

| Institution | Project | Dataset DOI | Publication |
|-------------|---------|-------------|-------------|
| University of Moratuwa | 500W Helical VAWT | [10.5281/zenodo.XXXX](#) | *Wind Engineering* 2026 |
| *(Add your project here via PR)* | | | |

---

### 🤝 Contributing

Contributions welcome! See [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

**Ways to contribute:**
- 🐛 Report bugs or hardware issues
- 💡 Suggest improvements to firmware/analysis scripts
- 📚 Share your project as a case study
- 🌍 Add supplier information for other countries

---

### 📜 License

- **Code** (firmware, analysis scripts): MIT License
- **Documentation** (templates, guides): CC-BY-4.0
- **Book content**: Copyright © 2025 Dr. Asitha Kulasekera

See [LICENSE](LICENSE) for full details.

---

### 📧 Contact

**Author:** Dr. Asitha Kulasekera  
**Institution:** University of Moratuwa, Sri Lanka  
**Email:** asitha@example.edu  
**ORCID:** [0000-0002-XXXX-XXXX](https://orcid.org/0000-0002-XXXX-XXXX)

**Issues/Questions:** Open an [issue](https://github.com/yourusername/wind-turbine-daq-guide/issues) on GitHub

---

### 🙏 Acknowledgments

This work was supported by:
- Sri Lanka National Science Foundation
- University of Moratuwa Faculty of Engineering

Special thanks to students who tested early versions of this methodology.

---

### 📊 Citation

If you use this resource in your research, please cite:

```
@software{kulasekera2025winddaq,
  author = {Kulasekera, Asitha},
  title = {Wind Turbine Data Acquisition System Guide},
  year = {2025},
  publisher = {GitHub},
  url = {https://github.com/yourusername/wind-turbine-daq-guide},
  doi = {10.5281/zenodo.XXXXXXX}
}
```

---

**⭐ Star this repository if you find it useful!**

[![GitHub stars](https://img.shields.io/github/stars/yourusername/wind-turbine-daq-guide.svg?style=social&label=Star)](https://github.com/yourusername/wind-turbine-daq-guide)
