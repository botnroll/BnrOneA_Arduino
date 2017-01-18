/*
TPA81 is a Pixel Thermal Array Sensor. 
Detects a candle flame at a range 2 meters (6ft) and is unaffected by ambient light!
Product can be found here:http://www.botnroll.com/index.php?id_product=131&controller=product&id_lang=7

 This sketch originates from:
********************************************
*    arduino example for TPA81 and LCD03    *
*         TPA81 controlled by I2C           *
*        LCD03 controlled by serial         *
*                                           *
*         By James Henderson 2012           *
********************************************

 This example was created by José Cruz (www.botnroll.com)
 on 17 March 2015

How the program works:
Reads the sensor array temperature and displays the information on Serial Monitor and Bot'n Roll ONE A LCD.

 This code example is in the public domain. 
 http://www.botnroll.com

*/

#include <BnrOneA.h>   // Bot'n Roll ONE A library
#include <SPI.h>       // SPI communication library required by BnrOne.cpp
#include <Wire.h>
BnrOneA one;           // declaration of object variable to control the Bot'n Roll ONE A

//constants definition
#define SSPIN  2       // Slave Select (SS) pin for SPI communication
#define ADDRESS     0x68       // Address of TPA81
#define SOFTREG     0x00       // Byte for software version
#define AMBIANT     0x01       // Byte for ambiant temperature

int temperature[] = {0,0,0,0,0,0,0,0};                   // Array to hold temperature data

void setup()
{
    Serial.begin(57600);     // sets baud rate to 57600bps for printing values at serial monitor.
    one.spiConnect(SSPIN);   // starts the SPI communication module
    one.stop();              // stops motors
    Wire.begin();
  
    byte software = getData(SOFTREG);      // Get software version 
    one.lcd1("TPA81 Version: ",software);    // Print software version on LCD
    Serial.print(" Software Ver: "); Serial.println(software);

    int ambiantTemp = getData(AMBIANT);                    // Get reading of ambiant temperature and print to LCD03 screen
    one.lcd2("Ambiant: ",ambiantTemp);
    Serial.print("Ambiant: "); Serial.println(ambiantTemp);

    delay(2000);  //Pause to read LCD
}
  
void loop()
{
    for(int i = 0; i < 8; i++)              // Loops and stores temperature data in array
    {                            
        temperature[i] = getData(i+2);
    } 
    one.lcd1(temperature[0],temperature[1],temperature[2],temperature[3]);
    one.lcd2(temperature[4],temperature[5],temperature[6],temperature[7]);
    Serial.print(" T0:"); Serial.print(temperature[0]);
    Serial.print(" T1:"); Serial.print(temperature[1]);
    Serial.print(" T2:"); Serial.print(temperature[2]);
    Serial.print(" T3:"); Serial.print(temperature[3]);
    Serial.print(" T4:"); Serial.print(temperature[4]);
    Serial.print(" T5:"); Serial.print(temperature[5]);
    Serial.print(" T6:"); Serial.print(temperature[6]);
    Serial.print(" T7:"); Serial.println(temperature[7]);
    delay(50);
}

byte getData(byte reg)
{                                   // Function to receive one byte of data from TPA81
    Wire.beginTransmission(ADDRESS);                        // Begin communication with TPA81
    Wire.write(reg);                                        // Send reg to TPA81
    Wire.endTransmission();
    Wire.requestFrom(ADDRESS, 1);                           // Request 1 byte
    while(Wire.available() < 1);                            // Wait for byte to arrive
    byte data = Wire.read();                                // Get byte
    return(data);                                           // return byte
}
