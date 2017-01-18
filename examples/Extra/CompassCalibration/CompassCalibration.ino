/* 
 This example was created by José Cruz (www.botnroll.com)
 on 09 January 2015
 
 How the program works:
 This examples calibrates the CMPS11 compass attached to Bot'n Roll ONE A.
 Follow the instructions show on the LCD and press the push buttons accordingly.
 The robot will rotate for a few seconds and at the end the compass calibration is finished.
 
 This code example is in the public domain. 
 http://www.botnroll.com
*/


#include <BnrOneA.h>   // Bot'n Roll ONE A library
#include <SPI.h>       // SPI communication library required by BnrOne.cpp
#include <Wire.h>

BnrOneA one;           // declaration of object variable to control the Bot'n Roll ONE A

//constants definition
#define SSPIN  2       // Slave Select (SS) pin for SPI communication
#define ADDRESS 0x60   // Define address of CMPS10

void setup()
{
  Wire.begin();            // Connect I2C bus
  Serial.begin(57600);
  one.spiConnect(SSPIN);   // start SPI communication module
  one.stop();              // stop motors
}

float read_bearing()
{
byte highByte, lowByte;    // highByte and lowByte store the bearing and fine stores decimal place of bearing

   Wire.beginTransmission(ADDRESS);           //start communication with CMPS10
   Wire.write(2);                             //Send the register we wish to start reading from
   Wire.endTransmission();

   Wire.requestFrom(ADDRESS, 2);              // Request 4 bytes from CMPS10
   while(Wire.available() < 2);               // Wait for bytes to become available
   highByte = Wire.read();
   lowByte = Wire.read();
   
return (float)((highByte<<8)+lowByte)/10;
}

char read_roll()
{
char roll;                 // Store  roll value of CMPS10, chars are used because they support signed value

   Wire.beginTransmission(ADDRESS);           //start communication with CMPS10
   Wire.write(5);                             //Send the register we wish to start reading from
   Wire.endTransmission();

   Wire.requestFrom(ADDRESS, 1);              // Request 4 bytes from CMPS10
   while(Wire.available() < 1);               // Wait for bytes to become available
   roll =Wire.read();
return roll;
}

char read_pitch()
{
   char pitch;                // Store pitch value of CMPS10, chars are used because they support signed value

   Wire.beginTransmission(ADDRESS);           //start communication with CMPS10
   Wire.write(4);                             //Send the register we wish to start reading from
   Wire.endTransmission();

   Wire.requestFrom(ADDRESS, 1);              // Request 4 bytes from CMPS10
   while(Wire.available() < 1);               // Wait for bytes to become available
   pitch = Wire.read();

   return pitch;
}

void calibrateCMPS11()
{
   one.move(-30,30); // Slowly rotate the compass on the horizontal plane in all directions

   Wire.beginTransmission(ADDRESS);           //start communication with CMPS10
   Wire.write(0);                             //Send the register we wish to start reading from
   Wire.write(0xF0);                          //Calibration sequence byte 1
   Wire.endTransmission();
   delay(20);

   Wire.beginTransmission(ADDRESS);           //start communication with CMPS10
   Wire.write(0);                             //Send the register we wish to start reading from
   Wire.write(0xF5);                          //Calibration sequence byte 2
   Wire.endTransmission();
   delay(20);

   Wire.beginTransmission(ADDRESS);           //start communication with CMPS10
   Wire.write(0);                             //Send the register we wish to start reading from
   Wire.write(0xF7);                          //Calibration sequence byte 2
   Wire.endTransmission();
   delay(20);
   
   one.move(-20,20); // Slowly rotate the compass on the horizontal plane in all directions
   delay(15000);
   
   Wire.beginTransmission(ADDRESS);           //start communication with CMPS10
   Wire.write(0);                             //Send the register we wish to start reading from
   Wire.write(0xF8);                          //Exit calibration mode
   Wire.endTransmission();
   delay(20);
   one.move(0,0); // Stop rotation

}



void loop()
{
float bearing;
char roll, pitch;
char temp[20];


   one.lcd1("   Press PB1   ");
   one.lcd2(" to calibrate  ");
   while(one.readButton()!=1)
       delay(50);

   one.lcd1("  Calibration");
   one.lcd2("    Starting");
   delay(1000);
   calibrateCMPS11();

   one.lcd1("  Calibration");
   one.lcd2("    Ended");
   delay(1000);

   while(one.readButton()!=1)
       delay(100);
   
}

