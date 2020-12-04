  /*
   *        Priprava zakladny -> minuty++ z menu 1
   *        hw preruseni u odpoctu?
   *        indikace uspesne komunikace
   *        automaticke mazani pokud cas > 60s
   *        Pozarni utok 2 moznost 0
   * 
  */
  #include <SoftwareSerial.h>
  #include <LiquidCrystal_I2C.h>
  #include <SPI.h>
  #include "SdFat.h"
  
  LiquidCrystal_I2C lcd(0x27, 20, 4);

  SoftwareSerial HC12(8, 9);

  bool levy = false,pravy = false;
  bool UtokDokonceny = false, manualne = false;
  byte terc = 0, x=0;
  bool ahoj = false;
  byte priprava = 5;
  byte minuty = 0;
  byte sekundy = 0;

  //SD KARTA
  File myFile;
  SdFat SD;
  byte ID = 1;;
  const byte chipSelect = 10;
  /////////
  unsigned long pomocna = 0;
  
  double L = 0, P = 0;
  
  double i = 0;
  double a = millis();
  double c;
  
  const byte pinyTlacitek[3] = {2, 3, 4};
  byte nove[3];
  byte stare[3] = {LOW, LOW, LOW};
  bool inputFlags[3] = {LOW, LOW, LOW};
  long debounce[3] = {0, 0, 0};
  byte Delay = 5;
  
  byte menu = 1;
  byte moznost = 0;
  
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
    
    
    if (!SD.begin(chipSelect)){
      lcd.print("Chybi SD karta");
      delay(1000);
      return;
    }    
    for (byte i = 0; i < 3; i++) {
      pinMode(pinyTlacitek[i], INPUT);
      digitalWrite(pinyTlacitek[i], HIGH); // pull-up 
    }
    lcd.clear();
    lcd.setCursor(1,1);
    lcd.print("Bezdratova Pozarni");
    lcd.setCursor(5,2);
    lcd.print("Casomira");
    lcd.setCursor(0,3);
    lcd.print("V1.0");
    delay(3000);
    lcd.clear();
    while (SD.exists(String(ID))){
      ID++;
    }
  }
 

  void loop() {
   
    VypisMenu();
    for (byte i = 0; i < 3; i++){
      byte reading = digitalRead(pinyTlacitek[i]);
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
    for (byte i = 0; i < 3; i++) {
      if (inputFlags[i] == HIGH) {
        if (i == 0){                //SELECT
          switch (menu)
          {
            case 1: // Hlavni menu
              {
                if (millis() > 5000){
                  switch (moznost) {
                    case 0: {
                      menu = 2;
                      moznost = 1;
                    } break;
                    case 1: {
                      menu = 3;
                      moznost = 1;
                    } break;
                    case 2: {
                      menu = 4;
                      moznost = 3;
                    } break;
                    case 3: {
                      menu = 5;
                      moznost = 1;
                    } break; 
                  }
                }   
              }
              break;
            case 2: // Pozarni utok
              {
                switch (moznost) {
                  case 1: {
                    menu = 6;
                    moznost = 2;
                  } break;
                  case 2: {
                    menu = 7;
                    moznost = 2;
                  } break;
                  case 3: {
                    menu = 1;
                    moznost = 0;
                  } break; 
                }
              }
              break;
            case 3: // Odpocet pripravy
              {
                switch (moznost) {
                  case 1: {
                    if (priprava == 6){
                      priprava = 1;
                    }
                    else {
                      priprava++;
                    }
                  } break;
                  case 2: {
                    while ((digitalRead(2) == HIGH)&&(ahoj == false)){
                      Odpocet();
                    }
                    ahoj = true;
                    minuty = priprava;
                    sekundy = 0;
                    menu = 1;
                    moznost = 0;
                  } break;
                  case 3: {
                    menu = 1;
                    moznost = 1;
                  } break; 
                }
              }
              break;
            case 4: // Historie
              {
                switch (x) {
                  case 0: {
                    menu = 1;
                    moznost = 2;
                  } break;
                  case 6: {
                    if (ID != 1){
                      ID--;
                    }
                  } break;
                  case 10: {
                    if (SD.exists(String(ID + 1))){
                      ID++;
                    }
                  } break;
                  case 13: {
                    //SMAZAT
                    }
                  } break;  
                }
              break;
            case 5: // Nejrychlejsi casy
              {
                switch (moznost) {
                  case 3: {
                    menu = 1;
                    moznost = 3;
                  } break; 
                }
              }
              break;
            case 6: // Automaticky
              {
                switch (moznost) {
                  case 2: {
                    menu = 8;
                    moznost = 3;
                  } break;
                  case 3: {
                    menu = 2;
                    moznost = 1;
                  } break; 
                }
              }
              break;
            case 7: // Manualne
              {
                switch (moznost) {
                  case 2: {
                    menu = 8;
                    moznost = 3;
                    manualne = 1;
                  } break;
                  case 3: {
                    menu = 2;
                    moznost = 2;
                  } break; 
                }
              }
              break;
            case 8: // Casomira
              {
                if (UtokDokonceny == true){
                  switch (x) {
                    case 0: {
                      if (manualne == 0){ 
                        menu = 6;  
                      }
                      else { 
                        menu = 7;
                      }
                      moznost = 3;
                    } break;
                    case 13: {
                      menu = 9; // Smazat
                      moznost = 2;
                      x = 4;
                    } break;
                  } 
                }
              }
              break;
            case 9: // Smazat
              {
                switch (x) {
                  case 4: {
                    UtokSmazan();
                    menu = 2;
                    moznost = 3;
                    x = 0;
                  } break;
                  case 11: {
                    menu = 8;
                    moznost = 3;
                    x = 0;
                  } break;
                }
              }
              break;
            }
          }
          if (i == 1){                //UP
            switch (menu){
              case 1: {  // 4 Moznosti
                if (moznost == 0){
                  moznost = 3;
                }
                else {
                  moznost--;
                }
              } break;
             case 2: // 3 Moznosti
             case 3: {
                if (moznost == 1){
                  moznost = 3;
                }
                else {
                  moznost--;
                }
              } break;
             case 4: {
                if (x == 0){
                  x = 13;}
                else if (x == 6){
                  x = 0;}
                else if (x == 10){
                  x = 6;}
                else if (x == 13){
                  x = 10;}
              } break;
             case 5: {
                //Nejrychlejsi sestriky
              } break;
             case 6:
             case 7: {
                if (moznost  == 2){
                  moznost = 3;}
                else {
                  moznost--; }
              } break;
             case 8: {
                if (x == 13){
                  x = 0; }
              } break;
             case 9: {
                if (x == 11){
                  x = 4; }
              } break;
            }
          }
          if (i == 2){            //DOWN
            switch (menu){
              case 1: {  // 4 Moznosti
                if (moznost == 3){
                  moznost = 0;
                }
                else {
                  moznost++;
                }
              } break;
             case 2: // 3 Moznosti
             case 3: {
                if (moznost == 3){
                  moznost = 1;
                }
                else {
                  moznost++;
                }
              } break;
             case 4: {
                if (x == 13){
                  x = 0;}
                else if (x == 10){
                  x = 13;}
                else if (x == 6){
                  x = 10;}
                else if (x == 0){
                  x = 6;}
              } break;
             case 5: {
                //Nejrychlejsi sestriky
              } break;
             case 6:
             case 7: {
                if (moznost  == 3){
                  moznost = 2;}
                else {
                  moznost++; }
              } break;
             case 8: {
                if (x == 0){
                  x = 13; }
              } break;
             case 9: {
                if (x == 4){
                  x = 11; }
              } break;
            }
          }
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
            lcd.print("Pozarni utok");
            lcd.setCursor(1,1);
            lcd.print("Automaticky");
            lcd.setCursor(1,2);
            lcd.print("Manualne");
            lcd.setCursor(1,3);
            lcd.print("Zpet");
            ahoj = false;
          //}
          //lcd.clear();
          //lcd.setCursor(1,1);
          //lcd.print("Zapni terce");
            
 
          
        }
        break;
      case 3: //Priprava
        {
         
          lcd.setCursor(0,0);
          lcd.print("Priprava zakladny");
          lcd.setCursor(1,1);
          lcd.print(priprava);
          lcd.setCursor(3,1);
          if ((priprava == 5)||(priprava == 6)){
            lcd.print("minut");
          }
          else if (priprava == 1){
            lcd.print("minuta");
          }
          else {
            lcd.print("minuty");
          }
          lcd.setCursor(1,2);
          lcd.print("Zahajit odpocet");
          lcd.setCursor(1,3);
          lcd.print("Zpet");
        }
        break;
      case 4: //Historie
        {
         
          lcd.setCursor(0,0);
          lcd.print("ID:");
          lcd.print(ID);
          lcd.setCursor(5,0);
          lcd.print("cas  ");
          lcd.setCursor(12,0);
          lcd.print("datum");
          lcd.setCursor(0,1);
          myFile = SD.open(String(ID) + "/Vysledny.txt");
          if (myFile) {
            for (byte data = 0; data < 5; data++){
              lcd.write(myFile.read());
            }
            myFile.close();
          } else {
            Serial.println("error opening Vys.txt");
          }
          lcd.setCursor(7,1);
          lcd.print("L:");
          myFile = SD.open(String(ID) + "/L.txt");
          if (myFile) {
            for (byte data = 0; data < 5; data++){
              lcd.write(myFile.read());
            }
            myFile.close();
          } else {
            Serial.println("error opening L.txt");
          }
          lcd.setCursor(7,2);
          lcd.print("P:");
          myFile = SD.open(String(ID) + "/P.txt");
          if (myFile) {
            for (byte data = 0; data < 5; data++){
              lcd.write(myFile.read());
            }
            myFile.close();
          } else {
            Serial.println("error opening P.txt");
          }//*
          lcd.setCursor(1,3);
          lcd.print("Zpet");
          lcd.setCursor(14,3);
          lcd.print("Smazat");
          lcd.setCursor(7,3);
          lcd.print("<");
          lcd.setCursor(11,3);
          lcd.print(">");
        }
        break;
      case 5: //Nejrychlejsi sestriky
        {
         
          lcd.setCursor(0,0);
          lcd.print("Nejrychlejsi utoky");
          lcd.setCursor(1,3);
          lcd.print("Zpet");
        }
        break;
      case 6: //Automaticky
        {
         
          UtokDokonceny = false;
          lcd.setCursor(0,0);
          lcd.print("Automaticky");
          lcd.setCursor(1,2);
          lcd.print("Spustit");
          lcd.setCursor(1,3);
          lcd.print("Zpet");
        }
        break;
      case 7: //Manualne
        {
         
          manualne = false;
          UtokDokonceny = false;
          lcd.setCursor(0,0);
          lcd.print("Manualne");
          lcd.setCursor(1,2);
          lcd.print("Spustit casomiru");
          lcd.setCursor(1,3);
          lcd.print("Zpet");
        }
        break;
      case 8: //Automaticky odpocet
        {  
          if (UtokDokonceny != true){
            if (manualne != true){
              Automaticky();
            }
            /*while (SD.exists(String(ID))){
                ID++;
            }*/
            Casomira();
            lcd.setCursor(0,3);
            lcd.print(">");
            
          }
         
          lcd.setCursor(0,0);
          lcd.print("ID:");
          lcd.print(ID);
          lcd.print(" cas ");
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
          /*SD.mkdir(ID);
          myFile = SD.open(String(ID) + "/L.txt", FILE_WRITE);
          if (myFile) {
            myFile.println(L);
            myFile.close();
            Serial.println("L done.");
          } else {
            Serial.println("error opening L test.txt");
          }
          myFile = SD.open(String(ID) + "/P.txt", FILE_WRITE);
          if (myFile) {
            myFile.println(P);
            myFile.close();
            Serial.println("P done.");
          } else {
            Serial.println("error opening P test.txt");
          }
          myFile = SD.open(String(ID) + "/Vysledny.txt", FILE_WRITE);
          if (myFile) {
            myFile.print(i);
            myFile.close();
            Serial.println("Vys done.");
          } else {
            Serial.println("error opening Vys test.txt");
          }*/
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
    lcd.clear();
    lcd.setCursor(x,moznost);
    lcd.print(">");
  }

  void Casomira() {
    lcd.clear();
    lcd.print("ID:");
    a = millis();
    lcd.print(ID);
    while (UtokDokonceny==0){ 
        
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
        //Serial.println(HC12.read());
        lcd.setCursor(16,1);
        lcd.print("    ");
        if ((terc == 1) && (levy == false) && (i > 1)){
            lcd.setCursor(8,1);
            lcd.print("L: ");
            lcd.print(i);
            L = i;
            levy = true;
            //digitalWrite(ledL, HIGH);
          }
        if ((terc == 2) && (pravy == false) && (i > 1)){
          lcd.setCursor(8,2);
          lcd.print("P: ");
          P = i;
          lcd.print(i);
          pravy = true;
          //digitalWrite(ledP, HIGH);
        }
      lcd.setCursor(16,1);
      lcd.print("    ");
      lcd.setCursor(16,2);
      lcd.print("    ");
       
      if (((levy == true)&&(pravy == true))||(i >= 10.99)){
        if (i >= 10.99){
          L = i;
          P = i;
        }
        levy = false;
        pravy = false;
        UtokDokonceny = true;
        //cas_utoku = cas;
        //datum_utoku = datum;
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
    for (byte i = 0; i < 3; i++){
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
    lcd.print(ID);
    lcd.setCursor(19,2);
    lcd.print(moznost);
    //lcd.setCursor(19,3);
    //lcd.print(UtokDokonceny);
  
   
  }

  void UtokSmazan(){
    lcd.clear();
    lcd.setCursor(2,1);
    lcd.print("Utok ID:");
    lcd.print(ID);
    /*if (ID != 1){
      SD.rmdir(ID);
      ID--;
    }*/
    lcd.print(" byl");
    lcd.setCursor(2,2);
    lcd.print("uspesne smazan");
    delay(2000);
  }

  void Odpocet(){
    lcd.clear();
    lcd.setCursor(0,3);
    lcd.print(">Zpet");
    minuty = priprava;
    do {
      lcd.setCursor(1,1);
      lcd.print(minuty);
      lcd.print(":");
      if (sekundy <= 9)
      {
        lcd.print("0");
      }
      lcd.print(sekundy);
      if (sekundy == 0 && minuty >= 1) {
        sekundy = 60;
        minuty--;
      }
      unsigned long currentMillis = millis();
      if (currentMillis - pomocna >= 1000) {
        pomocna = currentMillis;
        sekundy--;
      }
      
      if(digitalRead(2) == LOW){
        break;
      }
       
    } while ((minuty != 0)||(sekundy != 0));
  }

  /*char CtiSDkartu(char Zapis){
    myFile = SD.open(String(ID) + "/" + Zapis + ".txt");
    if (myFile) {
      for (byte data = 0; data < 5; data++){
        lcd.write(myFile.read());
      }
      myFile.close();
    } else {
      Serial.println("error opening  " + Zapis + ".txt");
    }
  }*/
