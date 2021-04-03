/*
  RECEITA

   10 - 13   varMin |Dez Bytes|
   14 - 17   TempFerv |Dez Bytes|

   100 - 101  Index  1   |Int Dois Bytes|
   102 - 103  Index  2   |Int Dois Bytes|
   104 - 105  Index  3   |Int Dois Bytes|
   106 - 107  Index  4   |Int Dois Bytes|
   108 - 109  Index  5   |Int Dois Bytes|
   110 - 111  Index  6   |Int Dois Bytes|
   112 - 113  Index  7   |Int Dois Bytes|
   114 - 115  Index  8   |Int Dois Bytes|
   116 - 117  Index  9   |Int Dois Bytes|
   118 - 119  Index 10   |Int Dois Bytes|

   120 - 130  Nome  1   |Char Dez Bytes|
   131 - 141  Nome  2   |Char Dez Bytes|
   142 - 152  Nome  3   |Char Dez Bytes|
   153 - 163  Nome  4   |Char Dez Bytes|
   164 - 174  Nome  5   |Char Dez Bytes|
   175 - 185  Nome  6   |Char Dez Bytes|
   186 - 196  Nome  7   |Char Dez Bytes|
   197 - 207  Nome  8   |Char Dez Bytes|
   208 - 218  Nome  9   |Char Dez Bytes|
   219 - 229  Nome  10  |Char Dez Bytes|

   720 - 849    Recipe Info  1    |Struct Receita|
   850 - 979  Recipe Info  2    |Struct Receita|
   980 - 1109   Recipe Info  3    |Struct Receita|
   1110 - 1239  Recipe Info  4    |Struct Receita|
   1240 - 1369  Recipe Info  5    |Struct Receita|
   1370 - 1499  Recipe Info  6    |Struct Receita|
   1500 - 1629  Recipe Info  7    |Struct Receita|
   1630 - 1759  Recipe Info  8    |Struct Receita|
   1760 - 1889  Recipe Info  9    |Struct Receita|
   1890 - 2019  Recipe Info 10    |Struct Receita|
*/

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Wire.h> 
#include <LiquidCrystal.h>
#include <EEPROM.h>
#include <Arduino.h>  // for type definitions

struct EtapaQuente{
  float tempMin = 0;
  float tempMax = 0;
  int duracao = 0;
};

struct Lupulos{
  int duracao = 255;
};

struct Receita {

  EtapaQuente mostura[10];
  EtapaQuente fervura[1];
  
  Lupulos lupulo[10];
};

struct Receita receita[10];
typedef void (*Funcao) (EtapaQuente* etapa);

#define BUZZER 27

//Porta do pino do rele bomba
#define RELE_BOMBA 29

//Porta do pino do rele resistencia
#define RELE_RESISTENCIA 31

//Porta do pino do rele valvula
#define RELE_VALVULA 33

// Porta do pino de sinal do DS18B20
#define ONE_WIRE_BUS 35


//Botoes
#define BTN_CONFIRMA 14
#define BTN_SOBE 15
#define BTN_DESCE 16
#define BTN_CANCELA 17

// Define uma instancia do oneWire para comunicacao com o sensor
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress sensorTemperature;


//lcd
LiquidCrystal lcd(2,3,4,5,6,7);

//Variaveis Millis
unsigned long _millisAnterior = 0;
unsigned long _millisAnteriorValvula = 0;
unsigned long _intervaloBuzzer = 1000;
unsigned int _tocarBuzzer = 1;

float _variacaoMinima;
boolean _alarmeAtivo= false;
boolean _alarmeAtivoHop= false;
boolean _statusResistencia = true;
boolean _targetTemperatura = false;
boolean _lupuloFlameOut = false;
boolean _mostura = false;
boolean _fervura = false;
boolean _brassagemFinalizada = false;
int _lupuloVez = 0;
int _step = 0;
unsigned long _millisHorarioAlarm;
unsigned long _millisHorarioAlarmLupulo;
unsigned long _millisHorarioInicioFervura;
 
void _()
{ 
}

#include "receita.h"
#include "metodosSensorTemp.h"
#include "metodosBuzzer.h"
#include "metodosEEPROM.h"
#include "metodosLcd.h"

void setup() { 

  Serial.begin(9600);

  lcd.begin(16, 2);
  
  EEPROM_readAnything(10, _variacaoMinima);

  iniciaSensorTemp();
 
  pinMode(BUZZER,OUTPUT);
  pinMode(RELE_RESISTENCIA,OUTPUT);
  pinMode(RELE_BOMBA,OUTPUT);
  digitalWrite(RELE_RESISTENCIA, LOW);
  digitalWrite(RELE_BOMBA, LOW);

  pinMode(BTN_CONFIRMA,INPUT_PULLUP);
  pinMode(BTN_CANCELA,INPUT_PULLUP);
  pinMode(BTN_SOBE,INPUT_PULLUP);
  pinMode(BTN_DESCE,INPUT_PULLUP);

  lcd.createChar(0, bell);
  lcd.createChar(1, note);
  lcd.createChar(2, clock);
  lcd.createChar(3, heart);
  lcd.createChar(4, termometro);
  lcd.createChar(5, graus);
  lcd.createChar(6, check);
  lcd.createChar(7, cross);
  lcd.createChar(8, target);
  lcd.home();
}

void loop() {

  mexerMenu();
    
}

void brassagem(){
  if(_mostura){
    while(_step <= 10){
          etapaMostura(receita[0].mostura,sizeof(receita[0].mostura)/sizeof(EtapaQuente)); 
      }

    desligaBombaRecirculacao();   
    lcd.clear();
    _millisHorarioAlarm =0;
    while(!digitalRead(BTN_CONFIRMA)){
        lcd.setCursor(0,0);
        lcd.print("Fim da Mostura");
        lcd.setCursor(0,1);
        lcd.print("Retire os Maltes");
    
        tocarBuzzer();
      }

      desligaBuzzer();
      _mostura = false;
      _fervura = true;
      _step = 0;
  }

  lcd.clear();
  if(_fervura){

    while(_step <= 1){
       etapaFervura(receita[0].fervura,receita[0].lupulo,sizeof(receita[0].lupulo)/sizeof(Lupulos)); 
    }

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Fervura Finalizada");
    delay(1000);
  }

  digitalWrite(RELE_RESISTENCIA, LOW); //Desliga rele
  if(_lupuloFlameOut){
    adicionarLupuloFervura(_lupuloVez);
    _lupuloFlameOut = false;
    delay(2000);
  }

  
  lcd.setCursor(0,0);
  lcd.clear();
  
  while(!digitalRead(BTN_CONFIRMA)){
    lcd.setCursor(0,0);
    lcd.print("BRASSAGEM FINALIZADA");
    lcd.setCursor(0,1);
    lcd.print("Press Confirmar");
    
    tocarBuzzer();
  }
  
  desligaBuzzer();

   _variacaoMinima;
  _alarmeAtivo= false;
  _alarmeAtivoHop= false;
  _statusResistencia = true;
  _targetTemperatura = false;
  _lupuloFlameOut = false;
  _mostura = false;
  _fervura = false;
  _lupuloVez = 0;
  _step = 0;
  
}

void etapaFervura(EtapaQuente etapa[],Lupulos lupulo[], int tam){
    
    switch(_step){

     case 0:  
      preAquecimento(aquecimentoPreFervura, etapa);
     break;
    
     case 1:
       int stepFervura = _step-1;
       setBuzzerTimer(etapa[stepFervura].duracao);
          
       while(!verificaAlarm()&& !(digitalRead(BTN_CONFIRMA) && digitalRead(BTN_CANCELA))){
        lcd.setCursor(0,0);
        lcd.print("Fervura");

        if(_alarmeAtivo){
             imprimiTempoRestante();
         }
         
        mostrarTemperatura();
        mostrarTempAlvo(etapa[stepFervura].tempMax);
        controlResistenceFervura(getTemperature(),etapa[stepFervura].tempMin,etapa[stepFervura].tempMax);

          if(_alarmeAtivo){
            if(!_alarmeAtivoHop){
              if(lupulo[_lupuloVez].duracao != 255){
                setBuzzerTimerHop(etapa[stepFervura].duracao, lupulo[_lupuloVez].duracao); 
              }  
            }            
          }
       }
       _step++;
      
    break;
    
    }
}

void etapaMostura(EtapaQuente etapa[],int tam){

  switch(_step){
 
    case 0:  //Pre-Aquecimento
      preAquecimento(aquecimentoPreMostura, etapa);
    break;

    case 11: break;
    
    default:
    etapaMostura(etapa);
    break;
    
  }
  
}

void controlResistence(float tempSensor, float tempMin, float tempMax, int duracao ){
  if(tempSensor < tempMax && _statusResistencia){
    digitalWrite(RELE_RESISTENCIA, HIGH); //Liga rele
  }
  else{
    _statusResistencia = false;
      if(tempSensor < tempMin && !_statusResistencia){
        digitalWrite(RELE_RESISTENCIA, HIGH); //Liga rele
        _statusResistencia = true;
      }
      else{
          digitalWrite(RELE_RESISTENCIA, LOW); //Desliga rele
      }
  }
}

void controlResistenceTempInicial(float tempSensor, float tempMin, float tempMax){
  if(tempSensor < tempMax && _statusResistencia){
    digitalWrite(RELE_RESISTENCIA, HIGH); //Liga rele
  }
  else{
    _statusResistencia = false;
      if(tempSensor < tempMin && !_statusResistencia){
        digitalWrite(RELE_RESISTENCIA, HIGH); //Liga rele
        _statusResistencia = true;
      }
      else{
          digitalWrite(RELE_RESISTENCIA, LOW); //Desliga rele
        
          adicionarMaltesMostura();

          _targetTemperatura =  true;

      }
  }
}

void controlResistenceFervura(float tempSensor, float tempMin, float tempMax){
  if(tempSensor < tempMax && _statusResistencia){
    digitalWrite(RELE_RESISTENCIA, HIGH); //Liga rele
  }
  else{
    _statusResistencia = false;
      if(tempSensor < tempMin && !_statusResistencia){
        digitalWrite(RELE_RESISTENCIA, HIGH); //Liga rele
        _statusResistencia = true;
      }
      else{
          digitalWrite(RELE_RESISTENCIA, HIGH); //Liga rele
      }
  }
}

void mash(EtapaQuente etapa[],int i){

         if(_alarmeAtivo){
             imprimiTempoRestante();
         }
         
        lcd.setCursor(10,1);
        mostrarTemperatura();
        lcd.setCursor(10,0);
        mostrarTempAlvo(etapa[_step-1].tempMax);
         
        controlResistence(getTemperature(),etapa[i-1].tempMin,etapa[i-1].tempMax, etapa[i-1].duracao);   
}

void adicionarMaltesMostura(){
     desligaBombaRecirculacao(); 
     lcd.clear();
     while(!digitalRead(BTN_CONFIRMA)){
      lcd.setCursor(0,0);
      lcd.print("Adicione os Maltes");
      lcd.setCursor(0,1);
      lcd.print("Press Confirmar");
      Serial.println("Adicione os Maltes");

      tocarBuzzer();
     }
     
     desligaBuzzer();
}

void adicionarLupuloFervura(int posicao){

    lcd.clear();
    while(!digitalRead(BTN_CONFIRMA)){
        
        lcd.setCursor(0,0);
        lcd.print("ADD LUPULO ");
        lcd.print(posicao + 1);
        lcd.setCursor(0,1);
        lcd.print("Press Confirmar");
        Serial.print("ADICIONE LUPULO MINUTO->");
        Serial.println(posicao + 1);

       tocarBuzzer();

      }
      desligaBuzzer();
      _lupuloVez++;
      lcd.clear();
}

void ligaBombaRecirculacao(){
  digitalWrite(RELE_BOMBA, LOW); //Liga rele
}


void desligaBombaRecirculacao(){
  digitalWrite(RELE_BOMBA, HIGH); //Desiga rele
}

void preAquecimento(Funcao funcao, EtapaQuente etapa[]) {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Aquecer:");
    mostrarTempAlvo(etapa[_step].tempMax);

     while(!_targetTemperatura && !(digitalRead(BTN_CONFIRMA) && digitalRead(BTN_CANCELA)) ){
        funcao(etapa);
     }

     lcd.clear();
     desligaBombaRecirculacao();
     _targetTemperatura = false;
     _step++;
     delay(2000);
}

void aquecimentoPreFervura(EtapaQuente etapa[]) {
      mostrarTemperatura();
      controlResistenceTempInicial(getTemperature(),etapa[_step].tempMin,etapa[_step].tempMax);
}

void aquecimentoPreMostura(EtapaQuente etapa[]) {
      ligaBombaRecirculacao();
      mostrarTemperatura();
      controlResistenceTempInicial(getTemperature(),etapa[_step].tempMin,etapa[_step].tempMax);
}


void etapaMostura(EtapaQuente etapa[]){  
   if(etapa[_step-1].duracao == 0) {
      _step++; 
      return;
    } else {
      setBuzzerTimer(etapa[_step-1].duracao);
    }
    imprimirEtapa();
         
    while(!verificaAlarm() && !(digitalRead(BTN_CONFIRMA) && digitalRead(BTN_CANCELA))){
        ligaBombaRecirculacao();
        mash(etapa,_step);
    }
    setBuzzerTimer(etapa[_step-1].duracao); 
    lcd.clear();
    desligaBombaRecirculacao();
    _step++;
    _alarmeAtivo= false;
    delay(2000);
}
