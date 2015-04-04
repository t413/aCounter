// Accurate Timing from timer tick interrupts
// by Tim O'Brien (t413.com), Sept. 16, 2010.

#include "interrupt_timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>


#define clockCyclesPerMicrosecond() ( F_CPU / 1000000L )
#define clockCyclesToMicroseconds(a) ( (a) / clockCyclesPerMicrosecond() )
#define microsecondsToClockCycles(a) ( (a) * clockCyclesPerMicrosecond() )

// the prescaler is set so that timer0 ticks every 64 clock cycles, and the
// the overflow handler is called every 256 ticks.
#define MICROSECONDS_PER_TIMER0_OVERFLOW (clockCyclesToMicroseconds(64 * 256))

// the whole number of milliseconds per timer0 overflow
#define MILLIS_INC (MICROSECONDS_PER_TIMER0_OVERFLOW / 1000)

// the fractional number of milliseconds per timer0 overflow. we shift right
// by three to fit these numbers into a byte. (for the clock speeds we care
// about - 8 and 16 MHz - this doesn't lose precision.)
#define FRACT_INC ((MICROSECONDS_PER_TIMER0_OVERFLOW % 1000) >> 3)
#define FRACT_MAX (1000 >> 3)

volatile unsigned long timer0_overflow_count = 0;
volatile unsigned long timer0_millis = 0;
static unsigned char timer0_fract = 0;

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

void timer0_init(void) {
    TCCR0A = (1<<WGM01) | (1<<WGM00);
    TCCR0B = (1<<CS01) | (1<<CS00); //sets prescale to clk/64
    TIMSK0 = (1<<TOIE0); //Timer/Counter0 Overflow interrupt is enabled (overflow in Timer/Counter0 == inturrupt)
    // DDRF |= (1<<6); //debug output on Port F6
    // if (PORTF & (1<<6)) { PORTF &= ~(1<<6); } //off
    // else { PORTF |= (1<<6); } //on
}


ISR(TIMER0_OVF_vect) {
    unsigned long m = timer0_millis;
    unsigned char f = timer0_fract;

    m += MILLIS_INC;
    f += FRACT_INC;
    if (f >= FRACT_MAX) {
        f -= FRACT_MAX;
        m += 1;
    }

    timer0_fract = f;
    timer0_millis = m;
    timer0_overflow_count++;
}

unsigned long millis() {
    unsigned long m;
    uint8_t oldSREG = SREG;

    // disable interrupts while we read timer0_millis or we might get an
    // inconsistent value (e.g. in the middle of a write to timer0_millis)
    cli();
    m = timer0_millis;
    SREG = oldSREG;
    sei();
    return m;
}

unsigned long micros() {
    unsigned long m;
    uint8_t oldSREG = SREG, t;

    cli();
    m = timer0_overflow_count;
    t = TCNT0;
    if ((TIFR0 & _BV(TOV0)) && (t < 255))
        m++;
    SREG = oldSREG;
    sei();
    return ((m << 8) + t) * (64 / clockCyclesPerMicrosecond());
}

void delay(unsigned long ms) {
    uint16_t start = (uint16_t)micros();

    while (ms > 0) {
        if (((uint16_t)micros() - start) >= 1000) {
            ms--;
            start += 1000;
        }
    }
}

/* Delay for the given number of microseconds.  Assumes a 8 or 16 MHz clock. */
void delayMicroseconds(unsigned int us) {
    //delay_us(us);
    if (--us == 0)
        return;
    if (--us == 0)
        return;
    us <<= 1;
    us--;
    // busy wait
    __asm__ __volatile__ (
    "1: sbiw %0,1" "\n\t" // 2 cycles
            "brne 1b" : "=w" (us) : "0" (us) // 2 cycles
    );
}
