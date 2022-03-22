#include <DS3231.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

// Modulo I2C display no endereco 0x27
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);

// Pino ligado ao sensor de nivel minimo
#define sen_nivel_minimo 4

// Pino ligado ao sensor de nivel maximo
#define sen_nivel_maximo 5

// Pino ligado ao Led verde de sinalização de tanque cheio
#define pin_led_green 7

// Pino ligado ao Led vermelho de sinalização de tanque vazio
#define pin_led_red 6

// Rele de acionamento da bomba de agua potavel
#define pin_rele_water 8

// Rele de acionamento da bomba de liquido nutritivel
#define pin_rele_fert 9

// Rele de acionamento da bomba de irrigação
#define pin_rele_rain 10

#define pin_button_STR 50

#define pin_button_MOD 52

//************************* User Defined Variables ********************************************************//
 
 
//##################################################################################
//-----------  Do not Replace R1 with a resistor lower than 300 ohms    ------------
//##################################################################################
 
 
int R1= 1000;
int Ra=25; //Resistance of powering Pins
int ECPin= A0;
int ECGround=A1;
int ECPower =A2;
 
 
//*********** Converting to ppm [Learn to use EC it is much better**************//
// Hana      [USA]        PPMconverion:  0.5
// Eutech    [EU]          PPMconversion:  0.64
//Tranchen  [Australia]  PPMconversion:  0.7
// Why didnt anyone standardise this?
 
 
float PPMconversion=0.7;
 
 
//*************Compensating for temperature ************************************//
//The value below will change depending on what chemical solution we are measuring
//0.019 is generaly considered the standard for plant nutrients [google "Temperature compensation EC" for more info
float TemperatureCoef = 0.019; //this changes depending on what chemical we are measuring
 
 
 
 
//********************** Cell Constant For Ec Measurements *********************//
//Mine was around 2.9 with plugs being a standard size they should all be around the same
//But If you get bad readings you can use the calibration script and fluid to get a better estimate for K
float K=2.88;
 
 
 
 
//************ Temp Probe Related *********************************************//
#define ONE_WIRE_BUS 13          // Data wire For Temp Probe is plugged into pin 10 on the Arduino
const int TempProbePossitive =8;  //Temp Probe power connected to pin 9
const int TempProbeNegative=9;    //Temp Probe Negative connected to pin 8
 
 
 
 
//***************************** END Of Recomended User Inputs *****************************************************************//

void controlaEC();
void GetEC();
void PrintReadings();
void controlaTanque();
void controlaIrrigacao();


 
OneWire oneWire(ONE_WIRE_BUS);// Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature.
 
 
float Temperature=10;
float EC=0;
float EC25 =0;
int ppm =0;
 
 
float raw= 0;
float Vin= 5;
float Vdrop= 0;
float Rc= 0;
float buffer=0;

void setup() {
  Serial.begin(9600);
  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(SUNDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(19, 4, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(1, 6, 2019);   // Set the date to January 1st, 2014
  
  pinMode (sen_nivel_minimo, INPUT);
  pinMode (sen_nivel_maximo, INPUT);
  pinMode (pin_led_green, OUTPUT);
  pinMode (pin_led_red, OUTPUT);
  pinMode (pin_rele_water, OUTPUT);
  pinMode (pin_rele_fert, OUTPUT);
  pinMode (pin_rele_rain, OUTPUT);
   pinMode(pin_button_STR, INPUT_PULLUP);
  pinMode(pin_button_MOD, INPUT_PULLUP);

  //*********************************Setup - runs Once and sets pins etc ******************************************************//
  pinMode(TempProbeNegative , OUTPUT ); //seting ground pin as output for tmp probe
  digitalWrite(TempProbeNegative , LOW );//Seting it to ground so it can sink current
  pinMode(TempProbePossitive , OUTPUT );//ditto but for positive
  digitalWrite(TempProbePossitive , HIGH );
  pinMode(ECPin,INPUT);
  pinMode(ECPower,OUTPUT);//Setting pin for sourcing current
  pinMode(ECGround,OUTPUT);//setting pin for sinking current
  digitalWrite(ECGround,LOW);//We can leave the ground connected permanantly
  
  // Initialize the rtc object
  rtc.begin();
  
  // Initialize the lcd object
  lcd.begin (16,2);

  delay(100);
  
  sensors.begin();// gives sensor time to settle
  
  delay(100);
  
  R1=(R1+Ra);
 
}

//************ This Loop Is called From Main Loop************************//
void GetEC(){
 
 
//*********Reading Temperature Of Solution *******************//
sensors.requestTemperatures();// Send the command to get temperatures
Temperature=sensors.getTempCByIndex(0); //Stores Value in Variable
 
 
 
 
//************Estimates Resistance of Liquid ****************//
digitalWrite(ECPower,HIGH);
raw= analogRead(ECPin);
raw= analogRead(ECPin);// This is not a mistake, First reading will be low beause if charged a capacitor
digitalWrite(ECPower,LOW);
 
 
 
 
//***************** Converts to EC **************************//
Vdrop= (Vin*raw)/1024.0;
Rc=(Vdrop*R1)/(Vin-Vdrop);
Rc=Rc-Ra; //acounting for Digital Pin Resitance
EC = 1000/(Rc*K);
 
 
//*************Compensating For Temperaure********************//
EC25  =  EC/ (1+ TemperatureCoef*(Temperature-25.0));
ppm=(EC25)*(PPMconversion*1000);
 
 
;}
//************************** End OF EC Function ***************************//

//***This Loop Is called From Main Loop- Prints to serial usefull info ***//
void PrintReadings(){
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
void controlaEC(){
  static unsigned long delayEC;
 
 
if ( (millis() - delayEC) < 500){ 
 
    GetEC();          //Calls Code to Go into GetEC() Loop [Below Main Loop] dont call this more that 1/5 hhz [once every five seconds] or you will polarise the water
    PrintReadings();  // Cals Print routine [below main loop]
 
  }

if ( (millis() - delayEC) >= 4500){
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

if ((estadoMinimo == 0) && (estadoMaximo == 0) && (estadoRele == 0)){
estadoTank = 1;
}

if ((estadoMinimo == 1) && (estadoMaximo == 0) && (estadoRele == 1)){
estadoTank = 2;
}

if ((estadoMinimo == 1) && (estadoMaximo == 1) && (estadoRele == 1)){
estadoTank = 3;
}

if ((estadoMinimo == 0) && (estadoMaximo == 1) && (estadoRele == 0 || estadoRele == 1)){
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
  if ( (millis() - delayPisca) < 200){
  //Desliga o led vermelho por que o tanque esta cheio
  digitalWrite(pin_led_red, LOW);
  } else {
  //Liga o led vermelho como um alarme
  digitalWrite(pin_led_red, HIGH);
  }
  if ( (millis() - delayPisca) >= 800){
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
}

void loop() {
static byte estadoMenu = 1;
static byte estadoSTR = 0;
static unsigned long delayBouce1;
static unsigned long delayBouce2;
static bool estadoBotaoMOD = true;
static bool estadoAntBotaoMOD = true;
static bool estadoBotaoSTR = true;
static bool estadoAntBotaoSTR = true;

estadoBotaoMOD = digitalRead (pin_button_MOD);
  if ((!estadoBotaoMOD && estadoAntBotaoMOD) && ((millis() - delayBouce1))){
      estadoMenu++;
      if (estadoMenu > 4) {
          estadoMenu = 1;
      }
      delayBouce1 = millis();
 }

estadoAntBotaoMOD = estadoBotaoMOD;

estadoBotaoSTR = digitalRead (pin_button_STR);
  if ((!estadoBotaoSTR && estadoAntBotaoSTR) && ((millis() - delayBouce2))){
      estadoSTR++;
      if (estadoSTR > 2) {
          estadoSTR = 0;
      }
      delayBouce2 = millis();
 }

estadoAntBotaoSTR = estadoBotaoSTR;


switch (estadoMenu) {
  case 1: {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("choose the opt");
    lcd.setCursor(0,1);
    lcd.print("with mode botton");
    break;
  }
  case 2: {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("mode - 1");
    lcd.setCursor(0,1);
    lcd.print("fill the tank");
    if ((estadoMenu == 2) && (estadoSTR == 1)){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("mode - 1");
    lcd.setCursor(0,1);
    lcd.print("start the fill");
    }
    if ((estadoMenu == 2) && (estadoSTR == 2)){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("mode - 1");
    lcd.setCursor(0,1);
    lcd.print("stop the fill");
    }
    break;
  }
  case 3: {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("mode - 2");
    lcd.setCursor(0,1);
    lcd.print("read the sensors");
    if ((estadoMenu == 3) && (estadoSTR == 1)){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("mode - 1");
    lcd.setCursor(0,1);
    lcd.print("start read");
    }
    if ((estadoMenu == 3) && (estadoSTR == 2)){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("mode - 1");
    lcd.setCursor(0,1);
    lcd.print("stop read");
    }
    break;
  }
  case 4: {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("mode - 3");
    lcd.setCursor(0,1);
    lcd.print("irrigation sys");
    if ((estadoMenu == 4) && (estadoSTR == 1)){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("mode - 1");
    lcd.setCursor(0,1);
    lcd.print("start rain");
    }
    if ((estadoMenu == 4) && (estadoSTR == 2)){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("mode - 1");
    lcd.setCursor(0,1);
    lcd.print("stop rain");
    }
    break;
  }
 
 } 
  
}
