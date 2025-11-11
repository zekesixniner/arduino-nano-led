#include <RTClib.h>

RTC_DS3231 rtc;
void setup() {
  if (!rtc.begin()) {
    Serial.println("Kunde inte hitta RTC-modul!");
    while (1);
  }

  // Ställ in RTC-tiden om den inte är satt (kommentera bort efter första användningen)
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));


// Ställ in RTC-tiden med en korrigering på +7 sekunder
  DateTime now = DateTime(F(__DATE__), F(__TIME__));
  rtc.adjust(now + TimeSpan(7)); // Lägg till 7 sekunder

  Serial.println("RTC-tiden är inställd med +7 sekunder korrigering.");


}

void loop() {

delay(80000);

}
