#include <LiquidCrystal_I2C.h>


LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);



const int  buttonPin = 2;    // the pin that the Up pushbutton is attached to

const int  buttonPin1 = 3;    // the pin that the Down pushbutton is attached to


// Variables will change:


int buttonPushCounter = 0;   // counter for the number of button presses
int buttonState5 = 0;         // current state of the button
int buttonState6 = 0;         // current state of the button
int lastButtonState = 0;     // previous state of the button


void setup() {


  // initialize the button pin as a input:
   
   pinMode(buttonPin, INPUT_PULLUP);
   
   pinMode(buttonPin1, INPUT_PULLUP);
   
   
   lcd.begin(16,2);
   
   lcd.setCursor(0,1);
   
   lcd.print("Volume:");
   
}


void loop() {


   // read the pushbutton up input pin:
   
   buttonState5 = digitalRead(buttonPin);

  // compare the buttonState to its previous state

   if (buttonState5 != lastButtonState) {
     
     // if the state has changed, increment the counter
     
     if (buttonState5 == HIGH)
     
     {
       
      buttonPushCounter++;
      lcd.setCursor(7,1);
           
      lcd.print(buttonPushCounter);
     }
     
      delay(50);

   }
   // save the current state as the last state,
  //for next time through the loop
   lastButtonState = buttonState5;
   
     // read the pushbutton down input pin:
   
   buttonState6 = digitalRead(buttonPin1);

  // compare the buttonState to its previous state

   if (buttonState6 != lastButtonState) {
     
     // if the state has changed, decrement the counter
     
     if (buttonState6 == HIGH)
     
     {
       
      buttonPushCounter-=1;
      lcd.setCursor(7,1);
          
      lcd.print(buttonPushCounter);
     }
     
         delay(50);    
         
         
         if (buttonPushCounter < 10)
         
         {
             lcd.setCursor(8,1);
          
              lcd.print("  ");
         }
         
         
         if (buttonPushCounter <= 0)
         
         {
           
           
            lcd.setCursor(7,1);
            
            lcd.print("OFF");
            
         }
         
         
         
         if (buttonPushCounter >= 25)
         
         {
           
           lcd.setCursor(7,1);
            
           lcd.print("Max");
           
         }
         
        
   }
   
   
   // save the current state as the last state,

  //for next time through the loop

   lastButtonState = buttonState6; 
   
   
}
