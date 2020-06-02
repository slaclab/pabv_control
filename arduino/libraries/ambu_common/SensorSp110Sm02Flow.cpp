
#include "SensorSp110Sm02Flow.h"
#include <Wire.h>

#include <Arduino.h>

#include <HardwareSerial.h>

SensorSp110Sm02Flow::SensorSp110Sm02Flow() : SensorSp110Sm02() {}

void SensorSp110Sm02Flow::update(uint32_t ctime) {
   SensorSp110Sm02::update(ctime);

   double sign;

   if ( scaled_ < 0 ) sign = -1.0;
   else sign = 1.0;

   scaled_ = sign * FLOW_BETA * sqrt(abs(scaled_));

}

