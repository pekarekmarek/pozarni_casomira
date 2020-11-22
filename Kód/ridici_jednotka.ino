  #include <SoftwareSerial.h>
  #include <LiquidCrystal_I2C.h>
  #include <Wire.h>
  LiquidCrystal_I2C lcd(0x27, 20, 4);

  SoftwareSerial HC12(10, 11);

  int levy = 0,pravy = 0,w = 0,terc = 0;
  
  double i = 0;
  double a = millis();
  double c;
  
  const int pocetTlacitek = 3;
  const int pinyTlacitek[pocetTlacitek] = {2, 3, 4};
  int nove[pocetTlacitek];
  int stare[pocetTlacitek] = {LOW, LOW, LOW};
  bool inputFlags[pocetTlacitek] = {LOW, LOW, LOW};
  long debounce[pocetTlacitek] = {0, 0, 0};
  long Delay = 5;
  
  int menu = 1;
  int moznost = 0;

  /*1-hlavni, 
   * 2-utok, 6-auto, 7-manualne, 8-casomira
   * 3-priprava, 9-odpocet
   * 4-historie, 10-
   * 5-nejrychlejsi sestriky, 
  */
  void Menu();
  void Sipka();
  
  void setup() {
    Serial.begin(9600);
    HC12.begin(9600);
    lcd.begin(20, 4);
    lcd.clear();
    lcd.backlight();
    for (int i = 0; i < pocetTlacitek; i++) {
      pinMode(pinyTlacitek[i], INPUT);
      digitalWrite(pinyTlacitek[i], HIGH); // pull-up 
    }
    lcd.setCursor(1,1);
    lcd.print("Bezdratova Pozarni");
    lcd.setCursor(5,2);
    lcd.print("Casomira");
    lcd.setCursor(0,3);
    lcd.print("V1.0");
    lcd.setCursor(13,3);
    lcd.print("By Marw");
    delay(3000);
    lcd.clear();
  }
 

  void loop() {
  while (HC12.available()){
    
    //Serial.println(millis());
    for (int i = 0; i < pocetTlacitek; i++){
      int reading = digitalRead(pinyTlacitek[i]);
      if (reading != stare[i]) {
        debounce[i] = millis();
      }
      if ((millis() - debounce[i]) > Delay) {
        if (reading != nove[i]) {
          nove[i] = reading;
          if (nove[i] == HIGH) {
            inputFlags[i] = HIGH;
          }
        }
      }
      stare[i] = reading;
    }
    for (int i = 0; i < pocetTlacitek; i++) {
      if (inputFlags[i] == HIGH) {
        if (i == 0) {
           if ((menu == 1)&&(millis() > 6000)){
              if (moznost == 1){
                menu = 3;
                //moznost = 1;
              }
              if (moznost == 0){
                menu = 2;
                //moznost = 1;
              }
              if (moznost == 2){
                menu = 4;
                moznost = 1;
              }
              if (moznost == 3){
                menu = 5;
                moznost = 1;
              }
                
            }
          if (menu == 2){ //Pozarni utok
            if (moznost == 3){ //Zpet
              menu = 1;
              moznost = 0;
            }
            if (moznost == 1){ //Automaticky
              menu = 8;
              moznost = 3;
            }
            if (moznost == 2){ //Manualne
              
            }
          }
          if (menu == 3){
            if (moznost == 3){
              menu = 1;
              moznost = 1;
            }
          }
          if (menu == 4){
            if (moznost == 3){
              menu = 1;
              moznost = 2;
            }
          }
          if (menu == 5){
            if (moznost == 3){
              menu = 1;
              moznost = 3;
            }
          }
          if (menu == 6){
            if (moznost == 2){
              menu = 8;
              moznost = 3;
            }
            if (moznost == 3){
              menu = 2;
              moznost = 2;
            }
          }
        }
        if (i == 1){
          if (moznost == 0)
            moznost = 3;
          else 
            moznost--;
        }
        if (i == 2){
          if (moznost == 3)
            moznost = 0;
          else 
            moznost++;
        }
        //inputAction(i);
        inputFlags[i] = LOW;
   
        Sipka(); 
        Menu();
      }
    }
       
    }
    lcd.clear();
    lcd.print("Unavailable");
    delay(1000);
    }
  void Menu()
  {
    switch (menu)
    {
      case 1: // Hlavni menu
        {
 
          lcd.setCursor(1,0);
          lcd.print("Pozarni utok");
          lcd.setCursor(1,1);
          lcd.print("Odpocet pripravy");
          lcd.setCursor(1,2);
          lcd.print("Historie casu");
          lcd.setCursor(1,3);
          lcd.print("Nejrychlejsi utoky");
        }
        break;
      case 2: //Pozarni utok
        {
          if(HC12.available()){
            lcd.setCursor(0,0);
            lcd.print(" == Pozarni utok == ");
            lcd.setCursor(1,1);
            lcd.print("Automaticky");
            lcd.setCursor(1,2);
            lcd.print("Manualne");
            lcd.setCursor(1,3);
            lcd.print("Zpet");
          }
          else {
            lcd.print("Zapni terce");
          }
          
        }
        break;
      case 3: //Priprava
        {
         
          lcd.setCursor(0,0);
          lcd.print(" Priprava zakladny  ");
          lcd.setCursor(1,1);
          lcd.print("X");
          lcd.setCursor(3,1);
          lcd.print("minut");
          lcd.setCursor(1,2);
          lcd.print("Zahajit odpocet");
          lcd.setCursor(1,3);
          lcd.print("Zpet");
        }
        break;
      case 4: //Historie
        {
         
          lcd.setCursor(0,0);
          lcd.print("Historie ");
          lcd.setCursor(1,3);
          lcd.print("Zpet");
        }
        break;
      case 5: //Nejrychlejsi sestriky
        {
         
          lcd.setCursor(0,0);
          lcd.print(" Nejrychlejsi utoky");
          lcd.setCursor(1,3);
          lcd.print("Zpet");
        }
        break;
      case 6: //Automaticky
        {
         
          lcd.setCursor(0,0);
          lcd.print(" == Automaticky ==  ");
          lcd.setCursor(1,2);
          lcd.print("Spustit odpocet");
          lcd.setCursor(1,3);
          lcd.print("Zpet");
        }
        break;
      case 7: //Manualne
        {
         
          lcd.setCursor(0,0);
          lcd.print("   == Manualne ==   ");
          lcd.setCursor(1,2);
          lcd.print("Spustit casomiru");
          lcd.setCursor(1,3);
          lcd.print("Zpet");
        }
        break;
      case 8: //Automaticky odpocet
        {
          lcd.clear();
          lcd.setCursor(0,1);
          lcd.print("Zavodnici na mista");
          delay(1000);
          lcd.clear();
          lcd.setCursor(0,1);
          lcd.print("Pripravte se");
          for (int i = 0; i < 3; i++){
            lcd.print(" .");
            delay(1000);
          }
          lcd.clear();
          lcd.setCursor(0,1);
          lcd.print("Pozor!");
          delay(random(2000,4000));
          Casomira();
        }
        break;
    }
  }

  void Sipka()
  {
    lcd.clear();
    lcd.setCursor(0,moznost);
    lcd.print(">");
    if ((menu == 2)||(menu == 3)){
      if (moznost == 0)
        moznost = 1; 
    }
  }

  void Casomira() {
    lcd.clear();
    lcd.print("ID:  ");
    //lcd.print(id);
    lcd.print("cas  ");
    lcd.print("datum");
    lcd.setCursor(0,1);
    a = millis();
    while (w==0){ 
        c = millis();
        i = (c - a) / 1000;
        lcd.print(i);
        lcd.setCursor(0,1);
        terc = HC12.read();
        Serial.println(HC12.read());
        lcd.setCursor(16,1);
        lcd.print("    ");
      if ((terc == 1)&&(levy == 0)){
            lcd.setCursor(8,1);
            lcd.print("L: ");
            lcd.print(i);
            levy = 1;
          }
      if ((terc == 2)&&(pravy == 0)){
        lcd.setCursor(8,2);
        lcd.print("P: ");
        lcd.print(i);
        pravy = 1;
      }
      lcd.setCursor(16,1);
      lcd.print("    ");
      lcd.setCursor(16,2);
      lcd.print("    ");
       
      if (((levy == 1)&&(pravy == 1))||(i >= 60)){
        levy = 0;
        pravy = 0;
        w=1;
        lcd.setCursor(0,3);
        lcd.print(">Zpet         Smazat");
        //delay(1500);
      }
   }
    //lcd.print(pravy);
    
    
  }

  
