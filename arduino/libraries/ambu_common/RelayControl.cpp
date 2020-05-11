
#include "RelayControl.h"
#include "AmbuConfig.h"
#include "GenericSensor.h"

#include <HardwareSerial.h>

RelayControl::RelayControl (AmbuConfig *conf, GenericSensor *press, unsigned int relayPin) {
   conf_ = conf;
   press_ = press;

   relayPin_ = relayPin;

   relayTime_ = 0;
   cycleCount_ = 0;

}

void RelayControl::setup() {
   relayTime_ = millis();
   cycleCount_ = 0;

   pinMode(relayPin_, OUTPUT);
   digitalWrite(relayPin_, RELAY_OFF);
}

void RelayControl::update(unsigned int ctime) {

   // First check autostart
   if ( press_->scaledValue() > conf_->getThold() ) autoStart_ = 1;
   else autoStart_ = 0;

   // Turn off time
   if ((ctime - relayTime_) > conf_->getOnTime() ) inhalation_ = 0;

   // End of cycle, turn on
   if ((inhalation_ == 0) && ((autoStart_ == 1) || ((ctime - relayTime_) > conf_->getPeriod()))) {
      relayTime_ = ctime;
      inhalation_  = 1;
      cycleCount_++;
   }

   // set relay relay
   if ( conf_->getRunState() == conf_->StateOff ) digitalWrite(relayPin_, RELAY_OFF);
   else if ( conf_->getRunState() == conf_->StateOn ) digitalWrite(relayPin_, RELAY_ON);
   else if ( conf_->getRunState() == conf_->StateCycle ) {
      if ( inhalation_ == 1 ) digitalWrite(relayPin_, RELAY_ON);
      else digitalWrite(relayPin_, RELAY_OFF);
   }
}

void RelayControl::sendString() {
   sprintf(txBuffer_," %u",cycleCount_);
   Serial.write(txBuffer_);
}

