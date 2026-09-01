#include "config.h"

#ifdef USE_IMU

Adafruit_MPU6050 mpu;

bool setupIMU() {
    // recoverI2CBus();
    Wire.begin(MPU_SDA, MPU_SCL, 100000);
    delay(100); 

    // Usiamo ancora la libreria solo per la configurazione iniziale comoda (range e filtri)
    if (!mpu.begin(MPU_ADDRESS, &Wire, 0)) {
        return false;
    }
    
    mpu.setAccelerometerRange(MPU6050_RANGE_2_G);
    mpu.setFilterBandwidth(MPU6050_BAND_5_HZ);
    
    return true;
}

MPUData readIMU() {
    MPUData currentData = {0, 0, 0};

    // 1. Diciamo al sensore che vogliamo leggere partendo dal registro 0x3B (ACCEL_XOUT_H)
    Wire.beginTransmission(MPU_ADDRESS);
    Wire.write(0x3B);
    Wire.endTransmission(false); 

    // 2. Richiediamo 6 byte consecutivi. 
    // FIX: Forziamo il tipo (uint8_t) per eliminare l'ambiguità del compilatore
    Wire.requestFrom((uint8_t)MPU_ADDRESS, (uint8_t)6, (uint8_t)true);

    if (Wire.available() == 6) {
        // 3. Bit Shifting: assembliamo i byte HIGH e LOW in interi a 16 bit
        currentData.x = (Wire.read() << 8) | Wire.read();
        currentData.y = (Wire.read() << 8) | Wire.read();
        currentData.z = (Wire.read() << 8) | Wire.read();
    }

    return currentData;
}

void printIMUData(const MPUData& data) {
    // Ora stampiamo i valori ADC grezzi e puri (-32768 a +32767)
    Serial.print("Raw X: "); Serial.print(data.x);
    Serial.print(" | Y: "); Serial.print(data.y);
    Serial.print(" | Z: "); Serial.println(data.z);
}

// ROUTINE DI EMERGENZA: Sblocca il bus I2C se il sensore è "Zombie"
// void recoverI2CBus() {
//   pinMode(I2C_SDA_PIN, INPUT_PULLUP);
//   pinMode(I2C_SCL_PIN, INPUT_PULLUP);
//   delay(10);
  
//   // Se SDA è bloccato giù, il sensore è in Latch-Up
//   if (digitalRead(I2C_SDA_PIN) == LOW) {
//     Serial.println("[ALLARME] Bus I2C bloccato! Eseguo sblocco hardware...");
//     pinMode(I2C_SCL_PIN, OUTPUT);
    
//     // Inviamo 16 colpi di clock per forzare l'MPU a liberare il bus
//     for (byte i = 0; i < 16; i++) {
//       digitalWrite(I2C_SCL_PIN, LOW);
//       delayMicroseconds(20);
//       digitalWrite(I2C_SCL_PIN, HIGH);
//       delayMicroseconds(20);
//     }
//     Serial.println("[ALLARME] Sblocco completato. Sensore resettato.");
//   }
// }

#endif // ENABLE_IMU