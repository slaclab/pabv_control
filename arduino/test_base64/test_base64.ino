#include <Arduino.h>
#include <Message.h>
#include <Comm.h>
Comm comm(Serial1);


void setup() {
 
  Serial.begin(57600);
  Serial1.begin(57600);
}
 
void loop() {
     uint32_t cycle=millis();
     float floats[5]= { 45.53,6554.44,223.55,443.6634,5466.44 };
     uint32_t uints[3]={ 1,2,3};
     Message m;
     m.writeData(Message::DATA,cycle,5,floats,2,uints);
     comm.send(m);
     m.writeString(Message::VERSION,cycle,"Hello World");
     comm.send(m);
     comm.read(m);
     Serial.println(m.nFloat());
     Serial.println(m.getString());
     Serial.println(m.status());
     delay(1000);          
}
