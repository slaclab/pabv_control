
#ifndef __AMBU_CONFIG_H__
#define __AMBU_CONFIG_H__

#define CONFIG_MILLIS 1000

class AmbuConfig {

   public:

      const unsigned int StateForceOff = 0;
      const unsigned int StateForceOn  = 1;
      const unsigned int StateRunOff   = 2;
      const unsigned int StateRunOn    = 3;

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

      char scanPeriod_[10];
      char scanOn_[10];
      char scanRun_[10];
      char scanStartThold_[10];
      char scanStopThold_[10];
      char scanVolThold_[10];

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
