# Advanced Calibration Resources (Research-Grade)

**🔍 Looking for quick calibration?** → See [`../../docs/calibration/`](../../docs/calibration/) for practical, same-day guides

---

## 🎯 Purpose of This Folder

This folder provides **advanced calibration methodologies** for publication-quality research data:

- ✅ **Formal uncertainty analysis** (GUM/ISO methodology)
- ✅ **Automation tools** (Python scripts for data processing)
- ✅ **Extended procedures** (multi-day field calibration protocols)
- ✅ **Templates** (certificates, advanced datasheets, config files)

### **When to Use This vs. `docs/calibration/`**

| **Use `docs/calibration/`** ✓ | **Use `hardware/calibration/`** ✓ |
|--------------------------------|-------------------------------------|
| ✅ Learning to calibrate for first time | ✅ Preparing data for peer-reviewed publication |
| ✅ Quick calibration (same-day) | ✅ Multi-point calibration (3-5 days) |
| ✅ ±5% accuracy acceptable | ✅ Need formal uncertainty quantification |
| ✅ Undergraduate project | ✅ Graduate research / thesis |
| ✅ Code examples needed | ✅ Automation tools needed |
| ✅ Troubleshooting help | ✅ Standards compliance (ISO, IEC, GUM) |

**📌 Most users should start with [`../../docs/calibration/`](../../docs/calibration/)** and come here only if formal uncertainty is required.

---

## 📂 Folder Contents

### **`procedures/`** - Extended Calibration Protocols

| File | Status | Extends | Adds |
|------|--------|---------|------|
| `01-field-calibration-extended.md` | 📝 Planned | `docs/.../anemometer_calibration.md` Method 1 | 24-48h data collection, binned regression, R² validation |
| `02-uncertainty-analysis-gum.md` | 📝 Planned | Simple ±5% rule in `docs/` | Full GUM partial derivatives, combined uncertainty, coverage factors |
| `03-multi-point-calibration.md` | 📝 Planned | Two-point method in `docs/` | 10+ calibration points, polynomial fits, interpolation tables |

**Current status:** Use practical guides in [`../../docs/calibration/`](../../docs/calibration/) then apply tools below for uncertainty.

---

### **`tools/`** - Automation Scripts ✅ *AVAILABLE NOW*

| Script | Status | Purpose | Input | Output |
|--------|--------|---------|-------|--------|
| `uncertainty-calculator.py` | ✅ **Ready** | GUM uncertainty propagation | OFFSET, SCALE, uncertainties | u_c(v), U(v, k=2), contribution breakdown |
| `calibration-curve-fitter.py` | 📝 Planned | Fit calibration data to polynomial | CSV with reference & test readings | Calibration equation, R², residual plot |
| `drift-analyzer.py` | 📝 Planned | Analyze sensor drift over time | Multiple calibration certificates | Drift rate, recalibration schedule |

**Installation:**
```bash
cd hardware/calibration/tools
pip install -r requirements.txt
```

**Test uncertainty calculator:**
```bash
python uncertainty-calculator.py --offset 0.4225 --scale 0.1975 --wind-speed 5.0
```

---

### **`templates/`** - Editable Forms ✅ *AVAILABLE NOW*

| Template | Status | Format | Use |
|----------|--------|--------|-----|
| `config-template-complete.txt` | ✅ **Ready** | Text (50+ parameters) | Complete config.txt with all firmware options |
| `calibration-certificate.docx` | 📝 Planned | Word | Formal calibration certificate for lab records |
| `calibration-datasheet-advanced.xlsx` | 📝 Planned | Excel | Includes automated uncertainty calculations |

**Current workaround:** Use templates from [`../../docs/calibration/`](../../docs/calibration/) for basic documentation.

---

### **`examples/`** - Reference Materials

| Example | Status | Description |
|---------|--------|-------------|
| `example-gum-calculation.pdf` | 📝 Planned | Worked example showing full GUM methodology |
| `example-field-data-48h.csv` | 📝 Planned | Sample 48-hour calibration dataset (86,400 samples) |
| `example-uncertainty-report.pdf` | 📝 Planned | Complete uncertainty budget for thesis appendix |

---

## 🚀 Quick Start (Advanced Users)

### **Scenario 1: "I need formal uncertainty for my thesis"**

**Step-by-step workflow:**

1. **Perform basic calibration** using practical guide:
   ```
   → Follow ../../docs/calibration/anemometer_calibration.md
   → Obtain: OFFSET = 0.4225 V, SCALE = 0.1975 V/(m/s)
   ```

2. **Estimate input uncertainties:**
   - Voltage measurement uncertainty: u(V) = 0.01 V (from multimeter specs)
   - OFFSET uncertainty: u(OFFSET) = σ/√10 = 0.0012/√10 = 0.0004 V
   - SCALE uncertainty: u(SCALE) = 0.002 V/(m/s) (from calibration standard deviation)

3. **Run GUM calculator:**
   ```bash
   cd tools
   python uncertainty-calculator.py \
     --offset 0.4225 --scale 0.1975 --wind-speed 5.0 \
     --u-voltage 0.01 --u-offset 0.0004 --u-scale 0.002
   ```

4. **Output example:**
   ```
   ╔═══════════════════════════════════════════════════════════╗
   ║        Wind Speed Uncertainty Analysis (GUM Method)       ║
   ╚═══════════════════════════════════════════════════════════╝
   
   INPUT PARAMETERS:
   ─────────────────────────────────────────────────────────────
   Measured voltage (V):          2.41 V
   Calibration OFFSET:            0.4225 V
   Calibration SCALE:             0.1975 V/(m/s)
   Calculated wind speed:         10.06 m/s
   
   UNCERTAINTY CONTRIBUTIONS:
   ─────────────────────────────────────────────────────────────
   Source              | Sensitivity | Uncertainty | Contribution
   ────────────────────┼─────────────┼─────────────┼─────────────
   Voltage measurement | 5.063 m/s/V | 0.0100 V    | 0.0506 m/s
   OFFSET constant     | -5.063 m/s/V| 0.0004 V    | 0.0020 m/s
   SCALE constant      | -50.93 m²/s | 0.0020 V/m/s| 0.1019 m/s
   
   COMBINED UNCERTAINTY:
   ─────────────────────────────────────────────────────────────
   Standard uncertainty u_c(v):   0.114 m/s
   Expanded uncertainty U(k=2):   0.228 m/s (95% confidence)
   Relative uncertainty:          2.27%
   
   RESULT:
   ═════════════════════════════════════════════════════════════
   Wind speed: 10.06 ± 0.23 m/s (k=2)
   ```

5. **Report in thesis:**
   > "Wind speed measurements were calibrated using a reference anemometer with combined standard uncertainty u_c(v) = 0.11 m/s at v = 10 m/s. The expanded uncertainty is U(v) = 0.23 m/s (coverage factor k = 2, approximately 95% confidence level), corresponding to a relative uncertainty of 2.3%. Uncertainty analysis followed the Guide to the Expression of Uncertainty in Measurement (GUM) methodology."

---

### **Scenario 2: "I need complete config.txt for my ESP32"**

**Use complete template:**

```bash
# Copy template to your SD card
cp templates/config-template-complete.txt /path/to/sd-card/config.txt

# Edit with your calibration values
notepad++ /path/to/sd-card/config.txt

# Key sections to customize:
# - Section 1: ANEMOMETER_OFFSET, ANEMOMETER_SCALE (from calibration)
# - Section 3: POWER_SHUNT_RESISTANCE (measure with multimeter)
# - Section 6: SITE_NAME, SITE_LATITUDE, SITE_LONGITUDE
# - Section 9: WIFI_SSID, WIFI_PASSWORD
```

**Template includes all 50+ parameters with documentation.**

---

### **Scenario 3: "I want to compare drift across multiple calibrations"**

**Planned feature (not yet available):**

1. Collect calibration certificates from initial, 3-month, 6-month calibrations
2. Run drift analyzer:
   ```bash
   python tools/drift-analyzer.py \
     --input cal_2025-01.csv cal_2025-04.csv cal_2025-07.csv \
     --output drift_report.pdf
   ```
3. Review output: Drift rate (% per month), prediction for next calibration

**Current workaround:** Manually plot OFFSET and SCALE values vs. time in Excel, fit linear trend.

---

## 📖 Calibration Workflow for Different Users

### **Path 1: Undergraduate Project** (±5% accuracy target)

```
┌────────────────────────────────────────────────────────┐
│ DURATION: 2 days                                        │
├────────────────────────────────────────────────────────┤
│ 1. Read book Appendix C overview                       │
│ 2. Follow ../../docs/calibration/anemometer_...md     │
│ 3. Use simple ±5% uncertainty estimate                 │
│ 4. Document in lab notebook                            │
│ 5. Apply corrections in firmware                       │
│ ✓ DONE                                                  │
└────────────────────────────────────────────────────────┘

Resources used:
  - docs/calibration/ (practical guides)
  - Book Appendix C
```

---

### **Path 2: Master's Thesis** (±3% accuracy, formal uncertainty)

```
┌────────────────────────────────────────────────────────┐
│ DURATION: 4-5 days                                      │
├────────────────────────────────────────────────────────┤
│ 1. Calibrate using ../../docs/calibration/ guides      │
│    (Day 1-2: anemometer, power sensor)                 │
│                                                         │
│ 2. Run GUM uncertainty calculator                      │
│    python tools/uncertainty-calculator.py ...          │
│    (Day 3: uncertainty analysis)                       │
│                                                         │
│ 3. Generate formal certificate (template planned)      │
│    (Day 4: documentation)                              │
│                                                         │
│ 4. Include uncertainty budget in thesis appendix       │
│    (Day 5: write-up)                                   │
│ ✓ DONE                                                  │
└────────────────────────────────────────────────────────┘

Resources used:
  - docs/calibration/ (practical guides)
  - hardware/calibration/tools/uncertainty-calculator.py
  - Book Appendix C (uncertainty theory)
```

---

### **Path 3: Doctoral Research / Publication** (±1-2% accuracy)

```
┌────────────────────────────────────────────────────────┐
│ DURATION: 7-10 days                                     │
├────────────────────────────────────────────────────────┤
│ 1. Multi-point calibration (procedures/03-multi...md)  │
│    (Day 1-3: 10+ calibration points)                   │
│                                                         │
│ 2. 48-hour field validation (procedures/01-field..md)  │
│    (Day 4-6: extended data collection)                 │
│                                                         │
│ 3. Full GUM uncertainty (procedures/02-gum...md)       │
│    (Day 7-8: complete uncertainty budget)              │
│                                                         │
│ 4. Quarterly drift analysis (tools/drift-analyzer.py)  │
│    (Day 9: long-term validation)                       │
│                                                         │
│ 5. Formal calibration certificates for all sensors     │
│    (Day 10: documentation)                             │
│                                                         │
│ 6. Peer review of methodology before submission        │
│ ✓ DONE                                                  │
└────────────────────────────────────────────────────────┘

Resources used:
  - hardware/calibration/procedures/ (all files)
  - hardware/calibration/tools/ (all scripts)
  - ISO/IEC Guide 98-3 (GUM standard)
  - IEC 61400-12-2 (wind turbine measurement standard)
```

---

## 🔗 Cross-References

### **Calibration Resource Matrix**

| Topic | Quick Guide (docs/) | Advanced (hardware/) |
|-------|---------------------|----------------------|
| **Anemometer calibration** | [`../../docs/calibration/anemometer_calibration.md`](../../docs/calibration/anemometer_calibration.md) | `procedures/01-field-extended.md` (planned) |
| **Power sensor calibration** | [`../../docs/calibration/power_sensor_calibration.md`](../../docs/calibration/power_sensor_calibration.md) | Use same + `tools/uncertainty-calculator.py` |
| **Uncertainty estimation** | Simple ±5% rule | `procedures/02-uncertainty-gum.md` (planned) |
| **Configuration files** | Brief code snippets | `templates/config-template-complete.txt` ✅ |
| **Troubleshooting** | [`../../docs/troubleshooting/hardware_debugging.md`](../../docs/troubleshooting/hardware_debugging.md) | Advanced diagnostics (planned) |

---

## 📜 Compliance & Standards

Resources in this folder support compliance with:

- ✅ **ISO/IEC Guide 98-3:2008 (GUM):** Uncertainty quantification methodology
- ✅ **IEC 61400-12-2:2013:** Power performance measurement for small wind turbines
- ✅ **ISO 17025:** General requirements for testing and calibration laboratories
- ✅ **FAIR Data Principles:** Findable, Accessible, Interoperable, Reusable

---

## 🛠️ Python Tool Installation

### **Requirements**

- **Python 3.8 or higher**
- **Operating systems:** Windows, macOS, Linux

### **Install Dependencies**

```bash
# Navigate to tools folder
cd hardware/calibration/tools

# Install required packages
pip install -r requirements.txt

# Verify installation
python uncertainty-calculator.py --help
```

### **Expected Output**

```
usage: uncertainty-calculator.py [-h] --offset OFFSET --scale SCALE
                                  --wind-speed WIND_SPEED
                                  [--u-voltage U_VOLTAGE]
                                  [--u-offset U_OFFSET]
                                  [--u-scale U_SCALE]

GUM-compliant wind speed uncertainty calculator
```

---

## 🚧 Development Roadmap

### **Phase 1: Core Tools** (✅ Complete)
- [x] Uncertainty calculator (Python)
- [x] Complete config.txt template
- [x] requirements.txt for Python dependencies

### **Phase 2: Extended Procedures** (📝 In Progress)
- [ ] Field calibration extended protocol
- [ ] GUM uncertainty analysis guide
- [ ] Multi-point calibration procedure

### **Phase 3: Templates** (📝 Planned)
- [ ] Calibration certificate (Word template)
- [ ] Advanced datasheet (Excel with formulas)
- [ ] Worked examples (PDF)

### **Phase 4: Additional Tools** (📝 Future)
- [ ] Calibration curve fitter
- [ ] Drift analyzer
- [ ] Automated report generator

**Want to contribute?** See [Contributing](#-contributing) section below.

---

## ❓ FAQ

### **Q1: Do I need Python to use these resources?**

**A:** Only if you want automated uncertainty calculation. For manual calculations:
- Use formulas in `procedures/02-uncertainty-gum.md` (when available)
- Use Excel for curve fitting (instructions in practical guides)

### **Q2: What if I can't access a wind tunnel or reference anemometer?**

**A:** Use the vehicle method in [`../../docs/calibration/anemometer_calibration.md`](../../docs/calibration/anemometer_calibration.md) (Method 3). While less accurate (±10%), it's sufficient for preliminary validation.

### **Q3: How often should I recalibrate?**

**A:** Depends on environment:
- **Indoor/benign:** Every 6 months
- **Coastal/tropical:** Every 3 months
- **After damage/repair:** Immediately

Track drift with `tools/drift-analyzer.py` (when available).

### **Q4: Can I use these procedures for other sensors?**

**A:** Yes! The GUM uncertainty methodology applies to any sensor. Adapt the input parameters:
- Replace OFFSET/SCALE with your sensor's calibration constants
- Identify your uncertainty sources (sensor specs, calibration equipment)
- Run `uncertainty-calculator.py` with modified parameters

---

## 🤝 Contributing

Found an error? Have a better uncertainty estimation method? Want to add validation data?

**Ways to contribute:**
1. **Submit improvements:** [Pull Request](https://github.com/asithakal/wind-turbine-daq-guide/pulls)
2. **Report issues:** [GitHub Issues](https://github.com/asithakal/wind-turbine-daq-guide/issues)
3. **Share calibration data:** Especially multi-month drift data

**Especially welcome:**
- Worked examples for different sensor types
- Validation datasets (anonymized if needed)
- Alternative calibration methodologies
- Translation of procedures to other languages

---

## 📧 Support

**For calibration questions:**

| Question Type | Resource |
|---------------|----------|
| **Basic calibration** | Start with [`../../docs/calibration/`](../../docs/calibration/) |
| **Uncertainty analysis** | Open GitHub issue with tag `[uncertainty]` |
| **Script errors** | Open GitHub issue with tag `[tools]`, include error message |
| **Methodology review** | Email (see book author contact info) |

**GitHub Issues:** https://github.com/asithakal/wind-turbine-daq-guide/issues

**Response time:** Typically 2-5 days

---

## 📚 Recommended Reading

### **For Uncertainty Analysis**
- ISO/IEC Guide 98-3:2008: Uncertainty of measurement — Part 3: Guide to the expression of uncertainty in measurement (GUM)
- NIST Technical Note 1297: Guidelines for Evaluating and Expressing the Uncertainty of NIST Measurement Results

### **For Wind Turbine Measurements**
- IEC 61400-12-2:2013: Wind energy generation systems — Part 12-2: Power performance of electricity-producing wind turbines based on nacelle anemometry
- IEC 61400-12-1:2017: Wind energy generation systems — Part 12-1: Power performance measurements of electricity producing wind turbines

### **For Calibration Management**
- ISO/IEC 17025:2017: General requirements for the competence of testing and calibration laboratories

---

## 📄 License

- **Documentation:** CC BY 4.0 (Creative Commons Attribution)
- **Python scripts:** MIT License
- **Templates:** CC0 1.0 Universal (Public Domain)

You are free to use, modify, and distribute with attribution.

See main repository [LICENSE](../../LICENSE) for details.

---

**Version:** 1.0  
**Last Updated:** 2026-01-03  
**Maintainer:** Dr. Asitha Kulasekera, University of Moratuwa  
**Book Reference:** Appendix C