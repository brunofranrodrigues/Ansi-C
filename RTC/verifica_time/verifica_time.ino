#include <DS3231.h>

// Rele de acionamento da bomba de irrigação
#define pin_rele_rain 10

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

Time t;
static byte estadoSTR1 = 1;

static byte estadoOn = 1;
static byte estadoOff = 1;

const int OnHour = 18;
const int OnMin = 20;

const int OffHour = 18;
const int OffMin = 40;

long OnTime = 0;           // milliseconds of on-time

long OffTime = 0;          // milliseconds of off-time

int state_reles_rain = LOW;

unsigned long startMillis = 0;

unsigned long currentMillis = 0;

unsigned long previousMillis = 0;


void setup() {
  Serial.begin(9600);
  rtc.begin();
  pinMode (pin_rele_rain, OUTPUT);
}

void loop() {

  currentMillis = millis();

  t = rtc.getTime();

  if ((t.hour >= OnHour && t.min >= OnMin) && (t.hour <= OffHour && t.min <= OffMin)) {

    OnTime = (3 * 60000);
    OffTime = (3 * 60000);

    Serial.println("Hora magica");
    Serial.println(OnTime);
    Serial.println(OffTime);

  } else {

    OnTime = (estadoOn * 60000);
    OffTime = (estadoOff * 60000);

    Serial.println("Normal");
    Serial.println(OnTime);
    Serial.println(OffTime);

  }

  switch (estadoSTR1)
  {
    case 0x01:
      if (estadoSTR1 == 1 && startMillis == 0) {
        startMillis = currentMillis;
        state_reles_rain = HIGH;
        digitalWrite(pin_rele_rain, state_reles_rain);
      }

      if ((state_reles_rain == LOW) && (currentMillis - previousMillis >= OnTime))
      {
        state_reles_rain = HIGH;  // turn it on
        previousMillis = currentMillis;
        digitalWrite(pin_rele_rain, state_reles_rain);    // Update the actual LED
      } else if ((state_reles_rain == HIGH) && (currentMillis - previousMillis >= OffTime))
      {
        state_reles_rain = LOW;
        previousMillis = currentMillis;
        digitalWrite(pin_rele_rain, state_reles_rain);
      }

      break;
    case 0x02:
      startMillis = currentMillis;
      state_reles_rain = LOW;  // turn it on
      digitalWrite(pin_rele_rain, state_reles_rain);
      break;
  }
}
