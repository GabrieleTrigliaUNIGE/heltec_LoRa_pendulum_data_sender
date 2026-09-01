#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// --- STRUTTURE DATI GLOBALI ---
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
#define MPU_ADDRESS 0x68 

#define BATTERY_PIN 1
#define BATTERY_CTRL_PIN 37

#define APP_TX_DUTYCYCLE 30000

// --- VARIABILE DI STATO RICARICA ---
// Questa variabile sopravvive al Deep Sleep dell'ESP32
extern RTC_DATA_ATTR bool isRecovering;

// --- PROTOTIPI DISPLAY ---
#ifdef USE_DISPLAY
#include <U8x8lib.h>
extern U8X8_SSD1306_128X64_NONAME_SW_I2C myDisplay; 

void setupDisplay();
void printDisplayMessage(const char* riga1, const char* riga2 = "");
void printDisplayMessage(const char* riga1, const char* riga2, const char* riga3);
void updateDisplayData(int packetNum);
void updateDisplayData(int packetNum, float volt);
void showChargingScreen(float volt, int percentage);
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

// --- PROTOTIPI BMS (Battery Management System) ---
float readBatteryVoltage();
void checkBatterySafety();

#endif // CONFIG_H