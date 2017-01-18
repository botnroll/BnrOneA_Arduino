/* 
 This example was created by José Cruz (www.botnroll.com)
 on the 12th February 2014
 
 This code example is in the public domain. 
 http://www.botnroll.com

Description:
This program allows a complete diagnosis of the whole Bot'n Roll ONE A hardware
*/

#include <BnrOneA.h>   // Bot'n Roll ONE A library
#include <SPI.h>       // SPI communication library required by BnrOne.cpp
BnrOneA one;           // declaration of object variable to control the Bot'n Roll ONE A

//constants definition
#define SSPIN  2       // Slave Select (SS) pin for SPI communication

char var_menu[10][15]={"1-LED", "2-LCD", "3-Bateria", "4-Botoes", "5-Infraverm.", "6-Motores", "7-Buzzer"};

void setup() 
{
    Serial.begin(57600);     // set baud rate to 57600bps for printing values at serial monitor.
    one.spiConnect(SSPIN);   // start SPI communication module
    one.stop();              // stop motors
    one.lcd2("  ");          // clear LCD 2nd line
}

int menu(int opcao)
{
int button;
int flag=1;

  while (flag)
  {
    one.lcd1(var_menu[opcao]);
    one.lcd2(" ");
    button=one.readButton();
    switch (button)
    {
      case 1: opcao--; break;
      case 2: opcao++; break;
      case 3: flag=0 ; break;
    }
    opcao=constrain(opcao, 0, 6);
    while(button=one.readButton());
    delay(200);
  }
  one.lcd2("  ");
return opcao;
}

void rotina_LED()
{
static int flag=1;
int button;

  while ((button=one.readButton())==0)
  {
    one.lcd2(flag?"ON":"OFF");
    one.led(flag);             // turn LED ON/OFF
    delay(500);
    flag=!flag;
  }
  while(button=one.readButton());
  one.lcd2("  ");
}

void rotina_LCD()
{
static int flag=1;
int button;
char string[]="               www.botnroll.com                ";
int i=0;

  while ((button=one.readButton())==0)
  {
    one.lcd2(string+i++);
    delay(300);
    if (i>=31) i=0;
  }
  while(button=one.readButton());
  one.lcd2("  ");
}

void rotina_Bateria()
{
float battery;
int button;

  while ((button=one.readButton())==0)
  {
    battery=one.readBattery(); // read battery voltage
    one.lcd2("Battery V: ",battery); // print data on LCD line 2
    delay(100);
  }
  while(button=one.readButton());
  one.lcd2("  ");
}

void rotina_Botoes()
{
static int flag=1;
int button;

  while ((button=one.readButton())!=3)
  {
    one.lcd2("botao: ", button);
    delay(100);
  }
  while(button=one.readButton());
  one.lcd2("  ");
}

void rotina_IV()
{
static int flag=1;
int button;

  while ((button=one.readButton())==0)
  {
    one.obstacleEmitters(flag);
    one.lcd2(flag ? " IR Emitters ON " : " IR Emitters OFF");
    delay(500);
    flag=!flag;
  }
  while(button=one.readButton());
  one.lcd2("  ");
}

void rotina_Motors()
{
int button;
int velocidade=0;
int incremento=20;

  while ((button=one.readButton())==0)
  {
    one.lcd2("Veloc.", velocidade);
    one.move(velocidade,velocidade);
    if (velocidade>= 100 || velocidade<=-100) incremento*=-1;
    velocidade+=incremento; 
    delay(500);
  }
  while(button=one.readButton());
  one.stop();
  one.lcd2("      Stop");
  delay(500);
  one.lcd2("  ");
}

void rotina_Buzzer()
{
int melody[] = { 262, 196, 196, 196, 196, 0, 247, 262, 0};
int noteDurations[] = { 4, 8, 8, 4, 4, 4, 4, 4, 4 };
int thisNote = 0;
int button;

  while ((button=one.readButton())==0)
  {
    one.lcd2("Nota: ", melody[thisNote]);
    int noteDuration = 1000/noteDurations[thisNote];
    tone(9, melody[thisNote],noteDuration);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(9);
    thisNote++; if (thisNote>9) thisNote=0;
  }
  while(button=one.readButton());
  one.lcd2("  ");
}

void loop()
{
static int opcao=0;

  opcao=menu(opcao);
  switch(opcao)
  {
    case 0: rotina_LED();     break;
    case 1: rotina_LCD();     break;
    case 2: rotina_Bateria(); break;
    case 3: rotina_Botoes();  break;
    case 4: rotina_IV();      break;
    case 5: rotina_Motors();  break;
    case 6: rotina_Buzzer();  break;
  }
}

