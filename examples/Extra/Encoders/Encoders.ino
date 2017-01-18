/* 
 This example was created by José Cruz (www.botnroll.com)
 on 28 November 2013
 
 This code example is in the public domain. 
 http://www.botnroll.com

This program shows an example of how to use encoders onthe wheels.
*/
 
#include <BnrOneA.h>   // Bot'n Roll ONE A library
#include <SPI.h>       // SPI communication library required by BnrOne.cpp
BnrOneA one;           // declaration of object variable to control the Bot'n Roll ONE A

//constants definitions
#define SSPIN  2    //Slave Select (SS) pin for SPI communication
#define M1  1       //Motor1
#define M2  2       //Motor2

//Variables to read from Bot'n Roll ONE
byte firm_int=0;      // the firmware version of your Bot'n Roll ONE board (visit http://www.botnroll.com/one/ to check if your version is updated)
byte firm_dec=0;      // the firmware version of your Bot'n Roll ONE board (visit http://www.botnroll.com/one/ to check if your version is updated)
byte firm_dev=0;      // the firmware version of your Bot'n Roll ONE board (visit http://www.botnroll.com/one/ to check if your version is updated)
float battery=0;      // battery reading
int speed1=65;
int speed2=65;
int encL=0;
int encR=0;

void setup()
{
    //setup routines
    Serial.begin(57600);     // set baud rate to 57600bps for printing values at serial monitor.
    one.spiConnect(SSPIN);   // start the SPI communication module
    one.stop();              // stop motors
    one.lcd1(" Bot'n Roll ONE");
    one.lcd2("www.botnroll.com");
    encL=one.readEncL();
    encR=one.readEncR();
    delay(1000);
}


void loop()
{
    encL=one.readEncLInc();
    encR=one.readEncRInc();
    int but=one.readButton();
    if(but==1)
    {
        speed1++;
        speed2++;
    }
    if(but==3)
    {
        speed1=0;
        speed2=0;
        encL=one.readEncL();
        encR=one.readEncR();
    }
    if(but==2)
    {
        speed1--;
        speed2--;
    }
    one.lcd1(speed1,encL);
    one.lcd2(speed2,encR);
    if(encL>=495)
    {
        speed1=0;
        speed2=0;
    }
    one.move(speed1,speed2);
    delay(50);
}

