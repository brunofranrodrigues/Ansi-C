const int pinoSensor = 8; //PINO DIGITAL UTILIZADO PELA SAÍDA DO SENSOR
const int pinoLed = 12; //PINO DIGITAL UTILIZADO PELO LED
 
void setup() {
  Serial.begin(9600); //INICIALIZA A SERIAL
  pinMode (pinoSensor, INPUT); //DEFINE O PINO COMO ENTRADA
  pinMode(pinoLed, OUTPUT); //DEFINE O PINO COMO SAÍDA
  digitalWrite(pinoLed, LOW); //LED INICIA DESLIGADO
}
void loop() {
  if(digitalRead(pinoSensor) == HIGH){ //SE LEITURA DO PINO FOR IGUAL A 1 (HIGH), FAZ
    digitalWrite(pinoLed, HIGH); //ACENDE O LED
  }
  else{ //SENÃO, FAZ
    digitalWrite(pinoLed, LOW); //ACENDE O LED
  }
}
