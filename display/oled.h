#pragma once
#include <stdint.h>
#include <stddef.h>

class i2c_avr;
class Font;

class oled {
    i2c_avr &i2c_;
    Font& font_;
    uint16_t address_;
    uint8_t row_, column_;

public:
    oled(i2c_avr &i2c, Font& font, uint16_t address=0x3C);
    void init();

    void setFont(Font& font);

    void send_cmd(uint8_t command);
    void send_byte(uint8_t val);

    void clearDisplay();
    void setRowCol(uint16_t row, uint16_t col);

    void drawChar(char c);
    void print(char *string);
    void print(char const* fmt, ... );
};

class Font {
public:
    virtual uint8_t getCharWidth(char c) = 0;
    virtual uint8_t getCharHeight(char c) = 0;
    virtual uint16_t getCharVerticalBitmask(char c, uint8_t vpos) = 0;
};
