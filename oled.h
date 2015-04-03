#pragma once
#include <stdint.h>
#include <stdio.h>
#include <avr/pgmspace.h>

class i2c_avr;
class Font;

class oled {
  i2c_avr &i2c;
  Font* font;
  uint16_t address;

public:
  oled(i2c_avr &i2c);
  void init(uint16_t address=0x3C);

  void setFont(Font* font);

  void send_cmd(uint8_t command);
  void send_byte(uint8_t val);

  void clear_display();
  void setXY(uint16_t row, uint16_t col);
  void drawChar(char c);
  void sendStr(char *string);

  void print(char *fmt, ... );

protected:
};

class Font {
public:
  virtual uint8_t getCharWidth(char c) = 0;
  virtual uint8_t getCharHeight(char c) = 0;
  virtual uint16_t getCharVerticalBitmask(char c, uint8_t vpos) = 0;
};
