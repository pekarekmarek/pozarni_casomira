#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial HC12(10, 11); // TX, RX

const int pravy = 4;
const int levy = 5;
int terc = 0;

void setup() {
  Serial.begin(9600);            
  HC12.begin(9600);   
  digitalWrite(levy, HIGH);
  digitalWrite(pravy, HIGH);
}
void loop() {  
  terc = 5;
  HC12.write(terc);
  if (digitalRead(levy) == LOW){
    terc = 1;
    HC12.write(terc);
  }
  if (digitalRead(pravy) == LOW){
    terc = 2;
    HC12.write(terc);
  }
  Serial.println(terc);
   
}