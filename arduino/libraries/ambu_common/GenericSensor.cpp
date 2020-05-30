#include "GenericSensor.h"
#include <Arduino.h>
GenericSensor::GenericSensor (uint8_t addr, Stream *serial) {
   addr_   = addr;
   scaled_ = 0.0;
   serial_ = serial;
}

void GenericSensor::setup() { }

void GenericSensor::update(uint32_t ctime) { }

void GenericSensor::reset(uint32_t ctime) {
   scaled_ = 0.0;
}

void GenericSensor::sendString() {
  serial_->print(" ");
  serial_->print(scaled_,6);
}

double GenericSensor::scaledValue() {
   return scaled_;
}

