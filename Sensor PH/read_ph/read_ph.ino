#include <LiquidCrystal_I2C.h>
#include <DS3231.h>
#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperature.h>

void controlaEC();
void GetEC();

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

OneWire oneWire(ONE_WIRE_BUS);// Setup a oneWire instance to communicate with any OneWire devices
DallasTemperature sensors(&oneWire);// Pass our oneWire reference to Dallas Temperature.


const byte phpin = A3;
float phValue;


float Temperature = 10;
float EC = 0;
float EC25 = 0;
int ppm = 0;

float raw = 0;
float Vin = 5;
float Vdrop = 0;
float Rc = 0;
float buffer = 0;



// Modulo I2C display no endereco 0x27
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);


void setup() {
  Serial.begin(9600);
  //Inicializa LCD 16 x 2
  lcd.begin (16, 2);

  pinMode(ECPin, INPUT);
  pinMode(ECPower, OUTPUT); //Setting pin for sourcing current
  pinMode(ECGround, OUTPUT); //setting pin for sinking current
  digitalWrite(ECGround, LOW); //We can leave the ground connected permanantly

  delay(100);

  sensors.begin();// gives sensor time to settle

  delay(100);

  R1 = (R1 + Ra);
}

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

//************************************* Main Loop - controlaEC() ***************************************************************//

//Moved Heavy Work To subroutines so you can call them from main loop without cluttering the main loop
void controlaEC() {
  static unsigned long delayEC;

  if ( (millis() - delayEC) < 500) {

    GetEC();          //Calls Code to Go into GetEC() Loop [Below Main Loop] dont call this more that 1/5 hhz [once every five seconds] or you will polarise the water
    //PrintReadingsEC();  // Cals Print routine [below main loop]

  }

  if ( (millis() - delayEC) >= 4500) {
    //Zera a variavel delayPisca com o tempo atual depois que passar 800 ms
    delayEC = millis();
  }

}
//************************************** End Of Main Loop **********************************************************************//


void loop() {
  static unsigned long delayPisca;
  //*********Reading Temperature Of Solution *******************//
  sensors.requestTemperatures();// Send the command to get temperatures
  Temperature = sensors.getTempCByIndex(0); //Stores Value in Variable

  controlaEC();
  GetEC();

  phValue = (1023 - analogRead(phpin)) /73.07;

  Serial.print("PH = ");
  Serial.println(phValue, 2);





  lcd.setCursor(0, 0);
  lcd.print("mode - 2");
  lcd.setCursor(0, 1);
  lcd.print("read the sensors");



  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("mode - 2");
  lcd.setCursor(0, 1);
  lcd.print("PH:");
  lcd.print(phValue);



  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("mode - 2");
  lcd.setCursor(0, 1);
  lcd.print("Temp:");
  lcd.print(Temperature);
  lcd.print(" *C ");




  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("mode - 2");
  lcd.setCursor(0, 1);
  lcd.print("EC:");
  lcd.print(EC25);



  delay(500);
}
