# Multi-Point Calibration Procedure

**Status:** 📝 Under development

This procedure extends the two-point calibration in practical guides to 10+ calibration points for improved accuracy.

## What This Will Cover

- **Calibration point selection** (optimal spacing strategy)
- **Polynomial curve fitting** (2nd and 3rd order)
- **Residual analysis** for fit quality
- **Interpolation vs. extrapolation** considerations
- **Implementation in firmware** (lookup tables or polynomial)

## For Now

Use the two-point calibration from practical guide:
- [`../../../docs/calibration/anemometer_calibration.md`](../../../docs/calibration/anemometer_calibration.md)

This is sufficient for most applications (±3-5% accuracy).

## When You Need This

- **High-precision research** (±1-2% target accuracy)
- **Wide measurement range** (0.5-25 m/s)
- **Non-linear sensors** (cup anemometers at low wind speeds)
- **Standards compliance** (IEC 61400-12 requirements)

## Planned Content

1. Optimal calibration point selection (Chebyshev spacing)
2. Polynomial vs. piecewise linear fitting
3. Overfitting detection and prevention
4. Residual plots and R² interpretation
5. Implementation strategies (lookup table vs. equation)
6. Validation at intermediate points
7. Complete workflow with Python scripts

**Expected completion:** Q3 2026

**Want to contribute?** Share your multi-point calibration data!