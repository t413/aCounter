#pragma once
#include <stdint.h>
#include <stdio.h>
#include <avr/pgmspace.h>
#include "oled.h" //for Font superclass


class Font5x7 : public Font {
  static const uint8_t font_5x7[] PROGMEM;
public:
  uint8_t getCharWidth(char c);
  uint8_t getCharHeight(char c);
  uint16_t getCharVerticalBitmask(char c, uint8_t vpos);
};

