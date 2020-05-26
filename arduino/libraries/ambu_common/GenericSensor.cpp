#include "GenericSensor.h"
#include <Arduino.h>
GenericSensor::GenericSensor (uint8_t addr) {
   addr_   = addr;
   scaled_ = 0.0;
}

void GenericSensor::setup() { }

void GenericSensor::update(uint32_t ctime) { }

void GenericSensor::reset(uint32_t ctime) {
   scaled_ = 0.0;
}

void GenericSensor::sendString() {
  Serial.print(" ");
  Serial.print(scaled_,6);
}

double GenericSensor::scaledValue() {
   return scaled_;
}

