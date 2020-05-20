
#ifndef __AMBU_CONFIG_UNO_H__
#define __AMBU_CONFIG_UNO_H__

#include <AmbuConfig.h>

class AmbuConfigUno : public AmbuConfig {

   public:

      AmbuConfigUno ();

      virtual void setup();

 private:
      virtual void storeConfig();

};

#endif
