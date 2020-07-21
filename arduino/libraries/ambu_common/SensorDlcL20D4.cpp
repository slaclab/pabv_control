
#include "SensorDlcL20D4.h"
#include <Wire.h>
#include <Arduino.h>

#include <HardwareSerial.h>

SensorDlcL20D4::SensorDlcL20D4 () : GenericSensor(DLC_L20D4_ADDR) { }

void SensorDlcL20D4::setup() {

   // Start new cycle for dlc
   Wire.beginTransmission(addr_);
   Wire.write(DLC_L20D4_CMD); // 2 cycle average = 8ms
   Wire.endTransmission();
}

void SensorDlcL20D4::update(uint32_t ctime) {
   uint8_t data[4];
   uint16_t x;
   double raw;

   // Read last cycles values for dlc
   Wire.requestFrom(addr_, byte(4));
   for (x=0; x < 4; x++) data[x] = Wire.read();

   if ( data[0] == 0x40 ) {

       raw = ((double)data[1] * 65536.0) + ((double)data[2] * 256.0) + (double)data[3];

       // Scaled value
       scaled_ = 1.25 * ((raw - 8388608.0) / 8388608.0) * 20.0 * 2.54;
   }

   // Start new cycle for dlc, if not busy
   if ( (data[0] & 0x20) == 0 ) {
       Wire.beginTransmission(addr_);
       Wire.write(DLC_L20D4_CMD); // 2 cycle average = 8ms
       Wire.endTransmission();
   }
}

