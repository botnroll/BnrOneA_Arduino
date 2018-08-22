/* 
 This example was created by José Cruz on June 2017 
 
 This code example is in the public domain. 
 http://www.botnroll.com

This program has a menu and several smaller programs in it.
To use all the functions the Bot´n Roll ONE A must be equipped with:
 - Rescue module (2 RGB sensors and 3 sonar's)
 - Line sensor
 - RGB LED
 - Shield with 3 trimmers on A0 A1 A2

*/
#include <BnrOneA.h>   // Bot'n Roll ONE A library
#include <EEPROM.h>    // EEPROM reading and writing
#include <SPI.h>       // SPI communication library required by BnrOne.cpp
#include <Wire.h>       // I2C communication library required by BnrRescue.cpp
#include <BnrRescue.h>   // Bot'n Roll CoSpace Rescue Module library

BnrOneA one;           // declaration of object variable to control the Bot'n Roll ONE A
BnrRescue brm;         // declaration of object variable to control Bot'n Roll Rescue Module

//definição de constantes
#define SSPIN  2    //Slave Select (SS) pin for SPI communication
#define MODULE_ADDRESS 0x2C        //default factory address
#define BROADCAST_ADDRESS 0x00      //i2c broadcast address
//#define M1  1       //Motor1
//#define M2  2       //Motor2
#define LINHA       1       //program to be executed on the menu
#define OBSTACULOS  2       //programa a executar no menu
#define CORLINHA    3
#define LEDBUTTON   4
#define LEDTRIMMER  5

#define EXIT        99      //Exit menu / Sair do menu

//float batmin=10.5;  // safety voltage for discharging the battery
int programa=0;       // selected program to be executed
int botao=0;          // O botão que foi pressionado
unsigned long tempo=0;
unsigned long tempoRGB=0;

#define LIMITES 100.0
double kp=1.3, ki=0.0013, kd=0.35;// PID Gain / Ganho PID
int vel=60;
int vCurve=3;

//Calibrate Line / Calibrar linha
#define VMAX 1000

//Obstacles avoidance variables / Variaveis para o programa de obstaculos
#define MIN_DIST 20    //Limit distance to be considered an obstacle. Higher values are not considered as obstacles.
                       //Distancia a partir da qual é considerada obstaculo. Valores acima não são considerados como obstáculos.
byte sonarL=0, sonarC=0, sonarR=0;
byte rgbL[3]={0};
byte rgbR[3]={0};

int maxSpeed=50;
int ctrlDist=MIN_DIST; //Control Distance / Distancia de controlo
int slope=0; //Resposta dos motores à variação da distancia. -> -10 a +10
double distWheel=double(100+slope)/double(ctrlDist); //Factor que converte a distância definida em valores para motores entre -100 a 100.

//Color identification / variáveis para identificação de cores
byte RSE[8]={0,0,0,0,0,0,0,0};  //Sensor esquerdo Red / Left sensor Red
byte GSE[8]={0,0,0,0,0,0,0,0};  //Sensor esquerdo Green / Left sensor Green
byte BSE[8]={0,0,0,0,0,0,0,0};  //Sensor esquerdo B / Left sensor Blue
byte RSD[8]={0,0,0,0,0,0,0,0};  //Sensor direito Red / Right sensor Red
byte GSD[8]={0,0,0,0,0,0,0,0};  //Sensor direito G / Right sensor Green
byte BSD[8]={0,0,0,0,0,0,0,0};  //Sensor direito B / Right sensor Blue
byte RLED[8]={96,132,132,96, 0, 0, 0,62}; //LED Red
byte GLED[8]={63, 39, 12, 0,38, 0,40, 0}; //LED G
byte BLED[8]={32,  0,  0, 0, 0,38,38,34}; //LED B

#define FLOOR 0     //Chão
#define COLOR1 1    //
#define COLOR2 2    //
#define COLOR3 3    //
#define COLOR4 4    //
#define COLOR5 5    //
#define COLOR6 6    //
#define COLOR7 7    //
#define UNDEFINED 8 //Cor não definida
unsigned int colorL=FLOOR;
unsigned int colorR=FLOOR;
#define R  0
#define G  1
#define B  2
#define LIM 10 //10

//LED RGB
#define LEDR    3
#define LEDG    5
#define LEDB    6


void setup() 
{  
  Serial.begin(57600);     // sets baud rate to 57600bps for printing values at serial monitor.
  one.spiConnect(SSPIN);   // starts the SPI communication module 
  brm.i2cConnect(MODULE_ADDRESS);
  brm.setSonarStatus(ON);
  brm.setRgbStatus(ON);
  one.stop();              // stop motors
  delay(500);
  one.minBat(10.5);      // safety voltage for discharging the battery
  startMenu();
  while(one.readButton()!=0);
}

void startMenu()
{
  int state=0, substate=0;
  while(state!=EXIT)
  {
    botao=one.readButton();
    if(botao==1) 
    {
      state++;
      substate=0;
      if(state>7) state=0;
    }
    if(botao==2) 
    {
      state--;
      substate=0;
      if(state<0) state=7;           
    }
    one.lcd2("PB1<>PB2 PB3-Sel");
    if(state==0)
    {
        one.lcd1("Follow Line");
        if(botao==3) 
        {  
          programa=LINHA;
          readLineEEPROM();
          state=EXIT;
        }
    }
    else if(state==1)
    {    
        one.lcd1("Avoid Obstacles");
        if(botao==3) 
        {  
          programa=OBSTACULOS;
          readObstaclesEEPROM();
          state=EXIT;
        }
    }
    else if(state==2)
    {    
        one.lcd1("Line & Colors");
        if(botao==3) 
        {          
           readLineEEPROM();
           readSensorColorsEEPROM();
           readLEDColorsEEPROM();
           programa=CORLINHA;
           state=EXIT;
        }
    }
    else if(state==3)
    {
        one.lcd1("RGB LED Buttons");
        if(botao==3) 
        {          
           programa=LEDBUTTON;
           state=EXIT;
        }
    }
    else if(state==4)
    {
        one.lcd1("RGB LED Trimmers");
        if(botao==3) 
        {          
           programa=LEDTRIMMER;
           state=EXIT;
        }      
    }
    else if(state==5)
    {    
        one.lcd1("Calib RGB Sensrs");
        if(botao==3) 
        {  
            menuCalibraCoresSensor();
        }
    }
    else if(state==6)
    {    
        one.lcd1("Calib RGB LED");
        if(botao==3) 
        {  
            menuCalibraCoresLED();
        }
    }
    else if(state==7)
    {    
        if(substate==0)one.lcd1("Calibrate Line");
        if(botao==3) 
        {           
          calibrateLine();         // Calibração do sensor de linha / Calibrate line sensor
          substate=1;
          while(one.readButton()!=0);
        }
        else if(botao==0)
        {
          int line=one.readLine(); //Ler a linha / Read line value
          if(substate==1)one.lcd1("Line:",line); //Apresenta valores no LCD / Print on LCD
        }
    }
    if(botao==1)
        while(one.readButton()==1); //Libertar botões / Release buttons
    if(botao==2)
        while(one.readButton()==2);     
    if(botao==3)
        while(one.readButton()==3);
    delay(50);
  }
   tempo=millis();
   tempoRGB=tempo;
}

void menuCalibraCoresLED()
{
 int state=1;
 int pred, pgreen, pblue;
  while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
  while(state!=EXIT)
  {
    delay(25);
    botao=one.readButton();
    if(botao==1) 
    {
      state++;
      if(state>7) state=0;
    }
    if(botao==2) 
    {
      state--;
      if(state<0) state=7;
    }
    
    if(state==1)
    {
        one.lcd1("Color1");
        pred=(1023-analogRead(0))/4;
        pgreen=(1023-analogRead(1))/4;
        pblue=(1023-analogRead(2))/4;
        
        one.lcd2(pred,pgreen,pblue);
        analogWrite(LEDR, (byte)pred);
        analogWrite(LEDG, (byte)pgreen);
        analogWrite(LEDB, (byte)pblue);
        if(botao==3) 
        {  
          RLED[state]=(byte)pred;
          GLED[state]=(byte)pgreen;
          BLED[state]=(byte)pblue;
          //printLEDColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }    
    else if(state==2)
    {
        one.lcd1("Color2");
        pred=(1023-analogRead(0))/4;
        pgreen=(1023-analogRead(1))/4;
        pblue=(1023-analogRead(2))/4;
        
        one.lcd2(pred,pgreen,pblue);
        analogWrite(LEDR, (byte)pred);
        analogWrite(LEDG, (byte)pgreen);
        analogWrite(LEDB, (byte)pblue);
        if(botao==3) 
        {  
          RLED[state]=(byte)pred;
          GLED[state]=(byte)pgreen;
          BLED[state]=(byte)pblue;
          //printLEDColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }
   else if(state==3)
    {
        one.lcd1("Color3");
        pred=(1023-analogRead(0))/4;
        pgreen=(1023-analogRead(1))/4;
        pblue=(1023-analogRead(2))/4;
        
        one.lcd2(pred,pgreen,pblue);
        analogWrite(LEDR, (byte)pred);
        analogWrite(LEDG, (byte)pgreen);
        analogWrite(LEDB, (byte)pblue);
        if(botao==3) 
        {  
          RLED[state]=(byte)pred;
          GLED[state]=(byte)pgreen;
          BLED[state]=(byte)pblue;
          //printLEDColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }
    else if(state==4)
    {
        one.lcd1("Color4");
        pred=(1023-analogRead(0))/4;
        pgreen=(1023-analogRead(1))/4;
        pblue=(1023-analogRead(2))/4;
        
        one.lcd2(pred,pgreen,pblue);
        analogWrite(LEDR, (byte)pred);
        analogWrite(LEDG, (byte)pgreen);
        analogWrite(LEDB, (byte)pblue);
        if(botao==3) 
        {  
          RLED[state]=(byte)pred;
          GLED[state]=(byte)pgreen;
          BLED[state]=(byte)pblue;
          //printLEDColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }  
    else if(state==5)
    {
        one.lcd1("Color5");
        pred=(1023-analogRead(0))/4;
        pgreen=(1023-analogRead(1))/4;
        pblue=(1023-analogRead(2))/4;
        
        one.lcd2(pred,pgreen,pblue);
        analogWrite(LEDR, (byte)pred);
        analogWrite(LEDG, (byte)pgreen);
        analogWrite(LEDB, (byte)pblue);
        if(botao==3) 
        {  
          RLED[state]=(byte)pred;
          GLED[state]=(byte)pgreen;
          BLED[state]=(byte)pblue;
          //printLEDColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }
    else if(state==6)
    {
        one.lcd1("Color6");
        pred=(1023-analogRead(0))/4;
        pgreen=(1023-analogRead(1))/4;
        pblue=(1023-analogRead(2))/4;
        
        one.lcd2(pred,pgreen,pblue);
        analogWrite(LEDR, (byte)pred);
        analogWrite(LEDG, (byte)pgreen);
        analogWrite(LEDB, (byte)pblue);
        if(botao==3) 
        {  
          RLED[state]=(byte)pred;
          GLED[state]=(byte)pgreen;
          BLED[state]=(byte)pblue;
          //printLEDColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }
    else if(state==7)
    {
        one.lcd1("Color7");
        pred=(1023-analogRead(0))/4;
        pgreen=(1023-analogRead(1))/4;
        pblue=(1023-analogRead(2))/4;
        
        one.lcd2(pred,pgreen,pblue);
        analogWrite(LEDR, (byte)pred);
        analogWrite(LEDG, (byte)pgreen);
        analogWrite(LEDB, (byte)pblue);
        if(botao==3) 
        {  
          RLED[state]=(byte)pred;
          GLED[state]=(byte)pgreen;
          BLED[state]=(byte)pblue;
          //printLEDColors();
          state=0;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }
   else if(state==0)
   {    
        one.lcd1(" Save and Exit  ");
        if(botao==3) 
        {  
          //Save on EEPROM
          writeLEDColorsEEPROM();
          state=EXIT;
          while(one.readButton()==3);//Espera que largue o botão  / Wait until button is released
        }
    }
    if(botao==1)
        while(one.readButton()==1); //Libertar botões  / release buttons
    if(botao==2)
        while(one.readButton()==2);      
  }
}

void menuCalibraCoresSensor()
{
  int state=0;
  while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
  while(state!=EXIT)
  {
    delay(25);
    botao=one.readButton();
    if(botao==1) 
    {
      state++;
      if(state>16) state=0;
    }
    if(botao==2) 
    {
      state--;
      if(state<0) state=16;
    }
    if(state==0)
    {
        one.lcd1("Col floor L");
        brm.readRgbL(&RSE[0],&GSE[0],&BSE[0]); 
        one.lcd2(RSE[0],GSE[0],BSE[0]);
        if(botao==3) 
        {  
  //        printColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão/ Wait until button is released
        }
    }
    else if(state==1)
    {    
        one.lcd1("Col floor R");
        brm.readRgbR(&RSD[0],&GSD[0],&BSD[0]); 
        one.lcd2(RSD[0],GSD[0],BSD[0]);
        if(botao==3) 
        {  
 //         printColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão/ Wait until button is released
        }
    }
    else if(state==2)
    {
        one.lcd1("Col1 L");
        brm.readRgbL(&RSE[1],&GSE[1],&BSE[1]); 
        one.lcd2(RSE[1],GSE[1],BSE[1]);
        if(botao==3) 
        {  
 //         printColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão/ Wait until button is released
        }
    }
    else if(state==3)
    {    
        one.lcd1("Col1 R");
        brm.readRgbR(&RSD[1],&GSD[1],&BSD[1]);  
        one.lcd2(RSD[1],GSD[1],BSD[1]);
        if(botao==3) 
        {   
//          printColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }
    else if(state==4)
    {
        one.lcd1("Col2 L");
        brm.readRgbL(&RSE[2],&GSE[2],&BSE[2]); 
        one.lcd2(RSE[2],GSE[2],BSE[2]);
        if(botao==3) 
        {  
//          printColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }
    else if(state==5)
    {    
        one.lcd1("Col2 R");
        brm.readRgbR(&RSD[2],&GSD[2],&BSD[2]);
        one.lcd2(RSD[2],GSD[2],BSD[2]);
        if(botao==3) 
        {  
//          printColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }    
   else if(state==6)
    {
        one.lcd1("Col3 L");
        brm.readRgbL(&RSE[3],&GSE[3],&BSE[3]); 
        one.lcd2(RSE[3],GSE[3],BSE[3]);
        if(botao==3) 
        {  
//          printColors();
          state++; 
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }
    else if(state==7)
    {    
        one.lcd1("Col3 R");
        brm.readRgbR(&RSD[3],&GSD[3],&BSD[3]); 
        one.lcd2(RSD[3],GSD[3],BSD[3]);
        if(botao==3) 
        {  
//          printColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }    
    
    else if(state==8)
    {
        one.lcd1("Col4 L");
        brm.readRgbL(&RSE[4],&GSE[4],&BSE[4]); 
        one.lcd2(RSE[4],GSE[4],BSE[4]);
        if(botao==3) 
        {  
//          printColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }
    else if(state==9)
    {    
        one.lcd1("Col4 R");
        brm.readRgbR(&RSD[4],&GSD[4],&BSD[4]); 
        one.lcd2(RSD[4],GSD[4],BSD[4]);
        if(botao==3) 
        {  
//          printColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }     
    else if(state==10)
    {
        one.lcd1("Col5 L");
        brm.readRgbL(&RSE[5],&GSE[5],&BSE[5]); 
        one.lcd2(RSE[5],GSE[5],BSE[5]);
        if(botao==3) 
        {  
//          printColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }
    else if(state==11)
    {    
        one.lcd1("Col5 R");
        brm.readRgbR(&RSD[5],&GSD[5],&BSD[5]); 
        one.lcd2(RSD[5],GSD[5],BSD[5]);
        if(botao==3) 
        {  
 //         printColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }          
    else if(state==12)
    {
        one.lcd1("Col6 L");
        brm.readRgbL(&RSE[6],&GSE[6],&BSE[6]); 
        one.lcd2(RSE[6],GSE[6],BSE[6]);
        if(botao==3) 
        {  
//          printColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }
    else if(state==13)
    {    
        one.lcd1("Col6 R");
        brm.readRgbR(&RSD[6],&GSD[6],&BSD[6]); 
        one.lcd2(RSD[6],GSD[6],BSD[6]);
        if(botao==3) 
        {  
//          printColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }         
    else if(state==14)
    {
        one.lcd1("Col7 L");
        brm.readRgbL(&RSE[7],&GSE[7],&BSE[7]); 
        one.lcd2(RSE[7],GSE[7],BSE[7]);
        if(botao==3) 
        {  
//          printColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }
    else if(state==15)
    {    
        one.lcd1("Col7 R");
        brm.readRgbR(&RSD[7],&GSD[7],&BSD[7]); 
        one.lcd2(RSD[7],GSD[7],BSD[7]);
        if(botao==3) 
        {  
//          printColors();
          state++;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
       }
    }      
   else if(state==16)
    {    
        one.lcd1("Save and Exit");
        if(botao==3) 
        {  
          //Save on EEPROM
          writeSensorColorsEEPROM();
 //         printColors();
          state=EXIT;
          while(one.readButton()==3);//Espera que largue o botão / Wait until button is released
        }
    }
    if(botao==1)
        while(one.readButton()==1); //Libertar botões
    if(botao==2)
        while(one.readButton()==2);        
  }
}
/********************************************************************************/
void writeSensorColorsEEPROM()
{
 //Escrever cores do sensor na EEPROM
   byte eepromADD=50;
   for(int i=0;i<8;i++)
   {
       EEPROM.write(eepromADD,RSE[i]);
       eepromADD++;
       EEPROM.write(eepromADD,GSE[i]);
       eepromADD++;
       EEPROM.write(eepromADD,BSE[i]);
       eepromADD++;
       EEPROM.write(eepromADD,RSD[i]);
       eepromADD++;
       EEPROM.write(eepromADD,GSD[i]);
       eepromADD++;
       EEPROM.write(eepromADD,BSD[i]);
       eepromADD++;
   }
}

void readSensorColorsEEPROM()
{
 //Ler cores do sensor da EEPROM
   byte eepromADD=50;
   for(int i=0;i<8;i++)
   {
       RSE[i]=EEPROM.read(eepromADD);
       eepromADD++;
       GSE[i]=EEPROM.read(eepromADD);
       eepromADD++;
       BSE[i]=EEPROM.read(eepromADD);
       eepromADD++;
       RSD[i]=EEPROM.read(eepromADD);
       eepromADD++;
       GSD[i]=EEPROM.read(eepromADD);
       eepromADD++;
       BSD[i]=EEPROM.read(eepromADD);
       eepromADD++;
   }
//   printColors();
}

/********************************************************************************/
void writeLEDColorsEEPROM()
{
 //Escrever cores do LED na EEPROM
   byte eepromADD=20;

   EEPROM.write(eepromADD,1);
   eepromADD++;
   for(int i=0;i<8;i++)
   {
       EEPROM.write(eepromADD,RLED[i]);
       eepromADD++;
       EEPROM.write(eepromADD,GLED[i]);
       eepromADD++;
       EEPROM.write(eepromADD,BLED[i]);
       eepromADD++;
   }
}

void readLEDColorsEEPROM()
{
 //Ler cores do LED da EEPROM
   byte eepromADD=20;
   if(EEPROM.read(eepromADD)==1)
   {
     eepromADD++;
     for(int i=0;i<8;i++)
     {
         RLED[i]=EEPROM.read(eepromADD);
         eepromADD++;
         GLED[i]=EEPROM.read(eepromADD);
         eepromADD++;
         BLED[i]=EEPROM.read(eepromADD);
         eepromADD++;
     }
   }
//   printLEDColors();
}

void loop() 
{
  if(programa==LINHA)  lineFollow();
  else if(programa==CORLINHA) corLinha();
  else if(programa==OBSTACULOS)  obstaculos();  
  else if(programa==LEDBUTTON) ledButton();
  else if(programa==LEDTRIMMER) ledTrimmer();

  if(millis()>=tempo) //A cada segundo
  {
      float battery=one.readBattery();
      one.lcd2("Battery V: ",battery); // prints data on LCD line 2
      tempo+=1000;
  }
}

void ledButton()
{
   int Red=0, Green=0, Blue=0, state=0;
   int inc=1;
   while(1)
   {
//      delay(50);
      if(state==0)
      {
          one.lcd1("COLOR3");
          if(one.readButton()==1)
          {
              Red+=inc;
              if(Red>255) Red=255;
          }
          if(one.readButton()==2)
          {
              Red-=inc;
              if(Red<0) Red=0;
          }
      }
      
      else if(state==1)
      {
          one.lcd1("COLOR4");
          if(one.readButton()==1)
          {
              Green+=inc;
              if(Green>255) Green=255;
          }
          if(one.readButton()==2)
          {
              Green-=inc;
              if(Green<0) Green=0;
          }
      }
      
      else if(state==2)
      {
          one.lcd1("COLOR6");
          if(one.readButton()==1)
          {
              Blue+=inc;
              if(Blue>255) Blue=255;
          }
          if(one.readButton()==2)
          {
              Blue-=inc;
              if(Blue<0) Blue=0;
          }
      }
      if(one.readButton()==3)
      {
        state++;
        if(state>2) state=0;
        while(one.readButton()==3);
      }      
      one.lcd2(Red,Green,Blue);      
      analogWrite(LEDR, (byte)Red);
      analogWrite(LEDG, (byte)Green);
      analogWrite(LEDB, (byte)Blue);
   }
}

void ledTrimmer()
{
   int pred, pgreen, pblue;
   while(1)
   {
      one.lcd1("RED  GREEN  BLUE ");
      pred=(1023-analogRead(0))/4;
      pgreen=(1023-analogRead(1))/4;
      pblue=(1023-analogRead(2))/4;
      
      one.lcd2(pred,pgreen,pblue);
      analogWrite(LEDR, (byte)pred);
      analogWrite(LEDG, (byte)pgreen);
      analogWrite(LEDB, (byte)pblue);
   }  
}

void corLinha()
{
   lineFollow();
   if(millis()>=tempoRGB) //A cada 30ms ler sensores RGB
   {
      //Ler sensores RGB
      brm.readRgbL(&rgbL[R],&rgbL[G],&rgbL[B]);    
      colorTestL();
      
      brm.readRgbR(&rgbR[R],&rgbR[G],&rgbR[B]);    
      colorTestR();
      colorAction();
      tempoRGB+=30;
   }
}

/*************************************************************************************************************************************/
void colorTestL()
{
  if(abs(rgbL[R]-RSE[FLOOR])<LIM && abs(rgbL[G]-GSE[FLOOR])<LIM && abs(rgbL[B]-BSE[FLOOR])<LIM) colorL=FLOOR;
  else if(abs(rgbL[R]-RSE[COLOR1])<LIM && abs(rgbL[G]-GSE[COLOR1])<LIM && abs(rgbL[B]-BSE[COLOR1])<LIM) colorL=COLOR1;
  else if(abs(rgbL[R]-RSE[COLOR2])<LIM && abs(rgbL[G]-GSE[COLOR2])<LIM && abs(rgbL[B]-BSE[COLOR2])<LIM) colorL=COLOR2;
  else if(abs(rgbL[R]-RSE[COLOR3])<LIM && abs(rgbL[G]-GSE[COLOR3])<LIM && abs(rgbL[B]-BSE[COLOR3])<LIM) colorL=COLOR3;
  else if(abs(rgbL[R]-RSE[COLOR4])<LIM && abs(rgbL[G]-GSE[COLOR4])<LIM && abs(rgbL[B]-BSE[COLOR4])<LIM) colorL=COLOR4;
  else if(abs(rgbL[R]-RSE[COLOR5])<LIM && abs(rgbL[G]-GSE[COLOR5])<LIM && abs(rgbL[B]-BSE[COLOR5])<LIM) colorL=COLOR5;
  else if(abs(rgbL[R]-RSE[COLOR6])<LIM && abs(rgbL[G]-GSE[COLOR6])<LIM && abs(rgbL[B]-BSE[COLOR6])<LIM) colorL=COLOR6;
  else if(abs(rgbL[R]-RSE[COLOR7])<LIM && abs(rgbL[G]-GSE[COLOR7])<LIM && abs(rgbL[B]-BSE[COLOR7])<LIM) colorL=COLOR7;
  else colorL=UNDEFINED;
}
void colorTestR()
{
  if(abs(rgbR[R]-RSD[FLOOR])<LIM && abs(rgbR[G]-GSD[FLOOR])<LIM && abs(rgbR[B]-BSD[FLOOR])<LIM) colorR=FLOOR;
  else if(abs(rgbR[R]-RSD[COLOR1])<LIM && abs(rgbR[G]-GSD[COLOR1])<LIM && abs(rgbR[B]-BSD[COLOR1])<LIM) colorR=COLOR1;
  else if(abs(rgbR[R]-RSD[COLOR2])<LIM && abs(rgbR[G]-GSD[COLOR2])<LIM && abs(rgbR[B]-BSD[COLOR2])<LIM) colorR=COLOR2;
  else if(abs(rgbR[R]-RSD[COLOR3])<LIM && abs(rgbR[G]-GSD[COLOR3])<LIM && abs(rgbR[B]-BSD[COLOR3])<LIM) colorR=COLOR3;
  else if(abs(rgbR[R]-RSD[COLOR4])<LIM && abs(rgbR[G]-GSD[COLOR4])<LIM && abs(rgbR[B]-BSD[COLOR4])<LIM) colorR=COLOR4;
  else if(abs(rgbR[R]-RSD[COLOR5])<LIM && abs(rgbR[G]-GSD[COLOR5])<LIM && abs(rgbR[B]-BSD[COLOR5])<LIM) colorR=COLOR5;
  else if(abs(rgbR[R]-RSD[COLOR6])<LIM && abs(rgbR[G]-GSD[COLOR6])<LIM && abs(rgbR[B]-BSD[COLOR6])<LIM) colorR=COLOR6;
  else if(abs(rgbR[R]-RSD[COLOR7])<LIM && abs(rgbR[G]-GSD[COLOR7])<LIM && abs(rgbR[B]-BSD[COLOR7])<LIM) colorR=COLOR7;
  else colorR=UNDEFINED;
}
/*************************************************************************************************************************************/
void colorAction()
{
    if(colorL!=FLOOR && colorL!=UNDEFINED)
    {
      ledRGB(colorL);
    }
    else if(colorR!=FLOOR && colorL!=UNDEFINED)  
    {
      ledRGB(colorR);
    }
//    one.lcd2(rgbL[R],rgbL[G],rgbL[B]);
//    one.lcd2(rgbR[R],rgbR[G],rgbR[B]);
}

void ledRGB(int color)
{
    switch(color)
    {
        case FLOOR:
          analogWrite(LEDR, 255);
          analogWrite(LEDG, 255);
          analogWrite(LEDB, 255);
          break;
        case COLOR3:
          analogWrite(LEDR, RLED[COLOR3]);
          analogWrite(LEDG, GLED[COLOR3]);
          analogWrite(LEDB, BLED[COLOR3]);
          break;
        case COLOR4:
          analogWrite(LEDR, RLED[COLOR4]);
          analogWrite(LEDG, GLED[COLOR4]);
          analogWrite(LEDB, BLED[COLOR4]);
          break;
        case COLOR6:
          analogWrite(LEDR, RLED[COLOR6]);
          analogWrite(LEDG, GLED[COLOR6]);
          analogWrite(LEDB, BLED[COLOR6]);
          break;
        case COLOR5:
          analogWrite(LEDR, RLED[COLOR5]);
          analogWrite(LEDG, GLED[COLOR5]);
          analogWrite(LEDB, BLED[COLOR5]);
          break;
        case COLOR1:
          analogWrite(LEDR, RLED[COLOR1]);
          analogWrite(LEDG, GLED[COLOR1]);
          analogWrite(LEDB, BLED[COLOR1]);
          break;
        case COLOR2:
          analogWrite(LEDR, RLED[COLOR2]);
          analogWrite(LEDG, GLED[COLOR2]);
          analogWrite(LEDB, BLED[COLOR2]);
          break;
        case COLOR7:
          analogWrite(LEDR, RLED[COLOR7]);
          analogWrite(LEDG, GLED[COLOR7]);
          analogWrite(LEDB, BLED[COLOR7]);
          break;
        default:
//          one.lcd1("UNDEF");
          break;
    }
}

void obstaculos()
{
   if(one.readButton()==3) 
      menuObstaculos();// Entra no menu de configuração / Enter config menu

   brm.readSonars(&sonarL,&sonarC,&sonarR);    
//   Serial.print("Sonars: ");Serial.print(sonarL,DEC); Serial.print(" , "); Serial.print(sonarC,DEC); Serial.print(" , "); Serial.println(sonarR,DEC);
   avoidObstacles();
   delay(60); //Leitura dos sonares a cada 60 ms. / Read sonar´s every 60ms
}

void avoidObstacles(void)
{
  int motorL=0, motorR=0;
  double obst=0.0;

  if(sonarC<=ctrlDist) // Se o sonar do centro detectar objeto / Center sonar is detecting
  {
    if(sonarL<=sonarR) // Se sonar esquerdo menor que o direito / Left sonar is detectins
    {
      if(sonarC<sonarL)
      {
          if(sonarL<ctrlDist)
            sonarL=(sonarC/2);
          else
            sonarL=sonarC;
      }
    }
    else if(sonarL>sonarR)// Se sonar direito menor que o esquerdo / Right sonar is detecting
    {
      if(sonarC<sonarR)
      {
          if(sonarR<ctrlDist)
            sonarR=(sonarC/2);
          else
            sonarR=sonarC;
      }
    }
    else
    {
//        Serial.print("Error  :");
    }
  }

  if(sonarL>ctrlDist)  sonarL=ctrlDist; 
  if(sonarR>ctrlDist)  sonarR=ctrlDist;
  obst=double(sonarR-sonarL)*distWheel;

  if(obst>=0.0)
  {  
     motorR=maxSpeed-(int)obst;
     motorL=maxSpeed;  
//     motorL=maxSpeed+(int)(obst/4);
  }
  else
  {
     motorL=maxSpeed+(int)obst;
     motorR=maxSpeed;  
//     motorR=maxSpeed-(int)(obst/4);  
  }
  
  one.move(motorL,motorR);
}

void menuObstaculos()
{
    int var=0;
    float temp=0.0;
    one.stop();
    one.lcd1("  Menu Config:");
    while(one.readButton()==3) //Espera que se largue o botão 3 / wait until button 3 is released
    {
      delay(50);
    }
    //*************** ctrlDist ***************
    var=ctrlDist;
    while(one.readButton()!=3)
    {
        one.lcd2(" Distancia:",var);
        while(one.readButton()==0);
        if(one.readButton()==1)
        {
          var+=1;
        }
        if(one.readButton()==2)
        {
          var-=1;
        }
        delay(100);
    }
    while(one.readButton()==3) //Espera que se largue o botão 3 / wait until button 3 is released
    {delay(50);}
    ctrlDist=var;        
    //*************** slope ***************
    var=slope;
    while(one.readButton()!=3)
    {
        one.lcd2(" Ganho Dist:",var);
        while(one.readButton()==0);
        if(one.readButton()==1)
        {
          var+=1;
        }
        if(one.readButton()==2)
        {
          var-=1;
        }
        delay(100);
    }
    while(one.readButton()==3) //Espera que se largue o botão 3 / wait until button 3 is released
    {delay(50);}
    slope=var;      
    //*************** maxSpeed ***************
    var=maxSpeed;
    while(one.readButton()!=3)
    {
        one.lcd2(" Velocidade:",var);
        while(one.readButton()==0);
        if(one.readButton()==1)
        {
          var+=1;
        }
        if(one.readButton()==2)
        {
          var-=1;
        }
        delay(100);
    }
    while(one.readButton()==3) //Espera que se largue o botão 3 / wait until button 3 is released
    {delay(50);}
    maxSpeed=var;     
    
    //Termina Configuração
    distWheel=double(100+slope)/double(ctrlDist);
    byte eevarH=highByte(ctrlDist);
    byte eevarL=lowByte(ctrlDist);
    EEPROM.write(0, eevarH);
    EEPROM.write(1, eevarL);
    eevarH=highByte(slope);
    eevarL=lowByte(slope);
    EEPROM.write(2, eevarH);
    EEPROM.write(3, eevarL);
    eevarH=highByte(maxSpeed);
    eevarL=lowByte(maxSpeed);
    EEPROM.write(4, eevarH);
    EEPROM.write(5, eevarL);
//    Serial.print(" DistWheel "); Serial.println(distWheel);
    one.lcd1("Config Terminada");
    one.lcd2("PB3 to Exit Menu");
    while(one.readButton()!=3)
    {
      delay(50);
    }
    while(one.readButton()==3)
    {
      delay(50);
    }
}

void readObstaclesEEPROM(void)
{   
   ctrlDist=(int)EEPROM.read(0);
   ctrlDist=ctrlDist<<8;
   ctrlDist=ctrlDist+(int)EEPROM.read(1);
   slope=(int)EEPROM.read(2);
   slope=slope<<8;
   slope=slope+(int)EEPROM.read(3);
   maxSpeed=(int)EEPROM.read(4);
   maxSpeed=maxSpeed<<8;
   maxSpeed=maxSpeed+(int)EEPROM.read(5);
   if(ctrlDist<0)
   {
     ctrlDist=30;
     slope=10;
   }
   if(maxSpeed<0) maxSpeed=50;
   distWheel=double(100+slope)/double(ctrlDist);
}
/********************************************************************************/

void lineFollow()
{
  int line=0;   // Valor da linha no sensor a cada instante
  int lineRef=0;// Valor de referência da linha para o controlo ( queremos que seja 0)
  int erroP=0;  // Erro proporcional
  static double erroI=0.0;
  static int erroD=0.0;
  static int erroPAnt=0;
  double output=0.0;
  
  int velM1=0, velM2=0;
  
  line=one.readLine(); // O valor da linha varia de -100 a +100
//  Serial.print(" Line:");Serial.print(line); 

  erroP=lineRef-line;
  erroD=erroP-erroPAnt;
  output = kp*(double)erroP + ki*erroI + kd*(double)erroD;
  //Limpar o erro integral se a linha passou para o outro lado ou se está no centro
  if(erroP*erroPAnt<=0)  erroI=0.0; //Limpar o erro integral
  if (output>LIMITES) output=LIMITES;      //Instrução para so calibrar movimento até a linha
  else if (output<-LIMITES) output=-LIMITES;  // mesma coisa do outro lado 
  else erroI+=(double)erroP;  // Se a saída está dentro dos limites incrementa o erro integral
  erroPAnt=erroP;
//  Serial.print("  Out:"); Serial.print(output);

//Limitar mínimos e máximos da velocidade dos motores
  velM1=vel-(int)output;
  velM2=vel+(int)output;
  if(velM1<-1)
      velM1=-1;
  if(velM2<-1)
      velM2=-1;
  if(velM1>vel+vCurve)
      velM1=vel+vCurve;
  if(velM2>vel+vCurve)
      velM2=vel+vCurve;

//  Serial.print("   M1:");  Serial.print(velM1); Serial.print("   M2:"); Serial.println(velM2);  
 if(one.obstacleSensors()!=0) 
    one.stop();
 else 
    one.move(velM1,velM2);
    
  //Menus de configuração
  if(one.readButton()==3) menuLine();// Entra no menu
}

void menuLine()
{
    int var=0;
    int butt=0;
    double temp=0.0;
    one.stop();
    one.lcd1("Menu Config:");
//    one.lcd2("PB1+ PB2- PB3ok");    
    while(one.readButton()!=0) //Espera que se largue o botão 3
    {
      delay(10);
    }
    //*************** Velociade Maxima ***************
    var=vel;
    while(butt!=3)
    {
        one.lcd2("VelMax:",var);
        while(one.readButton()==0);
        butt=one.readButton();
        if(butt==1)
        {
          var++;
        }
        if(butt==2)
        {
          var--;
        }
          delay(100);
    }
    while(butt!=0) //Espera que se largue o botão 3
    {butt=one.readButton();}
    vel=var;        
    //*************** Incremento da roda que acelera ***************
    var=vCurve;
    while(butt!=3)
    {
        one.lcd2("Curve Boost:",var);
        while(one.readButton()==0);
        butt=one.readButton();
        if(butt==1)
        {
          var++;
        }
        if(butt==2)
        {
          var--;
        }
        delay(100);
    }
    while(butt!=0) //Espera que se largue o botão 3
    {butt=one.readButton();}
    vCurve=var;  
     //*************** Ganho KP ***************
    temp=kp*1000;
    var=(int)temp;
    while(butt!=3)
    {
        one.lcd2(" kp:",var);
        while(one.readButton()==0);
        butt=one.readButton();
        if(butt==1)
        {
          var+=10;
        }
        if(butt==2)
        {
          var-=10;
        }
        delay(100);
    }
    while(butt!=0) //Espera que se largue o botão 3
    {butt=one.readButton();}
    kp=(double)var/1000;  
     //*************** Ganho KI ***************
    temp=ki*10000.0;
    var=(int)temp;
    while(butt!=3)
    {
        one.lcd2(" ki:",var);
        while(one.readButton()==0);
        butt=one.readButton();
        if(butt==1)
        {
          var+=1;
        }
        if(butt==2)
        {
          var-=1;
        }
        delay(100);
    }
    while(butt!=0) //Espera que se largue o botão 3
    {butt=one.readButton();}
    ki=(double)var/10000.0;  
     //*************** Ganho KP ***************
    temp=kd*1000;
    var=(int)temp;
    while(butt!=3)
    {
        one.lcd2(" kd:",var);
        while(one.readButton()==0);
        butt=one.readButton();
        if(butt==1)
        {
          var+=10;
        }
        if(butt==2)
        {
          var-=10;
        }
        delay(100);
    }
    while(butt!=0) //Espera que se largue o botão 3
    {butt=one.readButton();}
    kd=(double)var/1000;
    //*************** Termina Configuração *************** 
    writeLineEEPROM();
    one.lcd1("Line  Following!");
    one.lcd2("www.botnroll.com");
    delay(250);
}

void writeLineEEPROM()
{
    byte eepromADD=10;
    int var=0;

    var=vel;
    EEPROM.write(eepromADD,lowByte(var)); //Guardar em EEPROM
    eepromADD++;
    var=vCurve;
    EEPROM.write(eepromADD,lowByte(var)); //Guardar em EEPROM
    eepromADD++;
    var=(int)(kp*1000.0);
    EEPROM.write(eepromADD,highByte(var)); //Guardar em EEPROM
    eepromADD++;
    EEPROM.write(eepromADD,lowByte(var));
    eepromADD++;
    var=(int)(ki*10000.0);
    EEPROM.write(eepromADD,highByte(var)); //Guardar em EEPROM
    eepromADD++;
    EEPROM.write(eepromADD,lowByte(var));
    eepromADD++;
    var=(int)(kd*1000.0);
    EEPROM.write(eepromADD,highByte(var)); //Guardar em EEPROM
    eepromADD++;
    EEPROM.write(eepromADD,lowByte(var));
    eepromADD++;
}

void readLineEEPROM()
{
    byte eepromADD=10;
    int  var=0;
      
    vel=(int)EEPROM.read(eepromADD);
    eepromADD++;
    vCurve=(int)EEPROM.read(eepromADD);
    eepromADD++;
    var=0;
    var=(int)EEPROM.read(eepromADD);
    eepromADD++;
    var=var<<8;
    var+=(int)EEPROM.read(eepromADD);
    eepromADD++;
    kp=(double)var/1000.0;
    var=0;
    var=(int)EEPROM.read(eepromADD);
    eepromADD++;
    var=var<<8;
    var+=(int)EEPROM.read(eepromADD);
    eepromADD++;
    ki=(double)var/10000.0;
    var=0;
    var=(int)EEPROM.read(eepromADD);
    eepromADD++;
    var=var<<8;
    var+=(int)EEPROM.read(eepromADD);
    eepromADD++;
    kd=(double)var/1000.0;
    if(vel==255) vel=60;
    if(vCurve==255) vCurve=3;
    if(kp<0) kp=1.3;
    if(ki<0) ki=0.0013;
    if(kd<0) kd=0.35;
}

/*****************************************************************************/
void calibrateLine()
{
    static int SVal[8]={0,0,0,0,0,0,0,0};    
    static int SValMax[8]={0,0,0,0,0,0,0,0};
    static int SValMin[8]={1023,1023,1023,1023,1023,1023,1023,1023};
    int Vtrans=50;  //Line follower limit between white and black 
    int butt=0;
//    Serial.println("Calibrating Starting!");
    one.lcd1("   Calibrate    ");
    one.lcd2("   starting!    ");
    while(one.readButton()!=0)
    {
      delay(50);
    }
//    Serial.println("Press any button to end calibrate!");
//    one.lcd1("Press any button");
//    one.lcd2("to end calibrate");
 
//Calibração com o robô parado ou em movimento    
    one.move(25,-25);            //Em movimento calibra durante 4 segundos
    unsigned long time=millis();
    while(millis()<time+4000)
//  while(one.readButton()==0)     //Com o robô parado calibra até se carregar num botão
    {
 //     Serial.println(); Serial.print("Val: ");
      for(int i=0;i<8;i++)
      {
          int SVal=one.readAdc(i);
          if(SVal>SValMax[i])
          {
            SValMax[i]=SVal;
          }
          if(SVal<SValMin[i])
          {
            SValMin[i]=SVal;
          }          
//          Serial.print(SVal);Serial.print("  ");          
      }      
      Vtrans=0;
      for(int i=0;i<8;i++)
      {
//         Serial.print(SValMin[i]);Serial.print("  ");
         if(SValMin[i]>Vtrans)
           Vtrans=SValMin[i];
      }    
//     Serial.println();   
      delay(50);
   }
//   Serial.print("Vtrans:");Serial.println(Vtrans);
   one.stop();

   //Escrever valores na EEPROM
   byte eepromADD=100;
   for(int i=0;i<8;i++)
   {
       EEPROM.write(eepromADD,highByte(SValMax[i]));
       eepromADD++;
       EEPROM.write(eepromADD,lowByte(SValMax[i]));
       eepromADD++;
   }
   for(int i=0;i<8;i++)
   {
       EEPROM.write(eepromADD,highByte(SValMin[i]));
       eepromADD++;
       EEPROM.write(eepromADD,lowByte(SValMin[i]));
       eepromADD++;
   }
   EEPROM.write(eepromADD,highByte(Vtrans));
   eepromADD++;
   EEPROM.write(eepromADD,lowByte(Vtrans));
   eepromADD--;

   one.lcd1(" Calibrate done ");
   while(one.readButton()==0)
   {
      one.lcd2("Line Value: ",one.readLine());
      delay(25);
   }   
}

