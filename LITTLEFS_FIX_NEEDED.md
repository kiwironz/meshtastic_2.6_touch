# LittleFS Fix Required

## Current Status

✅ **PSRAM fix successful!** - Firmware now boots and runs  
❌ **LittleFS mount issue** - Old filesystem image still on device

## Boot Log Analysis

```
[   245][I][esp32-hal-psram.c:96] psramInit(): PSRAM enabled            ← SUCCESS!
[  5677][E][vfs_api.cpp:105] open(): /littlefs/prefs/db.proto does not exist, no permits for creation  ← PROBLEM
```

## Root Cause

The device still has the **OLD 9.9 MB LittleFS image** from previous builds (when using `eel_16MB_ota_spiffs_explicit.csv`).

Current partition table `default_16MB.csv` only allocates **3.375 MB** for spiffs.

When `LittleFS.begin(true)` runs:
- Finds existing 9.9 MB filesystem data
- Size mismatch with 3.375 MB partition
- Mount fails or mounts read-only
- Cannot create files → "no permits for creation"

## Solution

### Recommended: Build & Upload Fresh LittleFS

```bash
cd /home/user/meshtastic_2.6_touch
pio run -e eel_esp32s3_touch -t buildfs      # Build new ~8KB image
pio run -e eel_esp32s3_touch -t uploadfs     # Upload to device
pio run -e eel_esp32s3_touch -t upload       # Reflash firmware
pio device monitor -e eel_esp32s3_touch      # Monitor
```

### Alternative: Erase Partition First

```bash
esptool.py --port /dev/ttyACM0 erase_region 0xC90000 0x360000
pio run -e eel_esp32s3_touch -t upload
```

This erases the spiffs partition, then LittleFS.begin(true) will auto-format on first boot.

## Expected Result

After flashing fresh LittleFS:
- LittleFS mounts successfully  
- Files can be created
- Meshtastic starts normally
- Full functionality
