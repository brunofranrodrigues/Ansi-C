// --- Bibliotecas Auxiliares ---
#include <LiquidCrystal_I2C.h>                              //Biblioteca para o lcdlay LCD

// --- Mapeamento de Hardware ---
#define butUp    2                                     //Botão para selecionar tela acima no digital 12
#define butDown  3                                     //Botão para selecionar tela abaixo no digital 11
#define butP     53                                     //Botão de ajuste mais no digital 10
#define butM     52                                     //Botão de ajuste menos no digital 9
#define but_select   50                                     //Botão de seleção no digital 8


// --- Protótipo das Funções Auxiliares ---
void changeMenu();                                      //Função para modificar o menu atual
void lcdMenu();                                        //Função para mostrar o menu atual
void data_hora();                                       //Função do menu1, data e hora
void temperatura();                                     //Função do menu2, temperatura
void lights();                                          //Função do menu3, acionamento de lampadas
void menu4();                                           //Função do menu4

void lcdSubMenu4();                                    //Função do sub menu4
void readSelect(char option);                           //Função de Leitura do botão select para seleção de subMenus
void subMenu4_1();                                      //Função para subMenu4_1
void subMenu4_2();                                      //Função para subMenu4_2
void subMenu4_3();                                      //Função para subMenu4_3


// --- Variáveis Globais ---
char menu = 0x01;                                       //Variável para selecionar o menu
char subMenu4 = 0x01;                                   //Variável para selecionar subMenu no menu4
char set1 = 0x00, set2 = 0x00;                          //Controle das lâmpadas
boolean t_butUp, t_butDown, t_butP, t_butM, t_select;   //Flags para armazenar o estado dos botões

// Modulo I2C lcdlay no endereco 0x27
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

// --- Configurações Iniciais ---
void setup()
{
  lcd.begin(16, 2);                                     //Inicializa LCD 16 x 2

  pinMode(butUp, INPUT_PULLUP);
  pinMode(butDown, INPUT_PULLUP);
  pinMode(butP, INPUT_PULLUP);
  pinMode(butM, INPUT_PULLUP);
  pinMode(but_select , INPUT_PULLUP);


  t_butUp   = 0x00;                                      //limpa flag do botão Up
  t_butDown = 0x00;                                      //limpa flag do botão Down
  t_butP    = 0x00;                                      //limpa flag do botão P
  t_butM    = 0x00;                                      //limpa flag do botão M
  t_select  = 0x00;                                      //limpa flag do botão select



} //end setup


// --- Loop Infinito ---
void loop()
{


  changeMenu();
  lcdMenu();

} //end loop

// --- Desenvolvimento das Funções Auxiliares ---
void changeMenu()                                       //Modifica o menu atual
{
  if (!digitalRead(butUp))   t_butUp   = 0x01;         //Botão Up pressionado? Seta flag
  if (!digitalRead(butDown)) t_butDown = 0x01;         //Botão Down pressionado? Seta flag

  if (digitalRead(butUp) && t_butUp)                   //Botão Up solto e flag setada?
  { //Sim...
    t_butUp = 0x00;                                   //Limpa flag

    lcd.clear();                                     //Limpa lcdlay
    menu++;                                           //Incrementa menu

    if (menu > 0x04) menu = 0x01;                     //Se menu maior que 4, volta a ser 1

  } //end butUp

  if (digitalRead(butDown) && t_butDown)               //Botão Down solto e flag setada?
  { //Sim...
    t_butDown = 0x00;                                 //Limpa flag

    lcd.clear();                                     //Limpa lcdlay
    menu--;                                           //Decrementa menu

    if (menu < 0x01) menu = 0x04;                     //Se menu menor que 1, volta a ser 4

  } //end butDown

} //end changeMenu

void lcdMenu()                                         //Mostra o menu atual
{
  switch (menu)                                       //Controle da variável menu
  {
    case 0x01:                                       //Caso 1
      data_hora();                               //Chama a função de relógio

      break;                                     //break
    case 0x02:                                       //Caso 2
      temperatura();                             //Chama a função do termômetro

      break;                                     //break
    case 0x03:                                       //Caso 3
      lights();                                  //Chama função para acionamento de lâmpadas

      break;                                     //break
    case 0x04:                                       //Caso 4
      menu4();                                   //Chama função para o menu4

      break;                                     //break

  } //end switch menu

} //end lcdMenu

void data_hora()                                        //Data e Hora (menu1)
{

  lcd.setCursor(0, 0);                                //Posiciona cursor na coluna 1, linha 1
  lcd.print("Data e Hora");                           //Imprime mensagem
  lcd.setCursor(0, 1);                                //Posiciona cursor na coluna 1, linha 2

  // Desenvolver uma função de data e hora...

  lcd.print("27/08/15  15:14");                       //Mostrador (apenas ilustrativo......)

} //end data_hora

void temperatura()                                      //Temperatura (menu2)
{
  lcd.setCursor(0, 0);                                //Posiciona cursor na coluna 1, linha 1
  lcd.print("Temperatura");                           //Imprime mensagem
  lcd.setCursor(1, 1);                                //Posiciona cursor na coluna 2, linha 2
  lcd.print("25 Celsius");                            //Mostrador (apenas ilustrativo......)

} //end temperatura()

void lights()                                           //Acionamento de lâmpadas (menu3)
{

  lcd.setCursor(0, 0);                                //Posiciona cursor na coluna 1, linha 1
  lcd.print("Acionar Lampadas");                      //Imprime mensagem do menu 3


  if (!digitalRead(butP))    t_butP    = 0x01;         //Botão P pressionado? Seta flag
  if (!digitalRead(butM))    t_butM    = 0x01;         //Botão M pressionado? Seta flag

  if (digitalRead(butP) && t_butP)                     //Botão P solto e flag setada?
  { //Sim...
    t_butP = 0x00;                                    //Limpa flag

    set1++;                                           //Incrementa set1

    if (set1 > 2) set1 = 0x01;                        //Se maior que 2, volta a ser 1

    switch (set1)                                     //Controle do set1
    {
      case 0x01:                                    //Caso 1
        lcd.setCursor(0, 1);                   //Posiciona cursor na coluna 1, linha 2
        lcd.print("L1 on ");                   //Imprime mensagem

        break;                                  //Break
      case 0x02:                                    //Caso 2
        lcd.setCursor(0, 1);                   //Posiciona cursor na coluna 1, linha 2
        lcd.print("L1 off");                   //Imprime mensagem

        break;                                  //Break

    } //end switch set1

  } //end butP

  if (digitalRead(butM) && t_butM)                     //Botão D solto e flag setada?
  { //Sim...
    t_butM = 0x00;                                    //Limpa flag

    set2++;                                           //Incrementa set2

    if (set2 > 2) set2 = 0x01;                        //Se maior que 2, volta a ser 1

    switch (set2)                                     //Controle do set2
    {
      case 0x01:                                    //Caso 1
        lcd.setCursor(8, 1);                   //Posiciona cursor na coluna 8, linha 2
        lcd.print("L2 on ");                   //Imprime mensagem

        break;                                  //Break
      case 0x02:                                    //Caso 2
        lcd.setCursor(8, 1);                   //Posiciona cursor na coluna 8, linha 2
        lcd.print("L2 off");                   //Imprime mensagem

        break;                                  //Break

    } //end switch set1

  } //end butM


} //end lights

void menu4()                                            //Função genérica para um quarto menu...
{
  lcd.setCursor(0, 0);                                //Posiciona cursor na coluna 1, linha 1
  lcd.print("Menu 4");                                //Imprime mensagem

  lcdSubMenu4();

} //end menu4


//pode-se implementar quantos menus quiser
//............


void lcdSubMenu4()                                     //Mostra o sub menu atual para o menu 4
{

  if (!digitalRead(butP))    t_butP    = 0x01;         //Botão P pressionado? Seta flag
  if (!digitalRead(butM))    t_butM    = 0x01;         //Botão M pressionado? Seta flag

  if (digitalRead(butP) && t_butP)                     //Botão P solto e flag setada?
  { //Sim...
    t_butP = 0x00;                                    //Limpa flag

    subMenu4++;                                       //incrementa subMenu4

    if (subMenu4 > 3) subMenu4 = 0x01;                //se maior que 3, volta a ser 1


  } //end butP

  if (digitalRead(butM) && t_butM)                     //Botão D solto e flag setada?
  { //Sim...
    t_butM = 0x00;                                    //Limpa flag

    subMenu4--;                                       //decrementa subMenu4

    if (subMenu4 < 1) subMenu4 = 0x03;                //se menor que 1, volta a ser 3



  } //end butM



  switch (subMenu4)                                   //Controle da variável subMenu
  {
    case 0x01:                                       //Caso 1
      lcd.setCursor(1, 1);                      //Posiciona cursor na coluna 2, linha 2
      lcd.print("Sub Menu 01");
      readSelect(1);                             //Lê botão select com parâmetro 1

      break;                                     //break
    case 0x02:                                       //Caso 2
      lcd.setCursor(1, 1);                      //Posiciona cursor na coluna 2, linha 2
      lcd.print("Sub Menu 02");
      readSelect(2);                             //Lê botão select com parâmetro 2

      break;                                     //break
    case 0x03:                                       //Caso 2
      lcd.setCursor(1, 1);                      //Posiciona cursor na coluna 2, linha 2
      lcd.print("Sub Menu 03");
      readSelect(3);                             //Lê botão select com parâmetro 3

      break;                                     //break


  } //end switch menu

} //end lcdMenu


void readSelect(char option)                            //Leitura do botão select para seleção de subMenus
{

  if (!digitalRead(but_select))     t_select    = 0x01;     //Botão select pressionado? Seta flag


  if (digitalRead(but_select) && t_select)                 //Botão select solto e flag setada?
  { //Sim...
    t_select = 0x00;                                //Limpa flag

    switch (option)
    {
      case 0x01: subMenu4_1(); break;

      case 0x02: subMenu4_2(); break;

      case 0x03: subMenu4_3(); break;

    } //end switch option

  } //end if


} //end readSelect


void subMenu4_1()
{
  while (1)
  {
    lcd.setCursor(0, 0);                                //Posiciona cursor na coluna 1, linha 1
    lcd.print("Tarefa 1");                              //Imprime mensagem
    lcd.setCursor(0, 1);                                //Posiciona cursor na coluna 1, linha 2
    lcd.print("Em funcionamento");                      //Mostrador (apenas ilustrativo......)
  }

} //end subMenu4_1


void subMenu4_2()
{
  while (1)
  {
    lcd.setCursor(0, 0);                                //Posiciona cursor na coluna 1, linha 1
    lcd.print("Tarefa 2");                              //Imprime mensagem
    lcd.setCursor(0, 1);                                //Posiciona cursor na coluna 1, linha 2
    lcd.print("Em funcionamento");                      //Mostrador (apenas ilustrativo......)
  }

} //end subMenu4_2


void subMenu4_3()
{
  while (1)
  {
    lcd.setCursor(0, 0);                                //Posiciona cursor na coluna 1, linha 1
    lcd.print("Tarefa 3");                              //Imprime mensagem
    lcd.setCursor(0, 1);                                //Posiciona cursor na coluna 1, linha 2
    lcd.print("Em funcionamento");                      //Mostrador (apenas ilustrativo......)
  }

} //end subMenu4_3
