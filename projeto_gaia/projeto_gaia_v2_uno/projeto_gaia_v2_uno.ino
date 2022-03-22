/*

      AUTOR:            Bruno Rodrigues
      SKETCH:           projeto_gaia_V2
      DATA:             17/04/2019
      MODIFICACOES:

*/

// --- Bibliotecas Auxiliares ---
#include <LiquidCrystal_I2C.h>                             //Biblioteca para o display LCD
#include <DS3231.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

Time t;

// --- Mapeamento de Hardware ---
// Rele de acionamento da bomba de agua potavel
#define pin_rele_water 2

// Rele de acionamento da bomba de liquido nutritivo
#define pin_rele_fert 3

// Rele de acionamento da bomba de irrigação
#define pin_rele_rain 4

//************ Temp Probe Related *********************************************//
#define ONE_WIRE_BUS 5          // Data wire For Temp Probe is plugged into pin 5 on the Arduino 

// Pino ligado ao sensor de nivel minimo
#define sen_nivel_minimo 6

// Pino ligado ao sensor de nivel maximo
#define sen_nivel_maximo 7

// Pino ligado ao Led verde de sinalização de tanque cheio
#define pin_led_green 8

// Pino ligado ao Led vermelho de sinalização de tanque vazio
#define pin_led_red 9

#define buzzer 49                                       //pino da sirene

#define On_buttonPin  50

#define Off_buttonPin 51

#define butSTR   52                                    //Botão de start ou stop do ambiente

#define butMOD   53                                    //Botão para selecionar tela do menu


//************************* Inicio das variaveis de PH ******************************************************//

const byte phpin = A3;
float phValue;

//************************* Fim das variaveis de PH ********************************************************//

//************************* User Defined Variables ********************************************************//


//##################################################################################
//-----------  Do not Replace R1 with a resistor lower than 300 ohms    ------------
//##################################################################################


int R1 = 2000;
int Ra = 25; //Resistance of powering Pins
int ECPin = A0;
int ECGround = A2;
int ECPower = A1;


//*********** Converting to ppm [Learn to use EC it is much better**************//
// Hana      [USA]        PPMconverion:  0.5
// Eutech    [EU]          PPMconversion:  0.64
//Tranchen  [Australia]  PPMconversion:  0.7
// Why didnt anyone standardise this?


float PPMconversion = 0.7;


//*************Compensating for temperature ************************************//
//The value below will change depending on what chemical solution we are measuring
//0.019 is generaly considered the standard for plant nutrients [google "Temperature compensation EC" for more info
float TemperatureCoef = 0.019; //this changes depending on what chemical we are measuring

//********************** Cell Constant For Ec Measurements *********************//
//Mine was around 2.9 with plugs being a standard size they should all be around the same
//But If you get bad readings you can use the calibration script and fluid to get a better estimate for K
float K = 2.88;

//***************************** END Of Recomended User Inputs *****************************************************************//

// Modulo I2C display no endereco 0x27
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

OneWire oneWire(ONE_WIRE_BUS);// Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature.


void changeMenu();                                      //Função para modificar o menu atual
void dispMenu();                                        //Função para mostrar o menu atual

void checkONbtn();                                      //Função para verificar se o botão de timer on foi pressionado
void checkOFFbtn();                                     //Função para verificar se o botão de timer off foi pressionado

void menu1();                                           //Função do menu1 de visualização de tempo
void menu2();                                           //Função do menu2 de encher o tanque
void menu3();                                           //Função do menu3 de visualização dos sensores
void menu4();                                           //Função do menu4 de irrigar
void menu5();                                           //Função do menu5 timer para a irrigação


void controlaTanque();                                  //Função para controlar o processo de encher o tanque hidroponico
void controlaIrrigacao();                               //Função para efetuar o processo de irrigação no sistema hidroponico
void GetsensorReadings();                               //Função para exibir a info dos sensores de temp e de EC e enviar para o lcd
void GetEC();                                           //Função para ler o sensor de temperatura e de EC
void GetPh();                                           //Função para ler o sensor de PH
void PrintReadingsSerial();                             //Função para enviar a info dos sensores para a serial
void recvWithStartEndMarkers();
void parseData();
void showParsedData();
void BuzzerAlarme();                                    //Função para o Alarme sonoro


// --- Variáveis Globais ---
char menu = 0x01;                                       //Variável para selecionar o menu

boolean t_butMOD;                             //Flags para armazenar o estado dos botões

// Variaveis do botao de start para cada menu
static unsigned long delayBouce;

static bool estadoBotaoSTR = true;

static bool estadoAntBotaoSTR = true;

static byte estadoSTR = 0;

static byte estadoSTR1 = 0;

static byte estadoSTR2 = 0;
// end das variaveis da função de start

static unsigned long delayPiscaScreen;

unsigned long startMillis = 0;

unsigned long currentMillis = 0;

unsigned long previousMillis = 0;

static byte estadoOn = 15;

static byte estadoOff = 15;

const int OnHour = 23;

const int OnMin = 59;

const int OffHour = 5;

const int OffMin = 59;

long OnTime = 0;           // milliseconds of on-time

long OffTime = 0;          // milliseconds of off-time

int state_reles_rain = LOW;

float Temperature = 10;

float EC = 0;

float EC25 = 0;

int ppm = 0;

float raw = 0;

float Vin = 5;

float Vdrop = 0;

float Rc = 0;

float buffer = 0;

char sistema[32] = {0};

char estado[32] = {0};

const byte numChars = 32;
char receivedChars[numChars];
char tempChars[numChars];        // temporary array for use when parsing

boolean newData = false;


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

  pinMode(butMOD, INPUT_PULLUP);         //Entrada para os botões (digitais 2) com pull-ups internos
  pinMode(butSTR, INPUT_PULLUP);         //Entrada para os botões (digitais 3) com pull-ups internos
  pinMode(On_buttonPin, INPUT_PULLUP);         //Entrada para os botões (digitais 2) com pull-ups internos
  pinMode(Off_buttonPin, INPUT_PULLUP);         //Entrada para os botões (digitais 3) com pull-ups internos

  pinMode (sen_nivel_minimo, INPUT);     //Sensor de nivel minimo da caixa d agua
  pinMode (sen_nivel_maximo, INPUT);     //Sensor de nivel maximo da caixa d agua
  pinMode (pin_led_green, OUTPUT);       //Led de sinalização que a caixa d agua esta cheia
  pinMode (pin_led_red, OUTPUT);         //Led de sinalização que a caixa d agua esta vazia

  pinMode (pin_rele_water, OUTPUT);     //Rele de agua
  pinMode (pin_rele_fert, OUTPUT);      //Rele do fertilizante
  pinMode (pin_rele_rain, OUTPUT);      //Rele do sistema de irrigação

  pinMode(buzzer, OUTPUT);              //Saida para o buzzer no pino 50

  pinMode(ECPin, INPUT);
  pinMode(ECPower, OUTPUT); //Setting pin for sourcing current
  pinMode(ECGround, OUTPUT); //setting pin for sinking current

  digitalWrite(pin_rele_water, LOW);    // Setup dos reles para manter desligados
  digitalWrite(pin_rele_fert, LOW);     // Setup dos reles para manter desligados
  digitalWrite(pin_rele_rain, LOW);     // Setup dos reles para manter desligados
  digitalWrite(ECGround, LOW); //We can leave the ground connected permanantly

  t_butMOD   = 0x00;                                      //limpa flag do botão Mode

  delay(100);

  sensors.begin();// gives sensor time to settle

  delay(100);

  R1 = (R1 + Ra);

  noTone(buzzer);

}

void loop() {
  static unsigned long delayPrint;

  changeMenu();

  dispMenu();

  recvWithStartEndMarkers();
  if (newData == true) {
    strcpy(tempChars, receivedChars);
    // this temporary copy is necessary to protect the original data
    //   because strtok() used in parseData() replaces the commas with \0
    parseData();
    showParsedData();
    newData = false;
  }

  controlaIrrigacao();

  controlaTanque();


  if ( (millis() - delayPrint) < 60000) {

    GetsensorReadings();

  }

  if ( (millis() - delayPrint) >= 120000) {
    //Zera a variavel delayPrint com o tempo atual depois que passar 2 minutos
    delayPrint = millis();
  }

}

void changeMenu()                                       //Modifica o menu atual
{
  if (!digitalRead(butMOD))   t_butMOD   = 0x01;       //Botão Up pressionado? Seta flag

  if (digitalRead(butMOD) && t_butMOD)                  //Botão Up solto e flag setada?
  { //Sim...
    t_butMOD = 0x00;                                  //Limpa flag

    lcd.clear();                                     //Limpa display
    menu++;                                           //Incrementa menu

    if (menu > 0x05) menu = 0x01;                     //Se menu maior que 4, volta a ser 1

  } //end butMOD

} //end changeMenu

void dispMenu()                                         //Mostra o menu atual
{
  switch (menu)                                      //Controle da variável menu
  {
    case 0x01:                                       //Caso 1
      menu1();                                       //Chama a função de relógio

      break;                                         //break
    case 0x02:                                       //Caso 2
      menu2();                                       //Chama a função de encher o tanque

      break;                                         //break
    case 0x03:                                       //Caso 3
      menu3();                                       //Chama função para ler os sensores e imprimir no lcd

      break;                                         //break
    case 0x04:                                       //Caso 4
      menu4();                                       //Chama função para irrigar as camas de cultivos

      break;                                         //break
    case 0x05:                                       //Caso 5
      menu5();                                       //Chama de timer para a irrigação

      break;                                         //break

  } //end switch menu

} //end dispMenu

void menu1()
{
  lcd.setCursor(0, 0);
  lcd.print("Time:  ");
  lcd.print(rtc.getTimeStr());

  lcd.setCursor(0, 1);
  lcd.print("Date: ");
  lcd.print(rtc.getDateStr());


} //end menu1

void menu2()                                            //Função de encher o tanque baseado no sensor de nivel
{
  lcd.setCursor(0, 0);
  lcd.print("mode - 1");
  lcd.setCursor(0, 1);
  lcd.print("fill the tank");

  estadoBotaoSTR = digitalRead (butSTR);
  if ((!estadoBotaoSTR && estadoAntBotaoSTR) && ((millis() - delayBouce))) {
    estadoSTR++;
    if (estadoSTR > 2) {
      lcd.clear();
      estadoSTR = 0;
    }
    delayBouce = millis();
  }

  estadoAntBotaoSTR = estadoBotaoSTR;

  switch (estadoSTR)
  {
    case 0x01:
      lcd.setCursor(0, 0);
      lcd.print("mode - 1");
      lcd.setCursor(0, 1);
      lcd.print("fill the tank");
      lcd.setCursor(12, 0);
      lcd.print("ON");
      controlaTanque();
      break;
    case 0x02:
      lcd.setCursor(0, 0);
      lcd.print("mode - 1");
      lcd.setCursor(0, 1);
      lcd.print("fill the tank");
      lcd.setCursor(12, 0);
      lcd.print("OFF");
      controlaTanque();
      break;

  } //end switch set1

} //end menu2

void menu3()                                            //Função de visualização dos sensores no LCD
{
  lcd.setCursor(0, 0);
  lcd.print("mode - 2");
  lcd.setCursor(0, 1);
  lcd.print("read the sensors");

  estadoBotaoSTR = digitalRead (butSTR);
  if ((!estadoBotaoSTR && estadoAntBotaoSTR) && ((millis() - delayBouce))) {
    estadoSTR2++;
    if (estadoSTR2 > 2) {
      lcd.clear();
      estadoSTR2 = 0;
    }
    delayBouce = millis();
  }

  estadoAntBotaoSTR = estadoBotaoSTR;

  switch (estadoSTR2)
  {
    case 0x01:
      lcd.setCursor(12, 0);
      lcd.print("ON");
      lcd.setCursor(0, 1);
      lcd.print("                ");

      if ((millis() - delayPiscaScreen) < 4000) {
        //lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("mode - 2");
        lcd.setCursor(12, 0);
        lcd.print("ON");
        lcd.setCursor(0, 1);
        lcd.print("PH:");
        lcd.print(phValue);
      }

      if ( ((millis() - delayPiscaScreen) >= 4000) && ((millis() - delayPiscaScreen) < 8000) ) {
        //lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("mode - 2");
        lcd.setCursor(12, 0);
        lcd.print("ON");
        lcd.setCursor(0, 1);
        lcd.print("Temp:");
        lcd.print(Temperature);
        lcd.print(" *C ");
      }

      if ( ((millis() - delayPiscaScreen) >= 8000) && ((millis() - delayPiscaScreen) < 12000) ) {
        //lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("mode - 2");
        lcd.setCursor(12, 0);
        lcd.print("ON");
        lcd.setCursor(0, 1);
        lcd.print("EC:");
        lcd.print(EC25);
      }

      if ((millis() - delayPiscaScreen) >= 16000) {
        delayPiscaScreen = millis();
      }

      break;
    case 0x02:
      lcd.setCursor(0, 0);
      lcd.print("mode - 2");
      lcd.setCursor(0, 1);
      lcd.print("read the sensors");
      lcd.setCursor(12, 0);
      lcd.print("OFF");
      break;

  } //end switch set1



} //end menu3

void menu4()                                            //Função do sistema de irrigação
{
  lcd.setCursor(0, 0);
  lcd.print("mode - 3");
  lcd.setCursor(0, 1);
  lcd.print("irrigation sys");

  estadoBotaoSTR = digitalRead (butSTR);
  if ((!estadoBotaoSTR && estadoAntBotaoSTR) && ((millis() - delayBouce))) {
    estadoSTR1++;
    if (estadoSTR1 > 2) {
      lcd.clear();
      estadoSTR1 = 0;
    }
    delayBouce = millis();
  }

  estadoAntBotaoSTR = estadoBotaoSTR;

  switch (estadoSTR1)
  {
    case 0x01:
      lcd.setCursor(0, 0);
      lcd.print("mode - 3");
      lcd.setCursor(0, 1);
      lcd.print("irrigation sys");
      lcd.setCursor(12, 0);
      lcd.print("ON");
      controlaIrrigacao();
      break;
    case 0x02:
      lcd.setCursor(0, 0);
      lcd.print("mode - 3");
      lcd.setCursor(0, 1);
      lcd.print("irrigation sys");
      lcd.setCursor(12, 0);
      lcd.print("OFF");
      controlaIrrigacao();
      break;

  } //end switch set1


} //end menu4


void menu5()                                  //Função do timer para o sistema de irrigação
{
  checkONbtn();

  checkOFFbtn();

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
} //end menu5

//************************************* Main Loop - checkONbtn() ***************************************************************//
void checkONbtn()
{
  static bool estadoBotaoOn = true;

  static bool estadoAntBotaoOn = true;

  static unsigned long delayBouce;

  estadoBotaoOn = digitalRead (On_buttonPin);
  if ((!estadoBotaoOn && estadoAntBotaoOn) && ((millis() - delayBouce))) {
    estadoOn++;

    if (estadoOn > 60)
    {
      estadoOn = 0;
    }

    delayBouce = millis();
  }


}   //end checkONbtn
//************************************** End Of Main Loop *****************************************************************//

//************************************* Main Loop - checkOFFbtn() ***************************************************************//
void checkOFFbtn()
{

  static bool estadoBotaoOff = true;

  static bool estadoAntBotaoOff = true;

  static unsigned long delayBouce;

  estadoBotaoOff = digitalRead (Off_buttonPin);
  if ((!estadoBotaoOff && estadoAntBotaoOff) && ((millis() - delayBouce))) {
    estadoOff++;

    if (estadoOff > 60)
    {
      estadoOff = 0;
    }
    delayBouce = millis();
  }


} //end checkOFFbtn
//************************************** End Of Main Loop *****************************************************************//

//************************************* Main Loop - controlaIrrigacao() ***************************************************************//
void controlaIrrigacao()
{
  currentMillis = millis();

  t = rtc.getTime();

  if ((t.hour >= OnHour && t.min >= OnMin) && (t.hour <= OffHour && t.min <= OffMin)) {
    OnTime = (15 * 60000);

    OffTime = (60 * 60000);

  } else {

    OnTime = (estadoOn * 60000);

    OffTime = (estadoOff * 60000);
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
} //end controlaIrrigacao
//************************************** End Of Main Loop *****************************************************************//

//************************************* Main Loop - controlaTanque() ***************************************************************//
void controlaTanque()
{
  static byte estadoTank = 0;
  static byte estadoRele = 0;
  static bool estadoMinimo = true;
  static bool estadoMaximo = true;
  static unsigned long delayPisca;

  // Efetua a leitura dos sensores de nivel
  estadoMinimo = digitalRead(sen_nivel_minimo);
  estadoMaximo = digitalRead(sen_nivel_maximo);

  switch (estadoSTR)                               //Controle do set1
  {
    case 0x01:                                     //Caso 1
      if ((estadoMinimo == 0) && (estadoMaximo == 0) && (estadoRele == 0)) {
        estadoTank = 1;
      }

      if ((estadoMinimo == 1) && (estadoMaximo == 0) && (estadoRele == 1)) {
        estadoTank = 2;
      }

      if ((estadoMinimo == 1) && (estadoMaximo == 1) && (estadoRele == 1)) {
        estadoTank = 3;
      }

      if ((estadoMinimo == 1) && (estadoMaximo == 1) && (estadoRele == 0 || estadoRele == 1)) {
        estadoTank = 4;
      }

      if ((estadoMinimo == 0) && (estadoMaximo == 1) && (estadoRele == 0 || estadoRele == 1)) {
        estadoTank = 4;
      }

      switch (estadoTank) {
        case 1: {
            //Liga o led vermelho por que o tanque esta vazio
            digitalWrite(pin_led_red, HIGH);
            //Chama a função do alarme para quando o tanque esta vazio
            BuzzerAlarme();
            //Desliga o led verde por que o tanque esta vazio
            digitalWrite(pin_led_green, LOW);
            //Liga o rele para bombear agua e fertilizante pro tanque
            digitalWrite(pin_rele_water, HIGH);
            digitalWrite(pin_rele_fert, HIGH);
            estadoRele = 1; // relé ligado
            break;
          }

        case 2: {
            noTone(buzzer);
            if ( (millis() - delayPisca) < 500) {
              //Desliga o led vermelho por que o tanque esta cheio
              digitalWrite(pin_led_red, LOW);
            } else {
              //Liga o led vermelho como um alarme
              digitalWrite(pin_led_red, HIGH);
            }
            if ( (millis() - delayPisca) >= 1000) {
              //Zera a variavel delayPisca com o tempo atual depois que passar 1000 ms
              delayPisca = millis();
            }
            //Liga o led verde porque o tanque esta cheio
            digitalWrite(pin_led_green, HIGH);
            //Desliga o rele para bombear agua pro tanque porque esta cheio
            digitalWrite(pin_rele_water, HIGH);
            digitalWrite(pin_rele_fert, HIGH);
            estadoRele = 1; // relé ligado
            break;
          }

        case 3: {
            noTone(buzzer);
            //Desliga o led vermelho por que o tanque esta cheio
            digitalWrite(pin_led_red, LOW);
            //Liga o led verde por que o tanque esta cheio
            digitalWrite(pin_led_green, HIGH);
            //Desliga o rele para bombear agua pro tanque porque esta cheio
            digitalWrite(pin_rele_water, LOW);
            digitalWrite(pin_rele_fert, LOW);
            estadoRele = 0; // relé desligado
            break;
          }

        case 4: {
            noTone(buzzer);
            //Desliga o led vermelho por que o tanque esta cheio
            digitalWrite(pin_led_red, LOW);
            //Liga o led verde por que o tanque esta cheio
            digitalWrite(pin_led_green, HIGH);
            //Desliga o rele para bombear agua pro tanque porque esta cheio
            digitalWrite(pin_rele_water, LOW);
            digitalWrite(pin_rele_fert, LOW);
            estadoRele = 0; // relé desligado
            break;
          }
      }
      break;
    case 0x02:                                    //Caso 2
      noTone(buzzer);
      digitalWrite(pin_led_red, LOW);
      //Liga o led verde por que o tanque esta cheio
      digitalWrite(pin_led_green, HIGH);
      //Desliga o rele para bombear agua pro tanque porque esta cheio
      digitalWrite(pin_rele_water, LOW);
      digitalWrite(pin_rele_fert, LOW);
      estadoRele = 0; // relé desligado
      break;
  }

} //end controlaTanque
//************************************** End Of Main Loop *****************************************************************//

//************************************* Main Loop - GetPh() ***************************************************************//
void GetPh()
{

  phValue = (1023 - analogRead(phpin)) / 73.07;
  phValue = (phValue + 1);        //calibração.

}
//************************************** End Of Main Loop *****************************************************************//

//************************************* Main Loop - GetEC() ***************************************************************//
void GetEC()
{


  //*********Reading Temperature Of Solution *******************//
  sensors.requestTemperatures();// Send the command to get temperatures
  Temperature = sensors.getTempCByIndex(0); //Stores Value in Variable


  //************Estimates Resistance of Liquid ****************//
  digitalWrite(ECPower, HIGH);
  raw = analogRead(ECPin);
  raw = analogRead(ECPin); // This is not a mistake, First reading will be low beause if charged a capacitor
  digitalWrite(ECPower, LOW);

  //***************** Converts to EC **************************//
  Vdrop = (Vin * raw) / 1024.0;
  Rc = (Vdrop * R1) / (Vin - Vdrop);
  Rc = Rc - Ra; //acounting for Digital Pin Resitance
  EC = 1000 / (Rc * K);


  //*************Compensating For Temperaure********************//
  EC25  =  EC / (1 + TemperatureCoef * (Temperature - 25.0));
  ppm = (EC25) * (PPMconversion * 1000);


  ;
}
//************************************** End Of Main Loop ***************************************************************//

//************************************* Main Loop - PrintReadingsSerial() ***************************************************************//
void PrintReadingsSerial()
{
  /*
    Serial.print("<");
    Serial.print(phValue);
    Serial.print(",");
    Serial.print(EC25);
    Serial.print(",");
    Serial.print(Temperature);
    Serial.print(">");
  */


  Serial.print("<");
  Serial.print(phValue);
  Serial.print(",");
  Serial.print(EC25);
  Serial.print(",");
  Serial.print(Temperature);
  Serial.print(">");

  /*
    //********** Usued for Debugging ************
    Serial.print("Vdrop: ");
    Serial.println(Vdrop);
    Serial.print("Rc: ");
    Serial.println(Rc);
    Serial.print(EC);
    Serial.println("Siemens");
    //********** end of Debugging Prints *********
  */
};
//************************************** End Of Main Loop **********************************************************************//

void recvWithStartEndMarkers() {
  static boolean recvInProgress = false;
  static byte ndx = 0;
  char startMarker = '#';
  char endMarker = '#';
  char rc;

  while (Serial.available() > 0 && newData == false) {
    rc = Serial.read();

    if (recvInProgress == true) {
      if (rc != endMarker) {
        receivedChars[ndx] = rc;
        ndx++;
        if (ndx >= numChars) {
          ndx = numChars - 1;
        }
      }
      else {
        receivedChars[ndx] = '\0'; // terminate the string
        recvInProgress = false;
        ndx = 0;
        newData = true;
      }
    }

    else if (rc == startMarker) {
      recvInProgress = true;
    }
  }
}


void parseData() {

  // split the data into its parts

  char * strtokIndx; // this is used by strtok() as an index

  strtokIndx = strtok(receivedChars, ":");     // get the first part - the string
  strcpy(sistema, strtokIndx); // copy it to messageFromPC

  strtokIndx = strtok(NULL, ":");     // get the first part - the string
  strcpy(estado, strtokIndx); // copy it to messageFromPC

}

void showParsedData() {
  //Serial.println("sistema");
  //Serial.println(sistema);
  //Serial.println("estado");
  //Serial.println(estado);
  if ((strcmp(sistema, "tank") == 0) && (strcmp(estado, "ON") == 0)) {
    estadoSTR = 1;
    lcd.clear();
    //Serial.println(estadoSTR);
  } else if ((strcmp(sistema, "tank") == 0) && (strcmp(estado, "OFF") == 0)) {
    estadoSTR = 2;
    lcd.clear();
    //Serial.println(estadoSTR);
  }

  if ((strcmp(sistema, "irrigation") == 0) && (strcmp(estado, "ON") == 0)) {
    estadoSTR1 = 1;
    startMillis = 0;
    currentMillis = 0;
    lcd.clear();
    //Serial.println(estadoSTR1);
  } else if ((strcmp(sistema, "irrigation") == 0) && (strcmp(estado, "OFF") == 0)) {
    estadoSTR1 = 2;
    startMillis = 0;
    currentMillis = 0;
    lcd.clear();
    //Serial.println(estadoSTR1);
  }

}

//************************************* Main Loop - GetsensorReadings() ***************************************************************//
//Moved Heavy Work To subroutines so you can call them from main loop without cluttering the main loop
void GetsensorReadings()
{
  static unsigned long delayEC;

  if ( (millis() - delayEC) < 60000) {

    GetEC();          //Calls Code to Go into GetEC()
    GetPh();
    PrintReadingsSerial();  // Cals Print routine

  }

  if ( (millis() - delayEC) >= 120000) {
    //Zera a variavel delayEC com o tempo atual depois que passar 2 minutos
    delayEC = millis();
  }

}
//************************************** End Of Main Loop **********************************************************************//

//************************************* Main Loop - BuzzerAlarme() ***************************************************************//
void BuzzerAlarme()
{
  static unsigned long delayBuzzer;

  if ( (millis() - delayBuzzer) < 500) {
    //Ligando o buzzer com uma frequencia de 1500 hz.
    tone(buzzer, 1500);
  } else {
    //Desligando o buzzer.
    noTone(buzzer);
  }
  if ( (millis() - delayBuzzer) >= 1000) {
    delayBuzzer = millis();
  }

}
//************************************** End Of Main Loop **********************************************************************//
