
#ifndef __SENSOR_VOLUME_H__
#define __SENSOR_VOLUME_H__

#include "GenericSensor.h"

class SensorVolume : public GenericSensor {

      GenericSensor *flow_;
      uint32_t lTime_;

   public:

      SensorVolume (GenericSensor *flow, Stream *serial);

      void reset(uint32_t ctime);
      void update(uint32_t ctime);

};

#endif
