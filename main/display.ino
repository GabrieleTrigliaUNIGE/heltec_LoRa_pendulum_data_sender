#include "config.h"

#ifdef USE_DISPLAY

U8X8_SSD1306_128X64_NONAME_SW_I2C onBoardDisplay(OLED_CLOCK, OLED_DATA, OLED_RESET);

void setupDisplay() {
    pinMode(VEXT, OUTPUT);
    digitalWrite(VEXT, LOW); 
    delay(50); 
    
    onBoardDisplay.begin();
    onBoardDisplay.setFont(u8x8_font_chroma48medium8_r);
}

void printDisplayMessage(const char* riga1, const char* riga2) {
    onBoardDisplay.clear();
    onBoardDisplay.drawString(0, 0, riga1);
    if (riga2[0] != '\0') {
        onBoardDisplay.drawString(0, 2, riga2);
    }
}

void updateDisplayData(const MPUData& data) {
    onBoardDisplay.setCursor(0, 0); onBoardDisplay.print("Acc (m/s^2):");
    
    onBoardDisplay.setCursor(0, 2); onBoardDisplay.print("X: "); onBoardDisplay.print(data.x, 2); onBoardDisplay.print("  ");
    onBoardDisplay.setCursor(0, 4); onBoardDisplay.print("Y: "); onBoardDisplay.print(data.y, 2); onBoardDisplay.print("  ");
    onBoardDisplay.setCursor(0, 6); onBoardDisplay.print("Z: "); onBoardDisplay.print(data.z, 2); onBoardDisplay.print("  ");
}

#endif // USE_DISPLAY