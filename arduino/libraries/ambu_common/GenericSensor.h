
#ifndef __GENERIC_SENSOR_H__
#define __GENERIC_SENSOR_H__

class GenericSensor {

   protected:

      unsigned char addr_;
      unsigned char data_[10];
      unsigned int x_;
      double raw_;
      double scaled_;

   public:

      GenericSensor (unsigned char addr);

      virtual void setup();

      virtual void update(unsigned int ctime);

      virtual void reset(unsigned int ctime);

      void sendString();

      double scaledValue();
};

#endif
