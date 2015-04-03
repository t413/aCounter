#include "oled.h"

#include <stdlib.h>
#include <stdarg.h>
#include <interrupt_timer.h>
#include <i2c_avr.h>

oled::oled(i2c_avr &i2c, Font* font, uint16_t address) : 
    i2c_(i2c), font_(font), address_(address) { }

void oled::init(){
    send_cmd(0xae);    //display off
    send_cmd(0x2e);    //deactivate scrolling
    send_cmd(0xa4);    //SET All pixels OFF
    // send_cmd(0xa5);  //SET ALL pixels ON
    delay(50);
    send_cmd(0x20);    //Set Memory Addressing Mode
    send_cmd(0x02);    //Set Memory Addressing Mode to Page addressing mode(RESET)
    // send_cmd(0xa0); //colum address 0 mapped to SEG0 (POR)*** wires at bottom
    send_cmd(0xa1);    //colum address 127 mapped to SEG0 (POR) ** wires at top of board
    // send_cmd(0xC0); // Scan from Right to Left (POR)         *** wires at bottom
    send_cmd(0xC8);    // Scan from Left to Right               ** wires at top
    send_cmd(0xa6);    // Set WHITE chars on BLACK backround
    // send_cmd(0xa7); // Set BLACK chars on WHITE backround
    send_cmd(0x81);    // 81 Setup CONTRAST CONTROL, following byte is the contrast Value
    send_cmd(0xff);    // af contrast value between 1 ( == dull) to 256 ( == bright)
    delay(20);
    send_cmd(0xaf);    //display on
    delay(20);
}

void oled::setFont(Font* f) { font_ = f; }

void oled::send_cmd(uint8_t command) {
  i2c_.setSpeed(400000L);
  i2c_.writeReg(address_, 0x80, (uint8_t)command);
}

void oled::send_byte(uint8_t val) {
  i2c_.setSpeed(400000L);
  i2c_.writeReg(address_, 0x40, (uint8_t)val);
}

void oled::clearDisplay() {
  unsigned char i,k;
  for(k=0;k<8;k++) {	
    setXY(k,0);    
    for(i=0;i<128;i++) {    //clear all COL
      send_byte(0);          //clear all COL
      //delay(10);
    }
  }
}

void oled::setXY(uint16_t row, uint16_t col) {
  send_cmd(0xb0+row);                //set page address
  send_cmd(0x00+(8*col&0x0f));       //set low col address
  send_cmd(0x10+((8*col>>4)&0x0f));  //set high col address
}

void oled::drawChar(char c) {
//  uint16_t char_start = (c - 32) * 7;
  uint8_t cwidth = font_->getCharWidth(c);
//  uint8_t cheight = font->getCharHeight(c);
  for (uint8_t i=0; i<cwidth; i++) {
    uint16_t b = font_->getCharVerticalBitmask(c, i);
    //Todo support next vertical column for 2xHeight chars
    send_byte((uint8_t) b);
  }
}

void oled::sendStr(char *string) {
  while (*string) {
    drawChar(*string);
    *string += 1;
  }
}

void oled::print(char const* fmt, ... ) {
  char buf[128]; // resulting string limited to 128 chars
  va_list args;
  va_start (args, fmt );
  vsnprintf(buf, 128, fmt, args);
  va_end (args);
  sendStr(buf);
}

