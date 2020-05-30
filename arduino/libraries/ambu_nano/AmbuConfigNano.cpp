
#include "AmbuConfigNano.h"

#include <HardwareSerial.h>
#include <Arduino.h>


AmbuConfigNano::AmbuConfigNano(Stream *serial) : AmbuConfig (serial) { }

void AmbuConfigNano::setup() {

   // to be implemented
   AmbuConfig::setup();
}

void AmbuConfigNano::storeConfig() {
  // to be implemented
}

