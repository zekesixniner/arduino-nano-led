#include <RTClib.h>
#include <TimeLib.h>

RTC_DS3231 rtc;

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
  if (!rtc.begin()) {
    Serial.println("Kunde inte hitta RTC-modul!");
    while (1);
  }


//Ställ in RTC-tiden om den inte är satt (kommentera bort efter första användningen)
/*
 rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
// Ställ in RTC-tiden med en korrigering på +7 sekunder
  DateTime now = DateTime(F(__DATE__), F(__TIME__));
 rtc.adjust(now + TimeSpan(7)); // Lägg till 7 sekunder
 Serial.println("RTC-tiden är inställd med +7 sekunder korrigering.");
*/

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
  
  // Skriv ut tiden och solnedgångstiden för debug
  Serial.print("Aktuell lokal tid: ");
  Serial.print(currentYear);
  Serial.print("-");
  Serial.print(currentMonth);
  Serial.print("-");
  Serial.print(currentDay);
  Serial.print(" ");

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
  Serial.println(minute(sunsetTime));



delay(1000);

}
