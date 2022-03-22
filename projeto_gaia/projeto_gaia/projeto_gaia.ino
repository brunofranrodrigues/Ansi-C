// --- Bibliotecas Auxiliares ---
#include <LiquidCrystal_I2C.h>                             //Biblioteca para o display LCD
#include <DS3231.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

// --- Mapeamento de Hardware ---
#define butMOD    2                                     //Botão para selecionar tela acima no digital 2

#define butSTR    3                                     //Botão de start ou stop do ambiente

// Pino ligado ao sensor de nivel minimo
#define sen_nivel_minimo 4

// Pino ligado ao sensor de nivel maximo
#define sen_nivel_maximo 5

// Pino ligado ao Led verde de sinalização de tanque cheio
#define pin_led_green 6

// Pino ligado ao Led vermelho de sinalização de tanque vazio
#define pin_led_red 7

// Rele de acionamento da bomba de agua potavel
#define pin_rele_water 8

// Rele de acionamento da bomba de liquido nutritivel
#define pin_rele_fert 9

// Rele de acionamento da bomba de irrigação
#define pin_rele_rain 10

//************************* User Defined Variables ********************************************************//


//##################################################################################
//-----------  Do not Replace R1 with a resistor lower than 300 ohms    ------------
//##################################################################################


int R1 = 1000;
int Ra = 25; //Resistance of powering Pins
int ECPin = A0;
int ECGround = A1;
int ECPower = A2;


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




//************ Temp Probe Related *********************************************//
#define ONE_WIRE_BUS 11          // Data wire For Temp Probe is plugged into pin 11 on the Arduino  

//***************************** END Of Recomended User Inputs *****************************************************************//

// Modulo I2C display no endereco 0x27
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

OneWire oneWire(ONE_WIRE_BUS);// Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature.

void changeMenu();                                      //Função para modificar o menu atual
void dispMenu();                                        //Função para mostrar o menu atual
void menu1();
void menu2();
void menu3();
void menu4();

void controlaEC();                                      //Função para exibir a info dos sensores de temp e de EC e enviar para a serial
void GetEC();                                           //Função para ler o sensor de temperatura e de EC
void PrintReadingsEC();                                   //Função para enviar a info dos sensores para a serial
void controlaTanque();                                  //Função para controlar o processo de encher o tanque hidroponico
void controlaIrrigacao();                               //Função para efetuar o processo de irrigação no sistema hidroponico

// --- Variáveis Globais ---
char menu = 0x01;                                       //Variável para selecionar o menu
boolean t_butMOD, t_butSTR;                             //Flags para armazenar o estado dos botões

long OnTime = 200;           // milliseconds of on-time
long OffTime = 500;          // milliseconds of off-time
int ledState = LOW;
static byte estadoSTR = 0;
static byte estadoSTR1 = 0;
static unsigned long delayBouce2;
static bool estadoBotaoSTR = true;
static bool estadoAntBotaoSTR = true;

unsigned long previousMillis = 0;
unsigned long startMillis = 0;

float Temperature = 10;
float EC = 0;
float EC25 = 0;
int ppm = 0;

float raw = 0;
float Vin = 5;
float Vdrop = 0;
float Rc = 0;
float buffer = 0;

// --- Configurações Iniciais ---
void setup()
{
  Serial.begin(9600);
  // Initialize the rtc object
  rtc.begin();
  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(MONDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(05, 2, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(1, 28, 2019);   // Set the date to January 1st, 2014


  //Inicializa LCD 16 x 2
  lcd.begin (16, 2);
  pinMode (sen_nivel_minimo, INPUT);
  pinMode (sen_nivel_maximo, INPUT);
  pinMode (pin_led_green, OUTPUT);
  pinMode (pin_led_red, OUTPUT);
  pinMode (pin_rele_water, OUTPUT);
  pinMode (pin_rele_fert, OUTPUT);
  pinMode (pin_rele_rain, OUTPUT);
  pinMode(butMOD, INPUT_PULLUP);     //Entrada para os botões (digitais 2) com pull-ups internos
  pinMode(butSTR, INPUT_PULLUP);     //Entrada para os botões (digitais 3) com pull-ups internos

  //*********************************Setup - runs Once and sets pins etc ******************************************************//
  pinMode(ECPin, INPUT);
  pinMode(ECPower, OUTPUT); //Setting pin for sourcing current
  pinMode(ECGround, OUTPUT); //setting pin for sinking current
  digitalWrite(ECGround, LOW); //We can leave the ground connected permanantly

  t_butMOD   = 0x00;                                      //limpa flag do botão Up
  t_butSTR   = 0x00;                                      //limpa flag do botão P

  delay(100);

  sensors.begin();// gives sensor time to settle

  delay(100);

  R1 = (R1 + Ra);

} //end setup

void changeMenu()                                       //Modifica o menu atual
{
  if (!digitalRead(butMOD))   t_butMOD   = 0x01;       //Botão Up pressionado? Seta flag

  if (digitalRead(butMOD) && t_butMOD)                  //Botão Up solto e flag setada?
  { //Sim...
    t_butMOD = 0x00;                                  //Limpa flag

    lcd.clear();                                     //Limpa display
    menu++;                                           //Incrementa menu

    if (menu > 0x04) menu = 0x01;                     //Se menu maior que 4, volta a ser 1

  } //end butUp

} //end changeMenu


void dispMenu()                                         //Mostra o menu atual
{
  switch (menu)                                       //Controle da variável menu
  {
    case 0x01:                                       //Caso 1
      menu1();                                   //Chama a função de relógio

      break;                                     //break
    case 0x02:                                       //Caso 2
      menu2();                                   //Chama a função do termômetro

      break;                                     //break
    case 0x03:                                       //Caso 3
      menu3();                                   //Chama função para acionamento de lâmpadas

      break;                                     //break
    case 0x04:                                       //Caso 3
      menu4();                                   //Chama função para acionamento de lâmpadas

      break;                                     //break

  } //end switch menu

} //end dispMenu

//************ This Loop Is called From Main Loop************************//
void GetEC() {


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
//************************** End OF EC Function ***************************//

//***This Loop Is called From Main Loop- Prints to serial usefull info ***//
void PrintReadingsEC() {
  Serial.print("Rc: ");
  Serial.print(Rc);
  Serial.print(" EC: ");
  Serial.print(EC25);
  Serial.print(" Simens  ");
  Serial.print(ppm);
  Serial.print(" ppm  ");
  Serial.print(Temperature);
  Serial.println(" *C ");


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

//************************************* Main Loop - controlaEC() ***************************************************************//
//Moved Heavy Work To subroutines so you can call them from main loop without cluttering the main loop
void controlaEC() {
  static unsigned long delayEC;


  if ( (millis() - delayEC) < 500) {

    GetEC();          //Calls Code to Go into GetEC() Loop [Below Main Loop] dont call this more that 1/5 hhz [once every five seconds] or you will polarise the water
    PrintReadingsEC();  // Cals Print routine [below main loop]

  }

  if ( (millis() - delayEC) >= 4500) {
    //Zera a variavel delayPisca com o tempo atual depois que passar 800 ms
    delayEC = millis();
  }


}
//************************************** End Of Main Loop **********************************************************************//

void controlaTanque() {
  static byte estadoTank = 0;
  static byte estadoRele = 0;
  static bool estadoMinimo = true;
  static bool estadoMaximo = true;
  static unsigned long delayPisca;

  // Efetua a leitura dos sensores de nivel
  estadoMinimo = digitalRead(sen_nivel_minimo);
  estadoMaximo = digitalRead(sen_nivel_maximo);

  if ((estadoMinimo == 0) && (estadoMaximo == 0) && (estadoRele == 0)) {
    estadoTank = 1;
  }

  if ((estadoMinimo == 1) && (estadoMaximo == 0) && (estadoRele == 1)) {
    estadoTank = 2;
  }

  if ((estadoMinimo == 1) && (estadoMaximo == 1) && (estadoRele == 1)) {
    estadoTank = 3;
  }

  if ((estadoMinimo == 0) && (estadoMaximo == 1) && (estadoRele == 0 || estadoRele == 1)) {
    estadoTank = 4;
  }

  switch (estadoTank) {
    case 1: {
        //Liga o led vermelho por que o tanque esta vazio
        digitalWrite(pin_led_red, HIGH);
        //Desliga o led verde por que o tanque esta vazio
        digitalWrite(pin_led_green, LOW);
        //Liga o rele para bombear agua pro tanque
        digitalWrite(pin_rele_water, LOW);//invertido não sei porque
        digitalWrite(pin_rele_fert, LOW);//invertido não sei porque
        estadoRele = 1; // relé ligado
        break;
      }

    case 2: {
        if ( (millis() - delayPisca) < 200) {
          //Desliga o led vermelho por que o tanque esta cheio
          digitalWrite(pin_led_red, LOW);
        } else {
          //Liga o led vermelho como um alarme
          digitalWrite(pin_led_red, HIGH);
        }
        if ( (millis() - delayPisca) >= 800) {
          //Zera a variavel delayPisca com o tempo atual depois que passar 800 ms
          delayPisca = millis();
        }
        //Liga o led verde por que o tanque esta cheio
        digitalWrite(pin_led_green, HIGH);
        //Desliga o rele para bombear agua pro tanque porque esta cheio
        digitalWrite(pin_rele_water, LOW);//invertido não sei porque
        digitalWrite(pin_rele_fert, LOW);//invertido não sei porque
        estadoRele = 1; // relé ligado
        break;
      }

    case 3: {
        //Desliga o led vermelho por que o tanque esta cheio
        digitalWrite(pin_led_red, LOW);
        //Liga o led verde por que o tanque esta cheio
        digitalWrite(pin_led_green, HIGH);
        //Desliga o rele para bombear agua pro tanque porque esta cheio
        digitalWrite(pin_rele_water, HIGH);//invertido não sei porque
        digitalWrite(pin_rele_fert, HIGH);//invertido não sei porque
        estadoRele = 0; // relé ligado
        break;
      }

    case 4: {
        //Desliga o led vermelho por que o tanque esta cheio
        digitalWrite(pin_led_red, LOW);
        //Liga o led verde por que o tanque esta cheio
        digitalWrite(pin_led_green, HIGH);
        //Desliga o rele para bombear agua pro tanque porque esta cheio
        digitalWrite(pin_rele_water, HIGH);//invertido não sei porque
        digitalWrite(pin_rele_fert, HIGH);//invertido não sei porque
        estadoRele = 0; // relé ligado
        break;
      }

  }

}

void controlaIrrigacao() {
unsigned long currentMillis = millis();
/*  estadoBotaoSTR = digitalRead (butSTR);
  if ((!estadoBotaoSTR && estadoAntBotaoSTR) && ((millis() - delayBouce2))) {
    estadoSTR1++;
    if (estadoSTR1 > 2) {
      estadoSTR1 = 0;
    }
    delayBouce2 = millis();
  }

  estadoAntBotaoSTR = estadoBotaoSTR;*/

  switch (estadoSTR1)                                //Controle do set1
  {
    case 0x01:                                     //Caso 1
      if (estadoSTR1 == 1 && startMillis == 0) {
        startMillis = currentMillis;
        ledState = HIGH;  // turn it on
        digitalWrite(pin_rele_rain, ledState);
      }

      if ((ledState == LOW) && (currentMillis - previousMillis >= OnTime))
      {
        ledState = HIGH;  // turn it on
        previousMillis = currentMillis;     // Remember the time
        digitalWrite(pin_rele_rain, ledState);    // Update the actual LED
      } else if ((ledState == HIGH) && (currentMillis - previousMillis >= OffTime))
      {
        ledState = LOW;  // Turn it off
        previousMillis = currentMillis;    // Remember the time
        digitalWrite(pin_rele_rain, ledState);   // Update the actual LED
      }
      break;
    case 0x02:                                    //Caso 2
      ledState = LOW;  // Turn it off
      previousMillis = currentMillis;    // Remember the time
      digitalWrite(pin_rele_rain, ledState);   // Update the actual LED
      break;                             //Break
  } //end switch set1
}

void menu1()
{
  lcd.setCursor(0, 0);
  lcd.print("Time:  ");
  lcd.print(rtc.getTimeStr());

  lcd.setCursor(0, 1);
  lcd.print("Date: ");
  lcd.print(rtc.getDateStr());
} //end menu1

void menu2()                                            //Função genérica para um quarto menu...
{
  lcd.setCursor(0, 0);
  lcd.print("mode - 1");
  lcd.setCursor(0, 1);
  lcd.print("fill the tank");

  estadoBotaoSTR = digitalRead (butSTR);
  if ((!estadoBotaoSTR && estadoAntBotaoSTR) && ((millis() - delayBouce2))) {
    estadoSTR++;
    if (estadoSTR > 2) {
      estadoSTR = 0;
      lcd.clear();
    }
    delayBouce2 = millis();
  }

  estadoAntBotaoSTR = estadoBotaoSTR;
  switch (estadoSTR)                                     //Controle do set1
  {
    case 0x01:                                        //Caso 1
      lcd.setCursor(0, 0);
      lcd.print("mode - 1");
      lcd.setCursor(0, 1);
      lcd.print("fill the tank");
      lcd.setCursor(12, 0);                     //Posiciona cursor na coluna 1, linha 2
      lcd.print("ON");                         //Imprime mensagem
      controlaTanque();
      break;
    case 0x02:                                    //Caso 2
      lcd.setCursor(0, 0);
      lcd.print("mode - 1");
      lcd.setCursor(0, 1);
      lcd.print("fill the tank");
      lcd.setCursor(12, 0);                   //Posiciona cursor na coluna 1, linha 2
      lcd.print("OFF");                       //Imprime mensagem
      digitalWrite(pin_led_red, LOW);
      digitalWrite(pin_led_green, LOW);
      digitalWrite(pin_rele_water, LOW);//invertido não sei porque
      digitalWrite(pin_rele_fert, LOW);//invertido não sei porque
      break;                                  //Break
  } //end switch set1
} //end menu2

void menu3()                                            //Função genérica para um quarto menu...
{
  lcd.setCursor(0, 0);
  lcd.print("mode - 2");
  lcd.setCursor(0, 1);
  lcd.print("read the sensors");
} //end menu3


// --- Loop Infinito ---
void loop()
{
  changeMenu();
  dispMenu();
} //end loop
