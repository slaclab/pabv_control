
#include "GenericSensor.h"

#include <HardwareSerial.h>

GenericSensor::GenericSensor (unsigned char addr) {
   addr_ = addr;
   memset(buffer_,0,20);
   memset(data_,0,10);
}

void GenericSensor::setup() { }

void GenericSensor::update(unsigned int ctime) { }

void GenericSensor::sendString() {
   Serial.write(buffer_);
}

unsigned int GenericSensor::scaledValue() {
   return scaled_;
}

