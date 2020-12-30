/*
   *        indikace uspesne komunikace
   *        automaticke mazani pokud cas > 60s
   *        Pozarni utok 2 moznost 0
   * 
  */
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include "SdFat.h"
#include "RTClib.h"

LiquidCrystal_I2C lcd(0x27, 20, 4);
RTC_DS1307 rtc;
SoftwareSerial HC12(8, 9);

bool levy = false, pravy = false;
bool UtokDokonceny = false, manualne = false;
byte terc = 0, x = 0, predchoziMenu;
String cas, datum;
byte priprava = 5;
byte minuty = 0;
byte sekundy = 0;

//SD KARTA
File myFile;
SdFat SD;
byte ID = 1;
const byte ID_MAX = 255;
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

byte sipkaVpravo[] = {
    B00000,
    B00100,
    B00010,
    B11111,
    B00010,
    B00100,
    B00000,
    B00000};
byte sipkaVlevo[] = {
    B00000,
    B00100,
    B01000,
    B11111,
    B01000,
    B00100,
    B00000,
    B00000};
byte baterie[] = {
    B01110,
    B11011,
    B10001,
    B10001,
    B11111,
    B11111,
    B11111,
    B11111};
/*1-hlavni, 
   * 2-utok, 6-auto, 7-manualne, 8-casomira
   * 3-priprava, 9-odpocet
   * 4-historie, 10-
   * 5-nejrychlejsi sestriky, 
  */
void Menu();
void Sipka()
{
  lcd.clear();
  lcd.setCursor(x, moznost);
  lcd.print(">");
}
void VypisMenu();
void Odpocet();
void UtokSmazan();
void Automaticky();
void Casomira();
void CtiSD(char Zapis);
void ZapisSD(char Zapis, double Terc);
void Zapis_C_D(char Zapis, String hodnota);

void setup()
{
  Serial.begin(9600);
  //HC12.begin(9600);
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.createChar(0, sipkaVlevo);
  lcd.createChar(1, sipkaVpravo);
  lcd.createChar(2, baterie);

  if (!SD.begin(53))
  {
    lcd.print("Chybi SD karta");
    Serial.println("Chybi SD karta");
    delay(1000);
    return;
  }
  if (!rtc.begin())
  {
    lcd.println("chybi RTC");
    Serial.flush();
    abort();
  }
  if (!rtc.isrunning())
  {
    Serial.println("RTC nefakci, nastav cas!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  for (byte i = 0; i < 3; i++)
  {
    pinMode(pinyTlacitek[i], INPUT_PULLUP);
    digitalWrite(pinyTlacitek[i], HIGH);
  }
  lcd.clear();
  lcd.setCursor(1, 1);
  Serial.println("Bezdratova Pozarni");
  lcd.print("Bezdratova Pozarni");
  lcd.setCursor(5, 2);
  lcd.print("Casomira");
  delay(3000);
  for (int pomocnaID = 1; pomocnaID < ID_MAX; pomocnaID++)
  {
    if (SD.exists(String(pomocnaID)))
    {
      ID = pomocnaID;
    }
  }
  lcd.clear();
}

void loop()
{
  VypisMenu();
  for (byte i = 0; i < 3; i++)
  {
    byte reading = digitalRead(pinyTlacitek[i]);
    if (reading != stare[i])
    {
      debounce[i] = millis();
    }
    if ((millis() - debounce[i]) > Delay)
    {
      if (reading != nove[i])
      {
        nove[i] = reading;
        if (nove[i] == HIGH)
        {
          inputFlags[i] = HIGH;
        }
      }
    }
    stare[i] = reading;
  }
  for (byte i = 0; i < 3; i++)
  {
    if (inputFlags[i] == HIGH)
    {
      if (i == 0)
      { //SELECT
        Serial.println("SELECT");
        Serial.println(millis());
        switch (menu)
        {
        case 1: // Hlavni menu
        {
          if (millis() > 8000)
          {
            switch (moznost)
            {
            case 0:
            {
              menu = 2;
              moznost = 1;
            }
            break;
            case 1:
            {
              menu = 3;
              moznost = 1;
            }
            break;
            case 2:
            {
              menu = 4;
              moznost = 3;
            }
            break;
            case 3:
            {
              menu = 5;
              moznost = 1;
            }
            break;
            }
          }
        }
        break;
        case 2: // Pozarni utok
        {
          switch (moznost)
          {
          case 1:
          {
            menu = 6;
            moznost = 2;
          }
          break;
          case 2:
          {
            menu = 7;
            moznost = 2;
          }
          break;
          case 3:
          {
            menu = 1;
            moznost = 0;
          }
          break;
          }
        }
        break;
        case 3: // Odpocet pripravy
        {
          switch (moznost)
          {
          case 1:
          {
            if (priprava == 6)
            {
              priprava = 1;
            }
            else
            {
              priprava++;
            }
          }
          break;
          case 2:
          {
            Odpocet();
            minuty = priprava;
            sekundy = 0;
            menu = 1;
            moznost = 0;
          }
          break;
          case 3:
          {
            menu = 1;
            moznost = 1;
          }
          break;
          }
        }
        break;
        case 4: // Historie
        {
          switch (x)
          {
          case 0:
          {
            menu = 1;
            moznost = 2;
          }
          break;
          case 6:
          {
            if (ID != 1)
            {
              do
              {
                ID--;
              } while (!SD.exists(String(ID)));
            }
          }
          break;
          case 10:
          {
            if (SD.exists(String(ID + 1)))
            {
              ID++;
            }
            else
            {
              byte pomocnaID = ID;
              while (1)
              {
                ID++;
                Serial.println(ID);
                if (SD.exists(String(ID)))
                  break;
                if (ID == 20)
                {
                  ID = pomocnaID;
                  break;
                }
              }
            }
          }
          break;
          case 13:
          {
            predchoziMenu = menu;
            menu = 9; // Smazat
            moznost = 2;
            x = 4;
          }
          }
          break;
        }
        break;
        case 5: // Nejrychlejsi casy
        {
          switch (moznost)
          {
          case 3:
          {
            menu = 1;
            moznost = 3;
          }
          break;
          }
        }
        break;
        case 6: // Automaticky
        {
          switch (moznost)
          {
          case 2:
          {
            menu = 8;
            moznost = 3;
          }
          break;
          case 3:
          {
            menu = 2;
            moznost = 1;
          }
          break;
          }
        }
        break;
        case 7: // Manualne
        {
          switch (moznost)
          {
          case 2:
          {
            menu = 8;
            moznost = 3;
            manualne = 1;
          }
          break;
          case 3:
          {
            menu = 2;
            moznost = 2;
          }
          break;
          }
        }
        break;
        case 8: // Casomira
        {
          if (UtokDokonceny == true)
          {
            switch (x)
            {
            case 0:
            {
              if (manualne == 0)
              {
                menu = 6;
              }
              else
              {
                menu = 7;
              }
              moznost = 3;
            }
            break;
            case 13:
            {
              predchoziMenu = menu;
              menu = 9; // Smazat
              moznost = 2;
              x = 4;
            }
            break;
            }
          }
        }
        break;
        case 9: // Smazat
        {
          switch (x)
          {
          case 4:
          {
            UtokSmazan();
            if (predchoziMenu == 4)
            {
              menu = predchoziMenu;
              moznost = 3;
            }
            else if (predchoziMenu == 8)
            {
              menu = 2;
              moznost = 3;
            }
            x = 0;
          }
          break;
          case 11:
          {
            menu = predchoziMenu;
            moznost = 3;
            x = 0;
          }
          break;
          }
        }
        break;
        }
      }
      if (i == 1)
      { //UP
        switch (menu)
        {
        case 1:
        { // 4 Moznosti
          if (moznost == 0)
          {
            moznost = 3;
          }
          else
          {
            moznost--;
          }
        }
        break;
        case 2: // 3 Moznosti
        case 3:
        {
          if (moznost == 1)
          {
            moznost = 3;
          }
          else
          {
            moznost--;
          }
        }
        break;
        case 4:
        {
          if (x == 0)
          {
            x = 13;
          }
          else if (x == 6)
          {
            x = 0;
          }
          else if (x == 10)
          {
            x = 6;
          }
          else if (x == 13)
          {
            x = 10;
          }
        }
        break;
        case 5:
        {
          //Nejrychlejsi sestriky
        }
        break;
        case 6:
        case 7:
        {
          if (moznost == 2)
          {
            moznost = 3;
          }
          else
          {
            moznost--;
          }
        }
        break;
        case 8:
        {
          if (x == 13)
          {
            x = 0;
          }
        }
        break;
        case 9:
        {
          if (x == 11)
          {
            x = 4;
          }
        }
        break;
        }
      }
      if (i == 2)
      { //DOWN
        switch (menu)
        {
        case 1:
        { // 4 Moznosti
          if (moznost == 3)
          {
            moznost = 0;
          }
          else
          {
            moznost++;
          }
        }
        break;
        case 2: // 3 Moznosti
        case 3:
        {
          if (moznost == 3)
          {
            moznost = 1;
          }
          else
          {
            moznost++;
          }
        }
        break;
        case 4:
        {
          if (x == 13)
          {
            x = 0;
          }
          else if (x == 10)
          {
            x = 13;
          }
          else if (x == 6)
          {
            x = 10;
          }
          else if (x == 0)
          {
            x = 6;
          }
        }
        break;
        case 5:
        {
          //Nejrychlejsi sestriky
        }
        break;
        case 6:
        case 7:
        {
          if (moznost == 3)
          {
            moznost = 2;
          }
          else
          {
            moznost++;
          }
        }
        break;
        case 8:
        {
          if (x == 0)
          {
            x = 13;
          }
        }
        break;
        case 9:
        {
          if (x == 4)
          {
            x = 11;
          }
        }
        break;
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
  Serial.println("O co go");
  Serial.println(millis());
  switch (menu)
  {
  case 1: // Hlavni menu
  {

    lcd.setCursor(1, 0);
    lcd.print("Pozarni utok");
    lcd.setCursor(1, 1);
    lcd.print("Odpocet pripravy");
    lcd.setCursor(1, 2);
    lcd.print("Historie casu");
    lcd.setCursor(1, 3);
    lcd.print("Nejrychlejsi utoky");
  }
  break;
  case 2: //Pozarni utok
  {
    //while(HC12.available()){
    lcd.setCursor(0, 0);
    lcd.print("Pozarni utok");
    lcd.setCursor(1, 1);
    lcd.print("Automaticky");
    lcd.setCursor(1, 2);
    lcd.print("Manualne");
    lcd.setCursor(1, 3);
    lcd.print("Zpet");
    //}
    //lcd.clear();
    //lcd.setCursor(1,1);
    //lcd.print("Zapni terce");
  }
  break;
  case 3: //Priprava
  {

    lcd.setCursor(0, 0);
    lcd.print("Priprava zakladny");
    lcd.setCursor(1, 1);
    lcd.print(priprava);
    lcd.setCursor(3, 1);
    if ((priprava == 5) || (priprava == 6))
    {
      lcd.print("minut");
    }
    else if (priprava == 1)
    {
      lcd.print("minuta");
    }
    else
    {
      lcd.print("minuty");
    }
    lcd.setCursor(1, 2);
    lcd.print("Zahajit odpocet");
    lcd.setCursor(1, 3);
    lcd.print("Zpet");
  }
  break;
  case 4: //Historie
  {

    lcd.setCursor(0, 0);
    lcd.print("ID");
    lcd.print(ID);
    lcd.print(" ");
    CtiSD('C');
    lcd.print(" ");
    CtiSD('D');
    lcd.setCursor(0, 1);
    CtiSD('V');
    lcd.setCursor(7, 1);
    lcd.print("L:");
    CtiSD('L');
    lcd.setCursor(7, 2);
    lcd.print("P:");
    CtiSD('P');
    lcd.setCursor(1, 3);
    lcd.print("Zpet");
    lcd.setCursor(14, 3);
    lcd.print("Smazat");
    lcd.setCursor(7, 3);
    lcd.write(byte(0));
    lcd.setCursor(11, 3);
    lcd.write(byte(1));
  }
  break;
  case 5: //Nejrychlejsi sestriky
  {

    lcd.setCursor(0, 0);
    lcd.print("Nejrychlejsi utoky");
    lcd.setCursor(1, 3);
    lcd.print("Zpet");
  }
  break;
  case 6: //Automaticky
  {

    UtokDokonceny = false;
    lcd.setCursor(0, 0);
    lcd.print("Automaticky");
    lcd.setCursor(1, 2);
    lcd.print("Spustit");
    lcd.setCursor(1, 3);
    lcd.print("Zpet");
  }
  break;
  case 7: //Manualne
  {

    manualne = false;
    UtokDokonceny = false;
    lcd.setCursor(0, 0);
    lcd.print("Manualne");
    lcd.setCursor(1, 2);
    lcd.print("Spustit casomiru");
    lcd.setCursor(1, 3);
    lcd.print("Zpet");
    for (int pomocnaID = ID; pomocnaID < ID_MAX; pomocnaID++)
    {
      if (SD.exists(String(pomocnaID)))
      {
        ID = pomocnaID;
      }
    }
  }
  break;
  case 8: //Casomira
  {
    if (UtokDokonceny != true)
    {
      ID++;
      if (manualne != true)
      {
        Automaticky();
      }
      Casomira();
      lcd.setCursor(0, 3);
      lcd.print(">");
    }
    SD.mkdir(String(ID));
    lcd.setCursor(0, 0);
    lcd.print("ID");
    lcd.print(ID);
    lcd.print(" ");
    lcd.print(cas);
    Zapis_C_D('C', cas);
    lcd.print(" ");
    lcd.print(datum);
    Zapis_C_D('D', datum);
    lcd.setCursor(0, 1);
    lcd.print(i);
    ZapisSD('V', i);
    lcd.setCursor(8, 1);
    lcd.print("L: ");
    lcd.print(L);
    ZapisSD('L', L);
    lcd.setCursor(8, 2);
    lcd.print("P: ");
    lcd.print(P);
    ZapisSD('P', P);
    lcd.setCursor(1, 3);
    lcd.print("Zpet");
    lcd.setCursor(14, 3);
    lcd.print("Smazat");
  }
  break;
  case 9: //Smazat
  {

    lcd.setCursor(2, 1);
    lcd.print("Opravdu smazat?");
    lcd.setCursor(5, 2);
    lcd.print("Ano");
    lcd.setCursor(12, 2);
    lcd.print("Ne");
  }
  break;
  }
}

void Casomira()
{
  lcd.clear();
  lcd.print("ID");
  a = millis();
  i = 0;
  lcd.print(ID);
  DateTime now = rtc.now();
  lcd.setCursor(5, 0);
  lcd.print(now.hour(), DEC);
  lcd.print(":");
  if (now.minute() < 10)
    lcd.print("0");
  lcd.print(now.minute(), DEC);
  lcd.print(" ");
  lcd.print(now.day(), DEC);
  lcd.print("/");
  lcd.print(now.month(), DEC);
  lcd.print("/");
  lcd.print(now.year() - 2000, DEC);
  lcd.setCursor(0, 1);
  while (((levy != true) || (pravy != true)) && (i < 20.00))
  {

    lcd.setCursor(0, 1);
    c = millis();
    i = (c - a) / 1000;
    lcd.print(i);
    //Serial.println(HC12.read());
    if ((HC12.read() == 1) && (levy == false) && (i > 1))
    {
      lcd.setCursor(8, 1);
      lcd.print("L: ");
      lcd.print(i);
      L = i;
      levy = true;
      //digitalWrite(ledL, HIGH);
    }
    if ((HC12.read() == 2) && (pravy == false) && (i > 1))
    {
      lcd.setCursor(8, 2);
      lcd.print("P: ");
      P = i;
      lcd.print(i);
      pravy = true;
      //digitalWrite(ledP, HIGH);
    }
  }
  if (i >= 10.99)
  {
    L = 60.00;
    P = 60.00;
  }
  //
  levy = false;
  pravy = false;
  cas = String(now.hour()) + ":";
  if (now.minute() < 10)
    cas += "0";
  cas += String(now.minute());
  datum = String(now.day()) + "/" + String(now.month()) + "/" + String(now.year() - 2000);
  UtokDokonceny = true;
}

void Automaticky()
{
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Zavodnici na mista");
  for (int pomocnaID = ID; pomocnaID < ID_MAX; pomocnaID++)
  {
    if (SD.exists(String(pomocnaID)))
    {
      ID = pomocnaID;
    }
  }
  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Pripravte se");
  for (byte i = 0; i < 3; i++)
  {
    lcd.print(" .");
    delay(1000);
  }
  lcd.clear();
  lcd.setCursor(0, 1);
  lcd.print("Pozor!");
  delay(random(2000, 4000));
  //bzucak;
}

void VypisMenu()
{
  lcd.setCursor(19, 0);
  lcd.write(byte(2));
  if (ID > 9)
    lcd.setCursor(18, 1);
  else
    lcd.setCursor(19, 1);
  lcd.print(ID);
  lcd.setCursor(19, 2);
  lcd.print(moznost);
  lcd.setCursor(19, 3);
  lcd.print(UtokDokonceny);
}

void UtokSmazan()
{
  lcd.clear();
  lcd.setCursor(2, 1);
  lcd.print("Utok ID:");
  lcd.print(ID);
  lcd.print(" byl");
  lcd.setCursor(2, 2);
  lcd.print("uspesne smazan");
  if (ID != 1)
  {
    SD.remove(String(ID) + "/V.txt");
    SD.remove(String(ID) + "/L.txt");
    SD.remove(String(ID) + "/P.txt");
    SD.remove(String(ID) + "/C.txt");
    SD.remove(String(ID) + "/D.txt");
    SD.rmdir(String(ID));
    do
    {
      ID--;
    } while (!SD.exists(String(ID)));
  }

  delay(2000);
}

void Odpocet()
{
  lcd.clear();
  lcd.setCursor(0, 3);
  lcd.print(">Zpet");
  minuty = priprava;
  do
  {
    lcd.setCursor(1, 1);
    lcd.print(minuty);
    lcd.print(":");
    if (sekundy <= 9)
    {
      lcd.print("0");
    }
    lcd.print(sekundy);
    if (sekundy == 0 && minuty >= 1)
    {
      sekundy = 60;
      minuty--;
    }
    unsigned long Millis = millis();
    if (Millis - pomocna >= 1000)
    {
      pomocna = Millis;
      sekundy--;
    }

    byte reading = digitalRead(2);
    if (reading != stare[2])
    {
      debounce[2] = millis();
    }
    if ((millis() - debounce[2]) > Delay)
    {
      if (reading != nove[2])
      {
        nove[2] = reading;
        if (nove[2] == HIGH)
        {
          inputFlags[2] = HIGH;
        }
      }
    }
    stare[2] = reading;
    if (inputFlags[2] == HIGH)
      break;

  } while ((minuty != 0) || (sekundy != 0));
}

void CtiSD(char Zapis)
{
  Serial.println(ID);
  myFile = SD.open(String(ID) + "/" + String(Zapis) + ".txt");
  byte pocet;
  if (Zapis == 'D')
  {
    pocet = 8;
  }
  else
  {
    pocet = 5;
  }
  Serial.println(pocet);
  if (myFile)
  {
    for (byte data = 0; data < pocet; data++)
    {
      lcd.write(myFile.read());
    }
    myFile.close();
  }
  else
  {
    Serial.println("chyba otevreni " + String(Zapis) + ".txt");
  }
}

void ZapisSD(char Zapis, double Terc)
{
  myFile = SD.open(String(ID) + "/" + String(Zapis) + ".txt", FILE_WRITE);
  if (myFile)
  {
    myFile.print(Terc);
    myFile.close();
    Serial.println("writing done.");
  }
  else
  {
    Serial.println("error writing to " + String(Zapis) + ".txt");
  }
}

void Zapis_C_D(char Zapis, String hodnota)
{
  myFile = SD.open(String(ID) + "/" + String(Zapis) + ".txt", FILE_WRITE);
  if (myFile)
  {
    myFile.print(hodnota);
    myFile.close();
    Serial.println("writing done.");
  }
  else
  {
    Serial.println("error writing to " + String(Zapis) + ".txt");
  }
}
