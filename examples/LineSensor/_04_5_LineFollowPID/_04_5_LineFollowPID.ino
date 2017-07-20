/* 
 This example was created by José Cruz on October 2016 
 
 This code example is in the public domain. 
 http://www.botnroll.com

IMPORTANTE!!! Necessária calibração prévia dos sensores. _04_1_Calibrate.ino para a 
leitura da linha devolver corretamente valores entre -100 e +100 da posição da linha.
 
Seguimento de linha:
Os motores variam com a linha com controlo PID
Botão 3 (PB3) entra no menu de configuração do controlo.

https://www.desmos.com/calculator

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

#define LIMITES 100.0
double kp=1.3, ki=0.0013, kd=0.35;// Ganho proporcional
int vel=60;
int vCurve=3;

void setup() 
{  
  Serial.begin(57600);     // sets baud rate to 57600bps for printing values at serial monitor.
  one.spiConnect(SSPIN);   // starts the SPI communication module 
  one.minBat(batmin);      // safety voltage for discharging the battery
  one.stop();              // stop motors
  delay(500);
}

void loop() 
{
  int line=0;   // Valor da linha no sensor a cada instante
  int lineRef=0;// Valor de referência da linha para o controlo ( queremos que seja 0)
  int erroP=0;  // Erro proporcional
  static double erroI=0.0;
  static int erroD=0.0;
  static int erroPAnt=0;
  double output=0.0;
  
  int velM1=0, velM2=0;
  
  line=one.readLine(); // O valor da linha varia de -100 a +100
//  Serial.print(" Line:");Serial.print(line); 

  erroP=lineRef-line;
  erroD=erroP-erroPAnt;
  output = kp*(double)erroP + ki*erroI + kd*(double)erroD;
  //Limpar o erro integral se a linha passou para o outro lado ou se está no centro
  if(erroP*erroPAnt<=0)  erroI=0.0; //Limpar o erro integral
  if (output>LIMITES) output=LIMITES;      //Instrução para so calibrar movimento até a linha
  else if (output<-LIMITES) output=-LIMITES;  // mesma coisa do outro lado 
  else erroI+=(double)erroP;  // Se a saída está dentro dos limites incrementa o erro integral
  erroPAnt=erroP;
//  Serial.print("  Out:"); Serial.print(output);

//Limitar mínimos e máximos da velocidade dos motores
  velM1=vel-(int)output;
  velM2=vel+(int)output;
  if(velM1<-1)
      velM1=-1;
  if(velM2<-1)
      velM2=-1;
  if(velM1>vel+vCurve)
      velM1=vel+vCurve;
  if(velM2>vel+vCurve)
      velM2=vel+vCurve;

//  Serial.print("   M1:");  Serial.print(velM1); Serial.print("   M2:"); Serial.println(velM2);  
  one.move(velM1,velM2);
  
  //Menus de configuração
  if(one.readButton()==3) menu();// Entra no menu
}

void menu()
{
    int var=0;
    int butt=0;
    double temp=0.0;
    one.stop();
    one.lcd1("  Menu Config:");
    one.lcd2("PB1+ PB2- PB3ok");    
    while(one.readButton()!=0) //Espera que se largue o botão 3
    {
      delay(150);
    }
    
    //*************** Velociade Maxima ***************
    var=vel;
    while(butt!=3)
    {
        one.lcd2("    VelMax:",var);
        butt=one.readButton();
        if(butt==1)
        {
          var++;
          delay(150);
        }
        if(butt==2)
        {
          var--;
          delay(150);
        }
    }
    while(butt!=0) //Espera que se largue o botão 3
    {butt=one.readButton();}
    vel=var;        

    //*************** Incremento da roda que acelera ***************
    var=vCurve;
    while(butt!=3)
    {
        one.lcd2("Curve Boost:",var);
        butt=one.readButton();
        if(butt==1)
        {
          var++;
          delay(150);
        }
        if(butt==2)
        {
          var--;
          delay(150);
        }
    }
    while(butt!=0) //Espera que se largue o botão 3
    {butt=one.readButton();}
    vCurve=var;  

     //*************** Ganho KP ***************
    temp=kp*1000;
    var=(int)temp;
    while(butt!=3)
    {
        one.lcd2(" kp:",var);
        butt=one.readButton();
        if(butt==1)
        {
          var+=10;
          delay(150);
        }
        if(butt==2)
        {
          var-=10;
          delay(150);
        }
    }
    while(butt!=0) //Espera que se largue o botão 3
    {butt=one.readButton();}
    kp=(double)var/1000;  
    
     //*************** Ganho KI ***************
    temp=ki*10000.0;
    var=(int)temp;
    while(butt!=3)
    {
        one.lcd2(" ki:",var);
        butt=one.readButton();
        if(butt==1)
        {
          var+=1;
          delay(150);
        }
        if(butt==2)
        {
          var-=1;
          delay(150);
        }
    }
    while(butt!=0) //Espera que se largue o botão 3
    {butt=one.readButton();}
    ki=(double)var/10000.0;  
    
     //*************** Ganho KP ***************
    temp=kd*1000;
    var=(int)temp;
    while(butt!=3)
    {
        one.lcd2(" kd:",var);
        butt=one.readButton();
        if(butt==1)
        {
          var+=10;
          delay(150);
        }
        if(butt==2)
        {
          var-=10;
          delay(150);
        }
    }
    while(butt!=0) //Espera que se largue o botão 3
    {butt=one.readButton();}
    kd=(double)var/1000;  
                
    //*************** Termina Configuração *************** 
    one.lcd1("Line  Following!");
    one.lcd2("www.botnroll.com");
    delay(250);
}
