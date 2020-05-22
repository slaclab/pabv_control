
#ifndef __RELAY_CONTROL_H__
#define __RELAY_CONTROL_H__

#include <Arduino.h>

#define RELAY_ON  HIGH
#define RELAY_OFF LOW
#define MIN_OFF_MILLIS 10000

class AmbuConfig;
class GenericSensor;

class CycleControl {

      // States
      static const uint8_t StateOff = 0;
      static const uint8_t StateOn  = 1;

      // Alarm Bits
      static const uint8_t AlarmPipMax  = 1;
      static const uint8_t AlarmVolMax  = 2;
      static const uint8_t Alarm12V     = 4;
      static const uint8_t Alarm9V      = 8;

      AmbuConfig * conf_;
      GenericSensor * press_;
      GenericSensor * vol_;

      uint8_t  state_;
      uint32_t stateTime_;
      uint8_t  relayPin_;
      uint8_t  alarmState_;
      uint32_t cycleCount_;

   public:

      CycleControl (AmbuConfig *conf,
                    GenericSensor *press,
                    GenericSensor *vol,
                    uint8_t relayPin);

      void setup();

      void update(uint32_t ctime);

      void sendString();

      void clearAlarm();
};

#endif
