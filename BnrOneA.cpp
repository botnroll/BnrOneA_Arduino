/*
  BnrOneA.cpp - Library for interfacing with Bot'n Roll ONE Arduino Compatible from www.botnroll.com
  Created by José Cruz, November 28, 2013.
  Updated December 19, 2014.
  Released into the public domain.
*/

#include "SPI.h"
#include "BnrOneA.h"

///////////////////////////////////////////////////////////////////////
//setup routines
///////////////////////////////////////////////////////////////////////
void BnrOneA::spiConnect(byte sspin)
{
    _sspin=sspin;
    pinMode(_sspin,OUTPUT); //sspin as output
    SPI.begin();    //Initializes the SPI bus by setting SCK and MOSI to outputs, pulling SCK and MOSI low.
    SPI.setBitOrder(MSBFIRST); //Sets the order of the bits shifted out of and into the SPI bus MSBFIRST (most-significant bit first).
    SPI.setDataMode(SPI_MODE1); //Sets the SPI data mode: that is, clock polarity and phase.
    digitalWrite(_sspin, HIGH); // SPI in hold state by pulling SS high.
    delayMicroseconds(50);
}

///////////////////////////////////////////////////////////////////////
//private routines
///////////////////////////////////////////////////////////////////////
byte BnrOneA::spiRequestByte(byte command)
{
    byte value=(byte)0xFF;
    byte buffer[]={KEY1,KEY2};
    spiSendData(command,buffer,sizeof(buffer));//Request data from master
    digitalWrite(_sspin, LOW); // Select the SPI Slave device to start communication.
    delayMicroseconds(10);
    value=SPI.transfer(0x00);  // Reads one byte
    delayMicroseconds(60);
    digitalWrite(_sspin, HIGH); // Close communication with slave device.
    delayMicroseconds(10);
    return value;
}
int BnrOneA::spiRequestWord(byte command)
{
    byte value[2]={0,0};
    int i=0;
    byte buffer[]={KEY1,KEY2};
    spiSendData(command,buffer,sizeof(buffer));//Request data from master
    digitalWrite(_sspin, LOW); // Select the SPI Slave device to start communication.
    delayMicroseconds(10);
    for (i=0; i<2; i++)
    {
        value[i]=SPI.transfer(0x00);  // Reads one byte
        delayMicroseconds(60);
    }
    i=0;
    i=value[0];
    i=i<<8;
    i=i+value[1];
    digitalWrite(_sspin, HIGH); // Close communication with slave device.
    delayMicroseconds(10);
  return i;
}
void BnrOneA::spiSendData(byte command, byte buffer[], byte numBytes)
{
    digitalWrite(_sspin, LOW); // Select the SPI Slave device to start communication.
    delayMicroseconds(10);
    SPI.transfer(command);        // Sends one byte
    delayMicroseconds(60);
    for (int k =0; k< numBytes;k++)
    {
        SPI.transfer(buffer[k]);  // Sends one byte
    delayMicroseconds(60);
    }
    digitalWrite(_sspin, HIGH); // Close communication with slave device.
    delayMicroseconds(10);
}

///////////////////////////////////////////////////////////////////////
//Write routines
///////////////////////////////////////////////////////////////////////
void BnrOneA::move(int speedL,int speedR)
{
    byte speedL_H=highByte(speedL);
    byte speedL_L=lowByte(speedL);
    byte speedR_H=highByte(speedR);
    byte speedR_L=lowByte(speedR);

    byte buffer[]={KEY1,KEY2,speedL_H,speedL_L,speedR_H,speedR_L};
    spiSendData(COMMAND_MOVE,buffer,sizeof(buffer));
    delay(5);//Wait while command is processed
}
void BnrOneA::movePID(int speedL,int speedR)
{
    byte speedL_H=highByte(speedL);
    byte speedL_L=lowByte(speedL);
    byte speedR_H=highByte(speedR);
    byte speedR_L=lowByte(speedR);

    byte buffer[]={KEY1,KEY2,speedL_H,speedL_L,speedR_H,speedR_L};
    spiSendData(COMMAND_MOVE_PID,buffer,sizeof(buffer));
    delay(5);//Wait while command is processed
}
void BnrOneA::setPID(int kp, int ki, int kd)
{
    byte kp_H=highByte(kp);
    byte kp_L=lowByte(kp);
    byte ki_H=highByte(ki);
    byte ki_L=lowByte(ki);
    byte kd_H=highByte(kd);
    byte kd_L=lowByte(kd);

    byte buffer[]={KEY1,KEY2,kp_H,kp_L,ki_H,ki_L,kd_H,kd_L};
    spiSendData(COMMAND_PID_CFG,buffer,sizeof(buffer));
    delay(20);//Wait while command is processed
}
void BnrOneA::stop()
{
    byte buffer[]={KEY1,KEY2};
    spiSendData(COMMAND_STOP,buffer,sizeof(buffer));
    delay(5);//Wait while command is processed
}
void BnrOneA::brake(byte torqueL,byte torqueR)
{
    byte buffer[]={KEY1,KEY2,torqueL,torqueR};
    spiSendData(COMMAND_BRAKE,buffer,sizeof(buffer));
    delay(5);//Wait while command is processed
}
void BnrOneA::resetEncL()
{
    byte buffer[]={KEY1,KEY2};
    spiSendData(COMMAND_ENCL_RESET,buffer,sizeof(buffer));
    delay(1);//Wait while command is processed
}
void BnrOneA::resetEncR()
{
    byte buffer[]={KEY1,KEY2};
    spiSendData(COMMAND_ENCR_RESET,buffer,sizeof(buffer));
    delay(1);//Wait while command is processed
}
void BnrOneA::led(boolean state)
{
    if(state>1)
    {
        state=1;
    }
    byte buffer[]={KEY1,KEY2,(byte)state};
    spiSendData(COMMAND_LED,buffer,sizeof(buffer));
    delay(1);//Wait while command is processed
}
void BnrOneA::obstacleEmitters(boolean state)
{
    if(state>1)
    {
        state=1;
    }
    byte buffer[]={KEY1,KEY2,(byte)state};
    spiSendData(COMMAND_IR_EMITTERS,buffer,sizeof(buffer));
    delay(1);//Wait while command is processed
}
void BnrOneA::servo1(byte position)
{
    byte buffer[]={KEY1,KEY2,position};
    spiSendData(COMMAND_SERVO1,buffer,sizeof(buffer));
    delay(2);//Wait while command is processed
}
void BnrOneA::servo2(byte position)
{
    byte buffer[]={KEY1,KEY2,position};
    spiSendData(COMMAND_SERVO2,buffer,sizeof(buffer));
    delay(2);//Wait while command is processed
}

void BnrOneA::minBat(float batmin)
{
    int intg=(int)batmin;
    int dec=(int)((batmin-intg)*1000);
    byte intg_H=highByte(intg);
    byte intg_L=lowByte(intg);
    byte dec_H=highByte(dec);
    byte dec_L=lowByte(dec);
    byte buffer[]={KEY1,KEY2,intg_H,intg_L,dec_H,dec_L};
    spiSendData(COMMAND_BAT_MIN,buffer,sizeof(buffer));
    delay(25);//Wait while command is processed
}

//////////////////////////////////////////////////////////////////////////////////////
// Readings routines
//////////////////////////////////////////////////////////////////////////////////////
byte BnrOneA::readButton()
{
    int adc;
    byte button;
    adc=spiRequestWord(COMMAND_BUT_READ);
    if(adc>=0 && adc<250)
    {
      button=1;
    }
    else if(adc>=250 && adc<620)  //(adc>=250 && adc<598)
    {
      button=2;
    }
    else if(adc>=620 && adc<950) //(adc>=598 && adc<950)
    {
      button=3;
    }
    else
    {
      button=0;
    }
    return button;
}
float BnrOneA::readBattery()
{
   return (float)((float)(spiRequestWord(COMMAND_BAT_READ))/50.7);
}
int BnrOneA::readEncL()
{
    return spiRequestWord(COMMAND_ENCL);
}
int BnrOneA::readEncR()
{
    return spiRequestWord(COMMAND_ENCR);
}
int BnrOneA::readEncLInc()
{
    return spiRequestWord(COMMAND_ENCL_INC);
}
int BnrOneA::readEncRInc()
{
    return spiRequestWord(COMMAND_ENCR_INC);
}
/*
void BnrOneA::readFirmware(byte *firm1,byte *firm2,byte *firm3)
{
    byte value[3]={0,0,0};
    int k=0;
    byte buffer[]={KEY1,KEY2};
    spiSendData(COMMAND_FIRMWARE,buffer,sizeof(buffer));//Request data from master
    digitalWrite(_sspin, LOW); // Select the SPI Slave device to start communication.
    delayMicroseconds(5);
    for (k=0; k<3; k++)
    {
        value[k]=SPI.transfer(0x00);  // Reads one byte
        delayMicroseconds(50);
    }
    *firm1=value[0];
    *firm2=value[1];
    *firm3=value[2];
    digitalWrite(_sspin, HIGH); // Close communication with slave device.
}
 */
byte BnrOneA::obstacleSensors()
{
    return spiRequestByte(COMMAND_OBSTACLES);
}
byte BnrOneA::readIRSensors()
{
    return spiRequestByte(COMMAND_IR_SENSORS);
}
int BnrOneA::readAdc(byte channel)
{
  byte command=0x00;
  switch(channel)
  {
      case 0:
          command=COMMAND_ADC0;
          break;
      case 1:
          command=COMMAND_ADC1;
          break;
      case 2:
          command=COMMAND_ADC2;
          break;
      case 3:
          command=COMMAND_ADC3;
          break;
      case 4:
          command=COMMAND_ADC4;
          break;
      case 5:
          command=COMMAND_ADC5;
          break;
      case 6:
          command=COMMAND_ADC6;
          break;
      case 7:
          command=COMMAND_ADC7;
          break;
  }
  return spiRequestWord(command);
}

int BnrOneA::readAdc0()
{
    return spiRequestWord(COMMAND_ADC0);
}
int BnrOneA::readAdc1()
{
    return spiRequestWord(COMMAND_ADC1);
}
int BnrOneA::readAdc2()
{
    return spiRequestWord(COMMAND_ADC2);
}
int BnrOneA::readAdc3()
{
    return spiRequestWord(COMMAND_ADC3);
}
int BnrOneA::readAdc4()
{
    return spiRequestWord(COMMAND_ADC4);
}
int BnrOneA::readAdc5()
{
    return spiRequestWord(COMMAND_ADC5);
}
int BnrOneA::readAdc6()
{
    return spiRequestWord(COMMAND_ADC6);
}
int BnrOneA::readAdc7()
{
    return spiRequestWord(COMMAND_ADC7);
}
int BnrOneA::readDBG(byte channel)
{
  byte command=0x00;
  switch(channel)
  {
      case 0:
          command=0xC0;
          break;
      case 1:
          command=0xC1;
          break;
      case 2:
          command=0xC2;
          break;
      case 3:
          command=0xC3;
          break;
      case 4:
          command=0xC4;
          break;
      case 5:
          command=0xC5;
          break;
      case 6:
          command=0xC6;
          break;
      case 7:
          command=0xC7;
          break;
  }
  return spiRequestWord(command);
}
/**************************************************************/
/**** LCD LINE 1 Handlers *************************************/
/**************************************************************/
void BnrOneA::lcd1(byte string[])
{   
    int i,a;
    byte buffer[19];
    char string1[19],string2[19];
    for(i=0;i<16;i++){
        string2[i]=string[i];
    }
    string2[16]=0;
    a=sprintf(string1,string2);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<a;i++){
        buffer[i+2]=string1[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=' ';
    }
    spiSendData(COMMAND_LCD_L1,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd1(const char string[])
{
    int i,a;
    byte buffer[19];
    char string1[19],string2[19];
    for(i=0;i<16;i++){
        string2[i]=string[i];
    }
    string2[16]=0;
    a=sprintf(string1,"%s",string2);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<a;i++){
        buffer[i+2]=string1[i];
    }
    for(i=a;i<16;i++)
    {
        buffer[i+2]=' ';
    }
    spiSendData(COMMAND_LCD_L1,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd1(int number)
{
    int i, a=0;
    byte buffer[19];
    char string[17];
    a=sprintf(string,"%d",number);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<16;i++){
        buffer[i+2]=string[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=(' ');
    }
    spiSendData(COMMAND_LCD_L1,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd1(unsigned int number)
{
    int i, a=0;
    byte buffer[19];
    char string[17];
    a=sprintf(string,"%u",number);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<16;i++){
        buffer[i+2]=string[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=(' ');
    }
    spiSendData(COMMAND_LCD_L1,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd1(long int number)
{
    int i, a=0;
    byte buffer[19];
    char string[17];
    a=sprintf(string,"%ld",number);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<16;i++){
        buffer[i+2]=string[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=(' ');
    }
    spiSendData(COMMAND_LCD_L1,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd1(double number)
{
    int i,a;
    int intg;
	int dec;
    byte buffer[19];
    char string[19];
	bool flag_neg=0;
    
	if (number<-0.0001)
	{
		flag_neg=1;
		number*=-1.0;
	}
	dec = round((number-((double)(int)number))*100.0) % 100;
	intg = (dec==0 ? round(number):(int)number);
    a=sprintf(string,"%d.%02d            ",intg,dec);
	
    buffer[0]=KEY1;
    buffer[1]=KEY2;
	if (flag_neg==1)
		buffer[2]='-';
    for(i=0;i<16;i++){
        buffer[i+2+flag_neg]=string[i];
    }
    spiSendData(COMMAND_LCD_L1,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd1(const char string[],int number)
{
    int i, a, b;
    byte buffer[19];
    char string1[19],string2[19];
    for(i=0;i<16;i++){
        string2[i]=string[i];
    }
    string2[16]=0;
    a=sprintf(string1,string2);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<a;i++){
        buffer[i+2]=string1[i];
    }
    a+=2;
    b=sprintf(string1,"%d",number);
    for(i=0;i<b;i++){
        buffer[i+a]=string1[i];
    }
    for(i=a+b;i<18;i++){
        buffer[i]=(' ');
    }
    spiSendData(COMMAND_LCD_L1,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd1(const char string[],unsigned int number)
{
    int i, a, b;
    byte buffer[19];
    char string1[19],string2[19];
    for(i=0;i<16;i++){
        string2[i]=string[i];
    }
    string2[16]=0;
    a=sprintf(string1,string2);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<a;i++){
        buffer[i+2]=string1[i];
    }
    a+=2;
    b=sprintf(string1,"%u",number);
    for(i=0;i<b;i++){
        buffer[i+a]=string1[i];
    }
    for(i=a+b;i<18;i++){
        buffer[i]=(' ');
    }
    spiSendData(COMMAND_LCD_L1,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd1(const char string[],long int number)
{
    int i, a, b;
    byte buffer[19];
    char string1[19],string2[19];
    for(i=0;i<16;i++){
        string2[i]=string[i];
    }
    string2[16]=0;
    a=sprintf(string1,string2);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<a;i++){
        buffer[i+2]=string1[i];
    }
    a+=2;
    b=sprintf(string1,"%ld",number);
    for(i=0;i<b;i++){
        buffer[i+a]=string1[i];
    }
    for(i=a+b;i<18;i++){
        buffer[i]=(' ');
    }
    spiSendData(COMMAND_LCD_L1,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd1(const char string[],double number)
{
    int i, a, b;
    char string1[19];
    char string2[19];
    byte buffer[19];
    int intg, dec;
	bool flag_neg=0;

    for(i=0;i<16;i++){
        string2[i]=string[i];
    }
    string2[16]=0;    
    a=sprintf(string1,string2);    
	buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<a;i++){
        buffer[i+2]=string1[i];
    }
    a+=2;
	if (number<-0.0001){
		flag_neg=1;
		number*=-1.0;
	}
	dec = round((number-((double)(int)number))*100.0) % 100;
	intg = (dec==0 ? round(number):(int)number);
    b=sprintf(string2,"%d.%02d            ",intg,dec);
	if (flag_neg==1){
		buffer[a]='-';
		a++;
	}		
    for(i=0;i<b;i++){
		if((i+a)<18)
			buffer[i+a]=string2[i];
    }
	for(i=0;i<18;i++){
		Serial.println(buffer[i],DEC);
	}
	Serial.println();
    spiSendData(COMMAND_LCD_L1,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd1(unsigned int num1, unsigned int num2)
{
    int i, a=0;
    byte buffer[19];
    char string[17];
    a=sprintf(string,"%5u%5u",num1,num2);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<16;i++){
        buffer[i+2]=string[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=(' ');
    }
    spiSendData(COMMAND_LCD_L1,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd1(unsigned int num1, unsigned int num2, unsigned int num3)
{
    int i, a=0;
    byte buffer[19];
    char string[17];
    a=sprintf(string,"%5u%5u%5u",num1,num2,num3);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<16;i++){
        buffer[i+2]=string[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=(' ');
    }
    spiSendData(COMMAND_LCD_L1,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd1(unsigned int num1, unsigned int num2, unsigned int num3, unsigned int num4)
{
    int i, a=0;
    byte buffer[19];
    char string[17];
    a=sprintf(string,"%4u%4u%4u%4u",num1,num2,num3,num4);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<16;i++){
        buffer[i+2]=string[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=(' ');
    }
    spiSendData(COMMAND_LCD_L1,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd1(int num1, int num2)
{
    int i, a=0;
    byte buffer[19];
    char string[17];
    a=sprintf(string,"%5d%5d",num1,num2);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<16;i++){
        buffer[i+2]=string[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=(' ');
    }
    spiSendData(COMMAND_LCD_L1,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd1(int num1, int num2, int num3)
{
    int i, a=0;
    byte buffer[19];
    char string[17];
    a=sprintf(string,"%5d%5d%5d",num1,num2,num3);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<16;i++){
        buffer[i+2]=string[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=(' ');
    }
    spiSendData(COMMAND_LCD_L1,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd1(int num1, int num2, int num3, int num4)
{
    int i, a=0;
    byte buffer[19];
    char string[17];
    a=sprintf(string,"%4d%4d%4d%4d",num1,num2,num3,num4);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<16;i++){
        buffer[i+2]=string[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=(' ');
    }
    spiSendData(COMMAND_LCD_L1,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
/**************************************************************/
/**** LCD LINE 2 Handlers *************************************/
/**************************************************************/
void BnrOneA::lcd2(byte string[])
{
    int i,a;
    byte buffer[19];
    char string1[19],string2[19];
    for(i=0;i<16;i++){
        string2[i]=string[i];
    }
    string2[16]=0;
    a=sprintf(string1,string2);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<a;i++){
        buffer[i+2]=string1[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=' ';
    }
    spiSendData(COMMAND_LCD_L2,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd2(const char string[])
{
    int i,a;
    byte buffer[19];
    char string1[19],string2[19];
    for(i=0;i<16;i++){
        string2[i]=string[i];
    }
    string2[16]=0;
    a=sprintf(string1,string2);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<a;i++){
        buffer[i+2]=string1[i];
    }
    for(i=a;i<16;i++)
    {
        buffer[i+2]=' ';
    }
    spiSendData(COMMAND_LCD_L2,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd2(int number)
{
    int i, a=0;
    byte buffer[19];
    char string[17];
    a=sprintf(string,"%d",number);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<16;i++){
        buffer[i+2]=string[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=(' ');
    }
    spiSendData(COMMAND_LCD_L2,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd2(unsigned int number)
{
    int i, a=0;
    byte buffer[19];
    char string[17];
    a=sprintf(string,"%u",number);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<16;i++){
        buffer[i+2]=string[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=(' ');
    }
    spiSendData(COMMAND_LCD_L2,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd2(long int number)
{
    int i, a=0;
    byte buffer[19];
    char string[17];
    a=sprintf(string,"%ld",number);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<16;i++){
        buffer[i+2]=string[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=(' ');
    }
    spiSendData(COMMAND_LCD_L2,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd2(double number)
{
    int i,a;
    int intg;
	int dec;
    byte buffer[19];
    char string[19];
	bool flag_neg=0;
    
	if (number<-0.0001)
	{
		flag_neg=1;
		number*=-1.0;
	}
	dec = round((number-((double)(int)number))*100.0) % 100;
	intg = (dec==0 ? round(number):(int)number);
    a=sprintf(string,"%d.%02d            ",intg,dec);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
	if (flag_neg==1)
		buffer[2]='-';
    for(i=0;i<16;i++){
        buffer[i+2+flag_neg]=string[i];
    }
    spiSendData(COMMAND_LCD_L2,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd2(const char string[],int number)
{
    int i, a, b;
    byte buffer[19];
    char string1[19],string2[19];
    for(i=0;i<16;i++){
        string2[i]=string[i];
    }
    string2[16]=0;
    a=sprintf(string1,string2);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<a;i++){
        buffer[i+2]=string1[i];
    }
    a+=2;
    b=sprintf(string1,"%d",number);
    for(i=0;i<b;i++){
        buffer[i+a]=string1[i];
    }
    for(i=a+b;i<18;i++){
        buffer[i]=(' ');
    }
    spiSendData(COMMAND_LCD_L2,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd2(const char string[],unsigned int number)
{
    int i, a, b;
    byte buffer[19];
    char string1[19],string2[19];
    for(i=0;i<16;i++){
        string2[i]=string[i];
    }
    string2[16]=0;
    a=sprintf(string1,string2);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<a;i++){
        buffer[i+2]=string1[i];
    }
    a+=2;
    b=sprintf(string1,"%u",number);
    for(i=0;i<b;i++){
        buffer[i+a]=string1[i];
    }
    for(i=a+b;i<18;i++){
        buffer[i]=(' ');
    }
    spiSendData(COMMAND_LCD_L2,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd2(const char string[],long int number)
{
    int i, a, b;
    byte buffer[19];
    char string1[19],string2[19];
    for(i=0;i<16;i++){
        string2[i]=string[i];
    }
    string2[16]=0;
    a=sprintf(string1,string2);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<a;i++){
        buffer[i+2]=string1[i];
    }
    a+=2;
    b=sprintf(string1,"%ld",number);
    for(i=0;i<b;i++){
        buffer[i+a]=string1[i];
    }
    for(i=a+b;i<18;i++){
        buffer[i]=(' ');
    }
    spiSendData(COMMAND_LCD_L2,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd2(const char string[],double number)
{
    int i, a, b;
    char string1[19];
    char string2[19];
    byte buffer[19];
    int intg, dec;
	bool flag_neg=0;

    for(i=0;i<16;i++){
        string2[i]=string[i];
    }
    string2[16]=0;    
    a=sprintf(string1,string2);
	buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<a;i++){
        buffer[i+2]=string1[i];
    }
    a+=2;
	if (number<-0.0001){
		flag_neg=1;
		number*=-1.0;
	}
	dec = round((number-((double)(int)number))*100.0) % 100;
	intg = (dec==0 ? round(number):(int)number);
    b=sprintf(string2,"%d.%02d            ",intg,dec);
	if (flag_neg==1){
		buffer[a]='-';
		a++;
	}		
    for(i=0;i<b;i++){
		if((i+a)<18)
			buffer[i+a]=string2[i];
    }
    spiSendData(COMMAND_LCD_L2,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd2(unsigned int num1, unsigned int num2)
{
    int i, a=0;
    byte buffer[19];
    char string[17];
    a=sprintf(string,"%5u%5u",num1,num2);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<16;i++){
        buffer[i+2]=string[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=(' ');
    }
    spiSendData(COMMAND_LCD_L2,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd2(unsigned int num1, unsigned int num2, unsigned int num3)
{
    int i, a=0;
    byte buffer[19];
    char string[17];
    a=sprintf(string,"%5u%5u%5u",num1,num2,num3);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<16;i++){
        buffer[i+2]=string[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=(' ');
    }
    spiSendData(COMMAND_LCD_L2,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd2(unsigned int num1, unsigned int num2, unsigned int num3, unsigned int num4)
{
    int i, a=0;
    byte buffer[19];
    char string[17];
    a=sprintf(string,"%4u%4u%4u%4u",num1,num2,num3,num4);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<16;i++){
        buffer[i+2]=string[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=(' ');
    }
    spiSendData(COMMAND_LCD_L2,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd2(int num1, int num2)
{
    int i, a=0;
    byte buffer[19];
    char string[17];
    a=sprintf(string,"%5d%5d",num1,num2);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<16;i++){
        buffer[i+2]=string[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=(' ');
    }
    spiSendData(COMMAND_LCD_L2,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd2(int num1, int num2, int num3)
{
    int i, a=0;
    byte buffer[19];
    char string[17];
    a=sprintf(string,"%5d%5d%5d",num1,num2,num3);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<16;i++){
        buffer[i+2]=string[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=(' ');
    }
    spiSendData(COMMAND_LCD_L2,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
void BnrOneA::lcd2(int num1, int num2, int num3, int num4)
{
    int i, a=0;
    byte buffer[19];
    char string[17];
    a=sprintf(string,"%4d%4d%4d%4d",num1,num2,num3,num4);
    buffer[0]=KEY1;
    buffer[1]=KEY2;
    for(i=0;i<16;i++){
        buffer[i+2]=string[i];
    }
    for(i=a;i<16;i++){
        buffer[i+2]=(' ');
    }
    spiSendData(COMMAND_LCD_L2,buffer,sizeof(buffer));
    delay(19);//Wait while command is processed
}
