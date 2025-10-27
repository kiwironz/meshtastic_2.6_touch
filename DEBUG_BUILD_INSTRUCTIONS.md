# Debug Build Instructions - Boot Hang Investigation

## What We're Testing

I've added comprehensive debug logging throughout the NodeDB constructor to pinpoint exactly where the firmware hangs during boot. The debug statements use `Serial.println()` and `Serial.flush()` for immediate output.

## Build and Upload Instructions

1. **Build the firmware:**
   ```bash
   cd /home/user/meshtastic_2.6_touch
   pio run -e eel_esp32s3_touch
   ```

2. **Upload the firmware:**
   ```bash
   pio run -e eel_esp32s3_touch -t upload
   ```

3. **Monitor serial output:**
   ```bash
   pio device monitor -p /dev/ttyACM0 -b 115200
   ```

   Or use your preferred serial monitor tool.

## What to Look For

The debug output will show messages like:

```
DEBUG: main.cpp - Before new NodeDB
DEBUG: NodeDB constructor START
DEBUG: Before loadFromDisk()
DEBUG: After loadFromDisk()
DEBUG: After cleanupMeshDB()
DEBUG: Before get device unique id
DEBUG: ESP32-S3 - Before eFuse read
DEBUG: ESP32-S3 - After eFuse read
DEBUG: eFuse read SUCCESS
DEBUG: Before pickNewNodeNum()
DEBUG: After pickNewNodeNum()
DEBUG: Before PKI section
DEBUG: PKI section - checking license and region
DEBUG: PKI - generating/regenerating keys
DEBUG: After PKI section
DEBUG: Before getOrCreateMeshNode()
DEBUG: After getOrCreateMeshNode()
DEBUG: Before FSCom.exists check
DEBUG: Before preferences.begin()
DEBUG: After preferences.begin()
DEBUG: After preferences section
DEBUG: Before resetRadioConfig()
DEBUG: After resetRadioConfig()
DEBUG: Before saveToDisk()
DEBUG: NodeDB constructor COMPLETE
DEBUG: main.cpp - After new NodeDB
```

**The last debug message you see before the hang tells us exactly where the firmware is stopping.**

## Expected Results

Based on previous boot behavior, I expect we'll see:

1. PSRAM initialization succeeds ✓
2. LittleFS /prefs file errors (expected on first boot)
3. Debug messages from NodeDB constructor showing progress
4. **The hang will occur at one of these likely points:**
   - After "Before eFuse read" - if eFuse reading hangs
   - After "Before pickNewNodeNum()" - if node number generation hangs
   - After "PKI - generating/regenerating keys" - if PKI key generation hangs
   - After "Before preferences.begin()" - if NVS access hangs
   - After "Before saveToDisk()" - if filesystem write hangs

## Analysis After Testing

Once you have the serial output, **send me the complete boot log** showing:
1. All debug messages that appear
2. The last debug message before it hangs
3. Any error messages

This will tell us exactly which function is causing the hang.

## Files Modified for Debugging

- `src/mesh/NodeDB.cpp` - Added 20+ debug statements throughout the constructor
- `src/main.cpp` - Added debug statements around NodeDB instantiation

## Clean Up After Debugging

Once we identify the hang location and fix it, we can remove all the debug statements by reverting these changes or by searching for lines containing `Serial.println("DEBUG:` and removing them.

## Previous Findings

- ✓ PSRAM configuration fixed (qio_opi instead of qio_qspi)
- ✓ PSRAM initializes successfully
- ✓ LittleFS mounts successfully
- ✗ Firmware hangs after /prefs file errors (not caused by the errors themselves)
- ✗ Full chip erase didn't fix the hang (so not NVS corruption)

## Next Steps Based on Results

Depending on where the hang occurs, we'll:

1. **If hanging at eFuse read:** Check if eFuse block needs initialization or if there's a hardware issue
2. **If hanging at pickNewNodeNum():** Check random number generation or MAC address reading
3. **If hanging at PKI key generation:** May need to disable PKI temporarily or check crypto library
4. **If hanging at preferences.begin():** Despite full erase, may need different NVS initialization
5. **If hanging at saveToDisk():** Check LittleFS write permissions or filesystem corruption
6. **If hanging elsewhere:** We'll have narrowed it down significantly

The debug output will give us the exact answer.
