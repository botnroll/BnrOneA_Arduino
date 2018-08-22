/* 
 This example was created by Helder Ribeiro
 on 10 March 2016
 Revised by Jose Cruz (www.botnroll.com)
 on 11 August 2016
 
 This code example is in the public domain. 
 http://www.botnroll.com

 Remote control the Bot'n Roll ONE A robot using BnrOneA APP for Android, a Bluetooth Shield on the robot and Bluetooth communication.
 This program must be uploaded to the robot when using the Android APP to control Bot'n Roll ONE A using Bluetooth.
*/
#include <Wire.h>      // required by BnrRescue.cpp
#include <BnrRescue.h> // Bot'n Roll Rescue Module library
#include <BnrOneA.h>   // Bot'n Roll ONE A library
#include <SPI.h>       // SPI communication library required by BnrOne.cpp
#include <Servo.h>     // Gripper
#include "Timer.h"

#define SENDDATA
//#define READBLUE

//constants definitions
#define MODULE_ADDRESS 0x2C   //Rescue Module I2C address
#define SSPIN  2        // Slave Select (SS) pin for SPI communication

#define echoPin 8 // Sonar Echo Pin
#define trigPin 7 // Sonar Trigger Pin

#define ADDRESSCMPS10 0x60  // CMPS11 I2C address

#define ADDRESSTPA81 0x68      // TPA81 I2C address
#define SOFTREG     0x00       // Byte for software version
#define AMBIENT     0x01       // Byte for ambient temperature

#define maximumRange 200   // Maximum range needed (200cm)
#define minimumRange   0   // Minimum range needed

#define R 3	// Red 
#define G 5 // Green
#define B 6 // Blue

#define LS1  0 // Line Sensor 0
#define LS2  1
#define LS3  2
#define LS4  3
#define LS5  4
#define LS6  5
#define LS7  6
#define LS8  7

#define BuzzerPin  9 // Buzzer pin


BnrRescue brm; // declaration of object variable to control Bot'n Roll Rescue Module
BnrOneA one;   // declaration of object variable to control the Bot'n Roll ONE A

Servo Gripper;
Servo Lift;

long int timeStamo = 0,BuzzerStamp = 0;

Timer t;
int dataSendTimeout = 200; // ms

int tempOffset = 5;

bool asLineSensor = false,asCompass = false,asGripper = false,asSearchAndRescue = false,asCoSpace = false,asTemperature = false;

// the setup function runs once on power on or after pressing the reset button
void setup() {
  //setup routines
  brm.i2cConnect(MODULE_ADDRESS);    //Enable I2C communication
  brm.setModuleAddress(0x2C);        //Change I2C Address
  brm.setSonarStatus(ENABLE);   //Enable/Disable Sonar scanning
  brm.setRgbStatus(ENABLE);     //Enable/Disable RGB scanning
  
  one.spiConnect(SSPIN);  // starts the SPI communication module
  one.stop();             // stops motors
  Wire.begin();           // start I2C BUS
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Gripper.attach(5);  // Attach the Servo variable to pin 5
  Lift.attach(3);     // Attach the Servo variable to pin 3
  
  one.servo1(90); //Central position 0º - 180º
  one.servo2(90); //Central position 0º - 180º
  
  // SONAR setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  one.lcd1("Bot'n Roll ONE A");
  one.lcd2("Bluetooth V2.04");
     
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  
  analogWrite(R, 0);//RED 
  analogWrite(G, 0);//GREEN    
  analogWrite(B, 0);//BLUE 
  
  one.minBat(10.5);   // define de minimum battery voltage. Robot stops if voltage is below the specified value!
  
  Serial.begin(9600); //setup SerialPort
  
  t.every(dataSendTimeout, sendData);
}

// the loop function runs over and over again forever
void loop() {  
  
  #ifdef SENDDATA
  t.update();
  #endif
  
  if(Serial.available()>0)
  {
    String readBlueTooth = Serial.readStringUntil('\n');
    
    if(readBlueTooth[0]=='*')
    {
      readBlueTooth = readBlueTooth.substring(1, readBlueTooth.length());
      
      #ifdef READBLUE
      Serial.println(readBlueTooth);
      #endif
      
      int MotorLeft = getValue(readBlueTooth, ',', 0).toInt();
      int MotorRight = getValue(readBlueTooth, ',', 1).toInt();
      int Pan = getValue(readBlueTooth, ',', 2).toInt();
      int Tilt = getValue(readBlueTooth, ',', 3).toInt();
      int Grip = getValue(readBlueTooth, ',', 4).toInt();
      int Liftr = getValue(readBlueTooth, ',', 5).toInt();
      String LCD1s = getValue(readBlueTooth, ',', 6);
      String LCD2s = getValue(readBlueTooth, ',', 7);
      int r = getValue(readBlueTooth, ',', 8).toInt();
      int g = getValue(readBlueTooth, ',', 9).toInt();
      int b = getValue(readBlueTooth, ',', 10).toInt();
      int BuzzerFrequency = getValue(readBlueTooth, ',', 11).toInt();
      int BuzzerTime = getValue(readBlueTooth, ',', 12).toInt();
      
      one.move(MotorLeft,MotorRight);
      
      one.servo2(Pan);
      one.servo1(Tilt);
      
      Gripper.write(Grip);
      Lift.write(Liftr);
      char charBuf1[50];
      LCD1s.toCharArray(charBuf1, LCD1s.length()+1);
      char charBuf2[50];
      LCD2s.toCharArray(charBuf2, LCD2s.length()+1);
      
      one.lcd1(charBuf1);
      one.lcd2(charBuf2);
      
      analogWrite(R, r);//RED 
      analogWrite(G, g);//GREEN    
      analogWrite(B, b);//BLUE 
      
      if(BuzzerTime!=0) BuzzerStamp = millis()+BuzzerTime;
      
      if(BuzzerFrequency!=0 && BuzzerTime!=0)tone(BuzzerPin, BuzzerFrequency,BuzzerTime);
      else if (millis()>BuzzerStamp)noTone(9);
      
     }
     else if(readBlueTooth[0]=='#')
     {
        readBlueTooth = readBlueTooth.substring(1, readBlueTooth.length());
        
        asLineSensor = getValue(readBlueTooth, ',', 0).toInt();
        asCompass = getValue(readBlueTooth, ',', 1).toInt();
        asGripper = getValue(readBlueTooth, ',', 2).toInt();
        asSearchAndRescue = getValue(readBlueTooth, ',', 3).toInt();
        asCoSpace = getValue(readBlueTooth, ',', 4).toInt();
        asTemperature = getValue(readBlueTooth, ',', 5).toInt();     
     }
  }
}

float yaw = 0;
char roll = 0, pitch = 0;
byte pbutton;
long distance = 0;
int ambientTemp = 0;
int mediaTempTPA81 = 0;
byte sonarL=0, sonarC=0, sonarR=0;
byte rgbL[3]={0,0,0};
byte rgbR[3]={0,0,0};
int line[8];
byte obstacles;
float battery;

void sendData()
{
  if(asCompass){
    yaw = read_yaw();
    roll = read_roll();
    pitch = read_pitch();
  }
  
  pbutton = one.readButton();       // read the Push Button value
  
  if(asSearchAndRescue) distance = Sonar();
  
  if(asTemperature) {
    
    for(int i = 0; i < 8; i++)              // Loops and stores temperature data in array
    {                            
        mediaTempTPA81 += getData(i+2);
    }
    mediaTempTPA81 = mediaTempTPA81 /8;
    ambientTemp = getData(AMBIENT);                    // Get reading of ambient temperature and print to LCD03 screen
  }
  
  if(asCoSpace){
  brm.readSonars(&sonarL,&sonarC,&sonarR);       //Read the 3 sonar's distance in cm
  
  brm.readRgbL(&rgbL[0],&rgbL[1],&rgbL[2]);      //Read Left RGB sensor
  brm.readRgbR(&rgbR[0],&rgbR[1],&rgbR[2]);      //Read Right RGB sensor
  }
  
  if(asLineSensor) for(int i=0;i<8;i++) line[i] = one.readAdc(i);
  
  obstacles = one.obstacleSensors(); //Read obstacle sensors
  
  battery = one.readBattery(); // read battery voltage

  Serial.print(yaw);//0
  Serial.print(","); Serial.print((int)pitch);//1
  Serial.print(","); Serial.print((int)roll);//2
  Serial.print(","); Serial.print(pbutton);//3
  Serial.print(","); Serial.print(distance);//4
  Serial.print(","); Serial.print(mediaTempTPA81-tempOffset);//5
  Serial.print(","); Serial.print(ambientTemp);//6
  Serial.print(","); Serial.print(sonarL);//7
  Serial.print(","); Serial.print(sonarC);//8
  Serial.print(","); Serial.print(sonarR);//9
  Serial.print(","); Serial.print(rgbL[0]);Serial.print(",");Serial.print(rgbL[1]);Serial.print(",");Serial.print(rgbL[2]);//10//11//12
  Serial.print(","); Serial.print(rgbR[0]);Serial.print(",");Serial.print(rgbR[1]);Serial.print(",");Serial.print(rgbR[2]);//13//14//15
  for(int i=0;i<8;i++){ Serial.print(",");Serial.print(line[i]);}//16//17//18//19//20//21//22//23
  Serial.print(","); Serial.print(obstacles);//24
  Serial.print(","); Serial.println(battery);//25
  
}
