
#ifndef __AMBU_CONFIG_NANO_H__
#define __AMBU_CONFIG_NANO_H__

#include <AmbuConfig.h>
#include "Comm.h"

class AmbuConfigNano : public AmbuConfig {

   public:

   AmbuConfigNano (Comm &serial);

      virtual void setup();
      virtual void deviceID(cpuId &id);
 private:
      virtual void storeConfig();

};

#endif
