# Document Templates

Ready-to-use templates for FAIR data publication, project documentation, and thesis writing.

## 📂 Folder Contents

### **`documentation/`** ⭐ **(Most Used)**
Templates for publishing datasets and maintaining project records.

| Template | Purpose | When to Use |
|----------|---------|-------------|
| `dataset_README_template.md` | Dataset documentation (8-page format) | Publishing data to Zenodo/repository |
| `metadata_template.yaml` | Machine-readable metadata (Dublin Core) | Required for FAIR compliance |
| `lab_notebook_template.md` | Daily research log | Throughout project (weekly entries) |
| `requirements_template.xlsx` | Requirements Traceability Matrix | Project planning phase (Section 7.1) |
| `test_plan_template.xlsx` | Validation checklist | System testing phase (Section 7.3) |

### **`system_design/`**
Visual and textual templates for system architecture documentation.

- `architecture_diagram.drawio`: Editable system block diagram (Draw.io format)
- `architecture_diagram.png`: Exported image (for embedding in reports)
- `interface_spec_template.md`: Interface Control Document template

### **`publication/`**
Academic writing templates for thesis chapters and conference papers.

- `conference_paper_template.tex`: LaTeX template (IEEE format)
- `thesis_chapter_template.docx`: Methodology chapter outline
- `dataset_publication_checklist.md`: Pre-submission checklist

---

## 🚀 Quick Start Guide

### **1. Dataset Documentation (For Zenodo/Open Repository)**

**Use Case:** You've completed your 6-month data collection campaign and want to publish the dataset.

**Required Files:**
1. `dataset_README_template.md` → Rename to `README.md`
2. `metadata_template.yaml` → Rename to `metadata.yaml`

**Steps:**

#### **A. Copy templates to your dataset folder**

```
# Windows CMD
copy templates\documentation\dataset_README_template.md C:\MyProject\Dataset\README.md
copy templates\documentation\metadata_template.yaml C:\MyProject\Dataset\metadata.yaml
```

#### **B. Edit README.md**

Open `README.md` in any text editor. Find and replace placeholders:

| Placeholder | Replace With | Example |
|-------------|--------------|---------|
| `[YOUR NAME]` | Your full name | `Kasun Perera` |
| `[TITLE]` | Dataset title | `Power Coefficient Measurements for 500W Helical VAWT...` |
| `[DATE]` | Collection period | `June-December 2025` |
| `[LOCATION]` | Test site | `University of Moratuwa, Sri Lanka` |
| `[DOI]` | Zenodo DOI (after upload) | `10.5281/zenodo.1234567` |
| `[EMAIL]` | Contact email | `kasun.perera@uom.lk` |

#### **C. Edit metadata.yaml**

Same process—replace all `[BRACKETS]` with your project details.

**Critical fields:**
- `title`: Descriptive dataset title (< 200 characters)
- `creator`: Authors with ORCID IDs
- `date_coverage_start/end`: Exact measurement period
- `spatial`: GPS coordinates of test site
- `instrumentation`: Sensor models and specs

**Validation:** Use online YAML validator (https://www.yamllint.com/) to check syntax.

#### **D. Upload to Zenodo**

1. Go to https://zenodo.org/deposit/new
2. Upload files:
   - `full_dataset.csv` (your data)
   - `README.md` (edited template)
   - `metadata.yaml` (edited template)
   - `technical_report.pdf` (your thesis chapter or paper)
3. Zenodo auto-fills metadata from `metadata.yaml`
4. Publish → Receive DOI → Update DOI field in README

**Done!** Your dataset is now FAIR-compliant [citation:file:2].

---

### **2. Lab Notebook (Daily Documentation)**

**Use Case:** Supervisor requires weekly progress logs.

**Steps:**

1. Copy `lab_notebook_template.md` to your project folder
2. Rename to `YYYY-MM-DD_Lab_Notebook.md` (one file per week)
3. Fill in sections daily:
   - **Objective:** What you planned to do
   - **Activities:** What you actually did
   - **Results:** Key findings, data, photos
   - **Challenges:** Problems encountered
   - **Next Steps:** Tomorrow's plan

**Example entry:**

```
## 2025-06-15 (Monday)

### Objective
Install ESP32 on turbine and test SD card logging.

### Activities
- Mounted weatherproof enclosure on turbine tower (2 hours)
- Wired all sensors per schematic (1.5 hours)
- Uploaded firmware v1.2.3
- Tested continuous logging for 4 hours

### Results
- ✅ SD card logging functional (4.2 MB file generated)
- ✅ All sensors reading within expected ranges
- ⚠️ Wi-Fi signal weak at site (only 1 bar)

### Challenges
- Cable gland leaked during rain test → Added extra silicone sealant
- Hall sensor false triggers below 50 RPM → Added 100ms debounce in code

### Next Steps
- Leave system running overnight
- Check data quality tomorrow morning
- If stable, begin official data collection campaign

### Attachments
- Photo: `IMG_20250615_143021.jpg` (enclosure installation)
- Data file: `test_20250615.csv`
```

---

### **3. Requirements Traceability Matrix (RTM)**

**Use Case:** Planning your DAQ system (before building hardware).

**Steps:**

1. Open `requirements_template.xlsx` in Excel
2. Fill in tabs:
   - **Tab 1 (Requirements):** List functional and non-functional requirements
   - **Tab 2 (Design):** Map requirements to design choices
   - **Tab 3 (Verification):** Define test methods

**Example rows:**

| Req ID | Requirement | Priority | Design Solution | Verification Method | Status |
|--------|-------------|----------|-----------------|---------------------|--------|
| FR-1 | Measure wind speed 0.5-25 m/s | High | Inspeed Vortex anemometer | Wind tunnel test | ✅ Pass |
| NFR-2 | Withstand 95% humidity | High | IP67 enclosure + conformal coating | Humidity chamber test | ✅ Pass |
| FR-5 | Log at 1 Hz for 6 months | High | 32 GB SD card + CSV format | Endurance test (1 week) | ✅ Pass |

**Benefit:** Traceability for thesis methodology section [citation:file:3].

---

### **4. Test Plan Template**

**Use Case:** Validating your system before deployment.

**Steps:**

1. Open `test_plan_template.xlsx`
2. List all tests from Section 7.3 (book):
   - Sensor accuracy tests
   - Data integrity tests
   - Environmental stress tests
   - Long-term stability tests
3. Document results with pass/fail
4. Attach test data and photos

**Example:**

| Test ID | Test Description | Acceptance Criteria | Result | Evidence |
|---------|------------------|---------------------|--------|----------|
| HW-01 | Anemometer calibration | ±5% of reference | ✅ Pass | `calibration_cert_20250520.pdf` |
| SW-03 | SD card write reliability | Zero data loss over 72h | ✅ Pass | `sd_test_log.csv` |
| ENV-02 | Rain ingress test | No water inside after 30min spray | ✅ Pass | `IMG_20250522_rain_test.jpg` |

---

## 📝 Template Details

### **Dataset README Template**

**Format:** Markdown (8 pages when rendered)  
**Sections:**
1. Title and authors
2. Quick start instructions
3. Data format and column definitions
4. Methodology and instrumentation
5. Data quality notes
6. Usage and citation
7. Contact information
8. Version history

**Compliance:** Meets Zenodo, figshare, IEEE DataPort requirements.

---

### **Metadata YAML Template**

**Format:** YAML (machine-readable)  
**Standards:** Dublin Core + domain extensions  
**Fields:** 50+ metadata elements including:
- Creator (ORCID integration)
- Geospatial coverage (WGS84)
- Temporal coverage (ISO 8601)
- Instrumentation specifications
- Uncertainty quantification
- Funding acknowledgments

**Validation:** Compatible with DataCite schema v4.4.

---

### **Lab Notebook Template**

**Format:** Markdown (version-control friendly)  
**Structure:**
- Daily entries with timestamps
- Objective → Activities → Results → Challenges
- Links to photos, data files, code commits
- Weekly summary section

**Best Practice:** Commit to Git repository for tamper-proof record.

---

### **Requirements Template (RTM)**

**Format:** Excel (.xlsx) with 3 tabs  
**Methodology:** V-Model traceability (Section 7.1)  
**Columns:**
- Requirement ID (hierarchical: SYS-1, SYS-1.1, etc.)
- Description, priority, source
- Design solution, verification method
- Test results, status, notes

**Export:** Can be converted to CSV for thesis appendix.

---

### **Test Plan Template**

**Format:** Excel (.xlsx)  
**Compliance:** IEC 61400-12-2 testing guidelines  
**Test Categories:**
- Hardware verification
- Software validation
- Environmental qualification
- Calibration procedures

**Integration:** Links to [`../docs/calibration/`](../docs/calibration/) procedures.

---

## 🎓 Academic Writing Templates

### **Conference Paper Template (LaTeX)**

**Format:** IEEE two-column format  
**Sections:** Abstract, Introduction, Methodology, Results, Conclusion  
**Usage:**
```
# Compile with pdflatex
pdflatex conference_paper_template.tex
bibtex conference_paper_template
pdflatex conference_paper_template.tex
pdflatex conference_paper_template.tex
```

**Target Journals/Conferences:**
- *Wind Engineering*
- *Renewable Energy*
- IEEE ICIT (International Conference on Industrial Technology)

---

### **Thesis Chapter Template (Word)**

**Format:** Microsoft Word (.docx)  
**Chapter:** Methodology (typically Chapter 3 or 4)  
**Sections:**
- Experimental setup
- Instrumentation
- Data collection procedure
- Data processing pipeline
- Uncertainty analysis

**Styles:** Pre-formatted with Heading 1/2/3, captions, references.

---

### **Dataset Publication Checklist**

**Format:** Markdown checklist  
**Items:** 42-point pre-submission checklist covering:
- Data completeness
- Metadata quality
- File formats
- Documentation
- Licensing
- DOI registration
- Repository selection

**Use:** Check off items before final Zenodo upload.

---

## 🔧 Customization Tips

### **Adapting Templates to Your Project**

**Different turbine type (HAWT instead of VAWT)?**
- Update swept area formula in metadata: `π*R²` instead of `2*R*H`
- Modify aerodynamic terms in README

**Indoor wind tunnel instead of field test?**
- Change spatial metadata (latitude/longitude → lab location)
- Note controlled environment in methodology section

**Different country?**
- Update funding agencies in metadata
- Adjust environmental conditions (humidity, temperature ranges)

---

## 🌍 Language Adaptations

All templates are in English but can be translated:

**Tip for non-native speakers:**
- Use dataset README template as-is (English is standard for international datasets)
- Translate lab notebook for local supervisor
- Provide bilingual thesis abstract

---

## 📖 Related Documentation

- **FAIR Data Principles:** See book Section 6 (Data Management)
- **Requirements Engineering:** See book Section 7.1
- **Verification Methods:** See book Section 7.3

---

## 🤝 Contributing

Have a better template format? Spotted a typo?

Submit improvements via [Pull Request](https://github.com/asithakal/wind-turbine-daq-guide/pulls).

---

## 📜 License

All templates: CC BY 4.0 (Creative Commons Attribution)

**You may:**
- ✅ Use for commercial or non-commercial projects
- ✅ Modify and adapt
- ✅ Share with attribution

**You must:**
- 📌 Cite original source (this repository)

See [LICENSE](../LICENSE) for full terms.