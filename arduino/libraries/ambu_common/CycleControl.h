
#ifndef __RELAY_CONTROL_H__
#define __RELAY_CONTROL_H__

#include <Arduino.h>
#include <HardwareSerial.h>

#define RELAY_ON  HIGH
#define RELAY_OFF LOW

class AmbuConfig;
class GenericSensor;

class CycleControl {

      // States
      static const uint8_t StateOff  = 0;
      static const uint8_t StateOn   = 1;
      static const uint8_t StateHold = 2;

      // Statis Bits
      static const uint8_t StatusAlarmPipMax  = 0x01;
      static const uint8_t StatusAlarmVolMax  = 0x02;
      static const uint8_t StatusAlarm12V     = 0x04;
      static const uint8_t StatusAlarm9V      = 0x08;
      static const uint8_t StatusVolInh       = 0x10;

      // Min off period
      static const uint32_t MinOffMillis = 1000;

      AmbuConfig &conf_;
      GenericSensor &press_;
      GenericSensor &vol_;

      uint8_t  state_;
      uint32_t stateTime_;
      uint8_t  relayAPin_;
      uint8_t  relayBPin_;
      uint8_t  status_;
      uint32_t cycleCount_;
      double   currVmax_;
      double   prevVmax_;
      double   currPmax_;
      double   prevPmax_;

   public:

      CycleControl (AmbuConfig &conf,
                    GenericSensor &press,
                    GenericSensor &vol,
                    uint8_t relayAPin,
		    uint8_t relayBPin);

      void setup();

      void update(uint32_t ctime);


      void clearAlarm();

      uint32_t status() { return status_;}
      uint32_t cycleCount() { return cycleCount_;}
      float prevPmax() {return prevPmax_;}
      float prevVmax() {return prevVmax_;}
};

#endif
