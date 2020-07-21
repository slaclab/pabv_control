
#include "SensorHaf50Slpm.h"
#include <Wire.h>

#include <Arduino.h>

#include <HardwareSerial.h>

SensorHaf50Slpm::SensorHaf50Slpm () : GenericSensor(HAF_50SLPM_ADDR) { }


void SensorHaf50Slpm::update(uint32_t ctime) {
   uint8_t data[2];
   uint16_t x;
   uint16_t raw;
   // Read value
   Wire.requestFrom(addr_, byte(2));
   for (x=0; x < 2; x++) data[x] = Wire.read();

   raw = (data[0] << 8) | data[1];

   // Scaled value
   scaled_ = 50.0 * (((double(raw) / 16384.0) - 0.1) / 0.8);
}


