#include <Wire.h>
#include <RTClib.h>
#include <TimeLib.h>

RTC_DS3231 rtc;
const int mosfetPin = 3; // MOSFET Gate ansluten till D3 (PWM-pinne)

bool ledOn = false;
unsigned long ledOnTime = 0;
bool dimmingUp = false;
bool dimmingDown = false;

// Funktion för att hämta snitt-solnedgångstid för en given månad (UTC)
time_t calculateSunset(int year, int month, int day) {
  tmElements_t tm;
  tm.Year = year - 1970;
  tm.Month = month;
  tm.Day = day;
  tm.Second = 0;

  // Snitt-solnedgångstider för Malmö (UTC)
  switch (month) {
    case 1:  // Januari
      tm.Hour = 15;
      tm.Minute = 11;
      break;
    case 2:  // Februari
      tm.Hour = 16;
      tm.Minute = 10;
      break;
    case 3:  // Mars
      tm.Hour = 17;
      tm.Minute = 13;
      break;
    case 4:  // April
      tm.Hour = 18;
      tm.Minute = 13;
      break;
    case 5:  // Maj
      tm.Hour = 19;
      tm.Minute = 13;
      break;
    case 6:  // Juni
      tm.Hour = 19;
      tm.Minute = 49;
      break;
    case 7:  // Juli
      tm.Hour = 19;
      tm.Minute = 36;
      break;
    case 8:  // Augusti
      tm.Hour = 18;
      tm.Minute = 38;
      break;
    case 9:  // September
      tm.Hour = 17;
      tm.Minute = 22;
      break;
    case 10: // Oktober
      tm.Hour = 16;
      tm.Minute = 3;
      break;
    case 11: // November
      //tm.Hour = 15;
      //tm.Minute = 1;
      tm.Hour = 21;
      tm.Minute = 58;
      break;
    case 12: // December
      tm.Hour = 14;
      tm.Minute = 37;
      break;
  }

  return makeTime(tm);
}

void setup() {
  Serial.begin(9600);
  pinMode(mosfetPin, OUTPUT);
  digitalWrite(mosfetPin, LOW);

  if (!rtc.begin()) {
    Serial.println("Kunde inte hitta RTC-modul!");
    while (1);
  }
}

void loop() {
  DateTime now = rtc.now();

  // Hämta aktuell tid i UTC
  setTime(now.hour(), now.minute(), now.second(), now.day(), now.month(), now.year());
  time_t currentTime = now.unixtime();

  // Beräkna solnedgångstiden för idag
  time_t sunsetTime = calculateSunset(now.year(), now.month(), now.day());
  time_t targetTime = sunsetTime - 3600; // En timme innan solnedgång

  // Skriv ut tiden och solnedgångstiden för debug
  Serial.print("Aktuell tid: ");
  Serial.print(now.hour());
  Serial.print(":");
  Serial.print(now.minute());
  Serial.print(":");
  Serial.print(now.second());
  Serial.print(" | Solnedgång idag: ");
  Serial.print(hour(sunsetTime));
  Serial.print(":");
  Serial.print(minute(sunsetTime));
  Serial.print(" | Mål för uppdimning: ");
  Serial.print(hour(targetTime));
  Serial.print(":");
  Serial.println(minute(targetTime));

  // Kontrollera om det är dags att börja dimma upp (en timme innan solnedgång)
  if (!dimmingUp && !ledOn && now.unixtime() >= targetTime && now.unixtime() < sunsetTime) {
    Serial.println("Börjar dimma upp LED-stripen...");

    dimmingUp = true;

    // Dimma upp från 0% till 100% under 5 minuter
    for (int brightness = 0; brightness <= 255; brightness++) {
      analogWrite(mosfetPin, brightness);
 //     delay(114); // 255 steg * 114 ms ≈ 5 minuter
      delay(14); // 255 steg * 114 ms ≈ 5 minuter
    }

    ledOn = true;
    dimmingUp = false;
    Serial.println("LED-stripen är fullt tänd.");
  }

  // Kontrollera om det är dags att dimma ner (klockan 22:05 UTC)
//  if (ledOn && !dimmingDown && now.hour() == 22 && now.minute() == 5) {
    if (ledOn && !dimmingDown && now.hour() == 21 && now.minute() == 00) {
  
    Serial.println("Börjar dimma ner LED-stripen...");

    dimmingDown = true;

    // Dimma ner från 100% till 0% under 5 minuter
    for (int brightness = 255; brightness >= 0; brightness--) {
      analogWrite(mosfetPin, brightness);
     // delay(114); // 255 steg * 114 ms ≈ 5 minuter
      delay(14); // 255 steg * 114 ms ≈ 5 minuter
    }

    digitalWrite(mosfetPin, LOW); // Se till att MOSFET:n är helt avstängd
    ledOn = false;
    dimmingDown = false;
    Serial.println("LED-stripen är släckt.");
  }

  //delay(60000); // Uppdatera varje minut för att minska belastningen
  delay(5000); // Uppdatera varje minut för att minska belastningen
}
