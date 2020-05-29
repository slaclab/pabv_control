
#include "CycleControl.h"
#include "AmbuConfig.h"
#include "GenericSensor.h"

#include <HardwareSerial.h>

CycleControl::CycleControl (AmbuConfig *conf,
                            GenericSensor *press,
                            GenericSensor *vol,
                            uint8_t relayPin,
                            Stream *serial) {
   conf_  = conf;
   press_ = press;
   vol_   = vol;
   serial_ = serial;

   relayPin_ = relayPin;

   stateTime_ = 0;
   cycleCount_ = 0;
}

void CycleControl::setup() {
   stateTime_ = millis();
   cycleCount_ = 0;

   state_ = StateOff;
   status_ = 0;

   pinMode(relayPin_, OUTPUT);
   digitalWrite(relayPin_, RELAY_OFF);

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
   else {

      // Pressure triggers only valid when run is enabled
      if (conf_->getRunState() == conf_->StateRunOn) {

         // Turn off pressure threshold exceeded
         if ( press_->scaledValue() > conf_->getAdjPipMax() )  {
            status_ |= StatusAlarmPipMax;
            newState = StateOff;
         }

         // Turn off volume threshold exceeded
         if ( vol_->scaledValue() > conf_->getAdjVolMax() ) {
            status_ |= StatusAlarmVolMax;
            newState = StateOff;
         }
      }

      // On timer has been reached
      if ((ctime - stateTime_) > conf_->getOnTimeMillis()) {
         newState = StateOff;
      }
   }

   // State change
   if ( newState != state_ ) {

      // Start of a new cycle
      if ( newState == StateOn ) {
         vol_->reset(ctime);

         // Clear counters
         prevVmax_ = currVmax_;
         currVmax_ = 0;
         prevPmax_ = currPmax_;
         currPmax_ = 0;

         // Increment cycle count
         cycleCount_++;
      }

      stateTime_ = ctime;
      state_ = newState;
   }

   // Currently relay forced off
   if ( conf_->getRunState() == conf_->StateForceOff ) {
      digitalWrite(relayPin_, RELAY_OFF);
   }

   // Currently relay forced on
   else if ( conf_->getRunState() == conf_->StateForceOn ) {
      digitalWrite(relayPin_, RELAY_ON);
   }

   // Currently turned off
   else if ( conf_->getRunState() == conf_->StateRunOff ) {
      digitalWrite(relayPin_, RELAY_OFF);
   }

   // On state
   else if ( state_ == StateOn ) {
      digitalWrite(relayPin_, RELAY_ON);
   }

   // Off state
   else {
      digitalWrite(relayPin_, RELAY_OFF);
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

