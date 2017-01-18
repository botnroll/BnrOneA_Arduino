/* 
 This example was created by José Cruz (www.botnroll.com)
 on 28 November 2013
 
 This code example is in the public domain. 
 http://www.botnroll.com
*/

/****************************************************************
*                  Arduino CMPS11 example code                  *
*                    CMPS11 running I2C mode                    *
*                    by James Henderson, 2012                   *
*****************************************************************/
#include <BnrOneA.h>   // Bot'n Roll ONE A library
#include <SPI.h>       // SPI communication library required by BnrOne.cpp
#include <Wire.h>

BnrOneA one;           // declaration of object variable to control the Bot'n Roll ONE A

//constants definition
#define SSPIN  2       // Slave Select (SS) pin for SPI communication
#define ADDRESS 0x60                                          // Defines address of CMPS10

void setup()
{
  Wire.begin();                                               // Conects I2C
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
char roll;                 // Store  roll values of CMPS10, chars are used because they support signed value

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
char pitch;                // Store pitch values of CMPS10, chars are used because they support signed value

   Wire.beginTransmission(ADDRESS);           //start communication with CMPS10
   Wire.write(4);                             //Send the register we wish to start reading from
   Wire.endTransmission();

   Wire.requestFrom(ADDRESS, 1);              // Request 4 bytes from CMPS10
   while(Wire.available() < 1);               // Wait for bytes to become available
   pitch = Wire.read();

return pitch;
}

void loop()
{
float bearing;
char roll, pitch;
char temp[20];

   bearing=read_bearing();
   roll=read_roll();
   pitch=read_pitch();

   Serial.print("Bearing:"); Serial.print(bearing);
   Serial.print("   roll:"); Serial.print((int)roll);
   Serial.print("   pitch:"); Serial.println((int)pitch);

   one.lcd1("Bearing: ", bearing);
   sprintf(temp, "Rol:%d Pit:%d      ", (int)roll, (int)pitch);
   one.lcd2(temp);
    
   delay(50);
}

