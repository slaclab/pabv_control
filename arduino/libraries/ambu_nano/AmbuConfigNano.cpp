
#include "AmbuConfigNano.h"

#include <HardwareSerial.h>
#include <Arduino.h>


AmbuConfigNano::AmbuConfigNano() : AmbuConfig () { }

void AmbuConfigNano::setup() {
   addr_ = 0;

   period_ = 3000;
   onTime_ = 1000;
   runState_ = StateRunOn;
   startThold_ = -10.0;
   stopThold_ = 100.0;
   volThold_ = 200.0;
   storeConfig();
   confTime_ = millis();
}

void AmbuConfigNano::storeConfig() {
  // to be implemented
}
