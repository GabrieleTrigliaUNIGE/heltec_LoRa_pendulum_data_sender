#include "config.h"

#ifdef USE_IMU

// Creazione reale dell'oggetto
Adafruit_MPU6050 mpu;

bool setupIMU() {
    Wire.end(); 
    Wire.setPins(MPU_SDA, MPU_SCL); 
    Wire.begin();
    delay(100); 

    if (!mpu.begin(MPU_ADDRESS, &Wire, 0)) {
        return false;
    }
    
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
    
    return true;
}

MPUData readIMU() {
    sensors_event_t a, g, temp;
    mpu.getEvent(&a, &g, &temp);

    MPUData currentData;
    currentData.x = a.acceleration.x;
    currentData.y = a.acceleration.y;
    currentData.z = a.acceleration.z;

    return currentData;
}

void printIMUData(const MPUData& data) {
    Serial.print("Acc X: "); Serial.print(data.x);
    Serial.print(" | Y: "); Serial.print(data.y);
    Serial.print(" | Z: "); Serial.print(data.z);
    Serial.println(" m/s^2");
}

#endif // USE_IMU