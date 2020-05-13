
#include "SensorSp110Sm02Flow.h"
#include <Wire.h>

#include <Arduino.h>

#include <HardwareSerial.h>

SensorSp110Sm02Flow::SensorSp110Sm02Flow() : SensorSp110Sm02() {}

void SensorSp110Sm02Flow::update(unsigned int ctime) {
   SensorSp110Sm02::update(ctime);

   if ( scaled_ < 0 ) {
       sign_ = -1.0;
       scaled_ *= -1.0;
   }
   else sign_ = 1.0;

   scaled_ = sign_ * FLOW_BETA * sqrt(scaled_);
}

