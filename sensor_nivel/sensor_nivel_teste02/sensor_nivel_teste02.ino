//Programa: teste com dois sensores de nivel
//sensor_nivel_teste02


// Pino ligado ao sensor de nivel de liquido
int sensor_nivel_minimo = 4;

// Pino ligado ao sensor de nivel de liquido
int sensor_nivel_maximo = 5;

// Led verde
int pino_led_cheio = 6;

// Led vermelho
int pino_led_vazio = 7;

// Rele de acionamento da bomba
int pino_rele = 10;

//Variavel do sensor de nivel minimo
int estadoMinimo = 0;

//Variavel do sensor de nivel maximo
int estadoMaximo = 0;

//Variavel de controle do funcionamento da bomba
int estadoRele = 0; 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode (sensor_nivel_minimo, INPUT);
  pinMode (sensor_nivel_maximo, INPUT);
  pinMode (pino_led_cheio, OUTPUT);
  pinMode (pino_led_vazio, OUTPUT);
  pinMode (pino_rele, OUTPUT);
  
}

void loop() 
{
  estadoMinimo = digitalRead(sensor_nivel_minimo);
  estadoMaximo = digitalRead(sensor_nivel_maximo);

  if ((estadoMinimo == 0) && (estadoMaximo == 0) && (estadoRele == 0))
  {
  //Liga o led vermelho por que o tanque esta vazio
  digitalWrite(pino_led_vazio, HIGH);
  //Desliga o led verde por que o tanque esta vazio
  digitalWrite(pino_led_cheio, LOW);
  //Liga o rele para bombear agua pro tanque
  digitalWrite(pino_rele, LOW);//invertido não sei porque
  //Espera 200 mili para avaliar o proximo estado dos sensores
  estadoRele = 1; // relé ligado
  delay(200);
  
  }

  if ((estadoMinimo == 1) && (estadoMaximo == 0) && (estadoRele == 1))
  {
  //Desliga o led vermelho por que o tanque esta cheio
  digitalWrite(pino_led_vazio, LOW);
  delay(200);
  //Liga o led vermelho como um alarme
  digitalWrite(pino_led_vazio, HIGH);
  //Liga o led verde por que o tanque esta cheio
  digitalWrite(pino_led_cheio, HIGH);
  //Desliga o rele para bombear agua pro tanque porque esta cheio
  digitalWrite(pino_rele, LOW);//invertido não sei porque
  //Espera 200 mili para avaliar o proximo estado dos sensores
  estadoRele = 1; // relé ligado
  delay(200);
  }
  
  if ((estadoMinimo == 1) && (estadoMaximo == 1) && (estadoRele == 1))
  {
  //Desliga o led vermelho por que o tanque esta cheio
  digitalWrite(pino_led_vazio, LOW);
  //Liga o led verde por que o tanque esta cheio
  digitalWrite(pino_led_cheio, HIGH);
  //Desliga o rele para bombear agua pro tanque porque esta cheio
  digitalWrite(pino_rele, HIGH);//invertido não sei porque
  //Espera 200 mili para avaliar o proximo estado dos sensores
  estadoRele = 0; // relé ligado
  delay(200);
  }

  if ((estadoMinimo == 0) && (estadoMaximo == 1) && (estadoRele == 0 || estadoRele == 1))
  {
  //Desliga o led vermelho por que o tanque esta cheio
  digitalWrite(pino_led_vazio, LOW);
  //Liga o led verde por que o tanque esta cheio
  digitalWrite(pino_led_cheio, HIGH);
  //Desliga o rele para bombear agua pro tanque porque esta cheio
  digitalWrite(pino_rele, HIGH);//invertido não sei porque
  //Espera 200 mili para avaliar o proximo estado dos sensores
  estadoRele = 0; // relé ligado
  delay(200);
  }

delay(100);

}
