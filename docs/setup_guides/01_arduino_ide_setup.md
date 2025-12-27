# Arduino IDE Setup Guide

Complete instructions for installing Arduino IDE 2.x for ESP32 development.

**Estimated time:** 15 minutes  
**Skill level:** Beginner  
**Prerequisites:** Windows 10/11, macOS 10.14+, or Linux Ubuntu 20.04+

---

## Step 1: Download Arduino IDE

### **Windows**

1. Go to https://www.arduino.cc/en/software
2. Click **"Windows Win 10 and newer, 64 bits"** (MSI installer recommended)
3. Download will start automatically (~150 MB)

### **macOS**

1. Go to https://www.arduino.cc/en/software
2. Click **"macOS 10.14 'Mojave' or newer, 64 bits"**
3. Download DMG file (~200 MB)

### **Linux (Ubuntu/Debian)**

1. Go to https://www.arduino.cc/en/software
2. Click **"Linux AppImage 64 bits (X86-64)"**
3. Download AppImage file (~150 MB)

**Alternative (recommended for Linux):**

```
# Download directly via terminal
wget https://downloads.arduino.cc/arduino-ide/arduino-ide_2.3.2_Linux_64bit.AppImage

# Make executable
chmod +x arduino-ide_2.3.2_Linux_64bit.AppImage

# Run
./arduino-ide_2.3.2_Linux_64bit.AppImage
```

---

## Step 2: Install Arduino IDE

### **Windows**

1. Double-click downloaded MSI file
2. Click **"Next"** through installation wizard
3. Accept license agreement
4. Choose installation directory (default: `C:\Program Files\Arduino IDE`)
5. Click **"Install"**
6. Wait for installation (2-3 minutes)
7. Click **"Finish"**

**Windows Defender warning?** Click "More info" → "Run anyway"

### **macOS**

1. Double-click downloaded DMG file
2. Drag **Arduino IDE** icon to **Applications** folder
3. Eject DMG
4. Open **Applications** → Right-click **Arduino IDE** → **"Open"**
5. Click **"Open"** in security warning (first launch only)

**Gatekeeper blocking?** System Preferences → Security & Privacy → Click "Open Anyway"

### **Linux**

```
# Option 1: AppImage (no installation needed)
./arduino-ide_2.3.2_Linux_64bit.AppImage

# Option 2: Install system-wide
sudo mv arduino-ide_2.3.2_Linux_64bit.AppImage /opt/arduino-ide
sudo chmod +x /opt/arduino-ide
sudo ln -s /opt/arduino-ide /usr/local/bin/arduino-ide

# Now run from terminal:
arduino-ide
```

---

## Step 3: First Launch Configuration

1. **Launch Arduino IDE** (Windows: Start menu, macOS: Applications, Linux: terminal)

2. **Welcome screen** will appear:
   - Click **"Start"** to skip tutorial (or watch 2-minute intro video)

3. **Telemetry prompt:**
   - Choose **"Allow"** or **"Don't Allow"** (personal preference, no impact on functionality)

4. **Check installation:**
   - Top menu: **Tools** → **Board** → You should see "Arduino boards" list
   - If empty, installation incomplete (try reinstalling)

---

## Step 4: Configure Preferences (Recommended)

### **A. Enable Verbose Output**

Makes troubleshooting easier:

1. **File** → **Preferences** (Windows/Linux) or **Arduino IDE** → **Preferences** (macOS)
2. Check ☑ **"Show verbose output during: compilation"**
3. Check ☑ **"Show verbose output during: upload"**
4. Click **"OK"**

### **B. Increase Editor Font Size (Optional)**

If text is too small:

1. **File** → **Preferences**
2. **Interface scale:** Change from 100% to 125% or 150%
3. Restart Arduino IDE

### **C. Set Default Save Location**

Organize your sketches:

1. **File** → **Preferences**
2. **Sketchbook location:** Click folder icon
3. Choose location (e.g., `C:\Users\YourName\Documents\Arduino` or `~/Documents/Arduino`)
4. Click **"OK"**

---

## Step 5: Verify Installation

### **Test with Blink Example**

1. **File** → **Examples** → **01.Basics** → **Blink**
2. Code window opens showing the classic Blink sketch
3. Click **✓ (Verify)** button (top left, checkmark icon)
4. **Output window** should show:
   ```
   Compiling sketch...
   Sketch uses 1234 bytes (0%) of program storage space
   Global variables use 45 bytes (1%) of dynamic memory
   ```

**Success!** Arduino IDE is working correctly.

**Error message?**
- "Error compiling" → Try: **Sketch** → **Verify/Compile** again
- Still failing? Reinstall Arduino IDE

---

## Step 6: Update Board Manager Index (Preparation for ESP32)

Before installing ESP32 support (next guide), update indexes:

1. **Tools** → **Board** → **Boards Manager**
2. Wait for "Downloading index..." to complete (~30 seconds)
3. Close Boards Manager window

---

## Troubleshooting

### **Problem: Arduino IDE won't launch**

**Windows:**
```
# Check if Java is installed (Arduino IDE 2.x bundles Java, but check anyway)
java -version

# If missing, download from: https://www.java.com/download/
```

**macOS:**
```
# Check system logs
Console.app → Search "Arduino"
```

**Linux:**
```
# Check AppImage dependencies
ldd arduino-ide_2.3.2_Linux_64bit.AppImage

# Install missing libraries (Ubuntu/Debian):
sudo apt install libfuse2
```

---

### **Problem: "Permission denied" (Linux)**

```
# Add user to dialout group (required for USB access)
sudo usermod -a -G dialout $USER

# Log out and log back in for changes to take effect
```

---

### **Problem: Menus are blank or corrupted (Linux)**

```
# Run with compatibility mode
arduino-ide --disable-gpu
```

---

## Next Steps

✅ **Arduino IDE installed successfully!**

**Continue to:**
- [`02_esp32_driver_installation.md`](02_esp32_driver_installation.md) - Add ESP32 board support
- [`03_library_installation.md`](03_library_installation.md) - Install required libraries

---

## Version Information

**Guide tested with:**
- Arduino IDE 2.3.2 (December 2024 release)
- Windows 11, macOS 14 Sonoma, Ubuntu 22.04

**Older Arduino IDE 1.8.x?** This guide is for version 2.x. The legacy 1.8.x series still works but lacks modern features. Download link: https://www.arduino.cc/en/software (scroll to "Legacy IDE")

---

## Additional Resources

- **Official Arduino IDE documentation:** https://docs.arduino.cc/software/ide-v2
- **Arduino forum:** https://forum.arduino.cc/c/using-arduino/ide-2-x/
- **Video tutorial:** https://www.youtube.com/watch?v=... (Arduino official channel)

---

**Need help?** Open an issue: https://github.com/asithakal/wind-turbine-daq-guide/issues