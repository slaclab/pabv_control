
#include "SensorHaf50Slpm.h"
#include <Wire.h>

#include <Arduino.h>

#include <HardwareSerial.h>

SensorHaf50Slpm::SensorHaf50Slpm (Stream *serial) : GenericSensor(HAF_50SLPM_ADDR,serial) { }


void SensorHaf50Slpm::update(uint32_t ctime) {
   uint8_t data[2];
   uint16_t x;
   double raw;

   // Read value
   Wire.requestFrom(addr_, byte(2));
   for (x=0; x < 2; x++) data[x] = Wire.read();

   raw = (double)((data[0] << 8) | data[1]);

   // Scaled value
   scaled_ = 50.0 * (((raw / 16384.0) - 0.1) / 0.8);
}


