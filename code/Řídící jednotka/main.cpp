/*
  *        
  *        
  *     
  *        
*/
#include <Arduino.h>
#include <SoftwareSerial.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include "SdFat.h"
#include "RTClib.h"
#include <CSVFile.h>

#define buzzer 7
#define batterypin A1
#define nabijenipin 16

LiquidCrystal_I2C lcd(0x27, 20, 4);
RTC_DS1307 rtc;
SoftwareSerial HC12(10,11); // TX - RX

bool levy = false, pravy = false;
byte utok = 0;
bool UtokDokonceny = false, manualne = false;
byte terc = 0, x = 0, predchoziMenu;
String cas, datum;
byte priprava = 5;
byte minuty = 0;
byte sekundy = 0;
int stav;
int lastStav;
bool tercenabijeni = true;

//
byte pocetTeamu = 0;
String nazevteamu = "";
String team = "";
String team1 = "";
String team2 = "";
String team3 = "";

char znakyMale[2][20] = {{'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t'},
{'u','v','w','x','y','z','0','1','2','3','4','5','6','7','8','9','_'}};
char znakyVelke[2][20] = {{'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T'},
{'U','V','W','X','Y','Z','0','1','2','3','4','5','6','7','8','9','_'}};
bool malepismena = true;
//

//  SD KARTA
//File myFile;
SdFat SD;
File root;
CSVFile csv;

byte ID = 1;
#define ID_MAX 255
byte NejvyssiID = 0;
/////////
unsigned long pomocna = 0;
unsigned long pomocnanabijeniR = 0;
unsigned long pomocnanabijeniT = 0;
unsigned long pomocnaTerce = 0;

byte transmit = 0;
#define CAS_MAX 60.00
#define RF_DELAY 0.05
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

byte menu = 0;
byte moznost = 0;

byte baterkaR = 2;
byte baterkaT = 2;

byte sipkaNahoru[] = {
    B00000,
    B00100,
    B01110,
    B10101,
    B00100,
    B00100,
    B00100,
    B00000};
byte sipkaDolu[] = {
    B00000,
    B00100,
    B00100,
    B00100,
    B10101,
    B01110,
    B00100,
    B00000};
byte baterie100[] = {
    B01110,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111};
byte baterie80[] = {
    B01110,
    B11011,
    B10001,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111};
byte baterie60[] = {
    B01110,
    B11011,
    B10001,
    B10001,
    B11111,
    B11111,
    B11111,
    B11111};
byte baterie40[] = {
    B01110,
    B11011,
    B10001,
    B10001,
    B10001,
    B11111,
    B11111,
    B11111};
byte baterie20[] = {
    B01110,
    B11011,
    B10001,
    B10001,
    B10001,
    B10001,
    B11111,
    B11111};
byte baterieError[] = {
    B01110,
    B11011,
    B10101,
    B10101,
    B10101,
    B10001,
    B10101,
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
void sortNejrychlejsi(int numberOfNodes);
void sortID(int numberOfNodes);
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
void IndikaceBaterie();
void Odpocet();
void UtokSmazan();
void Automaticky();
void Casomira();
void najdiNejvyssiID();
void OpenCSV();
void VypisCSV();
double CtiV();
void CtiCSV(byte Field);
void ZapisCSV();
void SmazatZaznam();
void battery(char zarizeni);
void nabijeni(byte zarizeni);
void cursor();
void NacistTymy();
void radio();

void setup()
{
  Serial.begin(9600);
  HC12.begin(115200);
  lcd.init();
  lcd.clear();
  lcd.backlight();
  lcd.createChar(0, sipkaDolu);
  lcd.createChar(1, sipkaNahoru);
  lcd.createChar(6, baterie100);
  lcd.createChar(5, baterie80);
  lcd.createChar(4, baterie60);
  lcd.createChar(3, baterie40);
  lcd.createChar(2, baterie20);
  lcd.createChar(7, baterieError);
  firstNode = NULL;
  lastNode = NULL;
  lcd.clear();
  lcd.setCursor(1, 1);
  lcd.print("Bezdratova Pozarni");
  lcd.setCursor(5, 2);
  lcd.print("Casomira");
  delay(1500);
  if (!SD.begin(53))
  {
    lcd.clear();
    lcd.print("Chybi SD karta");
    Serial.println("Chybi SD karta");
    delay(1000);
  }
  if (!rtc.begin())
  {
    lcd.clear();
    lcd.print("chybi RTC");
    Serial.print("chybi RTC");
    delay(1000);
  }
  if (!rtc.isrunning())
  {
    lcd.clear();
    lcd.setCursor(2,2);
    lcd.print("RTC reset");
    Serial.println("RTC se resetl, nastav cas");
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    delay(1000);
  }
  for (byte i = 0; i < 3; i++)
  {
    pinMode(pinyTlacitek[i], INPUT_PULLUP);
    digitalWrite(pinyTlacitek[i], HIGH);
  }
  pinMode(buzzer, OUTPUT);
  pinMode(batterypin, INPUT);
  pinMode(nabijenipin, INPUT);
  NacistTymy();
  if (pocetTeamu == 0) menu = 1;
  /*createLinkedList();
  sortNejrychlejsi(numberOfNodes);
  currentNode = firstNode;
  najdiNejvyssiID();*/
  lcd.clear();
}

void loop()
{
  radio();
  IndikaceBaterie();
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
        case 0: // vyber teamu
        {
          
          if (millis() > 5000)
          {
            //Serial.println(menu);
            switch (moznost)
            {
            case 0:
            {
              team = team1;
            }
            break;
            case 1:
            {
              team = team2;
            }
            break;
            case 2:
            {
              team = team3;
            }
            break;
            case 3: //novy team
            {
              menu = 12;
              moznost = 3;
              team = "";
            }
            break;
            }
            if (moznost != 3) {
              menu = 1;
              moznost = 0;
              najdiNejvyssiID();
              //VypisCSV();
              createLinkedList();
              //sortNejrychlejsi(numberOfNodes);
              currentNode = firstNode;
            }
          }
        }
        break;
        case 1: // Hlavni menu
        {
          
          if (millis() > 5000)
          {
            //Serial.println(menu);
            switch (moznost)
            {
            case 0:
            {
              if (HC12.available()) {
                if (SD.begin(53)){
                  menu = 2;
                  moznost = 1;
                  ID = NejvyssiID;  
                } else {
                  menu = 11;
                  moznost = 3;
                }
              } else {
                menu = 10;
                moznost = 3;
              }
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
              sortID(numberOfNodes);
              currentNode = lastNode;
              ID = currentNode->ID_Ptr;
              menu = 4;
              moznost = 3;
            }
            break;
            case 3:
            {
              sortNejrychlejsi(numberOfNodes);
              currentNode = firstNode;
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
            //x = 0;
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
        case 10: // RF
        {
          if (HC12.available()){
            if (SD.begin(53)) {
              menu = 2;
              moznost = 1;
              ID = NejvyssiID;  
            } else {
              menu = 11;
              moznost = 3;
            }
          } else {
          menu = 1;
          moznost = 0;
          }
        }
        break;
        case 11: // SD
        {
          switch (x)
          {
          case 0:
          {
            menu = 2;
            moznost = 1;
            ID = NejvyssiID;
          }
          break;
          case 15:
          {
            menu = 1;
            moznost = 0;
            x = 0;
          }
          break;
          }
        }
        break;
        case 12: // Vytvorit team  11/13
        {
          if (moznost == 0 || moznost == 1){
            if (team.length() < 11) {
              if (malepismena) team += znakyMale[moznost][x];
              else team += znakyVelke[moznost][x];
            }  
              Serial.println(team.length()); 
          }
          else {
            switch (x)
            {
            case 0:
            {
              if (team.length() > 0) {
                menu = 0;
                moznost = pocetTeamu - 1; //
                OpenCSV();
                csv.close();
                NacistTymy();  
              }
            }
            break;
            case 11:
            {
              if (malepismena) malepismena = false;
              else malepismena = true;
            }
            break;
            case 13:
            {
              team.remove(team.length() - 1,1);
            }
            break;
            case 15:
            {
              menu = 0;
              moznost = 0;
              x = 0;
            }
            break;
            }
          } 
        }
        break;
        case 13: // Zmenit tym
        {
          switch (moznost)
          {
          case 0:
          {
            menu = 3;
            moznost = 1;
          }
          break;
          case 1:
          {
            sortID(numberOfNodes);
            currentNode = lastNode;
            ID = currentNode->ID_Ptr;
            menu = 4;
            moznost = 3;
          }
          break;
          case 2:
          {
            sortNejrychlejsi(numberOfNodes);
            currentNode = firstNode;
            ID = currentNode->ID_Ptr;
            menu = 5;
            moznost = 3;
          }
          break;
          case 3:
          {
            if (SD.begin(53)){
              menu = 0;
              moznost = 0;
            } 
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
        case 0:
        { 
          switch(pocetTeamu)
          {
            case 1:
            {
              if (moznost == 0) moznost = 3;
              else moznost = 0;
            }
            break;
            case 2:
            {
              if (moznost == 0) moznost = 3;
              else if (moznost == 1) moznost--;
              else moznost = 1;
            }
            break;
            case 3:
            {
              if (moznost == 0) moznost = 3;
              else moznost--;
            }
            break;
          }
        }
        break;
        case 1:
        case 13:
        {
          if (moznost == 0){
            if (menu == 1) {
              if (SD.begin(53)) menu = 13;
              moznost = 3;
            } else {
              menu = 1;
              moznost = 0;
            }
          } else moznost--;
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
          if (numberOfNodes != 0){
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
          if (numberOfNodes != 0){
            if (x == 13)
            {
              x = 0;
            } else x = 13;
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
        case 11:
        {
          if (x == 15)
          {
            x = 0;
          }
        }
        break;
        case 12:
        { // Vyber tymu
          if (moznost == 0){
            if (x == 19){
              moznost = 1;
              x = 0;
            } else x++;
          }
          else if (moznost == 1) {
            if (x == 16) {
              moznost = 2;
              x = 11;
            } else x++;
          }
          else if (moznost == 2) {
            if (x == 11) {
              x = 13;
            } else {
              moznost = 3;
              x = 0;
            }
          }
          else if (moznost == 3) {
            if (x == 0)
            {
              x = 15;
            }
            else
            {
              moznost = 0;
              x = 0;
            }
          }
        }
        break;
        }
      }
      if (i == 2)
      { //DOWN
        switch (menu)
        {
        case 0:
        { 
          switch(pocetTeamu)
          {
            case 1:
            {
              if (moznost == 0) moznost = 3;
              else moznost = 0;
            }
            break;
            case 2:
            {
              if (moznost == 3) moznost = 0;
              else if (moznost == 0) moznost++;
              else moznost = 3;
            }
            break;
            case 3:
            {
              if (moznost == 3) moznost = 0;
              else moznost++;
            }
            break;
          }
        }
        break;
        case 1:
        case 13:
        {
          if (moznost == 3){
            if (menu == 1) {
              if (SD.begin(53)) {
                menu = 13;
                moznost = 3;
              } else moznost = 0;
            } else {
              menu = 1;
              moznost = 0;
            }
          } else moznost++;
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
          if (numberOfNodes != 0){
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
          if (numberOfNodes != 0){
            if (x == 0)
            {
              x = 13;
            } else x = 0;
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
        case 11:
        {
          if (x == 0)
          {
            x = 15;
          }
        }
        break;
        case 12:
        { //Vyber tymu
         if (moznost == 0){
            if (x == 0){
              moznost = 3;
              x = 15;
            } else x--;
          }
          else if (moznost == 1) {
            if (x == 0) {
              moznost = 0;
              x = 19;
            } else x--;
          }
          else if (moznost == 2) {
            if (x == 11) {
              moznost = 1;
              x = 16;
            } else x = 11;
          }
          else if (moznost == 3) {
            if (x == 0)
            {
              moznost = 2;
              x = 13;
            }
            else
            {
              x = 0;
            }
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
  Serial.print("Menu: ");Serial.println(menu);
  switch (menu)
  {
  case 0: // Vyber tymu
  {

    lcd.setCursor(1, 0);
    lcd.print(team1);
    if (pocetTeamu > 1) {
      lcd.setCursor(1, 1);
      lcd.print(team2);
      if (pocetTeamu == 3){
        lcd.setCursor(1, 2);
        lcd.print(team3);
      }
    }
    lcd.setCursor(1,3);
    lcd.print("Novy tym");
    /*lcd.home();
    lcd.print("Vyber teamu:");
    for (byte i = 1; i <= pocetTeamu; i++){
      lcd.setCursor(1, i);
      lcd.print("Team" + String(i));
    }*/
  }
  break;
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
    lcd.setCursor(0, 0);
    lcd.print("Pozarni utok");
    lcd.setCursor(1, 1);
    lcd.print("Automaticky");
    lcd.setCursor(1, 2);
    lcd.print("Manualne");
    lcd.setCursor(1, 3);
    lcd.print("Zpet");
    Serial.print("trasmit: ");Serial.println(transmit);
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

    if (numberOfNodes != 0) {
      ID = currentNode->ID_Ptr;
      lcd.setCursor(0, 0);
      lcd.print("ID");
      lcd.print(ID);
      lcd.print(" ");
      CtiCSV(5);
      lcd.print(" ");
      CtiCSV(4);
      lcd.setCursor(0, 1);
      CtiCSV(1);
      lcd.setCursor(7, 1);
      lcd.print("L:");
      CtiCSV(2);
      lcd.setCursor(7, 2);
      lcd.print("P:");
      CtiCSV(3);
      lcd.setCursor(1, 3);
      lcd.print("Zpet");
      lcd.setCursor(14, 3);
      lcd.print("Smazat");
      lcd.setCursor(7, 3);
      //lcd.write(byte(0));
      lcd.write(127);
      lcd.setCursor(11, 3);
      lcd.write(126);
      //lcd.write(byte(1));
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
    Serial.print("trasmit: ");Serial.println(transmit);
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
    Serial.print("trasmit: ");Serial.println(transmit);
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
      if (SD.begin(53)){
        ZapisCSV();
        najdiNejvyssiID();
        if (numberOfNodes == 1) createLinkedList();
        else AddtoList();
        sortID(numberOfNodes);
        currentNode = lastNode;
        ID = currentNode->ID_Ptr;
        VypisCSV();
      }
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
    if (numberOfNodes != 0) lcd.print("Smazat");
    
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
  case 10: //RF
  {
    lcd.setCursor(0, 1);
    lcd.print("Zapni terce");
    lcd.setCursor(1, 3);
    lcd.print("Ok");
  }
  break;
  case 11: //Sd karta
  {
    lcd.setCursor(0, 0);
    lcd.print("Chybi SD karta");
    lcd.setCursor(0,1);
    lcd.print("Utok nebude");
    lcd.setCursor(0,2);
    lcd.print("zaznamenavan");
    lcd.setCursor(1,3);
    lcd.print("Pokracovat");
    lcd.setCursor(16,3);
    lcd.print("Zpet");
  }
  break;
  case 12: // novy tym
  {

    lcd.home();
    //lcd.cursor_off();
    lcd.noBlink();
    for (byte i = 0; i < 2; i++){
      for (byte j = 0; j < 20; j++){
        if (i != 1 || j <= 16) {
          if (malepismena) lcd.print(znakyMale[i][j]);
          else lcd.print(znakyVelke[i][j]);
        }
      }
      lcd.setCursor(0,1);
    }
    lcd.setCursor(0, 2);
    lcd.print(team);
    lcd.print("_");
    lcd.setCursor(12,2);
    if (malepismena) lcd.write(byte(1));
    else lcd.write(byte(0));
    lcd.setCursor(14,2);
    lcd.print("Smazat");
    lcd.setCursor(1, 3);
    lcd.print("Vytvorit");
    lcd.setCursor(16,3);
    lcd.print("Zpet");
    if (moznost != 3 && moznost != 2) {
      lcd.setCursor(x, moznost);
      lcd.blink();
    }

  }
  break;
  case 13: //Zmenit tym
  {
    lcd.setCursor(1, 0);
    lcd.print("Odpocet pripravy");
    lcd.setCursor(1,1);
    lcd.print("Historie casu");
    lcd.setCursor(1,2);
    lcd.print("Nejrychlejsi utoky");
    lcd.setCursor(1,3);
    lcd.print("Zmenit tym");
  }
  break;
  }
}

void Casomira()
{
  //tone(buzzer, 2080, 500);
  digitalWrite(buzzer, HIGH);
  delay(200);
  digitalWrite(buzzer, LOW);
  lcd.clear();
  lcd.print("ID");
  a = millis();
  i = 0;
  lcd.print(ID);
  L = 0;
  P = 0;
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
    Serial.println(HC12.read());
    if ((HC12.read() == 11) && (levy == false) && (i > 1))
    {
      lcd.setCursor(8, 1);
      lcd.print("L: ");
      L = i - RF_DELAY;
      lcd.print(L);
      levy = true;
    }
    if ((HC12.read() == 66) && (pravy == false) && (i > 1))
    {
      lcd.setCursor(8, 2);
      lcd.print("P: ");
      P = i - RF_DELAY;
      lcd.print(P);
      pravy = true;
    }
  }
  if (i >= CAS_MAX)
  {
    if (L == 0) L = CAS_MAX;
    if (P == 0) P = CAS_MAX;
    i = CAS_MAX;
  } else {
    i -= RF_DELAY;
  }
  lcd.setCursor(0,1);
  lcd.print(i);
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

void OpenCSV(){
  String teamcsv = String(team) + ".csv";
  char FILENAME[15];
  teamcsv.toCharArray(FILENAME,15);

  if (!csv.open(FILENAME, O_RDWR | O_CREAT)){
    Serial.println("Chyba otevreni " + String(team) + ".csv");
  }
}

void UtokSmazan()
{
  lcd.clear();
  lcd.setCursor(3, 1);
  lcd.print("Utok ID:");
  lcd.print(ID);
  lcd.print(" byl");
  lcd.setCursor(3, 2);
  lcd.print("uspesne smazan");
  if (numberOfNodes != 0)
  {
    SmazatZaznam();;
    deleteNode();
    najdiNejvyssiID();
    if (predchoziMenu == 5) {
      sortNejrychlejsi(numberOfNodes);
      currentNode = firstNode;
    }
    else {
      sortID(numberOfNodes);
      currentNode = lastNode;
    }
  }
  delay(1000);
}

void najdiNejvyssiID(){
  numberOfNodes = 0;
  ID = 0;

  OpenCSV();
  
  const byte BUFFER_SIZE = 5;
  char buffer[BUFFER_SIZE + 1];
  buffer[BUFFER_SIZE] = '\0';

  int numBuffer = 0;
    csv.gotoBeginOfFile();
    do {
      csv.seekCur(2);
      csv.readField(numBuffer, buffer, BUFFER_SIZE);
      //Serial.println(numBuffer);
      if (numBuffer > ID) {
        ID = numBuffer;
        numberOfNodes++;
      }
    } while(csv.nextLine());
    Serial.print("Number of nodes: ");
    Serial.println(numberOfNodes);
    Serial.print("Nejvyssi ID: ");
    Serial.println(ID);
    NejvyssiID = ID;
    
    csv.close();
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
  if (minuty == 0 && sekundy == 0){
    digitalWrite(buzzer, HIGH);
    delay(200);
    digitalWrite(buzzer, LOW);
  }
}

void VypisCSV(){
  OpenCSV();

  const byte BUFFER_SIZE = 8;
  char buffer[BUFFER_SIZE + 1];
  buffer[BUFFER_SIZE] = '\0';

  csv.gotoBeginOfFile();
  do {
    csv.readField(buffer, BUFFER_SIZE);
    Serial.print(buffer); Serial.print(',');
    csv.nextField();
    if (csv.isEndOfLine()) {
      csv.nextLine();
      Serial.print("\n");
    }
  } while (!csv.isEndOfFile());
  csv.close();
}

void CtiCSV(byte Field)
{
  OpenCSV();

  String IDstring = "ID" + String(ID);
  char IDcsv[6];
  IDstring.toCharArray(IDcsv, 6);
  csv.gotoBeginOfFile();
  csv.gotoLine(IDcsv);

  const byte BUFFER_SIZE = 8;
  char buffer[BUFFER_SIZE + 1];
  buffer[BUFFER_SIZE] = '\0';

  for (byte i = 0; i < Field; i++){
    csv.nextField();
  }

  csv.readField(buffer, BUFFER_SIZE);
  lcd.print(buffer);
  //Serial.println(buffer);
  
  csv.close();
}

double CtiV()
{
  csv.gotoBeginOfFile();
  String IDstring = "ID" + String(ID);
  char IDcsv[6];
  IDstring.toCharArray(IDcsv, 6);
  csv.gotoLine(IDcsv);
  const byte BUFFER_SIZE = 8;
  char buffer[BUFFER_SIZE + 1];
  buffer[BUFFER_SIZE] = '\0';

  csv.nextField();
  
  csv.readField(buffer, BUFFER_SIZE); 
  String stringBuffer = buffer;

  double cas = stringBuffer.toDouble();
  return cas;
}

void ZapisCSV()
{
  OpenCSV();
  String dataString = "";
  char data[50];
  if (numberOfNodes != 0){
    do {
      csv.nextLine();
    } while (csv.nextLine());
    csv.addLine();
  }
  dataString = "ID" + String(ID) + ",";
  if (i < 10) dataString += "0";
  dataString += String(i) + ",";
  if (L < 10) dataString += "0";
  dataString += String(L) + ",";
  if (P < 10) dataString += "0";
  dataString += String(P) + "," + String(datum) + "," + String(cas);
  dataString.toCharArray(data, 50);

  csv.write(data);
  csv.close();
}

void SmazatZaznam(){
  OpenCSV();
  String IDstring = "ID" + String(ID);
  char IDcsv[6];
  IDstring.toCharArray(IDcsv, 6);
  csv.gotoBeginOfFile();
  csv.gotoLine(IDcsv);
  csv.markLineAsDelete();
  Serial.print("Smazan ");
  Serial.println(IDcsv);
  csv.close();
  VypisCSV();
}

void createLinkedList()
{
  if (numberOfNodes != 0) {
    struct node *newNode;
    double nodeData;

    OpenCSV();
    
    const byte BUFFER_SIZE = 5;
    char buffer[BUFFER_SIZE + 1];
    buffer[BUFFER_SIZE] = '\0';
    int numBuffer = 0;
    int lastNumBuffer = 0;

    firstNode = (struct node *)malloc(sizeof(struct node));

    if (firstNode == NULL)
    {
      Serial.println("Zadny zaznam");
    }
    else
    {
      csv.gotoBeginOfFile();
      do {
        csv.seekCur(2);
        csv.readField(numBuffer, buffer, BUFFER_SIZE);
        if (numBuffer == 0) csv.nextLine();
      } while (numBuffer == 0);
      ID = numBuffer;
      lastNumBuffer = numBuffer;
      //Serial.print(numBuffer);
      //Serial.print("-");
      nodeData = CtiV();
      //Serial.println(nodeData);
      firstNode->data = nodeData;
      firstNode->prevPtr = NULL;
      firstNode->nextPtr = NULL;
      firstNode->ID_Ptr = ID;

      lastNode = firstNode;
      if (numberOfNodes > 1 ){
        do {
          csv.gotoBeginOfFile();
          do {
            csv.nextLine();
            csv.seekCur(2);
            csv.readField(numBuffer, buffer, BUFFER_SIZE);
          } while (numBuffer <= lastNumBuffer);
          //Serial.print(numBuffer);
          //Serial.print("-");
          ID = numBuffer;
          lastNumBuffer = numBuffer;
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
        } while(csv.nextLine());
      }
    }
    Serial.println("List created.");
    csv.close();
    //VypisCSV();
  }
}

void AddtoList(){
  struct node *newNode;
  double nodeData;
  newNode = (struct node *)malloc(sizeof(struct node));
  if (newNode == NULL){
    Serial.println("Memory cannot be allocated");
  }
  else {
    OpenCSV();
    nodeData = CtiV();
    csv.close();
    Serial.print("Added to list: ");
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

void sortNejrychlejsi(int numberOfNodes)
{
  if (numberOfNodes > 1){
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
    Serial.println("List sorted - Nejrychlejsi.");
  } 
}

void sortID(int numberOfNodes)
{
  if (numberOfNodes > 1) {
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
        if (currentNode->ID_Ptr > nextNode->ID_Ptr)
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
    Serial.println("List sorted - ID");
  }
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

void IndikaceBaterie()
{
  if (menu != 12 && menu != 6 && menu != 7 && menu != 8) {
    lcd.setCursor(18,0); lcd.print("R");
    if (digitalRead(nabijenipin) == HIGH) {
      if (millis() - pomocnanabijeniR >= 250)
      {
        lcd.setCursor(19,0);
        lcd.write(byte(baterkaR));
        pomocnanabijeniR = millis();
        if (baterkaR == 6) baterkaR = 2;
        else baterkaR++;
      }
    } else {
      stav = analogRead(batterypin);
      if (stav >= 820)  stav = 6;
      else if (stav >= 740 && stav < 820) stav = 5; 
      else if (stav >= 700 && stav < 740) stav = 4;
      else if (stav >= 640 && stav < 700) stav = 3;
      else if (stav >= 600 && stav < 640) stav = 2;
      else stav = 7;
      lcd.write(byte(stav));
    }
  }

    if (menu != 12 && menu != 6 && menu != 7 && menu != 8) {lcd.setCursor(18,1); lcd.print("T");}
    if (HC12.available() || !tercenabijeni) {
      pomocnaTerce = millis();
      stav = HC12.read();
      Serial.println(stav);
        if (stav == 8 || !tercenabijeni) {
          if (millis() - pomocnanabijeniT >= 250)
          {
            if (menu != 12 && menu != 6 && menu != 7 && menu != 8) {lcd.setCursor(19,1);
            lcd.write(byte(baterkaT));}
            pomocnanabijeniT = millis();
            if (baterkaT == 6) {baterkaT = 2; tercenabijeni = true;}
            else {baterkaT++; tercenabijeni = false;}
            //Serial.println(baterkaT);
          }
        } else if (stav > 1 && stav < 7 ) {
          if (menu != 12 && menu != 6 && menu != 7 && menu != 8) lcd.write(byte(stav));
        }
       
    } else if (menu != 12 && menu != 6 && menu != 7 && menu != 8) {
      if (millis() - pomocnaTerce >= 500){
        lcd.print("X");
      }
    }
}



/*void battery(char zarizeni){
  if (zarizeni == 'R') {
    stav = analogRead(batterypin);
    if (stav >= 820)  stav = 6;
    else if (stav >= 740 && stav < 820) stav = 5; 
    else if (stav >= 700 && stav < 740) stav = 4;
    else if (stav >= 640 && stav < 700) stav = 3;
    else if (stav >= 600 && stav < 640) stav = 2;
    else stav = 7;
    if (stav > 1 && stav < 8) {
      if (menu != 6 && menu != 7 && menu != 8)
        lcd.write(byte(stav));
    }
  }
  else if (zarizeni == 'T') {
      stav = HC12.read();
      Serial.println(stav);
      if (stav == 8 || !tercenabijeni) nabijeni(1);
      else if (stav > 1 && stav < 8) {
        if (menu != 6 && menu != 7 && menu != 8){
          //lcd.write(byte(stav));
          lcd.write(byte(stav));
        }
          
          
    }
  }
   4,2V -> 3,2V
  5V....1024
  4,2V...860
  4V.....820  100%
  3,8V...780  80%
  3,6V...740  60%
  3,4V...700  40% 
  3,2V...660  20%
  
}*/

/*void nabijeni(byte zarizeni){
  if (millis() - pomocnanabijeni >= 250)
  {
    lcd.setCursor(19,zarizeni);
    if (menu != 6 && menu != 7 && menu != 8) lcd.write(byte(cyklusBaterie));
    pomocnanabijeni = millis();
    if (cyklusBaterie == 6) {cyklusBaterie = 2; tercenabijeni = true;}
    else {cyklusBaterie++; tercenabijeni = false;}
  }
}*/

void NacistTymy() {
  root = SD.open("/");
  char FileName[20];
  pocetTeamu = 0;
  String nazevteamu = "";
  while (true) {
    File entry =  root.openNextFile();
    if (! entry) {
      break;
    }
    if (entry.isFile()) {
      entry.getName(FileName, 20);
      if ((strstr(FileName, ".csv") != NULL) && pocetTeamu < 3){
        //Serial.println(FileName);
        pocetTeamu++;
        nazevteamu = FileName;
        nazevteamu.remove(nazevteamu.length() - 4,4);
        //Serial.println(nazevteamu);
        if (pocetTeamu == 1) {
          team1 = nazevteamu;
          Serial.print("Team1: ");Serial.println(team1);
        }
        else if (pocetTeamu == 2) {
          team2 = nazevteamu;
          Serial.print("Team2: ");Serial.println(team2);
        }
        else {
          team3 = nazevteamu;
          Serial.print("Team3: ");Serial.println(team3);
        }
      }
      
    }
    entry.close();
  }
  Serial.print("Pocet teamu: ");Serial.println(pocetTeamu);
}

void radio(){
  if (!HC12.available()){
    switch (menu) {
      case 0:
      case 1:
      case 2:
      case 3:
      case 4:
      case 5:
      case 9:
      case 10:
      case 11:
      case 12:
      {
        transmit = 0;
        for (byte i = 0; i < 5; i++){
          HC12.write(transmit);
        }
      } break;
      case 6:
      case 7: {
        transmit = 1;
        for (byte i = 0; i < 5; i++){
          HC12.write(transmit);
        }
      } break;
    }
  }
}