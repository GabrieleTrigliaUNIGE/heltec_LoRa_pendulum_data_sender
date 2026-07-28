#include <Arduino.h>
#include "config.h"

void setup() {
    Serial.begin(115200);
    delay(3000); 
    
    Serial.println("\n--- Avvio Sistema ---");

    #ifdef USE_DISPLAY
    setupDisplay();
    printDisplayMessage("Avvio Sistema...");
    #endif

    #ifdef USE_IMU
    Serial.println("Cerco il sensore MPU6050...");
    if (!setupIMU()) {
        Serial.println("ERRORE: MPU6050 non trovato o bus bloccato!");
        #ifdef USE_DISPLAY
        printDisplayMessage("Errore I2C", "Check Cavi!");
        #endif
        
        while (1) { delay(10); } // Blocco di sicurezza
    }
    
    Serial.println("MPU6050 Trovato e Inizializzato!");
    #endif

    #ifdef USE_DISPLAY
    printDisplayMessage("Tutto OK", "Inizio lettura");
    delay(1000);
    display.clear(); 
    #endif
}

void loop() {
    MPUData sensorData = {0, 0, 0};

    #ifdef USE_IMU
    sensorData = readIMU();
    printIMUData(sensorData);
    #endif

    #ifdef USE_DISPLAY
    #ifdef USE_IMU
    updateDisplayData(sensorData);
    #endif
    #endif
    
    delay(200); 
}