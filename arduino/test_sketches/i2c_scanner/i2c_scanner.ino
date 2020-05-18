/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

#include <Wire.h>

#define DLC_L20D4_ADDR 0x2D
#define DLC_L20D4_CMD  0xAC

char data_[10];
int x_;
double raw_;
double scaled_;

void i2c_scan(){
  Serial.println("\nI2C Scanner");
  byte error, address;
  int nDevices;
  Serial.println("Scanning...");
  nDevices = 0;
  for(address = 1; address < 127; address++ ) {
    //Serial.println(address, HEX);
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
    if (error == 0) {
      Serial.print("I2C device found at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
      nDevices++;
    }
    else if (error==4) {
      Serial.print("Unknow error at address 0x");
      if (address<16) {
        Serial.print("0");
      }
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0) {
    Serial.println("No I2C devices found\n");
  }
  else {
    Serial.println("done\n");
  }
}

void print_data() {
  Wire.beginTransmission(DLC_L20D4_ADDR);
  Wire.write(DLC_L20D4_CMD);
  Wire.endTransmission();
  delay(1000);
  Wire.requestFrom(DLC_L20D4_ADDR, byte(4));
  for (x_=0; x_ < 4; x_++) {
    data_[x_]=0b00000000;
    data_[x_] = Wire.read();
    Serial.print(data_[x_], BIN);
    Serial.print("  ");
  }
  Serial.println("");

  raw_ = ((double)data_[1] * 65536.0) + ((double)data_[2] * 256.0) + (double)data_[3];
  Serial.println(raw_);
  scaled_ = 1.25 * ((raw_ - 8388608.0) / 8388608.0) * 20.0 * 2.54;
  Serial.println(scaled_);
  Serial.println("DONE");
}

void setup() {
  Wire.begin();
  Serial.begin(9600);
}
 
void loop() {
  i2c_scan();
  print_data();  
  delay(5000);          
}
