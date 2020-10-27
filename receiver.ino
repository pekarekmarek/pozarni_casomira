#include <SoftwareSerial.h>
SoftwareSerial HC12(10, 11); // HC-12 TX Pin, HC-12 RX Pin
int LED = 3;
int val;
void setup() {
  Serial.begin(9600);             // Serial port to computer
  HC12.begin(9600);               // Serial port to HC12
  pinMode(LED,OUTPUT);
}++
void loop() {
  while (HC12.available()) {        // If HC-12 has data
    val = HC12.read();
    Serial.println(HC12.read());      // Send the data to Serial monitor
    //analogWrite(LED,val);*/
    digitalWrite(LED, val);
  }  
}
