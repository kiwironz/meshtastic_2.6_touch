#pragma once

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

// LGFX_GENERIC - Template-based LGFX configuration using platformio.ini defines
// This is what device-ui includes to get the display driver

class LGFX : public lgfx::LGFX_Device
{
    // Panel type from LGFX_PANEL define
#if defined(LGFX_PANEL)
    #define PANEL_CLASS_HELPER2(x) lgfx::Panel_##x
    #define PANEL_CLASS_HELPER(x) PANEL_CLASS_HELPER2(x)
    PANEL_CLASS_HELPER(LGFX_PANEL) _panel_instance;
#else
    lgfx::Panel_ST7789 _panel_instance;
#endif

    lgfx::Bus_SPI _bus_instance;
    lgfx::Light_PWM _light_instance;

public:
    LGFX(void)
    {
        // Configure SPI bus
        {
            auto cfg = _bus_instance.config();

#ifdef LGFX_CFG_HOST
            cfg.spi_host = LGFX_CFG_HOST;
#else
            cfg.spi_host = SPI3_HOST;
#endif

#ifdef LGFX_PIN_SCK
            cfg.pin_sclk = LGFX_PIN_SCK;
#endif
#ifdef LGFX_PIN_MOSI
            cfg.pin_mosi = LGFX_PIN_MOSI;
#endif
#ifdef LGFX_PIN_MISO
            cfg.pin_miso = LGFX_PIN_MISO;
#endif
#ifdef LGFX_PIN_DC
            cfg.pin_dc = LGFX_PIN_DC;
#endif

            cfg.spi_mode = 0;
            cfg.use_lock = true;
            cfg.dma_channel = SPI_DMA_CH_AUTO;

#ifdef SPI_FREQUENCY
            cfg.freq_write = SPI_FREQUENCY;
#else
            cfg.freq_write = 40000000;
#endif
            cfg.freq_read = 16000000;

            _bus_instance.config(cfg);
            _panel_instance.setBus(&_bus_instance);
        }

        // Configure panel
        {
            auto cfg = _panel_instance.config();

#ifdef LGFX_PIN_CS
            cfg.pin_cs = LGFX_PIN_CS;
#endif
#ifdef LGFX_PIN_RST
            cfg.pin_rst = LGFX_PIN_RST;
#else
            cfg.pin_rst = -1;
#endif

            cfg.pin_busy = -1;

#ifdef LGFX_SCREEN_WIDTH
            cfg.memory_width = LGFX_SCREEN_WIDTH;
            cfg.panel_width = LGFX_SCREEN_WIDTH;
#else
            cfg.memory_width = 240;
            cfg.panel_width = 240;
#endif

#ifdef LGFX_SCREEN_HEIGHT
            cfg.memory_height = LGFX_SCREEN_HEIGHT;
            cfg.panel_height = LGFX_SCREEN_HEIGHT;
#else
            cfg.memory_height = 320;
            cfg.panel_height = 320;
#endif

            cfg.offset_x = 0;
            cfg.offset_y = 0;

#ifdef LGFX_ROTATION
            cfg.offset_rotation = LGFX_ROTATION;
#else
            cfg.offset_rotation = 0;
#endif

            cfg.dummy_read_pixel = 8;
            cfg.dummy_read_bits = 1;
            cfg.readable = true;

#ifdef LGFX_INVERT_COLOR
            cfg.invert = LGFX_INVERT_COLOR;
#else
            cfg.invert = false;
#endif

            cfg.rgb_order = false;
            cfg.dlen_16bit = false;
            cfg.bus_shared = true;

            _panel_instance.config(cfg);
        }

        // Configure backlight
        {
            auto cfg = _light_instance.config();

#ifdef LGFX_PIN_BL
            cfg.pin_bl = LGFX_PIN_BL;
#else
            cfg.pin_bl = -1;
#endif

#ifdef LGFX_INVERT_LIGHT
            cfg.invert = LGFX_INVERT_LIGHT;
#else
            cfg.invert = false;
#endif

#ifdef LGFX_PWM_FREQ
            cfg.freq = LGFX_PWM_FREQ;
#else
            cfg.freq = 44100;
#endif

#ifdef LGFX_PWM_CHANNEL
            cfg.pwm_channel = LGFX_PWM_CHANNEL;
#else
            cfg.pwm_channel = 7;
#endif

            _light_instance.config(cfg);
            _panel_instance.setLight(&_light_instance);
        }

        setPanel(&_panel_instance);
    }
};

// Global display instance for device-ui to use
static LGFX display;
