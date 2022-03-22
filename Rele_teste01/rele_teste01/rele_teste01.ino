#define pino_rele 10


void setup() {
  // put your setup code here, to run once:
  pinMode (pino_rele, OUTPUT);

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite ( pino_rele, LOW);
  delay (1000);
  
  digitalWrite ( pino_rele, HIGH);
  delay (500);

}
