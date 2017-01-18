/* 
 HC-SR04 Ping distance sensor:
 VCC to arduino 5v 
 GND to arduino GND
 Echo to Arduino pin 7 
 Trig to Arduino pin 8
 
 This sketch originates from Virtualmix: http://goo.gl/kJ8Gl

 This example was created by José Cruz (www.botnroll.com)
 on 28 November 2013
 
 This code example is in the public domain. 
 http://www.botnroll.com
*/
 
#include <SPI.h>                   // required by BnrOne.cpp
#include <BnrOneA.h>

//constants definitions
#define SSPIN  2    //Slave Select (SS) pin for SPI communication

#define echoPin 7 // Echo Pin
#define trigPin 8 // Trigger Pin
#define LEDPin 13 // Onboard LED

BnrOneA one;         //declaration of object variable to control the Bot'n Roll ONE

#define maximumRange 200   // Maximum range needed (200cm)
#define minimumRange   0   // Minimum range needed

int Sonar()
{
    unsigned long duration;  // Duration used to calculate distance
    int distance;
    unsigned long tempo=micros();
  
    digitalWrite(trigPin, LOW); 
    delayMicroseconds(2); 
  
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
  
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH, 11640);
    delayMicroseconds(16000 - (micros()-tempo));  // esta rotina demora sempre o mesmo tempo (maximo 16 milisegundos)
  
    //Calculate the distance (in cm) based on the speed of sound
    distance = (int)(duration/58.2);
    if (distance >= maximumRange || distance <= minimumRange)
      distance=-1;
  
    return distance;
}

void setup()
{
    //setup routines
    Serial.begin(57600);    // set baud rate to 115200bps for printing values in serial monitor.
    one.spiConnect(SSPIN);  // start SPI module
    one.stop();              // stop motors

    one.lcd1("www.botnroll.com");
    one.lcd2(" ");
 
    // SONAR setup
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(LEDPin,  OUTPUT); // Use LED indicator (if required)
}

void loop()
{
    long distance;
  
    distance=Sonar();
    one.lcd2("distance: ", distance);
    Serial.println(distance);
    delay(50);
}

