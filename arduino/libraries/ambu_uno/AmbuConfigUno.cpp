
#include "AmbuConfigUno.h"
#include <EEPROM.h>

#include <HardwareSerial.h>
#include <Arduino.h>


AmbuConfigUno::AmbuConfigUno () : AmbuConfig () { }

void AmbuConfigUno::setup() {
   addr_ = 0;

   EEPROM.get(addr_, period_);
   addr_ += 4;

   EEPROM.get(addr_, onTime_);
   addr_ += 4;
   addr_ += 4; // Old config

   EEPROM.get(addr_, runState_);
   addr_ += 4;
   addr_ += 4; // Old config

   EEPROM.get(addr_, startThold_);
   addr_ += 4;

   EEPROM.get(addr_, stopThold_);
   addr_ += 4;

   EEPROM.get(addr_, volThold_);
   addr_ += 4;

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

void AmbuConfigUno::storeConfig() {
   addr_ = 0;

   EEPROM.put(addr_, period_);
   addr_ += 4;

   EEPROM.put(addr_, onTime_);
   addr_ += 4;
   addr_ += 4; // Old config

   EEPROM.put(addr_, runState_);
   addr_ += 4;
   addr_ += 4; // Old config

   EEPROM.put(addr_, startThold_);
   addr_ += 4;

   EEPROM.put(addr_, stopThold_);
   addr_ += 4;

   EEPROM.put(addr_, volThold_);
}
