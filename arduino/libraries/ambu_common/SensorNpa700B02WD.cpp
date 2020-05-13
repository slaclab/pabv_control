
#include "SensorNpa700B02WD.h"
#include <Wire.h>

#include <Arduino.h>

#include <HardwareSerial.h>

SensorNpa700B02WD::SensorNpa700B02WD () : GenericSensor(NPA_700B_ADDR) { }


void SensorNpa700B02WD::update(unsigned int ctime) {

   // Read value
   Wire.requestFrom(addr_, byte(2));
   for (x_=0; x_ < 2; x_++) data_[x_] = Wire.read();

   // Trim bits
   data_[0] &= 0x3F;

   raw_ = (double)((data_[0] << 8) | data_[1]);

   // Scaled value:w
   scaled_ = (raw_-8192.0) * (2.0 / 8191.0);
}


