  #include <SoftwareSerial.h>
  #include <LiquidCrystal_I2C.h>
  #include <Wire.h>
  LiquidCrystal_I2C lcd(0x27, 20, 4);

  SoftwareSerial HC12(10, 11);

  int levy = 0,pravy = 0,UtokDokonceny = 0,terc = 0, x=0;
  const int ledP = 6;
  const int ledL = 5;

  double L = 0, P = 0;
  
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

    pinMode(ledP, OUTPUT);
    pinMode(ledL, OUTPUT);

    digitalWrite(ledP, LOW);
    digitalWrite(ledL, LOW);
    
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
   /* if ((menu == 8)&&(UtokDokonceny==1)){
      menu = 9;
    }    */
    VypisMenu();
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
           if (menu == 6){
              if (moznost == 2) {
                menu = 8;
                moznost = 3;
              }
            }
           
           if ((menu == 1)&&(millis() > 5000)){
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
              //while (HC12.available()) {
                if (moznost == 3){ //Zpet
                  menu = 1;
                  moznost = 0;
                }
                if (moznost == 1){ //Automaticky
                  menu = 6;
                  moznost = 2;
                }
                if (moznost == 2){ //Manualne
              }
                
                
              //}

            
          }
         
          if (menu == 6){
           
              if (moznost == 3){ // ZPET
                menu = 2;
                moznost = 1;
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
          if (menu == 9){  
            if (x == 4){
              UtokSmazan(); 
              menu = 6;
              moznost = 3;
              x = 0;
            }
          }
          
          if ((menu == 8)&&(UtokDokonceny==1)){
            if ((moznost == 3)&&(x == 13)){
              menu = 9; //SMAZAT
              moznost = 2;
              x = 4;
            }
          }
          
          if ((menu == 8)&&(UtokDokonceny==1)){
            if ((moznost == 3)&&(x == 0)){
              menu = 6;
              moznost = 3;
            }
          }    
          if (menu == 9){
            if (x == 11){
              menu = 8; 
              moznost = 3;
              x = 0;
            }
          }    
          
        }
        if (i == 1){
          if ((menu != 9)&&(menu!=8)){
            if (moznost == 0){
            moznost = 3;}
          else {
            moznost--;}
          }
          
          if (menu == 8){
            if (x == 13){
              x = 0;}
          }
          if (menu == 9){
            if (x == 11){
              x = 4;}
          }
        }
        if (i == 2){
          if ((menu != 9)&&(menu!=8)){
            if (moznost == 3){
              moznost = 0;}
            else {
              moznost++;}
          }
          if (menu == 8){
            if (x == 0){
              x = 13;}
          }
          if (menu == 9){
            if (x == 4){
              x = 11;}
          }
        }
        //inputAction(i);
        inputFlags[i] = LOW;
        Sipka(); 
        Menu();
      }
    }
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
          //while(HC12.available()){
            lcd.setCursor(0,0);
            lcd.print(" == Pozarni utok == ");
            lcd.setCursor(1,1);
            lcd.print("Automaticky");
            lcd.setCursor(1,2);
            lcd.print("Manualne");
            lcd.setCursor(1,3);
            lcd.print("Zpet");
          //}
          //lcd.clear();
          //lcd.setCursor(1,1);
          //lcd.print("Zapni terce");
            
 
          
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
         
          UtokDokonceny = 0;
          lcd.setCursor(0,0);
          lcd.print(" == Automaticky ==  ");
          lcd.setCursor(1,2);
          lcd.print("Spustit");
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
          if (UtokDokonceny != 1){
            
            Automaticky();
            Casomira();
            lcd.setCursor(0,3);
            lcd.print(">");
            
          }
          lcd.setCursor(0,0);
          lcd.print("ID:  ");
          lcd.setCursor(5,0);
          lcd.print("cas  ");
          lcd.setCursor(12,0);
          lcd.print("datum");
          lcd.setCursor(0,1);
          lcd.print(i);
          lcd.setCursor(8,1);
          lcd.print(L);
          lcd.setCursor(8,2);
          lcd.print(P);
          lcd.setCursor(1,3);
          lcd.print("Zpet");
          lcd.setCursor(14,3);
          lcd.print("Smazat");
        }
        break;
      case 9: //Smazat
        {
         
          lcd.setCursor(2,1);
          lcd.print("Opravdu smazat?");
          lcd.setCursor(5,2);
          lcd.print("Ano");
          lcd.setCursor(12,2);
          lcd.print("Ne");
        }
        break;
    }
  }

  void Sipka()
  {
    if ((menu == 2)||(menu == 3)){
      if (moznost == 0)
        moznost = 1;
    }
    if (menu == 8){
      if (moznost == 0){
        moznost = 3;
        x = 13;
      }
      if (moznost == 1){
        moznost = 3;
        x = 0;
      }
      if (moznost == 2){
        moznost = 3;
        x = 13;
      }
        
    }
    lcd.clear();
    lcd.setCursor(x,moznost);
    lcd.print(">");
     
   
    /*if (menu = 9){
      if ((moznost == 1)||(moznost == 2)){
        moznost = 3; 
      }
      if (moznost == 2){
        moznost = 3;
        x = 13;
      }
      else
        x = 0;
    }*/
  }

  /*void SipkaUtok(){
   * 
    lcd.setCursor(x, moznost);
    lcd.print(">");
  }*/

  void Casomira() {
    lcd.clear();
    lcd.print("ID:  ");
    a = millis();
    while (UtokDokonceny==0){ 
        //lcd.print(id);
        lcd.setCursor(5,0);
        lcd.print("cas  ");
        lcd.setCursor(12,0);
        lcd.print("datum");
        lcd.setCursor(0,1);
        
        c = millis();
        i = (c - a) / 1000;
        lcd.print(i);
        lcd.setCursor(0,1);
        terc = HC12.read();
        Serial.println(HC12.read());
        lcd.setCursor(16,1);
        lcd.print("    ");
      if ((terc == 1)&&(levy == 0)&&(i>1)){
            lcd.setCursor(8,1);
            lcd.print("L: ");
            lcd.print(i);
            L = i;
            levy = 1;
            digitalWrite(ledL, HIGH);
          }
        if ((terc == 2)&&(pravy == 0)&&(i>1)){
          lcd.setCursor(8,2);
          lcd.print("P: ");
          P = i;
          lcd.print(i);
          pravy = 1;
          digitalWrite(ledP, HIGH);
        }
      lcd.setCursor(16,1);
      lcd.print("    ");
      lcd.setCursor(16,2);
      lcd.print("    ");
       
      if (((levy == 1)&&(pravy == 1))||(i >= 10.99)){
        levy = 0;
        pravy = 0;
        UtokDokonceny=1;
        //cas_utoku = cas
        //datum_utoku = datum
      }
    } 
  }

  void Automaticky(){
    lcd.clear();
    lcd.setCursor(0,1);
    lcd.print("Zavodnici na mista");
    delay(1000);
    lcd.setCursor(0,1);
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
  }

  void VypisMenu(){
    lcd.setCursor(19,0);
    lcd.print(menu);
    lcd.setCursor(19,1);
    lcd.print(UtokDokonceny);
    lcd.setCursor(19,2);
    lcd.print(moznost);
   
  }

  void UtokSmazan(){
    lcd.clear();
    lcd.setCursor(2,1);
    lcd.print("Utok ID:  ");
    //lcd.print(id);
    lcd.print("byl");
    lcd.setCursor(1,2);
    lcd.print("uspesne smazan");
    delay(2000);
  }

  
