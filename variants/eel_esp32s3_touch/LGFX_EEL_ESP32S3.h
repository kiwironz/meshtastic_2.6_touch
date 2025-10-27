#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

// LGFX configuration for EEL ESP32-S3 Touch board
// 2.8" ST7789 TFT Display (240x320 physical, 320x240 landscape)
// XPT2046 resistive touch (pins defined but not enabled yet)
class LGFX_EEL_ESP32S3 : public lgfx::LGFX_Device
{
    lgfx::Panel_ST7789 _panel_instance;
    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;
    // Touch disabled for Stage 2a testing
    // lgfx::Touch_XPT2046 _touch_instance;

public:
    LGFX(void)
    {
        // ========================================
        // Configure SPI bus for display (SPI3)
        // ========================================
        {
            auto cfg = _bus_instance.config();
            
            // SPI configuration - using SPI3_HOST separate from radio (SPI2)
            cfg.spi_host = SPI3_HOST;              // Use SPI3 for display
            cfg.spi_mode = 0;                      // SPI mode 0 (CPOL=0, CPHA=0)
            cfg.freq_write = 40000000;             // 40MHz for writing
            cfg.freq_read = 16000000;              // 16MHz for reading
            cfg.spi_3wire = false;                 // 4-wire SPI
            cfg.use_lock = true;                   // Use SPI lock (for future touch)
            cfg.dma_channel = SPI_DMA_CH_AUTO;     // Auto DMA channel
            
            // SPI pin configuration - YOUR CUSTOM PINS
            cfg.pin_sclk = 2;                      // TFT_SCLK - GPIO2
            cfg.pin_mosi = 3;                      // TFT_MOSI - GPIO3
            cfg.pin_miso = 4;                      // TFT_MISO - GPIO4
            cfg.pin_dc = 1;                        // TFT_DC - GPIO1
            
            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        // ========================================
        // Configure ST7789 panel
        // ========================================
        {
            auto cfg = _panel_instance.config();
            
            // Pin assignments - YOUR CUSTOM PINS
            cfg.pin_cs = 16;                       // TFT_CS - GPIO16
            cfg.pin_rst = 12;                      // TFT_RST - GPIO12
            cfg.pin_busy = -1;                     // Not used

            // Use PSRAM for the frame buffer
            cfg.memory_alloc_type = lgfx::v1::heap_psram;
            
            // Display size configuration (ST7789 - 240x320 physical)
            cfg.memory_width = 240;                // Physical width
            cfg.memory_height = 320;               // Physical height
            cfg.panel_width = 240;                 // Panel width
            cfg.panel_height = 320;                // Panel height
            
            // Display settings
            cfg.offset_x = 0;                      // X offset
            cfg.offset_y = 0;                      // Y offset
            cfg.offset_rotation = 1;               // Rotation: 1 = landscape (320x240)
            cfg.dummy_read_pixel = 8;              // Dummy read bits for pixel
            cfg.dummy_read_bits = 1;               // Dummy read bits for data
            cfg.readable = true;                   // Display is readable
            cfg.invert = false;                    // ST7789 typically not inverted
            cfg.rgb_order = false;                 // Standard RGB order
            cfg.dlen_16bit = false;                // 8-bit data length
            cfg.bus_shared = true;                 // Shared with touch (future)

            _panel_instance.config(cfg);
        }

        // ========================================
        // Configure backlight PWM
        // ========================================
        {
            auto cfg = _light_instance.config();
            
            cfg.pin_bl = 15;                       // TFT_BL - GPIO15
            cfg.invert = false;                    // Normal polarity (HIGH = on)
            cfg.freq = 44100;                      // PWM frequency
            cfg.pwm_channel = 7;                   // PWM channel
            
            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        // ========================================
        // Configure XPT2046 resistive touch (DISABLED FOR NOW)
        // ========================================
        /*
        {
            auto cfg = _touch_instance.config();
            
            // Touch uses same SPI bus as display
            cfg.spi_host = SPI3_HOST;              // Same as display
            cfg.freq = 1000000;                    // 1MHz for touch (slower than display)
            cfg.pin_cs = 17;                       // TOUCH_CS - GPIO17
            cfg.pin_int = 14;                      // TOUCH_INT - GPIO14
            
            // Touch calibration values (default - adjust after testing)
            cfg.x_min = 0;                         // Min X (raw)
            cfg.x_max = 240;                       // Max X (raw)
            cfg.y_min = 0;                         // Min Y (raw)
            cfg.y_max = 320;                       // Max Y (raw)
            
            cfg.bus_shared = true;                 // Shared SPI with display
            cfg.offset_rotation = 1;               // Match display rotation
            
            _touch_instance.config(cfg);
            _panel_instance.setTouch(&_touch_instance);
        }
        */

        setPanel(&_panel_instance);
    }
};
