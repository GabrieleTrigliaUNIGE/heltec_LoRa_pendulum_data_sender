#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- STRUTTURE DATI GLOBALI ---
// Ora usiamo interi a 16-bit (2 byte) per ottimizzare il payload radio
struct MPUData {
    int16_t x;
    int16_t y;
    int16_t z;
};

// --- CONFIGURAZIONE FLAG ---
#define USE_DISPLAY
#define USE_IMU

// --- CONFIGURAZIONE PIN HARDWARE ---
#define OLED_CLOCK  18
#define OLED_DATA   17
#define OLED_RESET  21
#define VEXT        36 

#define MPU_SDA     41
#define MPU_SCL     42
#define MPU_ADDRESS 0x68 // Fissato grazie ad AD0 a GND

#define APP_TX_DUTYCYCLE 30000

// --- PROTOTIPI DISPLAY ---
#ifdef USE_DISPLAY
#include <U8x8lib.h>
extern U8X8_SSD1306_128X64_NONAME_SW_I2C myDisplay; // Ricordati che avevamo cambiato in myDisplay

void setupDisplay();
void printDisplayMessage(const char* riga1, const char* riga2 = "");
void updateDisplayData(const MPUData& data);
#endif

// --- PROTOTIPI MPU6050 ---
#ifdef USE_IMU
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
extern Adafruit_MPU6050 mpu;

bool setupIMU();
MPUData readIMU();
void printIMUData(const MPUData& data);
#endif

#endif // CONFIG_H