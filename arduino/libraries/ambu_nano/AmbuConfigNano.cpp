
#include "AmbuConfigNano.h"

#include <HardwareSerial.h>
#include <Arduino.h>


AmbuConfigNano::AmbuConfigNano(Comm &serial) : AmbuConfig (serial) {
  deviceID(cpuId_);

 }

void AmbuConfigNano::setup() {

   // to be implemented
   AmbuConfig::setup();
}

void AmbuConfigNano::storeConfig() {
  // to be implemented
}

void AmbuConfigNano::deviceID(cpuId &id) {
#ifdef ARDUINO_ARCH_MBED;
  // https://infocenter.nordicsemi.com/index.jsp?topic=%2Fcom.nordic.infocenter.nrf52832.ps.v1.1%2Fficr.html
  uint32_t *deviceID=(uint32_t*) 0x10000060;
  id[0]=deviceID[0];
  id[1]=deviceID[1];
  id[2]=0;
  id[3]=0;
#elif ARDUINO_ARCH_SAMD
#define SERIAL_NUMBER_WORD_0	*(volatile uint32_t*)(0x0080A00C)
#define SERIAL_NUMBER_WORD_1	*(volatile uint32_t*)(0x0080A040)
#define SERIAL_NUMBER_WORD_2	*(volatile uint32_t*)(0x0080A044)
#define SERIAL_NUMBER_WORD_3	*(volatile uint32_t*)(0x0080A048)
  // https://cdn.sparkfun.com/assets/6/3/d/d/2/Atmel-42181-SAM-D21_Datasheet.pdf
  id[0] = SERIAL_NUMBER_WORD_0;
  id[1] = SERIAL_NUMBER_WORD_1;
  id[2] = SERIAL_NUMBER_WORD_2;
  id[3] = SERIAL_NUMBER_WORD_3;
#else
  #error Unsupported platform
#endif



}

