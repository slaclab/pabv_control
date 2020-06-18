
#ifndef __RELAY_CONTROL_H__
#define __RELAY_CONTROL_H__

#include <Arduino.h>
#include <HardwareSerial.h>

#define RELAY_ON  HIGH
#define RELAY_OFF LOW

#define LED_ON  HIGH
#define LED_OFF LOW

#define PIEZO_ON  HIGH
#define PIEZO_OFF LOW

class AmbuConfig;
class GenericSensor;

class CycleControl {

      // States
      static const uint8_t StateOff  = 0;
      static const uint8_t StateOn   = 1;
      static const uint8_t StateHold = 2;

      // Statis Bits
      static const uint8_t StatusAlarmPipMax    = 0x01;
      static const uint8_t StatusAlarmVolLow    = 0x02;
      static const uint8_t StatusAlarm12V       = 0x04;
      static const uint8_t StatusWarn9V         = 0x08;
      static const uint8_t StatusVolInh         = 0x10;
      static const uint8_t StatusAlarmPressLow  = 0x20;
      static const uint8_t StatusWarnPeepMin    = 0x40;

      // Min off period
      static const uint32_t MinOffMillis = 1000;

      AmbuConfig &conf_;
      GenericSensor &press_;
      GenericSensor &vol_;

      uint8_t  state_;
      uint32_t stateTime_;
      uint32_t muteTime_;
      uint8_t  relayAPin_;
      uint8_t  relayBPin_;
      uint8_t  redLedPin_;
      uint8_t  yelLedPin_;
      uint8_t  piezoPin_;
      uint8_t  pin12V_;
      uint8_t  pin9V_;
      uint32_t cycleStatus_;
      uint32_t currStatus_;
      uint32_t cycleCountTotal_;
      uint32_t cycleCountReal_;
      double   currVmax_;
      double   prevVmax_;
      double   currPmax_;
      double   prevPmax_;
      double   currPmin_;
      double   prevPmin_;

   public:

      CycleControl (AmbuConfig &conf,
                    GenericSensor &press,
                    GenericSensor &vol,
                    uint8_t relayAPin,
                    uint8_t relayBPin,
                    uint8_t redLedPin,
                    uint8_t yelLedPin,
                    uint8_t piezoPin,
                    uint8_t pin12V,
                    uint8_t pin9V);

      void setup();

      void update(uint32_t ctime);


      void muteAlarm();

      uint32_t status() { return currStatus_;}
      uint32_t cycleCount() { return cycleCountTotal_;}
      float prevPmax() {return prevPmax_;}
      float prevVmax() {return prevVmax_;}
      float prevPmin() {return prevPmin_;}
};

#endif
