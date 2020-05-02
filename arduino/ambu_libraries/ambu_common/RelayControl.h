
#ifndef __RELAY_CONTROL_H__
#define __RELAY_CONTROL_H__

#include <Arduino.h>

#define RELAY_ON  HIGH
#define RELAY_OFF LOW

class AmbuConfig;
class GenericSensor;

class RelayControl {

      AmbuConfig * conf_;
      GenericSensor * flow_;

      unsigned int relayTime_;
      unsigned int relayPin_;
      unsigned int cycleCount_;
      unsigned int inhalation_;
      unsigned int autoStart_;

      char txBuffer_[20];

   public:

      RelayControl (AmbuConfig *conf, GenericSensor *flow, unsigned int relayPin);

      void setup();

      void update(unsigned int ctime);

      void sendString();
};

#endif
