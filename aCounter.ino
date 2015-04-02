#include "i2c_avr.h"
#include "oled.h"

i2c_avr i2c;
oled lcd(i2c);

void setup() {
  i2c.init();
  lcd.init();
}

  // lcd.clear_display();
  // lcd.send_cmd(0x20);            //Set Memory Addressing Mode
  // lcd.send_cmd(0x02);            //Set Memory Addressing Mode ab Page addressing mode(RESET)
  // lcd.send_cmd(0xa6);            //Set Normal Display (default)
boolean within(int16_t val, int16_t in, int16_t tollerance) {
  return abs(val - in) < tollerance;
}

struct Button {
  uint16_t trigger_value;
  String name;
  boolean value;
  uint32_t down;
  Button(uint16_t tv=0, String n="btn") : trigger_value(tv), name(n), value(0), down(0) { }
  boolean check(uint16_t val, uint32_t now_time) {
    boolean b = within(val, trigger_value, 5);
    if (b && !value) { down = millis(); }
    return ((value = b));
  }
};

void loop() {
  struct Button rb1(683,"rb1"), rb2(768,"rb2"), rb3(819,"rb3"); // red buttons!
  struct Button gb1(513,"gb1"), gb2(0,"gb2");             // green buttons!
  struct Button* b[] = { &rb1, &rb2, &rb3, &gb1, &gb2 };
  
  while (1) {
    uint32_t current = millis();
    uint16_t ana = analogRead(A3);
    for (uint8_t i=0; i<5; i++) { b[i]->check(ana, current); }
    
    for (uint8_t i=0; i<5; i++) { 
      if (b[i]->value) {
        lcd.setXY(0,5);
        lcd.print("on: %s.", b[i]->name);
      }
    }
    
    
    
  }
  
}

