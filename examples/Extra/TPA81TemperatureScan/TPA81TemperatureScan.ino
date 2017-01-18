/*
TPA81 is a Pixel Thermal Array Sensor. 
Detects a candle flame at a range 2 meters (6ft) and is unaffected by ambient light!
Product can be found here:http://www.botnroll.com/index.php?id_product=131&controller=product&id_lang=7

 This example was created by José Cruz (www.botnroll.com)
 on 17 March 2015
 
How the program works:
Using the Bot'n Roll ONE A pan & tilt module scans an area for the highest temperature. 
In the end of the scan points to the place here the highest temperature was found. 
Waits 3 seconds and starts a new scan.

Pan & tilt Module with sonar: http://www.botnroll.com/index.php?id_product=813&controller=product&id_lang=7

 This code example is in the public domain. 
 http://www.botnroll.com
*/
#include <BnrOneA.h>   // Bot'n Roll ONE A library
#include <SPI.h>       // SPI communication library required by BnrOne.cpp
#include <Wire.h>
BnrOneA one;           // declaration of object variable to control the Bot'n Roll ONE A

//constants definition
#define SSPIN  2       // Slave Select (SS) pin for SPI communication
#define ADDRESS     0x68      // Address of TPA81
#define SOFTREG     0x00      // Byte for software version
#define AMBIANT     0x01      // Byte for ambiant temperature
#define PAN_MIN     50
#define PAN_MAX     185
#define PAN_INC     35        //Increment angle for pan sweep -> 4x35=140 
#define PAN_MOVES   4         //Number of movements in sweep
#define TILT_MIN    20
#define TILT_MAX    80
#define TILT_INC    6         //Increment angle for tilt sweep   6x10=60
#define TILT_MOVES  10        //Number of movements in sweep
#define TEMP_ARRAY  8         //Temperature array size


int temperature[TEMP_ARRAY] = {0,0,0,0,0,0,0,0};                   // Array to hold temperature data
int temper_average[PAN_MOVES][TILT_MOVES];
int p,t,pan,tilt;

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
    one.servo1(PAN_MIN);
    one.servo2(TILT_MIN);
    delay(150);
    //sweep area and read temperatures
    for(p=0; p<PAN_MOVES; p++)
    {
      pan=(int)(PAN_MIN+(p*PAN_INC));
        one.servo1(pan);
        for(t=0; t<TILT_MOVES; t++)
        {
            tilt=(int)(TILT_MIN+(t*TILT_INC));
            one.servo2(tilt);
            if(pan!=PAN_MIN && tilt==TILT_MIN)
              delay(75);
            delay(40);
            int sum=0;
            for(int i = 0; i < 8; i++)              // Loops and stores temperature data in array
            {                            
                temperature[i] = getData(i+2);
                sum+=temperature[i];
            } 
            temper_average[p][t]=(int)(sum/TEMP_ARRAY);
            one.lcd1(temperature[0],temperature[1],temperature[2],temperature[3]);
            one.lcd2(temperature[4],temperature[5],temperature[6],temperature[7]);
            Serial.print(" T0:"); Serial.print(temperature[0]);
            Serial.print(" T1:"); Serial.print(temperature[1]);
            Serial.print(" T2:"); Serial.print(temperature[2]);
            Serial.print(" T3:"); Serial.print(temperature[3]);
            Serial.print(" T4:"); Serial.print(temperature[4]);
            Serial.print(" T5:"); Serial.print(temperature[5]);
            Serial.print(" T6:"); Serial.print(temperature[6]);
            Serial.print(" T7:"); Serial.print(temperature[7]);
            Serial.print(" PAN:"); Serial.print(p);
            Serial.print(" TILT:"); Serial.print(t);            
            Serial.print(" AVG:"); Serial.println(temper_average[p][t]);
       }
    }
    //Data treatment: Search for the maximum average and place the Pan & Tilt accordingly
    int avg_max=0;
    int maxp=0;
    int maxt=0;
    for(p=0; p<PAN_MOVES; p++)
    {
        for(t=0; t<TILT_MOVES; t++)
        {
           if(temper_average[p][t]>avg_max)
           {
               avg_max=temper_average[p][t];
               maxp=p;
               maxt=t;
           }
        }
    }
    pan=(int)(PAN_MIN+(maxp*PAN_INC));
    tilt=(int)(TILT_MIN+(maxt*TILT_INC));

    one.servo1(pan);
    one.servo2(tilt);
    Serial.print(" PAN:"); Serial.print(maxp);
    Serial.print(" TILT:"); Serial.print(maxt);            
    Serial.print(" AVG:"); Serial.println(temper_average[maxp][maxt]);
    delay(3000);
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
