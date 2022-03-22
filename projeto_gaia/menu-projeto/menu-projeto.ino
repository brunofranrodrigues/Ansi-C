// --- Bibliotecas Auxiliares ---
#include <LiquidCrystal_I2C.h>
#include <DS3231.h>

// Init the DS3231 using the hardware interface
DS3231  rtc(SDA, SCL);

// --- Mapeamento de Hardware ---
#define butMOD    2                                     //Botão para selecionar tela acima no digital 2
#define butSTR    3                                     //Botão de start ou stop do ambiente
#define butPlus   52                                     //Botão para selecionar tela acima no digital 2
#define butMinus  53                                     //Botão de start ou stop do ambiente


// Modulo I2C display no endereco 0x27
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

void changeMenu();                                      //Função para modificar o menu atual
void dispMenu();                                        //Função para mostrar o menu atual
void menu1();
void menu2();
void menu3();
void menu4();
void menu5();


// --- Variáveis Globais ---
char menu = 0x01;                                       //Variável para selecionar o menu
char subMenu = 0x01;                                       //Variável para selecionar o menu

boolean t_butMOD, t_butSTR, t_butPlus, t_butMinus;                             //Flags para armazenar o estado dos botões

static byte estadoSTR = 0;
static unsigned long delayBouce2;
static bool estadoBotaoSTR = true;
static bool estadoAntBotaoSTR = true;

static byte estadoPlus = 0;
static unsigned long delayBouce3;
static bool estadoBotaoPlus = true;
static bool estadoAntBotaoPlus = true;


void setup() {
  rtc.begin();
  //Inicializa LCD 16 x 2
  lcd.begin (16, 2);

  pinMode(butMOD, INPUT_PULLUP);     //Entrada para os botões (digitais 2) com pull-ups internos
  pinMode(butSTR, INPUT_PULLUP);     //Entrada para os botões (digitais 3) com pull-ups internos
  pinMode(butPlus, INPUT_PULLUP);     //Entrada para os botões (digitais 2) com pull-ups internos
  pinMode(butMinus, INPUT_PULLUP);     //Entrada para os botões (digitais 3) com pull-ups internos

  t_butMOD   = 0x00;                                      //limpa flag do botão Mode
  t_butSTR   = 0x00;                                      //limpa flag do botão start


}

void loop() {
  changeMenu();
  dispMenu();

}

void changeMenu()                                       //Modifica o menu atual
{
  if (!digitalRead(butMOD))   t_butMOD   = 0x01;       //Botão Up pressionado? Seta flag

  if (digitalRead(butMOD) && t_butMOD)                  //Botão Up solto e flag setada?
  { //Sim...
    t_butMOD = 0x00;                                  //Limpa flag

    lcd.clear();                                     //Limpa display
    menu++;                                           //Incrementa menu

    if (menu > 0x05) menu = 0x01;                     //Se menu maior que 4, volta a ser 1

  } //end butUp

} //end changeMenu

void dispMenu()                                         //Mostra o menu atual
{
  switch (menu)                                       //Controle da variável menu
  {
    case 0x01:                                       //Caso 1
      menu1();                                   //Chama a função de relógio

      break;                                     //break
    case 0x02:                                       //Caso 2
      menu2();                                   //Chama a função do termômetro

      break;                                     //break
    case 0x03:                                       //Caso 3
      menu3();                                   //Chama função para acionamento de lâmpadas

      break;                                     //break
    case 0x04:                                       //Caso 3
      menu4();                                   //Chama função para acionamento de lâmpadas

      break;                                     //break

    case 0x05:                                       //Caso 3
      menu5();                                   //Chama função para acionamento de lâmpadas

      break;                                     //break


  } //end switch menu

} //end dispMenu

void menu1()
{
  lcd.setCursor(0, 0);
  lcd.print("Time:  ");
  lcd.print(rtc.getTimeStr());

  lcd.setCursor(0, 1);
  lcd.print("Date: ");
  lcd.print(rtc.getDateStr());


} //end menu1

void menu2()                                            //Função genérica para um quarto menu...
{
  lcd.setCursor(0, 0);
  lcd.print("mode - 1");
  lcd.setCursor(0, 1);
  lcd.print("fill the tank");

  estadoBotaoSTR = digitalRead (butSTR);
  if ((!estadoBotaoSTR && estadoAntBotaoSTR) && ((millis() - delayBouce2))) {
    estadoSTR++;
    if (estadoSTR > 2) {
      lcd.clear();
      estadoSTR = 0;
    }
    delayBouce2 = millis();
  }

  estadoAntBotaoSTR = estadoBotaoSTR;
  switch (estadoSTR)                                     //Controle do set1
  {
    case 0x01:                                        //Caso 1
      lcd.setCursor(0, 0);
      lcd.print("mode - 1");
      lcd.setCursor(0, 1);
      lcd.print("fill the tank");
      lcd.setCursor(12, 0);                     //Posiciona cursor na coluna 1, linha 2
      lcd.print("ON");                         //Imprime mensagem

      break;
    case 0x02:                                    //Caso 2
      lcd.setCursor(0, 0);
      lcd.print("mode - 1");
      lcd.setCursor(0, 1);
      lcd.print("fill the tank");
      lcd.setCursor(12, 0);                   //Posiciona cursor na coluna 1, linha 2
      lcd.print("OFF");                       //Imprime mensagem

      break;                                  //Break

  } //end switch set1

} //end menu2

void menu3()                                            //Função genérica para um quarto menu...
{
  lcd.setCursor(0, 0);
  lcd.print("mode - 2");
  lcd.setCursor(0, 1);
  lcd.print("read the sensors");


} //end menu3

void menu4()                                            //Função genérica para um quarto menu...
{
  lcd.setCursor(0, 0);
  lcd.print("mode - 3");
  lcd.setCursor(0, 1);
  lcd.print("irrigation sys");

  estadoBotaoSTR = digitalRead (butSTR);
  if ((!estadoBotaoSTR && estadoAntBotaoSTR) && ((millis() - delayBouce2))) {
    estadoSTR++;
    if (estadoSTR > 2) {
      lcd.clear();
      estadoSTR = 0;
    }
    delayBouce2 = millis();
  }

  estadoAntBotaoSTR = estadoBotaoSTR;
  switch (estadoSTR)                                     //Controle do set1
  {
    case 0x01:                                        //Caso 1
      lcd.setCursor(0, 0);
      lcd.print("mode - 3");
      lcd.setCursor(0, 1);
      lcd.print("irrigation sys");
      lcd.setCursor(12, 0);                     //Posiciona cursor na coluna 1, linha 2
      lcd.print("ON");                         //Imprime mensagem

      break;
    case 0x02:                                    //Caso 2
      lcd.setCursor(0, 0);
      lcd.print("mode - 3");
      lcd.setCursor(0, 1);
      lcd.print("irrigation sys");
      lcd.setCursor(12, 0);                   //Posiciona cursor na coluna 1, linha 2
      lcd.print("OFF");                       //Imprime mensagem

      break;                                  //Break


  }

} //end menu4

void menu5()                                            //Função genérica para um quarto menu...
{
  lcd.setCursor(0, 0);
  lcd.print("mode - 4");
  lcd.setCursor(0, 1);
  lcd.print("change OnOff");

  estadoBotaoSTR = digitalRead (butSTR);
  if ((!estadoBotaoSTR && estadoAntBotaoSTR) && ((millis() - delayBouce2))) {
    estadoSTR++;
    if (estadoSTR > 2) {
      lcd.clear();
      estadoSTR = 0;
    }
    delayBouce2 = millis();
  }

  estadoAntBotaoSTR = estadoBotaoSTR;
  switch (estadoSTR)                                     //Controle do set1
  {
    case 0x01:                                        //Caso 1
      lcd.clear();    
      lcd.setCursor(0, 0);
      lcd.print("mode - 4");
      lcd.setCursor(0, 1);
      lcd.print("trocar ON");

      break;
    case 0x02:                                    //Caso 2
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("mode - 4");
      lcd.setCursor(0, 1);
      lcd.print("trocar OFF");

      break;                                  //Break


  }


} //end menu5
