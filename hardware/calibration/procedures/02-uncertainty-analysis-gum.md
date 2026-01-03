# GUM Uncertainty Analysis Methodology

**Status:** 📝 Under development

This procedure provides complete GUM (Guide to the Expression of Uncertainty in Measurement) methodology for wind speed measurements.

## What This Will Cover

- **Partial derivative derivation** (mathematical foundations)
- **Type A and Type B uncertainty evaluation**
- **Correlation considerations**
- **Coverage factors** and confidence levels
- **Worked examples** with real calibration data
- **Reporting templates** for thesis/publications

## For Now

Use the automated uncertainty calculator:

```bash
cd hardware/calibration/tools
python uncertainty-calculator.py --help
```

The calculator implements the GUM methodology. For theory, consult:
- ISO/IEC Guide 98-3:2008 (GUM)
- Book Appendix C.4 (uncertainty overview)

## Planned Content

1. Introduction to GUM concepts
2. Type A uncertainty (statistical analysis of repeated measurements)
3. Type B uncertainty (from specifications, certificates)
4. Sensitivity coefficients (partial derivatives)
5. Combined uncertainty calculation
6. Expanded uncertainty and coverage factors
7. Reporting uncertainty in publications
8. Complete worked example with real data

**Expected completion:** Q2 2026

**Want to contribute?** Share your uncertainty analysis examples!