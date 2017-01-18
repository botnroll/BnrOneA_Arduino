/* 
 This example was created by José Cruz (www.botnroll.com)
 on 28 November 2013
 
 This code example is in the public domain. 
 http://www.botnroll.com

This example shows how to use the Pan&Tilt (using servos)

NOTE:
Servo1 values vary between  10 - 170 (right - left) -> PAN
Servo2 values vary between  10 - 160 (upwards - head down) -> TILT
Avoid using the servos on the limit values.
*/
 
#include <BnrOneA.h>   // Bot'n Roll ONE A library
#include <SPI.h>       // SPI communication library required by BnrOne.cpp
BnrOneA one;           // declaration of object variable to control the Bot'n Roll ONE A

//constants definition
#define SSPIN  2       // Slave Select (SS) pin for SPI communication

void setup()
{
    Serial.begin(57600);     // set baud rate to 57600bps for printing values at serial monitor.
    one.spiConnect(SSPIN);   // start the SPI communication module
    one.stop();              // stop motors
    one.lcd1(" Bot'n Roll ONE");
    one.lcd2("www.botnroll.com");
    one.servo1(90); //Central position 0º - 180º
    one.servo2(90); //Central position 0º - 180º
    delay(1000);
}

void loop()
{
static int Pos_Servo1=90;
static int Pos_Servo2=90;
static int Servo=1;
int button;

    button=one.readButton();
    switch (button)
    {
        case 1: 
                if (Servo==1)
                  Pos_Servo1+=10;
                else
                  Pos_Servo2+=10;
                break;
        case 2:
                if (Servo==1)
                  Pos_Servo1-=10;
                else
                  Pos_Servo2-=10;
                break;
  
        case 3: Servo++; if (Servo>2) Servo=1; break;
    }

    one.lcd1("Position 1: ", Pos_Servo1);
    one.lcd2("Position 2: ", Pos_Servo2);
    switch (Servo)
    {
        case 1: one.servo1(Pos_Servo1); break;
        case 2: one.servo2(Pos_Servo2); break;
    }
    delay(100);
}
