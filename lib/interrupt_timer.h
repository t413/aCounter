// Accurate Timing from timer tick interrupts
// by Tim O'Brien (t413.com), Sept. 16, 2010.

#pragma once
#ifdef __cplusplus
extern "C" {
#endif


void timer0_init(void);
unsigned long millis();
unsigned long micros();
void delay(unsigned long ms);

#ifdef __cplusplus
}
#endif
