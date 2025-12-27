# Data Quality Assurance for Wind Turbine Measurements

Procedures for ensuring research-grade data quality throughout the measurement campaign.

**Target audience:** Graduate students, research technicians  
**Covers:** QA/QC principles, automated checks, validation procedures, outlier detection

---

## Why Data Quality Matters

### **Consequences of Poor Data Quality**

| Issue | Impact on Results | Thesis/Paper Risk |
|-------|-------------------|-------------------|
| **Uncalibrated sensors** | ±20% error in Cp | Reviewer rejection, impossible to publish |
| **Missing data (>20%)** | Biased averages | Insufficient data, cannot draw conclusions |
| **Undetected outliers** | Skewed statistics | Questioned methodology |
| **No uncertainty quantification** | Unquantified confidence | Fails reproducibility standards |
| **Undocumented gaps** | Loss of trust | Suspicion of cherry-picking data |

**"Garbage in, garbage out"** - Fix problems early, not during thesis writing

---

## QA/QC Framework

### **Quality Assurance (QA) vs. Quality Control (QC)**

| Aspect | Quality Assurance (QA) | Quality Control (QC) |
|--------|------------------------|----------------------|
| **When** | Before and during measurement | After measurement |
| **Goal** | Prevent errors | Detect and correct errors |
| **Examples** | Sensor calibration, standard procedures | Outlier removal, gap analysis |
| **Who** | Project planner | Data analyst |

**Both are essential** for research-grade data

---

## Phase 1: Pre-Deployment QA (Before Data Collection)

### **Checklist 1: Sensor Calibration**

**All sensors must be calibrated before deployment.**

| Sensor | Calibration Method | Acceptance Criteria | Certificate Required |
|--------|-------------------|---------------------|----------------------|
| **Anemometer** | Wind tunnel or reference comparison | ±5% of reference | Yes (photo OK) |
| **Power sensor** | Known voltage/current sources | ±3% | Yes (multimeter readings) |
| **Temperature** | Ice point (0°C) + boiling point (100°C) | ±1°C | Yes |
| **Pressure** | Reference barometer | ±1 hPa | Optional |
| **RPM (Hall)** | Known rotation rate (drill/motor) | ±1 RPM | Optional |

**Store calibration certificates in:** [`../../docs/calibration/`](../../docs/calibration/)

---

### **Checklist 2: System Validation Tests**

**Run these tests before field deployment:**

#### **Test 1: Data Completeness (24-Hour Test)**

**Procedure:**
1. Set up system in lab
2. Run continuously for 24 hours
3. Check for data gaps

**Acceptance:**
```
expected_samples = 24 * 60 * 60  # 86,400 samples at 1 Hz
actual_samples = len(df)
completeness = (actual_samples / expected_samples) * 100

print(f"Completeness: {completeness:.2f}%")
# Must be >99.5% for lab test (no environmental interference)
```

**Common failures:**
- SD card slow → upgrade to Class 10/UHS-I
- WiFi dropouts → disable MQTT during acquisition
- Power supply insufficient → use 2A adapter

---

#### **Test 2: Sensor Range Validation**

**Check all sensors are within spec:**

```
# Load 24-hour test data
df = pd.read_csv('test_data.csv')

# Define valid ranges
ranges = {
    'wind_speed_ms': (0.5, 25),
    'rotor_rpm': (0, 500),
    'power_w': (0, 600),
    'temp_c': (15, 45),
    'pressure_hpa': (990, 1020),
    'humidity_pct': (40, 100)
}

# Check for out-of-range values
for col, (min_val, max_val) in ranges.items():
    out_of_range = df[(df[col] < min_val) | (df[col] > max_val)]
    if len(out_of_range) > 0:
        print(f"WARNING: {col} has {len(out_of_range)} out-of-range values")
        print(out_of_range[[col]].head())
```

**Acceptance:** <0.1% out-of-range values

---

#### **Test 3: Timestamp Continuity**

**Check for clock drifts or resets:**

```
df['timestamp'] = pd.to_datetime(df['timestamp'])
df['time_diff'] = df['timestamp'].diff().dt.total_seconds()

# Expected: 1.0 second between samples
gaps = df[df['time_diff'] > 1.5]  # Allow 50% tolerance
print(f"Gaps detected: {len(gaps)}")

if len(gaps) > 0:
    print("Gap timestamps:")
    print(gaps[['timestamp', 'time_diff']])
```

**Acceptance:** No gaps >2 seconds (allows for occasional 1 missed sample)

---

#### **Test 4: Noise Floor Measurement**

**With zero input (wind = 0, turbine stopped), measure sensor noise:**

**Procedure:**
1. Disconnect turbine (zero mechanical input)
2. Block anemometer (or indoor calm air)
3. Log for 10 minutes

**Analysis:**
```
noise_df = df[(df['wind_speed_ms'] < 0.5) & (df['rotor_rpm'] < 1)]

print(f"Power sensor noise: {noise_df['power_w'].std():.3f} W")
print(f"Temp sensor noise: {noise_df['temp_c'].std():.3f} °C")
```

**Typical values:**
- Power: <0.5 W std dev
- Temperature: <0.1°C std dev

---

## Phase 2: During-Deployment QC (Weekly Checks)

### **Automated Daily Check Script**

**Run this script every morning to validate previous day's data:**

```
#!/usr/bin/env python3
"""
Daily data quality check
Usage: python daily_check.py --date 2025-06-15
"""

import pandas as pd
import argparse
from datetime import datetime

def check_completeness(df):
    """Check for data gaps"""
    expected = 86400  # 1 Hz * 86400 sec/day
    actual = len(df)
    completeness = (actual / expected) * 100
    
    if completeness < 95:
        print(f"⚠️  WARN: Completeness {completeness:.2f}% (expected >95%)")
    else:
        print(f"✅ PASS: Completeness {completeness:.2f}%")
    
    return completeness

def check_sensor_health(df):
    """Check for stuck sensors (flat lines)"""
    issues = []
    
    for col in ['wind_speed_ms', 'rotor_rpm', 'power_w', 'temp_c']:
        # Check for unchanging values over 1 hour (3600 samples)
        rolling_std = df[col].rolling(3600).std()
        stuck_periods = (rolling_std < 0.01).sum()
        
        if stuck_periods > 3600:  # >1 hour stuck
            issues.append(f"{col} appears stuck (flat line)")
    
    if issues:
        for issue in issues:
            print(f"⚠️  WARN: {issue}")
    else:
        print("✅ PASS: All sensors responsive")
    
    return len(issues) == 0

def check_outliers(df):
    """Check for extreme outliers (>5 sigma)"""
    outliers = {}
    
    for col in ['cp', 'lambda']:
        if col in df.columns:
            mean = df[col].mean()
            std = df[col].std()
            extreme = df[(df[col] < mean - 5*std) | (df[col] > mean + 5*std)]
            outliers[col] = len(extreme)
    
    total_outliers = sum(outliers.values())
    if total_outliers > len(df) * 0.01:  # >1% outliers
        print(f"⚠️  WARN: {total_outliers} extreme outliers ({total_outliers/len(df)*100:.2f}%)")
    else:
        print(f"✅ PASS: Outliers within normal range ({total_outliers})")
    
    return total_outliers

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('--date', required=True, help='Date in YYYY-MM-DD format')
    args = parser.parse_args()
    
    filename = f"{args.date}_data.csv"
    print(f"\n{'='*50}")
    print(f"Data Quality Check: {args.date}")
    print(f"{'='*50}\n")
    
    try:
        df = pd.read_csv(filename, parse_dates=['timestamp'])
    except FileNotFoundError:
        print(f"❌ ERROR: File {filename} not found")
        return
    
    # Run checks
    completeness = check_completeness(df)
    sensor_health = check_sensor_health(df)
    outlier_count = check_outliers(df)
    
    # Summary
    print(f"\n{'='*50}")
    if completeness > 95 and sensor_health and outlier_count < len(df) * 0.01:
        print("✅ Overall: PASS - Data quality acceptable")
    else:
        print("⚠️  Overall: REVIEW NEEDED - Issues detected")
    print(f"{'='*50}\n")

if __name__ == '__main__':
    main()
```

**Usage:**
```
python daily_check.py --date 2025-06-15
```

---

### **Weekly On-Site Inspection**

**Physical checks (15-20 minutes):**

**Enclosure:**
- [ ] No water inside
- [ ] Desiccant not saturated (still blue/orange, not pink)
- [ ] No insect intrusion
- [ ] Cable glands tight

**Sensors:**
- [ ] Anemometer spins freely (no bearing seize)
- [ ] No corrosion on terminals
- [ ] Cables not chafing
- [ ] Mounting hardware tight

**SD Card:**
- [ ] Free space remaining >20%
- [ ] No error LEDs on module
- [ ] Copy data to backup location

**Data Spot Check:**
- [ ] Open latest CSV in Excel
- [ ] Visually scan for obvious errors
- [ ] Check last timestamp is recent (<5 minutes ago)

**Log findings in lab notebook:** [`../../templates/documentation/lab_notebook_template.md`](../../templates/documentation/lab_notebook_template.md)

---

### **Monthly Sensor Drift Check**

**Compare current readings to baseline (calibration values):**

**Procedure:**
1. Bring reference instruments to site
2. Record 10 minutes of simultaneous measurements
3. Compare averages

**Example (anemometer check):**
```
Reference anemometer: 7.2 m/s (avg over 10 min)
System anemometer: 6.8 m/s (avg)
Difference: -5.6%

Action: Within ±5% tolerance, no recalibration needed
```

**If drift >5%:** Recalibrate sensor and note in dataset documentation

---

## Phase 3: Post-Campaign QC (After Data Collection)

### **Full Dataset Validation**

**Run comprehensive analysis before processing:**

#### **Analysis 1: Data Coverage Map**

**Visualize data gaps:**

```
import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

df = pd.read_csv('full_dataset.csv', parse_dates=['timestamp'])

# Create daily completeness array
df['date'] = df['timestamp'].dt.date
daily_counts = df.groupby('date').size()

expected_per_day = 86400
daily_completeness = (daily_counts / expected_per_day) * 100

# Plot
fig, ax = plt.subplots(figsize=(12, 4))
ax.bar(range(len(daily_completeness)), daily_completeness.values, color='steelblue')
ax.axhline(y=100, color='green', linestyle='--', label='100%')
ax.axhline(y=95, color='orange', linestyle='--', label='95% threshold')
ax.set_xlabel('Day of Campaign')
ax.set_ylabel('Data Completeness (%)')
ax.set_title('Daily Data Completeness')
ax.legend()
plt.tight_layout()
plt.savefig('data_coverage.png', dpi=300)
```

**Document all gaps in README:**
```
## Data Gaps

| Start Date | End Date | Duration | Reason |
|------------|----------|----------|--------|
| 2025-06-15 | 2025-06-17 | 3 days | Scheduled maintenance |
| 2025-08-05 | 2025-08-05 | 8 hours | Power outage |
| 2025-09-20 | 2025-09-20 | 4 hours | SD card replacement |
```

---

#### **Analysis 2: Sensor Cross-Validation**

**Check for physically impossible combinations:**

```
# Physics check: Cp cannot exceed Betz limit (0.593)
invalid_cp = df[df['cp'] > 0.6]
print(f"Invalid Cp values: {len(invalid_cp)} ({len(invalid_cp)/len(df)*100:.3f}%)")

# Physics check: Lambda relationship
# Lambda = (omega * R) / v_wind
# where omega = 2*pi*RPM/60
df['lambda_calculated'] = (df['rotor_rpm'] * 2 * np.pi / 60 * 0.6) / df['wind_speed_ms']
df['lambda_error'] = np.abs(df['lambda'] - df['lambda_calculated'])

large_errors = df[df['lambda_error'] > 0.1]
print(f"Lambda calculation errors: {len(large_errors)} ({len(large_errors)/len(df)*100:.3f}%)")
```

**Action:** Flag or remove physically impossible values

---

#### **Analysis 3: Statistical Outlier Detection**

**Multiple methods for robust detection:**

**Method 1: Tukey Fences (IQR method)**

```
def detect_outliers_iqr(df, column):
    """Interquartile range method"""
    Q1 = df[column].quantile(0.25)
    Q3 = df[column].quantile(0.75)
    IQR = Q3 - Q1
    
    lower_fence = Q1 - 3 * IQR
    upper_fence = Q3 + 3 * IQR
    
    outliers = df[(df[column] < lower_fence) | (df[column] > upper_fence)]
    return outliers

cp_outliers = detect_outliers_iqr(df, 'cp')
print(f"Cp outliers (IQR method): {len(cp_outliers)} ({len(cp_outliers)/len(df)*100:.2f}%)")
```

**Method 2: Z-Score (for normal distributions)**

```
from scipy import stats

def detect_outliers_zscore(df, column, threshold=3):
    """Z-score method (assumes normal distribution)"""
    z_scores = np.abs(stats.zscore(df[column].dropna()))
    outliers_mask = z_scores > threshold
    return df[outliers_mask]

wind_outliers = detect_outliers_zscore(df, 'wind_speed_ms')
print(f"Wind speed outliers (Z-score): {len(wind_outliers)}")
```

**Method 3: Rolling Median (for time-series)**

```
def detect_outliers_rolling(df, column, window=60, threshold=3):
    """Detect sudden spikes using rolling median"""
    rolling_median = df[column].rolling(window, center=True).median()
    rolling_std = df[column].rolling(window, center=True).std()
    
    deviation = np.abs(df[column] - rolling_median)
    outliers = df[deviation > threshold * rolling_std]
    return outliers

power_outliers = detect_outliers_rolling(df, 'power_w', window=600)  # 10-minute window
print(f"Power outliers (rolling): {len(power_outliers)}")
```

---

#### **Analysis 4: Uncertainty Quantification**

**Propagate sensor uncertainties to derived quantities:**

**Given sensor uncertainties:**
```
Δv (wind speed) = ±5%
Δω (RPM) = ±0.5 RPM
ΔP (power) = ±3%
Δρ (air density) = ±1%
```

**Calculate combined uncertainty in Cp:**

```
def calculate_cp_uncertainty(wind_speed, power, air_density, swept_area):
    """
    Cp = P / (0.5 * ρ * A * v³)
    
    Relative uncertainty (quadrature sum):
    ΔCp/Cp = √[(ΔP/P)² + (Δρ/ρ)² + (3*Δv/v)²]
    """
    u_power = 0.03  # ±3%
    u_density = 0.01  # ±1%
    u_wind = 0.05  # ±5%
    
    # Combined relative uncertainty
    u_cp_relative = np.sqrt(u_power**2 + u_density**2 + (3*u_wind)**2)
    
    # Calculate Cp
    wind_power = 0.5 * air_density * swept_area * wind_speed**3
    cp = power / wind_power
    
    # Absolute uncertainty
    u_cp_absolute = cp * u_cp_relative
    
    return cp, u_cp_absolute, u_cp_relative

# Example
cp, u_cp, u_cp_rel = calculate_cp_uncertainty(
    wind_speed=8.0,
    power=300,
    air_density=1.2,
    swept_area=1.8
)

print(f"Cp = {cp:.3f} ± {u_cp:.3f} ({u_cp_rel*100:.1f}%)")
# Output: Cp = 0.325 ± 0.027 (8.3%)
```

**Include uncertainty in all results tables and plots**

---

### **Data Cleaning Decision Tree**

**How to handle outliers:**

```
Outlier detected
   │
   ├─→ Physical impossibility (Cp > 0.6)?
   │    └─→ YES: Remove (data error)
   │
   ├─→ Sensor malfunction evident?
   │    └─→ YES: Remove range, document gap
   │
   ├─→ Explainable by weather event?
   │    └─→ YES: Keep, annotate in dataset
   │
   └─→ Isolated spike (<5 samples)?
        └─→ Flag but keep (let user decide)
```

**General rule:** Retain outliers unless clearly erroneous; flag with quality code

---

## Quality Flags System

### **Implement Multi-Level Quality Coding**

**Add quality flag column to dataset:**

```
df['quality_flag'] = 0  # Default: good data

# Level 1: Out of sensor range
df.loc[df['wind_speed_ms'] < 0.5, 'quality_flag'] = 1
df.loc[df['cp'] > 0.6, 'quality_flag'] = 1

# Level 2: Questionable (outlier but possible)
z_scores = np.abs(stats.zscore(df['cp'].dropna()))
df.loc[z_scores > 3, 'quality_flag'] = 2

# Level 3: Interpolated (gap-filled)
# (if you choose to fill gaps)
df.loc[df['wind_speed_ms'].isna(), 'quality_flag'] = 3

# Level 4: Sensor calibration drift suspected
# (after recalibration on 2025-10-15)
df.loc[df['timestamp'] >= '2025-10-15', 'quality_flag'] = 4
```

**Quality flag definitions (document in README):**
```
0 = Good data (passed all QC checks)
1 = Bad data (outside sensor range, should not be used)
2 = Questionable (statistical outlier, user discretion)
3 = Interpolated (gap-filled, use with caution)
4 = Recalibrated (sensor drift corrected, note discontinuity)
```

**Benefit:** Users can filter by quality level for different analyses

---

## Reporting Data Quality

### **Required Metadata Fields**

**In your dataset README, include:**

```
## Data Quality Summary

**Completeness:** 92.3% (14,610,720 valid of 15,840,000 expected records)

**Known Issues:**
- Anemometer bearing friction increased after 4 months → recalibrated 2025-10-15
- SD card replaced 2025-08-20 (no data loss)
- WiFi connectivity intermittent during heavy rain (local logging unaffected)

**Outliers:**
- 1,247 samples flagged as statistical outliers (0.9%)
- 85 samples removed (Cp > 0.6, physically impossible)
- Outliers retained with quality_flag = 2 (user discretion to filter)

**Sensor Uncertainties (95% confidence):**
| Parameter | Uncertainty |
|-----------|-------------|
| Wind speed | ±5% |
| Rotor RPM | ±0.5 RPM |
| Power | ±3% |
| Temperature | ±1°C |
| Pressure | ±1 hPa |
| **Combined Cp** | **±8%** |

**Calibration History:**
- 2025-05-15: Initial calibration (pre-deployment)
- 2025-07-20: Validation check (no recalibration needed)
- 2025-10-15: Anemometer recalibrated (bearing friction)
- 2025-12-05: Final validation (post-campaign)

**Quality Flags:**
- 13,451,890 samples (92.1%): quality_flag = 0 (good)
- 1,067,830 samples (7.3%): quality_flag = 2 (outliers)
- 85 samples (<0.001%): quality_flag = 1 (removed)
```

---

## QA/QC Checklist Summary

### **Pre-Deployment**
- [ ] All sensors calibrated with certificates
- [ ] 24-hour lab test completed (>99.5% completeness)
- [ ] Sensor noise floor measured
- [ ] Timestamp continuity verified
- [ ] Data backup procedure established

### **During Campaign (Weekly)**
- [ ] Daily automated QC script run
- [ ] Physical inspection performed
- [ ] Data backed up to secondary location
- [ ] Free SD card space checked (>20%)
- [ ] No visible sensor degradation

### **During Campaign (Monthly)**
- [ ] Sensor drift check vs. reference
- [ ] Recalibration if drift >5%
- [ ] Data quality report generated
- [ ] Lab notebook updated

### **Post-Campaign**
- [ ] Full dataset loaded and validated
- [ ] Data coverage map generated
- [ ] Outliers detected and flagged
- [ ] Uncertainty analysis completed
- [ ] Quality flags assigned
- [ ] Data quality summary written
- [ ] Known issues documented
- [ ] Calibration history finalized

---

## Tools and Scripts

### **Recommended QC Software**

| Tool | Purpose | Platform | Cost |
|------|---------|----------|------|
| **Python pandas** | Data validation scripts | Cross-platform | Free |
| **Excel** | Quick visual inspection | Windows/Mac | Paid |
| **MATLAB** | Statistical analysis | Cross-platform | Paid (university license) |
| **OpenRefine** | Data cleaning GUI | Cross-platform | Free |
| **R (tidyverse)** | Statistical QC | Cross-platform | Free |

---

### **Complete QC Script Template**

**Available in repository:**  
[`../../analysis/python_scripts/01_data_quality_check.py`](../../analysis/python_scripts/01_data_quality_check.py)

**Features:**
- Completeness check
- Range validation
- Outlier detection (IQR + Z-score)
- Gap analysis
- Sensor cross-validation
- Uncertainty calculation
- Automated report generation

---

## Case Study: Detecting Data Quality Issues

### **Real Example: Anemometer Bearing Failure**

**Symptoms (Week 18 of campaign):**
- Wind speed readings 10-15% lower than nearby weather station
- Standard deviation decreased (less natural variability)
- Rolling mean unusually smooth

**Detection:**
```
# Compare week 18 to week 2 (baseline)
week2 = df[(df['timestamp'] >= '2025-06-08') & (df['timestamp'] < '2025-06-15')]
week18 = df[(df['timestamp'] >= '2025-10-08') & (df['timestamp'] < '2025-10-15')]

print(f"Week 2 wind std dev: {week2['wind_speed_ms'].std():.3f}")
print(f"Week 18 wind std dev: {week18['wind_speed_ms'].std():.3f}")

# Week 2: 2.134 m/s std dev
# Week 18: 1.876 m/s std dev → 12% decrease (suspicious!)
```

**Root cause:** Anemometer bearing friction increased (salt spray corrosion)

**Action taken:**
1. Anemometer removed, cleaned, recalibrated
2. Data from week 15-18 flagged: `quality_flag = 4`
3. Correction factor applied: multiply by 1.12 (validated against reference)
4. Documented in dataset README under "Known Issues"

**Lesson:** Monthly drift checks are critical for long-term campaigns

---

## Additional Resources

### **Standards**
- **IEC 61400-12-2:2013** - Power performance measurements for small wind turbines
- **ISO/IEC Guide 98-3:2008** - Uncertainty of measurement (GUM)
- **ASTM E29-13** - Standard Practice for Using Significant Digits

### **Software**
- **Python pandas documentation:** https://pandas.pydata.org/docs/
- **SciPy stats module:** https://docs.scipy.org/doc/scipy/reference/stats.html
- **OpenRefine tutorial:** https://openrefine.org/docs

### **Reading**
- Taylor, J. R. (1997). *An Introduction to Error Analysis* (uncertainty quantification)
- NIST Engineering Statistics Handbook: https://www.itl.nist.gov/div898/handbook/

---

**Questions about data quality?** Open an issue: https://github.com/asithakal/wind-turbine-daq-guide/issues