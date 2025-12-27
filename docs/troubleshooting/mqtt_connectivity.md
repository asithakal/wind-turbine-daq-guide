# MQTT and WiFi Connectivity Troubleshooting

Complete guide for diagnosing and fixing WiFi, MQTT, and cloud connectivity issues.

**Quick navigation:**
- [WiFi Connection Problems](#wifi-connection-problems)
- [MQTT Broker Connection](#mqtt-broker-connection)
- [Publish/Subscribe Issues](#publishsubscribe-issues)
- [Network Performance](#network-performance)
- [Security and Authentication](#security-and-authentication)

---

## WiFi Connection Problems

### **Error: "WiFi connection failed" or "Connecting..."**

**Serial Monitor shows:**
```
Connecting to WiFi......................
WiFi connection failed!
```

**Diagnostic flowchart:**

```
WiFi won't connect?
│
├─ Check credentials:
│  ├─ SSID correct? (case-sensitive!)
│  ├─ Password correct? (case-sensitive!)
│  └─ Hidden network? (must set BSSID)
│
├─ Check router compatibility:
│  ├─ 2.4 GHz network? (ESP32 doesn't support 5 GHz!)
│  ├─ Open/WPA/WPA2? (WPA3 not supported by default)
│  └─ DHCP enabled?
│
├─ Check signal strength:
│  ├─ Too far from router? (ESP32 range ~30m indoors)
│  ├─ Metal enclosure blocking signal?
│  └─ See "Signal Strength" section
│
└─ Check ESP32:
   ├─ WiFi antenna connected? (some boards external antenna)
   ├─ Brownout during WiFi init? (high power draw)
   └─ Try different ESP32 board
```

---

### **Solution 1: Verify Credentials**

**Test code with diagnostic output:**

```
#include <WiFi.h>

const char* ssid = "YourNetworkName";      // Check spelling!
const char* password = "YourPassword123";  // Check case!

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== WiFi Connection Test ===");
  Serial.print("Connecting to: ");
  Serial.println(ssid);
  Serial.print("Password: ");
  Serial.println(password);  // Remove in production!
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
    
    // Print status code for debugging:
    Serial.print(" (Status: ");
    Serial.print(WiFi.status());
    Serial.print(")");
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Signal Strength (RSSI): ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
  } else {
    Serial.println("\nConnection FAILED!");
    printFailureReason(WiFi.status());
  }
}

void loop() {}

void printFailureReason(int status) {
  Serial.print("Failure reason: ");
  switch(status) {
    case WL_NO_SSID_AVAIL:
      Serial.println("SSID not found - Check network name!");
      break;
    case WL_CONNECT_FAILED:
      Serial.println("Connection failed - Wrong password?");
      break;
    case WL_IDLE_STATUS:
      Serial.println("WiFi idle - Still trying to connect");
      break;
    case WL_DISCONNECTED:
      Serial.println("Disconnected - Check router settings");
      break;
    default:
      Serial.print("Unknown status code: ");
      Serial.println(status);
  }
}
```

**WiFi status codes:**

| Code | Constant | Meaning | Solution |
|------|----------|---------|----------|
| 0 | WL_IDLE_STATUS | Connecting | Wait longer (increase timeout) |
| 1 | WL_NO_SSID_AVAIL | Network not found | Check SSID spelling, router on? |
| 3 | WL_CONNECTED | Success! | N/A |
| 4 | WL_CONNECT_FAILED | Wrong password | Verify password, case-sensitive |
| 6 | WL_DISCONNECTED | Lost connection | Signal too weak or interference |

---

### **Solution 2: Check 2.4 GHz vs 5 GHz**

**Problem:** ESP32 only supports 2.4 GHz WiFi, not 5 GHz

**How to check router:**

1. **Router admin page:**
   - Open browser: http://192.168.1.1 (or router IP)
   - Login with admin credentials
   - Look for "2.4 GHz" and "5 GHz" network settings
   - Ensure 2.4 GHz network is enabled

2. **Dual-band routers:**
   - May broadcast "YourNetwork" (5 GHz) and "YourNetwork-2.4G" (2.4 GHz)
   - Connect ESP32 to "-2.4G" network
   - Or separate SSIDs manually in router settings

3. **Test with phone:**
   - Most phones can see both 2.4 GHz and 5 GHz networks
   - Check WiFi analyzer app (e.g., "WiFi Analyzer" on Android)
   - Verify 2.4 GHz network visible at ESP32 location

---

### **Solution 3: Signal Strength Diagnosis**

**Measure signal strength (RSSI):**

```
void printWiFiSignal() {
  if (WiFi.status() == WL_CONNECTED) {
    int rssi = WiFi.RSSI();
    Serial.print("Signal Strength: ");
    Serial.print(rssi);
    Serial.print(" dBm - ");
    
    if (rssi > -50) {
      Serial.println("Excellent");
    } else if (rssi > -60) {
      Serial.println("Good");
    } else if (rssi > -70) {
      Serial.println("Fair - May have issues");
    } else {
      Serial.println("Weak - Expect connection problems!");
    }
  }
}
```

**RSSI guide:**

| RSSI (dBm) | Signal Quality | Expected Performance |
|------------|----------------|----------------------|
| -30 to -50 | Excellent | Stable, fast |
| -50 to -60 | Good | Reliable |
| -60 to -70 | Fair | Occasional drops |
| -70 to -80 | Weak | Frequent disconnects |
| < -80 | Very weak | Unusable |

**Improve signal:**

1. **Move ESP32 closer to router**
2. **Use external antenna** (if board supports)
3. **Add WiFi repeater/extender**
4. **Remove metal objects between ESP32 and router**
5. **Change router channel** (less congested channels 1, 6, 11)
6. **Reorient ESP32** (antenna direction matters)

---

### **Solution 4: Hidden Network Setup**

**If network is hidden:**

```
WiFi.begin(ssid, password);  // Won't work for hidden network

// Use this instead:
uint8_t bssid = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};  // Router MAC address[1]
WiFi.begin(ssid, password, 0, bssid);
```

**How to find BSSID (router MAC address):**

**Windows:**
```
netsh wlan show interfaces
# Look for "BSSID" field
```

**macOS/Linux:**
```
# macOS:
/System/Library/PrivateFrameworks/Apple80211.framework/Versions/Current/Resources/airport -I

# Linux:
iwconfig
```

---

### **Solution 5: Power-related WiFi Issues**

**Symptom:** WiFi connects but immediately disconnects, or "brownout" errors

**Cause:** WiFi initialization draws 150-250 mA, may exceed power supply

**Solutions:**

```
// 1. Reduce CPU frequency during WiFi connect:
setCpuFrequencyMhz(80);  // Reduce from 240 MHz
WiFi.begin(ssid, password);
// Wait for connection...
setCpuFrequencyMhz(240);  // Restore after connected

// 2. Add delay after WiFi init:
WiFi.begin(ssid, password);
delay(100);  // Let power stabilize

// 3. Reduce transmit power:
WiFi.setTxPower(WIFI_POWER_11dBm);  // Default is 20dBm
```

**Hardware solutions:**
- Add 100µF capacitor between 3.3V and GND (close to ESP32)
- Use external 5V/2A power supply (not USB)
- Use powered USB hub

---

## MQTT Broker Connection

### **Error: "MQTT connection failed" after WiFi connects**

**Serial Monitor shows:**
```
WiFi connected!
Connecting to MQTT broker...
MQTT connection failed, rc=-2
```

**Return codes (rc):**

| Code | Meaning | Common Cause |
|------|---------|--------------|
| -4 | MQTT_CONNECTION_TIMEOUT | Broker unreachable |
| -3 | MQTT_CONNECTION_LOST | Network interrupted |
| -2 | MQTT_CONNECT_FAILED | Wrong broker address/port |
| -1 | MQTT_DISCONNECTED | Broker refused connection |
| 0 | MQTT_CONNECTED | Success! |
| 1 | MQTT_CONNECT_BAD_PROTOCOL | Wrong MQTT version |
| 2 | MQTT_CONNECT_BAD_CLIENT_ID | Invalid client ID |
| 3 | MQTT_CONNECT_UNAVAILABLE | Broker down |
| 4 | MQTT_CONNECT_BAD_CREDENTIALS | Wrong username/password |
| 5 | MQTT_CONNECT_UNAUTHORIZED | Authentication failed |

---

### **Solution 1: Verify Broker Settings**

**Test code with diagnostics:**

```
#include <WiFi.h>
#include <PubSubClient.h>

const char* mqtt_server = "mqtt.example.com";  // Or IP: "192.168.1.100"
const int mqtt_port = 1883;                    // Default: 1883 (or 8883 for TLS)
const char* mqtt_user = "username";            // Leave "" if no auth
const char* mqtt_password = "password";        // Leave "" if no auth
const char* mqtt_client_id = "ESP32_Turbine_001";  // Unique ID

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  
  // Connect WiFi first (see WiFi section above)
  connectWiFi();
  
  // Configure MQTT
  client.setServer(mqtt_server, mqtt_port);
  
  // Connect to broker
  connectMQTT();
}

void connectMQTT() {
  Serial.print("Connecting to MQTT broker: ");
  Serial.print(mqtt_server);
  Serial.print(":");
  Serial.println(mqtt_port);
  
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    
    // Try to connect
    boolean connected;
    if (strlen(mqtt_user) > 0) {
      // With authentication
      connected = client.connect(mqtt_client_id, mqtt_user, mqtt_password);
    } else {
      // Without authentication
      connected = client.connect(mqtt_client_id);
    }
    
    if (connected) {
      Serial.println("Connected!");
      Serial.print("Client ID: ");
      Serial.println(mqtt_client_id);
    } else {
      int rc = client.state();
      Serial.print("Failed, rc=");
      Serial.print(rc);
      Serial.print(" - ");
      printMQTTError(rc);
      Serial.println("Retrying in 5 seconds...");
      delay(5000);
    }
  }
}

void printMQTTError(int rc) {
  switch(rc) {
    case -4:
      Serial.println("TIMEOUT - Broker unreachable (check IP/port)");
      break;
    case -2:
      Serial.println("CONNECT FAILED - Wrong broker address?");
      break;
    case 1:
      Serial.println("BAD PROTOCOL - Try MQTT v3.1.1");
      break;
    case 2:
      Serial.println("BAD CLIENT ID - Change to unique ID");
      break;
    case 4:
      Serial.println("BAD CREDENTIALS - Wrong username/password");
      break;
    case 5:
      Serial.println("UNAUTHORIZED - Check broker ACL permissions");
      break;
    default:
      Serial.println("Unknown error");
  }
}

void loop() {
  if (!client.connected()) {
    connectMQTT();
  }
  client.loop();
}
```

---

### **Solution 2: Test Broker Reachability**

**Before blaming ESP32 code, verify broker is reachable:**

**Test A: Ping broker**

```
# From computer on same network:
ping mqtt.example.com
# or
ping 192.168.1.100

# Should see replies. If "Request timed out" → Broker down or firewall blocking
```

**Test B: Test with MQTT client (mosquitto)**

```
# Install mosquitto client:
# Windows: https://mosquitto.org/download/
# macOS: brew install mosquitto
# Linux: sudo apt install mosquitto-clients

# Subscribe to test topic:
mosquitto_sub -h mqtt.example.com -p 1883 -t test/topic -v

# In another terminal, publish:
mosquitto_pub -h mqtt.example.com -p 1883 -t test/topic -m "Hello"

# If this works → Broker OK, issue is ESP32 code
# If fails → Broker issue (check broker logs, firewall)
```

**Test C: Online MQTT broker (for testing)**

Use public test broker:
```
const char* mqtt_server = "test.mosquitto.org";  // Public broker
const int mqtt_port = 1883;
const char* mqtt_user = "";  // No auth
const char* mqtt_password = "";
```

**If this works:** Your broker has connectivity/firewall issues

---

### **Solution 3: Firewall and Port Issues**

**Common causes:**

1. **Router firewall** blocking MQTT port (1883/8883)
2. **Broker firewall** not allowing ESP32 IP
3. **NAT/port forwarding** misconfigured (if broker external)

**Solutions:**

**A. Check broker firewall (if self-hosted):**

```
# Linux (allow MQTT port):
sudo ufw allow 1883/tcp
sudo ufw reload

# Check if port open:
sudo netstat -tuln | grep 1883
# Should show: 0.0.0.0:1883 LISTEN
```

**B. Test with broker on same network first:**
- Avoid internet/firewall complications
- Use broker LAN IP (e.g., 192.168.1.100)
- After working locally, tackle remote connection

**C. Use MQTT over WebSockets (bypasses some firewalls):**
```
// Not supported by PubSubClient library by default
// Use alternative library: "MQTT" by Joel Gaehwiler
```

---

## Publish/Subscribe Issues

### **Problem: Publish returns false, message not sent**

**Code:**
```
boolean success = client.publish("turbine/data", "test message");
if (!success) {
  Serial.println("Publish failed!");
}
```

**Causes:**

**1. Message too large**

**Default max:** 256 bytes (including topic)

**Solution:** Increase buffer size in library

```
// Option A: Edit PubSubClient.h (line ~26):
#define MQTT_MAX_PACKET_SIZE 512  // Increase from 256

// Option B: Set in code (PubSubClient v2.8+):
client.setBufferSize(512);
```

**2. Not connected to broker**

```
// Always check connection before publish:
void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();  // MUST call loop()!
  
  // Now publish:
  client.publish("turbine/power", String(powerValue).c_str());
}
```

**3. QoS level issues**

```
// PubSubClient only supports QoS 0 (fire and forget)
// For QoS 1/2, use different library:
// - "MQTT" by Joel Gaehwiler
// - "EspMQTTClient" by Patrick Lapointe
```

---

### **Problem: Not receiving subscribed messages**

**Code:**
```
client.subscribe("turbine/command");
// Callback never fires!
```

**Diagnostic checklist:**

**1. Callback function set?**

```
void messageCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message received: ");
  Serial.println(topic);
}

void setup() {
  // ... WiFi, MQTT connect ...
  client.setCallback(messageCallback);  // ← DON'T FORGET THIS!
  client.subscribe("turbine/command");
}
```

**2. client.loop() called regularly?**

```
void loop() {
  client.loop();  // ← MUST call every loop iteration!
  // This processes incoming messages
}
```

**3. Topic matches exactly?**

```
// MQTT topics are case-sensitive:
// Subscribe: "turbine/command" 
// Publish:   "Turbine/Command"  ← WON'T MATCH!

// Wildcards:
client.subscribe("turbine/#");  // Matches turbine/command, turbine/data, etc.
client.subscribe("turbine/+/data");  // Matches turbine/unit1/data, turbine/unit2/data
```

**4. Test with external publisher:**

```
# From computer, publish to topic:
mosquitto_pub -h mqtt.example.com -t turbine/command -m "test123"

# ESP32 should receive this
# If not → Subscription issue in code
# If yes → Problem with your publisher code
```

---

## Network Performance

### **Problem: Data transmission slow or blocking**

**Symptom:** 1 Hz logging works, but WiFi transmission causes delays

**Solution: Non-blocking MQTT**

```
// BAD: Blocks for seconds if WiFi disconnected
void loop() {
  if (!client.connected()) {
    reconnect();  // Blocks!
  }
  client.publish("turbine/data", data);  // May block!
  delay(1000);
}

// GOOD: Non-blocking with timeout
unsigned long lastReconnectAttempt = 0;

void loop() {
  // Non-blocking reconnect (only try once per 5 seconds)
  if (!client.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      if (reconnect()) {
        lastReconnectAttempt = 0;  // Reset on success
      }
    }
  } else {
    client.loop();
  }
  
  // Continue with data logging regardless of MQTT status
  logDataToSD();  // Primary: SD card (always works)
  
  // Secondary: Send to cloud (best effort)
  if (client.connected()) {
    client.publish("turbine/data", data);
  }
}
```

**Key principle:** **Local logging (SD) = primary, cloud = secondary**

---

### **Problem: High data rate overwhelms MQTT**

**Scenario:** 10 Hz sampling, trying to send every sample via MQTT

**Issue:** WiFi can't keep up, messages dropped

**Solutions:**

**1. Reduce transmission rate (aggregate):**

```
#define SAMPLES_PER_TRANSMISSION 10

float windSpeedSum = 0;
int sampleCount = 0;

void loop() {
  // Collect sample (10 Hz):
  float windSpeed = readWindSpeed();
  windSpeedSum += windSpeed;
  sampleCount++;
  
  // Transmit every 10 samples (1 Hz):
  if (sampleCount >= SAMPLES_PER_TRANSMISSION) {
    float avgWindSpeed = windSpeedSum / sampleCount;
    
    char payload;[2]
    snprintf(payload, sizeof(payload), "{\"wind_avg\":%.2f}", avgWindSpeed);
    client.publish("turbine/data", payload);
    
    // Reset
    windSpeedSum = 0;
    sampleCount = 0;
  }
  
  delay(100);  // 10 Hz
}
```

**2. Batch messages (JSON array):**

```
// Instead of 10 separate publishes:
// {"wind":7.2}
// {"wind":7.3}
// ...

// Send one message:
// {"samples":[7.2,7.3,7.1,7.4,7.2,7.3,7.5,7.4,7.3,7.2]}

String buildBatch() {
  StaticJsonDocument<512> doc;
  JsonArray samples = doc.createNestedArray("samples");
  
  for (int i = 0; i < 10; i++) {
    samples.add(buffer[i]);
  }
  
  String output;
  serializeJson(doc, output);
  return output;
}
```

---

## Security and Authentication

### **Problem: TLS/SSL connection fails**

**Error:**
```
MQTT connection failed, rc=-2
```

**ESP32 TLS setup (advanced):**

```
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

WiFiClientSecure secureClient;
PubSubClient client(secureClient);

void setup() {
  // ... WiFi setup ...
  
  // Set CA certificate (broker's certificate authority)
  secureClient.setCACert(root_ca);
  
  // Or skip verification (INSECURE! Testing only):
  secureClient.setInsecure();
  
  client.setServer(mqtt_server, 8883);  // TLS port: 8883
}

const char* root_ca = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ
... (rest of certificate) ...
-----END CERTIFICATE-----
)EOF";
```

**Easier:** Use non-TLS for local network (1883), only use TLS for internet-facing brokers

---

## Debugging Tools

### **MQTT Message Monitor**

```
void printMQTTDiagnostics() {
  Serial.println("\n=== MQTT Diagnostics ===");
  Serial.print("WiFi Status: ");
  Serial.println(WiFi.status() == WL_CONNECTED ? "Connected" : "Disconnected");
  Serial.print("MQTT Status: ");
  Serial.println(client.connected() ? "Connected" : "Disconnected");
  Serial.print("Broker: ");
  Serial.print(mqtt_server);
  Serial.print(":");
  Serial.println(mqtt_port);
  Serial.print("Client ID: ");
  Serial.println(mqtt_client_id);
  Serial.println("========================\n");
}
```

### **Packet Capture (Wireshark)**

**For deep debugging:**

1. Install Wireshark on computer
2. Capture on WiFi interface
3. Filter: `mqtt`
4. Inspect CONNECT, PUBLISH, SUBSCRIBE packets
5. Look for errors, disconnects

---

## Quick Reference: Common Fixes

| Problem | Quick Fix |
|---------|-----------|
| **Can't connect to WiFi** | Check SSID/password spelling, ensure 2.4 GHz |
| **MQTT rc=-4** | Broker unreachable (check IP/port with ping) |
| **MQTT rc=4** | Wrong username/password |
| **Publish returns false** | Message > 256 bytes (increase buffer) |
| **Not receiving subscriptions** | Add `client.loop()` in loop(), set callback |
| **WiFi keeps disconnecting** | Signal too weak (check RSSI), add capacitor |
| **Brownout during WiFi** | Use external 5V/2A power supply |

---

## Still Having Issues?

**Provide when asking for help:**

1. **Serial Monitor output** (full, from startup)
2. **Network details:**
   - Router model, 2.4/5 GHz
   - RSSI (signal strength)
   - Broker type (Mosquitto, HiveMQ, AWS IoT, etc.)
3. **Code** (minimal example reproducing issue)
4. **Broker logs** (if self-hosted)
5. **Wireshark capture** (if advanced user)

**Post to:**
- Repository: https://github.com/asithakal/wind-turbine-daq-guide/issues
- ESP32 Forum: https://esp32.com/ (Networking section)
- MQTT Forum: https://discourse.mqtt.org/

---

**Last updated:** December 2024  
**Tested with:** ESP32-WROOM-32, PubSubClient v2.8.0, Mosquitto broker v2.0