
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
      static const uint16_t StatusAlarmPipMax    = 0x0001;
      static const uint16_t StatusAlarmVolLow    = 0x0002;
      static const uint16_t StatusAlarm12V       = 0x0004;
      static const uint16_t StatusWarn9V         = 0x0008;
      static const uint16_t StatusVolInh         = 0x0010;
      static const uint16_t StatusAlarmPressLow  = 0x0020;
      static const uint16_t StatusWarnPeepMin    = 0x0040;
      static const uint16_t StatusWarnVolLow     = 0x0080;
      static const uint16_t StatusWarnVolMax     = 0x0100;

      static const uint16_t StatusAlarmWarnMask  = 0x1EF;

      // Min off period
      static const uint32_t MinOffMillis = 1000;

      AmbuConfig &conf_;
      GenericSensor &press_;
      GenericSensor &vol_;

      uint8_t  state_;
      uint32_t stateTime_;
      uint32_t muteTime_;
      uint32_t onStartTime_;
      uint32_t onTime_;
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
      double   ieRatio_;
      uint32_t inhTime_;
      uint32_t exhTime_;

      bool     wasOff_;

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
      uint32_t onTime()     { return onTime_;}
      float    ieRatio()    { return ieRatio_;}
      float prevPmax() {return prevPmax_;}
      float prevVmax() {return prevVmax_;}
      float prevPmin() {return prevPmin_;}
};

#endif
