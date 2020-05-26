
#ifndef __SENSOR_SP110SM02_FLOW_H__
#define __SENSOR_SP110SM02_FLOW_H__

#include "SensorSp110Sm02.h"

#define FLOW_BETA 51.7

class SensorSp110Sm02Flow : public SensorSp110Sm02 {

   public:

      SensorSp110Sm02Flow ();

      void update(uint32_t ctime);

};

#endif
