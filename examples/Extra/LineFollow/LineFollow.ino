/* 
 This example was created by Hélder Ribeiro (www.botnroll.com)
 on 04 June 2014
 Modified by José Cruz on 20 March 2015 
 
 
 This code example is in the public domain. 
 http://www.botnroll.com

This program follows the line
*/
 
#include <BnrOneA.h>   // Bot'n Roll ONE A library
#include <SPI.h>       // SPI communication library required by BnrOne.cpp
BnrOneA one;           // declaration of object variable to control the Bot'n Roll ONE A

//constants definitions
#define SSPIN  2    //Slave Select (SS) pin for SPI communication
#define M1  1       //Motor1
#define M2  2       //Motor2

#define smin 300 //Line follower limit between white and black 

float batmin=9.5;  //13.5 Lithium

int vel=40;

void setup() 
{  
      Serial.begin(57600);     // sets baud rate to 57600bps for printing values at serial monitor.
      one.spiConnect(SSPIN);   // starts the SPI communication module
      one.stop();              // stops all move
      one.minBat(batmin);
  
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
      int sensor0 = one.readAdc(0);
      int sensor1 = one.readAdc(1);
      int sensor2 = one.readAdc(2);
      int sensor3 = one.readAdc(3);
      int sensor4 = one.readAdc(4);
      int sensor5 = one.readAdc(5);
      int sensor6 = one.readAdc(6);
      int sensor7 = one.readAdc(7);
      
      if(sensor0 > smin && sensor1 > smin && sensor2 < smin && sensor3 < smin && sensor4 < smin && sensor5 < smin && sensor6 < smin && sensor7 < smin)//11000000
      {
        one.move((vel*(-1/4)),vel);
      }
      else if(sensor0 < smin && sensor1 > smin && sensor2 > smin && sensor3 < smin && sensor4 < smin && sensor5 < smin && sensor6 < smin && sensor7 < smin)//01100000
      {
        one.move(0,vel);
      }
      else if(sensor0 < smin && sensor1 < smin && sensor2 > smin && sensor3 > smin && sensor4 < smin && sensor5 < smin && sensor6 < smin && sensor7 < smin)//00110000
      {
        one.move((vel/2),vel);
      }
      else if(sensor0 < smin && sensor1 < smin && sensor2 < smin && sensor3 > smin && sensor4 > smin && sensor5 < smin && sensor6 < smin && sensor7 < smin)//00011000
      {
        one.move(vel,vel);
      }
      else if(sensor0 < smin && sensor1 < smin && sensor2 < smin && sensor3 < smin && sensor4 > smin && sensor5 > smin && sensor6 < smin && sensor7 < smin)//00001100
      {
        one.move(vel,(vel/2));
      }
       else if(sensor0 < smin && sensor1 < smin && sensor2 < smin && sensor3 < smin && sensor4 < smin && sensor5 > smin && sensor6 > smin && sensor7 < smin)//00000110
      {
        one.move(vel,0);
      }
      else if(sensor0 < smin && sensor1 < smin && sensor2 < smin && sensor3 < smin && sensor4 < smin && sensor5 < smin && sensor6 > smin && sensor7 > smin)//00000011
      {
        one.move(vel,(vel*(-1/4)));
      }  
}