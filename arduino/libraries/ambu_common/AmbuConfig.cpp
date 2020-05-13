
#include "AmbuConfig.h"
#include <EEPROM.h>

#include <HardwareSerial.h>
#include <Arduino.h>

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
   if ( runState_ == 0xFFFF ) runState_ = StateCycle;

   // Cast double locations to int * to check for empty storage locations
   if ( *((unsigned int *)(&startThold_)) == 0xFFFF ) startThold_ = -10.0;
   if ( *((unsigned int *)(&stopThold_)) == 0xFFFF ) stopThold_ = 100.0;
   if ( *((unsigned int *)(&volThold_)) == 0xFFFF ) volThold_ = 200.0;

   storeConfig();
   confTime_ = millis();
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
   if ( rxCount_ > 7 && rxBuffer_[rxCount_-1] == '\n') {

      // Parse string
      ret_ = sscanf(rxBuffer_,"%s %s %s %s %s %s %s", mark_, scanPeriod_, scanOn_, scanStartThold_, scanRun_, scanStopThold_, scanVolThold_);

      // Check marker
      if ( ret_ == 7 && strcmp(mark_,"CONFIG") == 0 ) {
         period_     = atoi(scanPeriod_);
         onTime_     = atoi(scanOn_);
         startThold_ = atof(scanStartThold_);
         runState_   = atoi(scanRun_);
         stopThold_  = atof(scanStopThold_);
         volThold_   = atof(scanVolThold_);
         storeConfig();
      }
      rxCount_ = 0;
   }

   if ((ctime - confTime_) > CONFIG_MILLIS) {
       Serial.print("CONFIG ");
       Serial.print(period_);
       Serial.print(" ");
       Serial.print(onTime_);
       Serial.print(" ");
       Serial.print(startThold_);
       Serial.print(" ");
       Serial.print(runState_);
       Serial.print(" ");
       Serial.print(stopThold_);
       Serial.print(" ");
       Serial.print(volThold_);
       Serial.print("\n");
       confTime_ = ctime;
   }
}

unsigned int AmbuConfig::getPeriod() {
   return period_;
}

unsigned int AmbuConfig::getOnTime() {
   return onTime_;
}

double AmbuConfig::getStartThold() {
   return startThold_;
}

double AmbuConfig::getStopThold() {
   return stopThold_;
}

double AmbuConfig::getVolThold() {
   return volThold_;
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

