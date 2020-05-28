#include <Arduino.h>
#include <Message.h>
void setup() {
 
  Serial.begin(57600);
}
 
void loop() {
     uint32_t cycle=millis();
     float floats[5]= { 45.53,6554.44,223.55,443.6634,5466.44 };
     uint32_t uints[3]={ 1,2,3};
     Message m;
     m.writeData(Message::DATA,cycle,5,floats,2,uints);
     Serial.print(m.getBuffer());
     m.writeString(Message::VERSION,cycle,"Hello World");
     Serial.print(m.getBuffer());
     delay(1000);          
}
