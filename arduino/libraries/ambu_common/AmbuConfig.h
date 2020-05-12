
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
      char txBuffer_[100];
      char mark_[10];

      unsigned int rxCount_;
      unsigned int addr_;

      int ret_;
      char c_;

      unsigned int confTime_;
      unsigned int period_;
      unsigned int onTime_;
      unsigned int startThold_;
      unsigned int stopThold_;
      unsigned int runState_;

      unsigned int scanPeriod_;
      unsigned int scanOn_;
      unsigned int scanStartThold_;
      unsigned int scanStopThold_;
      unsigned int scanRun_;

      void storeConfig();

   public:

      AmbuConfig ();

      virtual void setup();

      virtual void update(unsigned int ctime);

      unsigned int getPeriod();

      unsigned int getOnTime();

      unsigned int getStartThold();

      unsigned int getStopThold();

      unsigned int getRunState();

};

#endif
