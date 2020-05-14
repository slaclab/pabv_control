
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

void SensorDlcL20D4::update(unsigned int ctime) {

   // Read last cycles values for dlc
   Wire.requestFrom(addr_, byte(4));
   for (x_=0; x_ < 4; x_++) data_[x_] = Wire.read();

   //Serial.print("Debug: Status :");
   //Serial.print(data_[0],HEX);
   //Serial.print("\n");

   if ( data_[0] == 0x40 ) {

       raw_ = ((double)data_[1] * 65536.0) + ((double)data_[2] * 256.0) + (double)data_[3];

       // Scaled value
       scaled_ = 1.25 * ((raw_ - 8388608.0) / 8388608.0) * 20.0 * 2.54;
   }

   // Start new cycle for dlc, if not busy
   if ( (data_[0] & 0x20) == 0 ) {
       Wire.beginTransmission(addr_);
       Wire.write(DLC_L20D4_CMD); // 2 cycle average = 8ms
       Wire.endTransmission();
   }
}

