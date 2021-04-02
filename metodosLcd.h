String _menuItems[] = {"Iniciar Brassagem","Criar Receita","Deletar Receita","Config Variacao","Config Ferv Temp","Modo Manual"};
String _menuMostura[] = {"Mostura 1", "Mostura 2", "Mostura 3",
                         "Mostura 4", "Mostura 5", "Mostura 6",
                         "Mostura 7", "Mostura 8", "Mostura 9","Mostura 10"};
String _menuFervura[] = {"Fervura","Lupulo 1", "Lupulo 2", "Lupulo 3",
                         "Lupulo 4", "Lupulo 5", "Lupulo 6",
                         "Lupulo 7", "Lupulo 8", "Lupulo 9","Lupulo 10"};                         
//======define charset
 uint8_t bell[8] = {0x4,0xe,0xe,0xe,0x1f,0x0,0x4};
 uint8_t note[8] = {0x2,0x3,0x2,0xe,0x1e,0xc,0x0};
 uint8_t clock[8] = {0x0,0xe,0x15,0x17,0x11,0xe,0x0};
 uint8_t heart[8] = {0x0,0xa,0x1f,0x1f,0xe,0x4,0x0};
 uint8_t termometro[8] = {0x01,0x0E,0x0A,0x0E,0x0E,0x1F,0x1F,0x1F};
 uint8_t graus[8] = {0x00,0x00,0x18,0x18,0x07,0x04,0x04,0x07};
 uint8_t check[8] = {0x0,0x1,0x3,0x16,0x1c,0x8,0x0};
 uint8_t cross[8] = {0x0,0x1b,0xe,0x4,0xe,0x1b,0x0};
 uint8_t target[8] = {0x00,0x0E,0x15,0x1B,0x15,0x0E,0x00,0x00};                  

char alfabeto[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','X','Y','Z',' ','0','1','2','3','4','5','6','7','8','9'};
char nomeReceita[10];
int _menuAtual=0;
bool _menuAtualMostura=false;
bool _menuAtualFervura=false;
bool _menuAtualNome=false;
bool _menuConfigVarMin=false;
bool _menuConfigTempFerv=false;

int letra=0;
int letraGravar=0;
int linha=0;
int coluna=0;
bool escrevendo = false;

long previousMillisLcd = 0;
long intervalLcd = 1000;
unsigned long currentMillisLcd;

String nomeReceitaLer;


void addLetra(){
  
  letra++;
    if(letra >= (sizeof(alfabeto)-1)){
    letra =0;
   }
}   

void removeLetra(){
  
  letra--;
    if(letra < 0){
    letra = (sizeof(alfabeto)-1);
   }
}

void apagarLetra(){
  
  removeLetra();
  lcd.setCursor(coluna,linha);
  lcd.print(" ");
  

  coluna--;
  if(coluna < 0){
    coluna = 0;
   }
 
  letra = 0;
  nomeReceita[letraGravar] =  ' ';
  letraGravar--;
  if(coluna < 0){
    letraGravar = 0;
   }
  lcd.setCursor(coluna,linha);
  lcd.print(alfabeto[letra]);

   escrevendo = false;
  
}

void addNomeReceita() {  
  delay(150);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Escolha um Nome");
  delay(2000);
  lcd.clear();
  
  while(_menuAtualNome){
         
       if(digitalRead(BTN_SOBE) && letraGravar<10){
         lcd.setCursor(coluna,linha);
         lcd.blink();
         addLetra();
         lcd.print(alfabeto[letra]);
         nomeReceita[letraGravar] = alfabeto[letra]; 
        
         escrevendo = true;
         previousMillisLcd = millis();
    
       }
      else if(digitalRead(BTN_DESCE)&& letraGravar<10){
         lcd.setCursor(coluna,linha);
         lcd.blink();
         removeLetra();
         lcd.print(alfabeto[letra]);
         nomeReceita[letraGravar] = alfabeto[letra];
        escrevendo = true;
        previousMillisLcd = millis();
    
      }
      else{
        lcd.blink();
        if(letraGravar<10){
        lcd.blink();
        lcd.setCursor(coluna,linha);
        lcd.print(alfabeto[letra]);
        }
        
      }

      if(digitalRead(BTN_CONFIRMA) && digitalRead(BTN_CANCELA)){
          _menuAtualNome = false;
          return NULL;
      }
        
      if(digitalRead(BTN_CONFIRMA)){
        if(nomeReceita[0]=='\0') {
            strcpy(nomeReceita, "NoName");
        }
        
        lcd.clear();
        gravaReceitaEEPROM(verificaPosicaoMemoriaVazia(), nomeReceita, receita[0]);
        letra=0;
        letraGravar=0;
        linha=0;
        coluna=0;
        escrevendo = false;
       _menuAtualNome = false;
        
      }
      
      if(digitalRead(BTN_CANCELA)){
        lcd.blink();
        apagarLetra();
        
      }
      
       currentMillisLcd = millis();
      //Lógica de verificação do tempo
      if ((currentMillisLcd - previousMillisLcd > intervalLcd) && escrevendo) { 
          escrevendo = false;
          letraGravar++;
        
          coluna++;
          letra =0;
        
      }
      delay(150);
  }

 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("RECEITA SALVA");
 delay(2000);
 
}


void menuMostura(){
  delay(150);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Configurar Mash");
  delay(2000);
  
  int posicaoMostura = 1;
  bool timeTemp = true;
  float tempo = 0;
  float tempratura = 0;
  
 while(_menuAtualMostura && posicaoMostura <= 10){
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(_menuMostura[posicaoMostura-1]);
    lcd.setCursor(0, 1);
    lcd.write(byte(2));
    lcd.setCursor(6, 1);
    lcd.write(byte(4));

    if(timeTemp){
       lcd.blink();
       lcd.setCursor(8, 1);
       lcd.print( tempratura);
       lcd.write(byte(5));
       lcd.setCursor(2, 1);
       lcd.print((int) tempo);

        if(digitalRead(BTN_SOBE)){
           tempo++;
             if(tempo > 999){
              tempo = 0;
             }
      
        }
        else if(digitalRead(BTN_DESCE)){
           tempo--;
             if(tempo < 0){
              tempo = 999;
             }
        }

        else if(digitalRead(BTN_CONFIRMA) && digitalRead(BTN_CANCELA)){
          timeTemp = false ;
          _menuAtualMostura = false;
          return NULL;
        }

        else if(digitalRead(BTN_CONFIRMA)){
          timeTemp = false ;
        }
        
        if(digitalRead(BTN_CANCELA)){
          timeTemp = true ;
          
        }

    }
    else{
       lcd.blink();
       lcd.setCursor(2, 1);
       lcd.print((int) tempo);
       lcd.setCursor(8, 1);
       lcd.print( tempratura);
       lcd.write(byte(5));

        if(digitalRead(BTN_SOBE)){
           tempratura= tempratura+0.5;
             if(tempratura > 120){
              tempratura = 0;
             }
      
        }
        else if(digitalRead(BTN_DESCE)){
           tempratura= tempratura-0.5;
             if(tempratura < 0){
              tempratura = 120;
             }
        }

        else if(digitalRead(BTN_CONFIRMA) && digitalRead(BTN_CANCELA)){
          timeTemp = false ;
          _menuAtualMostura = false;
          return NULL;
        }
        
        else if(digitalRead(BTN_CONFIRMA)){
          EEPROM_readAnything(10, _variacaoMinima);
          addEtapaMostura(receita[0].mostura,posicaoMostura,tempratura,(int) tempo,_variacaoMinima );
          posicaoMostura++;
           timeTemp = true ;
           tempo = 0;
           tempratura = 0;
        }
        
        if(digitalRead(BTN_CANCELA)){
          timeTemp = true ;
          
        }
    }
   
    delay(150);
 }

 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Mash Configurado");
 delay(2000);

}

void menuFervura(){
  delay(150);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Config Fervura");
  delay(2000);
  
  int posicaoFervura = 1;
  bool timeTemp = true;
  int tempo = 0;
  int tempoLupulo = 255;
  float tempratura;
  EEPROM_readAnything(14, tempratura);
  
 while(_menuAtualFervura && posicaoFervura < 2){
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(_menuFervura[posicaoFervura-1]);
    lcd.setCursor(0, 1);
    lcd.write(byte(2));
    lcd.setCursor(6, 1);
    lcd.write(byte(4));

    if(timeTemp){
       lcd.blink();
       lcd.setCursor(8, 1);
       lcd.print( tempratura);
       lcd.write(byte(5));
       lcd.setCursor(2, 1);
       lcd.print((int) tempo);

        if(digitalRead(BTN_SOBE)){
           tempo++;
             if(tempo > 999){
              tempo = 0;
             }
      
        }
        else if(digitalRead(BTN_DESCE)){
           tempo--;
             if(tempo < 0){
              tempo = 999;
             }
        }
        
        else if(digitalRead(BTN_CONFIRMA) && digitalRead(BTN_CANCELA)){
          timeTemp = false ;
          _menuAtualFervura = false;
          return NULL;
        }
        
        else if(digitalRead(BTN_CONFIRMA)){
          timeTemp = false ;
        }
        
        if(digitalRead(BTN_CANCELA)){
          timeTemp = true ;
          
        }

    }
    else{
       lcd.blink();
       lcd.setCursor(2, 1);
       lcd.print((int) tempo);
       lcd.setCursor(8, 1);
       lcd.print( tempratura);
       lcd.write(byte(5));

        if(digitalRead(BTN_SOBE)){
           tempratura= tempratura+0.5;
             if(tempratura > 120){
              tempratura = 0;
             }
      
        }
        else if(digitalRead(BTN_DESCE)){
           tempratura= tempratura-0.5;
             if(tempratura < 0){
              tempratura = 120;
             }
        }
        
        else if(digitalRead(BTN_CONFIRMA) && digitalRead(BTN_CANCELA)){
          timeTemp = false ;
          _menuAtualFervura = false;
          return NULL;
        }
        
        else if(digitalRead(BTN_CONFIRMA)){
          addEtapaFervura(receita[0].fervura,tempratura,(int) tempo);
          posicaoFervura++;
           timeTemp = true ;
           tempo = 0;
           tempratura = 0;
        }
        
        if(digitalRead(BTN_CANCELA)){
          timeTemp = true ;
          
        }
    }
   
    delay(150);
 }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Config Lupulos");
    delay(2000);
    
 while(_menuAtualFervura && posicaoFervura < 12){
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(_menuFervura[posicaoFervura-1]);
    lcd.setCursor(0, 1);
    lcd.write(byte(2));
    lcd.setCursor(1, 1);
 

       lcd.blink();
       lcd.setCursor(3, 1);

       if(tempoLupulo==255){
        lcd.print(" ");
       }
       else{
        lcd.print(tempoLupulo); 
       }
       

        if(digitalRead(BTN_SOBE)){
           tempoLupulo++;
             if(tempoLupulo > 255){
              tempoLupulo = 0;
             }
      
        }
        else if(digitalRead(BTN_DESCE)){
           tempoLupulo--;
             if(tempoLupulo < 0){
              tempoLupulo = 255;
             }
        }
        
        else if(digitalRead(BTN_CONFIRMA) && digitalRead(BTN_CANCELA)){
          timeTemp = false ;
          _menuAtualFervura = false;
          return NULL;
        }
        
        else if(digitalRead(BTN_CONFIRMA)){
          addLupulo(receita[0].lupulo,posicaoFervura-1,(int) tempoLupulo);
          posicaoFervura++;
           timeTemp = true ;
           tempoLupulo = 255;
           tempratura = 0;
           delay(1000);
        }
        
        if(digitalRead(BTN_CANCELA)){
         //TODO
        }
        
    delay(150);
 }
 lcd.clear();
 lcd.setCursor(0, 0);
 lcd.print("Ferv Configurado");
 delay(2000);

}

void imprimirEtapa(){
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Etapa ");
      lcd.setCursor(6,0);
      lcd.print(_step);
}

void imprimiTempoRestante(){
    lcd.setCursor(0,1);
    lcd.print("Tempo:");
    lcd.print((((_millisHorarioAlarm - millis())/1000/60))+1); // converter millis para minutos
    lcd.write(byte(2));
}


void mostrarTemperatura(){
  lcd.setCursor(10,1);
  lcd.write(byte(4));
  lcd.print(getTemperature(),1);
  lcd.write(byte(5));
}

void mostrarTempAlvo(float temp){
  Serial.print("Alvo->");
  Serial.println(temp);
  lcd.setCursor(10,0);
  lcd.write(byte(8));
  lcd.print(temp,1);
  lcd.write(byte(5));
}

void modoManual(){
  bool varModoManual = true;
  float temperatura = 0;
  EtapaQuente etapa[1];
  
  while(varModoManual){
      delay(150);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Escolha Temperatura");
      lcd.setCursor(0, 1);
      lcd.print(temperatura);
      
     if(digitalRead(BTN_SOBE)){
        temperatura= temperatura+0.5;
        if(temperatura > 120){
          temperatura = 0;
         }
      }
      
      else if(digitalRead(BTN_DESCE)){
         temperatura= temperatura-0.5;
           if(temperatura < 0){
            temperatura = 120;
           }
      }

      else if(digitalRead(BTN_CANCELA)){
          varModoManual = false ;
      }

      else if(digitalRead(BTN_CONFIRMA)){
          while(!digitalRead(BTN_CANCELA)){
            delay(150);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Modo");
            lcd.setCursor(0,1);
            lcd.print("Manual");
            mostrarTempAlvo(temperatura);

            ligaBombaRecirculacao();
            mostrarTemperatura();
            controlResistenceTempInicial(getTemperature(),(temperatura - _variacaoMinima), temperatura);  
          }
       }
  } 
}

void mexerMenu(){

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(_menuItems[_menuAtual]);
  lcd.setCursor(0, 1);
  lcd.write("<");
  lcd.setCursor(15, 1);
  lcd.write(">");
  
 if(digitalRead(BTN_SOBE)){
     _menuAtual++;
       if(_menuAtual >5){
        _menuAtual = 0;
       }

  }
  else if(digitalRead(BTN_DESCE)){
     _menuAtual--;
       if(_menuAtual < 0){
        _menuAtual = 5;
       }
  }

  if(digitalRead(BTN_CONFIRMA)){
    switch(_menuAtual){
       case 0:
       escolheReceita();
       break;
       
       case 1:
       _menuAtualMostura = true;
        menuMostura();
       _menuAtualMostura = false;
       _menuAtualFervura = true;
        menuFervura();
       _menuAtualFervura = false;
       _menuAtualNome = true;
        addNomeReceita();
       _menuAtualNome = false;
       break;

       case 2:
       deletarReceita();
       break;

       case 3:
        menuConfigVarMin();
       break;

       case 4:
        menuConfigTempFerv();
       break;

       case 5:
        modoManual();
       break;
    }
  }
   delay(150);

}
