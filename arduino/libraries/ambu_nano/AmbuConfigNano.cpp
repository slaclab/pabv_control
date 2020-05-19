
#include "AmbuConfigNano.h"

#include <HardwareSerial.h>
#include <Arduino.h>


AmbuConfigNano::AmbuConfigNano() : AmbuConfig () { }

void AmbuConfigNano::setup() {
   addr_ = 0;

   // Just in case the values are bad
   if ( period_ == 0xFFFF ) period_ = 3000;
   if ( onTime_ == 0xFFFF ) onTime_ = 1000;
   if ( runState_ == 0xFFFF ) runState_ = StateRunOn;

   // Cast double locations to int * to check for empty storage locations
   if ( *((unsigned int *)(&startThold_)) == 0xFFFF ) startThold_ = -10.0;
   if ( *((unsigned int *)(&stopThold_)) == 0xFFFF ) stopThold_ = 100.0;
   if ( *((unsigned int *)(&volThold_)) == 0xFFFF ) volThold_ = 200.0;

   storeConfig();
   confTime_ = millis();
}

void AmbuConfigNano::storeConfig() {
  // to be implemented
}
