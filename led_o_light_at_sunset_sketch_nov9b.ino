#include <Wire.h>
#include <RTClib.h>
#include <TimeLib.h>

RTC_DS3231 rtc;
const int mosfetPin = 3; // MOSFET Gate ansluten till D3 (PWM-pinne)

bool ledOn = false;
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
      tm.Hour = 15;
      tm.Minute = 1;
      // Ednast för TEST, ta bort denna if-sats i prod
      if (day == 23) {
      tm.Hour = 17;
      tm.Minute = 14;
      }
      //
      
      break;
    case 12: // December
      tm.Hour = 14;
      tm.Minute = 37;
      break;
  }

  return makeTime(tm);
}

// Funktion för att kontrollera om det är sommartid (CEST, UTC+2)
bool isDaylightSavingTime(int year, int month, int day) {
  // Sommartid i Sverige: sista söndagen i mars till sista söndagen i oktober
  if (month < 3 || month > 10) return false;
  if (month > 3 && month < 10) return true;

  // Mars: sista söndagen
  if (month == 3) {
    int lastSunday = 31;
    while (dayOfTheWeek(year, month, lastSunday) != 0) { // 0 = söndag
      lastSunday--;
    }
    if (day >= lastSunday) return true;
  }
  // Oktober: sista söndagen
  else if (month == 10) {
    int lastSunday = 31;
    while (dayOfTheWeek(year, month, lastSunday) != 0) { // 0 = söndag
      lastSunday--;
    }
    if (day < lastSunday) return true;
  }

  return false;
}

// Funktion för att beräkna veckodag (0=söndag, 1=måndag, etc.)
int dayOfTheWeek(int y, int m, int d) {
  if (m < 3) {
    m += 12;
    y--;
  }
  int h = (d + 13*(m+1)/5 + 2*y + y/4 - y/100 + y/400) % 7;
  return (h + 6) % 7; // 0 = söndag, 1 = måndag, etc.
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

  // Hämta aktuell tid i lokal tid (CET/CEST)
  int currentYear = now.year();
  int currentMonth = now.month();
  int currentDay = now.day();
  int currentHour = now.hour();
  int currentMinute = now.minute();
  int currentSecond = now.second();

  // Kontrollera om det är sommartid
  bool isDST = isDaylightSavingTime(currentYear, currentMonth, currentDay);

  // Justera timmen för UTC (subtrahera 1 för CET, 2 för CEST)
  int utcHour = currentHour - (isDST ? 2 : 1);

  // Skapa en Unix-tid för aktuell tid i UTC
  tmElements_t tm;
  tm.Year = currentYear - 1970;
  tm.Month = currentMonth;
  tm.Day = currentDay;
  tm.Hour = utcHour;
  tm.Minute = currentMinute;
  tm.Second = currentSecond;
  time_t currentTime = makeTime(tm);

  // Beräkna solnedgångstiden för idag (UTC)
  time_t sunsetTime = calculateSunset(currentYear, currentMonth, currentDay);
  time_t targetTime = sunsetTime - 3600; // En timme innan solnedgång (UTC)

  // Skriv ut tiden och solnedgångstiden för debug
  Serial.print("Aktuell lokal tid: ");
  Serial.print(currentHour);
  Serial.print(":");
  Serial.print(currentMinute);
  Serial.print(":");
  Serial.print(currentSecond);
  Serial.print(" | Aktuell UTC-tid: ");
  Serial.print(hour(currentTime));
  Serial.print(":");
  Serial.print(minute(currentTime));
  Serial.print(" | Solnedgång idag (UTC): ");
  Serial.print(hour(sunsetTime));
  Serial.print(":");
  Serial.print(minute(sunsetTime));
  Serial.print(" | Mål för uppdimning (UTC): ");
  Serial.print(hour(targetTime));
  Serial.print(":");
  Serial.println(minute(targetTime));

  // Kontrollera om det är dags att börja dimma upp (en timme innan solnedgång, UTC)
  if (!dimmingUp && !ledOn && currentTime >= targetTime && currentTime < sunsetTime) {
    Serial.println("Börjar dimma upp LED-stripen...");

    dimmingUp = true;

    // Dimma upp från 0% till 100% under 30 sekunder
    for (int brightness = 0; brightness <= 255; brightness++) {
      analogWrite(mosfetPin, brightness);
      delay(14); // 255 steg * 14 ms ≈ 3.5 sekunder (för snabb test)
    }

    ledOn = true;
    dimmingUp = false;
    Serial.println("LED-stripen är fullt tänd.");
  }

  // Kontrollera om det är dags att dimma ner (klockan 22:05 UTC)
  if (ledOn && !dimmingDown && hour(currentTime) == 22 && minute(currentTime) == 5) {
  
    Serial.println("Börjar dimma ner LED-stripen...");

    dimmingDown = true;

    // Dimma ner från 100% till 0% under 30 sekunder
    for (int brightness = 255; brightness >= 0; brightness--) {
      analogWrite(mosfetPin, brightness);
      delay(14); // 255 steg * 14 ms ≈ 3.5 sekunder (för snabb test)
    }

    digitalWrite(mosfetPin, LOW); // Se till att MOSFET:n är helt avstängd
    ledOn = false;
    dimmingDown = false;
    Serial.println("LED-stripen är släckt.");
  }

  //  delay(60000); // Uppdatera varje minut för att minska belastningen
  delay(5000); // Uppdatera var 5:e sekund för att minska belastningen
}
