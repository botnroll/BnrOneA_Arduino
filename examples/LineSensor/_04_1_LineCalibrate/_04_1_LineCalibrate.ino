/* 
 This example was created by José Cruz on October 2016 
 
 This code example is in the public domain. 
 http://www.botnroll.com

Calibração do sensor de linha.
A rotina de calibração é chamada do Setup()
Analisa e regista os valores máximos e mínimos de cada sensor usando os vectores SValMax[8] e SValMin[8].
Valores baixos para cor branca e elevados para cor preta.
O valor de transição de branco para preto (Vtrans) é definido pelo utilizador:
  Vtans deve ser visto como: o valor mais baixo acima da cor branca que podemos considerar como sendo preto.
  Por defeito é sugerido o maior valor dos mínimos.
  Vtrans deverá ser o mais baixo possível mas que permita destinguir a transição de branco para preto com segurança.
Armazena os valores na EEPROM para uso futuro em outros programas.

A calibração pode ser efetuada com o robô parado ou em movimento:
  Com o robô parado calibra até se carregar num botão.
  Em movimento calibra com o robô a rodar durante 4 segundos. Colocar o robo no chão com a linha ao centro.
Comentários no código permitem a selecção do modo de calibração.

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
  calibrateLine();         // Calibração do sensor de linha
  setupLine();
  delay(1000);
}

void calibrateLine()
{
    static int SVal[8]={0,0,0,0,0,0,0,0};    
    static int SValMax[8]={0,0,0,0,0,0,0,0};
    static int SValMin[8]={1023,1023,1023,1023,1023,1023,1023,1023};
    int butt=0;
    Serial.println("Calibrating Starting!");
    one.lcd1("  Calibrating   ");
    one.lcd2("   starting!    ");
    while(one.readButton()!=0)
    {
      delay(50);
    }
    Serial.println("Press any button to end calibrate!");
    one.lcd1("Press any button");
    one.lcd2("to end calibrate");
 
//Calibração com o robô parado ou em movimento    
    one.move(25,-25);            //Em movimento calibra durante 4 segundos
    unsigned long time=millis();
    while(millis()<time+4000)
//  while(one.readButton()==0)     //Com o robô parado calibra até se carregar num botão
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

   //Escrever valores na EEPROM
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
       one.lcd1("  Verify white ");
       one.lcd2("   for Vtrans: ");
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
       one.lcd1("  Define Value ");
       one.lcd2("  Vtrans:", Vtrans);
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
          one.lcd2("  Vtrans:", Vtrans);
          delay(100);
        }
        if(butt==2)
        {
          Vtrans-=10;
          one.lcd2("  Vtrans:", Vtrans);
          delay(100);
        }
       }
       EEPROM.write(eepromADD,highByte(Vtrans));
       eepromADD++;
       EEPROM.write(eepromADD,lowByte(Vtrans));
       eepromADD--;
       one.lcd1(" Calibrate done ");
       one.lcd2("PB1=see PB3=exit");
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
  int line=readLine(); //Ler a linha
  Serial.print(" Line:"); Serial.println(line); 
  one.lcd1("     Line:"); //Apresenta valores no LCD
  one.lcd2("     ",line); //Apresenta valores no LCD
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
              idMax=i;       //Registar o indice do sensor
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
  //    return lineValue;  // Valores de 0 a 9000
      return (int)((double)(lineValue+1)*0.022222)-100;  // Valores de -100 a 100
}

