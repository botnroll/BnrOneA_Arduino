/* 
 This example was created by José Cruz on October 2016 
 
 This code example is in the public domain. 
 http://www.botnroll.com

Line sensor calibrate
The calibrate routine is called in Setup()
Reads and stores the maximum and minimum value for every sensor on vectors SValMax[8] and SValMin[8].
Low values for white and high values for black.
The transition value from white to black (Vtrans) is defined by the user:
  Vtrans is the lowest value above white colour that can be considered black.
  By default is suggested the highest of the lower values.
  Vtrans should be as low as possible as long as it assures a safe transition from white to black.
Stores the values on EEPROM so they can be used in your programs after robot restart.

To calibrate place the robot over the line with the line at the centre of the sensor.
The robot rotates during 4 seconds acquiring the 8 sensor max and min values.

The registered values are displayed on the LCD. Use the push buttons to see more values.
Calibration ends after you define Vtrans value.
In order to adjust Vtrans, sensor reading values should be analysed in real time and at different places on the track.
 
<> 
 
Calibração do sensor de linha.
A rotina de calibração é chamada do Setup()
Analisa e regista os valores máximos e mínimos de cada sensor usando os vectores SValMax[8] e SValMin[8].
Valores baixos para cor branca e elevados para cor preta.
O valor de transição de branco para preto (Vtrans) é definido pelo utilizador:
  Vtrans é o valor mais baixo acima da cor branca que podemos considerar como sendo preto.
  Por defeito é sugerido o maior valor dos mínimos.
  Vtrans deverá ser o mais baixo possível mas que permita destinguir a transição de branco para preto com segurança.
Armazena os valores na EEPROM para uso futuro em outros programas.

Para calibrar deve-se colocar o robô em cima da linha com a linha no centro do sensor.
O robô roda durante 4 segundos adquirindo os valores maximos e mínimos dos 8 sensores.

Os valores registados são apresentados no LCD. Usar botões para apresentar mais valores.
A calibração só termina depois de definido o valor de transição Vtrans.  
Para ajustar Vtrans devem ser analisados os valores de leitura em tempo real e em diferentes posições da pista.
*/

#include <BnrOneA.h>   // Bot'n Roll ONE A library
#include <EEPROM.h>    // EEPROM reading and writing
#include <SPI.h>       // SPI communication library required by BnrOne.cpp
BnrOneA one;           // declaration of object variable to control the Bot'n Roll ONE A

//constants definitions
#define SSPIN  2    //Slave Select (SS) pin for SPI communication
#define M1  1       //Motor1
#define M2  2       //Motor2

float batmin=10.5;  // safety voltage for discharging the battery

#define VMAX 1000
int SValMax[8]={1023,1023,1023,1023,1023,1023,1023,1023};
int SValMin[8]={0,0,0,0,0,0,0,0};
double SFact[8];
int Vtrans=50;  //Line follower limit between white and black 

void setup() 
{  
  Serial.begin(57600);     // sets baud rate to 57600bps for printing values at serial monitor.
  one.spiConnect(SSPIN);   // starts the SPI communication module
  one.stop();              // stop motors
  one.minBat(batmin);      // safety voltage for discharging the battery
  delay(1000);
  calibrateLine();         // Calibrate line sensor <> Calibração do sensor de linha
  setupLine();
 }

void calibrateLine()
{
	one.lcd1(" Press a button ");
	one.lcd2("  to calibrate  ");
	while(one.readButton()==0)
		delay(50);
  Serial.println("Calibrate Starting!");
  one.lcd1("   Calibrate   ");
  one.lcd2("   starting!    ");
	delay(1000);

    static int SVal[8]={0,0,0,0,0,0,0,0};    
    static int SValMax[8]={0,0,0,0,0,0,0,0};
    static int SValMin[8]={1023,1023,1023,1023,1023,1023,1023,1023};
    int butt=0;

    while(one.readButton()!=0)
    {
      delay(50);
    }

 //Calibrates during 4 seconds <> Calibra durante 4 segundos
    one.move(25,-25);            
    unsigned long time=millis();
    while(millis()<time+4000)
    {
      Serial.println(); Serial.print("Val: ");
      for(int i=0;i<8;i++)
      {
          int SVal=one.readAdc(i);
          if(SVal>SValMax[i])
          {
            SValMax[i]=SVal;
          }
          if(SVal<SValMin[i])
          {
            SValMin[i]=SVal;
          }          
          Serial.print(SVal);Serial.print("  ");          
      }  
      Serial.println(); Serial.print("Max: ");
      
      for(int i=0;i<8;i++)
      {
          Serial.print(SValMax[i]);Serial.print("  ");          
      }  
      Serial.println(); Serial.print("Min: ");
      
      Vtrans=0;
      for(int i=0;i<8;i++)
      {
         Serial.print(SValMin[i]);Serial.print("  ");
         if(SValMin[i]>Vtrans)
           Vtrans=SValMin[i];
      }    
     Serial.println();   
      delay(50);
   }
   Serial.print("Vtrans:");Serial.println(Vtrans);
   one.stop();

   //Write values on EEPROM <> Escrever valores na EEPROM
   byte eepromADD=100;
   for(int i=0;i<8;i++)
   {
       EEPROM.write(eepromADD,highByte(SValMax[i]));
       eepromADD++;
       EEPROM.write(eepromADD,lowByte(SValMax[i]));
       eepromADD++;
   }
   for(int i=0;i<8;i++)
   {
       EEPROM.write(eepromADD,highByte(SValMin[i]));
       eepromADD++;
       EEPROM.write(eepromADD,lowByte(SValMin[i]));
       eepromADD++;
   }

   Serial.println("Calibrate Done! Press a button...");
   one.lcd1(" Calibrate done ");
   one.lcd2(" Press a button ");
   while(one.readButton()!=3)
   {
       one.lcd1("Max1  2   3   4 ");
       one.lcd2(SValMax[0],SValMax[1],SValMax[2],SValMax[3]);
       while(one.readButton()!=0)
       {
          delay(100);
       }       
       while(one.readButton()==0)
       {
          delay(100);
       }
       one.lcd1("Max5  6   7   8 ");
       one.lcd2(SValMax[4],SValMax[5],SValMax[6],SValMax[7]);
       while(one.readButton()!=0)
       {
          delay(100);
       }       
       while(one.readButton()==0)
       {
          delay(100);
       }
       one.lcd1("Min1  2   3   4 ");
       one.lcd2(SValMin[0],SValMin[1],SValMin[2],SValMin[3]);
       while(one.readButton()!=0)
       {
          delay(100);
       }       
       while(one.readButton()==0)
       {
          delay(100);
       }
       one.lcd1("Min5  6   7   8 ");
       one.lcd2(SValMin[4],SValMin[5],SValMin[6],SValMin[7]);
       while(one.readButton()!=0)
       {
          delay(100);
       }       
       while(one.readButton()==0)
       {
          delay(100);
       }         
       one.lcd1(" Test white to  ");
       one.lcd2(" adjust Vtrans: ");
       while(one.readButton()!=0)
       {
          delay(100);
       }       
       while(one.readButton()==0)
       {
          delay(100);
       }              
       while(one.readButton()!=0)
       {
          delay(100);
       }           
       while(one.readButton()==0)
       {
          for(int i=0;i<8;i++)
          {
            SVal[i]=one.readAdc(i);
          }
          one.lcd1(SVal[0]-SValMin[0],SVal[1]-SValMin[1],SVal[2]-SValMin[2],SVal[3]-SValMin[3]);
          one.lcd2(SVal[4]-SValMin[4],SVal[5]-SValMin[5],SVal[6]-SValMin[6],SVal[7]-SValMin[7]);
          delay(100);
       }
       one.lcd1("  PB1++  PB2-- ");
       one.lcd2("   Vtrans:", Vtrans);
       while(one.readButton()!=0)
       {
          delay(100);
       }   
       butt=0;    
       while(butt!=3)
       {
        butt=one.readButton();
        if(butt==1)
        {
          Vtrans+=10;
          one.lcd2("   Vtrans:", Vtrans);
          delay(100);
        }
        if(butt==2)
        {
          Vtrans-=10;
          one.lcd2("   Vtrans:", Vtrans);
          delay(100);
        }
       }
       EEPROM.write(eepromADD,highByte(Vtrans));
       eepromADD++;
       EEPROM.write(eepromADD,lowByte(Vtrans));
       eepromADD--;
       one.lcd1("PB1=AdjustVtrans");
       one.lcd2("PB3=end");
       while(one.readButton()!=0)
       {
          delay(100);
       }       
       while(one.readButton()==0)
       {
          delay(100);
       }
   }   
   one.lcd1("  Calibrating   ");
   one.lcd2("     Done!      ");
   while(one.readButton()==0)
   {
       delay(50);
   }   
}

void loop() 
{
  int line=readLine(); // Read line <> Ler a linha
  Serial.print(" Line:"); Serial.println(line); 
  one.lcd1("     Line:"); //Print values on the LCD <> Apresenta valores no LCD
  one.lcd2("     ",line); //Print values on the LCD <> Apresenta valores no LCD
  delay(50);
}

void setupLine()
{  
   //Read EEPROM values <> Ler valores da EEPROM
   byte eepromADD=100;
   Serial.println("Setup:"); Serial.print("Max: ");
   for(int i=0;i<8;i++)
   {
       SValMax[i]=(int)EEPROM.read(eepromADD);
       SValMax[i]=SValMax[i]<<8;
       eepromADD++;
       SValMax[i]+=(int)EEPROM.read(eepromADD);
       eepromADD++;
       Serial.print(SValMax[i]);Serial.print("  ");
   }
   Serial.println(); Serial.print("Min: ");
   for(int i=0;i<8;i++)
   {
       SValMin[i]=(int)EEPROM.read(eepromADD);
       SValMin[i]=SValMin[i]<<8;
       eepromADD++;
       SValMin[i]+=(int)EEPROM.read(eepromADD);
       eepromADD++;
       Serial.print(SValMin[i]);Serial.print("  ");
   }   
   Vtrans=(int)EEPROM.read(eepromADD);
   Vtrans=Vtrans<<8;
   eepromADD++;
   Vtrans+=(int)EEPROM.read(eepromADD);
   Serial.println(); Serial.print("Vtrans: "); Serial.print(Vtrans); Serial.println();
   
   for(int i=0;i<8;i++)
   {
      SFact[i]=(double)VMAX/(double)(SValMax[i]-SValMin[i]); //Calculate factor for each sensor <> Calcular fator de cada sensor
   }
}

int readLine()
{
      int Vrt1=SValMin[1]*2, Vrt2=SValMin[6]*2;
      int SValR[8];  
      int SValN[10]={Vrt1,0,0,0,0,0,0,0,0,Vrt2};    
      int idMax=-1, SMax=-1;
      int lineValue=-1;
      int flag=-1;
      static int prevLineValue=0;

	  
      //Read the 8 sensor values <> Leitura dos valores dos 8 sensores
      for(int i=0;i<8;i++)
      {
          SValR[i]=one.readAdc(i);
      }

      //Normalize values between 0 and 1000 <> Normalizar valores entre 0 e 1000
      for(int i=1;i<9;i++)
      {
          SValN[i]=(int)((double)((SValR[i-1]-SValMin[i-1]))*SFact[i-1]); //Registar o valor efetivo máximo de cada sensor
          if(SValN[i]>SMax)
            {
              SMax=SValN[i]; //Identify the sensor with the highest value <> Identificar o sensor com valor efectivo máximo
              idMax=i;       //Store the sensor index <> Registar o indice do sensor
            }
      }
      
      if(SMax>Vtrans && SValN[idMax-1]>=SValN[idMax+1]) //If previous is bigger than the next <> Se o anterior for maior que o seguinte
      {
          lineValue=VMAX*(idMax-1)+SValN[idMax];     
             flag=0;
      }
      else if(SMax>Vtrans && SValN[idMax-1]<SValN[idMax+1]) //If previous is smaller than the next <> Se o anterior for menor que o seguinte
      {
          if(idMax!=8) //If not the last sensor <> Se não é o último sensor
          {
             lineValue=VMAX*idMax+SValN[idMax+1];
             flag=1;
          }
          else //If this is the last sensor <> Se é o último sensor
          {
             lineValue=VMAX*idMax+VMAX-SValN[idMax];
             flag=2;
          }
      }
      if(lineValue==-1)//Lost the line -> all white <> Saiu da linha -> tudo branco
      {
        if(prevLineValue>4500)
        {
          lineValue=9000;
        }
        else
        {
          lineValue=0;
        }
      }      
      else if(lineValue<-1 || lineValue>9000) //Possible reading errors <> Possiveis erros de leitura
      {
        lineValue=prevLineValue;
      }
      else //If normal values <> Se valores normais
      {
        prevLineValue=lineValue;
      }
  //    return lineValue;  //Values from 0 to 9000 <> Valores de 0 a 9000
      return (int)((double)(lineValue+1)*0.022222)-100;  //Values from -100 to 100 <> Valores de -100 a 100
}

