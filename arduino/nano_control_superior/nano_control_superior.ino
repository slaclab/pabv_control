
#include <Wire.h>
#include <AmbuConfigNano.h>
#include <CycleControl.h>
#include <SensorDlcL20D4.h>
#include <SensorSp110Sm02Flow.h>
#include <SensorVolume.h>
#include <stdint.h>
#include <HardwareSerial.h>

#define RELAYA_PIN 8
#define RELAYB_PIN 7
#define SENSOR_PERIOD_MILLIS 9
//#define SerialPort Serial
#define SerialPort Serial1

AmbuConfigNano      * conf  = new AmbuConfigNano(&SerialPort);
SensorDlcL20D4      * press = new SensorDlcL20D4(&SerialPort);
SensorSp110Sm02Flow * flow  = new SensorSp110Sm02Flow(&SerialPort);
SensorVolume        * vol   = new SensorVolume(flow,&SerialPort);
CycleControl        * relay = new CycleControl(conf,press,vol,RELAYA_PIN,RELAYB_PIN,&SerialPort);

uint32_t sensorTime;

void setup() {

   SerialPort.begin(57600);
   SerialPort.begin(57600);
   SerialPort.print("DEBUG Booted\n");

   Wire.begin();

   relay->setup();

   // Wait 5 seconds for pressure to settle
   SerialPort.print("DEBUG Wait 5 seconds\n");
   delay(5000);

   conf->setup();
   press->setup();
   flow->setup();
   vol->setup();

   sensorTime = millis();
   SerialPort.print("DEBUG setup done\n");
}

void loop() {
   uint32_t currTime;

   currTime = millis();

   if ((currTime - sensorTime) > SENSOR_PERIOD_MILLIS ) {

      press->update(currTime);
      flow->update(currTime);
      vol->update(currTime);
      // Generate serial output
      SerialPort.print("STATUS ");
      SerialPort.print(currTime);
      relay->sendString();
      press->sendString();
      flow->sendString();
      vol->sendString();
      SerialPort.print("\n");
      sensorTime = currTime;
   }

   relay->update(currTime);
   conf->update(currTime,relay);

}
