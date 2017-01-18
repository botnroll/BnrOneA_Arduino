/* 
 This example was created by José Cruz (www.botnroll.com)
 on 28 November 2013
 
 This code example is in the public domain. 
 http://www.botnroll.com
 
This program shows how to use the Gripper.

NOTE:
Gripper1 values vary between  80 - 160 (upwards - downwards) - (130 corresopnds to Horizontal)
Gripper2 values vary between  18 - 120 (closed - opened)

*/
 
#include <BnrOneA.h>   // Bot'n Roll ONE A library
#include <SPI.h>       // SPI communication library required by BnrOne.cpp
#include <Servo.h>
BnrOneA one;           // declaration of object variable to control the Bot'n Roll ONE A

//constants definition
#define SSPIN  2       // Slave Select (SS) pin for SPI communication

Servo gripper1;
Servo gripper2;

void setup()
{
    Serial.begin(57600);     // set baud rate to 57600bps for printing values at serial monitor.
    one.spiConnect(SSPIN);   // start SPI communication module
    one.stop();              // stop motors
    one.lcd1(" Bot'n Roll ONE");
    one.lcd2("www.botnroll.com");
    delay(1000);

    gripper1.attach(3);
    gripper2.attach(5);
}

void gripper_open()
{
  gripper2.write(120);
}

void gripper_close()
{
  gripper2.write(0);
}

void loop()
{
static int Pos_Servo1=140;
static int Pos_Servo2=120;
static int Servo=1;
int button;

    button=one.readButton();
    switch (button)
    {
      case 1: 
              if (Servo==1)
                Pos_Servo1+=5;
              else
                Pos_Servo2+=5;
              break;
      case 2:
              if (Servo==1)
                Pos_Servo1-=5;
              else
                Pos_Servo2-=5;
              break;

      case 3: Servo++; if (Servo>2) Servo=1; break;
    }

    one.lcd1("Gripper 1: ", Pos_Servo1);
    one.lcd2("Gripper 2: ", Pos_Servo2);
    switch (Servo)
    {
      case 1: gripper1.write(Pos_Servo1); break;
      case 2: gripper2.write(Pos_Servo2); break;
    }
    delay(200);
}
