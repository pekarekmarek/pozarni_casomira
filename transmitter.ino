#include <SoftwareSerial.h>
SoftwareSerial HC12(10, 11); 
//int pot = A2;
int val;
void setup() {
  Serial.begin(9600);           
  HC12.begin(9600);               
  //pinMode(pot,INPUT);
}
void loop() {  
  val = 1;
  HC12.write(val);
  delay(100);
  val = 0;
  HC12.write(val);
  delay(900);
  //int val = map(analogRead(pot),0,1024,0,255);
       
}
