/* 
 This example was created by José Cruz (www.botnroll.com) on October 2016 
 
 This code example is in the public domain. 
 http://www.botnroll.com

Leitura da linha que devolve um valor linear entre -100 e +100 da posição da linha.
Necessária calibração prévia dos sensores. _04_1_Calibrate.ino

A linha no sensor varia de 0 a 9000.
Efetua a leitura dos sensores e registar o sensor com o valor máximo.
O sensor vizinho com maior valor define a posição da linha entre os dois sensores: Maximo e vizinho com maior valor.

*/
#include <BnrOneA.h>   // Bot'n Roll ONE A library
#include <EEPROM.h>    // EEPROM reading and writing
#include <SPI.h>       // SPI communication library required by BnrOne.cpp
BnrOneA one;           // declaration of object variable to control the Bot'n Roll ONE A

//constants definitions
#define SSPIN  2    //Slave Select (SS) pin for SPI communication
#define M1  1       //Motor1
#define M2  2       //Motor2

#define VMAX 1000
int SValMax[8]={1023,1023,1023,1023,1023,1023,1023,1023};
int SValMin[8]={0,0,0,0,0,0,0,0};
double SFact[8];
int Vtrans=50;  //


float batmin=10.5;  // safety voltage for discharging the battery


void setup() 
{  
      Serial.begin(57600);     // sets baud rate to 57600bps for printing values at serial monitor.
      one.spiConnect(SSPIN);   // starts the SPI communication module
      one.stop();              // stop motors
      one.minBat(batmin);      // safety voltage for discharging the battery
      setupLine();
}


void loop() 
{
      int line=readLine();
      Serial.print("Line: ");Serial.println(line);  
      one.lcd2(" Line: ",line);
      delay(50);   
}

void setupLine()
{  
   //Ler valores da EEPROM
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
      SFact[i]=(double)VMAX/(double)(SValMax[i]-SValMin[i]); //Calcular fator de cada sensor
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

      //Leitura dos valores dos 8 sensores
      for(int i=0;i<8;i++)
      {
          SValR[i]=one.readAdc(i);
      }

      //Normalizar valores entre 0 e 1000
      for(int i=1;i<9;i++)
      {
          SValN[i]=(int)((double)((SValR[i-1]-SValMin[i-1]))*SFact[i-1]); //Registar o valor efetivo máximo de cada sensor
          if(SValN[i]>SMax)
            {
              SMax=SValN[i]; //Identificar o sensor com valor efectivo máximo
              idMax=i;      //Registar o indice do sensor
            }
      }
      
      if(SMax>Vtrans && SValN[idMax-1]>=SValN[idMax+1]) //Se o anterior for maior que o seguinte
      {
          lineValue=VMAX*(idMax-1)+SValN[idMax];     
             flag=0;
      }
      else if(SMax>Vtrans && SValN[idMax-1]<SValN[idMax+1]) //Se o anterior for menor que o seguinte
      {
          if(idMax!=8) // Se não é o último sensor
          {
             lineValue=VMAX*idMax+SValN[idMax+1];
             flag=1;
          }
          else //Se é o último sensor
          {
             lineValue=VMAX*idMax+VMAX-SValN[idMax];
             flag=2;
          }
      }
/*      
      for(int i=0;i<10;i++)
      {
          Serial.print(SValN[i]);Serial.print("  ");
      }
      Serial.print("idx:"); Serial.print(idMax);
      if(flag==0)
          Serial.print(" A>s");
      else if(flag==1)
          Serial.print(" a<S");
      else if(flag==2)
          Serial.print(" |<<");
      else      
          Serial.print(" #");
      Serial.print("  ");
*/
      if(lineValue==-1)//saíu da linha -> tudo branco
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
      else if(lineValue<-1 || lineValue>9000) //Possiveis erros de leitura
      {
        lineValue=prevLineValue;
      }
      else //se valores normais
      {
        prevLineValue=lineValue;
      }
//      return lineValue;  // Valores de 0 a 9000
      return (int)((double)(lineValue+1)*0.022222)-100;  // Valores de -100 a 100
}

