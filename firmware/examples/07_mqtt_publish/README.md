# Example 07: MQTT Cloud Transmission

## Purpose

Send sensor data to cloud MQTT broker for real-time monitoring and remote data access.

## Learning Objectives

- Understand MQTT protocol (publish/subscribe messaging)
- Connect to public and private MQTT brokers
- Format data as JSON for transmission
- Implement reconnection logic for network failures
- Balance cloud transmission with local SD logging
- Subscribe to command topics (optional)

## Hardware Required

- ESP32 board
- WiFi network with internet access

## Connections

**None** - uses built-in WiFi

## MQTT Broker Options

### **1. HiveMQ Public Broker (Testing)**

```
const char* MQTT_BROKER = "broker.hivemq.com";
const int MQTT_PORT = 1883;
// No authentication required
```

**✅ Advantages:**
- Free, no registration
- Good for testing

**❌ Disadvantages:**
- Public (anyone can read your data)
- No data persistence
- Rate limited

**Use for:** Development and testing only

---

### **2. Local Mosquitto Broker (Production)**

```
const char* MQTT_BROKER = "192.168.1.100";  // Your server IP
const int MQTT_PORT = 1883;
```

**Setup on Linux:**
```
sudo apt install mosquitto mosquitto-clients
sudo systemctl start mosquitto
```

**✅ Advantages:**
- Private (stays on local network)
- Fast (no internet delay)
- Full control

**Use for:** Lab/field deployments without internet

---

### **3. InfluxDB + Grafana (Recommended for Research)**

**Setup MQTT → InfluxDB pipeline:**

1. Install Telegraf (data collector):
   ```
   sudo apt install telegraf
   ```

2. Configure Telegraf (`/etc/telegraf/telegraf.conf`):
   ```
   [[inputs.mqtt_consumer]]
     servers = ["tcp://localhost:1883"]
     topics = ["turbine/+/data"]
     data_format = "json"
   
   [[outputs.influxdb_v2]]
     urls = ["http://localhost:8086"]
     token = "your-token"
     organization = "your-org"
     bucket = "turbine-data"
   ```

3. Visualize in Grafana dashboard

**✅ Advantages:**
- Time-series database (optimized for sensor data)
- Beautiful real-time dashboards
- Historical data analysis

---

### **4. Cloud MQTT Services**

| Service | Free Tier | Cost (Paid) | Notes |
|---------|-----------|-------------|-------|
| **AWS IoT Core** | 12 months | $0.08/million messages | Enterprise-grade |
| **Google Cloud IoT** | None | $0.0045/MB | Being deprecated (2023) |
| **Azure IoT Hub** | None | $10/month | Good Microsoft integration |
| **Adafruit IO** | 30 data points/min | $10/month | Beginner-friendly |

## WiFi & MQTT Configuration

**Update credentials:**

```
// WiFi
const char* WIFI_SSID = "YourNetwork";
const char* WIFI_PASSWORD = "YourPassword";

// MQTT
const char* MQTT_BROKER = "broker.hivemq.com";
const char* MQTT_CLIENT_ID = "ESP32_Turbine_001";  // MUST BE UNIQUE!
```

**⚠️ Client ID must be unique:** If two devices use same ID, they'll disconnect each other.

**Solution:** Use MAC address or serial number:
```
String clientID = "ESP32_Turbine_" + WiFi.macAddress();
mqttClient.connect(clientID.c_str());
```

## Expected Output

**Serial Monitor (115200 baud):**

```
=================================
MQTT Cloud Transmission - Example 07
=================================

Connecting to WiFi: YourNetwork
..........
WiFi connected!
IP Address: 192.168.1.15

Connecting to MQTT broker: broker.hivemq.com
MQTT connected!

Publishing sensor data to cloud...
Time(s) Wind(m/s)       Power(W)        RPM     Status
------------------------------------------------------------
5.0     5.45            124.8           244.5   Published
10.0    5.63            127.2           245.8   Published
15.0    5.52            123.9           244.2   Published
```

## Viewing Published Data

### **Method 1: MQTT Explorer (GUI)**

1. Download: http://mqtt-explorer.com/
2. Connect to `broker.hivemq.com:1883`
3. Subscribe to `turbine/#` (all topics)
4. See messages in real-time

---

### **Method 2: Mosquitto Client (Command Line)**

```
# Subscribe to all turbine topics
mosquitto_sub -h broker.hivemq.com -t "turbine/#" -v

# Output:
turbine/turbine001/data {"timestamp":12345,"wind_speed_ms":5.45,"power_w":124.8,"rpm":244.5,"temperature_c":28.5,"lambda":2.15}
turbine/turbine001/status online
```

---

### **Method 3: Python Script**

```
import paho.mqtt.client as mqtt

def on_message(client, userdata, msg):
    print(f"{msg.topic}: {msg.payload.decode()}")

client = mqtt.Client()
client.on_message = on_message
client.connect("broker.hivemq.com", 1883, 60)
client.subscribe("turbine/#")
client.loop_forever()
```

## Topic Hierarchy

**Best practice:** Use hierarchical topics for organization

```
turbine/
├── turbine001/
│   ├── data          ← Sensor measurements (JSON)
│   ├── status        ← Online/offline
│   ├── errors        ← Error messages
│   └── commands      ← Control commands (subscribe)
├── turbine002/
│   ├── data
│   ...
```

**Wildcards:**
- `turbine/+/data` - All turbines, data topic only
- `turbine/#` - All turbines, all topics

## JSON Payload Format

**Published data:**

```
{
  "timestamp": 12345,
  "wind_speed_ms": 5.45,
  "power_w": 124.8,
  "rpm": 244.5,
  "temperature_c": 28.5,
  "lambda": 2.15
}
```

**Advantages:**
- Human-readable
- Easy to parse (Python, JavaScript, InfluxDB)
- Extensible (add new fields without breaking subscribers)

**Size:** ~120 bytes (well within MQTT limits)

## Troubleshooting

### **Problem: "MQTT connection FAILED, rc=-2"**

**Error codes:**
- `-2` - Network connection failed
- `-4` - Server unavailable
- `2` - Client ID rejected
- `5` - Authentication failed

**Cause (rc=-2):** Cannot reach broker

**Solutions:**
1. Check broker address and port
2. Ping broker: `ping broker.hivemq.com`
3. Check firewall (port 1883 must be open)
4. Try different broker

---

### **Problem: Connected but no data received on subscriber**

**Cause 1:** Wrong topic subscription

```
# Subscribe to exact topic:
mosquitto_sub -h broker.hivemq.com -t "turbine/turbine001/data"
```

**Cause 2:** Retained messages filling queue

Clear retained message:
```
mosquitto_pub -h broker.hivemq.com -t "turbine/turbine001/data" -r -n
```

**Cause 3:** Public broker overloaded

Try different public broker or use local mosquitto.

---

### **Problem: "MQTT publish failed - message too large"**

**Cause:** Payload exceeds buffer size (default 256 bytes)

**Solution:** Increase buffer in PubSubClient.h:

```
// Line 26 in PubSubClient.h:
#define MQTT_MAX_PACKET_SIZE 512  // Increase from 256
```

Or reduce payload size (remove unnecessary fields).

---

### **Problem: Frequent disconnections**

**Cause:** Network instability or broker timeout

**Solutions:**

1. **Increase keep-alive interval:**
   ```
   mqttClient.setKeepAlive(60);  // Send ping every 60 seconds
   ```

2. **Add Last Will Testament (LWT):**
   ```
   mqttClient.connect(MQTT_CLIENT_ID, TOPIC_STATUS, 0, true, "offline");
   // Broker publishes "offline" if client disconnects unexpectedly
   ```

3. **Reconnection with exponential backoff:**
   ```
   int delay_ms = 1000 * (1 << reconnectAttempts);  // 1s, 2s, 4s, 8s...
   delay(min(delay_ms, 60000));  // Cap at 60 seconds
   ```

## MQTT QoS (Quality of Service)

```
mqttClient.publish(topic, payload, retained, qos);
```

| QoS | Delivery | Use Case |
|-----|----------|----------|
| **0** | At most once (fire and forget) | Non-critical data (fast) |
| **1** | At least once (may duplicate) | Important data (default) |
| **2** | Exactly once (slowest) | Critical commands |

**For sensor data:** QoS 0 is sufficient (next reading arrives soon anyway).

## Retained Messages

```
mqttClient.publish(TOPIC_STATUS, "online", true);  // Retained
//                                           ^^^^
```

**Retained message:** Broker stores it and sends to new subscribers immediately.

**Use for:** Status messages ("online"/"offline"), configuration

**Don't use for:** High-frequency sensor data (wastes broker memory)

## Local-First Architecture

**Best practice:** SD card primary, cloud secondary

```
void loop() {
  // 1. Always log to SD card (reliable)
  logToSDCard(data);
  
  // 2. Try to send to cloud (best-effort)
  if (mqttClient.connected()) {
    publishSensorData(data);
  }
  // Continue even if MQTT fails
}
```

**Advantages:**
- Data never lost (SD card backup)
- System works offline
- Cloud provides real-time monitoring

## Power Consumption

**WiFi + MQTT active:** ~200 mA

For battery systems, reduce transmission frequency:

```
const int PUBLISH_INTERVAL_MS = 60000;  // Every 1 minute (not 5 seconds)
```

Or use WiFi sleep between transmissions:
```
// Send data
publishSensorData();

// Sleep WiFi for 5 minutes
WiFi.disconnect();
WiFi.mode(WIFI_OFF);
delay(300000);  // 5 minutes

// Wake and reconnect
WiFi.mode(WIFI_STA);
connectWiFi();
connectMQTT();
```

## Security Considerations

### **Public Brokers:**

❌ **Never send:**
- Personal identifiable information
- Passwords or credentials
- Sensitive research data

✅ **OK to send:**
- Anonymized sensor readings
- Test data
- Non-critical monitoring

### **Private Brokers:**

Enable authentication:

```
mqttClient.connect(MQTT_CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD);
```

**Mosquitto auth setup:**
```
# Create password file
sudo mosquitto_passwd -c /etc/mosquitto/passwd turbine_user

# Edit /etc/mosquitto/mosquitto.conf:
allow_anonymous false
password_file /etc/mosquitto/passwd
```

### **TLS/SSL Encryption:**

For internet-facing brokers, use encrypted connection (port 8883):

```
WiFiClientSecure wifiClient;
wifiClient.setCACert(ca_cert);  // Load CA certificate
PubSubClient mqttClient(wifiClient);
mqttClient.setServer(MQTT_BROKER, 8883);
```

## Code Explanation

### **MQTT State Codes:**

```
mqttClient.state()
```

| Code | Meaning |
|------|---------|
| `-4` | MQTT_CONNECTION_TIMEOUT |
| `-3` | MQTT_CONNECTION_LOST |
| `-2` | MQTT_CONNECT_FAILED |
| `0` | MQTT_CONNECTED |
| `2` | MQTT_CONNECT_BAD_CLIENT_ID |
| `5` | MQTT_CONNECT_UNAUTHORIZED |

### **JSON Serialization:**

```
StaticJsonDocument<256> doc;  // 256 bytes on stack
doc["key"] = value;
serializeJson(doc, buffer);   // Convert to string
```

**Alternative (dynamic):**
```
DynamicJsonDocument doc(256);  // 256 bytes on heap (slower)
```

Use `StaticJsonDocument` for fixed-size messages (faster, predictable memory).

## Integration with Complete System

**Combine with Examples 5 & 6:**

```
// Log to SD with timestamp
File dataFile = SD.open("/data.csv", FILE_APPEND);
dataFile.print(getISO8601Timestamp());  // From Example 06
dataFile.print(",");
dataFile.println(windSpeed, 2);
dataFile.close();

// Send to cloud
publishSensorData();
```

## Next Steps

✅ **Example 07 Complete!**

Continue to:
- [Example 08: Complete Integration](../08_complete_integration/) - Combine all features

## Further Reading

- **MQTT Protocol:** https://mqtt.org/
- **PubSubClient Library:** https://github.com/knolleary/pubsubclient
- **HiveMQ Documentation:** https://www.hivemq.com/docs/
- **Mosquitto Broker:** https://mosquitto.org/