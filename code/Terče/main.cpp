#include <Arduino.h>
#include <SoftwareSerial.h>
SoftwareSerial HC12(10, 11); // HC-12 TX Pin, HC-12 RX Pin
#define batterypin A1
#define nabijenipin 12 
#define levy 2
#define pravy 3

#define TransmitCount 10

byte terc = 0;
byte transmit = 0;
int stav;

 
unsigned long pomocna = 0;

void setup() {
  Serial.begin(9600);            
  HC12.begin(115200);               
  pinMode(levy,INPUT_PULLUP);
  pinMode(pravy,INPUT_PULLUP);
  pinMode(batterypin, INPUT);
  pinMode(nabijenipin, INPUT);
}
void loop() {  
  if (HC12.available()) {
    transmit = HC12.read();
    if (transmit == 0) {
      stav = analogRead(batterypin);
      if (stav >= 820)  stav = 6;
      else if (stav >= 740 && stav < 820) stav = 5; 
      else if (stav >= 700 && stav < 740) stav = 4;
      else if (stav >= 640 && stav < 700) stav = 3;
      else if (stav >= 600 && stav < 640) stav = 2;
      else stav = 7;
      if (digitalRead(nabijenipin) == HIGH) stav = 8;
      for (byte i = 0; i < TransmitCount; i++) {
        HC12.write(stav);
        Serial.println(stav);
      }
    }
  }
  while (transmit == 1){
    if (digitalRead(levy) == LOW) {
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
      if (HC12.available()) transmit = HC12.read();
  }    
}