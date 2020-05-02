
#ifndef __SENSOR_NPA700B02WD_H__
#define __SENSOR_NPA700B02WD_H__

#include "GenericSensor.h"

#define NPA_700B_ADDR  0x28

class SensorNpa700B02WD : public GenericSensor {

   public:

      SensorNpa700B02WD ();

      void update(unsigned int ctime);

};

#endif
