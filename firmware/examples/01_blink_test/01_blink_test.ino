/*
 * Example 01: Blink Test
 * 
 * Purpose: Verify ESP32 is working and Arduino IDE is configured correctly
 * 
 * Learning Objectives:
 * - Basic GPIO control
 * - Using digitalWrite() and delay()
 * - Serial Monitor output
 * 
 * Hardware Required:
 * - ESP32 development board (any model)
 * - USB cable
 * 
 * Connections:
 * - None (uses built-in LED on GPIO 2)
 * 
 * Expected Behavior:
 * - Onboard LED blinks every 1 second
 * - Serial Monitor shows "LED ON" / "LED OFF" messages
 * 
 * Troubleshooting:
 * - LED not blinking? Try changing LED_PIN to 5, 13, or check your board pinout
 * - No Serial output? Check baud rate is set to 115200
 * 
 * Author: Wind Turbine DAQ Guide
 * License: MIT
 */

// Define LED pin (GPIO 2 is common for most ESP32 boards)
const int LED_PIN = 2;

void setup() {
  // Initialize serial communication at 115200 baud
  Serial.begin(115200);
  
  // Wait for serial port to connect (useful for troubleshooting)
  delay(1000);
  
  Serial.println("\n=================================");
  Serial.println("ESP32 Blink Test - Example 01");
  Serial.println("=================================");
  Serial.println("If LED is blinking, your ESP32 is working correctly!");
  Serial.println();
  
  // Set LED pin as output
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // Turn LED on
  digitalWrite(LED_PIN, HIGH);
  Serial.println("LED ON");
  delay(1000);  // Wait 1 second
  
  // Turn LED off
  digitalWrite(LED_PIN, LOW);
  Serial.println("LED OFF");
  delay(1000);  // Wait 1 second
}