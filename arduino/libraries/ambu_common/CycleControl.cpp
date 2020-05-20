
#include "CycleControl.h"
#include "AmbuConfig.h"
#include "GenericSensor.h"

#include <HardwareSerial.h>

CycleControl::CycleControl (AmbuConfig *conf,
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

void CycleControl::setup() {
   stateTime_ = millis();
   cycleCount_ = 0;

   state_ = StateOff;

   pinMode(relayPin_, OUTPUT);
   digitalWrite(relayPin_, RELAY_OFF);
}

void CycleControl::update(unsigned int ctime) {

   // Currently relay forced off
   if ( conf_->getRunState() == conf_->StateForceOff ) {
      digitalWrite(relayPin_, RELAY_OFF);
      state_ = StateOff;
      stateTime_ = ctime;
   }

   // Currently relay forced on
   else if ( conf_->getRunState() == conf_->StateForceOn ) {
      digitalWrite(relayPin_, RELAY_ON);
      state_ = StateOn;
      stateTime_ = ctime;
   }

   // Currently turned off
   else if ( conf_->getRunState() == conf_->StateRunOff ) {
      digitalWrite(relayPin_, RELAY_OFF);
      state_ = StateOff;
      stateTime_ = ctime;
   }

   // Off portion of the cycle
   else if ( state_ == StateOff ) {

      // Turn on volume threshold exceeded, and we have met min off period
      if ( ( (press_->scaledValue() < conf_->getVolInThold()) && ((ctime - stateTime_) > MIN_OFF_MILLIS)) ||

           // Off timer has been reached
           ( (ctime - stateTime_) > conf_->getOffTimeMillis()) ) {

         digitalWrite(relayPin_, RELAY_ON);
         stateTime_ = ctime;
         state_ = StateOn;
         vol_->reset(ctime);
         cycleCount_++;
      }
   }

   // On portion of the cycle
   else {

      // Turn off pressure threshold exceeded
      if ( ( press_->scaledValue() > conf_->getAdjPipMax() ) ||

           // Turn off volume threshold exceeded
           ( vol_->scaledValue() > conf_->getAdjVolMax() ) ||

           // On timer has been reached
           ((ctime - stateTime_) > conf_->getOnTimeMillis()) ) {

         digitalWrite(relayPin_, RELAY_OFF);
         stateTime_ = ctime;
         state_ = StateOff;
      }
   }
}

void CycleControl::sendString() {
   sprintf(txBuffer_," %u",cycleCount_);
   Serial.write(txBuffer_);
}


void CycleControl::clearAlarm() {


}

