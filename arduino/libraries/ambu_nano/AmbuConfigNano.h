
#ifndef __AMBU_CONFIG_NANO_H__
#define __AMBU_CONFIG_NANO_H__

#include <AmbuConfig.h>

class AmbuConfigNano : public AmbuConfig {

   public:

      AmbuConfigNano (Stream *serial);

      virtual void setup();

 private:
      virtual void storeConfig();

};

#endif
