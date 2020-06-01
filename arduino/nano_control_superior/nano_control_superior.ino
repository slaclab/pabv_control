
#include <Wire.h>
#include <AmbuConfigNano.h>
#include <CycleControl.h>
#include <SensorDlcL20D4.h>
#include <SensorSp110Sm02Flow.h>
#include <SensorVolume.h>
#include <stdint.h>
#include <HardwareSerial.h>
#include <Comm.h>

#define RELAYA_PIN 8
#define RELAYB_PIN 7
#define SENSOR_PERIOD_MILLIS 9
//#define SerialPort Serial
#define SerialPort Serial1

// Uart on pin D9/10 for display communication
// https://stackoverflow.com/questions/57175348/softwareserial-for-arduino-nano-33-iot
#ifdef ARDUINO_ARCH_MBED
UART uart(digitalPinToPinName(9), digitalPinToPinName(10), NC,NC);
#elif  ARDUINO_ARCH_SAMD
#include "wiring_private.h"
Uart uart (&sercom0, 9, 10, SERCOM_RX_PAD_1, UART_TX_PAD_0);
// Attach the interrupt handler to the SERCOM
void SERCOM0_Handler()
{
    uart.IrqHandler();
}
#else
#error Unsupported Hardware
#endif

Comm displayComm(uart);
Comm serComm(SerialPort);

AmbuConfigNano conf(serComm);
SensorDlcL20D4 press;
SensorSp110Sm02Flow flow;
SensorVolume vol(flow);
CycleControl relay(conf,press,vol,RELAYA_PIN,RELAYB_PIN);


uint32_t sensorTime;

void setup() {

   SerialPort.begin(57600);
#ifdef  ARDUINO_ARCH_SAMD
   pinPeripheral(9, PIO_SERCOM_ALT);
   pinPeripheral(10, PIO_SERCOM_ALT);
#endif
   uart.begin(9600);
   Message m;
   m.writeString(Message::DEBUG,millis(),"Booted");
   serComm.send(m);

   Wire.begin();

   relay.setup();

   // Wait 5 seconds for pressure to settle
   m.writeString(Message::DEBUG,millis(),"Wait 5 seconds");
   serComm.send(m);
   delay(5000);

   conf.setup();
   press.setup();
   flow.setup();
   vol.setup();

   sensorTime = millis();
   m.writeString(Message::DEBUG,millis(),"Setup Done");
   serComm.send(m);
}

void loop() {
   uint32_t currTime;

   currTime = millis();

   if ((currTime - sensorTime) > SENSOR_PERIOD_MILLIS ) {

      press.update(currTime);
      flow.update(currTime);
      vol.update(currTime);
      float sendFloat[5];
      uint32_t sendInt[2];
      sendFloat[0]=relay.prevVmax();
      sendFloat[1]=relay.prevPmax();
      sendFloat[2]=press.scaledValue();
      sendFloat[3]=flow.scaledValue();
      sendFloat[4]=vol.scaledValue();
      sendInt[0]=relay.cycleCount();
      sendInt[1]=relay.status();
      // Update display every second
      Message m;
      if((currTime%1000)==0) {	
	m.writeData(Message::DATA,currTime,3,sendFloat,0,0);
	displayComm.send(m);
      }    
      m.writeData(Message::DATA,currTime,5,sendFloat,2,sendInt);
      serComm.send(m);
      sensorTime = currTime;
   }

   relay.update(currTime);
   conf.update(currTime,relay);

}
