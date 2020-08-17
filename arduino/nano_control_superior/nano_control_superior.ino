
#include <Wire.h>
#include <AmbuConfig.h>
#include <CycleControl.h>
#include <SensorDlcL20D4.h>
#include <SensorSp110Sm02Flow.h>
#include <SensorVolume.h>
#include <stdint.h>
#include <HardwareSerial.h>
#include <Comm.h>
#include <WDTZero.h>
#include <Arduino.h>
#define RELAYA_PIN 8
#define RELAYB_PIN 7
#define AUDIO_HIGH_PIN 4
#define AUDIO_MED_PIN  3
#define AUDIO_LOW_PIN  2
#define PIN_12V A6
#define PIN_9V  A7
#define SENSOR_PERIOD_MILLIS 9
#define DISPLAY_PERIOD_MILLIS 999
//#define SerialPort Serial
#define SerialPort Serial1  // Interface to the MAX3232

// Uart on pin D9/10 for display communication
// https://stackoverflow.com/questions/57175348/softwareserial-for-arduino-nano-33-iot
#ifdef ARDUINO_ARCH_MBED
UART uart(digitalPinToPinName(5), digitalPinToPinName(6), NC,NC);
#elif  ARDUINO_ARCH_SAMD
#include "wiring_private.h"
Uart uart (&sercom1, 12, 11, SERCOM_RX_PAD_3, UART_TX_PAD_0);
// Attach the interrupt handler to the SERCOM
void SERCOM1_Handler()
{
    uart.IrqHandler();
}
#else
#error Unsupported Hardware
#endif


WDTZero watchdog;


Comm displayComm(uart);    // To second NANO for local-display
Comm serComm(SerialPort);  // To MAX3232


AmbuConfig conf(serComm,displayComm);
SensorDlcL20D4 press;
SensorSp110Sm02Flow flow;
SensorVolume vol(flow);
CycleControl relay(conf,press,vol,RELAYA_PIN,RELAYB_PIN,AUDIO_HIGH_PIN,AUDIO_MED_PIN,AUDIO_LOW_PIN,PIN_12V,PIN_9V);


uint32_t sensorTime;
uint32_t displayTime;
void setup() {
#ifdef  ARDUINO_ARCH_SAMD
     pinPeripheral(11, PIO_SERCOM);
     pinPeripheral(12, PIO_SERCOM);
#endif
   SerialPort.begin(57600);
   uart.begin(9600);
   Serial.begin(57600);
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
   watchdog.setup(WDT_HARDCYCLE2S );
}

void loop() {
   uint32_t currTime=millis();

   if ((currTime - sensorTime) > SENSOR_PERIOD_MILLIS ) {
      press.update(currTime);
      flow.update(currTime);
      vol.update(currTime);
      float sendFloat[6];
      uint32_t sendInt[3];
      sendFloat[0]=relay.prevVmax();
      sendFloat[1]=relay.prevPmax();
      sendFloat[2]=press.scaledValue();
      sendFloat[3]=flow.scaledValue();
      sendFloat[4]=vol.scaledValue();
      sendFloat[5]=relay.ieRatio();
      sendInt[0]=relay.cycleCount();
      sendInt[1]=relay.status();
      sendInt[2]=relay.onTime();
      // Update display every second
      Message m;
      m.writeData(Message::DATA,currTime,6,sendFloat,3,sendInt);
      serComm.send(m);
      sensorTime = currTime;
   }
   // Update display every second
   if ((currTime - displayTime) > DISPLAY_PERIOD_MILLIS )  {
     Message m;
      float sendFloat[10];
      uint32_t sendInt[3];
      float parms[6];
      sendFloat[0]=relay.prevPmin();
      sendFloat[1]=relay.prevPmax();
      sendFloat[2]=relay.prevVmax();
      sendFloat[3]=relay.ieRatio();
      sendFloat[4]=conf.getRespRate();
      sendFloat[5]=conf.getInhTime();
      sendFloat[6]=conf.getVolInThold();
      sendFloat[7]=conf.getVolMax();
      sendFloat[8]=conf.getPeepMin();
      sendFloat[9]=conf.getPipMax();
      sendInt[0]=conf.getRunState();
      sendInt[1]=conf.getRunMode();
      sendInt[2]=relay.status();
     m.writeData(Message::DATA,currTime,10,sendFloat,3,sendInt);
     displayComm.send(m);
     displayTime=currTime;
     // reset watchdog
     watchdog.clear();
   }

   relay.update(currTime);
   conf.update(currTime,relay);



}
