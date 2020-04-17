
#include <Wire.h>

const unsigned int RelayCount     = 2;
const unsigned int RelayPins[2]   = {2,3};
const unsigned int AnalogCount    = 4;
const unsigned int AnalogPins[4]  = {2,3,4,5};
const unsigned int AnalogMillis   = 9;
const unsigned int DefRelayPeriod = 3000;
const unsigned int DefRelayOn     = 1000;
const unsigned int DefStartThold  = 0xFFFF;

const byte I2cAddrHw  = 73;
const byte I2cAddrNpa = 40;

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
unsigned int compValue;
unsigned int inhalation;
byte i2cRaw[2];
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
   inhalation  = 0;

   Serial.begin(57600);
   Wire.begin();
}

void (* resetFunc)(void) = 0;

void loop() {
   autoStart = 0;

   currTime = millis();

   if ((currTime - analogTime) > AnalogMillis ) {

      // Read npa sensor
      Wire.requestFrom(I2cAddrNpa, byte(2));
      i2cHigh = Wire.read() & 0x3F;
      i2cLow = Wire.read();

      // Read honywell
      Wire.requestFrom(I2cAddrHw, byte(2));
      for (x=0; x < 2; x++) i2cRaw[x] = Wire.read();

      sprintf(txBuffer,"ANALOG %i 0x%.2x%.2x 0x%.2x%.2x\n", (cycleCount / 100),
                       i2cRaw[0], i2cRaw[1],
                       i2cHigh, i2cLow);

      Serial.write(txBuffer);
      analogTime = currTime;
      cycleCount += 1;
   }
}

