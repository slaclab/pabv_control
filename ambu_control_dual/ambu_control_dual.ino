
#include <Wire.h>

const unsigned int RelayCount     = 2;
const unsigned int RelayPins[2]   = {2,3};
const unsigned int AnalogCount    = 4;
const unsigned int AnalogPins[4]  = {2,3,4,5};
const unsigned int AnalogMillis   = 10;
const unsigned int DefRelayPeriod = 3000;
const unsigned int DefRelayOn     = 1000;
const unsigned int DefStartThold  = 7000;

const byte I2cAddrDlc = 41;
const byte I2cAddrNpa = 40;
const byte I2cCmdDlc  = 0xAC; // 2 cycle average = 8ms

unsigned int startThold;
unsigned int relayPeriod;
unsigned int relayOn;
unsigned int x;
unsigned int analogTime;
unsigned int currTime;
unsigned int relayTime;
unsigned int rxCount;
unsigned int scanPeriod;
unsigned int scanOn;
unsigned int scanThold;
unsigned int cycleCount;
unsigned int autoStart;
byte i2cRaw[4];
byte i2cLow;
byte i2cHigh;

char txBuffer[100];
char rxBuffer[50];
char mark[20];
char c;
int  ret;

void setup() {

   pinMode(RelayPins[0], OUTPUT);
   pinMode(RelayPins[1], OUTPUT);
   digitalWrite(RelayPins[0], LOW);
   digitalWrite(RelayPins[1], HIGH);

   analogTime = millis();
   relayTime  = millis();
   cycleCount = 0;
   rxCount    = 0;

   relayPeriod = DefRelayPeriod;
   relayOn     = DefRelayOn;
   startThold  = DefStartThold;

   Serial.begin(9600);
   Wire.begin();
}

void (* resetFunc)(void) = 0;

void loop() {
   autoStart = 0;

   currTime = millis();

   if ((currTime - analogTime) > AnalogMillis ) {

      // Read last cycles values for dlc
      Wire.requestFrom(I2cAddrDlc, byte(4));
      for (x=0; x < 4; x++) i2cRaw[x] = Wire.read();

      if ( (unsigned int)((i2cRaw[1] << 8) | i2cRaw[2]) < startThold ) autoStart = 1;

      // Start new cycle for dlc
      Wire.beginTransmission(I2cAddrDlc);
      Wire.write(I2cCmdDlc);
      Wire.endTransmission();

      // Read npa sensor
      Wire.requestFrom(I2cAddrNpa, byte(2));
      i2cHigh = Wire.read() & 0x3F;
      i2cLow = Wire.read();

      sprintf(txBuffer,"ANALOG %i 0x%.2x%.2x%.2x 0x%.2x%.2x\n", cycleCount,
                       i2cRaw[1], i2cRaw[2], i2cRaw[3],
                       i2cHigh, i2cLow);

      Serial.write(txBuffer);
      analogTime = currTime;
   }

   // Turn off time
   if ((currTime - relayTime) > relayOn ) {
      digitalWrite(RelayPins[0], HIGH);
      digitalWrite(RelayPins[1], LOW);
   }

   // End of cycle, turn on
   if ( autoStart || ((currTime - relayTime) > relayPeriod )) {
      digitalWrite(RelayPins[0], LOW);
      digitalWrite(RelayPins[1], HIGH);
      relayTime = currTime;
      cycleCount++;
   }

   // Get serial data
   while (Serial.available()) {
      if ( rxCount == 49 ) rxCount = 0;

      c = Serial.read();
      rxBuffer[rxCount++] = c;
      rxBuffer[rxCount] = '\0';
   }

   // Check for incoming message
   if ( rxCount > 6 && rxBuffer[rxCount-1] == '\n') {

      // Parse string
      ret = sscanf(rxBuffer,"%s %i %i %i", mark, &scanPeriod, &scanOn, &scanThold);

      // Check for reset
      if ( ret > 0 && strcmp(mark,"RESET") == 0 ) resetFunc();

      // Check for Count Reset
      if ( ret > 0 && strcmp(mark,"CNTRST") == 0 ) cycleCount = 0;

      // Check marker
      else if ( ret == 3 && strcmp(mark,"PERIOD") == 0 ) {
         relayPeriod = scanPeriod;
         relayOn     = scanOn;
         startThold  = scanThold;

         sprintf(txBuffer,"PERIOD %i %i %i\n",scanPeriod,scanOn,scanThold);
         Serial.write(txBuffer);
      }
      rxCount = 0;
   }
}

