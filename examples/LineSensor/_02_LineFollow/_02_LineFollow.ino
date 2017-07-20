/* 
 This example was created by José Cruz on October 2016 
 
 This code example is in the public domain. 
 http://www.botnroll.com

Line Following:
Test wich sensor detects the line by comparing sensor values.
The motors speed is set for each sensor detecting the line.
<>
Seguimento de linha:
Testa qual o sensor que deteta a linha comparando os valores dos sensores.
A velocidade dos motores é definida para cada uma das 8 posições da linha no sensor.
*/
#include <BnrOneA.h>   // Bot'n Roll ONE A library
#include <EEPROM.h>    // EEPROM reading and writing
#include <SPI.h>       // SPI communication library required by BnrOne.cpp
BnrOneA one;           // declaration of object variable to control the Bot'n Roll ONE A

//Constants definitions
//Definir constantes
#define SSPIN  2    //Slave Select (SS) pin for SPI communication
#define M1  1       //Motor1
#define M2  2       //Motor2

//Transition value between white and black
//Valor de transição entre branco e preto
#define Vtrans 300

//Battery protection (lower voltage)
//Protecção da bateria (tensão mínima)
float batmin=10.5;
int vel=40;
//Speed for the robot movement
//Velocidade do robô

void setup() 
{  
      Serial.begin(57600);     // sets baud rate to 57600bps for printing values at serial monitor.
      one.spiConnect(SSPIN);   // starts the SPI communication module
      one.stop();              // stop motors
      one.minBat(batmin);
      one.lcd1(" Bot'n Roll ONE");
      //Wait for a button to be pressed to move motors
      //Espera pressionar um botão para mover motores
      while(one.readButton()==0)
      {
        one.lcd2("Press a button!");
        delay(50);
      }
      one.move(vel,vel);
      one.lcd2("Line Following!");	
}

void loop() 
{
      //Read values from the 8 sensors
      //Ler o valor dos 8 sensores
      int sensor0 = one.readAdc(0);
      int sensor1 = one.readAdc(1);
      int sensor2 = one.readAdc(2);
      int sensor3 = one.readAdc(3);
      int sensor4 = one.readAdc(4);
      int sensor5 = one.readAdc(5);
      int sensor6 = one.readAdc(6);
      int sensor7 = one.readAdc(7);

      //Lado esquerdo do exterior para o centro      
      if(sensor0 > Vtrans )//10000000
      {
        one.move(-1,40);
      }
      else if(sensor1 > Vtrans)//01000000
      {
        one.move(5,40);
      }
      else if(sensor2 > Vtrans)//00100000
      {
        one.move(20,40);
      }
      else if(sensor3 > Vtrans)//00010000
      {
        one.move(vel,vel);
      }      
      //Lado direito do exterior para o centro
      else if(sensor7 > Vtrans)//00000001
      {
        one.move(40,-1);
      }             
      else if(sensor6 > Vtrans)//00000010
      {
        one.move(40,5);
      }        
      else if(sensor5 > Vtrans)//00000100
      {
        one.move(40,20);
      }
      else if(sensor4 > Vtrans)//00001000
      {
        one.move(vel,vel);
      }   
}
