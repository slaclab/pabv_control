
#include "CycleControl.h"
#include "AmbuConfig.h"
#include "GenericSensor.h"

CycleControl::CycleControl (AmbuConfig &conf,
                            GenericSensor &press,
                            GenericSensor &vol,
                            uint8_t relayAPin,
                            uint8_t relayBPin,
                            uint8_t redLedPin,
                            uint8_t yelLedPin,
                            uint8_t piezoPin,
                            uint8_t pin12V,
                            uint8_t pin9V):
             conf_(conf),
             press_(press),
             vol_(vol),
             relayAPin_(relayAPin),
             relayBPin_(relayBPin),
             redLedPin_(redLedPin),
             yelLedPin_(yelLedPin),
             piezoPin_(piezoPin),
             pin9V_(pin9V),
             pin12V_(pin12V),
             stateTime_(0),
             cycleCountTotal_(0),
             cycleCountReal_(0),
             onTime_(0),
             onStartTime_(0),
             ieRatio_(0),
             inhTime_(0),
             muteTime_(0),
             wasOff_(true)
{  }


void CycleControl::setup() {
   stateTime_ = millis();
   onTime_ = 0;
   onStartTime_ = millis();
   ieRatio_ = 0.0;
   inhTime_ = 0;

   cycleCountTotal_ = 0;
   cycleCountReal_ = 0;

   state_ = StateOff;
   cycleStatus_ = 0;
   currStatus_ = 0;

   pinMode(relayAPin_, OUTPUT);
   pinMode(relayBPin_, OUTPUT);
   pinMode(redLedPin_, OUTPUT);
   pinMode(yelLedPin_, OUTPUT);
   pinMode(piezoPin_,  OUTPUT);

   digitalWrite(relayAPin_, RELAY_OFF);
   digitalWrite(relayBPin_, RELAY_OFF);
   digitalWrite(redLedPin_, LED_OFF);
   digitalWrite(yelLedPin_, LED_OFF);
   digitalWrite(piezoPin_,  PIEZO_OFF);

   currVmax_ = 0;
   prevVmax_ = 0;
   currPmax_ = 0;
   prevPmax_ = 0;
   currPmin_ = 99.9;
   prevPmin_ = 99.9;
}

void CycleControl::update(uint32_t ctime) {
   uint8_t newState;
   uint32_t val;

   newState = state_;

   // Keep track of vmax
   if ( vol_.scaledValue() > currVmax_ ) currVmax_ = vol_.scaledValue();

   // Keep track of pmax (PIP)
   if ( press_.scaledValue() > currPmax_ ) currPmax_ = press_.scaledValue();

   // Keep track of pmin (PEEP)
   if ( press_.scaledValue() < currPmin_ ) currPmin_ = press_.scaledValue();

   // Pressure checks for alarms
   if ((conf_.getRunState() == conf_.StateRunOn) && (cycleCountReal_ > 5) ) {

      // Max pressure threshold exceeded
      if ( press_.scaledValue() > conf_.getPipMax() )  {
         cycleStatus_ |= StatusAlarmPipMax;
         currStatus_  |= StatusAlarmPipMax;
      }

      // Min pressure threshold exceeded
      if ( press_.scaledValue() < conf_.getPeepMin() )  {
         cycleStatus_ |= StatusWarnPeepMin;
         currStatus_  |= StatusWarnPeepMin;
      }
   }

   // Check 9V level, double check if below threshold
   if ( (( val = analogRead(pin9V_)) < 230 ) && (cycleCountReal_ > 5) ) {
      if ( ( val = analogRead(pin9V_)) < 230 ) {
         cycleStatus_ |= StatusWarn9V;
         currStatus_  |= StatusWarn9V;

         Serial.print("9V = ");
         Serial.println(val);
      }
   }

   // Check 12V level, double check if below threshold
   if ( ( ( val = analogRead(pin12V_)) < 300  ) && (cycleCountReal_ > 5)) {
      if ( ( val = analogRead(pin12V_)) < 300 ) {
         cycleStatus_ |= StatusAlarm12V;
         currStatus_  |= StatusAlarm12V;

         Serial.print("12V = ");
         Serial.println(val);
      }
   }

   // Off portion of the cycle
   if ( state_ == StateOff ) {

      // Turn on volume threshold exceeded, and we have met min off period, and running is enabled
      if ( (conf_.getRunState() == conf_.StateRunOn) &&
           (press_.scaledValue() < conf_.getVolInThold()) &&
           ((ctime - stateTime_) > MinOffMillis) ) {

         newState = StateOn;
         cycleStatus_ |= StatusVolInh;
         currStatus_  |= StatusVolInh;
      }

      // Turn on based upon time
      else if ( (ctime - stateTime_) > conf_.getOffTimeMillis()) newState = StateOn;
   }

   // On portion of the cycle
   else if ( state_ == StateOn ) {

      // Pressure triggers only valid when run is enabled
      if (conf_.getRunState() == conf_.StateRunOn) {

         // Turn off pressure threshold exceeded
         if ( press_.scaledValue() > conf_.getAdjPipMax() )  {
            newState = StateHold;
         }

         // Turn off volume threshold exceeded
         if ( vol_.scaledValue() > conf_.getAdjVolMax() ) {
            newState = StateHold;
         }
      }

      // On timer has been reached
      if ((ctime - stateTime_) > conf_.getOnTimeMillis()) newState = StateOff;
   }

   // Hold
   else {

      // On timer has been reached
      if ((ctime - stateTime_) > conf_.getOnTimeMillis()) newState = StateOff;
   }

   // State change
   if ( newState != state_ ) {

      // Start of a new cycle
      if ( newState == StateOn ) {
         vol_.reset(ctime);

         if (conf_.getRunState() == conf_.StateRunOn) {

            // Volume on previous cycle never exceeded 100mL
            if ( (currVmax_ < 100.0) && (cycleCountReal_ > 5)) cycleStatus_ |= StatusAlarmVolLow;

            // Pressure on previous cycle never exceeded 5cmH20
            if ( (currPmax_ < 5.0) && (cycleCountReal_ > 5)) cycleStatus_ |= StatusAlarmPressLow;

            // Update adjust volume max
            if ( wasOff_ ) conf_.initAdjVolMax();
            else conf_.updateAdjVolMax(currVmax_);
            wasOff_ = false;
         }

         else wasOff_ = true;

         // Clear counters
         prevVmax_ = currVmax_;
         currVmax_ = 0.0;
         prevPmax_ = currPmax_;
         currPmax_ = 0.0;
         prevPmin_ = currPmin_;
         currPmin_ = 99.9;

         // Update status to current cycle values to clear old alarms
         currStatus_  = cycleStatus_;
         cycleStatus_ = 0;

         // Increment cycle count
         cycleCountTotal_++;
         if (conf_.getRunState() == conf_.StateRunOn) cycleCountReal_++;
         else cycleCountReal_ = 0;

         // compute i/e ratio
         ieRatio_ = float(inhTime_) / float(ctime - stateTime_);

         stateTime_ = ctime;
      }

      // Going to off
      else if ( newState == StateOff ) {

          // Transitioning from on directly to off
          if ( state_ == StateOn ) inhTime_ = (ctime - stateTime_);

         stateTime_ = ctime;
      }

      // Going to hold, store inhalation time
      else if ( newState == StateHold ) {
          inhTime_ = (ctime - stateTime_);
      }

      state_ = newState;

      // Upper 8 bits of status contains current state
      currStatus_ &= 0x00FFFFFF;
      currStatus_ |= (state_ << 24);
   }

   // Currently forced off, paddle up
   if ( conf_.getRunState() == conf_.StateForceOff ) {
      digitalWrite(relayAPin_, RELAY_OFF);
      digitalWrite(relayBPin_, RELAY_ON);
   }

   // Currently orced on, paddle down
   else if ( conf_.getRunState() == conf_.StateForceOn ) {
      digitalWrite(relayAPin_, RELAY_ON);
      digitalWrite(relayBPin_, RELAY_ON);
   }

   // Currently off, paddle up
   else if ( conf_.getRunState() == conf_.StateRunOff ) {
      digitalWrite(relayAPin_, RELAY_OFF);
      digitalWrite(relayBPin_, RELAY_ON);
   }

   // On state, paddle down, opening valve1, closing valve 2
   else if ( state_ == StateOn ) {
      digitalWrite(relayAPin_, RELAY_ON);
      digitalWrite(relayBPin_, RELAY_OFF);
   }

   // Off state, paddle up, closing valve 1, opening valve 2
   else if ( state_ == StateOff ) {
      digitalWrite(relayAPin_, RELAY_OFF);
      digitalWrite(relayBPin_, RELAY_ON);
   }

   // Hold state, closing both valve 1 and valve 2
   else {
      digitalWrite(relayAPin_, RELAY_OFF);
      digitalWrite(relayBPin_, RELAY_OFF);
   }

   // Alarms LED
   if ( (currStatus_ & StatusAlarmPipMax   ) ||
        (currStatus_ & StatusAlarmVolLow   ) ||
        (currStatus_ & StatusAlarm12V      ) ||
        (currStatus_ & StatusAlarmPressLow ) ) {

      digitalWrite(redLedPin_, LED_ON);
   }
   else digitalWrite(redLedPin_, LED_OFF);

   // Warning LED
   if ( (currStatus_ & StatusWarn9V        ) ||
        (currStatus_ & StatusWarnPeepMin   ) ) {

      digitalWrite(yelLedPin_, LED_ON);
   }
   else digitalWrite(yelLedPin_, LED_OFF);

   // Alarm Audio
   if ( ( (currStatus_ & StatusAlarmPipMax   ) ||
          (currStatus_ & StatusAlarmVolLow   ) ||
          (currStatus_ & StatusAlarmPressLow ) ) && ((ctime - muteTime_) > 120000) ) {

      digitalWrite(piezoPin_, PIEZO_ON);
   }
   else digitalWrite(piezoPin_, PIEZO_OFF);

   // Calculate time since going to on state
   if (conf_.getRunState() == conf_.StateRunOn) {
       onTime_ = float(ctime - onStartTime_) / 1000.0;
   }
   else {
       onStartTime_ = ctime;
   }
}

void CycleControl::muteAlarm() {
   muteTime_ = millis();
}

