#ifndef Pins_Arduino_h
#define Pins_Arduino_h

#include <stdint.h>

#define USB_VID 0x303a
#define USB_PID 0x1001

static const uint8_t LED_BUILTIN = 35;
#define BUILTIN_LED LED_BUILTIN
#define LED_BUILTIN LED_BUILTIN

static const uint8_t TX = 43;
static const uint8_t RX = 44;

static const uint8_t SDA = 4;
static const uint8_t SCL = 3;

static const uint8_t SS   = 8;
static const uint8_t MOSI = 10;
static const uint8_t MISO = 11;
static const uint8_t SCK  = 9;

static const uint8_t Vext     = 36;
static const uint8_t LED      = 35;
static const uint8_t RST_OLED = 21;
static const uint8_t SCL_OLED = 18;
static const uint8_t SDA_OLED = 17;

// Radio SX1262 (stesso pinout della V3)
static const uint8_t RST_LoRa  = 12;
static const uint8_t BUSY_LoRa = 13;
static const uint8_t DIO0      = 14; // in realtà è DIO1 dell'SX1262, Heltec lo chiama così

#endif /* Pins_Arduino_h */
