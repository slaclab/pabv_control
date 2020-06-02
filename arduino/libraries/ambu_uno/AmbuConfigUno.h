
#ifndef __AMBU_CONFIG_UNO_H__
#define __AMBU_CONFIG_UNO_H__

#include <AmbuConfig.h>

class AmbuConfigUno : public AmbuConfig {

   public:

      AmbuConfigUno (Comm &serial);
      void AmbuConfigUno::deviceID(uint32_t &id[4])
      virtual void setup();

 private:
      virtual void storeConfig();

};

#endif
