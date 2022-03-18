/*
  Titre      : Communication I2C
  Auteur     : Crepin Vardin Fouelefack
  Date       : 17/03/2022
  Description: Affichage de l'heure et la date du RTC et de la pressin du BMP32 sur l'ecran I2C grace a la communication I2C
  Version    : 0.0.1
*/


#include <Adafruit_Sensor.h> 
#include <Adafruit_BMP280.h> 
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1306.h>  
#include "RTClib.h" 

#define SEALEVELPRESSURE_HPA (1013.25)

// Creation d'objetS
RTC_DS3231 rtc; 
Adafruit_BMP280 bmp; 
Adafruit_SSD1306 display = Adafruit_SSD1306(128, 64, &Wire); 

unsigned long delayTime;
char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday",
                             "Thursday", "Friday", "Saturday"};

void setup()
{

  Serial.begin(9600);
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3C (for the 128x64)
                                             // init done

// si adresse BMP ne correspond pas
  if (!bmp.begin(0x76))
  {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                     "try a different address!"));
    while (1)
      delay(10);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_FORCED,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

#ifndef ESP8266
  while (!Serial)
    ; // wait for serial port to connect. Needed for native USB
#endif

  if (!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    Serial.flush();
    while (1)
      delay(10);
  }

  if (rtc.lostPower())
  {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
}

void loop()
{
                   // Impression de l'heure et la date du jour 
  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[now.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(now.hour()+1, DEC);
  Serial.print(':');
  Serial.print(now.minute()+5, DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

// Repositionnement et nettoyage de l'ecran OLED
  display.setCursor(0, 0);
  display.clearDisplay();

  display.setTextColor(WHITE);
  display.setTextSize(1.9);
  display.setCursor(0, 1);
  display.print((String)"Date: ");
   display.print(now.year(), DEC);
  display.print('/');
  display.print(now.month(), DEC);
  display.print('/');
  display.print(now.day(), DEC);
  display.print("");
 
  display.setCursor(0, 16);
  display.print((String)"Heure: ");
  display.print(now.hour()+1, DEC);
  display.print(':');
  display.print(now.minute()+5, DEC);
  display.print(':');
  display.print(now.second(), DEC);
  display.display();

  delay(2000);

// Repositionnement et nettoyage de l'ecran OLED
   display.setCursor(0, 0);
  display.clearDisplay();

  // Affichage de la pression
  display.setTextColor(WHITE);
  display.setTextSize(1);                                                        
  display.setCursor(0, 40);                                                      
  display.print((String) "Pressure: " + (bmp.readPressure() / 100.0F) + " hpa"); 
  display.display();
  delay(2000);

}
