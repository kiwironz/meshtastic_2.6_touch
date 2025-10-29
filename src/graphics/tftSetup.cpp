#if HAS_TFT

#include "SPILock.h"
#include "sleep.h"

#include "api/PacketAPI.h"
#include "comms/PacketClient.h"
#include "comms/PacketServer.h"
#include "graphics/DeviceScreen.h"
#include "graphics/driver/DisplayDriverConfig.h"
#include "lvgl.h"
#include "FSCommon.h"
#include "configuration.h"

#ifdef ARCH_PORTDUINO
#include "PortduinoGlue.h"
#include <thread>
#endif

DeviceScreen *deviceScreen = nullptr;

#ifdef ARCH_ESP32
// Get notified when the system is entering light sleep
CallbackObserver<DeviceScreen, void *> tftSleepObserver =
    CallbackObserver<DeviceScreen, void *>(deviceScreen, &DeviceScreen::prepareSleep);
CallbackObserver<DeviceScreen, esp_sleep_wakeup_cause_t> endSleepObserver =
    CallbackObserver<DeviceScreen, esp_sleep_wakeup_cause_t>(deviceScreen, &DeviceScreen::wakeUp);
#endif

void test_lvgl_draw()
{
    LOG_INFO("TEST: Drawing test label to screen");

    // Get the active screen
    lv_obj_t *scr = lv_scr_act();
    if (!scr) {
        LOG_ERROR("TEST: lv_scr_act() returned NULL!");
        return;
    }
    LOG_INFO("TEST: Active screen at %p", scr);

    // Set screen background to red for visibility
    lv_obj_set_style_bg_color(scr, lv_color_hex(0xFF0000), LV_PART_MAIN);
    lv_obj_set_style_bg_opa(scr, LV_OPA_COVER, LV_PART_MAIN);
    LOG_INFO("TEST: Screen background set to RED");

    // Create a label
    lv_obj_t *label = lv_label_create(scr);
    if (!label) {
        LOG_ERROR("TEST: lv_label_create() returned NULL!");
        return;
    }
    LOG_INFO("TEST: Label created at %p", label);

    // Set label text
    lv_label_set_text(label, "MESHTASTIC\nTEST SCREEN\nIf you see this\nLVGL WORKS!");
    LOG_INFO("TEST: Label text set");

    // Center the label
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);
    LOG_INFO("TEST: Label centered");

    // Set label style - use default font
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    LOG_INFO("TEST: Label style set (white text, default font)");

    LOG_INFO("TEST: Drawing complete, triggering screen refresh");
}

void tft_task_handler(void *param = nullptr)
{
    while (true) {
        spiLock->lock();
        deviceScreen->task_handler();
        spiLock->unlock();
        deviceScreen->sleep();
    }
}

void tftSetup(void)
{
    LOG_INFO("tftSetup() called - initializing TFT display");
#ifndef ARCH_PORTDUINO
    LOG_INFO("Creating DeviceScreen...");
    deviceScreen = &DeviceScreen::create();
    LOG_INFO("DeviceScreen created at %p", deviceScreen);
    LOG_INFO("Creating PacketAPI...");
    PacketAPI::create(PacketServer::init());
    LOG_INFO("Initializing DeviceScreen with PacketClient...");
    deviceScreen->init(new PacketClient);
    LOG_INFO("DeviceScreen initialized");

    // WORKAROUND: Delete corrupted uiconfig.proto if it exists
    LOG_INFO("Checking for corrupted uiconfig.proto file...");
    if (FSCom.exists("/prefs/uiconfig.proto")) {
        LOG_WARN("Found /prefs/uiconfig.proto, deleting to clear corruption...");
        FSCom.remove("/prefs/uiconfig.proto");
        LOG_INFO("Deleted /prefs/uiconfig.proto");
    }

    // Check if device-ui created any UI objects
    delay(1000);  // Give device-ui time to create UI
    lv_obj_t *scr = lv_scr_act();
    uint32_t child_count = lv_obj_get_child_cnt(scr);
    LOG_INFO("Active screen has %d children (UI objects)", child_count);

    if (child_count == 0) {
        LOG_WARN("No UI objects created by device-ui! Drawing test screen instead...");
        test_lvgl_draw();
    } else {
        LOG_INFO("Device-ui created UI objects successfully");
    }
#else
    if (settingsMap[displayPanel] != no_screen) {
        DisplayDriverConfig displayConfig;
        static char *panels[] = {"NOSCREEN", "X11",     "FB",      "ST7789",  "ST7735",  "ST7735S",
                                 "ST7796",   "ILI9341", "ILI9342", "ILI9486", "ILI9488", "HX8357D"};
        static char *touch[] = {"NOTOUCH", "XPT2046", "STMPE610", "GT911", "FT5x06"};
#if defined(USE_X11)
        if (settingsMap[displayPanel] == x11) {
            if (settingsMap[displayWidth] && settingsMap[displayHeight])
                displayConfig = DisplayDriverConfig(DisplayDriverConfig::device_t::X11, (uint16_t)settingsMap[displayWidth],
                                                    (uint16_t)settingsMap[displayHeight]);
            else
                displayConfig.device(DisplayDriverConfig::device_t::X11);
        } else
#elif defined(USE_FRAMEBUFFER)
        if (settingsMap[displayPanel] == fb) {
            if (settingsMap[displayWidth] && settingsMap[displayHeight])
                displayConfig = DisplayDriverConfig(DisplayDriverConfig::device_t::FB, (uint16_t)settingsMap[displayWidth],
                                                    (uint16_t)settingsMap[displayHeight]);
            else
                displayConfig.device(DisplayDriverConfig::device_t::FB);
        } else
#endif
        {
            displayConfig.device(DisplayDriverConfig::device_t::CUSTOM_TFT)
                .panel(DisplayDriverConfig::panel_config_t{.type = panels[settingsMap[displayPanel]],
                                                           .panel_width = (uint16_t)settingsMap[displayWidth],
                                                           .panel_height = (uint16_t)settingsMap[displayHeight],
                                                           .rotation = (bool)settingsMap[displayRotate],
                                                           .pin_cs = (int16_t)settingsMap[displayCS],
                                                           .pin_rst = (int16_t)settingsMap[displayReset],
                                                           .offset_x = (uint16_t)settingsMap[displayOffsetX],
                                                           .offset_y = (uint16_t)settingsMap[displayOffsetY],
                                                           .offset_rotation = (uint8_t)settingsMap[displayOffsetRotate],
                                                           .invert = settingsMap[displayInvert] ? true : false,
                                                           .rgb_order = (bool)settingsMap[displayRGBOrder],
                                                           .dlen_16bit = settingsMap[displayPanel] == ili9486 ||
                                                                         settingsMap[displayPanel] == ili9488})
                .bus(DisplayDriverConfig::bus_config_t{.freq_write = (uint32_t)settingsMap[displayBusFrequency],
                                                       .freq_read = 16000000,
                                                       .spi{.pin_dc = (int8_t)settingsMap[displayDC],
                                                            .use_lock = true,
                                                            .spi_host = (uint16_t)settingsMap[displayspidev]}})
                .input(DisplayDriverConfig::input_config_t{.keyboardDevice = settingsStrings[keyboardDevice],
                                                           .pointerDevice = settingsStrings[pointerDevice]})
                .light(DisplayDriverConfig::light_config_t{.pin_bl = (int16_t)settingsMap[displayBacklight],
                                                           .pwm_channel = (int8_t)settingsMap[displayBacklightPWMChannel],
                                                           .invert = (bool)settingsMap[displayBacklightInvert]});
            if (settingsMap[touchscreenI2CAddr] == -1) {
                displayConfig.touch(
                    DisplayDriverConfig::touch_config_t{.type = touch[settingsMap[touchscreenModule]],
                                                        .freq = (uint32_t)settingsMap[touchscreenBusFrequency],
                                                        .pin_int = (int16_t)settingsMap[touchscreenIRQ],
                                                        .offset_rotation = (uint8_t)settingsMap[touchscreenRotate],
                                                        .spi{
                                                            .spi_host = (int8_t)settingsMap[touchscreenspidev],
                                                        },
                                                        .pin_cs = (int16_t)settingsMap[touchscreenCS]});
            } else {
                displayConfig.touch(DisplayDriverConfig::touch_config_t{
                    .type = touch[settingsMap[touchscreenModule]],
                    .freq = (uint32_t)settingsMap[touchscreenBusFrequency],
                    .x_min = 0,
                    .x_max =
                        (int16_t)((settingsMap[touchscreenRotate] & 1 ? settingsMap[displayWidth] : settingsMap[displayHeight]) -
                                  1),
                    .y_min = 0,
                    .y_max =
                        (int16_t)((settingsMap[touchscreenRotate] & 1 ? settingsMap[displayHeight] : settingsMap[displayWidth]) -
                                  1),
                    .pin_int = (int16_t)settingsMap[touchscreenIRQ],
                    .offset_rotation = (uint8_t)settingsMap[touchscreenRotate],
                    .i2c{.i2c_addr = (uint8_t)settingsMap[touchscreenI2CAddr]}});
            }
        }
        deviceScreen = &DeviceScreen::create(&displayConfig);
        PacketAPI::create(PacketServer::init());
        deviceScreen->init(new PacketClient);
    } else {
        LOG_INFO("Running without TFT display!");
    }
#endif

    if (deviceScreen) {
        LOG_INFO("deviceScreen is valid, setting up task handler");
#ifdef ARCH_ESP32
        tftSleepObserver.observe(&notifyLightSleep);
        endSleepObserver.observe(&notifyLightSleepEnd);
        LOG_INFO("Creating tft_task_handler on core 0");
        xTaskCreatePinnedToCore(tft_task_handler, "tft", 10240, NULL, 1, NULL, 0);
        LOG_INFO("tft_task_handler created successfully");
#elif defined(ARCH_PORTDUINO)
        std::thread *tft_task = new std::thread([] { tft_task_handler(); });
#endif
    } else {
        LOG_ERROR("deviceScreen is NULL - display will not work!");
    }
}

#endif