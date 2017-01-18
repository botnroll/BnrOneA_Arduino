/* 
 This example was created by José Cruz (www.botnroll.com)
 on the 29th January 2014
 
 This code example is in the public domain. 
 http://www.botnroll.com

Description:
Infra-Red obstacle detection LEDs (IRE1 and IRE2) are turned ON and OFF every second. 
Messages are printed on the LCD and Serial Monitor accordingly.
WARNING!!!
Infra-Red light can damage your eyes if you look to emmiting IR LED's.
You will not be able to see the LED's emmiting light because human eyes cannot see infra-red light.
You can see the IR LED's light using a camera (from you cell phone or smartphone, for example).
Placing an obstacle 2cm in front of the LEDS should turn IRS1 and IRS2 ON and OFF.
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
}

void loop()
{
    one.obstacleEmitters(ON);          // activate IR emitter LEDs
    Serial.println("IR Emitters ON "); // print data on serial monitor.
    one.lcd2(" IR Emitters ON ");      // print text on LCD line 2
    delay(1000);                       // wait 1 second
    one.obstacleEmitters(OFF);         // deactivate IR emitter LEDs
    Serial.println("IR Emitters OFF"); // print data on serial monitor.
    one.lcd2(" IR Emitters OFF ");     // print text on LCD line 2
    delay(1000);                       // wait 1 second
}
