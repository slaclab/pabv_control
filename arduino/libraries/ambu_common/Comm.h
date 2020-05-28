#ifndef _COMM_H_
#define _COMM_H_
#include <Arduino.h>
#include <Message.h>
#ifdef ARDUINO_ARCH_MBED
#elif  ARDUINO_ARCH_SAMD
#else
#include <HardwareSerial.h>
#endif
class Comm {
 public:
  Comm(uint8_t rx,uint8_t tx);

  void begin(uint32_t baud);
  void read(Message &msg);
  void send(const Message &msg);
#ifdef ARDUINO_ARCH_MBED
  UART ser;  
#elif ARDUINO_ARCH_SAMD
  static Uart ser;
#else
  HardwareSerial &ser;
#endif
 private:
char rxBuffer[256];
uint16_t rxCount;
 uint8_t  _rx,_tx;
 // error counters
 uint32_t _err_trailer;
 uint32_t _err_overflow;
};

#endif
