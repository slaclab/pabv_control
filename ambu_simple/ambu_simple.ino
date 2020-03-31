
#define OUTPIN 2

void setup()   {
  pinMode(OUTPIN, OUTPUT);

  digitalWrite(OUTPIN, LOW);   // enable on

}

void loop() {

  digitalWrite(OUTPIN, HIGH);   // enable on

  delay(1000);

  digitalWrite(OUTPIN, LOW);   // enable on

  delay(2000);
}

