
void button_reset(){
  if(digitalRead(Botonreset)  == LOW){
    pausecontador= 1;
    delay(500);
  }
  while(pausecontador == 1){
    if(digitalRead(Botonreset)  == LOW){
      estado_reset = 0;
      estado_inicio = 1;
      pausecontador= 0;
      delay(500);
    }
    Serial.println("esperando_reset");
    if(digitalRead(BotonCiclos) == LOW){
      Serial.println("stop");
      estado_reset = 1;
      estado_inicio = 0;
      pausecontador = 0;
    }
  }
  EEPROM.update(7, estado_reset);
  EEPROM.update(8, estado_inicio);
  if(estado_reset == 1){
    digitalWrite(dirPin, HIGH);
    dir_estado = 0;
  }
  
}

void button_start(){
  if(digitalRead(Botonstart) == LOW) {
    estado_inicio = 1;
    estado_reset = 0;
    EEPROM.update(0, estado_reset);
    EEPROM.update(1, estado_reset);  
    EEPROM.update(2, estado_reset);
    EEPROM.update(3, estado_reset);
    EEPROM.update(7, estado_reset);
    EEPROM.update(8, estado_inicio);
    EEPROM.update(9,  estado_reset);
    EEPROM.update(10, estado_reset);
    EEPROM.update(11, estado_reset);
    EEPROM.update(12, estado_reset);
    accion[0].byte1 = EEPROM.read(0);
    accion[0].byte2 = EEPROM.read(1);
    accion[1].byte1 = EEPROM.read(2);
    accion[1].byte2 = EEPROM.read(3);
    accion[0].count = accion[0].byte1<<8|accion[0].byte2; //Subidas
    accion[1].count = accion[1].byte1<<8|accion[1].byte2; //Bajadas
    releValor[0].byte1 = EEPROM.read(9);
    releValor[0].byte2 = EEPROM.read(10);
    releValor[1].byte1 = EEPROM.read(11);
    releValor[1].byte2 = EEPROM.read(12);
    releValor[0].count = releValor[0].byte1<<8|releValor[0].byte2; //Subidas accionamientos relevador
    releValor[1].count = releValor[1].byte1<<8|releValor[1].byte2; //Bajadas accionamientos relevador
    countinicio = 0;
    EEPROM.update(13, countinicio);
    
    Serial.print("INICIO");
    lcd.setCursor(0, 0);
    lcd.print("Sub: ");
    lcd.print("    ");
    lcd.setCursor(10, 0);
    lcd.print("Baj: ");
    lcd.print("    ");
    lcd.setCursor(0, 1);
    lcd.print("RSub:");
    lcd.print("    ");
    lcd.setCursor(10, 1);
    lcd.print("RBaj:");
    lcd.print("    "); 
    lcd.setCursor(10, 3);
    lcd.print("Bloq:");
    lcd.print("    ");
    delay(10);
    lcd.setCursor(10, 3);
    lcd.print("Bloq:0");
  }
}

void button_ciclos(){
  if(digitalRead(BotonCiclos) == LOW) {
    limiteContador = limiteContador + 1;
    if(limiteContador > 15){
      limiteContador = 1;
    lcd.setCursor(0, 2);
    lcd.print("Ciclos:");
    lcd.print("    ");
    lcd.setCursor(0, 2);
    lcd.print("Ciclos:");
    lcd.print(Ciclos[limiteContador]*100);
    }
    EEPROM.update(5, limiteContador); 
    Serial.println(limiteContador);
    lcd.setCursor(0, 2);
    lcd.print("Ciclos:");
    lcd.print(Ciclos[limiteContador]*100);
    delay(200);
  }
}

void button_timedelay(){  
  if(digitalRead(BotonVel) == LOW) {
    Serial.println("delay");
    timedelay = EEPROM.read(6);
    timedelay = timedelay+2;
    if(timedelay > 11){
      timedelay = 1;
    lcd.setCursor(0, 3);
    lcd.print("Vel:");
    lcd.print("  ");
    lcd.setCursor(0, 3);
    lcd.print("Vel:");
    lcd.print(timedelay);
    
    }
    EEPROM.update(6, timedelay); 
    Serial.println(timedelay);
    lcd.setCursor(0, 3);
    lcd.print("Vel:");
    lcd.print(timedelay);
    delay(200);
  }
}

void sensor_bloqueo(){
  int countBloqueo  =0;
  int resultResiduo =1;

  
  if(accion[1].count >= CiclosBloqueo){
    resultResiduo = accion[1].count % CiclosBloqueo;
  }

  if( resultResiduo == 0 && (digitalRead(fin_carrera3)==LOW) && varauxiliarbloqueo == 0){
    while(countBloqueo < SegundosBloqueado){
      countBloqueo++;
      Serial.println("while");
      delay(1000);
    }
    BloqueosTotales = accion[1].count/CiclosBloqueo;
    lcd.setCursor(10, 3);
    lcd.print("Bloq:");
    lcd.print("  ");
    
    lcd.setCursor(10, 3);
    lcd.print("Bloq:");
    lcd.print(BloqueosTotales);
    Serial.println("if");
    varauxiliarbloqueo = 1;
  }
}

void Conteo_relevadores(){
  if((digitalRead(entrada_relevador_delantera)==LOW)&& !!EnableSumaRele[0]){
    Serial.println("entra1");
    releValor[0].count++;
    releValor[0].byte1 = releValor[0].count >> 8;
    releValor[0].byte2 = releValor[0].count & 255;
    EEPROM.update(9,  releValor[0].byte1);
    EEPROM.update(10, releValor[0].byte2);
    EnableSumaRele[0] =1;
    previousEnableSumaRele = millis();
    lcd.setCursor(0, 1);
    lcd.print("RSub:");
    lcd.print("    ");
    lcd.setCursor(0, 1);
    lcd.print("RSub:");
    lcd.print(releValor[0].count);
    delay(500);
  }
  if((digitalRead(entrada_relevador_trasera)==LOW)&& !!EnableSumaRele[1]){
    Serial.println("entra2");
    releValor[1].count++;
    releValor[1].byte1 = releValor[1].count >> 8;
    releValor[1].byte2 = releValor[1].count & 255;
    EEPROM.update(11, releValor[1].byte1);
    EEPROM.update(12, releValor[1].byte2);
    EnableSumaRele[1] =1;
    previousEnableSumaRele = millis();
    lcd.setCursor(10, 1);
    lcd.print("RBaj:");
    lcd.print("    ");
    lcd.setCursor(10, 1);
    lcd.print("RBaj:");
    lcd.print(releValor[1].count);
    delay(500);
  }
  
  if(digitalRead(entrada_relevador_delantera)==HIGH){
    EnableSumaRele[0]= 0;
  }
  if(digitalRead(entrada_relevador_trasera)==HIGH){
    EnableSumaRele[1]= 0;
  }


}
