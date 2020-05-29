
#ifndef __SENSOR_DCLL20DR_H_
#define __SENSOR_DCLL20DR_H_

#include "GenericSensor.h"

#define DLC_L20D4_ADDR 0x29
#define DLC_L20D4_CMD  0xAC

class SensorDlcL20D4 : public GenericSensor {

   public:

      SensorDlcL20D4 (HardwareSerial *serial);

      void setup();

      void update(uint32_t ctime);

};

#endif
