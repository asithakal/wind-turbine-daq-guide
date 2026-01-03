#!/usr/bin/env python3
"""
Wind Speed Measurement Uncertainty Calculator
==============================================
Implements GUM (Guide to Expression of Uncertainty in Measurement) 
methodology for wind speed calibration uncertainty propagation.

Calibration equation: v = (V - OFFSET) / SCALE

Where:
  v = wind speed (m/s)
  V = measured voltage (V)
  OFFSET = zero-wind voltage (V)
  SCALE = sensitivity (V/(m/s))

Uncertainty propagation uses partial derivatives:
  u_c(v)² = (∂v/∂V)² u(V)² + (∂v/∂OFFSET)² u(OFFSET)² + (∂v/∂SCALE)² u(SCALE)²

Usage:
    python uncertainty-calculator.py \
        --offset 0.4225 --scale 0.1975 --wind-speed 5.0 \
        --u-voltage 0.01 --u-offset 0.0024 --u-scale 0.002

Reference: 
    - Main guide Appendix C.4.2
    - ISO/IEC Guide 98-3:2008 (GUM)

Author: Dr. Asitha Kulasekera
License: MIT
Repository: github.com/asithakal/wind-turbine-daq-guide
Version: 1.0
Date: 2026-01-03
"""

import argparse
import sys
import math

def calculate_voltage_from_wind_speed(wind_speed, offset, scale):
    """Calculate voltage from wind speed using calibration equation."""
    return wind_speed * scale + offset

def calculate_wind_speed_from_voltage(voltage, offset, scale):
    """Calculate wind speed from voltage using calibration equation."""
    return (voltage - offset) / scale

def calculate_partial_derivatives(voltage, offset, scale):
    """
    Calculate partial derivatives for uncertainty propagation.
    
    ∂v/∂V = 1/SCALE
    ∂v/∂OFFSET = -1/SCALE
    ∂v/∂SCALE = -(V - OFFSET) / SCALE²
    
    Returns:
        tuple: (dv_dV, dv_dOffset, dv_dScale)
    """
    dv_dV = 1.0 / scale
    dv_dOffset = -1.0 / scale
    dv_dScale = -(voltage - offset) / (scale ** 2)
    
    return dv_dV, dv_dOffset, dv_dScale

def calculate_combined_uncertainty(voltage, offset, scale, u_voltage, u_offset, u_scale):
    """
    Calculate combined standard uncertainty using GUM methodology.
    
    Args:
        voltage: Measured voltage (V)
        offset: Calibration OFFSET constant (V)
        scale: Calibration SCALE constant (V/(m/s))
        u_voltage: Standard uncertainty in voltage measurement (V)
        u_offset: Standard uncertainty in OFFSET (V)
        u_scale: Standard uncertainty in SCALE (V/(m/s))
    
    Returns:
        dict: Uncertainty analysis results
    """
    # Calculate wind speed
    wind_speed = calculate_wind_speed_from_voltage(voltage, offset, scale)
    
    # Calculate partial derivatives
    dv_dV, dv_dOffset, dv_dScale = calculate_partial_derivatives(voltage, offset, scale)
    
    # Calculate uncertainty contributions
    contrib_voltage = abs(dv_dV * u_voltage)
    contrib_offset = abs(dv_dOffset * u_offset)
    contrib_scale = abs(dv_dScale * u_scale)
    
    # Combined standard uncertainty (root sum of squares)
    u_combined = math.sqrt(
        (dv_dV * u_voltage)**2 +
        (dv_dOffset * u_offset)**2 +
        (dv_dScale * u_scale)**2
    )
    
    # Expanded uncertainty (k=2, approximately 95% confidence)
    k = 2
    U_expanded = k * u_combined
    
    # Relative uncertainty
    relative_uncertainty_pct = (u_combined / wind_speed) * 100 if wind_speed > 0 else 0
    
    return {
        'wind_speed': wind_speed,
        'voltage': voltage,
        'offset': offset,
        'scale': scale,
        'u_voltage': u_voltage,
        'u_offset': u_offset,
        'u_scale': u_scale,
        'dv_dV': dv_dV,
        'dv_dOffset': dv_dOffset,
        'dv_dScale': dv_dScale,
        'contrib_voltage': contrib_voltage,
        'contrib_offset': contrib_offset,
        'contrib_scale': contrib_scale,
        'u_combined': u_combined,
        'U_expanded': U_expanded,
        'k': k,
        'relative_uncertainty_pct': relative_uncertainty_pct
    }

def print_results(results):
    """Print formatted uncertainty analysis results."""
    
    print("\n" + "═" * 65)
    print("║" + " " * 8 + "Wind Speed Uncertainty Analysis (GUM Method)" + " " * 12 + "║")
    print("═" * 65)
    
    print("\nINPUT PARAMETERS:")
    print("─" * 65)
    print(f"Measured voltage (V):          {results['voltage']:.4f} V")
    print(f"Calibration OFFSET:            {results['offset']:.4f} V")
    print(f"Calibration SCALE:             {results['scale']:.4f} V/(m/s)")
    print(f"Calculated wind speed:         {results['wind_speed']:.2f} m/s")
    
    print("\nUNCERTAINTY CONTRIBUTIONS:")
    print("─" * 65)
    print(f"{'Source':<20} | {'Sensitivity':<11} | {'Uncertainty':<11} | {'Contribution'}")
    print("─" * 20 + "┼" + "─" * 13 + "┼" + "─" * 13 + "┼" + "─" * 13)
    print(f"{'Voltage measurement':<20} | {results['dv_dV']:>7.3f} m/s/V | {results['u_voltage']:>7.4f} V    | {results['contrib_voltage']:>7.4f} m/s")
    print(f"{'OFFSET constant':<20} | {results['dv_dOffset']:>7.3f} m/s/V | {results['u_offset']:>7.4f} V    | {results['contrib_offset']:>7.4f} m/s")
    print(f"{'SCALE constant':<20} | {results['dv_dScale']:>7.2f} m²/s  | {results['u_scale']:>7.4f} V/m/s | {results['contrib_scale']:>7.4f} m/s")
    
    print("\nCOMBINED UNCERTAINTY:")
    print("─" * 65)
    print(f"Standard uncertainty u_c(v):   {results['u_combined']:.4f} m/s")
    print(f"Expanded uncertainty U(k={results['k']}):   {results['U_expanded']:.4f} m/s ({int((1 - 1/results['k']**2)*100)}% confidence)")
    print(f"Relative uncertainty:          {results['relative_uncertainty_pct']:.2f}%")
    
    print("\nRESULT:")
    print("═" * 65)
    print(f"Wind speed: {results['wind_speed']:.2f} ± {results['U_expanded']:.2f} m/s (k={results['k']})")
    print("═" * 65)
    
    print("\nINTERPRETATION:")
    print("─" * 65)
    if results['relative_uncertainty_pct'] < 3:
        quality = "Excellent"
        recommendation = "Suitable for publication-quality research."
    elif results['relative_uncertainty_pct'] < 5:
        quality = "Good"
        recommendation = "Suitable for most research applications."
    elif results['relative_uncertainty_pct'] < 10:
        quality = "Acceptable"
        recommendation = "Suitable for preliminary studies."
    else:
        quality = "Poor"
        recommendation = "Consider recalibration or better equipment."
    
    print(f"Uncertainty quality: {quality}")
    print(f"Recommendation: {recommendation}")
    
    print("\nDOMINANT UNCERTAINTY SOURCE:")
    print("─" * 65)
    max_contrib = max(results['contrib_voltage'], results['contrib_offset'], results['contrib_scale'])
    if max_contrib == results['contrib_voltage']:
        dominant = "Voltage measurement"
        advice = "Use a more accurate multimeter or ADC."
    elif max_contrib == results['contrib_offset']:
        dominant = "OFFSET calibration"
        advice = "Improve zero-point measurement procedure (more readings, better environment)."
    else:
        dominant = "SCALE calibration"
        advice = "Use more calibration points or better reference anemometer."
    
    print(f"Largest contributor: {dominant} ({max_contrib:.4f} m/s)")
    print(f"To improve accuracy: {advice}")
    print()

def main():
    parser = argparse.ArgumentParser(
        description='GUM-compliant wind speed uncertainty calculator',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Basic usage with default uncertainties
  python uncertainty-calculator.py --offset 0.4225 --scale 0.1975 --wind-speed 5.0
  
  # Full specification
  python uncertainty-calculator.py \\
    --offset 0.4225 --scale 0.1975 --wind-speed 5.0 \\
    --u-voltage 0.01 --u-offset 0.0024 --u-scale 0.002

Default uncertainties:
  - Voltage: 0.01 V (typical for 4.5-digit multimeter)
  - OFFSET: 0.002 V (typical for 10-reading zero-point calibration)
  - SCALE: 0.002 V/(m/s) (typical for 2-point calibration)

For more information, see: hardware/calibration/README.md
        """
    )
    
    # Required arguments
    parser.add_argument('--offset', type=float, required=True,
                       help='Calibration OFFSET constant (V)')
    parser.add_argument('--scale', type=float, required=True,
                       help='Calibration SCALE constant (V/(m/s))')
    parser.add_argument('--wind-speed', type=float, required=True,
                       help='Wind speed at which to evaluate uncertainty (m/s)')
    
    # Optional uncertainty arguments with defaults
    parser.add_argument('--u-voltage', type=float, default=0.01,
                       help='Standard uncertainty in voltage measurement (V) [default: 0.01]')
    parser.add_argument('--u-offset', type=float, default=0.002,
                       help='Standard uncertainty in OFFSET (V) [default: 0.002]')
    parser.add_argument('--u-scale', type=float, default=0.002,
                       help='Standard uncertainty in SCALE (V/(m/s)) [default: 0.002]')
    
    args = parser.parse_args()
    
    # Validate inputs
    if args.scale <= 0:
        print("ERROR: SCALE must be positive", file=sys.stderr)
        sys.exit(1)
    
    if args.wind_speed < 0:
        print("ERROR: Wind speed cannot be negative", file=sys.stderr)
        sys.exit(1)
    
    if args.u_voltage < 0 or args.u_offset < 0 or args.u_scale < 0:
        print("ERROR: Uncertainties cannot be negative", file=sys.stderr)
        sys.exit(1)
    
    # Calculate voltage from wind speed
    voltage = calculate_voltage_from_wind_speed(args.wind_speed, args.offset, args.scale)
    
    # Perform uncertainty analysis
    results = calculate_combined_uncertainty(
        voltage, args.offset, args.scale,
        args.u_voltage, args.u_offset, args.u_scale
    )
    
    # Print results
    print_results(results)
    
    return 0

if __name__ == '__main__':
    sys.exit(main())