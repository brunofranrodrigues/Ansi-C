#include <DS3231.h>
#define pinLed1 2
#define pinBotao1 7

DS3231  rtc(SDA, SCL);
Time t;

unsigned long previousMillis = 0;        // will store last time LED was updated
unsigned long startMillis = 0;

const int nightOnHour = 16;
const int nightOnMin = 30;
long OnTime = 200;           // milliseconds of on-time
long OffTime = 500;          // milliseconds of off-time
int ledState = LOW;
static byte estadoSTR = 0;
static unsigned long delayBouce2;
static bool estadoBotaoSTR = true;
static bool estadoAntBotaoSTR = true;

void setup()
{
  Serial.begin(9600);
  // set the digital pin as output:
  pinMode(pinLed1, OUTPUT);
  pinMode(pinBotao1, INPUT_PULLUP);

}

void loop()
{
  unsigned long currentMillis = millis();
  estadoBotaoSTR = digitalRead (pinBotao1);
  if ((!estadoBotaoSTR && estadoAntBotaoSTR) && ((millis() - delayBouce2))) {
    estadoSTR++;
    if (estadoSTR > 2) {
      estadoSTR = 0;
    }
    delayBouce2 = millis();
  }

  estadoAntBotaoSTR = estadoBotaoSTR;
  switch (estadoSTR)                                //Controle do set1
  {
    case 0x01:                                     //Caso 1
      Serial.println("LIGHT ON");

      if (estadoSTR == 1 && startMillis == 0) {
        startMillis = currentMillis;
        ledState = HIGH;  // turn it on
        digitalWrite(pinLed1, ledState);
      }
      
      if ((ledState == LOW) && (currentMillis - previousMillis >= OnTime))
      {
        ledState = HIGH;  // turn it on
        previousMillis = currentMillis;     // Remember the time
        digitalWrite(pinLed1, ledState);    // Update the actual LED
        //Serial.println("LIGHT ON");
      } else if ((ledState == HIGH) && (currentMillis - previousMillis >= OffTime))
      {
        ledState = LOW;  // Turn it off
        previousMillis = currentMillis;    // Remember the time
        digitalWrite(pinLed1, ledState);   // Update the actual LED
        //Serial.println("LIGHT OFF");
      }      
      break;
    case 0x02:                                    //Caso 2
      ledState = LOW;  // Turn it off
      previousMillis = currentMillis;    // Remember the time
      digitalWrite(pinLed1, ledState);   // Update the actual LED
      Serial.println("LIGHT OFF");
      break;                             //Break
  } //end switch set1

}
