#pragma once
#include <stdint.h>
#include <stdio.h>
#include <avr/pgmspace.h>

class i2c_avr;

class oled {
  i2c_avr &i2c;
  uint16_t address;
  static uint8_t font_7x[] PROGMEM;
  
public:
  oled(i2c_avr &i2c);
  void init(uint16_t address=0x3C);
  
  void send_cmd(uint8_t command);
  void send_byte(uint8_t val);

  void clear_display();
  void setXY(uint16_t row, uint16_t col);
  void drawChar(char c);
  void sendStr(char *string);
  
  void print(char *fmt, ... );
  
protected:
};

