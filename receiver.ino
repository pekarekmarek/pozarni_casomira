#include <SoftwareSerial.h>
SoftwareSerial HC12(10, 11); 
int LED = 3;
int val;
void setup() {
  Serial.begin(9600);             
  HC12.begin(9600);               
  pinMode(LED,OUTPUT);
}++
void loop() {
  while (HC12.available()) {       
    val = HC12.read();
    Serial.println(HC12.read());      
    //analogWrite(LED,val);*/
    digitalWrite(LED, val);
  }  
}
