// Programa : Arduino Motor Shield - Controle motores DC
// Autor : Arduino e Cia
 
// carrega a biblioteca AFMotor
#include <AFMotor.h>

// Define o motor1 ligado a conexao 1
AF_DCMotor motor1(1); 
// Define o motor2 ligado a conexao 4
AF_DCMotor motor2(4); 
 
void setup()
{
  // Define a velocidade maxima para os motores 1 e 2
  motor1.setSpeed(255); 
  motor2.setSpeed(255); 

}
 
void loop()
{
  // Aciona o motor 1 no sentido horario
  motor1.run(FORWARD); 
  // Aciona o motor 2 no sentido anti-horario
  motor2.run(BACKWARD); 
 
  // Aguarda 5 segundos
  delay(5000);

  // Desliga os 2 motores
  motor1.run(RELEASE); 
  motor2.run(RELEASE); 

  // Aciona o motor 1 no sentido anti-horario
  motor1.run(BACKWARD);
  // Aciona o motor 2 no sentido horario
  motor2.run(FORWARD); 
 
  // Aguarda 5 segundos
  delay(5000);
  
  // Desliga os 2 motores
  motor1.run(RELEASE); 
  motor2.run(RELEASE);
}
