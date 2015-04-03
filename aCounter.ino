#include "i2c_avr.h"
#include "oled.h"
#include "fonts.h"

i2c_avr i2c;
oled lcd(i2c);
Font5x7 smallfont;

void setup() {
  i2c.init();
  lcd.init();
  lcd.setFont(&smallfont);

  lcd.clear_display();
  lcd.send_cmd(0x20);            //Set Memory Addressing Mode
  lcd.send_cmd(0x02);            //Set Memory Addressing Mode ab Page addressing mode(RESET)
  lcd.send_cmd(0xa6);            //Set Normal Display (default)
}

boolean within(int16_t val, int16_t in, int16_t tollerance) {
  return abs(val - in) < tollerance;
}

struct Button {
  uint16_t trigger_value;
  String name;
  boolean value;
  uint32_t down;
  int16_t presses;

  Button(uint16_t tv=0, String n="btn") :
    trigger_value(tv), name(n),
    value(0), down(0), presses(0) { }

  boolean check(uint16_t val, uint32_t now_time) {
    boolean b = within(val, trigger_value, 5);
    if (b && !value) { down = now_time; }
    if (down && !b && value && ((now_time - down) > 10) && ((now_time - down) < 1000)) { presses++; }
    if (down && b && value && ((now_time - down) > 1000)) { presses--; down = 0; }
    return ((value = b));
  }
};

void loop() {
  struct Button rb1(683,"hits"), rb2(768,"spits"), rb3(819,"averts"); // red buttons!
  struct Button gb1(513,"tant"), gb2(0,"cry");             // green buttons!
  struct Button* buttons[] = { &rb1, &rb2, &rb3, &gb1, &gb2 };

  while (1) {
    uint32_t current = millis();
    uint16_t ana = analogRead(A3);
    for (uint8_t i=0; i<5; i++) { buttons[i]->check(ana, current); }


    lcd.setXY(1,0);
    lcd.print("%s: %d. ", rb1.name.c_str(), rb1.presses);
    lcd.setXY(0,4);
    lcd.print("%s: %d. ", rb2.name.c_str(), rb2.presses);
    lcd.setXY(2,5);
    lcd.print("%s: %d. ", rb3.name.c_str(), rb3.presses);
    lcd.setXY(7,0);
    lcd.print("%s: %d. ", gb1.name.c_str(), gb1.presses);
    lcd.setXY(7,8);
    lcd.print("%s: %d. ", gb2.name.c_str(), gb2.presses);

    lcd.setXY(6,0);
    lcd.print("at: %ld.", current >> 8);
  }

}

