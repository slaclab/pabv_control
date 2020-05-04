
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

   // Scaled value
   scaled_ = (data_[0] << 8) | data_[1];

   // Create serial representation
   sprintf(buffer_," 0x%.2x%.2x", data_[0], data_[1]);

}


