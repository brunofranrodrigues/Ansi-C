#include <DS3231.h>

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

Time t;

// Rele de acionamento da bomba de irrigação
#define pin_rele_rain 10

#define butSTR   8

static unsigned long delayBouce;

static bool estadoBotaoSTR = true;

static bool estadoAntBotaoSTR = true;

static byte estadoSTR = 0;

unsigned long startMillis = 0;

unsigned long currentMillis = 0;

unsigned long previousMillis = 0;

static byte estadoOn = 1;

static byte estadoOff = 1;

const int OnHour = 23;

const int OnMin = 59;

const int OffHour = 5;

const int OffMin = 59;

long OnTime = 0;           // milliseconds of on-time

long OffTime = 0;          // milliseconds of off-time

int state_reles_rain = LOW;

void setup() {
  Serial.begin(9600);

  rtc.begin();

  pinMode(butSTR, INPUT_PULLUP);         //Entrada para os botões (digitais 3) com pull-ups internos
  pinMode (pin_rele_rain, OUTPUT);      //Rele do sistema de irrigação

}

void loop() {
  currentMillis = millis();
  
  t = rtc.getTime();

  estadoBotaoSTR = digitalRead (butSTR);
  if ((!estadoBotaoSTR && estadoAntBotaoSTR) && ((millis() - delayBouce))) {
    estadoSTR++;
    if (estadoSTR > 2) {
      estadoSTR = 0;
    }
    delayBouce = millis();
  }

  if ((t.hour >= OnHour && t.min >= OnMin) && (t.hour <= OffHour && t.min <= OffMin)) {
    OnTime = (2 * 60000);

    OffTime = (2 * 60000);

  } else {

    OnTime = (estadoOn * 60000);

    OffTime = (estadoOff * 60000);
  }

  switch (estadoSTR)
  {
     case 0x01:
      if (estadoSTR == 1 && startMillis == 0) {
        startMillis = currentMillis;
        state_reles_rain = HIGH;
        digitalWrite(pin_rele_rain, state_reles_rain);
      }

      if ((state_reles_rain == LOW) && (currentMillis - previousMillis >= OnTime))
      {
        state_reles_rain = HIGH;  // turn it on
        previousMillis += OnTime;
        digitalWrite(pin_rele_rain, state_reles_rain);    // Update the actual LED
      } else if ((state_reles_rain == HIGH) && (currentMillis - previousMillis >= OffTime))
      {
        state_reles_rain = LOW;
        previousMillis += OffTime;
        digitalWrite(pin_rele_rain, state_reles_rain);
      }

      break;
    case 0x02:
      startMillis = 0;
      currentMillis = 0;
      state_reles_rain = LOW;  // turn it on
      digitalWrite(pin_rele_rain, state_reles_rain);
      break;
  }

}
