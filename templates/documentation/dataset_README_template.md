===================================================================

WIND TURBINE PERFORMANCE DATASET - README

===================================================================

Title: Power Coefficient Measurements for 500W Helical VAWT 

       under Tropical Coastal Conditions

Authors: ABC Perera, X

Institution: University of Moratuwa, Sri Lanka

Date Published: December 24, 2025

DOI: 10.5281/zenodo.1234567

License: CC BY 4.0

===================================================================

QUICK START

===================================================================

1. DATASET CONTENTS

   - full_dataset.csv (780 MB): Complete 6-month measurement campaign

   - cp_lambda_binned.csv: Processed performance curve data

   - metadata.yaml: Comprehensive metadata (read this first!)

   - technical_report.pdf: Detailed methodology and uncertainty analysis

   - /calibration/: Sensor calibration certificates

   - /scripts/: Python data processing scripts

   - README.txt: This file

2. SYSTEM REQUIREMENTS

   - Any software supporting CSV: Excel, Python (pandas), MATLAB, R

   - For analysis scripts: Python 3.7+, pandas, numpy, matplotlib, scipy

3. DATA FORMAT

   Comma-separated values (CSV) with header row.

   Missing data coded as: NaN

   Timestamp format: ISO 8601 (YYYY-MM-DDTHH:MM:SS+0530)

   

   Columns:

   timestamp, wind_speed_ms, rotor_rpm, power_w, temp_c, 

   pressure_hpa, humidity_pct, air_density_kgm3, lambda, cp

4. QUICK ANALYSIS

   Load in Python:

   >>> import pandas as pd

   >>> df = pd.read_csv('full_dataset.csv', parse_dates=['timestamp'])

   >>> df.describe()

   

   Load in MATLAB:

   >> data = readtable('full_dataset.csv');

   >> summary(data)

===================================================================

DATA QUALITY NOTES

===================================================================

- Data completeness: 92.3% (14.6M of 15.8M expected records)

- Known gaps: 2025-07-15 to 2025-07-18 (scheduled maintenance)

- Calibration drift correction applied: 2025-10-15 onwards

- Outliers flagged but not removed (manual review recommended)

Measurement Uncertainties:

- Wind speed: ±5%

- Rotor speed: ±0.5 RPM

- Power: ±3%

- Cp: ±8% (combined)

- λ: ±6% (combined)

===================================================================

USAGE AND CITATION

===================================================================

This dataset is licensed under Creative Commons Attribution 4.0 

International (CC BY 4.0). You are free to:

- Share: copy and redistribute

- Adapt: remix, transform, and build upon

Under the following terms:

- Attribution: cite this dataset (see below)

- No additional restrictions

Recommended Citation:

Perera, A.B.C., & X. (2025). Power Coefficient Measurements 

for 500W Helical VAWT under Tropical Coastal Conditions [Data set]. 

Zenodo. https://doi.org/10.5281/zenodo.1234567

===================================================================

CONTACT

===================================================================

For questions, corrections, or collaboration:

ABC Perera

Email: ABC.perera@example.com

ORCID: 0000-0002-1234-5678

Technical issues with data quality or processing:

Dr. X

Email: X@uom.lk

===================================================================

ACKNOWLEDGMENTS

===================================================================

This research was supported by:

- Sri Lanka National Science Foundation (Grant NSF/RG/2024/ME/12)

- University of Moratuwa Research Grant (SRC/LT/2024/15)

Special thanks to:

- Rajitha Silva (technical support)

- University of Moratuwa Mechanical Engineering Workshop

===================================================================

VERSION HISTORY

===================================================================

v1.0 (2025-12-24): Initial public release

===================================================================
