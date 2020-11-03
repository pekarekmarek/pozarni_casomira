#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
LiquidCrystal_I2C lcd(0x27,16,2);

SoftwareSerial HC12(10, 11); // HC-12 TX Pin, HC-12 RX Pin
//const int led1 = 2;
//const int led2 = 3;
const int okay = 2;
const int up = 3;
const int down = 4;
int levy = 0,pravy = 0,w = 0,terc = 0;

double i = 0;
double a = millis();
double c ;

/*const int numOfInputs = 2;
const int inputPins[numOfInputs] = {2,3};
const int outputPin = 10;

int LEDState = 0;
int inputState[numOfInputs];
int lastInputState[numOfInputs] = {LOW,LOW};
bool inputFlags[numOfInputs] = {LOW,LOW};
int inputCounters[numOfInputs];

long lastDebounceTime[numOfInputs] = {0,0};
long debounceDelay = 50;*/

//int val;
void setup() {
  Serial.begin(9600);             // Serial port to computer
  HC12.begin(9600);
    
  lcd.begin(16, 2);
  lcd.clear();
  lcd.backlight();
  
  //pinMode(led1,OUTPUT);
  //pinMode(led2,OUTPUT);
  pinMode(okay,INPUT);
  pinMode(up,INPUT);
  pinMode(down,INPUT);
  //digitalWrite(led1, LOW);
  //digitalWrite(led2, LOW);
  
  digitalWrite(okay, HIGH);
  digitalWrite(up, HIGH);
  digitalWrite(down, HIGH);
}
void loop() {
   
   while (HC12.available()) {        // If HC-12 has data
    lcd.clear();
    lcd.print("Casomira");
    w=0;
    terc = 0;
    delay(50);
    if(digitalRead(okay) == LOW){
      lcd.clear();
      a = millis();
      
      while (w==0){ 
        c = millis();
        i = (c - a) / 1000;
        lcd.print(i);
        lcd.setCursor(0,0);
        terc = HC12.read();
        Serial.println(HC12.read());
        if ((terc == 1)&&(levy == 0)){
          lcd.setCursor(8,0);
          lcd.print("L: ");
          lcd.print(i);
          levy = 1;
        }
        if ((terc == 2)&&(pravy == 0)){
          lcd.setCursor(8,1);
          lcd.print("P: ");
          lcd.print(i);
          pravy = 1;
        }
        if (((levy == 1)&&(pravy == 1))||(i >= 60)){
          levy = 0;
          pravy = 0;
          while(digitalRead(okay) == HIGH){
            lcd.setCursor(0,0);
            delay(2000);
          }
          lcd.clear();
          w=1;
          lcd.print("Casomira");
          delay(1500);
        }
      }
      
    Serial.println(HC12.read());      // Send the data to Serial monitor
    //analogWrite(LED,val);
    //digitalWrite(led1, val);
    } 
  }
  lcd.clear();
  lcd.print("No radio");
  delay(1000);

// Start printing elements as i ndividuals
 
// Delay so the program doesn't print non-stop
 /* buttonState = digitalRead(button);
  if (buttonState != lastButtonState) {
        // jestliže se stav změnil, navyšte hodnotu počítadla
        if (buttonState == HIGH) {
            // jestliže je současný stav HIGH, tlačítko přešlo
            //z off na on:
            buttonPushCounter++;
            Serial.println("on");
            Serial.print("number of button pushes:  ");
            Serial.println(buttonPushCounter);
        }
        else {
            // jestliže je současný stav LOW, tlačítko přešlo
            // z on na off:
            Serial.println("off");
        }
    }
    // uložte současný stav jako „poslední stav“, 
    //abyste ho mohli v příští smyčce použít
    lastButtonState = buttonState;
    
    
    // zapne LED po každých čtyřech stlačeních
    // použitím modula počítadla.
    // funkce modulo vám vrátí zbytek
    // z dělení dvou čísel:
    if (buttonPushCounter % 2 == 0) {
        digitalWrite(led1, HIGH);
    } else {
        digitalWrite(led1, LOW);
    }
    if (buttonPushCounter % 2 != 0) {
        digitalWrite(led2, HIGH);
    } else {
        digitalWrite(led2, LOW);
    }*/
   
  
}
