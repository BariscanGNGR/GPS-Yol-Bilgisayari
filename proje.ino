#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <dht11.h>
#include "U8glib.h"
//#include <EEPROM.h>
#include <EEPROMex.h>
#define DHT11PIN 2

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE | U8G_I2C_OPT_DEV_0);
dht11 DHT11;
int ekranSayaci = 0;

int kmh;
int uydu;
int sicaklik;
double toplamKm;
double tripMetre;
double mS;
double yakitTuketimi;
char buffer[10];
bool gecikme;
byte sayac = 0;
double tm =0;
  
/*
   This sample code demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;

// The TinyGPS++ object
TinyGPSPlus gps;

// The serial connection to the GPS device
SoftwareSerial ss(RXPin, TXPin);

void setup()
{
  Serial.begin(115200);
  ss.begin(GPSBaud);
  pinMode(13,OUTPUT);
  pinMode(12, INPUT_PULLUP);
  EEPROM.setMaxAllowedWrites(32000);
}
void loop()
{   
    toplamKm = EEPROM.readDouble(0);
    tripMetre = EEPROM.readDouble(4); 
    
    int chk = DHT11.read(DHT11PIN);
    sicaklik = (int)DHT11.temperature;

    kmh = (int)gps.speed.kmph();
    mS = (double)gps.speed.mps();
    uydu = (int)gps.satellites.value();

//sıfırlama tusu xd
   int buttonValue = digitalRead(12);
   if (buttonValue == LOW){
      //digitalWrite(13,HIGH);
      ekranSayaci++;
      sayac++;
      
      if(ekranSayaci >= 5)
          ekranSayaci=0;

      if(sayac == 6)
      {
        tripMetre = 0;
        ekranSayaci=5;
        //draw();
        sayac=0;
      }
   } else {
      //digitalWrite(13, LOW);
      sayac=0;
      if(ekranSayaci >= 5)
          ekranSayaci=0;
   }
  //yakit 
  double yT0= 9.6;
  double yT80 = 4.7;
  double yT200 = 16.3;
  double yT080 = 0.0612;
  double yT80200 = 0.0966;

  if(kmh == 0)
  {
    tm=0;
  }
  else if(kmh != 0 && kmh <= 80)
  {
      tm = yT0 - (kmh * yT080);
  }
  else if(kmh > 80 &&kmh <= 200 && kmh != 0)
  {
    tm = yT80 + ((kmh-80) * yT80200);
  }
    toplamKm = toplamKm + mS;
    tripMetre = tripMetre + mS;
// buraya kadar

   u8g.firstPage();
    do{
      draw();
    } while(u8g.nextPage());
    Serial.print("km : ");
    Serial.println(toplamKm);
    Serial.print("trip : ");
    Serial.println(tripMetre);
    Serial.print("Sicaklik : ");
    Serial.println(sicaklik);
    EEPROM.writeDouble(0, toplamKm);
    EEPROM.writeDouble(4, tripMetre);
    digitalWrite(13,HIGH);
    smartDelay(1000);
    digitalWrite(13,LOW);

  if (millis() > 5000 && gps.charsProcessed() < 10)
    {//Serial.println(F("No GPS data received: check wiring")); 
      }
}

void draw(void)
{
  int km;
  int trip;
  char tur[10];
  switch (ekranSayaci)
  {
    case 0://0 KMH
          u8g.setFont(u8g_font_fub25n);
          sprintf(buffer , "%3d" ,kmh);
          u8g.drawStr(30,40,buffer);

          u8g.setFont(u8g_font_fub14);
          u8g.drawStr(65,58,"km/h");
          sprintf(buffer,"%2d", uydu);
          u8g.drawStr(0,14,buffer);

          //debug
          //u8g.setPrintPos(30,70);
          //u8g.print(mS);
        break;
    case 1://yakit tüketimi
          u8g.setFont(u8g_font_fub20n);
          u8g.setPrintPos(30, 40);
          u8g.print(tm);
                    
          //sprintf(buffer , "%3f" ,tm);
          //u8g.drawStr(30,40,buffer);
          u8g.setFont(u8g_font_fub14);
          u8g.drawStr(0,58,"Yakit/100km");
          
          sprintf(buffer, "%2d" , uydu);
          u8g.drawStr(0,14,buffer);
          break;
     case 2:
          u8g.setFont(u8g_font_fub25n);
          if(toplamKm < 10000)
          {
            km = (int)toplamKm ;
            sprintf(tur,"Metre");
          }
          else if(toplamKm >= 10000)
           {
            km = (int)toplamKm/ 1000;
            sprintf(tur,"Kmetre");
           }
          sprintf(buffer , "%d" ,km);
          u8g.drawStr(30,40,buffer);
          u8g.setFont(u8g_font_fub14);
          u8g.drawStr(65,58,tur);

          sprintf(buffer, "%2d" , uydu);
          u8g.drawStr(0,14,buffer);
        break;
     case 3:
          u8g.setFont(u8g_font_fub25n);
          if(tripMetre < 10000)
          {
            trip = (int)tripMetre;
            sprintf(tur,"Trip-Metre");
          }
          else if(tripMetre >= 10000)
           {
            trip = (int)tripMetre  / 1000;
            sprintf(tur,"Trip-Kmetre");
           }
          sprintf(buffer , "%d" ,trip);
          u8g.drawStr(30,40,buffer);
          u8g.setFont(u8g_font_fub14);
          u8g.drawStr(65,58,tur);

          sprintf(buffer, "%2d" , uydu);
          u8g.drawStr(0,14,buffer);
        break;
        case 4:
          u8g.setFont(u8g_font_fub25n);
          sprintf(buffer , "%d" ,sicaklik);
          u8g.drawStr(30,40,buffer);
          u8g.setFont(u8g_font_fub14);
          u8g.drawStr(65,58,"derece");
          
          u8g.setFont(u8g_font_fub14);
          sprintf(buffer, "%2d" , uydu);
          u8g.drawStr(0,14,buffer);
        break;
        
        case 5:
          u8g.setFont(u8g_font_fub14);
          u8g.drawStr(0,40,"TMetre sifirlandi");
          break;
  }
}


static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do 
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}
