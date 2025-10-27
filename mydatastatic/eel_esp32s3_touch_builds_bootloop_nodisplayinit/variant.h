#pragma once

#ifndef VARIANT_HAS_SCREEN
#define VARIANT_HAS_SCREEN
#endif

#define PRIVATE_HW
#define USE_LVGL
#define USE_LGFX
#define USE_SCREEN 0
#define HAS_SCREEN 0
#define HAS_TFT 1
#define GFX_DRIVER_INC "graphics/LGFX/LGFX_GENERIC.h"
#define MESHTASTIC_EXCLUDE_LEGACY_TFT 1

// Override tftSetup() to avoid "undefined reference" error
#if defined(MESHTASTIC_EXCLUDE_LEGACY_TFT) && (MESHTASTIC_EXCLUDE_LEGACY_TFT == 1)
inline void tftSetup() {}
#endif


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

// I2C (not physically connected, but defined for compatibility)
// #define HAS_I2C 0  
#define I2C_SDA 35
#define I2C_SCL 36
// #undef SDA_PIN
// #undef SCL_PIN
// #define SDA_PIN -1
// #define SCL_PIN -1


// NO BATTERY MONITORING
// GPIO21 does not have ADC capability on ESP32-S3
// ADC1 pins (GPIO1-10) are all assigned to other functions
// Battery monitoring will be added in next hardware revision