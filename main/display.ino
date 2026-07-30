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
    if (riga2[0] != '\0') {
        myDisplay.drawString(0, 2, riga2);
    }
}

void updateDisplayData(const MPUData& data) {
    myDisplay.setCursor(0, 0); myDisplay.print("Acc (m/s^2):");
    
    // Convertiamo il raw 16-bit in m/s^2 usando il fattore di scala di 8G (4096 LSB/g) e la gravità (9.81)
    myDisplay.setCursor(0, 2); myDisplay.print("X: "); myDisplay.print((data.x / 4096.0) * 9.81, 2); myDisplay.print("  ");
    myDisplay.setCursor(0, 4); myDisplay.print("Y: "); myDisplay.print((data.y / 4096.0) * 9.81, 2); myDisplay.print("  ");
    myDisplay.setCursor(0, 6); myDisplay.print("Z: "); myDisplay.print((data.z / 4096.0) * 9.81, 2); myDisplay.print("  ");
}

#endif // ENABLE_DISPLAY