#ifndef __GENERIC_SENSOR_H__
#define __GENERIC_SENSOR_H__

#include <stdint.h>

class GenericSensor {

   protected:

      uint8_t addr_;
      double  scaled_;

      HardwareSerial *serial_;

   public:

      GenericSensor (uint8_t addr, HardwareSerial *serial);

      virtual void setup();

      virtual void update(uint32_t ctime);

      virtual void reset(uint32_t ctime);

      void sendString();

      double scaledValue();
};

#endif
