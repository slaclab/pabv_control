#ifndef _COMM_H_
#define _COMM_H_
#include <Arduino.h>
#include <Message.h>
#ifdef ARDUINO_ARCH_MBED
#elif  ARDUINO_ARCH_SAMD
#else
#include <HardwareSerial.h>
#endif
//#ifdef ARDUINO_ARCH_MBED
//using SerialType=UART;
//#elif ARDUINO_ARCH_SAMD
//using SerialType=Uart;
//#else
using SerialType=HardwareSerial;
//#endif



class Comm {
 public:
  Comm(SerialType &s);
  void begin(uint32_t baud);
  void read(Message &msg);
  void send(const Message &msg);
  SerialType &ser;

 private:
char rxBuffer[256];
uint16_t rxCount;
 uint8_t  _rx,_tx;
 // error counters
 uint32_t _err_trailer;
 uint32_t _err_overflow;
};

#endif
