#include "i2c_avr.h"

#include <stdlib.h>
#include "Arduino.h"
#include <avr/pgmspace.h>
#include <avr/io.h>

#define I2C_PULLUPS_ENABLE    PORTC |= 1<<4; PORTC |= 1<<5;   // PIN A4&A5 (SDA&SCL)
#define I2C_PULLUPS_DISABLE   PORTC &= ~(1<<4); PORTC &= ~(1<<5);

void i2c_avr::init(unsigned long i2c_speed, uint8_t pullups) {
  neutralizeTime = i2c_errors_count = 0;
  if (pullups) {
    I2C_PULLUPS_ENABLE
  } else {
    I2C_PULLUPS_DISABLE
  }
  TWSR = 0;                                    // no prescaler => prescaler = 1
  setSpeed(i2c_speed);
  TWCR = 1<<TWEN;                              // enable twi module, no interrupt
}

void i2c_avr::setSpeed(unsigned long i2c_speed) {
  TWBR = ((F_CPU / i2c_speed) - 16) / 2;   // change the I2C clock rate
}

void i2c_avr::rep_start(uint8_t address) {
  TWCR = (1<<TWINT) | (1<<TWSTA) | (1<<TWEN) ; // send REPEAT START condition
  waitTransmissionI2C();                       // wait until transmission completed
  TWDR = address;                              // send device address
  TWCR = (1<<TWINT) | (1<<TWEN);
  waitTransmissionI2C();                       // wail until transmission completed
}

void i2c_avr::stop(void) {
  TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
  //  while(TWCR & (1<<TWSTO));                // <- can produce a blocking state with some WMP clones
}

void i2c_avr::write(uint8_t data ) {	
  TWDR = data;                                 // send data to the previously addressed device
  TWCR = (1<<TWINT) | (1<<TWEN);
  waitTransmissionI2C();
}

uint8_t i2c_avr::read(uint8_t ack) {
  TWCR = (1<<TWINT) | (1<<TWEN) | (ack? (1<<TWEA) : 0);
  waitTransmissionI2C();
  uint8_t r = TWDR;
  if (!ack) stop();
  return r;
}

uint8_t i2c_avr::readAck() {
  return read(1);
}

uint8_t i2c_avr::readNak(void) {
  return read(0);
}

void i2c_avr::waitTransmissionI2C() {
  uint16_t count = 255;
  while (!(TWCR & (1<<TWINT))) {
    count--;
    if (count==0) {              //we are in a blocking state => we don't insist
      TWCR = 0;                  //and we force a reset on TWINT register
      neutralizeTime = micros(); //we take a timestamp here to neutralize the value during a short delay
      i2c_errors_count++;
      break;
    }
  }
}

size_t i2c_avr::read_to_buf(uint8_t add, void *buf, size_t size) {
  rep_start((add<<1) | 1);	// I2C read direction
  size_t bytes_read = 0;
  uint8_t *b = (uint8_t*)buf;
  while (size--) {
    /* acknowledge all but the final byte */
    *b++ = read(size > 0);
    /* TODO catch I2C errors here and abort */
    bytes_read++;
  }
  return bytes_read;
}

size_t i2c_avr::read_reg_to_buf(uint8_t add, uint8_t reg, void *buf, size_t size) {
  rep_start(add<<1); // I2C write direction
  write(reg);        // register selection
  return read_to_buf(add, buf, size);
}

void i2c_avr::writeReg(uint8_t add, uint8_t reg, uint8_t val) {
  rep_start(add<<1); // I2C write direction
  write(reg);        // register selection
  write(val);        // value to write in register
  stop();
}

/* transform a series of bytes from big endian to little
   endian and vice versa. */
void swap_endianness(void *buf, size_t size) {
  /* we swap in-place, so we only have to
  * place _one_ element on a temporary tray
  */
  uint8_t tray;
  uint8_t *from;
  uint8_t *to;
  /* keep swapping until the pointers have assed each other */
  for (from = (uint8_t*)buf, to = &from[size-1]; from < to; from++, to--) {
    tray = *from;
    *from = *to;
    *to = tray;
  }
}

