#pragma once
#include <stdint.h>
#include <stdio.h>

class i2c_avr {
public:
  void init(unsigned long i2c_speed=100000L, uint8_t pullups=1);
  void setSpeed(unsigned long i2c_speed);
  
  void write(uint8_t data );
  uint8_t read(uint8_t ack);
  void writeReg(uint8_t add, uint8_t reg, uint8_t val) ;
  size_t read_to_buf(uint8_t add, void *buf, size_t size);
  size_t read_reg_to_buf(uint8_t add, uint8_t reg, void *buf, size_t size);

  int16_t  i2c_errors_count;
protected:
  void rep_start(uint8_t address);
  void stop(void);
  uint8_t readAck();
  uint8_t readNak();
  void waitTransmissionI2C();

  uint32_t neutralizeTime;
};

void swap_endianness(void *buf, size_t size);


