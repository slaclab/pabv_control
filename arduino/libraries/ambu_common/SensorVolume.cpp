
#include "SensorVolume.h"
#include <Wire.h>

#include <Arduino.h>

#include <HardwareSerial.h>

SensorVolume::SensorVolume(GenericSensor *flow, HardwareSerial *serial) : GenericSensor(0,serial) {
   flow_ = flow;
}

void SensorVolume::reset(uint32_t ctime) {
    GenericSensor::reset(ctime);
    lTime_ = ctime;
}

void SensorVolume::update(uint32_t ctime) {
   uint32_t durr;

   durr = ctime - lTime_;
   lTime_ = ctime;

   scaled_ += flow_->scaledValue() * ((double)durr / 60.0);
}

