#include <SoftwareSerial.h>
SoftwareSerial HC12(10, 11); // HC-12 TX Pin, HC-12 RX Pin
//int pot = A2;

const int pravy = 4;
const int levy = 5;
int terc = 0;

void setup() {
  Serial.begin(9600);             // Serial port to computer
  HC12.begin(9600);   
  digitalWrite(levy, LOW);
  digitalWrite(pravy, LOW);// Serial port to HC12
  //pinMode(pot,INPUT);
}
void loop() {  
  terc = 0;
  HC12.write(terc);
  if (digitalRead(levy) == HIGH){
    terc = 1;
    //HC12.write(terc);
  }
  if (digitalRead(pravy) == HIGH){
    terc = 2;
    //HC12.write(terc);
  }
  else {
    terc = 0;
    digitalWrite(levy, LOW);
    digitalWrite(pravy, LOW);
  }
  
  //int val = map(analogRead(pot),0,1024,0,255);
       // Send that data to HC-12  
}
