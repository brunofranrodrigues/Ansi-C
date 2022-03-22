// --- Bibliotecas Auxiliares ---
#include <LiquidCrystal_I2C.h>
#include <DS3231.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

// --- Mapeamento de Hardware ---
#define butMOD    2                                     //Botão para selecionar tela acima no digital 2
#define butSTR    3                                     //Botão de start ou stop do ambiente

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

void controlaEC();

// --- Variáveis Globais ---
char menu = 0x01;                                       //Variável para selecionar o menu
boolean t_butMOD, t_butSTR;                             //Flags para armazenar o estado dos botões

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

float Temperature = 10;
float EC = 0;
float EC25 = 0;
int ppm = 0;

float raw = 0;
float Vin = 5;
float Vdrop = 0;
float Rc = 0;
float buffer = 0;

void setup() {
  Serial.begin(9600);
  rtc.begin();
  //Inicializa LCD 16 x 2
  lcd.begin (16, 2);

  pinMode(butMOD, INPUT_PULLUP);     //Entrada para os botões (digitais 2) com pull-ups internos
  pinMode(butSTR, INPUT_PULLUP);     //Entrada para os botões (digitais 3) com pull-ups internos

  //*********************************Setup - runs Once and sets pins etc ******************************************************//
  pinMode(ECPin, INPUT);
  pinMode(ECPower, OUTPUT); //Setting pin for sourcing current
  pinMode(ECGround, OUTPUT); //setting pin for sinking current
  digitalWrite(ECGround, LOW); //We can leave the ground connected permanantly

  t_butMOD   = 0x00;                                      //limpa flag do botão Mode
  t_butSTR   = 0x00;                                      //limpa flag do botão start

  delay(100);

  sensors.begin();// gives sensor time to settle

  delay(100);

  R1 = (R1 + Ra);

}

void loop() {
  changeMenu();
  dispMenu();

}

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
      lcd.clear();
      estadoSTR = 0;
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

      break;
    case 0x02:                                    //Caso 2
      lcd.setCursor(0, 0);
      lcd.print("mode - 1");
      lcd.setCursor(0, 1);
      lcd.print("fill the tank");
      lcd.setCursor(12, 0);                   //Posiciona cursor na coluna 1, linha 2
      lcd.print("OFF");                       //Imprime mensagem

      break;                                  //Break

  } //end switch set1

} //end menu2

void menu3()                                            //Função genérica para um quarto menu...
{
  lcd.setCursor(0, 0);
  lcd.print("mode - 2");
  lcd.setCursor(0, 1);
  lcd.print("read the sensors");

  estadoBotaoSTR = digitalRead (butSTR);
  if ((!estadoBotaoSTR && estadoAntBotaoSTR) && ((millis() - delayBouce2))) {
    estadoSTR2++;
    if (estadoSTR2 > 2) {
      lcd.clear();
      estadoSTR2 = 0;
    }
    delayBouce2 = millis();
  }

  estadoAntBotaoSTR = estadoBotaoSTR;

  switch (estadoSTR2)                                     //Controle do set1
  {
    case 0x01:                                        //Caso 1
      lcd.setCursor(0, 0);
      lcd.print("mode - 2");
      lcd.setCursor(0, 1);
      lcd.print("read the sensors");
      lcd.setCursor(12, 0);                     //Posiciona cursor na coluna 1, linha 2
      lcd.print("ON");                         //Imprime mensagem
      controlaEC();
      break;
    case 0x02:                                    //Caso 2
      lcd.setCursor(0, 0);
      lcd.print("mode - 2");
      lcd.setCursor(0, 1);
      lcd.print("read the sensors");
      lcd.setCursor(12, 0);                   //Posiciona cursor na coluna 1, linha 2
      lcd.print("OFF");                       //Imprime mensagem
      controlaEC();
      break;                                  //Break

  } //end switch set1

} //end menu3

void menu4()                                            //Função genérica para um quarto menu...
{
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

      break;
    case 0x02:                                    //Caso 2
      lcd.setCursor(0, 0);
      lcd.print("mode - 3");
      lcd.setCursor(0, 1);
      lcd.print("irrigation sys");
      lcd.setCursor(12, 0);                   //Posiciona cursor na coluna 1, linha 2
      lcd.print("OFF");                       //Imprime mensagem

      break;                                  //Break

  } //end switch set1

} //end menu3

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

switch (estadoSTR2)                                //Controle do set1
  {
  case 0x01:                                     //Caso 1

  if ( (millis() - delayEC) < 500) {

    GetEC();          //Calls Code to Go into GetEC() Loop [Below Main Loop] dont call this more that 1/5 hhz [once every five seconds] or you will polarise the water
    PrintReadingsEC();  // Cals Print routine [below main loop]

  }

  if ( (millis() - delayEC) >= 4500) {
    //Zera a variavel delayPisca com o tempo atual depois que passar 800 ms
    delayEC = millis();
  }
  break;
  case 0x02:
  Serial.print("Rc: ");
  
  Serial.print(" EC: ");
  
  Serial.print(" Simens  ");
  
  Serial.print(" ppm  ");
  
  Serial.println(" *C ");
  break;
  }  

}
//************************************** End Of Main Loop **********************************************************************//
