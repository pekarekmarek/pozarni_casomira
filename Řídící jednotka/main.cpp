/*
   *        Mazani primo po utoku -> deleteNode nefakci 
   *          ?? Sort pro IDcka?
   *        if jeden terc ok, druhy N
   *        indikace uspesne komunikace
   *        automaticke mazani pokud cas > 60s ?
   * d
   *      
   * 
  */
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include "SdFat.h"


#include "RTClib.h"

#define buzzer 12

LiquidCrystal_I2C lcd(0x27, 20, 4);
RTC_DS1307 rtc;
SoftwareSerial HC12(10,11); // TX - RX

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
byte NejvyssiID = 0;
/////////
unsigned long pomocna = 0;

const double CAS_MAX = 20.00;
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

struct node
{
  double data;
  struct node *nextPtr;
  struct node *prevPtr;
  byte ID_Ptr;
};

struct node *firstNode;
struct node *lastNode;
struct node *currentNode;
int numberOfNodes = 0;

void createLinkedList();
void AddtoList();
void sortLinkedList(int numberOfNodes);
void displayLinkedList();
void deleteNode();
void deleteFirstNode();
void deleteLastNode();

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
void najdiNejvyssiID();
void CtiSD(char Zapis);
double CtiV();
void ZapisSD(char Zapis, double Terc);
void Zapis_C_D(char Zapis, String hodnota);

void setup()
{
  Serial.begin(9600);
  HC12.begin(9600);
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.createChar(0, sipkaVlevo);
  lcd.createChar(1, sipkaVpravo);
  lcd.createChar(2, baterie);
  firstNode = NULL;
  lastNode = NULL;

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
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  for (byte i = 0; i < 3; i++)
  {
    pinMode(pinyTlacitek[i], INPUT_PULLUP);
    digitalWrite(pinyTlacitek[i], HIGH);
  }
  pinMode(buzzer, OUTPUT);
  lcd.clear();
  lcd.setCursor(1, 1);
  lcd.print("Bezdratova Pozarni");
  lcd.setCursor(5, 2);
  lcd.print("Casomira");
  delay(2000);
  createLinkedList();
  sortLinkedList(numberOfNodes);
  currentNode = firstNode;
  najdiNejvyssiID();
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
        switch (menu)
        {
        case 1: // Hlavni menu
        {
          
          if (millis() > 8000)
          {
            Serial.println(menu);
            switch (moznost)
            {
            case 0:
            {
              menu = 2;
              moznost = 1;
              ID = NejvyssiID;
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
              ID = NejvyssiID;
            }
            break;
            case 3:
            {
              sortLinkedList(numberOfNodes);
              currentNode = firstNode;
              //ID = NejvyssiID;
              ID = currentNode->ID_Ptr;
              menu = 5;
              moznost = 3;
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
            
            
            /*if (SD.exists(String(ID + 1)))
            {
              ID++;
            }
            else
            {*/
              if (ID != NejvyssiID){
                do {
                  ID++;
                } while (!SD.exists(String(ID)));
              }
              /*byte pomocnaID = ID;
              while (1)
              {
                ID++;
                Serial.println(ID);
                if (SD.exists(String(ID)))
                  break;
                if (ID >= ID_MAX)
                {
                  ID = pomocnaID;
                  break;
                }
              }*/
            //}
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
          switch (x)
          {
          case 0:
          {
            menu = 1;
            moznost = 3;
          }
          break;
          case 6:
          {
            if (currentNode->prevPtr != NULL){
              currentNode = currentNode->prevPtr;
            }
          }
          break;
          case 10:
          {
            if (currentNode->nextPtr != NULL){
              currentNode = currentNode->nextPtr;
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
            if (ID != 0){
              UtokSmazan();
            }
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
            else if (predchoziMenu == 5){
              menu = predchoziMenu;
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
        case 5:
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
        case 5:
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
  Serial.println(menu);
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
    if(HC12.available()){
    lcd.setCursor(0, 0);
    lcd.print("Pozarni utok");
    lcd.setCursor(1, 1);
    lcd.print("Automaticky");
    lcd.setCursor(1, 2);
    lcd.print("Manualne");
    lcd.setCursor(1, 3);
    lcd.print("Zpet");
    }
    else {
      lcd.clear();
      lcd.setCursor(1,1);
      lcd.print("Zapni terce");
    }
   
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
  case 5: //Nejrychlejsi sestriky
  {

    if (ID != 0) {
      if (menu == 5){
        ID = currentNode->ID_Ptr;
      }
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
    else {
      lcd.setCursor(3,1);
      lcd.print("Zadny zaznam");
      lcd.setCursor(4,2);
      lcd.print("na SD karte");
      lcd.setCursor(1, 3);
      lcd.print("Zpet");
    }

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
  }
  break;
  case 8: //Casomira
  {
    if (UtokDokonceny != true)
    {
      ID++;
      NejvyssiID++;
      if (manualne != true)
      {
        Automaticky();
      }
      Casomira();
      lcd.setCursor(0, 3);
      lcd.print(">");
      SD.mkdir(String(ID));
      Zapis_C_D('C', cas);
      Zapis_C_D('D', datum);
      ZapisSD('V', i);
      ZapisSD('L', L);
      ZapisSD('P', P);
      AddtoList();
      numberOfNodes++;
      sortLinkedList(numberOfNodes);
    }
    
    lcd.setCursor(0, 0);
    lcd.print("ID");
    lcd.print(ID);
    lcd.print(" ");
    lcd.print(cas);
    
    lcd.print(" ");
    lcd.print(datum);
    
    lcd.setCursor(0, 1);
    lcd.print(i);
    
    lcd.setCursor(8, 1);
    lcd.print("L: ");
    lcd.print(L);
    
    lcd.setCursor(8, 2);
    lcd.print("P: ");
    lcd.print(P);
    
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
  tone(buzzer, 2100, 500);
  lcd.clear();
  lcd.print("ID");
  a = millis();
  i = 0;
  lcd.print(ID);
  DateTime now = rtc.now();
  lcd.print(" ");
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
  while (((levy != true) || (pravy != true)) && (i < CAS_MAX))
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
      tone(buzzer, 2090, 500);
    }
    if ((HC12.read() == 2) && (pravy == false) && (i > 1))
    {
      lcd.setCursor(8, 2);
      lcd.print("P: ");
      P = i;
      lcd.print(i);
      pravy = true;
      tone(buzzer, 2090, 500);
    }
  }
  if (i >= CAS_MAX)
  {
    L = 60.00;
    P = 60.00;
  }
  //
  digitalWrite(buzzer, LOW);
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
  //lcd.setCursor(19, 3);
  //lcd.print(UtokDokonceny);
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
  if (ID != 0)
  {
    SD.remove(String(ID) + "/V.txt");
    SD.remove(String(ID) + "/L.txt");
    SD.remove(String(ID) + "/P.txt");
    SD.remove(String(ID) + "/C.txt");
    SD.remove(String(ID) + "/D.txt");
    SD.rmdir(String(ID));
    /*do
    {
      ID--;
    } while (!SD.exists(String(ID)));*/
    deleteNode();
    /*if (ID == NejvyssiID){
      currentNode = currentNode->prevPtr;
    }
    else {
      currentNode = currentNode->prevPtr;
    }*/
    //numberOfNodes--;
    najdiNejvyssiID();
    sortLinkedList(numberOfNodes);
    currentNode = firstNode;
    //ID = firstNode->ID_Ptr;

  }

  delay(1000);
}

void najdiNejvyssiID(){
  numberOfNodes = 0;
  for (int pomocnaID = 0; pomocnaID < ID_MAX; pomocnaID++)
  {
    if (SD.exists(String(pomocnaID)))
    {
      ID = pomocnaID;
      numberOfNodes++;
    }
    NejvyssiID = ID;
  }
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
  //Serial.println(ID);
  myFile = SD.open(String(ID) + "/" + String(Zapis) + ".txt");
  byte read;
  if (myFile)
  {
    read = myFile.read();
    while (read != 255){
      lcd.write(read);
      read = myFile.read();
      //Serial.print(read);
    }

    myFile.close();
  }
  else
  {
    Serial.println("chyba otevreni " + String(Zapis) + ".txt");
  }
}

double CtiV()
{
  //Serial.println(ID);
  double cas = 0;
  float x = 10.0;
  byte read;
  myFile = SD.open(String(ID) + "/V.txt");
  if (myFile)
  {
    for (byte data = 0; data < 5; data++)
    {
      read = myFile.read();
        switch (read)
        {
        case 48:
        {
          cas += x * 0;
        }
        break;
        case 49:
        {
          cas += x * 1;
        }
        break;
        case 50:
        {
          cas += x * 2;
        }
        break;
        case 51:
        {
          cas += x * 3;
        }
        break;
        case 52:
        {
          cas += x * 4;
        }
        break;
        case 53:
        {
          cas += x * 5;
        }
        break;
        case 54:
        {
          cas += x * 6;
        }
        break;
        case 55:
        {
          cas += x * 7;
        }
        break;
        case 56:
        {
          cas += x * 8;
        }
        break;
        case 57:
        {
          cas += x * 9;
        }
        break;
        }
        //Serial.println(read);
        if (read != 46)
          x = x / 10;
    }
      
    myFile.close();
  }
  else
  {
    Serial.println("chyba otevreni V.txt");
  }
  return cas;
}

void ZapisSD(char Zapis, double Terc)
{
  myFile = SD.open(String(ID) + "/" + String(Zapis) + ".txt", FILE_WRITE);
  if (myFile)
  {
    if(Terc < 10) myFile.print("0");
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

void createLinkedList()
{
  struct node *newNode;
  double nodeData;

  firstNode = (struct node *)malloc(sizeof(struct node));

  if (firstNode == NULL)
  {
    Serial.println("Memory cannot be allocated");
  }
  else
  {
    ID = 1;
    nodeData = CtiV();
    //Serial.println(nodeData);
    firstNode->data = nodeData;
    firstNode->prevPtr = NULL;
    firstNode->nextPtr = NULL;
    firstNode->ID_Ptr = ID;

    lastNode = firstNode;
      for (ID = 2; ID < ID_MAX; ID++)
      {
        if (SD.exists(String(ID)))
        {
          newNode = (struct node *)malloc(sizeof(struct node));
          if (newNode == NULL){
            Serial.println("Memory cannot be allocated");
          }
          else {
            nodeData = CtiV();
            //Serial.println(nodeData);
            newNode->data = nodeData;
            newNode->ID_Ptr = ID;
            newNode->nextPtr = NULL;
            newNode->prevPtr = NULL;

            newNode->prevPtr = lastNode;
            lastNode->nextPtr = newNode;

            lastNode = newNode;

          }  
        }
      }
  }
  Serial.println("List created.");
}

void AddtoList(){
  struct node *newNode;
  double nodeData;
  newNode = (struct node *)malloc(sizeof(struct node));
  if (newNode == NULL){
    Serial.println("Memory cannot be allocated");
  }
  else {
    nodeData = CtiV();
    Serial.println("Added to list:");
    Serial.println(nodeData);
    newNode->data = nodeData;
    newNode->ID_Ptr = ID;
    newNode->nextPtr = NULL;
    newNode->prevPtr = NULL;

    newNode->prevPtr = lastNode;
    lastNode->nextPtr = newNode;

    lastNode = newNode;


  } 
}

void sortLinkedList(int numberOfNodes)
{
  int nodeCtr;
  int ctr;
  double nodeDataCopy;
  byte nodeIDCopy;
  struct node *currentNode;
  struct node *nextNode;

  for (nodeCtr = numberOfNodes - 2; nodeCtr >= 0; nodeCtr--)
  {
    currentNode = firstNode;
    nextNode = currentNode->nextPtr;

    for (ctr = 0; ctr <= nodeCtr; ctr++)
    {
      if (currentNode->data > nextNode->data)
      {
        nodeDataCopy = currentNode->data;
        nodeIDCopy = currentNode->ID_Ptr;
        currentNode->data = nextNode->data;
        currentNode->ID_Ptr = nextNode->ID_Ptr;
        nextNode->data = nodeDataCopy;
        nextNode->ID_Ptr = nodeIDCopy;
      }

      currentNode = nextNode;
      nextNode = nextNode->nextPtr;
    }
  }
  Serial.println("List sorted.");
}

void deleteNode(){
  struct node *nodeToDelete;

  nodeToDelete = currentNode;

  if (nodeToDelete == firstNode){
    deleteFirstNode();
    Serial.println("Prvni zaznam vymazan.");
  }
  else if(nodeToDelete == lastNode){
    deleteLastNode();
    Serial.println("Posledni zaznam vymazan.");
  }
  else if (nodeToDelete != NULL){
    currentNode->prevPtr->nextPtr = currentNode->nextPtr;
    currentNode->nextPtr->prevPtr = currentNode->prevPtr;
    Serial.println("Zaznam " + String(currentNode->ID_Ptr) + " vymazan.");
    free(nodeToDelete);
  }
  else {
    Serial.println("Spatna pozice pro smazani.");
  }
}

void deleteFirstNode(){
  struct node *nodeToDelete;

  if (firstNode == NULL){
    Serial.println("Zadne data v listu na prvni pozici.");
  }
  else {
    nodeToDelete = firstNode;

    firstNode = firstNode->nextPtr;
    firstNode->prevPtr = NULL;
    
    free(nodeToDelete);
  }
}

void deleteLastNode(){
  struct node *nodeToDelete;

  if (lastNode == NULL){
    Serial.println("Zadne data v listu na posledni pozici.");
  }
  else {
    nodeToDelete = lastNode;

    lastNode = lastNode->prevPtr;
    lastNode->nextPtr = NULL;
    
    free(nodeToDelete);
  }
}
