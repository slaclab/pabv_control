
#ifndef __AMBU_CONFIG_H__
#define __AMBU_CONFIG_H__

#define CONFIG_MILLIS 1000

class AmbuConfig {

   public:

      const unsigned int StateOff   = 0;
      const unsigned int StateOn    = 1;
      const unsigned int StateCycle = 2;

   private:

      char rxBuffer_[100];
      char mark_[10];

      unsigned int rxCount_;
      unsigned int addr_;

      int ret_;
      char c_;

      unsigned int confTime_;
      unsigned int period_;
      unsigned int onTime_;
      unsigned int runState_;

      double startThold_;
      double stopThold_;
      double volThold_;

      unsigned int scanPeriod_;
      unsigned int scanOn_;
      unsigned int scanRun_;

      double scanStartThold_;
      double scanStopThold_;
      double scanVolThold_;

      void storeConfig();

   public:

      AmbuConfig ();

      virtual void setup();

      virtual void update(unsigned int ctime);

      unsigned int getPeriod();

      unsigned int getOnTime();

      double getStartThold();

      double getStopThold();

      double getVolThold();

      unsigned int getRunState();

};

#endif
