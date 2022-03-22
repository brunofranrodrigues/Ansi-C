#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Modulo I2C display no endereco 0x27
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

#define On_buttonPin  8
#define Off_buttonPin 9
#define butSTR 10
#define pin_rele_rain 11

static byte estadoOn = 0;

static bool estadoBotaoOn = true;

static bool estadoAntBotaoOn = true;

static byte estadoOff = 0;

static bool estadoBotaoOff = true;

static bool estadoAntBotaoOff = true;

static unsigned long delayBouce;

static byte estadoSTR = 0;

static unsigned long delayBouce2;

static bool estadoBotaoSTR = true;

static bool estadoAntBotaoSTR = true;

int state_reles_rain = LOW;

unsigned long startMillis = 0;

unsigned long currentMillis = 0;

unsigned long previousMillis = 0;

long OnTime = 0;           // milliseconds of on-time
long OffTime = 0;          // milliseconds of off-time

void menu1();

void checkONbtn();

void checkOFFbtn();

void checkSTRbtn();

void controlaIrrigacao(); 

void setup()
{
  Serial.begin(9600);
  pinMode( On_buttonPin, INPUT_PULLUP);
  pinMode( Off_buttonPin, INPUT_PULLUP);

  lcd.begin (16, 2);
}


void loop()
{
  
  menu1();
  checkONbtn();
  checkOFFbtn();
  checkSTRbtn();
  controlaIrrigacao();

}

void menu1()
{
  lcd.setCursor(0, 0);
  lcd.print(" Ajuste de Tempo ");
  lcd.setCursor(0, 1);
  lcd.print(" ON:  ");
  lcd.setCursor(5, 1);
  if (estadoOn < 10) lcd.print("0");
  lcd.print(estadoOn);
  lcd.setCursor(7, 1);
  lcd.print(" OFF:  ");
  lcd.setCursor(13, 1);
  if (estadoOff < 10) lcd.print("0");
  lcd.print(estadoOff);
} //end menu1



void checkONbtn()
{
  estadoBotaoOn = digitalRead (On_buttonPin);
  if ((!estadoBotaoOn && estadoAntBotaoOn) && ((millis() - delayBouce))) {
    estadoOn++;
    
    if (estadoOn > 60)
    {
      estadoOn = 0;
    }

    delayBouce = millis();
  }


}

void checkOFFbtn()
{
  estadoBotaoOff = digitalRead (Off_buttonPin);
  if ((!estadoBotaoOff && estadoAntBotaoOff) && ((millis() - delayBouce))) {
    estadoOff++;

    if (estadoOff > 60)
    {
      estadoOff = 0;
    }
    delayBouce = millis();
  }


}

void checkSTRbtn()
{
  estadoBotaoSTR = digitalRead (butSTR);
  if ((!estadoBotaoSTR && estadoAntBotaoSTR) && ((millis() - delayBouce2))) {
    estadoSTR++;

    Serial.print("###");
    Serial.print("Start");
    Serial.print(estadoSTR);
    Serial.print("###");
    
    if (estadoSTR > 2) {
      estadoSTR = 0;
    }
    
    delayBouce2 = millis();
  }


}

void controlaIrrigacao()                                          //Função genérica para um quarto menu...
{
  unsigned long currentMillis = millis();

  OnTime = (estadoOn * 60000);

  OffTime = (estadoOff * 60000);

  switch (estadoSTR)                                     //Controle do set1
  {
    case 0x01:                                        //Caso 1
      if (estadoSTR == 1 && startMillis == 0) {
        startMillis = currentMillis;
        state_reles_rain = HIGH;  // turn it on
        digitalWrite(pin_rele_rain, state_reles_rain);
      }

      if ((state_reles_rain == LOW) && (currentMillis - previousMillis >= OnTime))
      {
        state_reles_rain = HIGH;  // turn it on
        previousMillis = currentMillis;     // Remember the time
        digitalWrite(pin_rele_rain, state_reles_rain);    // Update the actual LED
      } else if ((state_reles_rain == HIGH) && (currentMillis - previousMillis >= OffTime))
      {
        state_reles_rain = LOW;  // Turn it off
        previousMillis = currentMillis;    // Remember the time
        digitalWrite(pin_rele_rain, state_reles_rain);   // Update the actual LED
      }

      break;
    case 0x02:                                    //Caso 2
      startMillis = currentMillis;
      state_reles_rain = LOW;  // turn it on
      digitalWrite(pin_rele_rain, state_reles_rain);
      break;                                  //Break
  }
} //end controlaIrrigacao
