#ifndef _COMM_H_
#define _COMM_H_
#include <Arduino.h>
#include <Message.h>




class Comm {
 public:
  Comm(Stream &s);
  void begin(uint32_t baud);
  void read(Message &msg);
  void send(const Message &msg);
  Stream &ser;

 private:
char rxBuffer[256];
uint16_t rxCount;
 uint8_t  _rx,_tx;
 // error counters
 uint32_t _err_trailer;
 uint32_t _err_overflow;
};

#endif
