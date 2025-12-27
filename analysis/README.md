# Data Analysis Scripts

Python scripts for processing wind turbine performance data and generating publication-quality results.

## 📂 Folder Contents

### **`python_scripts/`**
Progressive analysis pipeline for Cp-λ curve generation.

| Script | Purpose | Input | Output |
|--------|---------|-------|--------|
| `01_data_quality_check.py` | Validate CSV completeness, detect gaps | `raw_data.csv` | Quality report (terminal) |
| `02_steady_state_filter.py` | Identify steady-state conditions | `raw_data.csv` | `steady_state.csv` |
| `03_cp_lambda_analysis.py` | Calculate Cp-λ curve with binning | `steady_state.csv` | `cp_lambda_binned.csv`, plot |
| `04_uncertainty_analysis.py` | Propagate sensor uncertainties | `cp_lambda_binned.csv` | Uncertainty report |
| `05_plotting.py` | Generate publication figures | `cp_lambda_binned.csv` | High-res PNG/PDF plots |
| `utils.py` | Shared helper functions | (imported by others) | N/A |

### **`example_data/`**
Sample datasets for testing scripts without real data.

- `sample_dataset_1day.csv`: 86,400 records (24 hours @ 1 Hz)
- `sample_processed.csv`: Expected output from analysis pipeline
- `example_output_plots/`: Reference figures for verification

---

## 🚀 Quick Start

### **1. Install Python Dependencies**

Requires Python 3.7 or newer.

```
# Navigate to analysis folder
cd analysis

# Install required packages
pip install -r requirements.txt
```

**Installs:** pandas, numpy, scipy, matplotlib, seaborn, pyyaml

### **2. Test with Sample Data**

```
# Run complete analysis on sample data
python python_scripts/03_cp_lambda_analysis.py \
  --input example_data/sample_dataset_1day.csv \
  --output test_results/
```

**Expected output:**
```
Loading data from example_data/sample_dataset_1day.csv...
  → Loaded 86400 records

Applying quality filters...
  → Removed 1247 invalid records (1.4%)
  → Retained 85153 valid records

Calculating derived quantities (λ, Cp)...
  → λ range: 0.12 to 3.85
  → Cp range: 0.003 to 0.387

Identifying steady-state periods...
  → Steady-state records: 62341 (73.2%)

Binning data (λ bin width = 0.1)...
  → Generated 38 bins with ≥30 samples each
  → Peak Cp: 0.358 at λ = 2.10

Generating Cp-λ curve plot...
  → Saved plot to test_results/cp_lambda_curve.png

Saved binned results to test_results/cp_lambda_binned.csv

✅ Analysis complete!
   Peak Cp: 0.358
   Optimal λ: 2.10
```

### **3. Analyze Your Data**

Replace input path with your dataset:

```
python python_scripts/03_cp_lambda_analysis.py \
  --input /path/to/your/full_dataset.csv \
  --output results/
```

---

## 📊 Data Format Requirements

### **Input CSV Structure**

Scripts expect CSV files with the following columns:

| Column Name | Description | Units | Example |
|-------------|-------------|-------|---------|
| `timestamp` | Measurement timestamp | ISO 8601 | `2025-06-15T14:32:18+0530` |
| `wind_speed_ms` | Wind speed at hub height | m/s | `7.35` |
| `rotor_rpm` | Rotor rotational speed | RPM | `142.5` |
| `power_w` | Electrical power output | W | `287.3` |
| `temp_c` | Ambient temperature | °C | `28.4` |
| `pressure_hpa` | Barometric pressure | hPa | `1012.3` |
| `humidity_pct` | Relative humidity | % | `82.1` |
| `air_density_kgm3` | Air density (optional) | kg/m³ | `1.184` |
| `lambda` | Tip speed ratio (optional) | - | `2.15` |
| `cp` | Power coefficient (optional) | - | `0.342` |

**Notes:**
- Missing values: Use `NaN` or leave blank
- Scripts auto-calculate `air_density`, `lambda`, `cp` if missing
- Timestamp must be parseable by pandas (`parse_dates=True`)

### **Configuring Turbine Parameters**

Edit constants in `03_cp_lambda_analysis.py`:

```
# Line 12-15: Turbine geometry
ROTOR_RADIUS = 0.60  # meters (change to your turbine radius)
ROTOR_HEIGHT = 1.50  # meters (change to your turbine height)
SWEPT_AREA = 1.80    # m² (2*R*H for VAWT, π*R² for HAWT)
```

---

## 🧪 Script Details

### **Script 01: Data Quality Check**

```
python python_scripts/01_data_quality_check.py --input data.csv
```

**Checks:**
- ✅ File format validity
- ✅ Required columns present
- ✅ Timestamp continuity (detects gaps)
- ✅ Data completeness percentage
- ✅ Out-of-range values
- ✅ Duplicate timestamps

**Output:** Terminal report with recommendations

---

### **Script 02: Steady-State Filter**

```
python python_scripts/02_steady_state_filter.py \
  --input data.csv \
  --output steady_state.csv \
  --window 10 \
  --threshold 0.5
```

**Parameters:**
- `--window`: Rolling window size (minutes) [default: 10]
- `--threshold`: Max wind speed std dev (m/s) [default: 0.5]

**Methodology:** Filters periods where wind speed standard deviation < threshold over rolling window (IEC 61400-12-2 guideline) [citation:file:1].

---

### **Script 03: Cp-Lambda Analysis** ⭐ **(Primary Analysis)**

```
python python_scripts/03_cp_lambda_analysis.py \
  --input steady_state.csv \
  --output results/ \
  --bin_width 0.1 \
  --min_samples 30
```

**Parameters:**
- `--bin_width`: Lambda bin size [default: 0.1]
- `--min_samples`: Minimum samples per bin [default: 30]

**Outputs:**
1. `cp_lambda_binned.csv`: Binned performance data with statistics
2. `cp_lambda_curve.png`: Publication-quality plot (300 DPI)

---

### **Script 04: Uncertainty Analysis**

```
python python_scripts/04_uncertainty_analysis.py \
  --input cp_lambda_binned.csv \
  --output uncertainty_report.txt
```

**Calculates:**
- Combined uncertainty in Cp (quadrature sum method)
- Uncertainty propagation from sensor specs
- Confidence intervals (95%)

**Reference:** ISO GUM uncertainty framework

---

### **Script 05: Plotting**

```
python python_scripts/05_plotting.py \
  --input cp_lambda_binned.csv \
  --output figures/ \
  --format pdf
```

**Generates:**
- Cp vs λ curve with error bars
- Power vs wind speed curve
- Time-series plots (wind, power, RPM)
- Wind rose diagram

**Formats:** PNG (default), PDF, SVG

---

## 🔬 Example Analysis Workflow

Full pipeline for thesis/paper:

```
# Step 1: Check data quality
python python_scripts/01_data_quality_check.py --input raw_data.csv

# Step 2: Filter steady-state
python python_scripts/02_steady_state_filter.py \
  --input raw_data.csv \
  --output steady_state.csv

# Step 3: Calculate Cp-λ curve
python python_scripts/03_cp_lambda_analysis.py \
  --input steady_state.csv \
  --output results/

# Step 4: Uncertainty analysis
python python_scripts/04_uncertainty_analysis.py \
  --input results/cp_lambda_binned.csv \
  --output results/uncertainty_report.txt

# Step 5: Generate publication figures
python python_scripts/05_plotting.py \
  --input results/cp_lambda_binned.csv \
  --output figures/ \
  --format pdf
```

---

## 📈 Interpreting Results

### **Typical Cp-λ Curve Characteristics**

| Feature | Typical Value (VAWT) | Interpretation |
|---------|----------------------|----------------|
| **Peak Cp** | 0.25 - 0.40 | Aerodynamic efficiency (Betz limit = 0.593) |
| **Optimal λ** | 1.5 - 2.5 | Design tip speed ratio |
| **Cut-in λ** | < 1.0 | Self-starting capability |
| **Stall λ** | > 3.0 | Over-speed inefficiency |

**Red flags:**
- Cp > 0.6: Calculation error (exceeds Betz limit)
- Cp < 0.1: Poor turbine performance or measurement issues
- Multiple peaks: Check data quality, possible turbulence

---

## 🛠️ Troubleshooting

**Problem:** `ModuleNotFoundError: No module named 'pandas'`  
**Solution:** Run `pip install -r requirements.txt`

**Problem:** "No steady-state periods found"  
**Solution:** Reduce `--threshold` parameter (try 0.8 m/s instead of 0.5)

**Problem:** Cp values all near zero  
**Solution:** Check turbine geometry constants (`ROTOR_RADIUS`, `SWEPT_AREA`)

**Problem:** Script runs but no output files  
**Solution:** Check `--output` directory exists (create manually if needed)

---

## 🔗 Related Resources

- **Firmware for data collection:** [`../firmware/`](../firmware/)
- **Dataset templates:** [`../templates/documentation/`](../templates/documentation/)
- **Methodology details:** See book Section 8 (Data Processing Pipeline)

---

## 🤝 Contributing

Improve analysis methods? Add new scripts?

- Submit via [Pull Request](https://github.com/asithakal/wind-turbine-daq-guide/pulls)
- Suggest features via [Issues](https://github.com/asithakal/wind-turbine-daq-guide/issues)

---

## 📜 License

Code: MIT License  
Documentation: CC BY 4.0

See [LICENSE](../LICENSE) for details.