#ifndef _COMM_H_
#define _COMM_H_

class Comm {
 public:
  Comm();

  void begin();
  void read(float *data);
  void send(const float *data);
 private:
  UART ser;  
char rxBuffer[256];
uint16_t rxCount=0;
};

#endif
