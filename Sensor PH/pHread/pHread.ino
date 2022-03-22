//pHread
const byte phpin = A3;

float Po;

void setup() {
  Serial.begin(9600);

}

void loop() {
  Po = (1023 - analogRead(phpin)) /73.07;
  Po = (Po + 1); //calibração
  Serial.println(Po, 2);
  delay(1000);
}
