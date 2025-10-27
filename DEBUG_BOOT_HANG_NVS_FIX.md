# ESP32-S3 Boot Hang Fix - NVS Partition Corruption

## Issue Summary

The EEL ESP32-S3 Touch board firmware successfully boots and initializes PSRAM, but hangs during NodeDB initialization and never reaches Meshtastic startup.

## Boot Sequence Analysis

### What Works ✓
- ESP32 bootloader starts correctly
- PSRAM initializes successfully: `[245][I][esp32-hal-psram.c:96] psramInit(): PSRAM enabled`
- LittleFS mounts and reads most files correctly

### Where It Hangs ✗
Boot log shows:
```
[  5677][E][vfs_api.cpp:105] open(): /littlefs/prefs/db.proto does not exist, no permits for creation
[  5743][E][vfs_api.cpp:105] open(): /littlefs/prefs/config.proto does not exist, no permits for creation
[  5757][E][vfs_api.cpp:105] open(): /littlefs/prefs/module.proto does not exist, no permits for creation
[  5797][E][vfs_api.cpp:105] open(): /littlefs/prefs/uiconfig.proto does not exist, no permits for creation
← Firmware hangs here, no further output
```

## Root Cause Identified

### The /prefs Errors Are NOT The Problem

These errors are **expected on first boot** when configuration files don't exist yet. Other LittleFS files are being read successfully, so the filesystem is working correctly.

### The Actual Hang Location

**File**: `src/mesh/NodeDB.cpp`
**Location**: Lines 309-312 in NodeDB constructor

```cpp
#ifdef ARCH_ESP32
    Preferences preferences;
    preferences.begin("meshtastic", false);  // ← HANGS HERE
    myNodeInfo.reboot_count = preferences.getUInt("rebootCounter", 0);
    preferences.end();
#endif
```

### Why It Hangs

The ESP32 `Preferences` API accesses the **NVS (Non-Volatile Storage) partition**, which is **completely separate from LittleFS**:

- **NVS Partition**: offset 0x9000, size 20KB (0x5000) - stores system preferences
- **LittleFS Partition**: offset 0xC90000, size 3.375MB - stores files

The NVS partition is likely **corrupt or uninitialized**, causing `preferences.begin()` to hang indefinitely.

### Why NVS Became Corrupt

Previous builds may have used different partition tables (e.g., the old 9.9MB LittleFS custom partition table). When partition layouts change, the NVS partition can become corrupt if not properly erased.

## Solution

### Option 1: Erase NVS Partition Only (Recommended First)

This preserves the LittleFS filesystem and only reinitializes the NVS:

```bash
# Erase the NVS partition (offset 0x9000, size 20KB)
esptool.py --port /dev/ttyACM0 erase_region 0x9000 0x5000

# Rebuild and reflash firmware (this will auto-initialize NVS)
pio run -e eel_esp32s3_touch -t upload
```

### Option 2: Full Chip Erase (Guaranteed Clean Slate)

If NVS erase doesn't work, do a complete flash erase:

```bash
# Erase entire flash chip
esptool.py --port /dev/ttyACM0 erase_flash

# Rebuild and upload everything
pio run -e eel_esp32s3_touch -t upload
pio run -e eel_esp32s3_touch -t uploadfs
```

## Expected Result After Fix

After erasing and reinitializing NVS, the firmware should:
1. Boot successfully (PSRAM enabled) ✓
2. Mount LittleFS ✓
3. Show the /prefs file errors (normal on first boot) ✓
4. **Continue past preferences.begin()** ← This should now work
5. Display Meshtastic startup messages
6. Start the Meshtastic mesh networking stack

## Partition Table Reference

The `default_16MB.csv` partition table layout:

| Name     | Type | SubType | Offset   | Size      | Purpose                    |
|----------|------|---------|----------|-----------|----------------------------|
| nvs      | data | nvs     | 0x9000   | 20KB      | System preferences (NVS)   |
| otadata  | data | ota     | 0xe000   | 8KB       | OTA update metadata        |
| app0     | app  | ota_0   | 0x10000  | 6.4MB     | Firmware slot 0            |
| app1     | app  | ota_1   | 0x650000 | 6.4MB     | Firmware slot 1 (OTA)      |
| spiffs   | data | spiffs  | 0xc90000 | 3.375MB   | LittleFS filesystem        |
| coredump | data | coredump| 0xFF0000 | 64KB      | Crash dumps                |

## Previous Fix: PSRAM Configuration

The **primary fix** that got the firmware booting was changing the PSRAM memory type in `boards/eel_esp32s3_touch.json`:

```json
{
  "build": {
    "arduino": {
      "memory_type": "qio_opi"  // Changed from "qio_qspi"
    }
  }
}
```

This fix allowed PSRAM to initialize correctly. Without it, the firmware would crash immediately after bootloader handoff.

## Debugging Timeline

1. **Initial Issue**: Firmware wouldn't boot past ESP32 bootloader
2. **Fix 1 - PSRAM**: Changed memory_type to qio_opi → Firmware now boots and PSRAM works
3. **Current Issue**: Firmware hangs at NodeDB initialization when accessing NVS
4. **Fix 2 - NVS**: Erase and reinitialize NVS partition → Should complete boot sequence

## Files Modified

- `boards/eel_esp32s3_touch.json` - Fixed PSRAM memory_type to qio_opi
- `variants/eel_esp32s3_touch/platformio.ini` - Commented out NO_USB_SERIAL, fixed HAS_WIFI/HAS_BLUETOOTH flags
- `.gitignore` - Updated for backup directories

## Next Steps

1. Try Option 1 (erase NVS partition only)
2. If that doesn't work, try Option 2 (full chip erase)
3. Monitor serial output during boot to confirm Meshtastic starts
4. Once booting successfully, configure the device via serial or BLE
