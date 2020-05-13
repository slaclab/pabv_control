
#include "RelayControl.h"
#include "AmbuConfig.h"
#include "GenericSensor.h"

#include <HardwareSerial.h>

RelayControl::RelayControl (AmbuConfig *conf,
                            GenericSensor *press,
                            GenericSensor *vol,
                            unsigned int relayPin) {
   conf_  = conf;
   press_ = press;
   vol_   = vol;

   relayPin_ = relayPin;

   stateTime_ = 0;
   cycleCount_ = 0;
}

void RelayControl::setup() {
   stateTime_ = millis();
   cycleCount_ = 0;

   state_ = StateOff;

   pinMode(relayPin_, OUTPUT);
   digitalWrite(relayPin_, RELAY_OFF);
}

void RelayControl::update(unsigned int ctime) {

   // Currently forced off
   if ( state_ == StateOff ) {

      // Transition to Forced on
      if ( conf_->getRunState() == conf_->StateOn ) {
         digitalWrite(relayPin_, RELAY_ON);
         stateTime_ = ctime;
         state_ = StateOn;
      }

      // Transition to Cycle
      else if ( conf_->getRunState() == conf_->StateCycle ) {
         digitalWrite(relayPin_, RELAY_OFF);
         stateTime_ = ctime;
         state_ = StateCycleOff;
      }
   }

   // Currently forced on
   else if ( state_ == StateOn ) {

      // Transition to Forced off
      if ( conf_->getRunState() == conf_->StateOff ) {
         digitalWrite(relayPin_, RELAY_OFF);
         stateTime_ = ctime;
         state_ = StateOff;
      }

      // Transition to Cycle
      else if ( conf_->getRunState() == conf_->StateCycle ) {
         digitalWrite(relayPin_, RELAY_OFF);
         stateTime_ = ctime;
         state_ = StateCycleOff;
      }
   }

   // Currently off portion of cycle
   else if ( state_ == StateCycleOff ) {

      // Transition to forced off
      if ( conf_->getRunState() == conf_->StateOff ) {
         digitalWrite(relayPin_, RELAY_OFF);
         stateTime_ = ctime;
         state_ = StateOff;
      }

      // Transition to forced on
      else if ( conf_->getRunState() == conf_->StateOn ) {
         digitalWrite(relayPin_, RELAY_ON);
         stateTime_ = ctime;
         state_ = StateOn;
      }

      // Turn on threshold exceeded, and we have met min off period
      else if ( (press_->scaledValue() < conf_->getStartThold()) && ((ctime - stateTime_) > MIN_OFF_MILLIS)) {
         digitalWrite(relayPin_, RELAY_ON);
         stateTime_ = ctime;
         state_ = StateCycleOn;
         vol_->reset(ctime);
         cycleCount_++;
      }

      // Off timer has been reached
      else if ((ctime - stateTime_) > (conf_->getPeriod() - conf_->getOnTime())) {
         digitalWrite(relayPin_, RELAY_ON);
         stateTime_ = ctime;
         state_ = StateCycleOn;
         vol_->reset(ctime);
         cycleCount_++;
      }
   }

   // Currently on portion of cycle
   else if (state_ == StateCycleOn ) {

      // Transition to forced off
      if ( conf_->getRunState() == conf_->StateOff ) {
         digitalWrite(relayPin_, RELAY_OFF);
         stateTime_ = ctime;
         state_ = StateOff;
      }

      // Transition to forced on
      else if ( conf_->getRunState() == conf_->StateOn ) {
         digitalWrite(relayPin_, RELAY_ON);
         stateTime_ = ctime;
         state_ = StateOn;
      }

      // Turn off threshold exceeded
      else if ( press_->scaledValue() > conf_->getStopThold() ) {
         digitalWrite(relayPin_, RELAY_OFF);
         stateTime_ = ctime;
         state_ = StateCycleOff;
      }

      // On timer has been reached
      else if ((ctime - stateTime_) > conf_->getOnTime()) {
         digitalWrite(relayPin_, RELAY_OFF);
         stateTime_ = ctime;
         state_ = StateCycleOff;
      }
   }
}

void RelayControl::sendString() {
   sprintf(txBuffer_," %u",cycleCount_);
   Serial.write(txBuffer_);
}

