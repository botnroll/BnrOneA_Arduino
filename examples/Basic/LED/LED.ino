/* 
 This example was created by José Cruz (www.botnroll.com)
 on the 29th January 2014
 
 This code example is in the public domain. 
 http://www.botnroll.com

Description:
The LED is turned ON and OFF every second. 
A messsage is sent to the Serial Monitor accordingly.
*/

#include <BnrOneA.h>   // Bot'n Roll ONE A library
#include <SPI.h>       // SPI communication library required by BnrOne.cpp
BnrOneA one;           // declaration of object variable to control the Bot'n Roll ONE A

//constants definition
#define SSPIN  2       // Slave Select (SS) pin for SPI communication

void setup() 
{
    Serial.begin(57600);     // set baud rate to 57600bps for printing values at serial monitor.
    one.spiConnect(SSPIN);   // start the communication module
    one.stop();              // stop motors
}

void loop()
{    
    one.led(HIGH);             // turn LED ON
    Serial.println("LED ON");  // print data on serial monitor.
    delay(1000);               // waits 1 second
    one.led(LOW);              // turn LED OFF
    Serial.println("LED OFF"); // print data on serial monitor.
    delay(1000);               // wait 1 second
}

