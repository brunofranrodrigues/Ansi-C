// Programa : Teste sensor liquido Arduino
// Autor : Arduino e Cia

// Pino ligado ao sensor de nivel de liquido
int pinosensor = 3;
// Led verde
int pino_led_cheio = 6;
// Led vermelho
int pino_led_vazio = 7;

int pino_rele = 10;

void setup()
{
  Serial.begin(9600);
  pinMode(pinosensor, INPUT);
  pinMode(pino_led_cheio, OUTPUT);
  pinMode(pino_led_vazio, OUTPUT);
  pinMode (pino_rele, OUTPUT);
}

void loop()
{
  int estado = digitalRead(pinosensor);
  Serial.print("Estado sensor : ");
  Serial.println(estado);
  switch(estado)
  {
  case 0:
    digitalWrite(pino_led_vazio, LOW);
    digitalWrite(pino_led_cheio, HIGH);
    break;
  case 1:
    digitalWrite(pino_led_vazio, HIGH);
    digitalWrite(pino_led_cheio, LOW);
    digitalWrite(pino_rele, HIGH);
    break;
  }
  delay(100);
}
