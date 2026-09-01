#include "config.h"

// Inizializza la bandierina di recupero a false al primo vero avvio a freddo
RTC_DATA_ATTR bool isRecovering = false; 

float readBatteryVoltage() {
    pinMode(BATTERY_CTRL_PIN, OUTPUT);
    analogSetPinAttenuation(BATTERY_PIN, ADC_11db);
    
    // --- TENTATIVO 1: Logica LOW (Standard vecchie V3) ---
    digitalWrite(BATTERY_CTRL_PIN, LOW);
    delay(50);
    analogRead(BATTERY_PIN); delay(10); // Dummy read
    
    int raw_adc = analogRead(BATTERY_PIN);
    float adc_mV = analogReadMilliVolts(BATTERY_PIN);
    
    // --- TENTATIVO 2: Logica HIGH (Nuove V3) ---
    // Se raw_adc è minore di 100, significa che stiamo leggendo il vuoto (Mosfet spento)
    if (raw_adc < 100) {
        digitalWrite(BATTERY_CTRL_PIN, HIGH);
        delay(50);
        analogRead(BATTERY_PIN); delay(10); // Dummy read
        
        raw_adc = analogRead(BATTERY_PIN);
        adc_mV = analogReadMilliVolts(BATTERY_PIN);
        
        // Spegniamo il sensore riportandolo a LOW (visto che HIGH lo ha acceso)
        digitalWrite(BATTERY_CTRL_PIN, LOW); 
    } else {
        // Spegniamo il sensore riportandolo a HIGH (visto che LOW lo ha acceso)
        digitalWrite(BATTERY_CTRL_PIN, HIGH); 
    }
    
    // Calcolo finale col moltiplicatore
    float voltage = (adc_mV * 4.9) / 1000.0;
    
    // --- DEBUG ---
    Serial.printf("\n[BMS] Raw ADC (0-4095): %d | Millivolt Letti: %.0f mV -> Tensione stimata: %.2f V\n", raw_adc, adc_mV, voltage);
    
    return voltage;
}

void checkBatterySafety() {
    float volt = readBatteryVoltage();
    
    // Se la tensione scende a livelli critici per la chimica LiPo
    if (volt > 2.0 && volt < 3.20) { 
        Serial.printf("\n!!! BATTERIA CRITICA (%.2f V) !!!\n", volt);
        Serial.println("Protezione attiva: Entro in Ibernazione per 10 minuti.");
        
        #ifdef USE_DISPLAY
        myDisplay.clear();
        myDisplay.drawString(0, 0, "BATT. CRITICA!");
        myDisplay.drawString(0, 2, "Ibernazione...");
        myDisplay.drawString(0, 4, "Collega USB-C");
        #endif
        
        delay(4000); 
        
        // Alziamo la bandierina: il sistema si bloccherà finché non sarà carico al 100%
        isRecovering = true; 
        
        // Ibernazione controllata (sveglia tra 10 minuti)
        uint64_t sleep_time_us = 10ULL * 60ULL * 1000000ULL;
        esp_sleep_enable_timer_wakeup(sleep_time_us);
        esp_deep_sleep_start(); 
    }
}