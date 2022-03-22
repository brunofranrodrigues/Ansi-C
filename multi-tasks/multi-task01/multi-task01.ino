#define pinLed1 12
#define pinLed2 11
#define pinLed3 10

#define pinBotao1 8
#define pinBotao2 7

void controlaLed1();
void controlaLed2();
void controlaLed3();

void setup() {
  pinMode(pinLed1, OUTPUT);
  pinMode(pinLed2, OUTPUT);
  pinMode(pinLed3, OUTPUT);

  pinMode(pinBotao1, INPUT_PULLUP);
  pinMode(pinBotao2, INPUT_PULLUP);

}

void controlaLed1() {
static unsigned long delayPisca;

  if ( (millis() - delayPisca) < 500){
  digitalWrite(pinLed1, HIGH);
  } else {
  digitalWrite(pinLed1, LOW);
  }
  if ( (millis() - delayPisca) >= 1000){
  delayPisca = millis();
  }

}

void controlaLed2() {
  digitalWrite (pinLed2, !digitalRead (pinBotao1));
}

void controlaLed3() {
static byte estado = 1;
static unsigned long delayBouce;
static bool estadoBotao = true;
static bool estadoAntBotao = true;
static unsigned long delayPisca;

  estadoBotao = digitalRead (pinBotao2);
  if ((!estadoBotao && estadoAntBotao) && ((millis() - delayBouce))){
      estado++;
      if (estado > 3) {
          estado = 1;
      }
      delayBouce = millis();
 }
 
 estadoAntBotao = estadoBotao;

 switch (estado) {
  case 1: {
    digitalWrite (pinLed3, LOW);
    break;
  }
  
  case 2: {
    digitalWrite (pinLed3, HIGH);
    break;
  }
  
  case 3: {
    if ((millis() - delayPisca) < 200) {
       digitalWrite (pinLed3, HIGH);  
    }

    if ( ((millis() - delayPisca) >= 200) && ((millis() - delayPisca) < 400) ){
      digitalWrite (pinLed3, LOW);  
    }

    if ( ((millis() - delayPisca) >= 400) && ((millis() - delayPisca) < 600) ){
       digitalWrite (pinLed3, HIGH);  
    }

   if ( ((millis() - delayPisca) >= 600) && ((millis() - delayPisca) < 800) ){
       digitalWrite (pinLed3, LOW);  
    }

    if ((millis() - delayPisca) >= 1800){
        delayPisca = millis();
    }

    break;
  }
 
 }
 
}

void loop() {
  controlaLed1();
  controlaLed2();
  controlaLed3();

}









  
