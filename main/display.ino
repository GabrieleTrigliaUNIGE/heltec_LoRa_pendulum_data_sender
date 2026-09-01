#include "config.h"

#ifdef USE_DISPLAY

U8X8_SSD1306_128X64_NONAME_SW_I2C myDisplay(OLED_CLOCK, OLED_DATA, OLED_RESET);

void setupDisplay() {
    pinMode(VEXT, OUTPUT);
    digitalWrite(VEXT, LOW); 
    delay(50); 
    
    myDisplay.begin();
    myDisplay.setFont(u8x8_font_chroma48medium8_r);
}

void printDisplayMessage(const char* riga1, const char* riga2) {
    myDisplay.clear();
    myDisplay.drawString(0, 0, riga1);
    if (riga2[0] != '\0') myDisplay.drawString(0, 2, riga2);
}

void printDisplayMessage(const char* riga1, const char* riga2, const char* riga3) {
    myDisplay.clear();
    myDisplay.drawString(0, 0, riga1);
    if (riga2[0] != '\0') myDisplay.drawString(0, 2, riga2);
    if (riga3[0] != '\0') myDisplay.drawString(0, 4, riga3);
}

void updateDisplayData(int packetNum) {
    float volt = readBatteryVoltage();
    updateDisplayData(packetNum, volt); // Passa la palla alla Versione 2
}

void updateDisplayData(int packetNum, float volt) {
    myDisplay.clear();
    myDisplay.setCursor(0, 0);
    myDisplay.print("-- STATO NODO --");

    myDisplay.setCursor(0, 2);
    myDisplay.print("V_Read: ");
    myDisplay.print(volt, 2); 
    myDisplay.print(" V");

    myDisplay.setCursor(0, 4);
    if (volt > 3.95) {
        myDisplay.print("Bat: ALIM. USB");
    } else if (volt < 1.0) {
        myDisplay.print("Bat: NON RILEVATA");
    } else {
        myDisplay.print("Bat: ALIM. LIPO");
    }

    myDisplay.setCursor(0, 6);
    myDisplay.print("Pkt: #");
    myDisplay.print(packetNum);
}

void showChargingScreen(float volt, int percentage) {
    myDisplay.clear();
    myDisplay.setCursor(0, 0); 
    myDisplay.print("== IN CARICA ==");
    
    myDisplay.setCursor(0, 2); 
    myDisplay.print("Livello: "); 
    myDisplay.print(percentage); 
    myDisplay.print("%");
    
    myDisplay.setCursor(0, 4); 
    myDisplay.print("Tensione: "); 
    myDisplay.print(volt, 2); 
    myDisplay.print("V");
    
    myDisplay.setCursor(0, 6); 
    myDisplay.print("Attendere...");
}

#endif