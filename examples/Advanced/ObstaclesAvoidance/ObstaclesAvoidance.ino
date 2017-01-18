/* 
 This example was created by José Cruz (www.botnroll.com)
 on the 29th January 2014
 
 This code example is in the public domain. 
 http://www.botnroll.com

Description:

*/
#include <BnrOneA.h>   // Bot'n Roll ONE A library
#include <SPI.h>      // SPI communication library required by BnrOne.cpp
BnrOneA one;           // declaration of object variable to control the Bot'n Roll ONE A

//constants definition
#define SSPIN  2      // Slave Select (SS) pin for SPI communication

void setup() 
{ 
    //setup routines -> runs only once when program starts
    Serial.begin(57600);     // set baud rate to 57600bps for printing values at serial monitor. Press (ctrl+shift+m) after uploading    
    one.spiConnect(SSPIN);   // start the SPI communication module
    one.stop();              // stop motors
    one.obstacleEmitters(ON);// activate obstacles detection
}

void loop()
{
    byte obstacles=one.obstacleSensors(); //Read obstacle sensors
    switch(obstacles)
    {
        case 0:   // no obstacles detected
            one.lcd2("  No Obstacles");
            one.move(60,60);     // Move Forward
            break;
        case 1:   // obstacle detected on Left sensor
            one.lcd2("   Left Sensor");
            one.move(60,-60);    // Rotate Right
            break;
        case 2:   // obstacle detected on Right sensor
            one.lcd2("  Right Sensor");
            one.move(-60,60);    // Rotate Left
            break;
        case 3:   // obstacle detected on both sensors
            one.lcd2("  Both Sensors");
            one.move(-60,-60);   // Move Backwards
            delay(1000);
            one.move(-60,60);    // Rotate Left      
            delay(500);     
            break;
    }
    delay(50);
}

