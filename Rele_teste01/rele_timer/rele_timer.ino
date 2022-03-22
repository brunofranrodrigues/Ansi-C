// --- Bibliotecas Auxiliares ---
#include <LiquidCrystal_I2C.h>                             //Biblioteca para o display LCD
#include <DS3231.h>

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

#define butSTR    52                                     //Botão de start ou stop do ambiente
#define butPlus   51                                     //Botão de + para minutos
#define butMinus  50                                     //Botão de - para minutos

// Rele de acionamento da bomba de irrigação
#define pin_rele_rain 4

// Modulo I2C display no endereco 0x27
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void controlaIrrigacao();                               //Função para efetuar o processo de irrigação no sistema hidroponico

// --- Variáveis Globais ---
boolean t_butSTR;                             //Flags para armazenar o estado dos botões

static byte estadoSTR = 0;

static byte estadoSTR1 = 0;

static byte estadoSTR2 = 0;

static unsigned long delayBouce2;

static bool estadoBotaoSTR = true;

static bool estadoAntBotaoSTR = true;


long OnTime = 200;           // milliseconds of on-time
long OffTime = 500;          // milliseconds of off-time

int state_reles_rain = LOW;

unsigned long startMillis = 0;
unsigned long currentMillis = 0;
unsigned long previousMillis = 0;


// --- Configurações Iniciais ---
void setup() {
  Serial.begin(9600);
  
  rtc.begin();
  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(MONDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(05, 2, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(1, 28, 2019);   // Set the date to January 1st, 2014

  //Inicializa LCD 16 x 2
  lcd.begin (16, 2);

  pinMode(butSTR, INPUT_PULLUP);     //Entrada para os botões (digitais 3) com pull-ups internos
  pinMode(butPlus, INPUT_PULLUP);     //Entrada para os botões (digitais 2) com pull-ups internos
  pinMode(butMinus, INPUT_PULLUP);     //Entrada para os botões (digitais 2) com pull-ups internos

  pinMode (pin_rele_rain, OUTPUT);    //Rele do sistema de irrigação

  digitalWrite(pin_rele_rain, LOW);     // Setup dos reles para manter desligados

}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("mode - 3");
  lcd.setCursor(0, 1);
  lcd.print("irrigation sys");

  estadoBotaoSTR = digitalRead (butSTR);
  if ((!estadoBotaoSTR && estadoAntBotaoSTR) && ((millis() - delayBouce2))) {
    estadoSTR1++;
    if (estadoSTR1 > 2) {
      lcd.clear();
      estadoSTR1 = 0;
    }
    delayBouce2 = millis();
  }

  estadoAntBotaoSTR = estadoBotaoSTR;
  switch (estadoSTR1)                                     //Controle do set1
  {
    case 0x01:                                        //Caso 1
      lcd.setCursor(0, 0);
      lcd.print("mode - 3");
      lcd.setCursor(0, 1);
      lcd.print("irrigation sys");
      lcd.setCursor(12, 0);                     //Posiciona cursor na coluna 1, linha 2
      lcd.print("ON");                         //Imprime mensagem
      controlaIrrigacao();
      break;
    case 0x02:                                    //Caso 2
      lcd.setCursor(0, 0);
      lcd.print("mode - 3");
      lcd.setCursor(0, 1);
      lcd.print("irrigation sys");
      lcd.setCursor(12, 0);                   //Posiciona cursor na coluna 1, linha 2
      lcd.print("OFF");                       //Imprime mensagem
      controlaIrrigacao();
      break;                                  //Break

  } //end switch set1

}

void controlaIrrigacao()                                          //Função genérica para um quarto menu...
{
  unsigned long currentMillis = millis();
  switch (estadoSTR1)                                     //Controle do set1
  {
    case 0x01:                                        //Caso 1
      if (estadoSTR1 == 1 && startMillis == 0) {
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
