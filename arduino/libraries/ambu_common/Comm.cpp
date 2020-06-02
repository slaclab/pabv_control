#include "Arduino.h"
#include "Comm.h"
// implementation for MBED and SAMD
//https://stackoverflow.com/questions/57175348/softwareserial-for-arduino-nano-33-iot






Comm::Comm(Stream &s) : rxCount(0),ser(s),
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


void Comm::read(Message &msg) {
  while (ser.available()) {
    char c= ser.read();  
    if(c=='-') {
      rxBuffer[rxCount] = '\0';
      if(1) {
        msg.decode(rxCount,rxBuffer);
	Serial.println(msg.nInt());
	Serial.println(msg.nFloat());
	for(unsigned i=0;i<msg.nInt();i++) Serial.println(msg.getInt()[i]);
	for(unsigned i=0;i<msg.nFloat();i++) Serial.println(msg.getFloat()[i]);
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
