#ifndef DISPLAY_H
#define DISPLAY_H

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RST 14

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

class Display {
    private:
        Adafruit_SSD1306 display;

    public:
        Display();

        void writeLine(int x, int y, int size, const char* message, int line);

        void clear();
};

#endif