
#include "GenericSensor.h"

#include <HardwareSerial.h>

GenericSensor::GenericSensor (unsigned char addr) {
   addr_ = addr;
   memset(data_,0,10);
   raw_ = 0.0;
   scaled_ = 0.0;
}

void GenericSensor::setup() { }

void GenericSensor::update(unsigned int ctime) { }

void GenericSensor::reset(unsigned int ctime) {
   raw_ = 0.0;
   scaled_ = 0.0;
}

void GenericSensor::sendString() {
   Serial.print(" ");
   Serial.print(scaled_,6);
}

double GenericSensor::scaledValue() {
   return scaled_;
}

