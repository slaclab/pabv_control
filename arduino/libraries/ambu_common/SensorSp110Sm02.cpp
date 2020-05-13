
#include "SensorSp110Sm02.h"
#include <Wire.h>

#include <Arduino.h>

#include <HardwareSerial.h>

SensorSp110Sm02::SensorSp110Sm02 () : GenericSensor(SP11_SM02_ADDR) { }

void SensorSp110Sm02::setup() {

   // setup for the Superior SP110 sensor
   Wire.beginTransmission(addr_);

   // Sets 2cm with single point 0 offset correction
   // 250 Hz, so it should always be faster than the 9ms AnalogMillis
   Wire.write(SP11_SM02_INIT1);
   Wire.endTransmission();
   delay(SP11_SM02_DELAY);
   Wire.beginTransmission(addr_);
   Wire.write(SP11_SM02_INIT2);
   Wire.endTransmission(); // Auto-Zero is done
   delay(SP11_SM02_DELAY);
}

void SensorSp110Sm02::update(unsigned int ctime) {

   // Read value
   Wire.requestFrom(addr_, byte(2));
   for (x_=0; x_ < 2; x_++) data_[x_] = Wire.read();

   raw_ = (double)((data_[0] << 8) | data_[1]);

   // Negative
   if ( data_[0] & 0x80 ) raw_ -= 65536.0;

   scaled_ = raw_ * (2.0 / (0.9 * 32768.0));
}

