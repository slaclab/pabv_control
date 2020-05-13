
#include "SensorVolume.h"
#include <Wire.h>

#include <Arduino.h>

#include <HardwareSerial.h>

SensorVolume::SensorVolume(GenericSensor *flow) : GenericSensor(0) {
   flow_ = flow;
}

void SensorVolume::reset(unsigned int ctime) {
    GenericSensor::reset(ctime);
    lTime_ = ctime;
}

void SensorVolume::update(unsigned int ctime) {
   durr_ = ctime - lTime_;
   lTime_ = ctime;

   scaled_ += flow_->scaledValue() * ((double)durr_ / 60.0);
}

