#pragma once

// #ifndef VARIANT_HAS_SCREEN
// #define VARIANT_HAS_SCREEN
// #endif

// #define PRIVATE_HW
// #define USE_LVGL
// #define USE_LGFX
// #define USE_SCREEN 0
// #define HAS_SCREEN 0
// #define HAS_TFT 1
// #define GFX_DRIVER_INC "graphics/LGFX/LGFX_GENERIC.h"
// #define MESHTASTIC_EXCLUDE_LEGACY_TFT 1

// Override tftSetup() to avoid "undefined reference" error
// #if defined(MESHTASTIC_EXCLUDE_LEGACY_TFT) && (MESHTASTIC_EXCLUDE_LEGACY_TFT == 1)
// inline void tftSetup() {}
// #endif


// Board identification
#define HW_VENDOR meshtastic_HardwareModel_PRIVATE_HW

// LoRa radio pins (SPI2_HOST - default)
#define LORA_SCK 5
#define LORA_MISO 6
#define LORA_MOSI 7
#define LORA_CS 8
#define LORA_DIO1 11
#define LORA_RESET 9
#define LORA_DIO2 10

// SX126X aliases for radio driver
#define SX126X_CS LORA_CS
#define SX126X_DIO1 LORA_DIO1
#define SX126X_RESET LORA_RESET
#define SX126X_BUSY LORA_DIO2
#define SX126X_RXEN -1
#define SX126X_TXEN -1

// TFT Display pins (SPI3_HOST)
#define TFT_CS 16
#define TFT_DC 1
#define TFT_RST 12
#define TFT_BL 15

// Touch pins (shared SPI3 with display)
#define TOUCH_CS 17
#define TOUCH_INT 14

// User interface
#define BUTTON_PIN 18

// NeoPixel LED
#define NEOPIXEL_DATA 13

// I2C (not physically connected) - indicate no I2C available from the variant
#define HAS_I2C 0
// Indicate there is no Arduino Wire/I2C support on this variant so code paths that
// call Wire.begin() without explicit pins are not taken.
#define HAS_WIRE 0
// Set invalid pins so runtime code that checks for valid pins can skip I2C init
#define I2C_SDA -1
#define I2C_SCL -1
// #undef SDA_PIN
// #undef SCL_PIN
// #define SDA_PIN -1
// #define SCL_PIN -1

// If pins are set to a negative value, undefine the macros so code that uses
// #if defined(I2C_SDA) checks won't attempt to initialize I2C. This keeps the
// variant-only approach and avoids changing tree files.
#if defined(I2C_SDA) && (I2C_SDA < 0)
#undef I2C_SDA
#endif
#if defined(I2C_SCL) && (I2C_SCL < 0)
#undef I2C_SCL
#endif
#if defined(I2C_SDA1) && (I2C_SDA1 < 0)
#undef I2C_SDA1
#endif
#if defined(I2C_SCL1) && (I2C_SCL1 < 0)
#undef I2C_SCL1
#endif


// NO BATTERY MONITORING
// GPIO21 does not have ADC capability on ESP32-S3
// ADC1 pins (GPIO1-10) are all assigned to other functions
// Battery monitoring will be added in next hardware revision