#include "config.h"

#ifdef USE_DISPLAY

U8X8_SSD1306_128X64_NONAME_SW_I2C display(OLED_CLOCK, OLED_DATA, OLED_RESET);

void setupDisplay() {
    pinMode(VEXT, OUTPUT);
    digitalWrite(VEXT, LOW); 
    delay(50); 
    
    display.begin();
    display.setFont(u8x8_font_chroma48medium8_r);
}

void printDisplayMessage(const char* riga1, const char* riga2) {
    display.clear();
    display.drawString(0, 0, riga1);
    if (riga2[0] != '\0') {
        display.drawString(0, 2, riga2);
    }
}

void updateDisplayData(const MPUData& data) {
    display.setCursor(0, 0); display.print("Acc (m/s^2):");
    
    display.setCursor(0, 2); display.print("X: "); display.print(data.x, 2); display.print("  ");
    display.setCursor(0, 4); display.print("Y: "); display.print(data.y, 2); display.print("  ");
    display.setCursor(0, 6); display.print("Z: "); display.print(data.z, 2); display.print("  ");
}

#endif // USE_DISPLAY