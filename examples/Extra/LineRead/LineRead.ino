/* 
 This example was created by José Cruz (www.botnroll.com)
 on 28 November 2013
 
 This code example is in the public domain. 
 http://www.botnroll.com
*/
 
#include <SPI.h>                   // required by BnrOne.cpp
#include <BnrOneA.h>

//constants definitions
#define SSPIN  2    //Slave Select (SS) pin for SPI communication
#define M1  1       //Motor1
#define M2  2       //Motor2
BnrOneA one;        //declaration of object variable to control the Bot'n Roll ONE

//Variables to read from Bot'n Roll ONE
byte firm_int=0;      // the firmware version of your Bot'n Roll ONE board (visit http://www.botnroll.com/one/ to check if your version is updated)
byte firm_dec=0;      // the firmware version of your Bot'n Roll ONE board (visit http://www.botnroll.com/one/ to check if your version is updated)
byte firm_dev=0;      // the firmware version of your Bot'n Roll ONE board (visit http://www.botnroll.com/one/ to check if your version is updated)
float battery=0;      // battery reading

void setup()
{
    //setup routines
    Serial.begin(57600);    // set baud rate to 57600bps for printing values in serial monitor. Press (ctrl+shift+m) after uploading6
    one.spiConnect(SSPIN);  // start the SPI module
    one.stop();              // stop motors
    one.lcd1(" Bot'n Roll ONE");
    one.lcd2("www.botnroll.com");
    delay(1000);
}

#define LS1  0
#define LS2  1
#define LS3  2
#define LS4  3
#define LS5  4
#define LS6  5
#define LS7  6
#define LS8  7

void loop()
{
  int line[8];
  for(int i=0;i<8;i++)
    {
        line[i]=one.readAdc(i);
        Serial.print("  ");Serial.print(line[i]);
    }
    Serial.println();
    one.lcd1(line[LS1],line[LS2],line[LS3],line[LS4]);
    one.lcd2(line[LS5],line[LS6],line[LS7],line[LS8]);
    delay(100);
}

