#include "Arduino.h"
#include "Comm.h"
#ifdef ARDUINO_ARCH_SAMD
#include "wiring_private.h"
#endif
// implementation for MBED and SAMD
//https://stackoverflow.com/questions/57175348/softwareserial-for-arduino-nano-33-iot






Comm::Comm(SerialType &s) : rxCount(0),ser(s),
				    _err_trailer(0),_err_overflow(0)
{
  memset(rxBuffer,0,sizeof(rxBuffer));   
}


  //#ifdef ARDUINO_ARCH_SAMD
  //static Uart *serInstance=nullptr;
  //void SERCOM0_Handler()
  //{
  //if(serInstance) serInstance->IrqHandler();
  //}
  //#endif

void Comm::begin(uint32_t baud) {
//#ifdef ARDUINO_ARCH_SAMD
//serInstance=&this->ser;
//pinPeripheral(_rx, PIO_SERCOM_ALT);
//pinPeripheral(_tx, PIO_SERCOM_ALT);
//#endif
 ser.begin(baud);
}

void Comm::read(Message &msg) {
  while (ser.available()) {
    char c= ser.read();  
    if(c=='-') {
      char c1=ser.read();
      char c2=ser.read();
      rxBuffer[rxCount] = '\0';
      if(c1=='-' && c2=='-') {
        msg.decode(rxCount,rxBuffer);
      } else {
        _err_trailer++;
      }
      rxCount=0;      
    } else {
      if(rxCount<sizeof(rxBuffer)) {
	rxBuffer[rxCount++] = c;
      } else {
        _err_overflow++;
	rxCount=0;
      }
    }
  }
}
void Comm::send(const Message &msg) {
  ser.print(msg.getBuffer());
} 
