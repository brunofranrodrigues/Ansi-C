// Programa : Modulo I2C Display e Modulo DS1307
// Autor : Arduino e Cia

#include <DS3231.h>
#include <LiquidCrystal_I2C.h>

DS3231  rtc(SDA, SCL);

// Modulo I2C display no endereco 0x27
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3, POSITIVE);


void setup()
{
  
  Serial.begin(9600);
  lcd.begin (16,2);
  // Initialize the rtc object
  rtc.begin();
  
  // The following lines can be uncommented to set the date and time
  //rtc.setDOW(WEDNESDAY);     // Set Day-of-Week to SUNDAY
  //rtc.setTime(21, 5, 0);     // Set the time to 12:00:00 (24hr format)
  //rtc.setDate(1, 16, 2019);   // Set the date to January 1st, 2014
  //A linha abaixo pode ser retirada apos setar a data e hora
  //SelecionaDataeHora(); 
}

void loop() { 
 lcd.setCursor(0,0);
 lcd.print("Time:  ");
 lcd.print(rtc.getTimeStr());
 
 lcd.setCursor(0,1);
 lcd.print("Date: ");
 lcd.print(rtc.getDateStr());
 
 delay(1000); 
}
