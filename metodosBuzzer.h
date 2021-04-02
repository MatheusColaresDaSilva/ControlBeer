int minParaSeg(int valor){
  return valor * 60;
}

void tocarBuzzer(){
    
    unsigned long _millisAtual = millis();
  
     if (_millisAtual - _millisAnterior > _intervaloBuzzer){
        if (_tocarBuzzer == 1){
            digitalWrite(BUZZER, HIGH);
            _tocarBuzzer = 0;
         }
        else{
            digitalWrite(BUZZER, LOW);
            _tocarBuzzer = 1;
         }
        _millisAnterior = _millisAtual;
      } 
}

void desligaBuzzer(){
  digitalWrite(BUZZER, LOW);
}

void setBuzzerTimer(int valor){

 valor = minParaSeg(valor);
 _millisHorarioAlarm = millis() + ((unsigned long)valor * 1000);
 _millisHorarioInicioFervura = _millisHorarioAlarm - ((unsigned long)valor * 1000);
 _alarmeAtivo = true;
}

void setBuzzerTimerHop(int tempoFervura, int tempoLupulo){

 int valor = tempoFervura - tempoLupulo;

 if(valor == 0){
  adicionarLupuloFervura(_lupuloVez);
  return;
 }
 if(tempoLupulo==0){
  _lupuloFlameOut = true;
  return;
 }

 valor = minParaSeg(valor);
 _millisHorarioAlarmLupulo = _millisHorarioInicioFervura + ((unsigned long)valor * 1000);

 _alarmeAtivoHop = true;

  
}

boolean verificaAlarm(){

    signed long Milis =  millis();
    signed long diferencaMilis = _millisHorarioAlarm - Milis;
    signed long diferencaMilisLupulo = _millisHorarioAlarmLupulo - Milis;    
    
    bool flag = diferencaMilis <= 0;
    bool flag_lupulo = diferencaMilisLupulo <= 0;

    if (flag && _mostura)
    {
      digitalWrite(BUZZER, HIGH);
      delay(1000);
      digitalWrite(BUZZER, LOW);
      return true;
    }
    
    if (flag && _fervura)
    {
      return true;
    }
    
    if (flag_lupulo && _fervura && _alarmeAtivoHop)
    {
        adicionarLupuloFervura(_lupuloVez);
        _alarmeAtivoHop = false;
    }

   return false;
}
