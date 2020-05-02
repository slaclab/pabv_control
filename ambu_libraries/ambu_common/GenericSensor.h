
#ifndef __GENERIC_SENSOR_H__
#define __GENERIC_SENSOR_H__

class GenericSensor {

   protected:

      unsigned char addr_;
      unsigned char data_[10];
      unsigned int scaled_;
      unsigned int x_;

      char buffer_[20];

   public:

      GenericSensor (unsigned char addr);

      virtual void setup();

      virtual void update(unsigned int ctime);

      void sendString();

      unsigned int scaledValue();
};

#endif
