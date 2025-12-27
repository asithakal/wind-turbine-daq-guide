/*
 * Configuration File - Separate for easy customization
 * Copy this file and modify for your specific setup
 */

#ifndef CONFIG_H
#define CONFIG_H

// ============================================================================
// NETWORK CONFIGURATION
// ============================================================================
#define WIFI_SSID "YourWiFiNetwork"
#define WIFI_PASSWORD "YourPassword"
#define ENABLE_WIFI true

#define MQTT_BROKER "broker.hivemq.com"
#define MQTT_PORT 1883
#define MQTT_CLIENT_ID "ESP32_Turbine_001"  // MUST BE UNIQUE!
#define ENABLE_MQTT true

// ============================================================================
// TURBINE SPECIFICATIONS
// ============================================================================
#define ROTOR_RADIUS_M 0.60
#define ROTOR_HEIGHT_M 1.50
#define SWEPT_AREA_M2 1.80
#define PULSES_PER_REV 1

// ============================================================================
// SENSOR CALIBRATION
// ============================================================================
#define ANEM_OFFSET_V 0.4
#define ANEM_SCALE_V_PER_MS 0.2
#define VOLTAGE_DIVIDER_RATIO 2.0

// ============================================================================
// TIMEZONE
// ============================================================================
#define TIMEZONE "IST-5:30"  // Sri Lanka/India

#endif