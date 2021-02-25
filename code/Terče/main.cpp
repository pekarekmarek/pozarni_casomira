#include <Arduino.h>
#include <SoftwareSerial.h>
SoftwareSerial HC12(10, 11); // HC-12 TX Pin, HC-12 RX Pin
#define batterypin A1
#define nabijenipin 6 ///dasdsadasd
#define levy 5
#define pravy 4
byte terc = 0;
int stav;

void setup() {
  Serial.begin(115200);             // Serial port to computer
  HC12.begin(115200);               // Serial port to HC12
  pinMode(levy,INPUT_PULLUP);
  pinMode(pravy,INPUT_PULLUP);
  pinMode(batterypin, INPUT);
  pinMode(nabijenipin, INPUT);
}
void loop() {  
  stav = analogRead(batterypin);
  if (stav >= 820)  stav = 6;
  else if (stav >= 740 && stav < 820) stav = 5; 
  else if (stav >= 700 && stav < 740) stav = 4;
  else if (stav >= 640 && stav < 700) stav = 3;
  else if (stav >= 600 && stav < 640) stav = 2;
  if (digitalRead(nabijenipin) == HIGH) stav = 8;
  HC12.write(stav);
  Serial.println(stav);
  if (digitalRead(levy) == LOW){
    terc = 11;
    HC12.write(terc);
    Serial.println(terc);
  }
  if (digitalRead(pravy) == LOW) {
    terc = 66;
    HC12.write(terc);
    Serial.println(terc);
    
  }
  terc = 0;
  
}