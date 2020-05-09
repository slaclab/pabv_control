
#include "AmbuConfig.h"
#include <EEPROM.h>

#include <HardwareSerial.h>

AmbuConfig::AmbuConfig () {
   memset(rxBuffer_,0,20);
   rxCount_ = 0;
}

void AmbuConfig::setup() {
   addr_ = 0;

   EEPROM.get(addr_, period_);
   addr_ += 4;

   EEPROM.get(addr_, onTime_);
   addr_ += 4;

   EEPROM.get(addr_, thold_);
   addr_ += 4;

   EEPROM.get(addr_, runState_);
   addr_ += 4;

   // Just in case the values are bad
   if ( period_ == 0xFFFF ) {
      period_   = 3000;
      onTime_   = 1000;
      thold_    = 0x8a14;
      runState_ = StateCycle;
      storeConfig();
   }
}

void AmbuConfig::update(unsigned int ctime) {

   // Get serial data
   while (Serial.available()) {
      if ( rxCount_ == 49 ) rxCount_ = 0;

      c_ = Serial.read();
      rxBuffer_[rxCount_++] = c_;
      rxBuffer_[rxCount_] = '\0';
   }

   // Check for incoming message
   if ( rxCount_ > 6 && rxBuffer_[rxCount_-1] == '\n') {

      // Parse string
      ret_ = sscanf(rxBuffer_,"%s %u %u %u %u", mark_, &scanPeriod_, &scanOn_, &scanThold_, &scanRun_);

      // Check marker
      if ( ret_ == 5 && strcmp(mark_,"CONFIG") == 0 ) {
         period_   = scanPeriod_;
         onTime_   = scanOn_;
         thold_    = scanThold_;
         runState_ = scanRun_;

         storeConfig();
      }
      rxCount_ = 0;
   }
}

void AmbuConfig::sendString() {
   sprintf(txBuffer_, " %u %u 0x%x %u", period_, onTime_, thold_, runState_);

   Serial.write(txBuffer_);
}

unsigned int AmbuConfig::getPeriod() {
   return period_;
}

unsigned int AmbuConfig::getOnTime() {
   return onTime_;
}

unsigned int AmbuConfig::getThold() {
   return thold_;
}

unsigned int AmbuConfig::getRunState() {
   return runState_;
}

void AmbuConfig::storeConfig() {
   addr_ = 0;

   EEPROM.put(addr_, period_);
   addr_ += 4;

   EEPROM.put(addr_, onTime_);
   addr_ += 4;

   EEPROM.put(addr_, thold_);
   addr_ += 4;

   EEPROM.put(addr_, runState_);
   addr_ += 4;
}

