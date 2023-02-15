#include <display.h>

Display::Display(): display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST) {
    Wire.begin(OLED_SDA, OLED_SCL);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // Address 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
        for(;;); // Don't proceed, loop forever
    }
}

void Display::writeLine(int x, int y, int size, const char* message, int line) {
    display.setTextColor(WHITE);
    display.setCursor(x,y);
    while(line-- > 0) display.println();
    display.printf("%s\n", message);
    display.display();
}

void Display::clear() {
    display.clearDisplay();
    display.display();
}