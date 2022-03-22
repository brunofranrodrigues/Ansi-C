// Programa : Teste modulo wireless ESP8266
// Autor : Arduino e Cia

// Carrega as bibliotecas ESP8266 e SoftwareSerial
#include "ESP8266_Lib.h"
#include "SoftwareSerial.h"

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "MAUI";
char pass[] = "@dm!nm3a1x8@";

// Cria uma serial nas portas 2 (RX) e 3 (TX)
SoftwareSerial minhaSerial(2 , 3);

// Define que o modulo ira utilizar a serial minhaSerial
ESP8266 wifi(&minhaSerial);

#define ESP8266_BAUD 115200

void setup()
{
  minhaSerial.begin(ESP8266_BAUD);
  delay(10);
  Serial.begin(9600);
}

void loop(void)
{
  Serial.print("Inicializando modulo\r\n");
  Serial.print("Versao do firmware: ");
  Serial.println(wifi.getVersion().c_str());
  // Define modo de operacao como STA (station)
  if (wifi.setOprToStation()) {
    Serial.print("Modo STA ok\r\n");
  } else {
    Serial.print("Erro ao definir modo STA !r\n");
  }

  // Conexao a rede especificada em SSID
  if (wifi.joinAP(ssid, pass)) {
    Serial.print("Conectado com sucesso a rede wireless\r\n");
    Serial.print("IP: ");
    Serial.println(wifi.getLocalIP().c_str());
  } else {
    Serial.print("Erro ao conectar rede wireless !!!\r\n");
  }

  Serial.print("*** Fim ***\r\n");
  while (1) {}
}
