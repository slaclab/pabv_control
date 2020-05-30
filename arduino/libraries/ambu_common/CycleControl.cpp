
#include "CycleControl.h"
#include "AmbuConfig.h"
#include "GenericSensor.h"

#include <HardwareSerial.h>

CycleControl::CycleControl (AmbuConfig *conf,
                            GenericSensor *press,
                            GenericSensor *vol,
                            uint8_t relayAPin,
                            uint8_t relayBPin,
                            Stream *serial) {
   conf_  = conf;
   press_ = press;
   vol_   = vol;
   serial_ = serial;

   relayAPin_ = relayAPin;
   relayBPin_ = relayBPin;

   stateTime_ = 0;
   cycleCount_ = 0;
}

void CycleControl::setup() {
   stateTime_ = millis();
   cycleCount_ = 0;

   state_ = StateOff;
   status_ = 0;

   pinMode(relayAPin_, OUTPUT);
   pinMode(relayBPin_, OUTPUT);
   digitalWrite(relayAPin_, RELAY_OFF);
   digitalWrite(relayBPin_, RELAY_OFF);

   currVmax_ = 0;
   prevVmax_ = 0;
   currPmax_ = 0;
   prevPmax_ = 0;
}

void CycleControl::update(uint32_t ctime) {
   uint8_t newState;

   newState = state_;

   // Keep track of vmax
   if ( vol_->scaledValue() > currVmax_ ) currVmax_ = vol_->scaledValue();

   // Keep track of pmax
   if ( press_->scaledValue() > currPmax_ ) currPmax_ = press_->scaledValue();

   // Off portion of the cycle
   if ( state_ == StateOff ) {

      // Turn on volume threshold exceeded, and we have met min off period, and running is enabled
      if ( (conf_->getRunState() == conf_->StateRunOn) &&
           (press_->scaledValue() < conf_->getVolInThold()) &&
           ((ctime - stateTime_) > MinOffMillis) ) {

         newState = StateOn;
         status_ |= StatusVolInh;
      }

      // Turn on based upon time
      else if ( (ctime - stateTime_) > conf_->getOffTimeMillis()) {
         newState = StateOn;
         status_ &= (0xFF^StatusVolInh);
      }
   }

   // On portion of the cycle
   else if ( state_ == StateOn ) {

      // Pressure triggers only valid when run is enabled
      if (conf_->getRunState() == conf_->StateRunOn) {

         // Turn off pressure threshold exceeded
         if ( press_->scaledValue() > conf_->getAdjPipMax() )  {
            status_ |= StatusAlarmPipMax;
            newState = StateHold;
         }

         // Turn off volume threshold exceeded
         if ( vol_->scaledValue() > conf_->getAdjVolMax() ) {
            status_ |= StatusAlarmVolMax;
            newState = StateHold;
         }
      }

      // On timer has been reached
      if ((ctime - stateTime_) > conf_->getOnTimeMillis()) newState = StateOff;
   }

   // Hold
   else {

      // On timer has been reached
      if ((ctime - stateTime_) > conf_->getOnTimeMillis()) newState = StateOff;
   }

   // State change
   if ( newState != state_ ) {

      // Start of a new cycle
      if ( newState == StateOn ) {
         stateTime_ = ctime;
         vol_->reset(ctime);

         // Clear counters
         prevVmax_ = currVmax_;
         currVmax_ = 0;
         prevPmax_ = currPmax_;
         currPmax_ = 0;

         // Increment cycle count
         cycleCount_++;
      }

      // Going to off
      else if ( newState == StateOff ) {
         stateTime_ = ctime;
      }

      state_ = newState;
   }

   // Currently forced off, paddle up
   if ( conf_->getRunState() == conf_->StateForceOff ) {
      digitalWrite(relayAPin_, RELAY_OFF);
      digitalWrite(relayBPin_, RELAY_ON);
   }

   // Currently orced on, paddle down
   else if ( conf_->getRunState() == conf_->StateForceOn ) {
      digitalWrite(relayAPin_, RELAY_ON);
      digitalWrite(relayBPin_, RELAY_ON);
   }

   // Currently off, paddle up
   else if ( conf_->getRunState() == conf_->StateRunOff ) {
      digitalWrite(relayAPin_, RELAY_OFF);
      digitalWrite(relayBPin_, RELAY_ON);
   }

   // On state, paddle down
   else if ( state_ == StateOn ) {
      digitalWrite(relayAPin_, RELAY_ON);
      digitalWrite(relayBPin_, RELAY_OFF);
   }

   // Off state, paddle up
   else if ( state_ == StateOn ) {
      digitalWrite(relayAPin_, RELAY_OFF);
      digitalWrite(relayBPin_, RELAY_ON);
   }

   // Hold state
   else {
      digitalWrite(relayAPin_, RELAY_OFF);
      digitalWrite(relayBPin_, RELAY_OFF);
   }
}

void CycleControl::sendString() {
   serial_->print(" ");
   serial_->print(cycleCount_);
   serial_->print(" ");
   serial_->print(status_);
   serial_->print(" ");
   serial_->print(prevVmax_);
   serial_->print(" ");
   serial_->print(prevPmax_);
}


void CycleControl::clearAlarm() {
   status_ = 0;
}

