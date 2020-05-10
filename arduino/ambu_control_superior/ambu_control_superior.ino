
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <AmbuConfig.h>
#include <RelayControl.h>
#include <SensorDlcL20D4.h>
#include <SensorSp110Sm02.h>

//#define RELAY_PIN 2
#define SENSOR_PERIOD_MILLIS 9
#define pSoftOnSwitch 2
#define pAlarmRelay 3
#define pSolenoidRelay 4
#define pLEDRed 5
#define pLEDYellow 6
#define pEncoderSW 8
#define pEncoderR 9
#define pEncoderL 10
#define pAnalog12V A0
#define pAnalog9V A1
#define pAnalog3V A2

int currentParameter = 0;
int encoderPushes = 0;
int encoderVal = 0;
bool stateA, stateB;
long encoderPushTime = 0; //millis
long encoderTurnTime = 0; //millis
const long encoderTurnDeadTime = 50; //millis
const long encoderPushDeadTime = 500; //millis
// Ventilator Parameters
// default, change_by, min, max
float pRR[] = {20.0, 0.5, 10.0, 35.0}; // breaths/min
float pIT[] = {1.0, 0.1, 0.5, 2};      // relay open time 
float pTH[] = {-5.0, 0.1, -10.0, 0} ;  // cmH2O

AmbuConfig      * conf  = new AmbuConfig();
SensorDlcL20D4  * press = new SensorDlcL20D4();
SensorSp110Sm02 * flow  = new SensorSp110Sm02();
RelayControl    * relay = new RelayControl(conf, flow, pSolenoidRelay);

Adafruit_SSD1306 display = (128, 64, &Wire, -1);

unsigned int sensorTime;
unsigned int currTime;

void setup() {

   Serial.begin(57600);
   Wire.begin();
   
   // Setup the LED pins for warnings
   pinMode(pLEDRed, OUTPUT); //bring high to turn on
   pinMode(pLEDYellow, OUTPUT); //bring high to turn on
   digitalWrite(pLEDRed, false);
   digitalWrite(pLEDYellow, false);
   // Setup soft switch that pulls the pin down for ON
   pinMode(pSoftOnSwitch, INPUT_PULLUP);
   // Setup Encoder 
   pinMode(pEncoderR, INPUT_PULLUP);
   pinMode(pEncoderL, INPUT_PULLUP);
   pinMode(pEncoderSW, INPUT_PULLUP);   
   // Setup Piezo Alarm // This is not working for some reason
   //pinMode(pAlarmRelay, OUTPUT); // bring low to turn on 
   //digitalWrite(pAlarmRelay, false);
   
   conf->setup();
   flow->setup();
   relay->setup();

   if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
      Serial.println(F("SSD1360 allocation failed"));
      //workingDisplay = false  //TODO prevent hangup in this case
   }
   update_display();
   sensorTime = millis();
}

void loop() {

   currTime = millis();

   if ((currTime - sensorTime) > SENSOR_PERIOD_MILLIS ) {

      press->update(currTime);
      flow->update(currTime);

      // Generate serial output
      Serial.write("STATUS");
      relay->sendString();
      conf->sendString();
      press->sendString();
      flow->sendString();
      Serial.write("\n");

      sensorTime = currTime;
   }

   relay->update(currTime);
   conf->update(currTime);

   if ( digitalRead(pEncoderSW) == false ) {
      if (currTime - encoderPushTime > encoderPushDeadTime) {
         encoderPushTime = currTime;
         encoderPushes++;
         currentParameter = encoderPushes % 3;
         update_display();
      }
   }
   stateA = digitalRead(pEncoderL);
   stateB = digitalRead(pEncoderR);
   if (stateA == false) {
      if (currTime - encoderTurnTime > encoderTurnDeadTime) {
         encoderTurnTime = currTime;
         if (stateB != stateA) {
               encoderVal++;
               update_parameter(1);
         }
         else {
               encoderVal--;
               update_parameter(-1);
         }
      }
   }
}

void update_display() {
   Serial.write("Update Display");
   display.clearDisplay();
   display.setTextColor(WHITE);
   // First Line
   display.setTextSize(3);
   display.setCursor(1,1);
   if (currentParameter == 0) {
      display.print("RR:");
      display.print(pRR[0], 1);
   }
   else if (currentParameter == 1) {
      display.print("IT:");
      display.print(pIT[0], 1);
   }
   else {
      display.print("TH:");
      display.print(pTH[0], 1);
   }
   // LINE 2
   display.setTextSize(2);
   display.setCursor(1,26);
   if (currentParameter == 0) {
      display.print("IT:");
      display.print(pIT[0], 1);
   }
   else if (currentParameter == 1) {
      display.print("TH:");
      display.print(pTH[0], 1);
   }
   else {
      display.print("RR:");
      display.print(pRR[0], 1);
   }
   // LINE 3  
   display.setTextSize(2);
   display.setCursor(1, 44);
   if (currentParameter == 0) {
      display.print("TH:");
      display.print(pTH[0], 1);
   }
   else if (currentParameter == 1) {
      display.print("RR:");
      display.print(pRR[0], 1);
   }
   else {
      display.print("IT:");
      display.print(pIT[0], 1);
   }  
   display.display();
}

void update_parameter(int i){
   switch (currentParameter) {
      case 0:
         // RR
         pRR[0] = pRR[0] + pRR[1]*i;
         if (pRR[0] < pRR[2]) {
            pRR[0] = pRR[2];
         }
         else if (pRR[0] > pRR[3]) {
            pRR[0] = pRR[3];
         }
      case 1:
         // IT
         pIT[0] = pIT[0] + pIT[1]*i;
         if (pIT[0] < pIT[2]) {
            pIT[0] = pIT[2];
         }
         else if (pIT[0] > pIT[3]) {
            pIT[0] = pIT[3];
         }
      case 2:
         // TH
         pTH[0] = pTH[0] + pTH[1]*i;
         if (pTH[0] < pTH[2]) {
            pTH[0] = pTH[2];
         }
         else if (pTH[0] > pTH[3]) {
            pTH[0] = pTH[3];
         }
   }
   update_display();
}