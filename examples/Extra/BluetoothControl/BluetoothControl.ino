/* 
 This example was created by Helder Ribeiro
 on 10 March 2016
 Revised by Jose Cruz (www.botnroll.com)
 on 23 March 2016
 
 This code example is in the public domain. 
 http://www.botnroll.com

 This program allows a complete diagnosis of the whole Bot'n Roll ONE A hardware
 This program must be uploaded to the robot when using the Android APP to control Bot'n Roll ONE A using Bluetooth.
*/
#include <Wire.h>        // required by BnrRescue.cpp
#include <BnrRescue.h>   // Bot'n Roll CoSpace Rescue Module library
#include <BnrOneA.h>   // Bot'n Roll ONE A library
#include <SPI.h>       // SPI communication library required by BnrOne.cpp
#include <Servo.h>     // Gripper
#include "Timer.h"

//constants definitions
#define MODULE_ADDRESS 0x2C        //default factory address
#define SSPIN  2       // Slave Select (SS) pin for SPI communication

#define echoPin 8 // Echo Pin
#define trigPin 7 // Trigger Pin

#define ADDRESSCMPS10 0x60  // Defines address of CMPS10

#define ADDRESSTPA81     0x68       // Address of TPA81
#define SOFTREG     0x00       // Byte for software version
#define AMBIANT     0x01       // Byte for ambiant temperature

#define maximumRange 200   // Maximum range needed (200cm)
#define minimumRange   0   // Minimum range needed

#define R 3
#define G 5
#define B 6

#define LS1  0
#define LS2  1
#define LS3  2
#define LS4  3
#define LS5  4
#define LS6  5
#define LS7  6
#define LS8  7

#define BuzzerPin  9


BnrRescue brm;         // declaration of object variable to control Bot'n Roll Rescue Module
BnrOneA one;           // declaration of object variable to control the Bot'n Roll ONE A

Servo Gripper;
Servo Lift;

long int timeStamo = 0,BuzzerStamp = 0;

Timer t;
int dataSendTimeout = 200; // ms

int tempOffset = 5;

// the setup function runs once when you press reset or power the board
void setup() {
  //setup routines
  brm.i2cConnect(MODULE_ADDRESS);    //Enable I2C communication
  brm.setModuleAddress(0x2C);        //Change I2C Address
  brm.setSonarStatus(ENABLE);   //Enable/Disable Sonar scanning
  brm.setRgbStatus(ENABLE);     //Enable/Disable RGB scanning
  
  one.spiConnect(SSPIN);   // starts the SPI communication module
  one.stop();             // stops motors
  Wire.begin();          // Conects I2C
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  /*Gripper.attach(5);    // Attach the Servo variable to pin 5
  Lift.attach(3);    // Attach the Servo variable to pin 3*/
  
  one.servo1(90); //Central position 0º - 180º
  one.servo2(90); //Central position 0º - 180º
  
  // SONAR setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  one.lcd1("Bot'n Roll ONE A");
  one.lcd2("Bluetooth V2");
     
  pinMode(R, OUTPUT);
  pinMode(G, OUTPUT);
  pinMode(B, OUTPUT);
  
  analogWrite(R, 0);//RED 
  analogWrite(G, 0);//GREEN    
  analogWrite(B, 0);//BLUE 
  
  one.minBat(10.5);        // define de minimum battery voltage. Robot stops if voltage is below the specified value!
  
  Serial.begin(9600);//setup SerialPort
  
  t.every(dataSendTimeout, sendData);
  
  
}

// the loop function runs over and over again forever
void loop() {  
  
  t.update();
  
  if(Serial.available()>0)
  {
    String readBlueTooth = Serial.readStringUntil('\n');
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
    /*Serial.print("Pan: ");
    Serial.print(Pan);
    Serial.print("Tilt: ");
    Serial.println(Tilt);*/
    
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
 
    /*if(BuzzerFrequency!=0 && BuzzerTime!=0)tone(BuzzerPin, BuzzerFrequency,BuzzerTime);
    else noTone(9);*/
    
    /*Serial.print(" : ");
    Serial.print(r);
    Serial.print(" - ");
    Serial.print(g);
    Serial.print(" - ");
    Serial.println(b);*/
    
    /*Serial.print("Tempo: ");
    Serial.print(millis()-timeStamo);
    Serial.print(" : ");
    Serial.print(r);
    Serial.print(" - ");
    Serial.print(g);
    Serial.print(" - ");
    Serial.println(b);
    timeStamo = millis();*/
    
  }
  
}

float yaw = 0;
char roll, pitch;
byte pbutton;
long distance = 0;
int ambiantTemp = 0;
int mediaTempTPA81 = 0;
byte sonarL=0, sonarC=0, sonarR=0;
byte rgbL[3]={0,0,0};
byte rgbR[3]={0,0,0};
int line[8];
byte obstacles;
float battery;

void sendData()
{
  yaw = read_yaw();
  roll = read_roll();
  pitch = read_pitch();
  pbutton = one.readButton();       // read the Push Button value
  distance = Sonar();
  for(int i = 0; i < 8; i++)              // Loops and stores temperature data in array
  {                            
      mediaTempTPA81 += getData(i+2);
  }
  mediaTempTPA81 = mediaTempTPA81 /8;
  ambiantTemp = getData(AMBIANT);                    // Get reading of ambiant temperature and print to LCD03 screen
  
  brm.readSonars(&sonarL,&sonarC,&sonarR);       //Read the 3 sonars distance in cm
  
  brm.readRgbL(&rgbL[0],&rgbL[1],&rgbL[2]);      //Read Left RGB sensor
  brm.readRgbR(&rgbR[0],&rgbR[1],&rgbR[2]);      //Read Right RGB sensor
  
  for(int i=0;i<8;i++) line[i] = one.readAdc(i);
  
  obstacles = one.obstacleSensors(); //Read obstacle sensors
  
  battery = one.readBattery(); // read battery voltage

  Serial.print(yaw);//0
  Serial.print(","); Serial.print((int)pitch);//1
  Serial.print(","); Serial.print((int)roll);//2
  Serial.print(","); Serial.print(pbutton);//3
  Serial.print(","); Serial.print(distance);//4
  Serial.print(","); Serial.print(mediaTempTPA81-tempOffset);//5
  Serial.print(","); Serial.print(ambiantTemp);//6
  Serial.print(","); Serial.print(sonarL);//7
  Serial.print(","); Serial.print(sonarC);//8
  Serial.print(","); Serial.print(sonarR);//9
  Serial.print(","); Serial.print(rgbL[0]);Serial.print(",");Serial.print(rgbL[1]);Serial.print(",");Serial.print(rgbL[2]);//10//11//12
  Serial.print(","); Serial.print(rgbR[0]);Serial.print(",");Serial.print(rgbR[1]);Serial.print(",");Serial.print(rgbR[2]);//13//14//15
  for(int i=0;i<8;i++){ Serial.print(",");Serial.print(line[i]);}//16//17//18//19//20//21//22//23
  Serial.print(","); Serial.print(obstacles);//24
  Serial.print(","); Serial.println(battery);//25
  
}
