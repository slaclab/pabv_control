#ifndef __GENERIC_SENSOR_H__
#define __GENERIC_SENSOR_H__

#include <stdint.h>
#include <Arduino.h>
#include <HardwareSerial.h>

class GenericSensor {

   protected:

      uint8_t addr_;
      double  scaled_;


   public:

      GenericSensor (uint8_t addr);

      virtual void setup();

      virtual void update(uint32_t ctime);

      virtual void reset(uint32_t ctime);

      double scaledValue();
};

#endif
