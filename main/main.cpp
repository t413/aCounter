#include <i2c_avr.h>
#include <analog.h>
#include <oled.h>
#include <interrupt_timer.h>

#define abs(x) ((x > 0)? (x) : (-x))
bool within(int16_t val, int16_t in, int16_t tollerance) {
  return abs(val - in) < tollerance;
}

struct Button {
    uint16_t trigger_value;
    char const* name;
    bool value;
    uint32_t down;
    int16_t presses;

    Button(uint16_t tv=0, char const* n="btn") :
            trigger_value(tv), name(n),
            value(0), down(0), presses(0) { }

    bool check(uint16_t val, uint32_t now_time) {
        bool b = within(val, trigger_value, 5);
        if (b && !value) { down = now_time; }
        if (down && !b && value && ((now_time - down) > 10) && ((now_time - down) < 1000)) { presses++; }
        if (down && b && value && ((now_time - down) > 1000)) { presses--; down = 0; }
        return ((value = b));
    }
};

int main() {
    i2c_avr i2c;
    oled lcd(i2c);

    lcd.init();

    struct Button rb1(683,"hits"), rb2(768,"spits"), rb3(819,"averts"); // red buttons!
    struct Button gb1(513,"tant"), gb2(0,"cry");             // green buttons!
    struct Button* buttons[] = { &rb1, &rb2, &rb3, &gb1, &gb2 };

    lcd.clearDisplay();
    lcd.setXY(0,4);
    lcd.print(rb1.name);
    lcd.setXY(40,4);
    lcd.print(rb2.name);
    lcd.setXY(90,4);
    lcd.print(rb3.name);
    // lcd.setCursor(0,4);
    // lcd.print("%s: %d. ", rb2.name.c_str(), rb2.presses);
    // lcd.setCursor(2,5);
    // lcd.print("%s: %d. ", rb3.name.c_str(), rb3.presses);
    // lcd.setCursor(7,0);
    // lcd.print("%s: %d. ", gb1.name.c_str(), gb1.presses);
    // lcd.setCursor(7,8);
    // lcd.print("%s: %d. ", gb2.name.c_str(), gb2.presses);

    // lcd.setCursor(6,0);
    // lcd.print("at: %ld.", current >> 8);

    while (1) {
        uint32_t current = millis();
        uint16_t ana = analogRead(3);
        for (uint8_t i=0; i<5; i++) { buttons[i]->check(ana, current); }

        //lcd.setTextSize(2);
        lcd.setXY(0, 0);
        lcd.print("%d",rb1.presses);
        lcd.setXY(54, 0);
        lcd.print("%d",rb2.presses);
        lcd.setXY(100, 0);
        lcd.print("%d",rb3.presses);
    }

    // lcd.drawLine(0, 0, 127, 63, WHITE);
    // lcd.drawLine(0, 63, 127, 0, WHITE);
    // lcd.drawCircle(63, 31, 10, WHITE),
    // lcd.update();
    // delay(200);
    // lcd.clear();
    // lcd.setTextSize(1);
    // lcd.setTextColor(WHITE);
    // lcd.setCursor(0,0);
    // lcd.println("Hello, world!");
    // lcd.setTextColor(BLACK, WHITE); // 'inverted' text
    // lcd.println(3.141592);
    // lcd.setTextSize(2);
    // lcd.setTextColor(WHITE);
    // lcd.print("0x");
    // lcd.println(0xDEADBEEF, HEX);
    // lcd.update();
    // delay(200);
    // lcd.clear();
}

