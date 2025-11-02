/*
Du vill att LED-lampan ska tändas både vid jämna timmar och jämna minuter (t.ex. 12:14) och vid udda timmar och udda minuter (t.ex. 13:13), och alltid släcka efter 25 sekunder. Här är den korrekta koden som uppfyller detta:

Uppdaterad kod för att tända LED-lampan vid både jämna timmar/jämna minuter och udda timmar/udda minuter

Nu är det **krystallklart**! Du vill att LED-lampan ska tändas **både vid jämna timmar och jämna minuter** (t.ex. 12:14) **och vid udda timmar och udda minuter** (t.ex. 13:13), och **alltid släcka efter 25 sekunder**. Här är den **korrekta koden** som uppfyller detta:

---

### **Uppdaterad kod för att tända LED-lampan vid både jämna timmar/jämna minuter och udda timmar/udda minuter:**



### **Förklaring av koden:**

1. **Kontrollera jämna timmar/jämna minuter och udda timmar/udda minuter:**
   - `evenHourAndMinute` kontrollerar om både timmen och minuten är jämna (`% 2 == 0`).
   - `oddHourAndMinute` kontrollerar om både timmen och minuten är udda (`% 2 != 0`).

2. **Tänd LED-lampan vid rätt tillfällen:**
   - Om antingen `evenHourAndMinute` eller `oddHourAndMinute` är sanna, och det är en **ny minut** sedan senaste kontrollen, tänds LED-lampan.

3. **Släck LED-lampan efter 25 sekunder:**
   - När LED-lampan tänds, sparas den aktuella tiden i `ledOnTime`.
   - Koden kontrollerar sedan om 25 sekunder har passerat genom att jämföra `currentTime.unixtime()` med `ledOnTime`.
   - Om 25 sekunder har passerat, släcks LED-lampan genom att sätta `mosfetPin` till `LOW`.

4. **Uppdatera senaste timmen och minuten:**
   - När LED-lampan tänds, uppdateras `lastHour` och `lastMinute` för att förhindra att lampan tänds igen förrän nästa giltiga tillfälle.

---

### **Exempel på när LED-lampan tänds:**
- **12:14** (jämn timme, jämn minut) → Tänds
- **13:13** (udda timme, udda minut) → Tänds
- **13:15** (udda timme, udda minut) → Tänds
- **17:37** (udda timme, udda minut) → Tänds
- **12:13** (jämn timme, udda minut) → Tänds **inte**
- **15:44** (udda timme, jämn minut) → Tänds **inte**

---

### **Förväntat resultat:**
- **LED-lampan tänds** vid både jämna timmar/jämna minuter och udda timmar/udda minuter.
- **LED-lampan släcks** efter **25 sekunder**.
- **LED-lampan tänds inte igen** förrän nästa giltiga tillfälle.
- **Serial Monitor** visar tiden och meddelanden om när LED-lampan tänds och släcks.

---


*/


#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;
const int mosfetPin = 3; // MOSFET Gate ansluten till D3

bool ledOn = false;
unsigned long ledOnTime = 0;
int lastHour = -1; // Spara den senaste timmen
int lastMinute = -1; // Spara den senaste minuten

void setup() {
  Serial.begin(9600);
  pinMode(mosfetPin, OUTPUT);
  digitalWrite(mosfetPin, LOW); // Se till att MOSFET:n är avstängd vid start
  

  if (!rtc.begin()) {
    Serial.println("Kunde inte hitta RTC-modul!");
    while (1);
  }

  Serial.println("Startup!");

  // Ställ in RTC-tiden om den inte är satt (kommentera bort efter första användningen)
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

/*/
// Ställ in RTC-tiden med en korrigering på +7 sekunder
  DateTime now = DateTime(F(__DATE__), F(__TIME__));
  rtc.adjust(now + TimeSpan(7)); // Lägg till 7 sekunder

  Serial.println("RTC-tiden är inställd med +7 sekunder korrigering.");
*/

}

void loop() {
  DateTime now = rtc.now();

  // Skriv ut tiden i Serial Monitor
  Serial.print(now.year(), DEC);
  Serial.print('-');
  Serial.print(now.month(), DEC);
  Serial.print('-');
  Serial.print(now.day(), DEC);
  Serial.print(' ');
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();

  // Kontrollera om nuvarande timme och minut är jämna eller udda på rätt sätt
  bool evenHourAndMinute = (now.hour() % 2 == 0) && (now.minute() % 2 == 0);
  bool oddHourAndMinute = (now.hour() % 2 != 0) && (now.minute() % 2 != 0);

  // Om jämn timme och jämn minut eller udda timme och udda minut, och det är en ny minut sedan senaste kontrollen
  if ((evenHourAndMinute || oddHourAndMinute) && (now.minute() != lastMinute || now.hour() != lastHour) && !ledOn) {
    digitalWrite(mosfetPin, HIGH); // Tänd LED-lampan
    ledOn = true;
    ledOnTime = now.unixtime(); // Spara tiden när LED-lampan tändes
    Serial.println("LED-lampa tändes!");
    lastHour = now.hour(); // Uppdatera senaste timmen
    lastMinute = now.minute(); // Uppdatera senaste minuten
  }

  // Om LED-lampan är tänd och 25 sekunder har passerat, släck den
  if (ledOn) {
    DateTime currentTime = rtc.now();
    if (currentTime.unixtime() - ledOnTime >= 55) {
      digitalWrite(mosfetPin, LOW); // Släck LED-lampan
      ledOn = false;
      Serial.println("LED-lampa släcktes efter 55 sekunder.");
    }
  }

  delay(800); // Uppdatera snabbt för att fånga 25-sekundersintervallet
}
