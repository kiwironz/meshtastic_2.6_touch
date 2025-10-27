#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

// Default SPI (SPI2_HOST) - Used by Radio
static const uint8_t SS    = 8;   // LORA_CS
static const uint8_t MOSI  = 7;   // LORA_MOSI
static const uint8_t MISO  = 6;   // LORA_MISO
static const uint8_t SCK   = 5;   // LORA_SCK

// Display SPI (SPI3_HOST) - Not default, explicitly configured in LGFX
// These are NOT the default SPI pins
// Display uses: SCLK=2, MOSI=3, MISO=4, CS=16

// I2C (not connected on hardware)
static const uint8_t SDA = 19;
static const uint8_t SCL = 20;

// GPIO pins
static const uint8_t LED_BUILTIN = 13;  // NeoPixel
#define BUILTIN_LED LED_BUILTIN

// User button
static const uint8_t KEY_BUILTIN = 18;

// Analog pins
static const uint8_t A0 = 1;
static const uint8_t A1 = 2;
static const uint8_t A2 = 3;
static const uint8_t A3 = 4;
static const uint8_t A4 = 5;
static const uint8_t A5 = 6;
static const uint8_t A6 = 7;
static const uint8_t A7 = 8;
static const uint8_t A8 = 9;
static const uint8_t A9 = 10;
static const uint8_t A10 = 11;
static const uint8_t A11 = 12;
static const uint8_t A12 = 13;
static const uint8_t A13 = 14;
static const uint8_t A14 = 15;
static const uint8_t A15 = 16;
static const uint8_t A16 = 17;
static const uint8_t A17 = 18;
static const uint8_t A18 = 19;
static const uint8_t A19 = 20;
static const uint8_t A20 = 21;  // Battery voltage sense

// Touch inputs (ADC pins)
static const uint8_t T1 = 1;
static const uint8_t T2 = 2;
static const uint8_t T3 = 3;
static const uint8_t T4 = 4;
static const uint8_t T5 = 5;
static const uint8_t T6 = 6;
static const uint8_t T7 = 7;
static const uint8_t T8 = 8;
static const uint8_t T9 = 9;
static const uint8_t T10 = 10;
static const uint8_t T11 = 11;
static const uint8_t T12 = 12;
static const uint8_t T13 = 13;
static const uint8_t T14 = 14;

// DAC pins
static const uint8_t DAC1 = 17;
static const uint8_t DAC2 = 18;

#endif /* Pins_Arduino_h */
