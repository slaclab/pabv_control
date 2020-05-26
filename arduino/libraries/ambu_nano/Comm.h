#ifndef _COMM_H_
#define _COMM_H_

class Comm {
 public:
  Comm();

  void begin();
  void read(float *data);
  void send(const float *data);
 private:
#ifdef ARDUINO_ARCH_MBED
  UART ser;  
#else
 public:
  static Uart ser;
#endif
 private:
char rxBuffer[256];
uint16_t rxCount;
};

#endif
