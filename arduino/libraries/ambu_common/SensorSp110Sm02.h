
#ifndef __SENSOR_SP110SM02_H__
#define __SENSOR_SP110SM02_H__

#include "GenericSensor.h"

#define SP11_SM02_ADDR  0x28
#define SP11_SM02_INIT1 0b01001001
#define SP11_SM02_INIT2 0b01101001
#define SP11_SM02_DELAY 200

class SensorSp110Sm02 : public GenericSensor {

   public:

      SensorSp110Sm02 (Stream *serial);

      void setup();

      virtual void update(uint32_t ctime);

};

#endif
