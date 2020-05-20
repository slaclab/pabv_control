
#ifndef __SENSOR_VOLUME_H__
#define __SENSOR_VOLUME_H__

#include "GenericSensor.h"

class SensorVolume : public GenericSensor {

      GenericSensor *flow_;
      unsigned int lTime_;

   public:

      SensorVolume (GenericSensor *flow);

      void reset(uint32_t ctime);
      void update(uint32_t ctime);

};

#endif
