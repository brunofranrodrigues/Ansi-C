void setup() {
  Serial.begin(9600);
  Serial1.begin(115200);
}
 
void loop() {
  // Ve se tem sinal na Serial1 e manda para a Serial
  if (Serial1.available()) {
    int inByte = Serial1.read();
    Serial.write(inByte);
  }
 
  // Analogamente, ve se tem sinal na Serial, e manda para Serial1
  if (Serial.available()) {
    int inByte = Serial.read();
    Serial1.write(inByte);
  }
}
