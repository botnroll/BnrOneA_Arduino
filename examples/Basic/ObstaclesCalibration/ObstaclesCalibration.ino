/* 
 This example was created by José Cruz (www.botnroll.com)
 on the 29th January 2014
 
 This code example is in the public domain. 
 http://www.botnroll.com

Description:
Calibrates IR for Obstacles detection
*/

#include <BnrOneA.h>   // Bot'n Roll ONE A library
#include <SPI.h>       // SPI communication library required by BnrOne.cpp
BnrOneA one;           // declaration of object variable to control the Bot'n Roll ONE A

//constants definition
#define SSPIN  2       // Slave Select (SS) pin for SPI communication

void setup() 
{
    Serial.begin(57600);     // set baud rate to 57600bps for printing values at serial monitor.
    one.spiConnect(SSPIN);   // start SPI communication module
    one.stop();              // stop motors
    one.obstacleEmitters(ON);// activate IR emitters
}

void loop()
{
    byte obstacle=one.obstacleSensors(); // read obstacle sensors
    switch(obstacle) // test variable
    {
        case 0:      // no obstacle detected
            one.lcd2(" No Obstacles: ",obstacle);
            Serial.print("No Obstacles: ");Serial.println(obstacle);
            break;
        case 1:      // obstacle detected on Left sensor
            one.lcd2(" Left  Sensor: ",obstacle);
            Serial.print("Left  Sensor: ");Serial.println(obstacle);
            break;
        case 2:      // obstacle detected on Right sensor
            one.lcd2(" Right Sensor: ",obstacle);
            Serial.print("Right Sensor: ");Serial.println(obstacle);
            break;
        case 3:      // obstacle detected on both sensors
            one.lcd2(" Both Sensors: ",obstacle);
            Serial.print("Both Sensors: ");Serial.println(obstacle);
            break;
        default:     // in case of reading error
             one.lcd2(" Error: ",obstacle);
            Serial.print("Error: ");Serial.println(obstacle);
            break;
    }
    delay(5);
}

