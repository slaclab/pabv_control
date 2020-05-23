
#include "CycleControl.h"
#include "AmbuConfig.h"
#include "GenericSensor.h"

#include <HardwareSerial.h>

CycleControl::CycleControl (AmbuConfig *conf,
                            GenericSensor *press,
                            GenericSensor *vol,
                            uint8_t relayPin) {
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
   alarmState_ = 0;

   pinMode(relayPin_, OUTPUT);
   digitalWrite(relayPin_, RELAY_OFF);

   currVmax_ = 0;
   prevVmax_ = 0;
   currPmax_ = 0;
   prevPmax_ = 0;
}

void CycleControl::update(uint32_t ctime) {

   // Keep track of vmax
   if ( vol_->scaledValue() > currVmax_ ) currVmax_ = vol_->scaledValue();

   // Keep track of pmax
   if ( press_->scaledValue() > currPmax_ ) currPmax_ = press_->scaledValue();

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
         prevVmax_ = currVmax_;
         currVmax_ = 0;
         prevPmax_ = currPmax_;
         currPmax_ = 0;
      }
   }

   // On portion of the cycle
   else {

      // Turn off pressure threshold exceeded
      if ( press_->scaledValue() > conf_->getAdjPipMax() )  {
         alarmState_ |= AlarmPipMax;
         digitalWrite(relayPin_, RELAY_OFF);
         stateTime_ = ctime;
         state_ = StateOff;
      }

      // Turn off volume threshold exceeded
      if ( vol_->scaledValue() > conf_->getAdjVolMax() ) {
            alarmState_ |= AlarmVolMax;
            digitalWrite(relayPin_, RELAY_OFF);
            stateTime_ = ctime;
            state_ = StateOff;
      }

      // On timer has been reached
      if ((ctime - stateTime_) > conf_->getOnTimeMillis()) {
         digitalWrite(relayPin_, RELAY_OFF);
         stateTime_ = ctime;
         state_ = StateOff;
      }
   }
}

void CycleControl::sendString() {
   Serial.print(" ");
   Serial.print(cycleCount_);
   Serial.print(" ");
   Serial.print(alarmState_);
   Serial.print(" ");
   Serial.print(prevVmax_);
   Serial.print(" ");
   Serial.print(prevPmax_);
}


void CycleControl::clearAlarm() {
   alarmState_ = 0;
}

