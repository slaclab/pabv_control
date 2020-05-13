
#ifndef __SENSOR_VOLUME_H__
#define __SENSOR_VOLUME_H__

#include "GenericSensor.h"

class SensorVolume : public GenericSensor {

      GenericSensor *flow_;
      unsigned int lTime_;
      unsigned int durr_;

   public:

      SensorVolume (GenericSensor *flow);

      void reset(unsigned int ctime);
      void update(unsigned int ctime);

};

#endif
