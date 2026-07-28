#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// =========================================================================
// STRUTTURE DATI GLOBALI
// =========================================================================
struct MPUData {
    float x;
    float y;
    float z;
};

// =========================================================================
// 1. CONFIGURAZIONE DEI FLAG
// =========================================================================
#define USE_DISPLAY
#define USE_IMU

// =========================================================================
// 2. CONFIGURAZIONE DEI PIN HARDWARE (Heltec V4)
// =========================================================================
#define OLED_CLOCK  18
#define OLED_DATA   17
#define OLED_RESET  21
#define VEXT        36 

#define MPU_SDA     41
#define MPU_SCL     42
#define MPU_ADDRESS 0x68

// =========================================================================
// 3. PROTOTIPI DEL DISPLAY OLED
// =========================================================================
#ifdef USE_DISPLAY
#include <U8x8lib.h>
extern U8X8_SSD1306_128X64_NONAME_SW_I2C display; // Istanza dichiarata qui

void setupDisplay();
void printDisplayMessage(const char* riga1, const char* riga2 = "");
void updateDisplayData(const MPUData& data);
#endif

// =========================================================================
// 4. PROTOTIPI DEL SENSORE IMU (MPU6050)
// =========================================================================
#ifdef USE_IMU
#include <Wire.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>

extern Adafruit_MPU6050 mpu; // Istanza dichiarata qui

bool setupIMU();
MPUData readIMU();
void printIMUData(const MPUData& data);
#endif

#endif // CONFIG_H