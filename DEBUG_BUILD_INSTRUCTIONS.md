# Debug Build Instructions - Boot Hang Investigation

## What We're Testing

I've added comprehensive debug logging throughout the NodeDB constructor to pinpoint exactly where the firmware hangs during boot. The debug statements use `LOG_INFO()` which outputs to the ESP32 logging system (same format as the vfs_api.cpp errors you're already seeing).

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

The debug output will show messages in the ESP32 logging format (same as the errors you're already seeing):

```
[   245][I][esp32-hal-psram.c:96] psramInit(): PSRAM enabled
[  5677][E][vfs_api.cpp:105] open(): /littlefs/prefs/db.proto does not exist, no permits for creation
[  5742][I][NodeDB.cpp:196] NodeDB(): DEBUG: NodeDB constructor START
[  5743][I][NodeDB.cpp:197] NodeDB(): Init NodeDB
[  5744][I][NodeDB.cpp:198] NodeDB(): DEBUG: Before loadFromDisk()
[  5750][I][NodeDB.cpp:200] NodeDB(): DEBUG: After loadFromDisk()
[  5751][I][NodeDB.cpp:202] NodeDB(): DEBUG: After cleanupMeshDB()
[  5752][I][NodeDB.cpp:210] NodeDB(): DEBUG: Before get device unique id
[  5753][I][NodeDB.cpp:213] NodeDB(): DEBUG: ESP32-S3 - Before eFuse read
[  5754][I][NodeDB.cpp:218] NodeDB(): DEBUG: ESP32-S3 - After eFuse read
[  5755][I][NodeDB.cpp:222] NodeDB(): DEBUG: eFuse read SUCCESS
[  5756][I][NodeDB.cpp:256] NodeDB(): DEBUG: Before pickNewNodeNum()
[  5800][I][NodeDB.cpp:258] NodeDB(): DEBUG: After pickNewNodeNum()
[  5801][I][NodeDB.cpp:274] NodeDB(): DEBUG: Before PKI section
... and so on
```

**The last debug message you see before the hang tells us exactly where the firmware is stopping.**

The timestamps (in milliseconds) will help us see if there's a long delay between steps.

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

Once we identify the hang location and fix it, we can remove all the debug statements by reverting these changes or by searching for lines containing `LOG_INFO("DEBUG:` and removing them.

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
