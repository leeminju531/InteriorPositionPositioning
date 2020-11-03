#include <SoftwareSerial.h>
SoftwareSerial Serial1(2,3); //RX, TX 연결

//SoftwareSerial Serial1(2,3); 

void setup() {
  // put your setup code here, to run once:
 Serial.begin(9600);
 Serial1.begin(9600);
// Serial1.begin(9600);
 
}

void loop() {
  if (Serial1.available()) {
    Serial.write(Serial1.read());
    
  }
  
  if (Serial.available()) {
    byte data = Serial.read();
    Serial1.write(data);
  }
  
}
