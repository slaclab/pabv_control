
#include <Wire.h>
#include <SensorHaf50Slpm.h>
#include <SensorSp110Sm02.h>

#define SENSOR_PERIOD_MILLIS 9
#define SerialPort Serial
//#define SerialPort Serial1

SensorHaf50Slpm * ref  = new SensorHaf50Slpm(&SerialPort);
SensorSp110Sm02 * flow = new SensorSp110Sm02(&SerialPort);

uint32_t sensorTime;
uint32_t cycleCount;

void setup() {

   SerialPort.begin(57600);
   SerialPort.print("DEBUG Booted\n");

   Wire.begin();

   // Wait 5 seconds for pressure to settle
   SerialPort.print("DEBUG Wait 5 seconds\n");
   delay(5000);

   ref->setup();
   flow->setup();

   sensorTime = millis();
   SerialPort.print("DEBUG setup done\n");

   cycleCount = 0;
}

void loop() {

   uint32_t currTime;

   currTime = millis();

   if ((currTime - sensorTime) > SENSOR_PERIOD_MILLIS ) {

      ref->update(currTime);
      flow->update(currTime);

      // Generate serial output
      SerialPort.print("STATUS ");
      SerialPort.print(currTime);
      SerialPort.print(cycleCount / 100);
      SerialPort.print(" 0 0 0"); // alarm, volmax, pip max
      ref->sendString();
      flow->sendString();
      SerialPort.print(" 0\n"); // volume
      sensorTime = currTime;
      cycleCount += 1;
   }
}

