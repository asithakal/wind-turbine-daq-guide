# Example 01: Blink Test

## Purpose

Verify your ESP32 board and Arduino IDE are configured correctly by blinking the onboard LED.

## Learning Objectives

- Configure GPIO pins as outputs
- Use `digitalWrite()` to control digital pins
- Print debug messages to Serial Monitor
- Understand `setup()` and `loop()` structure

## Hardware Required

- ESP32 development board
- USB cable (data capable, not charge-only)

## Connections

**None required** - uses built-in LED on GPIO 2 (most ESP32 boards)

## Upload Instructions

1. Open `01_blink_test.ino` in Arduino IDE
2. Select **Tools** → **Board** → **ESP32 Dev Module**
3. Select **Tools** → **Port** → (your COM port)
4. Click **Upload** button (→)
5. Wait for "Done uploading" message

## Expected Behavior

**LED:** Blinks on/off every 1 second

**Serial Monitor (115200 baud):**
```
=================================
ESP32 Blink Test - Example 01
=================================
If LED is blinking, your ESP32 is working correctly!

LED ON
LED OFF
LED ON
LED OFF
...
```

## Troubleshooting

**LED not blinking?**

- **Wrong pin:** Change `LED_PIN` to:
  - `5` (common on some boards)
  - `13` (NodeMCU-32S)
  - `22` (some WROOM boards)
- **Board variant:** Some boards have no onboard LED - connect external LED to GPIO 2 via 330Ω resistor

**No Serial output?**

- Check baud rate: **Tools** → **Serial Monitor** → Set dropdown to **115200**
- Try pressing **EN** (reset) button on board after opening Serial Monitor

**Upload fails?**

- Hold **BOOT** button during upload
- Try lower upload speed: **Tools** → **Upload Speed** → **115200**
- Check USB cable (must be data cable, not charge-only)

## Next Steps

✅ **Example 01 Complete!**

Once LED is blinking, continue to:
- [Example 02: Single Sensor](../02_single_sensor/) - Read analog wind speed sensor

## Code Explanation

```
const int LED_PIN = 2;  // GPIO 2 is onboard LED on most ESP32 boards

void setup() {
  Serial.begin(115200);      // Start serial at 115200 baud
  pinMode(LED_PIN, OUTPUT);  // Configure pin as output
}

void loop() {
  digitalWrite(LED_PIN, HIGH);  // Turn LED on (3.3V)
  delay(1000);                  // Wait 1000ms (1 second)
  
  digitalWrite(LED_PIN, LOW);   // Turn LED off (0V)
  delay(1000);                  // Wait 1 second
}
```

**Key functions:**
- `pinMode(pin, mode)` - Configure pin direction (INPUT/OUTPUT)
- `digitalWrite(pin, value)` - Set digital pin HIGH (3.3V) or LOW (0V)
- `delay(ms)` - Pause program for milliseconds
- `Serial.println(text)` - Print text to Serial Monitor with newline