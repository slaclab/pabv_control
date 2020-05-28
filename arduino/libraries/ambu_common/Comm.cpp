#include "Arduino.h"
#include "Comm.h"
#ifdef ARDUINO_ARCH_SAMD
#include "wiring_private.h"
#endif
// implementatio for MBED and SAMD
//https://stackoverflow.com/questions/57175348/softwareserial-for-arduino-nano-33-iot

Comm::Comm(uint8_t rx,uint8_t tx) : rxCount(0), _rx(rx),_tx(tx),
				    _err_trailer(0),_err_overflow(0)
#ifdef ARDUINO_ARCH_MBED
				  ,ser(digitalPinToPinName(rx), digitalPinToPinName(tx), NC,NC) 
#elif ARDUINO_ARCH_MKR
				  ,ser(Serial)
#endif
{
  memset(rxBuffer,0,sizeof(rxBuffer));
}

#ifdef ARDUINO_ARCH_SAMD
void SERCOM0_Handler()
{
  Comm::ser.IrqHandler();
}
#endif

void Comm::begin(uint32_t baud) {
#ifdef ARDUINO_ARCH_SAMD
pinPeripheral(5, PIO_SERCOM_ALT);
pinPeripheral(6, PIO_SERCOM_ALT);
#endif
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
      if(rxCount<sizeof(rxBuffer))
	rxBuffer[rxCount++] = c;
      else
        _err_overflow++;
	rxCount=0;
    }
  }
}
void Comm::send(const Message &msg) {
  ser.print(msg.getBuffer());
} 
#ifdef ARDUINO_ARCH_MBED
  
#elif  ARDUINO_ARCH_SAMD
Uart Comm::ser = Uart(&sercom0, 5, 6, SERCOM_RX_PAD_1, UART_TX_PAD_0);
#else
#endif
