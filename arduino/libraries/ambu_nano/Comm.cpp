
#include "Arduino.h"
#include "Comm.h"

// implementatio for MBED and SAMD
//https://stackoverflow.com/questions/57175348/softwareserial-for-arduino-nano-33-iot

#ifdef ARDUINO_ARCH_MBED
Comm::Comm() : ser(digitalPinToPinName(5), digitalPinToPinName(6), NC,NC) {

  rxCount=0;
  memset(rxBuffer,0,sizeof(rxBuffer));
}
#else
#include "wiring_private.h"
Comm::Comm()  {
  rxCount=0;
  memset(rxBuffer,0,sizeof(rxBuffer));
}
void SERCOM0_Handler()
{
  Comm::ser.IrqHandler();
}
#endif
#ifdef ARDUINO_ARCH_MBED
void Comm::begin() {
  ser.begin(9600);
}
#else
void Comm::begin() {
pinPeripheral(5, PIO_SERCOM_ALT);
pinPeripheral(6, PIO_SERCOM_ALT);
ser.begin(9600);
}
#endif

void Comm::read(float *data) {
  while (ser.available()) {
    char c= ser.read();  
    if(c=='\n') {
      rxBuffer[rxCount] = '\0';    
      float f[9];
      int res=sscanf(rxBuffer,"PARAM %f %f %f %f %f %f %f %f %f",
		     &f[0],&f[1],&f[2],
		     &f[3],&f[4],&f[5],
		     &f[6],&f[7],&f[9]
		     );
      if(res==9) for(unsigned i=0;i<9;i++) data[i]=f[i];
      rxCount = 0;
      
    } else {
      if(rxCount<sizeof(rxBuffer))
	rxBuffer[rxCount++] = c;
      else
	rxCount=0;
    }
  }
}
void Comm::send(const float *data) {
  ser.print("PARAM ");
  char s[64];
  for(unsigned i=0;i<9;i++) {
    sprintf(s,"%f",data[i]);
    ser.print(s);
    ser.print(" ");
  }
  ser.print("\n");
} 
#ifdef ARDUINO_ARCH_MBED

#else
Uart Comm::ser = Uart(&sercom0, 5, 6, SERCOM_RX_PAD_1, UART_TX_PAD_0);
#endif
