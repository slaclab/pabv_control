
const unsigned int RelayCount     = 2;
const unsigned int RelayPins[2]   = {2,3};
const unsigned int AnalogCount    = 4;
const unsigned int AnalogPins[4]  = {2,3,4,5};
const unsigned int AnalogMillis   = 10;
const unsigned int DefRelayPeriod = 3000;
const unsigned int DefRelayOn     = 1000;

unsigned int relayPeriod;
unsigned int relayOn;
unsigned int x;
unsigned int analogValues[4];
unsigned int analogTime;
unsigned int currTime;
unsigned int relayTime;
unsigned int rxCount;
unsigned int scanPeriod;
unsigned int scanOn;
unsigned int cycleStart;

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
   rxCount    = 0;

   relayPeriod = DefRelayPeriod;
   relayOn     = DefRelayOn;

   Serial.begin(9600);
}

void (* resetFunc)(void) = 0;

void loop() {

   cycleStart = 0;

   for (x=0; x < AnalogCount; x++)
      analogValues[x] = analogRead(AnalogPins[x]);

   currTime = millis();

   // Turn off time
   if ((currTime - relayTime) > relayOn ) {
      digitalWrite(RelayPins[0], HIGH);
      digitalWrite(RelayPins[1], LOW);
   }

   // End of cycle, turn on
   if ((currTime - relayTime) > relayPeriod ) {
      digitalWrite(RelayPins[0], LOW);
      digitalWrite(RelayPins[1], HIGH);
      relayTime = currTime;
      cycleStart = 1;
   }

   if ((currTime - analogTime) > AnalogMillis ) {
      sprintf(txBuffer,"ANALOG %i %i %i %i %i\n", cycleStart,
                       analogValues[0], analogValues[1],
                       analogValues[2], analogValues[3]);

      Serial.write(txBuffer);
      analogTime = currTime;
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
      ret = sscanf(rxBuffer,"%s %i %i", mark, &scanPeriod, &scanOn);

      // Check for reset
      if ( ret > 0 && strcmp(mark,"RESET") == 0 ) resetFunc();

      // Check marker
      else if ( ret == 3 && strcmp(mark,"PERIOD") == 0 ) {
         relayPeriod = scanPeriod;
         relayOn     = scanOn;

         sprintf(txBuffer,"PERIOD %i %i\n",scanPeriod,scanOn);
         Serial.write(txBuffer);
      }
      rxCount = 0;
   }
}

