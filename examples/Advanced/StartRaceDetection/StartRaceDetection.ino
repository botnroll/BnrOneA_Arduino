/* 
 This example was created by José Cruz (www.botnroll.com)
 on 28 November 2013
 
 Modified by José Fernandes on 21 July 2014
 Revised by José Cruz on 23 March 2016 
 
 This code example is in the public domain. 
 http://www.botnroll.com

This program detects automatic start on the race.
*/

#include <BnrOneA.h>   // Bot'n Roll ONE A library
#include <EEPROM.h>    // EEPROM reading and writing
#include <SPI.h>       // SPI communication library required by BnrOne.cpp
BnrOneA one;           // declaration of object variable to control the Bot'n Roll ONE A

//constants definition
#define SSPIN  2       // Slave Select (SS) pin for SPI communication

void setup()
{
	one.spiConnect(SSPIN);  // start SPI communication module
	one.stop();             // stop motors
	one.lcd1("IR testing"); // print on LCD line 1
	one.lcd2("STOP");       // print on LCD line 2
	one.obstacleEmitters(OFF); //Deactivate obstacles IR emitters
	delay(4000);           //Time to stabilize IR sensors (DO NOT REMOVE!!!)
        static byte start=0;
 	while(!start)
        {
          start=automatic_start();
        }
 	one.move(50,50); //The robot starts by moving forwards
 	one.lcd2("GO"); //Remove when racing for best performance!
}


bool automatic_start()
{ 
	bool actstate=one.readIRSensors(); //read actual IR sensors state
	
	if(!actstate) //If state is LOW
	{
		unsigned long int tempo_A=millis(); //read time
		while(!actstate) //while state is LOW
		{
			actstate=one.readIRSensors(); //read actual IR sensors state
			if((millis()-tempo_A)>50) //if time is low for more than 50ms
			{
				return true; //Start Race
			}
		}
	}
	return false;
}


void loop()
{

}