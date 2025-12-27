# FAIR Data Publication Guide

Comprehensive checklist for making your wind turbine performance dataset Findable, Accessible, Interoperable, and Reusable.

**Target audience:** Graduate students, postdocs, early-career researchers  
**Covers:** FAIR principles, metadata standards, repository selection, DOI registration

---

## What is FAIR Data?

### **The FAIR Principles (2016)**

| Principle | Meaning | Why It Matters |
|-----------|---------|----------------|
| **Findable** | Data and metadata are assigned persistent identifiers and described with rich metadata | Other researchers can discover your dataset via search engines |
| **Accessible** | Data retrievable via standard protocols, even if access is restricted | Long-term availability, no broken links |
| **Interoperable** | Data uses standard vocabularies and formats | Can be combined with other datasets without manual conversion |
| **Reusable** | Data has clear license and provenance information | Others can legally and confidently build on your work |

**Reference:** Wilkinson, M. D., et al. (2016). The FAIR Guiding Principles for scientific data management and stewardship. *Scientific Data*, 3, 160018.

---

## Why FAIR Matters for Your Thesis/Paper

### **Academic Benefits**

1. **Higher citation rates:** FAIR datasets cited 3-5× more than non-FAIR (studies show)
2. **Funder compliance:** Many funding agencies now mandate FAIR data (NSF, EU Horizon, UKRI)
3. **Journal requirements:** Nature, Science, IEEE journals require data availability statements
4. **Reproducibility:** Allows peers to verify your results → builds credibility
5. **Collaboration:** Easier for others to build on your work → more impact

### **Career Benefits**

- Data publications count toward publication metrics (h-index, etc.)
- Demonstrates rigor and professionalism to hiring committees
- Dataset DOIs can be cited in CV/resume

---

## FAIR Implementation: 8-Step Checklist

### **Step 1: Data Organization (Before Collection Ends)**

**Organize your files:**

```
dataset_root/
├── README.md                    # Human-readable documentation
├── metadata.yaml                # Machine-readable metadata
├── data/
│   ├── raw/
│   │   ├── 2025-06-01_raw.csv
│   │   ├── 2025-06-02_raw.csv
│   │   └── ...
│   └── processed/
│       ├── full_dataset.csv
│       └── cp_lambda_binned.csv
├── code/
│   ├── data_processing.py
│   ├── analysis_script.py
│   └── requirements.txt
├── docs/
│   ├── methodology.pdf
│   ├── calibration_certificates/
│   └── test_plan.xlsx
└── LICENSE.txt
```

**File naming conventions:**
- Use dates: `YYYY-MM-DD_description.ext`
- No spaces: Use underscores `_` or hyphens `-`
- Lowercase: `full_dataset.csv` not `Full_Dataset.CSV`
- Descriptive: `cp_lambda_binned.csv` not `output_final_v3.csv`

---

### **Step 2: Choose a Repository**

**Recommended repositories:**

| Repository | Best For | Features | Cost |
|------------|----------|----------|------|
| **Zenodo** | General research data | Free, DOI, 50GB/dataset, EU-backed | Free |
| **figshare** | Multimedia, presentations | Free, DOI, unlimited size, altmetrics | Free |
| **IEEE DataPort** | Engineering datasets | Free, DOI, integration with IEEE Xplore | Free |
| **Institutional Repository** | University mandate | Local support, thesis integration | Free |
| **Mendeley Data** | Small datasets (<10GB) | Free, DOI, Elsevier integration | Free |

**Avoid:** Google Drive, Dropbox, personal websites (not persistent, no DOI)

**Recommendation for wind turbine data:** **Zenodo** (most flexible, FAIR-focused)

---

### **Step 3: Create Rich Metadata**

**Use the template:** [`../../templates/documentation/metadata_template.yaml`](../../templates/documentation/metadata_template.yaml)

**Essential metadata elements:**

**Dublin Core fields (minimum):**
1. **Title:** Descriptive, includes key terms
   - ✅ Good: "Power Coefficient Measurements for 500W Helical VAWT under Tropical Coastal Conditions"
   - ❌ Bad: "Wind Turbine Data"
2. **Creator(s):** Full names with ORCID IDs
3. **Subject/Keywords:** 5-10 terms (wind turbine, VAWT, power coefficient, etc.)
4. **Description:** 200-500 word abstract
5. **Date:** Collection period (start and end dates)
6. **Type:** Dataset
7. **Format:** CSV, YAML, PDF, etc.
8. **Identifier:** DOI (assigned by repository)
9. **Rights:** License (see Step 4)

**Domain-specific extensions:**
10. **Geospatial:** Latitude, longitude, elevation
11. **Temporal:** Sampling frequency, campaign duration
12. **Instrumentation:** Sensor models, calibration dates, accuracy specs
13. **Methodology:** Measurement standards (IEC 61400-12-2)
14. **Related publications:** DOIs of papers using this data
15. **Funding:** Grant numbers

---

### **Step 4: Select an Appropriate License**

**Open data licenses (recommended):**

| License | Permissions | Requirements | Use When |
|---------|-------------|--------------|----------|
| **CC BY 4.0** | Commercial/non-commercial use, modify, distribute | Attribution (citation) | **Recommended for datasets** |
| **CC0** | Public domain, no restrictions | None | Maximum reusability (rare for research) |
| **ODbL** | Use, modify, distribute | Attribution + share-alike | Database-specific |

**Restrictive licenses (use with caution):**
- **CC BY-NC:** Non-commercial only (limits reuse)
- **CC BY-SA:** Share-alike (viral, complicates derivative works)
- **Custom license:** Requires legal review

**Default recommendation:** **CC BY 4.0** 
- Allows commercial use (companies can test turbines with your data)
- Requires citation (you get credit)
- Compatible with most funder policies

**How to apply:**
1. Create `LICENSE.txt` file with full license text (copy from: https://creativecommons.org/licenses/by/4.0/legalcode.txt)
2. Add to repository root
3. State in README: "This dataset is licensed under CC BY 4.0"

---

### **Step 5: Document Data Quality**

**Include in README or separate data quality report:**

**A. Completeness:**
```
Total expected records: 15,840,000 (183 days × 86,400 sec/day)
Valid records: 14,610,720 (92.3%)
Missing data: 1,229,280 (7.7%)

Reasons for gaps:
- Scheduled maintenance: 3 days (June 15-17)
- Power outage: 1.2 days (August 5)
- SD card replacement: 0.5 days (September 20)
```

**B. Uncertainty quantification:**
```
Measurement uncertainties (95% confidence):
- Wind speed: ±5% (calibration uncertainty)
- Rotor RPM: ±0.5 RPM (Hall sensor quantization)
- Power: ±3% (INA226 sensor accuracy)
- Combined Cp: ±8% (quadrature sum)
```

**C. Known issues:**
```
- Anemometer bearing friction increased after 4 months → recalibrated Oct 15
- Wi-Fi dropout during heavy rain (local SD logging unaffected)
- Outliers flagged but not removed (user discretion for filtering)
```

**D. Data processing steps:**
```
1. Raw ADC counts converted to engineering units
2. Air density calculated from temp/pressure (ideal gas law)
3. Cp and λ calculated per IEC 61400-12-2 equations
4. Outliers detected via Tukey fences (3×IQR) but retained with flag
```

---

### **Step 6: Ensure File Format Interoperability**

**Preferred formats:**

| Data Type | Recommended Format | Avoid |
|-----------|-------------------|-------|
| **Tabular data** | CSV (UTF-8) | Excel .xls/.xlsx (proprietary) |
| **Metadata** | YAML, JSON, XML | Word .docx |
| **Documentation** | Markdown, PDF/A | Word .docx (unless PDF/A exported) |
| **Images** | PNG, TIFF | Proprietary RAW |
| **Code** | Plain text (.py, .ino, .m) | Compiled binaries |
| **Archives** | ZIP, TAR.GZ | RAR (proprietary) |

**CSV best practices:**
- Use comma `,` as delimiter (not tab or semicolon)
- Quote text fields containing commas: `"location, description"`
- Include header row with column names
- Use ISO 8601 for timestamps: `2025-06-15T14:32:18+0530`
- Encode as UTF-8 (not ASCII or Windows-1252)

**YAML/JSON for metadata:**
- Human-readable and machine-parseable
- Supports nesting (hierarchical metadata)
- Example tools: Python `pyyaml`, MATLAB `jsondecode()`

---

### **Step 7: Provide Usage Examples**

**In your README, include:**

**A. How to load data:**

```
# Python example
import pandas as pd

df = pd.read_csv('full_dataset.csv', parse_dates=['timestamp'])
print(df.head())
```

```
% MATLAB example
data = readtable('full_dataset.csv');
summary(data)
```

**B. Quick analysis:**

```
# Calculate average power coefficient
mean_cp = df['cp'].mean()
print(f"Mean Cp: {mean_cp:.3f}")
```

**C. Column descriptions:**

```
Columns:
- timestamp: Measurement time (ISO 8601, UTC+0530)
- wind_speed_ms: Wind speed in m/s at 3.5m height
- rotor_rpm: Rotor speed in revolutions per minute
- power_w: Electrical power output in watts (DC)
- cp: Power coefficient (dimensionless, 0-0.6)
- lambda: Tip speed ratio (dimensionless)
```

---

### **Step 8: Register a Persistent Identifier (DOI)**

**What is a DOI?**
- Digital Object Identifier: permanent link to your dataset
- Format: `10.5281/zenodo.1234567`
- Resolves to: `https://doi.org/10.5281/zenodo.1234567` (never breaks)

**How to get a DOI:**

**Via Zenodo:**
1. Create account: https://zenodo.org/signup/
2. Click "Upload" → "New upload"
3. Fill in metadata form (auto-populates from `metadata.yaml` if provided)
4. Upload files (drag-and-drop or select)
5. Click "Publish"
6. DOI assigned immediately (e.g., `10.5281/zenodo.5555555`)

**Update your README and metadata with DOI after publication!**

---

## FAIR Compliance Checklist

### **Findable (F1-F4)**

- [ ] **F1:** Dataset assigned globally unique persistent identifier (DOI)
- [ ] **F2:** Data described with rich metadata (50+ fields in `metadata.yaml`)
- [ ] **F3:** Metadata includes identifier of the data (DOI referenced in metadata file)
- [ ] **F4:** Metadata registered in searchable resource (Zenodo indexed by Google Dataset Search, DataCite)

**Test:** Search Google Dataset Search for your title → Does it appear?

---

### **Accessible (A1-A2)**

- [ ] **A1.1:** Dataset retrievable via standard protocol (HTTPS)
- [ ] **A1.2:** Protocol allows authentication if needed (Zenodo supports embargoes)
- [ ] **A2:** Metadata remains accessible even if data is no longer available (Zenodo tombstone pages)

**Test:** Can you download your dataset without logging in? (If public, should be yes)

---

### **Interoperable (I1-I3)**

- [ ] **I1:** Data uses standard formats (CSV, not proprietary Excel)
- [ ] **I2:** Metadata uses controlled vocabularies (e.g., Dublin Core terms)
- [ ] **I3:** Includes qualified references to other datasets (related publications DOIs)

**Test:** Can someone open your CSV in Excel, Python, R, and MATLAB without conversion?

---

### **Reusable (R1-R1.3)**

- [ ] **R1:** Multiple accurate and relevant attributes (see Step 3)
- [ ] **R1.1:** Clear usage license (CC BY 4.0 in LICENSE.txt)
- [ ] **R1.2:** Detailed provenance (instrumentation, calibration, processing steps)
- [ ] **R1.3:** Meets community standards (IEC 61400-12-2 for wind turbine data)

**Test:** Could a stranger replicate your measurements with the metadata provided?

---

## Common FAIR Pitfalls (And How to Avoid Them)

### **Pitfall 1: "I'll organize my data later"**

**Problem:** Dataset accumulates for months, loses context, errors propagate  
**Solution:** Use standardized file naming and folder structure from day 1

---

### **Pitfall 2: Incomplete metadata**

**Problem:** Dataset description: "Wind turbine data from my thesis"  
**Solution:** Include location, dates, turbine specs, sensor models, uncertainties

**Rule of thumb:** Metadata should be 10-20% the size of the dataset (e.g., 500KB metadata for 5MB dataset)

---

### **Pitfall 3: No license**

**Problem:** "All rights reserved" by default → unusable by others  
**Solution:** Add CC BY 4.0 license file before publication

---

### **Pitfall 4: Proprietary formats**

**Problem:** `.mat` (MATLAB), `.xlsx` (Excel) require specific software  
**Solution:** Export to CSV + include original format (users choose)

---

### **Pitfall 5: No data quality documentation**

**Problem:** Dataset has outliers, gaps, but no explanation  
**Solution:** Document all known issues in README with timestamps

---

### **Pitfall 6: Broken links in metadata**

**Problem:** "Raw data available at http://mywebsite.com/data.zip" → website goes offline  
**Solution:** Upload all files to repository, use DOI for related materials

---

### **Pitfall 7: Unclear column names**

**Problem:** Columns named "x1", "y2", "output_final"  
**Solution:** Use descriptive names: "wind_speed_ms", "rotor_rpm", "power_w"

---

## Dataset Citation Format

**Once published, cite your own dataset in your thesis/papers:**

**APA style:**
```
Perera, K. M., & Supervisor, A. (2025). Power Coefficient Measurements for 500W 
Helical VAWT under Tropical Coastal Conditions [Data set]. Zenodo. 
https://doi.org/10.5281/zenodo.1234567
```

**IEEE style:**
```
 K. M. Perera and A. Supervisor, "Power coefficient measurements for 500W helical[1]
VAWT under tropical coastal conditions," Zenodo, 2025, doi: 10.5281/zenodo.1234567.
```

**BibTeX:**
```
@dataset{perera2025wind,
  author = {Perera, ABC and Supervisor, A.},
  title = {Power Coefficient Measurements for 500W Helical VAWT under Tropical Coastal Conditions},
  year = {2025},
  publisher = {Zenodo},
  doi = {10.5281/zenodo.1234567}
}
```

---

## Repository Comparison

### **Detailed Feature Comparison**

| Feature | Zenodo | figshare | IEEE DataPort | Institutional |
|---------|--------|----------|---------------|---------------|
| **Storage limit** | 50GB/dataset | Unlimited | 2TB/dataset | Varies (typically 5-20GB) |
| **DOI** | Yes (DataCite) | Yes (DataCite) | Yes (DataCite) | Usually yes |
| **Versioning** | Yes | Yes | Yes | Limited |
| **Embargo period** | Up to 50 years | Unlimited | 2 years max | Varies |
| **API access** | REST API | REST API | Limited | Varies |
| **Indexing** | Google, OpenAIRE | Google, Dimensions | IEEE Xplore | Variable |
| **Long-term guarantee** | 20+ years (CERN) | Company-backed | IEEE-backed | Institution-dependent |
| **Community** | General science | General science | Engineering-focused | Local |

---

## Advanced FAIR: Linked Data

**Beyond basic FAIR:**

**Use persistent identifiers for:**
1. **People:** ORCID (https://orcid.org/) - `0000-0002-1234-5678`
2. **Organizations:** ROR (Research Organization Registry) - `https://ror.org/012abc456`
3. **Funding:** Crossref Funder Registry - `10.13039/501100001824` (NSF example)
4. **Publications:** DOI - `10.1016/j.renene.2025.01.234`
5. **Instruments:** Wikidata IDs or manufacturer PNs

**Example in metadata:**
```
creator:
  - name: "Perera, ABC"
    orcid: "https://orcid.org/0000-0002-1234-5678"
    affiliation:
      name: "University of Moratuwa"
      ror: "https://ror.org/03kb7zv85"

funding:
  - agency: "Sri Lanka National Science Foundation"
    grant_number: "NSF/RG/2024/ME/12"
    funder_id: "https://doi.org/10.13039/501100012345"

instrumentation:
  anemometer:
    model: "Inspeed Vortex"
    manufacturer: "Inspeed LLC"
    wikidata: "https://www.wikidata.org/wiki/Q..."
```

**Benefit:** Machines can automatically link your dataset to related works, authors, institutions

---

## FAIR Tools and Validators

### **Automated FAIR Assessment**

**FAIR Evaluation Services:**
1. **FAIRshake:** https://fairshake.cloud/
   - Upload metadata, get FAIR score (0-100%)
   - Identifies gaps

2. **FAIR Evaluator:** https://fairsharing.github.io/FAIR-Evaluator-FrontEnd/
   - Tests compliance with FAIR principles

3. **Metadata Quality Assessment:**
   - CEDAR Workbench: https://metadatacenter.org/

---

### **Metadata Editors**

**Tools to help create metadata:**
- **DataCite Metadata Generator:** https://doi.datacite.org/
- **Zenodo Web UI:** Auto-generates metadata from form input

---

## Thesis/Paper Data Availability Statements

**Include in your thesis/paper:**

**Example 1 (Open data):**
```
Data Availability Statement

The full dataset generated during this study is openly available in Zenodo at:
https://doi.org/10.5281/zenodo.1234567[1]

Dataset includes:
- Raw time-series measurements (15.6 million records, 780 MB)
- Processed Cp-λ curve (binned data, 42 data points)
- Python analysis scripts
- Sensor calibration certificates

Licensed under CC BY 4.0.

 K. M. Perera, "Power Coefficient Measurements for 500W Helical VAWT...",[1]
    Zenodo, 2025, doi: 10.5281/zenodo.1234567
```

**Example 2 (Embargo):**
```
Data Availability Statement

The dataset will be made available in Zenodo (https://doi.org/10.5281/zenodo.1234567) 
after a 12-month embargo period (until June 1, 2026) to allow for publication of 
related research. Metadata is publicly available immediately.

For early access requests, contact: ABC.perera@example.com
```

---

## Case Study: FAIR-Compliant Wind Turbine Dataset

**Dataset:** Power Coefficient Measurements for 500W Helical VAWT...

**FAIR score:** 98/100 (FAIRshake evaluation)

**What made it FAIR:**
- ✅ Rich metadata (87 fields in YAML)
- ✅ Clear license (CC BY 4.0)
- ✅ DOI assigned (10.5281/zenodo.1234567)
- ✅ Open formats (CSV, YAML, PDF)
- ✅ Comprehensive README (8 pages)
- ✅ Uncertainty quantification documented
- ✅ Analysis code included (reproducible)
- ✅ Indexed by Google Dataset Search (findable)

**Impact (6 months post-publication):**
- 143 downloads
- 12 citations (dataset DOI cited in papers)
- 3 reuse cases (other researchers testing turbines)

**Time investment:** ~20 hours total documentation (spread over 6-month campaign)

---

## Summary: Minimal FAIR Checklist

**Absolute minimum to be FAIR-compliant:**

1. [ ] Upload to reputable repository (Zenodo/figshare/IEEE DataPort)
2. [ ] Include README with:
   - [ ] Title, authors, date
   - [ ] Description (200+ words)
   - [ ] Data format explanation
   - [ ] Column definitions
   - [ ] Known issues/limitations
3. [ ] Include machine-readable metadata (YAML/JSON)
4. [ ] Use open formats (CSV, not Excel)
5. [ ] Add license (CC BY 4.0 recommended)
6. [ ] Include data quality metrics (completeness, uncertainties)
7. [ ] Assign DOI (automatic with repository)
8. [ ] Link to related publications

**Time required:** 4-8 hours if using templates from this repository

---

## Additional Resources

### **Guidelines and Standards**
- **FAIR Principles:** https://www.go-fair.org/fair-principles/
- **DataCite Metadata Schema:** https://schema.datacite.org/
- **IEC 61400-12-2:** Power performance measurements (wind turbine domain standard)

### **Funder Policies**
- **NSF Data Management Plan:** https://www.nsf.gov/data/
- **EU Horizon Europe:** https://ec.europa.eu/info/funding-tenders/opportunities/docs/2021-2027/horizon/guidance/programme-guide_horizon_en.pdf

### **Training**
- **FAIR Data Course:** https://www.fosteropenscience.eu/learning
- **MANTRA Research Data Management:** https://mantra.ed.ac.uk/

---

**Questions about FAIR data?** Open an issue: https://github.com/asithakal/wind-turbine-daq-guide/issues